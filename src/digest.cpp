// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// digest.cpp: create hash function digests and summary
//
// Copyright (C) 2015    Xiuwen Zheng
//
// gdsfmt is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License Version 3 as
// published by the Free Software Foundation.
//
// gdsfmt is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with gdsfmt.
// If not, see <http://www.gnu.org/licenses/>.

#define COREARRAY_GDSFMT_PACKAGE

#include "R_GDS_CPP.h"
#include <Rdefines.h>
#include <R_ext/Rdynload.h>


using namespace std;
using namespace CoreArray;

extern "C"
{
static SEXP ToHex(C_UInt8 Code[], size_t Len)
{
	char buffer[1024 + 1];
	char *p = buffer;
	for (; Len > 0; Code++, Len--)
	{
		C_UInt8 v1 = (*Code) & 0x0F;
		C_UInt8 v2 = (*Code) >> 4;
		*p++ = (v2 < 10) ? (v2 + '0') : (v2 - 10 + 'a');
		*p++ = (v1 < 10) ? (v1 + '0') : (v1 - 10 + 'a');
	}
	*p = 0;
	return mkString(buffer);
}


/// create hash function digests
/** \param Node        [in] the GDS node
 *  \param Algorithm   [in] e.g., md5
**/
COREARRAY_DLL_EXPORT SEXP gdsDigest(SEXP Node, SEXP Algorithm)
{
	static const char *digest_package = "digest";

	#define LOAD(name)	\
		*(DL_FUNC*)(&name) = R_FindSymbol(#name, digest_package, NULL); \
		if (!name) return rv_ans;

	#define COMPUTE(fun)  \
		for (int i=0; i < (int)Data.size(); i++) \
		{ \
			CdStream *stream = Data[i]; \
			SIZE64 Size = stream->GetSize(); \
			stream->SetPosition(0); \
			for (SIZE64 p=0; p < Size; ) \
			{ \
				SIZE64 L = Size - p; \
				if (L > sizeof(Buffer)) L = sizeof(Buffer); \
				p += L; \
				stream->Read(Buffer, L); \
				(*fun)(&ctx, Buffer, L); \
			} \
		}


	const char *algo = CHAR(STRING_ELT(Algorithm, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		if (dynamic_cast<CdContainer*>(Obj))
			static_cast<CdContainer*>(Obj)->CloseWriter();

		vector<CdStream*> Data;
		Obj->GetOwnBlockStream(Data);
		if (Data.empty())
			throw ErrGDSFile("There is no data field.");

		rv_ans = NA_STRING;
		C_UInt8 Buffer[65536];

		if (strcmp(algo, "md5") == 0)
		{
			typedef struct _md5_context
			{
				C_UInt32 total[2];
				C_UInt32 state[4];
				C_UInt8  buffer[64];
			} md5_context;

			void (*md5_starts)(md5_context *);
			void (*md5_update)(md5_context *, C_UInt8 *, C_UInt32);
			void (*md5_finish)(md5_context *, C_UInt8[16]);
			LOAD(md5_starts);
			LOAD(md5_update);
			LOAD(md5_finish);

			md5_context ctx;
			(*md5_starts)(&ctx);
			COMPUTE(md5_update);

			C_UInt8 digest[16];
			(*md5_finish)(&ctx, digest);
			rv_ans = ToHex(digest, 16);

        } else if (strcmp(algo, "sha1") == 0)
        {
			typedef struct _sha1_context
			{
				C_UInt32 total[2];
				C_UInt32 state[5];
				C_UInt8 buffer[64];
			} sha1_context;

			void (*sha1_starts)(sha1_context *);
			void (*sha1_update)(sha1_context *, C_UInt8 *, C_UInt32);
			void (*sha1_finish)(sha1_context *, C_UInt8[20]);
			LOAD(sha1_starts);
			LOAD(sha1_update);
			LOAD(sha1_finish);

			sha1_context ctx;
			(*sha1_starts)(&ctx);
			COMPUTE(sha1_update);

			C_UInt8 digest[20];
			(*sha1_finish)(&ctx, digest);
			rv_ans = ToHex(digest, 20);

        } else if (strcmp(algo, "sha256") == 0)
        {
			typedef struct _sha256_context
			{
				C_UInt32 total[2];
				C_UInt32 state[8];
				C_UInt8 buffer[64];
			} sha256_context;

			void (*sha256_starts)(sha256_context *);
			void (*sha256_update)(sha256_context *, C_UInt8 *, C_UInt32);
			void (*sha256_finish)(sha256_context *, C_UInt8[32]);
			LOAD(sha256_starts);
			LOAD(sha256_update);
			LOAD(sha256_finish);

			sha256_context ctx;
			(*sha256_starts)(&ctx);
			COMPUTE(sha256_update);

			C_UInt8 digest[32];
			(*sha256_finish)(&ctx, digest);
			rv_ans = ToHex(digest, 32);

        } else if (strcmp(algo, "sha384") == 0)
        {
			typedef struct _SHA384_CTX {
				C_UInt64 state[8];
				C_UInt64 bitcount[2];
				C_UInt8	buffer[128];
			} SHA384_CTX;

			void (*SHA384_Init)(SHA384_CTX *);
			void (*SHA384_Update)(SHA384_CTX *, const C_UInt8 *, size_t);
			void (*SHA384_Final)(C_UInt8[48], SHA384_CTX *);
			LOAD(SHA384_Init);
			LOAD(SHA384_Update);
			LOAD(SHA384_Final);

			SHA384_CTX ctx;
			(*SHA384_Init)(&ctx);
			COMPUTE(SHA384_Update);

			C_UInt8 digest[48];
			(*SHA384_Final)(digest, &ctx);
			rv_ans = ToHex(digest, 48);

		} else if (strcmp(algo, "sha512") == 0)
        {
			typedef struct _SHA512_CTX {
				C_UInt64 state[8];
				C_UInt64 bitcount[2];
				C_UInt8	buffer[128];
			} SHA512_CTX;

			void (*SHA512_Init)(SHA512_CTX *);
			void (*SHA512_Update)(SHA512_CTX *, const C_UInt8 *, size_t);
			void (*SHA512_Final)(C_UInt8[64], SHA512_CTX *);
			LOAD(SHA512_Init);
			LOAD(SHA512_Update);
			LOAD(SHA512_Final);

			SHA512_CTX ctx;
			(*SHA512_Init)(&ctx);
			COMPUTE(SHA512_Update);

			C_UInt8 digest[64];
			(*SHA512_Final)(digest, &ctx);
			rv_ans = ToHex(digest, 64);
        }

	COREARRAY_CATCH
}



static SEXP _GetRes(double xmin, double xmax, C_Int64 nNaN, C_Int64 decimal[],
	int dec_size)
{
	if (!IsFinite(xmin)) xmin = R_NaN;
	if (!IsFinite(xmax)) xmax = R_NaN;

	SEXP rv_ans = PROTECT(NEW_LIST(4));
	SET_ELEMENT(rv_ans, 0, ScalarReal(xmin));
	SET_ELEMENT(rv_ans, 1, ScalarReal(xmax));
	SET_ELEMENT(rv_ans, 2, ScalarReal(nNaN));
	SEXP dec = PROTECT(NEW_NUMERIC(dec_size));
	SEXP nm = PROTECT(NEW_STRING(dec_size));
	for (int i=0; i < dec_size; i++)
	{
		REAL(dec)[i] = decimal[i];
		if (i == 0)
			SET_STRING_ELT(nm, i, mkChar("int"));
		else if (i < (dec_size-1))
		{
			string s = ".";
			for (int j=1; j < i; j++) s.append("0");
			s.append("1");
			SET_STRING_ELT(nm, i, mkChar(s.c_str()));
		} else
			SET_STRING_ELT(nm, i, mkChar("other"));
	}

	SET_NAMES(dec, nm);
	SET_ELEMENT(rv_ans, 3, dec);
	nm = PROTECT(NEW_STRING(4));
	SET_STRING_ELT(nm, 0, mkChar("min"));
	SET_STRING_ELT(nm, 1, mkChar("max"));
	SET_STRING_ELT(nm, 2, mkChar("num_nan"));
	SET_STRING_ELT(nm, 3, mkChar("decimal"));
	SET_NAMES(rv_ans, nm);
	UNPROTECT(4);

	return rv_ans;
}

/// summary GDS data
/** \param Node        [in] the GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsSummary(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		if (dynamic_cast<CdContainer*>(Obj))
		{
			CdContainer *Var = static_cast<CdContainer*>(Obj);
			CdIterator it = Var->IterBegin();
			C_Int64 Cnt = Var->TotalCount();

			switch (Var->SVType())
			{
			case svInt8:
			case svUInt8:
			case svInt16:
			case svUInt16:
			case svInt32:
			case svUInt32:
			case svInt64:
			case svUInt64:
				throw ErrGDSFmt("No support.");

			case svFloat32:
				{
					const ssize_t SIZE = 65536 / sizeof(float);
					float Buffer[SIZE];
					char numbuf[64];
					float xmin=Infinity, xmax=NegInfinity;
					C_Int64 nNaN = 0;
					C_Int64 decimal[8] = { 0,0,0,0,0,0,0,0 }; // int, dec.1, dec.01, ..., other
					while (Cnt > 0)
					{
						ssize_t L = (Cnt <= SIZE) ? Cnt : SIZE; 
						Cnt -= L;
						it.ReadData(Buffer, L, svFloat32);
						float *p = Buffer;
						for (; L > 0; L--)
						{
							const float v = *p++;
							if (IsFinite(v))
							{
								if (v < xmin) xmin = v;
								if (v > xmax) xmax = v;
								FmtText(numbuf, sizeof(numbuf), "%.6g", v);
								const char *s = numbuf;
								int ndec = 0;
								for (; *s; s++) if (*s == '.') break;
								if (*s == '.') for (; *(++s); ) ndec++;
								if (ndec > 7) ndec = 7;
								decimal[ndec] ++;
							} else {
								nNaN ++; decimal[7] ++;
							}
						}
					}
					rv_ans = _GetRes(xmin, xmax, nNaN, decimal, 8);
					break;
				}

			case svFloat64:
				{
					const ssize_t SIZE = 65536 / sizeof(double);
					double Buffer[SIZE];
					char numbuf[64];
					double xmin=Infinity, xmax=NegInfinity;
					C_Int64 nNaN = 0;
					C_Int64 decimal[16]; // int, dec.1, ..., other
					memset(decimal, 0, sizeof(decimal));
					while (Cnt > 0)
					{
						ssize_t L = (Cnt <= SIZE) ? Cnt : SIZE; 
						Cnt -= L;
						it.ReadData(Buffer, L, svFloat64);
						double *p = Buffer;
						for (; L > 0; L--)
						{
							const double v = *p++;
							if (IsFinite(v))
							{
								if (v < xmin) xmin = v;
								if (v > xmax) xmax = v;
								FmtText(numbuf, sizeof(numbuf), "%.14g", v);
								const char *s = numbuf;
								int ndec = 0;
								for (; *s; s++) if (*s == '.') break;
								if (*s == '.') for (; *(++s); ) ndec++;
								if (ndec > 15) ndec = 15;
								decimal[ndec] ++;
							} else {
								nNaN ++; decimal[15] ++;
							}
						}
					}
					rv_ans = _GetRes(xmin, xmax, nNaN, decimal, 16);
					break;
				}

			// svStrUTF8     = 15,   ///< UTF-8 string
			// svStrUTF16    = 16    ///< UTF-16 string
			default:
				throw ErrGDSFmt("No support.");
			}
		} else
			throw ErrGDSFmt("There is no data field.");

	COREARRAY_CATCH
}

} // extern "C"
