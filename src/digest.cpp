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
// Copyright (C) 2015-2024    Xiuwen Zheng
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
// ----------------------------------------------------------------------------
// Hash function digests
// ----------------------------------------------------------------------------

static const char *PKG_DIGEST = "digest";

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
	return Rf_mkString(buffer);
}

#define LOAD_DIGEST(name)    { \
	DL_FUNC f = R_FindSymbol(#name, PKG_DIGEST, NULL); \
	if (!f) return NA_STRING; \
	memcpy(&name, &f, sizeof(f)); \
}

#define INIT_HASH_PROC    \
	vector<CdStream*> Data;  \
	Obj->GetOwnBlockStream(Data);  \
	if (Data.empty()) throw ErrGDSFile("There is no data field.");  \
	vector<string> FactorText;  \
	int Num_FactorText = 0;  \
	const char BlankChar = 0;  \
	if (is_factor)  \
	{  \
		int nProtected = 1;  \
		SEXP Val = PROTECT(Rf_ScalarInteger(1));  \
		nProtected += GDS_R_Set_IfFactor(Obj, Val);  \
		SEXP level = GET_LEVELS(Val);  \
		Num_FactorText = Rf_length(level);  \
		for (int i=0; i < Num_FactorText; i++)  \
			FactorText.push_back(CHAR(STRING_ELT(level, i)));  \
		UNPROTECT(nProtected);  \
	}  \
	C_UInt8 Buffer[65536];

#define COMPUTE_DIGEST(fun)    { \
	if (use_R_obj) \
	{ \
		CdContainer *Var = static_cast<CdContainer*>(Obj); \
		CdIterator it = Var->IterBegin(); \
		C_Int64 Cnt = Var->TotalCount(); \
		if (is_factor) \
		{ \
			const ssize_t BufNum = 65536 / sizeof(int); \
			while (Cnt > 0) \
			{ \
				ssize_t L = (Cnt <= BufNum) ? Cnt : BufNum; \
				Cnt -= L; \
				it.ReadData((void*)Buffer, L, svInt32); \
				for (int *p = (int*)Buffer; L > 0; L--) \
				{ \
					int v = *p++; \
					if ((0 < v) && (v <= Num_FactorText)) \
					{ \
						string &s = FactorText[v - 1]; \
						(*fun)(&ctx, (C_UInt8*)s.c_str(), s.size()+1); \
					} else { \
						(*fun)(&ctx, (C_UInt8*)&BlankChar, 1); \
					} \
				} \
			} \
		} else if ((SV==svInt8) || (SV==svInt32) || (SV==svFloat64)) \
		{ \
			ssize_t SIZE = (SV==svInt8 ? sizeof(C_Int8) : \
				(SV==svInt32 ? sizeof(int) : sizeof(double))); \
			const ssize_t BufNum = 65536 / SIZE; \
			while (Cnt > 0) \
			{ \
				ssize_t L = (Cnt <= BufNum) ? Cnt : BufNum; \
				Cnt -= L; \
				it.ReadData((void*)Buffer, L, SV); \
				(*fun)(&ctx, Buffer, L*SIZE); \
			} \
		} else { \
			UTF8String Buffer[65536]; \
			while (Cnt > 0) \
			{ \
				ssize_t L = (Cnt <= 65536) ? Cnt : 65536; \
				Cnt -= L; \
				it.ReadData((void*)Buffer, L, svStrUTF8); \
				for (UTF8String *p=Buffer; L > 0; L--, p++) \
					(*fun)(&ctx, (C_UInt8*)p->c_str(), p->size()+1); \
			} \
		} \
	} else { \
		for (int i=0; i < (int)Data.size(); i++) \
		{ \
			CdStream *stream = Data[i]; \
			SIZE64 Size = stream->GetSize(); \
			stream->SetPosition(0); \
			for (SIZE64 p=0; p < Size; ) \
			{ \
				SIZE64 L = Size - p; \
				if (L > (int)sizeof(Buffer)) L = sizeof(Buffer); \
				p += L; \
				stream->Read(Buffer, L); \
				(*fun)(&ctx, Buffer, L); \
			} \
		} \
	} \
}


/// MD5 hash code
static SEXP digest_md5(PdGDSObj Obj, C_SVType SV, bool is_factor, bool use_R_obj)
{
	INIT_HASH_PROC
	typedef struct _md5_context
	{
		C_UInt32 total[2];
		C_UInt32 state[4];
		C_UInt8  buffer[64];
		char tmp[1024];
	} md5_context;

	void (*md5_starts)(md5_context *);
	void (*md5_update)(md5_context *, C_UInt8 *, C_UInt32);
	void (*md5_finish)(md5_context *, C_UInt8[16]);
	LOAD_DIGEST(md5_starts);
	LOAD_DIGEST(md5_update);
	LOAD_DIGEST(md5_finish);

	md5_context ctx;
	(*md5_starts)(&ctx);
	COMPUTE_DIGEST(md5_update);

	C_UInt8 digest[16];
	(*md5_finish)(&ctx, digest);
	return ToHex(digest, 16);
}

/// SHA1 hash code
static SEXP digest_sha1(PdGDSObj Obj, C_SVType SV, bool is_factor, bool use_R_obj)
{
	INIT_HASH_PROC
	typedef struct _sha1_context
	{
		C_UInt32 total[2];
		C_UInt32 state[5];
		C_UInt8 buffer[64];
		char tmp[1024];
	} sha1_context;

	void (*sha1_starts)(sha1_context *);
	void (*sha1_update)(sha1_context *, C_UInt8 *, C_UInt32);
	void (*sha1_finish)(sha1_context *, C_UInt8[20]);
	LOAD_DIGEST(sha1_starts);
	LOAD_DIGEST(sha1_update);
	LOAD_DIGEST(sha1_finish);

	sha1_context ctx;
	(*sha1_starts)(&ctx);
	COMPUTE_DIGEST(sha1_update);

	C_UInt8 digest[20];
	(*sha1_finish)(&ctx, digest);
	return ToHex(digest, 20);
}

/// SHA256 hash code
static SEXP digest_sha256(PdGDSObj Obj, C_SVType SV, bool is_factor, bool use_R_obj)
{
	INIT_HASH_PROC
	typedef struct _sha256_context
	{
		C_UInt32 total[2];
		C_UInt32 state[8];
		C_UInt8 buffer[64];
		char tmp[1024];
	} sha256_context;

	void (*sha256_starts)(sha256_context *);
	void (*sha256_update)(sha256_context *, C_UInt8 *, C_UInt32);
	void (*sha256_finish)(sha256_context *, C_UInt8[32]);
	LOAD_DIGEST(sha256_starts)
	LOAD_DIGEST(sha256_update)
	LOAD_DIGEST(sha256_finish);

	sha256_context ctx;
	(*sha256_starts)(&ctx);
	COMPUTE_DIGEST(sha256_update);

	C_UInt8 digest[32];
	(*sha256_finish)(&ctx, digest);
	return ToHex(digest, 32);
}

/// SHA384 hash code
static SEXP digest_sha384(PdGDSObj Obj, C_SVType SV, bool is_factor, bool use_R_obj)
{
	INIT_HASH_PROC
	typedef struct _SHA384_CTX {
		C_UInt64 state[8];
		C_UInt64 bitcount[2];
		C_UInt8	buffer[128];
		char tmp[1024];
	} SHA384_CTX;

	void (*SHA384_Init)(SHA384_CTX *);
	void (*SHA384_Update)(SHA384_CTX *, const C_UInt8 *, size_t);
	void (*SHA384_Final)(C_UInt8[48], SHA384_CTX *);
	LOAD_DIGEST(SHA384_Init);
	LOAD_DIGEST(SHA384_Update);
	LOAD_DIGEST(SHA384_Final);

	SHA384_CTX ctx;
	(*SHA384_Init)(&ctx);
	COMPUTE_DIGEST(SHA384_Update);

	C_UInt8 digest[48];
	(*SHA384_Final)(digest, &ctx);
	return ToHex(digest, 48);
}

/// SHA512 hash code
static SEXP digest_sha512(PdGDSObj Obj, C_SVType SV, bool is_factor, bool use_R_obj)
{
	INIT_HASH_PROC
	typedef struct _SHA512_CTX {
		C_UInt64 state[8];
		C_UInt64 bitcount[2];
		C_UInt8	buffer[128];
		char tmp[1024];
	} SHA512_CTX;

	void (*SHA512_Init)(SHA512_CTX *);
	void (*SHA512_Update)(SHA512_CTX *, const C_UInt8 *, size_t);
	void (*SHA512_Final)(C_UInt8[64], SHA512_CTX *);
	LOAD_DIGEST(SHA512_Init);
	LOAD_DIGEST(SHA512_Update);
	LOAD_DIGEST(SHA512_Final);

	SHA512_CTX ctx;
	(*SHA512_Init)(&ctx);
	COMPUTE_DIGEST(SHA512_Update);

	C_UInt8 digest[64];
	(*SHA512_Final)(digest, &ctx);
	return ToHex(digest, 64);
}


/// create hash function digests
/** \param Node        [in] the GDS node
 *  \param Algorithm   [in] e.g., md5
 *  \param UseRObj     [in] convert to R object (integer, double, character) if TRUE
**/
COREARRAY_DLL_EXPORT SEXP gdsDigest(SEXP Node, SEXP Algorithm, SEXP UseRObj)
{
	const char *algo = CHAR(STRING_ELT(Algorithm, 0));
	const bool use_R_obj = (Rf_asLogical(UseRObj) == TRUE);

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		C_SVType SV = svCustom;
		bool is_factor = false;
		if (dynamic_cast<CdContainer*>(Obj))
		{
			static_cast<CdContainer*>(Obj)->CloseWriter();
			SV = static_cast<CdContainer*>(Obj)->SVType();
			if (use_R_obj)
			{
				if (COREARRAY_SV_INTEGER(SV))
				{
					is_factor = GDS_R_Is_Factor(Obj);
					unsigned nbit = static_cast<CdContainer*>(Obj)->BitOf();
					SV = (nbit <= 8) ? svInt8 : svInt32;
				} else if (COREARRAY_SV_FLOAT(SV))
					SV = svFloat64;
				else if (COREARRAY_SV_STRING(SV))
					SV = svStrUTF8;
				else
					throw ErrGDSFile("No valid data field.");
			}
		} else {
			if (use_R_obj)
				throw ErrGDSFile("No valid data field.");
		}

		if (strcmp(algo, "md5") == 0)
		{
			rv_ans = digest_md5(Obj, SV, is_factor, use_R_obj);
        } else if (strcmp(algo, "sha1") == 0)
        {
			rv_ans = digest_sha1(Obj, SV, is_factor, use_R_obj);
        } else if (strcmp(algo, "sha256") == 0)
        {
			rv_ans = digest_sha256(Obj, SV, is_factor, use_R_obj);
        } else if (strcmp(algo, "sha384") == 0)
        {
			rv_ans = digest_sha384(Obj, SV, is_factor, use_R_obj);
		} else if (strcmp(algo, "sha512") == 0)
        {
			rv_ans = digest_sha512(Obj, SV, is_factor, use_R_obj);
        } else
			throw ErrGDSFile("Unknown digest method.");

	COREARRAY_CATCH
}



// ----------------------------------------------------------------------------
// Summary function
// ----------------------------------------------------------------------------

static SEXP _GetRes(double xmin, double xmax, C_Int64 nNA, C_Int64 decimal[],
	int dec_size)
{
	if (!IsFinite(xmin)) xmin = R_NaN;
	if (!IsFinite(xmax)) xmax = R_NaN;

	int nProtected = 0;
	SEXP rv_ans = PROTECT(NEW_LIST(4));
	nProtected ++;
	SET_ELEMENT(rv_ans, 0, Rf_ScalarReal(xmin));
	SET_ELEMENT(rv_ans, 1, Rf_ScalarReal(xmax));
	SET_ELEMENT(rv_ans, 2, Rf_ScalarReal(nNA));
	if (decimal)
	{
		SEXP dec = PROTECT(NEW_NUMERIC(dec_size));
		SEXP nm = PROTECT(NEW_STRING(dec_size));
		nProtected += 2;
		for (int i=0; i < dec_size; i++)
		{
			REAL(dec)[i] = decimal[i];
			if (i == 0)
				SET_STRING_ELT(nm, i, Rf_mkChar("int"));
			else if (i < (dec_size-1))
			{
				string s = ".";
				for (int j=1; j < i; j++) s.append("0");
				s.append("1");
				SET_STRING_ELT(nm, i, Rf_mkChar(s.c_str()));
			} else
				SET_STRING_ELT(nm, i, Rf_mkChar("other"));
		}
		SET_NAMES(dec, nm);
		SET_ELEMENT(rv_ans, 3, dec);
	}

	SEXP nm = PROTECT(NEW_STRING(4));
	nProtected ++;
	SET_STRING_ELT(nm, 0, Rf_mkChar("min"));
	SET_STRING_ELT(nm, 1, Rf_mkChar("max"));
	SET_STRING_ELT(nm, 2, Rf_mkChar("num_na"));
	SET_STRING_ELT(nm, 3, Rf_mkChar("decimal"));
	SET_NAMES(rv_ans, nm);
	UNPROTECT(nProtected);

	return rv_ans;
}

/// summary GDS data
/** \param Node        [in] the GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsSummary(SEXP Node)
{
	#define COMPUTE(stype, dtype) \
		{ \
			const ssize_t SIZE = 65536 / sizeof(stype); \
			stype Buffer[SIZE]; \
			dtype xmin=TdTraits<dtype>::Max(); \
			dtype xmax=TdTraits<dtype>::Min(); \
			while (Cnt > 0) \
			{ \
				ssize_t L = (Cnt <= SIZE) ? Cnt : SIZE; \
				Cnt -= L; \
				it.ReadData(Buffer, L, TdTraits<stype>::SVType); \
				for (stype *p = Buffer; L > 0; L--) \
				{ \
					dtype v = *p++; \
					if (v < xmin) xmin = v; \
					if (v > xmax) xmax = v; \
				} \
			} \
			rv_ans = _GetRes( \
				xmin==TdTraits<dtype>::Max() ? R_NaN : (double)xmin, \
				xmax==TdTraits<dtype>::Min() ? R_NaN : (double)xmax, \
				0, NULL, 0); \
			break; \
		}

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
				COMPUTE(C_Int8, int)
			case svUInt8:
				COMPUTE(C_UInt8, int)
			case svInt16:
				COMPUTE(C_Int16, int)
			case svUInt16:
				COMPUTE(C_UInt16, int)
			case svUInt32:
				COMPUTE(C_UInt32, C_Int64)
			case svInt64:
				COMPUTE(C_Int64, C_Int64)
			case svUInt64:
				COMPUTE(C_UInt64, C_UInt64)
			case svInt32:
				{
					const ssize_t SIZE = 65536 / sizeof(C_Int32);
					C_Int32 Buffer[SIZE];
					C_Int64 xmin=TdTraits<C_Int64>::Max();
					C_Int64 xmax=TdTraits<C_Int64>::Min();
					C_Int64 nNA = 0;
					while (Cnt > 0)
					{
						ssize_t L = (Cnt <= SIZE) ? Cnt : SIZE;
						Cnt -= L;
						it.ReadData(Buffer, L, svInt32);
						for (C_Int32 *p = Buffer; L > 0; L--)
						{
							C_Int32 v = *p++;
							if (v != NA_INTEGER)
							{
								if (v < xmin) xmin = v;
								if (v > xmax) xmax = v;
							} else
								nNA ++;
						}
					}
					rv_ans = _GetRes(
						xmin==TdTraits<C_Int64>::Max() ? R_NaN : (double)xmin,
						xmax==TdTraits<C_Int64>::Min() ? R_NaN : (double)xmax,
						nNA, NULL, 0);
					break;
				}
			case svFloat32:
				{
					const ssize_t SIZE = 65536 / sizeof(float);
					float Buffer[SIZE];
					char numbuf[64];
					float xmin=Infinity, xmax=NegInfinity;
					C_Int64 nNA = 0;
					C_Int64 decimal[8] = { 0,0,0,0,0,0,0,0 }; // int, dec.1, dec.01, ..., other
					while (Cnt > 0)
					{
						ssize_t L = (Cnt <= SIZE) ? Cnt : SIZE; 
						Cnt -= L;
						it.ReadData(Buffer, L, svFloat32);
						for (float *p = Buffer; L > 0; L--)
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
								nNA ++; decimal[7] ++;
							}
						}
					}
					rv_ans = _GetRes(xmin, xmax, nNA, decimal, 8);
					break;
				}

			case svFloat64:
				{
					const ssize_t SIZE = 65536 / sizeof(double);
					double Buffer[SIZE];
					char numbuf[64];
					double xmin=Infinity, xmax=NegInfinity;
					C_Int64 nNA = 0;
					C_Int64 decimal[16]; // int, dec.1, ..., other
					memset(decimal, 0, sizeof(decimal));
					while (Cnt > 0)
					{
						ssize_t L = (Cnt <= SIZE) ? Cnt : SIZE; 
						Cnt -= L;
						it.ReadData(Buffer, L, svFloat64);
						for (double *p = Buffer; L > 0; L--)
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
								nNA ++; decimal[15] ++;
							}
						}
					}
					rv_ans = _GetRes(xmin, xmax, nNA, decimal, 16);
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

	#undef COMPUTE
}

} // extern "C"
