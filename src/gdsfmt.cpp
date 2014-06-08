// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// gdsfmt.cpp: the R interface of CoreArray library
//
// Copyright (C) 2013	Xiuwen Zheng
//
// This file is part of CoreArray.
//
// CoreArray is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License Version 3 as
// published by the Free Software Foundation.
//
// CoreArray is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with CoreArray.
// If not, see <http://www.gnu.org/licenses/>.

// TODO: double check 'ClearDim' in futures

#include <CoreDEF.h>
#include <dType.h>
#include <dSeq.h>

#include <R.h>
#include <Rdefines.h>
#include <string.h>

#include <map>
#include <string>
#include <memory>

// R_XLEN_T_MAX is defined, R >= v3.0
#ifndef R_XLEN_T_MAX
#  define R_xlen_t	R_len_t
#  define XLENGTH	Rf_length
#endif


using namespace std;
using namespace CoreArray;


#define LongBool int

#ifdef COREARRAY_GNUG
#  ifdef COREARRAY_WINDOWS
#    define DLLEXPORT __attribute__((dllexport))
#  else
#    define DLLEXPORT
#  endif
#else
#  define DLLEXPORT __declspec(dllexport)
#endif


#define CORETRY           try {
#define CORECATCH(cmd)    } \
	catch (exception &E) { \
		Init.LastError = E.what(); \
		cmd; \
	} \
	catch (const char *E) { \
		Init.LastError = E; \
		cmd; \
	}
#define CORECATCH_ERROR    } \
	catch (exception &E) { \
		Init.LastError = E.what(); \
		error(Init.LastError.c_str()); \
	} \
	catch (const char *E) { \
		Init.LastError = E; \
		error(Init.LastError.c_str()); \
	}	\
	return R_NilValue;



/// the object for initialization
class TInit
{
public:
	/// the maximum number of supported GDS files
	const static int MaxFiles = 256;
	/// the buffer of GDS files
	CdGDSFile *Files[MaxFiles];
	/// the last error message
	string LastError;

	struct strCmp {
		bool operator()(const char* s1, const char* s2) const
			{ return strcmp(s1, s2) < 0; }
	};

	map<const char*, const char*, strCmp> ClassMap;

	/// constructor
	TInit()
	{
		// initialize the local variables
		_InitClassFlag = false;
		for (int i=0; i < MaxFiles; i++) Files[i] = NULL;

		ClassMap["folder"] = "$FOLDER$";

		// **************************************************************
		// integer

		ClassMap["int8"] = TdTraits<CoreArray::Int8>::StreamName();
		ClassMap["uint8"] = TdTraits<CoreArray::UInt8>::StreamName();
		ClassMap["int16"] = TdTraits<CoreArray::Int16>::StreamName();
		ClassMap["uint16"] = TdTraits<CoreArray::UInt16>::StreamName();
		ClassMap["int24"] = TdTraits<CoreArray::Int24>::StreamName();
		ClassMap["uint24"] = TdTraits<CoreArray::UInt24>::StreamName();
		ClassMap["int32"] = TdTraits<CoreArray::Int32>::StreamName();
		ClassMap["uint32"] = TdTraits<CoreArray::UInt32>::StreamName();
		ClassMap["int64"] = TdTraits<CoreArray::Int64>::StreamName();
		ClassMap["uint64"] = TdTraits<CoreArray::UInt64>::StreamName();

		ClassMap["bit1"] = TdTraits< CoreArray::BITS<1> >::StreamName();
		ClassMap["bit2"] = TdTraits< CoreArray::BITS<2> >::StreamName();
		ClassMap["bit3"] = TdTraits< CoreArray::BITS<3> >::StreamName();
		ClassMap["bit4"] = TdTraits< CoreArray::BITS<4> >::StreamName();
		ClassMap["bit5"] = TdTraits< CoreArray::BITS<5> >::StreamName();
		ClassMap["bit6"] = TdTraits< CoreArray::BITS<6> >::StreamName();
		ClassMap["bit7"] = TdTraits< CoreArray::BITS<7> >::StreamName();
		ClassMap["bit8"] = TdTraits< CoreArray::UInt8 >::StreamName();

		ClassMap["bit9"] = TdTraits< CoreArray::BITS<9> >::StreamName();
		ClassMap["bit10"] = TdTraits< CoreArray::BITS<10> >::StreamName();
		ClassMap["bit11"] = TdTraits< CoreArray::BITS<11> >::StreamName();
		ClassMap["bit12"] = TdTraits< CoreArray::BITS<12> >::StreamName();
		ClassMap["bit13"] = TdTraits< CoreArray::BITS<13> >::StreamName();
		ClassMap["bit14"] = TdTraits< CoreArray::BITS<14> >::StreamName();
		ClassMap["bit15"] = TdTraits< CoreArray::BITS<15> >::StreamName();
		ClassMap["bit16"] = TdTraits< CoreArray::UInt16 >::StreamName();

		ClassMap["bit24"] = TdTraits< CoreArray::UInt24 >::StreamName();
		ClassMap["bit32"] = TdTraits< CoreArray::UInt32 >::StreamName();
		ClassMap["bit64"] = TdTraits< CoreArray::UInt64 >::StreamName();

		ClassMap["sbit2"] = TdTraits< CoreArray::BITS<-2> >::StreamName();
		ClassMap["sbit3"] = TdTraits< CoreArray::BITS<-3> >::StreamName();
		ClassMap["sbit4"] = TdTraits< CoreArray::BITS<-4> >::StreamName();
		ClassMap["sbit5"] = TdTraits< CoreArray::BITS<-5> >::StreamName();
		ClassMap["sbit6"] = TdTraits< CoreArray::BITS<-6> >::StreamName();
		ClassMap["sbit7"] = TdTraits< CoreArray::BITS<-7> >::StreamName();
		ClassMap["sbit8"] = TdTraits< CoreArray::Int8 >::StreamName();

		ClassMap["sbit9"] = TdTraits< CoreArray::BITS<-9> >::StreamName();
		ClassMap["sbit10"] = TdTraits< CoreArray::BITS<-10> >::StreamName();
		ClassMap["sbit11"] = TdTraits< CoreArray::BITS<-11> >::StreamName();
		ClassMap["sbit12"] = TdTraits< CoreArray::BITS<-12> >::StreamName();
		ClassMap["sbit13"] = TdTraits< CoreArray::BITS<-13> >::StreamName();
		ClassMap["sbit14"] = TdTraits< CoreArray::BITS<-14> >::StreamName();
		ClassMap["sbit15"] = TdTraits< CoreArray::BITS<-15> >::StreamName();
		ClassMap["sbit16"] = TdTraits< CoreArray::Int16 >::StreamName();

		ClassMap["sbit24"] = TdTraits< CoreArray::Int24 >::StreamName();
		ClassMap["sbit32"] = TdTraits< CoreArray::Int32 >::StreamName();
		ClassMap["sbit64"] = TdTraits< CoreArray::Int64 >::StreamName();

		// **************************************************************
		// Float
		ClassMap["float32"] = TdTraits<CoreArray::Float32>::StreamName();
		ClassMap["float64"] = TdTraits<CoreArray::Float64>::StreamName();

		// **************************************************************
		// String
		ClassMap["string"] = TdTraits< VARIABLE_LENGTH<UTF8*> >::StreamName();
		ClassMap["string16"] = TdTraits< VARIABLE_LENGTH<UTF16*> >::StreamName();
		ClassMap["string32"] = TdTraits< VARIABLE_LENGTH<UTF32*> >::StreamName();
		ClassMap["fstring"] = TdTraits< FIXED_LENGTH<UTF8*> >::StreamName();
		ClassMap["fstring16"] = TdTraits< FIXED_LENGTH<UTF16*> >::StreamName();
		ClassMap["fstring32"] = TdTraits< FIXED_LENGTH<UTF32*> >::StreamName();

		// **************************************************************
		// R storage mode
		ClassMap["integer"] = TdTraits<CoreArray::Int32>::StreamName();
		ClassMap["numeric"] = TdTraits<CoreArray::Float64>::StreamName();
		ClassMap["float"] = TdTraits<CoreArray::Float32>::StreamName();
		ClassMap["double"] = TdTraits<CoreArray::Float64>::StreamName();
		ClassMap["character"] = TdTraits< VARIABLE_LENGTH<UTF8*> >::StreamName();
		ClassMap["logical"] = TdTraits<CoreArray::Int32>::StreamName();
		ClassMap["list"] = "$FOLDER$";
	}

	/// destructor
	~TInit()
	{
		for (int i=0; i < MaxFiles; i++)
		{
			if (Files[i] != NULL)
			{
				try {
					delete Files[i];
				}
				catch (...) { }
			}
		}
	}

	/// check init
	void CheckInit()
	{
		// register classess
		if (!_InitClassFlag)
		{
			RegisterClass();
			_InitClassFlag = true;
		}
	}

	/// get an empty GDS file and its index
	CdGDSFile *GetEmptyFile(int &OutIndex)
	{
		CheckInit();
		for (int i=0; i < MaxFiles; i++)
		{
			if (Files[i] == NULL)
			{
				CdGDSFile *rv = new CdGDSFile;
				OutIndex = i; Files[i] = rv;
				return rv;
			}
		}
		OutIndex = -1;
		throw ErrSequence(
			"You have opened %d GDS files, and no more is allowed!", MaxFiles);
	}

	/// get a specified GDS file
	CdGDSFile *GetFile(int Index)
	{
		if ((Index<0) || (Index>=MaxFiles))
			throw ErrSequence("Invalid GDS file ID!");
		CdGDSFile *rv = Files[Index];
		if (rv == NULL)
			throw ErrSequence("The GDS file has been closed.");
		return rv;
	}

private:
	bool _InitClassFlag;
};


static TInit Init;

/// error exception
class ErrGDSFmt: public ErrCoreArray
{
public:
	ErrGDSFmt() {};
	ErrGDSFmt(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
	ErrGDSFmt(const std::string &msg) { fMessage = msg; }
};


// error information
static const char *erNotFolder =
	"It is not a folder!";
static const char *erReadOnly =
	"Read only and please call 'compression.gdsn(node, \"\")' before writing.";
static const char *erWriteOnly =
	"Write only and please call 'readmode.gdsn' before reading.";


extern "C"
{

/// the number of preserved integers for a pointer to a GDS object
#define NUMBER_INT_FOR_GDSOBJ	4

/// (CdGDSObj*)  -->  int
inline static void _GDSObj2Int(int dst[], CdGDSObj *src)
{
	memset(dst, 0, sizeof(int)*NUMBER_INT_FOR_GDSOBJ);
	memcpy(dst, &src, sizeof(CdGDSObj*));
}

/// int  -->  (CdGDSObj*)
inline static CdGDSObj * _Int2GDSObj(int src[])
{
	CdGDSObj *dst = NULL;
	memcpy(&dst, src, sizeof(CdGDSObj*));
	return dst;
}


extern SEXP gdsObjWriteAll(SEXP Node, SEXP Val, SEXP Check);
extern SEXP gdsObjSetDim(SEXP Node, SEXP DLen);




// ************************************************************************ //
// ************************************************************************ //
// ************************************************************************ //

/// return true, if Obj is a logical object in R
DLLEXPORT bool gds_Is_R_Logical(CdGDSObj &Obj)
{
	return Obj.Attribute().HasName("R.logical");
}


/// return true, if Obj is a factor variable
DLLEXPORT bool gds_Is_R_Factor(CdGDSObj &Obj)
{
	if (Obj.Attribute().HasName("R.class") && Obj.Attribute().HasName("R.levels"))
	{
		return (Obj.Attribute()["R.class"].GetStr8() == "factor");
	} else
		return false;
}


/// return 1, if Obj is a factor object in R; otherwise return 0
DLLEXPORT int gds_Set_If_R_Factor(CdGDSObj &Obj, SEXP val)
{
	int nProtected = 0;

	if (Obj.Attribute().HasName("R.class") && Obj.Attribute().HasName("R.levels"))
	{
		if (Obj.Attribute()["R.class"].GetStr8() == "factor")
		{
			if (Obj.Attribute()["R.levels"].IsArray())
			{
				const TdsAny *p = Obj.Attribute()["R.levels"].GetArray();
				UInt32 L = Obj.Attribute()["R.levels"].GetArrayLength();

				SEXP levels;
				PROTECT(levels = NEW_CHARACTER(L));
				nProtected ++;
				for (UInt32 i=0; i < L; i++)
				{
					SET_STRING_ELT(levels, i, mkCharCE(p[i].
						GetStr8().c_str(), CE_UTF8));
				}

				SET_LEVELS(val, levels);
				SET_CLASS(val, mkString("factor"));
			} else if (Obj.Attribute()["R.levels"].IsString())
			{
				SEXP levels;
				PROTECT(levels = NEW_CHARACTER(1));
				nProtected ++;
				SET_STRING_ELT(levels, 0, mkCharCE(Obj.Attribute()
					["R.levels"].GetStr8().c_str(), CE_UTF8));

				SET_LEVELS(val, levels);
				SET_CLASS(val, mkString("factor"));
			}
		}
	}

	// output
	return nProtected;
}


/// return an R data object
/** \param Obj       [in] a list of objects of 'gdsn' class
 *  \param Start     [in] could be NULL
 *  \param Length    [in] could be NULL
**/
DLLEXPORT SEXP gds_Read_SEXP(CdSequenceX *Obj, CoreArray::Int32 const* Start,
	CoreArray::Int32 const* Length, const CBOOL *const Selection[])
{
	CORETRY
		SEXP rv_ans = R_NilValue;

		if (Obj->DimCnt() > 0)
		{
			int nProtected = 0;

			CdSequenceX::TSeqDimBuf St, Cnt;
			if (Start == NULL)
			{
				memset(St, 0, sizeof(St));
				Start = St;
			}
			if (Length == NULL)
			{
				Obj->GetDimLen(Cnt);
				Length = Cnt;
			}

			CdSequenceX::TSeqDimBuf ValidCnt;
			Obj->GetInfoSelection(Start, Length, Selection, NULL, NULL, ValidCnt);

			Int64 TotalCount = 1;
			for (int i=0; i < Obj->DimCnt(); i++)
				TotalCount *= ValidCnt[i];

			if (TotalCount > 0)
			{
				void *buffer;
				TSVType SV;
				if (COREARRAY_SV_INTEGER(Obj->SVType()))
				{
					if (gds_Is_R_Logical(*Obj))
					{
						PROTECT(rv_ans = NEW_LOGICAL(TotalCount));
						buffer = LOGICAL(rv_ans);
					} else {
						PROTECT(rv_ans = NEW_INTEGER(TotalCount));
						nProtected += gds_Set_If_R_Factor(*Obj, rv_ans);
						buffer = INTEGER(rv_ans);
					}
					SV = svInt32;
				} else if (COREARRAY_SV_FLOAT(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_NUMERIC(TotalCount));
					buffer = REAL(rv_ans);
					SV = svFloat64;
				} else if (COREARRAY_SV_STRING(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_CHARACTER(TotalCount));
					buffer = NULL;
					SV = svStrUTF8;
				} else
					throw ErrGDSFmt("Invalid SVType of array-oriented object.");
				nProtected ++;

				// initialize dimension
				if (Obj->DimCnt() > 1)
				{
					SEXP dim;
					PROTECT(dim = NEW_INTEGER(Obj->DimCnt()));
					nProtected ++;
					int I = 0;
					for (int k=Obj->DimCnt()-1; k >= 0; k--)
						INTEGER(dim)[ I++ ] = ValidCnt[k];
					SET_DIM(rv_ans, dim);
				}

				if (buffer != NULL)
				{
					if (!Selection)
						Obj->rData(Start, Length, buffer, SV);
					else
						Obj->rDataEx(Start, Length, Selection, buffer, SV);
				} else {
					vector<string> strbuf(TotalCount);
					if (!Selection)
						Obj->rData(Start, Length, &strbuf[0], SV);
					else
						Obj->rDataEx(Start, Length, Selection, &strbuf[0], SV);
					for (size_t i=0; i < strbuf.size(); i++)
						SET_STRING_ELT(rv_ans, i, mkChar(strbuf[i].c_str()));
				}
			} else {
				if (COREARRAY_SV_INTEGER(Obj->SVType()))
				{
					if (gds_Is_R_Logical(*Obj))
					{
						PROTECT(rv_ans = NEW_LOGICAL(0));
					} else {
						PROTECT(rv_ans = NEW_INTEGER(0));
						nProtected += gds_Set_If_R_Factor(*Obj, rv_ans);
					}
				} else if (COREARRAY_SV_FLOAT(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_NUMERIC(0));
				} else if (COREARRAY_SV_STRING(Obj->SVType()))
				{
					PROTECT(rv_ans = NEW_CHARACTER(0));
				} else
					throw ErrGDSFmt("Invalid SVType of array-oriented object.");
				nProtected ++;
			}

			// unprotect the object
			if (nProtected > 0)
				UNPROTECT(nProtected);
		}

		return rv_ans;
	}
	catch (ErrAllocRead &E) {
		Init.LastError = erWriteOnly;
		error(Init.LastError.c_str());
	}
	catch (exception &E) {
		Init.LastError = E.what();
		error(Init.LastError.c_str());
	}
	catch (const char *E) {
		Init.LastError = E;
		error(Init.LastError.c_str());
	}
	return R_NilValue;
}



// ****************************************************************************
// File Operations
// ****************************************************************************

/// create a GDS file
/** \param FileName    [in] the file name
 *  output:
 *    $filename    the file name to be created
 *    $id          ID of GDS file, internal use
 *    $root        the root of hierachical structure
 *    $readonly	   whether it is read-only or not
**/
DLLEXPORT SEXP gdsCreateGDS(SEXP FileName)
{
	const char *fn = CHAR(STRING_ELT(FileName, 0));
	CdGDSFile *file = NULL;
	int gds_id = -1;

	CORETRY

		file = Init.GetEmptyFile(gds_id);
		file->SaveAsFile(fn);

		SEXP ans_rv, tmp;
		PROTECT(ans_rv = NEW_LIST(4));
			SET_ELEMENT(ans_rv, 0, FileName);
			PROTECT(tmp = NEW_INTEGER(1));
			INTEGER(tmp)[0] = gds_id;
			SET_ELEMENT(ans_rv, 1, tmp);
			PROTECT(tmp = NEW_INTEGER(NUMBER_INT_FOR_GDSOBJ));
			_GDSObj2Int(INTEGER(tmp), &(file->Root()));
			SET_ELEMENT(ans_rv, 2, tmp);
			PROTECT(tmp = NEW_LOGICAL(1));
			LOGICAL(tmp)[0] = FALSE;
			SET_ELEMENT(ans_rv, 3, tmp);
		UNPROTECT(4);
		return ans_rv;

	CORECATCH(
		if ((file!=NULL) && !file->Log().List().empty())
		{
			Init.LastError.append(sLineBreak);
			Init.LastError.append("Log:");
			for (size_t i=0; i < file->Log().List().size(); i++)
			{
				Init.LastError.append(sLineBreak);
				Init.LastError.append(file->Log().List()[i].Msg);
			}
		}
		if (file) delete file;
		if (gds_id >= 0) Init.Files[gds_id] = NULL;
		error(Init.LastError.c_str());
	)

	return R_NilValue;
}


/// open a existing GDS file
/** \param FileName    [in] the file name
 *  output:
 *    $filename    the file name to be created
 *    $id          ID of GDS file, internal use
 *    $root        the root of hierachical structure
 *    $readonly	   whether it is read-only or not
**/
DLLEXPORT SEXP gdsOpenGDS(SEXP FileName, SEXP ReadOnly)
{
	const char *fn = CHAR(STRING_ELT(FileName, 0));
	CdGDSFile *file = NULL;
	int gds_id = -1;

	CORETRY

		file = Init.GetEmptyFile(gds_id);
		file->LoadFile(fn, LOGICAL(ReadOnly)[0]==TRUE);

		SEXP ans_rv, tmp;
		PROTECT(ans_rv = NEW_LIST(4));
			SET_ELEMENT(ans_rv, 0, FileName);
			PROTECT(tmp = NEW_INTEGER(1));
			INTEGER(tmp)[0] = gds_id;
			SET_ELEMENT(ans_rv, 1, tmp);
			PROTECT(tmp = NEW_INTEGER(NUMBER_INT_FOR_GDSOBJ));
			_GDSObj2Int(INTEGER(tmp), &(file->Root()));
			SET_ELEMENT(ans_rv, 2, tmp);
			PROTECT(tmp = NEW_LOGICAL(1));
			LOGICAL(tmp)[0] = (LOGICAL(ReadOnly)[0]==TRUE);
			SET_ELEMENT(ans_rv, 3, tmp);
		UNPROTECT(4);
		return ans_rv;

	CORECATCH(
		if ((file!=NULL) && !file->Log().List().empty())
		{
			Init.LastError.append(sLineBreak);
			Init.LastError.append("Log:");
			for (size_t i=0; i < file->Log().List().size(); i++)
			{
				Init.LastError.append(sLineBreak);
				Init.LastError.append(file->Log().List()[i].Msg);
			}
		}
		if (file) delete file;
		if (gds_id >= 0) Init.Files[gds_id] = NULL;
		error(Init.LastError.c_str());
	)

	return R_NilValue;
}


/// close a GDS file
/** \param gds_id      [in] the internal file id
**/
DLLEXPORT SEXP gdsCloseGDS(SEXP gds_id)
{
	CORETRY
		int id = INTEGER(gds_id)[0];
		CdGDSFile *file = Init.GetFile(id);
		Init.Files[id] = NULL;
		delete file;
	CORECATCH_ERROR
}


/// synchronize a GDS file
/** \param gds_id      [in] the internal file id
**/
DLLEXPORT SEXP gdsSyncGDS(SEXP gds_id)
{
	CORETRY
		int id = INTEGER(gds_id)[0];
		Init.GetFile(id)->SyncFile();
	CORECATCH_ERROR
}


/// detect whether a file has been opened
/** \param gds_id      [in] the internal file index
**/
DLLEXPORT SEXP gdsFileValid(SEXP gds_id)
{
	int id = INTEGER(gds_id)[0];
	if ((0 <= id) && (id < Init.MaxFiles))
	{
		if (Init.Files[id]) return R_NilValue;
	}
	error("The GDS file has been closed.");
	return R_NilValue;
}


/// clean up fragments of a GDS file
/** \param FileName    [in] the file name
 *  \param Deep        [in] if TRUE, clean up the file with a deep-construction strategy
 *  \param verbose     [in] if TRUE, show information
**/
DLLEXPORT SEXP gdsTidyUp(SEXP FileName, SEXP Deep, SEXP Verbose)
{
	const char *fn = CHAR(STRING_ELT(FileName, 0));
	const bool verbose = (LOGICAL(Verbose)[0] == TRUE);

	CORETRY
		CdGDSFile file(fn, CdGDSFile::dmOpenReadWrite);
		if (verbose)
		{
			Rprintf("Clean up the fragments of GDS file:\n");
			Rprintf("\topen the file \"%s\" (size: %s).\n", fn,
				IntToStr(file.GetFileSize()).c_str());
			Rprintf("\t# of fragments in total: %d.\n",
				file.GetNumOfFragment());
			Rprintf("\tsave it to \"%s.tmp\".\n", fn);
		}
		file.TidyUp(LOGICAL(Deep)[0] == TRUE);
		if (verbose)
		{
			Rprintf("\trename \"%s.tmp\" (size: %s).\n", fn,
				IntToStr(file.GetFileSize()).c_str());
			Rprintf("\t# of fragments in total: %d.\n",
				file.GetNumOfFragment());
		}
	CORECATCH_ERROR
}




// *****************************************************************************
// File Structure Operations
// *****************************************************************************

/// detect whether a node is valid (internal use)
/** \param Node        [in] a specified GDS node **/
static CdGDSObj *_NodeValidSEXP(SEXP Node)
{
	CdGDSObj *n = _Int2GDSObj(INTEGER(Node));
	try {
		if (n != NULL)
		{
			CdGDSFile *file = n->GDSFile();
			if (file != NULL)
			{
				for (int i=0; i < Init.MaxFiles; i++)
				{
					if (Init.Files[i] == file)
						return n;
				}
			}
			n = NULL;
		}
	}
	catch (exception &E)
		{ n = NULL; }
	catch (...)
		{ n = NULL; }
	if (n == NULL)
		error("The GDS file has been closed.");
	return n;
}


/// detect whether a node is valid
/** \param Node        [in] a specified GDS node
**/
DLLEXPORT SEXP gdsNodeValid(SEXP Node)
{
	_NodeValidSEXP(Node);
	return R_NilValue;
}


/// get the number of variables in a folder
/** \param Node        [in] a specified GDS node
**/
DLLEXPORT SEXP gdsNodeChildCnt(SEXP Node)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	CORETRY
		int rv = (dynamic_cast<CdGDSFolder*>(n)) ?
			(static_cast<CdGDSFolder*>(n)->Count()) : 0;
		SEXP ans_rv;
		PROTECT(ans_rv = NEW_INTEGER(1));
		INTEGER(ans_rv)[0] = rv;
		UNPROTECT(1);
		return ans_rv;
	CORECATCH_ERROR
}


/// get the name of a specified node
/** \param Node        [in] a specified GDS node
 *  \param Full        [in] if TRUE, return the name with full path
**/
DLLEXPORT SEXP gdsNodeName(SEXP Node, SEXP Full)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	CORETRY
		string nm;
		if (LOGICAL(Full)[0] == TRUE)
			nm = UTF16toUTF8(n->FullName());
		else
			nm = UTF16toUTF8(n->Name());
		SEXP ans_rv;
		PROTECT(ans_rv = NEW_STRING(1));
		SET_STRING_ELT(ans_rv, 0, mkChar(nm.c_str()));
		UNPROTECT(1);
		return ans_rv;
	CORECATCH_ERROR
}


/// enumerate the names of its child nodes
/** \param Node        [in] a specified GDS node
**/
DLLEXPORT SEXP gdsNodeEnumName(SEXP Node)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	CORETRY
		if (dynamic_cast<CdGDSFolder*>(n))
		{
			CdGDSFolder &Dir = *static_cast<CdGDSFolder*>(n);
			SEXP ans_rv;
			PROTECT(ans_rv = NEW_STRING(Dir.Count()));
			for (int i=0; i < (int)Dir.Count(); i++)
			{
				SET_STRING_ELT(ans_rv, i,
					mkChar(UTF16toUTF8(Dir.ObjItem(i)->Name()).c_str()));
			}
			UNPROTECT(1);
			return ans_rv;
		} else
			throw ErrGDSFmt(erNotFolder);
	CORECATCH_ERROR
}


/// get the node with index or indices
/** \param Node        [in] a specified GDS node
 *  \param Path        [in] the full path of a specified node
 *  \param Index       [in] the index or indices of a specified node
 *  \param Silent      [in] return R_NilValue if fails and `Silent=TRUE'
**/
DLLEXPORT SEXP gdsNodeIndex(SEXP Node, SEXP Path, SEXP Index, SEXP Silent)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	bool silent_flag = (LOGICAL(Silent)[0] == TRUE);

	CORETRY
		int nProtected = 0;

		if (Rf_isNull(Path))
		{
			// "path=NULL, index=..."

			if (!Rf_isNumeric(Index) && !Rf_isString(Index))
				throw ErrGDSFile("`index' should numeric values or characters.");
			if (Rf_isReal(Index))
			{
				PROTECT(Index = Rf_coerceVector(Index, INTSXP));
				nProtected ++;
			}

			for (int i=0; i < XLENGTH(Index); i++)
			{
				if (!dynamic_cast<CdGDSFolder*>(n))
				{
					string pn = UTF16toUTF8(n->FullName());
					if (pn.empty()) pn = "$ROOT$";
					throw ErrGDSFile("'%s' is not a folder.", pn.c_str());
				}
				CdGDSFolder &Dir = *static_cast<CdGDSFolder*>(n);

				if (Rf_isInteger(Index))
				{
					int idx = INTEGER(Index)[i];
					if ((idx < 1) || (idx > (int)Dir.Count()))
					{
						string pn = UTF16toUTF8(n->FullName());
						if (pn.empty()) pn = "$ROOT$";
						throw ErrGDSFile("'%s' index[%d], out of range 1..%d.",
							pn.c_str(), idx, Dir.Count());
					}
					n = Dir.ObjItem(idx - 1);
				} else if (Rf_isString(Index))
				{
					const char *nm = CHAR(STRING_ELT(Index, i));
					CdGDSObj *tmp = n;
					n = Dir.ObjItemEx(nm);
					if (n == NULL)
					{
						string pn = UTF16toUTF8(n->FullName());
						if (pn.empty()) pn = "$ROOT$";
						throw ErrGDSFile("'%s' has no node of '%s'.",
							pn.c_str(), nm);
					}
				} else
					throw ErrGDSFile("Internal error in `gdsNodeIndex'.");
			}
		} else {
			// "path=..., index=NULL"

			if (!Rf_isNull(Index))
				throw ErrGDSFile("`index' should be NULL if `path' is specified.");
			if (!Rf_isString(Path))
				throw ErrGDSFile("`path' should be character-type.");
			if (XLENGTH(Path) != 1)
				throw ErrGDSFile("Please use '/' as a separator.");

			CdGDSFolder &Dir = *static_cast<CdGDSFolder*>(n);
			const char *nm = CHAR(STRING_ELT(Path, 0));
			n = Dir.PathEx(PChartoUTF16(nm));
			if (n == NULL)
				throw ErrGDSFile("There is no '%s'.", nm);
		}

		SEXP ans_rv;
		PROTECT(ans_rv = NEW_INTEGER(NUMBER_INT_FOR_GDSOBJ));
			_GDSObj2Int(INTEGER(ans_rv), n);
		nProtected ++;
		UNPROTECT(nProtected);

		return ans_rv;
	}
	catch (exception &E) {
		Init.LastError = E.what();
		if (!silent_flag)
			error(Init.LastError.c_str());
	}
	catch (const char *E) {
		Init.LastError = E;
		if (!silent_flag)
			error(Init.LastError.c_str());
	}
	return R_NilValue;
}


/// get the description of a specified node
/** \param Node        [in] a specified GDS node
**/
DLLEXPORT SEXP gdsNodeObjDesp(SEXP Node)
{
	CdGDSObj *n = _NodeValidSEXP(Node);

	CORETRY

		SEXP ans_rv, tmp;
		int nProtected = 0;
		PROTECT(ans_rv = NEW_LIST(8)); nProtected++;

			// 1: name
			PROTECT(tmp = NEW_STRING(1)); nProtected++;
			SET_ELEMENT(ans_rv, 0, tmp);
			SET_STRING_ELT(tmp, 0, mkChar(UTF16toUTF8(n->Name()).c_str()));

			// 2: full name
			PROTECT(tmp = NEW_STRING(1)); nProtected++;
			SET_ELEMENT(ans_rv, 1, tmp);
			SET_STRING_ELT(tmp, 0, mkChar(UTF16toUTF8(n->FullName()).c_str()));

			// 3: storage, the description of data field, such like "Int32"
			PROTECT(tmp = NEW_STRING(1)); nProtected++;
			SET_ELEMENT(ans_rv, 2, tmp);
			SET_STRING_ELT(tmp, 0, mkChar(n->dTraitName()));

			// 4: type (a factor)
			//   1 -- rtNULL,    2 -- rtFolder, 3 -- rtRaw,
			//   4 -- rtInteger, 5 -- rtFactor, 6 -- rtLogical,
			//   7 -- rtReal,    8 -- rtString, 9 -- rtUnknown
			PROTECT(tmp = NEW_INTEGER(1)); nProtected++;
			SET_ELEMENT(ans_rv, 3, tmp);
			if (dynamic_cast<CdGDSNull*>(n))
				INTEGER(tmp)[0] = 1;
			else if (dynamic_cast<CdGDSFolder*>(n))
				INTEGER(tmp)[0] = 2;
			else if (dynamic_cast<CdGDSStreamContainer*>(n))
				INTEGER(tmp)[0] = 3;
			else if (dynamic_cast<CdContainer*>(n))
			{
				CdContainer* nn = static_cast<CdContainer*>(n);
				TSVType sv = nn->SVType();
				if (COREARRAY_SV_INTEGER(sv))
				{
					if (gds_Is_R_Factor(*n))
						INTEGER(tmp)[0] = 5;
					else if (gds_Is_R_Logical(*n))
						INTEGER(tmp)[0] = 6;
					else
						INTEGER(tmp)[0] = 4;
				} else if (COREARRAY_SV_FLOAT(sv))
					INTEGER(tmp)[0] = 7;
				else if (COREARRAY_SV_STRING(sv))
					INTEGER(tmp)[0] = 8;
				else
					INTEGER(tmp)[0] = 9;
			} else
				INTEGER(tmp)[0] = 9;

			// 5: is.array
			PROTECT(tmp = NEW_LOGICAL(1)); nProtected++;
			SET_ELEMENT(ans_rv, 4, tmp);
			LOGICAL(tmp)[0] = dynamic_cast<CdSequenceX*>(n) ? TRUE : FALSE;

			// 6: dim, the dimension of data field
			// 7: compress, the compression method: "", "ZIP"
			// 8: cpratio, data compression ratio, "NaN" indicates no compression
			if (dynamic_cast<CdSequenceX*>(n))
			{
				CdSequenceX *Obj = static_cast<CdSequenceX*>(n);

				PROTECT(tmp = NEW_INTEGER(Obj->DimCnt())); nProtected++;
				SET_ELEMENT(ans_rv, 5, tmp);
				for (int i=0; i < Obj->DimCnt(); i++)
					INTEGER(tmp)[i] = Obj->GetDLen(Obj->DimCnt()-i-1);

				SEXP coder, ratio;
				PROTECT(coder = NEW_STRING(1)); nProtected++;
				SET_ELEMENT(ans_rv, 6, coder);
				SET_STRING_ELT(coder, 0, mkChar(""));

				PROTECT(ratio = NEW_NUMERIC(1)); nProtected++;
				SET_ELEMENT(ans_rv, 7, ratio);
				REAL(ratio)[0] = R_NaN;

				if (Obj->PipeInfo())
				{
					SET_STRING_ELT(coder, 0, mkChar(Obj->PipeInfo()->Coder()));
					if (Obj->PipeInfo()->StreamTotalIn() > 0)
					{
						REAL(ratio)[0] = (double)
							Obj->PipeInfo()->StreamTotalOut() /
							Obj->PipeInfo()->StreamTotalIn();
					}
				}
			} else if (dynamic_cast<CdGDSStreamContainer*>(n))
			{
				CdGDSStreamContainer *Obj = static_cast<CdGDSStreamContainer*>(n);

				PROTECT(tmp = NEW_NUMERIC(1)); nProtected++;
				SET_ELEMENT(ans_rv, 5, tmp);

				SEXP coder, ratio;
				PROTECT(coder = NEW_STRING(1)); nProtected++;
				SET_ELEMENT(ans_rv, 6, coder);
				SET_STRING_ELT(coder, 0, mkChar(""));

				PROTECT(ratio = NEW_NUMERIC(1)); nProtected++;
				SET_ELEMENT(ans_rv, 7, ratio);
				REAL(ratio)[0] = R_NaN;

				if (Obj->PipeInfo())
				{
					REAL(tmp)[0] = Obj->PipeInfo()->StreamTotalIn();
					SET_STRING_ELT(coder, 0, mkChar(Obj->PipeInfo()->Coder()));
					if (Obj->PipeInfo()->StreamTotalIn() > 0)
					{
						REAL(ratio)[0] = (double)
							Obj->PipeInfo()->StreamTotalOut() /
							Obj->PipeInfo()->StreamTotalIn();
					}
				} else
					REAL(tmp)[0] = Obj->GetSize();
			}

		UNPROTECT(nProtected);
		return ans_rv;

	CORECATCH_ERROR
}


/// add a new node
/** \param Node        [in] a specified GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param Val         [in] the values
 *  \param Storage     [in] the mode of storage
 *  \param ValDim      [in] the dimension
 *  \param Compress    [in] the method of compression
 *  \param CloseZip    [in] if compressing data and TRUE, get into read mode after adding
 *  \param Check       [in]
 *  \param Replace     [in]
**/
DLLEXPORT SEXP gdsAddNode(SEXP Node, SEXP NodeName, SEXP Val, SEXP Storage,
	SEXP ValDim, SEXP Compress, SEXP CloseZip, SEXP Check, SEXP Replace)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	const char *nm  = CHAR(STRING_ELT(NodeName, 0));
	const char *stm = CHAR(STRING_ELT(Storage, 0));
	const char *cp = CHAR(STRING_ELT(Compress, 0));

	if (!Rf_isNull(ValDim) && !Rf_isNumeric(ValDim))
		error("`valdim' should be NULL or a numeric vector");

	if (!dynamic_cast<CdGDSFolder*>(n))
		error(erNotFolder);

	CORETRY
		CdGDSFolder &Dir = *static_cast<CdGDSFolder*>(n);
		int IdxReplace = -1;

		if (LOGICAL(Replace)[0] == TRUE)
		{
			CdGDSObj *tmp = Dir.ObjItemEx(nm);
			if (tmp)
			{
				IdxReplace = Dir.IndexObj(tmp);
				Dir.DeleteObj(tmp, true);
			}
		}

		CdGDSObj *ans_rv = NULL;

		// class name mapping
		if (strcmp(stm, "NULL") == 0)
		{
			ans_rv = new CdGDSNull();
		} else {
			map<const char*, const char*, TInit::strCmp>::iterator it;
			it = Init.ClassMap.find(stm);

			if (it != Init.ClassMap.end())
				stm = it->second;

			if (strcmp(stm, "$FOLDER$") != 0)
			{
				CdObjClassMgr::TdOnObjCreate OnCreate =
					dObjManager().NameToClass(stm);
				if (OnCreate)
				{
					CdObject *obj = OnCreate();
					if (dynamic_cast<CdGDSObj*>(obj))
						ans_rv = static_cast<CdGDSObj*>(obj);
					else
						delete obj;
				} else {
					CdObjClassMgr::TdOnObjCreate OnCreate =
						dObjManager().NameToClass(
						(string("d") + string(stm)).c_str());
					if (OnCreate)
					{
						CdObject *obj = OnCreate();
						if (dynamic_cast<CdGDSObj*>(obj))
							ans_rv = static_cast<CdGDSObj*>(obj);
						else
							delete obj;
					}
				}
			} else
				ans_rv = &Dir.AddFolder(nm);
		}

		if (ans_rv == NULL)
			throw ErrGDSFmt("Not support the storage mode '%s'.", stm);

		SEXP ans_rv_node;
		PROTECT(ans_rv_node = NEW_INTEGER(NUMBER_INT_FOR_GDSOBJ));
			_GDSObj2Int(INTEGER(ans_rv_node), ans_rv);

		if (dynamic_cast<CdSequenceX*>(ans_rv))
		{
			// set the compression mode
			CdSequenceX *Obj = static_cast<CdSequenceX*>(ans_rv);

			CORETRY
				Obj->AddDim(0);
				Obj->SetPackedMode(cp);
				Dir.InsertObj(IdxReplace, nm, Obj);
			CORECATCH({ delete Obj; throw; })

			if (!Rf_isNull(Val))
			{
				if (Rf_isNumeric(Val) || Rf_isString(Val) ||
					Rf_isLogical(Val) || Rf_isFactor(Val))
				{
					// check first
					if ((dynamic_cast<CdFStr8*>(ans_rv) ||
						dynamic_cast<CdFStr16*>(ans_rv)) && Rf_isString(Val))
					{
						int MaxLen = 0;
						R_xlen_t len = XLENGTH(Val);
						for (R_xlen_t i=0; i < len; i++)
						{
							const char *s = CHAR(STRING_ELT(Val, i));
							int l = strlen(s);
							if (l > MaxLen) MaxLen = l;
						}
						if (dynamic_cast<CdFStr8*>(ans_rv))
							static_cast<CdFStr8*>(ans_rv)->SetMaxLength(MaxLen);
						else
							static_cast<CdFStr16*>(ans_rv)->SetMaxLength(MaxLen);
					}

					// call write all
					gdsObjWriteAll(ans_rv_node, Val, Check);

					// close the compression
					if (Obj->PipeInfo() && (LOGICAL(CloseZip)[0]==TRUE))
						Obj->CloseWriter();

					// set dimensions
					if (!Rf_isNull(ValDim))
						gdsObjSetDim(ans_rv_node, ValDim);
				} else
					throw ErrGDSFmt("Not support `val'.");
			} else {
				// set dimensions
				if (!Rf_isNull(ValDim))
					gdsObjSetDim(ans_rv_node, ValDim);
			}
		} else {
			CORETRY
				if (!dynamic_cast<CdGDSFolder*>(ans_rv))
					Dir.AddObj(nm, ans_rv);
			CORECATCH({ delete ans_rv; throw; })
		}

		UNPROTECT(1);
		return ans_rv_node;

	CORECATCH_ERROR
}


/// add a new node with a specified file
/** \param Node        [in] a specified GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param FileName    [in] the name of input file
 *  \param Compress    [in] the method of compression
**/
DLLEXPORT SEXP gdsAddFile(SEXP Node, SEXP NodeName, SEXP FileName,
	SEXP Compress)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	const char *nm = CHAR(STRING_ELT(NodeName, 0));
	const char *fn = CHAR(STRING_ELT(FileName, 0));
	const char *cp = CHAR(STRING_ELT(Compress, 0));

	CORETRY
		// the pointer to the directory
		if (!dynamic_cast<CdGDSFolder*>(n))
			throw ErrGDSFmt(erNotFolder);
		CdGDSFolder &Dir = *static_cast<CdGDSFolder*>(n);

		TdAutoRef<CBufdStream> file(new CBufdStream(
			new CdFileStream(fn, CdFileStream::fmOpenRead)));
		CdGDSStreamContainer *vObj = new CdGDSStreamContainer();
		vObj->SetPackedMode(cp);
		n = Dir.AddObj(nm, vObj);
		vObj->CopyFrom(*file.get());
		vObj->CloseWriter();

		SEXP ans_rv;
		PROTECT(ans_rv = NEW_INTEGER(NUMBER_INT_FOR_GDSOBJ));
			_GDSObj2Int(INTEGER(ans_rv), n);
		UNPROTECT(1);

		return ans_rv;

	CORECATCH_ERROR
}


/// get the file from a file node
/** \param Node        [in] a specified GDS node
 *  \param OutFileName [in] the name for output file
**/
DLLEXPORT SEXP gdsGetFile(SEXP Node, SEXP OutFileName)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	const char *fn = CHAR(STRING_ELT(OutFileName, 0));

	CORETRY
		// the pointer to the file
		if (!dynamic_cast<CdGDSStreamContainer*>(n))
			throw ErrGDSFmt("It is not a stream container!");

		CdGDSStreamContainer *Obj = static_cast<CdGDSStreamContainer*>(n);
		TdAutoRef<CBufdStream> file(new CBufdStream(
			new CdFileStream(fn, CdFileStream::fmCreate)));
		Obj->CopyTo(*file.get());

	CORECATCH_ERROR
}


/// delete a node
/** \param Node        [in] a specified GDS node
 *  \param Force       [in] if TRUE, remove a folder no matter whether it is empty
**/
DLLEXPORT SEXP gdsDeleteNode(SEXP Node, SEXP Force)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	CORETRY
		if (n->Folder())
			n->Folder()->DeleteObj(n, LOGICAL(Force)[0]==TRUE);
		else
			throw ErrGDSFmt("Can not delete the root.");
	CORECATCH_ERROR
}


/// rename a node
/** \param Node        [in] a specified GDS node
 *  \param NewName     [in] the new name
**/
DLLEXPORT SEXP gdsRenameNode(SEXP Node, SEXP NewName)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	const char *nm = CHAR(STRING_ELT(NewName, 0));
	CORETRY
		n->SetName(nm);
	CORECATCH_ERROR
}




// ****************************************************************************
// Attribute Operations
// ****************************************************************************

/// get the attribute(s) of a GDS node
/** \param Node        [in] a specified GDS node
**/
DLLEXPORT SEXP gdsGetAttr(SEXP Node)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	CORETRY

		if (n->Attribute().Count() > 0)
		{
			SEXP ans_rv;
			int nProtected = 0;
			PROTECT(ans_rv = NEW_LIST(n->Attribute().Count()));
			nProtected ++;

			// the values
			for (int i=0; i < (int)n->Attribute().Count(); i++)
			{
				const TdsAny *p = &(n->Attribute()[i]);
				R_xlen_t Cnt = 1;
				if (p->IsArray())
				{
					Cnt = p->GetArrayLength();
					p = p->GetArray();
				}

				SEXP tmp = R_NilValue;
				if (Cnt > 0)
				{
					if (p->IsInt())
					{
						PROTECT(tmp = NEW_INTEGER(Cnt));
						nProtected ++;
						for (R_xlen_t i=0; i < Cnt; i++, p++)
							INTEGER(tmp)[i] = p->GetInt32();
					} else if (p->IsFloat())
					{
						PROTECT(tmp = NEW_NUMERIC(Cnt));
						nProtected ++;
						for (R_xlen_t i=0; i < Cnt; i++, p++)
							REAL(tmp)[i] = p->GetFloat64();
					} else if (p->IsString())
					{
						PROTECT(tmp = NEW_STRING(Cnt));
						nProtected ++;
						for (R_xlen_t i=0; i < Cnt; i++, p++)
							SET_STRING_ELT(tmp, i, mkChar(p->GetStr8().c_str()));
					} else if (p->IsBool())
					{
						PROTECT(tmp = NEW_LOGICAL(1));
						nProtected ++;
						for (R_xlen_t i=0; i < Cnt; i++, p++)
							LOGICAL(tmp)[i] = p->GetBool() ? TRUE : FALSE;
					}
				}
				SET_ELEMENT(ans_rv, i, tmp);
			}

			// the values
			SEXP nlist;
			PROTECT(nlist = NEW_STRING(n->Attribute().Count()));
			nProtected ++;
			for (int i=0; i < (int)n->Attribute().Count(); i++)
			{
				SET_STRING_ELT(nlist, i,
					mkChar(UTF16toUTF8(n->Attribute().Names(i)).c_str()));
			}
			SET_NAMES(ans_rv, nlist);

			UNPROTECT(nProtected);

			return ans_rv;
		}
	CORECATCH_ERROR
}


/// set an attribute
/** \param Node        [in] a specified GDS node
 *  \param Name        [in] the name of attribute
 *  \param Val         [in] the value
**/
DLLEXPORT SEXP gdsPutAttr(SEXP Node, SEXP Name, SEXP Val)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	const char *nm = CHAR(STRING_ELT(Name, 0));

	if (!Rf_isNull(Val) && !Rf_isInteger(Val) && !Rf_isReal(Val) &&
			!Rf_isString(Val) && !Rf_isLogical(Val))
		error("Unsupported type!");

	CORETRY

		TdsAny *p;
		if (n->Attribute().HasName(nm))
			p = &(n->Attribute()[nm]);
		else
			p = &(n->Attribute().Add(nm));

		if (Rf_isInteger(Val))
		{
			if (Rf_length(Val) == 1)
				p->SetInt32(*INTEGER(Val));
			else
				p->SetArray(INTEGER(Val), Rf_length(Val));
		} else if (Rf_isReal(Val))
		{
			if (Rf_length(Val) == 1)
				p->SetFloat64(*REAL(Val));
			else
				p->SetArray(REAL(Val), Rf_length(Val));
		} else if (Rf_isString(Val))
		{
			if (Rf_length(Val) == 1)
			{
				p->SetStr8(CHAR(STRING_ELT(Val, 0)));
			} else {
				vector<string> buf(Rf_length(Val));
				for (R_xlen_t i=0; i < Rf_length(Val); i++)
					buf[i] = CHAR(STRING_ELT(Val, i));
				p->SetArray(&(buf[0]), Rf_length(Val));
			}
		} else if (Rf_isLogical(Val))
		{
			if (Rf_length(Val) == 1)
			{
				p->SetBool(LOGICAL(Val)[0] == TRUE);
			} else {
				auto_ptr<bool> buf(new bool[Rf_length(Val)]);
				for (R_xlen_t i=0; i < Rf_length(Val); i++)
					buf.get()[i] = (LOGICAL(Val)[i] == TRUE);
				p->SetArray(buf.get(), Rf_length(Val));
			}
		}

	CORECATCH_ERROR
}


/// delete an attribute
/** \param Node        [in] a specified GDS node
 *  \param Name        [in] the name of attribute
**/
DLLEXPORT SEXP gdsDeleteAttr(SEXP Node, SEXP Name)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	const char *nm = CHAR(STRING_ELT(Name, 0));

	CORETRY
		n->Attribute().Delete(PChartoUTF16(nm));
	CORECATCH_ERROR
}





// ****************************************************************************
// Data Operations
// ****************************************************************************

/// convert RType to SVType
inline static TSVType RtoSV(int RType)
{
	switch (RType) {
		case 1: return svInt32;
		case 2: return svFloat64;
		case 3: return svStrUTF8;
		case 4: return svInt32;
		default: throw ErrGDSFmt("Invalid RType %d", RType);
	}
}

/// append data to a node
/** \param Node        [in] a specified GDS node
 *  \param Val         [in] the values
 *  \param Check       [in] if TRUE check
**/
DLLEXPORT SEXP gdsObjAppend(SEXP Node, SEXP Val, SEXP Check)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val))
		error("`val' should be integer, numeric, character or logical.");
	
	CdGDSObj *n = _NodeValidSEXP(Node);
	CORETRY
		if (dynamic_cast<CdSequenceX*>(n))
		{
			int nProtected = 0;
			CdSequenceX *Obj = static_cast<CdSequenceX*>(n);

			TSVType sv = Obj->SVType();
			if (COREARRAY_SV_INTEGER(sv))
			{
				PROTECT(Val = Rf_coerceVector(Val, INTSXP));
				nProtected ++;
				Obj->Append(INTEGER(Val), XLENGTH(Val), svInt32);
			} else if (COREARRAY_SV_FLOAT(sv))
			{
				PROTECT(Val = Rf_coerceVector(Val, REALSXP));
				nProtected ++;
				Obj->Append(REAL(Val), XLENGTH(Val), svFloat64);
			} else if (COREARRAY_SV_STRING(sv))
			{
				PROTECT(Val = Rf_coerceVector(Val, STRSXP));
				nProtected ++;
				R_xlen_t Len = XLENGTH(Val);
				if (LOGICAL(Check)[0] == TRUE)
				{
					for (R_xlen_t i=0; i < Len; i++)
					{
						if (STRING_ELT(Val, i) == NA_STRING)
						{
							warning("Missing characters are converted to \"\".");
							break;
						}
					}
				}
				vector<string> buf(Len);
				for (R_xlen_t i=0; i < Len; i++)
				{
					SEXP s = STRING_ELT(Val, i);
					if (s != NA_STRING) buf[i] = CHAR(s);
				}
				Obj->Append(&(buf[0]), Len, svStrUTF8);
			} else {
				throw ErrGDSFmt("Not support!");
			}

			UNPROTECT(nProtected);

			if (Obj->PipeInfo())
				Obj->PipeInfo()->UpdateStreamSize();

			if (LOGICAL(Check)[0] == TRUE)
			{
				if (dynamic_cast<CdVectorX*>(Obj))
				{
					CdVectorX *vObj = static_cast<CdVectorX*>(Obj);
					if (vObj->CurrectCnt() != vObj->Count())
						warning("Not a complete subset of data.");
				}
			}
		} else
			throw ErrGDSFmt("Not support!");
	}
	catch (ErrAllocWrite &E) {
		Init.LastError = erReadOnly;
		error(Init.LastError.c_str());
	}
	catch (exception &E) {
		Init.LastError = E.what();
		error(Init.LastError.c_str());
	}
	catch (const char *E) {
		Init.LastError = E;
		error(Init.LastError.c_str());
	}
	return R_NilValue;
}


/// read data from a node
/** \param Node        [in] a specified GDS node
 *  \param Start       [in] the starting position
 *  \param Count       [in] the count of each dimension
 *  \param Simplify    [in] if TRUE, convert to a vector if possible
**/
DLLEXPORT SEXP gdsObjReadData(SEXP Node, SEXP Start, SEXP Count,
	SEXP Simplify)
{
	CdGDSObj *n = _NodeValidSEXP(Node);

	if (!Rf_isNull(Start) && !Rf_isNumeric(Start))
		error("`start' should be numeric.");
	if (!Rf_isNull(Count) && !Rf_isNumeric(Count))
		error("`count' should be numeric.");
	if ((Rf_isNull(Start) && !Rf_isNull(Count)) ||
			(!Rf_isNull(Start) && Rf_isNull(Count)))
		error("`start' and 'count' should be both NULL.");

	if (!Rf_isLogical(Simplify) || (XLENGTH(Simplify)!=1))
		error("`simplify' should be a logical variable with 1 element.");

	if (!dynamic_cast<CdSequenceX*>(n))
		error("There is no data field.");
	CdSequenceX *Obj = static_cast<CdSequenceX*>(n);

	CdSequenceX::TSeqDimBuf DStart, DLen;
	CoreArray::Int32 *pDS=NULL, *pDL=NULL;
	if (!Rf_isNull(Start) && !Rf_isNull(Count))
	{
		int Len = Obj->DimCnt();
		CdSequenceX::TSeqDimBuf DCnt;
		Obj->GetDimLen(DCnt);

		PROTECT(Start = Rf_coerceVector(Start, INTSXP));
		if (XLENGTH(Start) != Len)
			error("The length of `start' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Start)[i];
			if ((v < 1) || (v > DCnt[Len-i-1]))
				error("`start' is invalid.");
			DStart[Len-i-1] = v-1;
		}
		pDS = DStart;

		PROTECT(Count = Rf_coerceVector(Count, INTSXP));
		if (XLENGTH(Count) != Len)
			error("The length of `count' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Count)[i];
			if (v == -1)
				v = DCnt[Len-i-1] - DStart[Len-i-1];
			if ((v < 0) || ((DStart[Len-i-1]+v) > DCnt[Len-i-1]))
				error("`count' is invalid.");
			DLen[Len-i-1] = v;
		}
		pDL = DLen;

		UNPROTECT(2);
	}

	SEXP ans_rv = gds_Read_SEXP(Obj, pDS, pDL, NULL);
	if (LOGICAL(Simplify)[0] == TRUE)
	{
		PROTECT(ans_rv);
		SEXP dim = getAttrib(ans_rv, R_DimSymbol);
		if (!Rf_isNull(dim))
		{
			int Num_GreaterOne = 0;
			for (int i=0; i < XLENGTH(dim); i++)
			{
				if (INTEGER(dim)[i] > 1)
					Num_GreaterOne ++;
			}
			if (Num_GreaterOne <= 1)
				setAttrib(ans_rv, R_DimSymbol, R_NilValue);
		}
		UNPROTECT(1);
	}

	return ans_rv;
}


/// read data from a node with a selection
/** \param Node        [in] a specified GDS node
 *  \param Selection   [in] the logical variable of selection
 *  \param Simplify    [in] if TRUE, convert to a vector if possible
**/
DLLEXPORT SEXP gdsObjReadExData(SEXP Node, SEXP Selection, SEXP Simplify)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	if (!Rf_isLogical(Simplify) || (XLENGTH(Simplify)!=1))
		error("`simplify' should be a logical variable with 1 element.");

	CORETRY
		if (dynamic_cast<CdSequenceX*>(n))
		{
			CdSequenceX *Obj = static_cast<CdSequenceX*>(n);
			vector< vector<CBOOL> > Select;

			if (Rf_isVectorList(Selection))
			{
				if (XLENGTH(Selection) != Obj->DimCnt())
					throw ErrGDSFmt("The dimension of `sel' is not correct.");

				Select.resize(Obj->DimCnt());
				for (int i=0; i < XLENGTH(Selection); i++)
				{
					SEXP tmp = VECTOR_ELT(Selection, i);
					if (Rf_isLogical(tmp))
					{
						int k = Obj->DimCnt() - i - 1;
						R_xlen_t Len = Obj->GetDLen(k);
						if (XLENGTH(tmp) != Len)
							throw ErrGDSFmt("The length of `sel[[%d]]' is not correct.", i+1);
						vector<CBOOL> &ss = Select[k];
						ss.resize(Obj->GetDLen(k));
						for (R_xlen_t j=0; j < Len; j++)
							ss[j] = (LOGICAL(tmp)[j] == TRUE);
					} else {
						throw ErrGDSFmt(
							"`sel[[%d]]' should be a logical variable.", i+1);
					}
				}
			} else
				throw ErrGDSFmt("`sel' should be a list or a logical variable.");

			vector<CBOOL*> SelList(Obj->DimCnt());
			for (int i=0; i < Obj->DimCnt(); i++)
				SelList[i] = &(Select[i][0]);

			SEXP ans_rv = gds_Read_SEXP(Obj, NULL, NULL, &(SelList[0]));
			if (LOGICAL(Simplify)[0] == TRUE)
			{
				PROTECT(ans_rv);
				SEXP dim = getAttrib(ans_rv, R_DimSymbol);
				if (!Rf_isNull(dim))
				{
					int Num_GreaterOne = 0;
					for (int i=0; i < XLENGTH(dim); i++)
					{
						if (INTEGER(dim)[i] > 1)
							Num_GreaterOne ++;
					}
					if (Num_GreaterOne <= 1)
						setAttrib(ans_rv, R_DimSymbol, R_NilValue);
				}
				UNPROTECT(1);
			}

			return ans_rv;
		} else
			throw ErrGDSFmt("There is no data field.");

	CORECATCH_ERROR
}


/// write data to a node
/** \param Node        [in] a specified GDS node
 *  \param Val         [in] the input values
**/
DLLEXPORT SEXP gdsObjWriteAll(SEXP Node, SEXP Val, SEXP Check)
{
	CdGDSObj *n = _NodeValidSEXP(Node);

	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val))
		error("`val' should be integer, numeric, character or logical.");

	if (!Rf_isLogical(Check) || (XLENGTH(Check) <= 0))
		error("`check' should be a logical variable.");

	CORETRY
		if (dynamic_cast<CdSequenceX*>(n))
		{
			int nProtected = 0;
			CdSequenceX *Obj = static_cast<CdSequenceX*>(n);
			TSVType ObjSV = Obj->SVType();

			if (COREARRAY_SV_INTEGER(ObjSV))
			{
				PROTECT(Val = Rf_coerceVector(Val, INTSXP));
				nProtected ++;
			} else if (COREARRAY_SV_FLOAT(ObjSV))
			{
				PROTECT(Val = Rf_coerceVector(Val, REALSXP));
				nProtected ++;
			} else if (COREARRAY_SV_STRING(ObjSV))
			{
				PROTECT(Val = Rf_coerceVector(Val, STRSXP));
				nProtected ++;
			} else
				throw ErrGDSFmt("Not support!");

			int DDimCnt;
			CdSequenceX::TSeqDimBuf DDim;
			SEXP dim = getAttrib(Val, R_DimSymbol);
			if (Rf_isNull(dim))
			{
				const int MAX_INT = 2147483647;
				DDimCnt = 1;
				if (XLENGTH(Val) > MAX_INT)
				{
					throw ErrGDSFmt(
						"The length of `val' should not be greater than %d.",
						MAX_INT);
				}
				DDim[0] = XLENGTH(Val);
			} else {
				DDimCnt = XLENGTH(dim);
				for (int i=0; i < DDimCnt; i++)
					DDim[i] = INTEGER(dim)[DDimCnt-i-1];
			}

			Obj->ClearDim();
			for (int i=0; i < DDimCnt; i++)
				Obj->AddDim(0);
			for (int i=DDimCnt-1; i > 0; i--)
				Obj->SetDLen(i, DDim[i]);

			if (COREARRAY_SV_INTEGER(ObjSV))
			{
				Obj->Append(INTEGER(Val), XLENGTH(Val), svInt32);
			} else if (COREARRAY_SV_FLOAT(ObjSV))
			{
				Obj->Append(REAL(Val), XLENGTH(Val), svFloat64);
			} else if (COREARRAY_SV_STRING(ObjSV))
			{
				R_xlen_t Len = XLENGTH(Val);
				if (LOGICAL(Check)[0] == TRUE)
				{
					for (R_xlen_t i=0; i < Len; i++)
					{
						if (STRING_ELT(Val, i) == NA_STRING)
						{
							warning("Missing characters are converted to \"\".");
							break;
						}
					}
				}
				vector<string> buf(Len);
				for (R_xlen_t i=0; i < Len; i++)
				{
					SEXP s = STRING_ELT(Val, i);
					if (s != NA_STRING) buf[i] = CHAR(s);
				}
				Obj->Append(&(buf[0]), Len, svStrUTF8);
			} else
				throw ErrGDSFmt("Not support!");

			UNPROTECT(nProtected);

		} else
			throw ErrGDSFmt("The GDS node does not support 'write.gdsn'!");
	}
	catch (ErrAllocWrite &E) {
		Init.LastError = erReadOnly;
		error(Init.LastError.c_str());
	}
	catch (exception &E) {
		Init.LastError = E.what();
		error(Init.LastError.c_str());
	}
	catch (const char *E) {
		Init.LastError = E;
		error(Init.LastError.c_str());
	}
	return R_NilValue;
}


/// write data to a node
/** \param Node        [in] a specified GDS node
 *  \param Val         [in] the values
 *  \param Start       [in] the starting positions
 *  \param Count       [in] the count
 *  \param Check       [in] if TRUE, check any(is.na(val)) if val is character
**/
DLLEXPORT SEXP gdsObjWriteData(SEXP Node, SEXP Val,
	SEXP Start, SEXP Count, SEXP Check)
{
	CdGDSObj *n = _NodeValidSEXP(Node);

	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val))
		error("`val' should be integer, numeric, character or logical.");

	if (!Rf_isLogical(Check) || (XLENGTH(Check) <= 0))
		error("`check' should be a logical variable.");

	if (!Rf_isNull(Start) && !Rf_isNumeric(Start))
		error("`start' should be numeric.");
	if (!Rf_isNull(Count) && !Rf_isNumeric(Count))
		error("`count' should be numeric.");
	if ((Rf_isNull(Start) && !Rf_isNull(Count)) ||
			(!Rf_isNull(Start) && Rf_isNull(Count)))
		error("`start' and 'count' should be both NULL.");

	if (!dynamic_cast<CdSequenceX*>(n))
		error("There is no data field.");
	CdSequenceX *Obj = static_cast<CdSequenceX*>(n);

	CdSequenceX::TSeqDimBuf DStart, DLen;
	if (!Rf_isNull(Start) && !Rf_isNull(Count))
	{
		int Len = Obj->DimCnt();
		CdSequenceX::TSeqDimBuf DCnt;
		Obj->GetDimLen(DCnt);

		PROTECT(Start = Rf_coerceVector(Start, INTSXP));
		if (XLENGTH(Start) != Len)
			error("The length of `start' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Start)[i];
			if ((v < 1) || (v > DCnt[Len-i-1]))
				error("`start' is invalid.");
			DStart[Len-i-1] = v-1;
		}

		PROTECT(Count = Rf_coerceVector(Count, INTSXP));
		if (XLENGTH(Count) != Len)
			error("The length of `count' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Count)[i];
			if (v == -1) v = DCnt[Len-i-1];
			if ((v < 0) || ((DStart[Len-i-1]+v) > DCnt[Len-i-1]))
				error("`count' is invalid.");
			DLen[Len-i-1] = v;
		}

		UNPROTECT(2);
	}

	CORETRY
		int nProtected = 0;

		TSVType ObjSV = Obj->SVType();
		if (COREARRAY_SV_INTEGER(ObjSV))
		{
			PROTECT(Val = Rf_coerceVector(Val, INTSXP));
			nProtected ++;
			Obj->wData(DStart, DLen, INTEGER(Val), svInt32);
		} else if (COREARRAY_SV_FLOAT(ObjSV))
		{
			PROTECT(Val = Rf_coerceVector(Val, REALSXP));
			nProtected ++;
			Obj->wData(DStart, DLen, REAL(Val), svFloat64);
		} else if (COREARRAY_SV_STRING(ObjSV))
		{
			PROTECT(Val = Rf_coerceVector(Val, STRSXP));
			nProtected ++;
			R_xlen_t Len = XLENGTH(Val);
			if (LOGICAL(Check)[0] == TRUE)
			{
				for (R_xlen_t i=0; i < Len; i++)
				{
					if (STRING_ELT(Val, i) == NA_STRING)
					{
						warning("Missing characters are converted to \"\".");
						break;
					}
				}
			}
			vector<string> buf(Len);
			for (R_xlen_t i=0; i < Len; i++)
			{
				SEXP s = STRING_ELT(Val, i);
				if (s != NA_STRING) buf[i] = CHAR(s);
			}
			Obj->wData(DStart, DLen, &(buf[0]), svStrUTF8);
		} else
			throw ErrGDSFmt("Not support!");

		UNPROTECT(nProtected);
	}
	catch (ErrAllocWrite &E) {
		Init.LastError = erReadOnly;
		error(Init.LastError.c_str());
	}
	catch (exception &E) {
		Init.LastError = E.what();
		error(Init.LastError.c_str());
	}
	catch (const char *E) {
		Init.LastError = E;
		error(Init.LastError.c_str());
	}
	return R_NilValue;
}


/// assign data to a GDS variable
/** \param dest_obj    [in] a specified GDS node
 *  \param src_obj     [in] the number of dimension
 *  \param append      [in] the starting positions
**/
DLLEXPORT SEXP gdsAssign(SEXP dest_obj, SEXP src_obj, SEXP append)
{
	CdGDSObj *Dst = _NodeValidSEXP(dest_obj);
	CdGDSObj *Src = _NodeValidSEXP(src_obj);

	CORETRY
		// assign
		if (dynamic_cast<CdContainer*>(Dst))
		{
			static_cast<CdContainer*>(Dst)->AssignOne(*Src,
				LOGICAL(append)[0]==TRUE);
		} else {
			Dst->AssignOne(*Src);
		}
	CORECATCH_ERROR
}


/// set the dimension of data to a node
/** \param Node        [in] a specified GDS node
 *  \param DLen        [in] the new sizes of dimension
**/
DLLEXPORT SEXP gdsObjSetDim(SEXP Node, SEXP DLen)
{
	CdGDSObj *n = _NodeValidSEXP(Node);

	CORETRY
		PROTECT(DLen = Rf_coerceVector(DLen, INTSXP));

		if (dynamic_cast<CdSequenceX*>(n))
		{
			CdSequenceX *Obj = static_cast<CdSequenceX*>(n);
			if (XLENGTH(DLen) < Obj->DimCnt())
			{
				throw ErrGDSFmt("New dimension should not be less than the currect.");
			} if (XLENGTH(DLen) > CdSequenceX::MAX_SEQ_DIM)
			{
				throw ErrGDSFmt(
					"The total number of dimensions should not be greater than %d.",
					CdSequenceX::MAX_SEQ_DIM);
			} else {
				for (int i=Obj->DimCnt()+1; i <= XLENGTH(DLen); i++)
					Obj->AddDim(-1);
				for (int i=XLENGTH(DLen)-1; i >= 0; i--)
					Obj->SetDLen(i, INTEGER(DLen)[XLENGTH(DLen)-i-1]);
			}
		} else
			throw ErrGDSFmt("Not support!");

		UNPROTECT(1);

	CORECATCH_ERROR
}


/// set a new compression mode
/** \param Node        [in] a specified GDS node
 *  \param Compress    [in] the compression mode
**/
DLLEXPORT SEXP gdsObjCompress(SEXP Node, SEXP Compress)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	const char *cp = CHAR(STRING_ELT(Compress, 0));
	CORETRY
		if (dynamic_cast<CdContainer*>(n))
		{
			static_cast<CdContainer*>(n)->SetPackedMode(cp);
		} else
			throw ErrGDSFmt("Not allow compression / decompression.");
	CORECATCH_ERROR
}


/// close the compression mode if possible
/** \param Node        [in] a specified GDS node
**/
DLLEXPORT SEXP gdsObjCompressClose(SEXP Node)
{
	CdGDSObj *n = _NodeValidSEXP(Node);
	CORETRY
		if (dynamic_cast<CdContainer*>(n))
		{
			static_cast<CdContainer*>(n)->CloseWriter();
		}
	CORECATCH_ERROR
}


/// get the last error message
DLLEXPORT SEXP gdsLastErrGDS()
{
	SEXP ans_rv;
	PROTECT(ans_rv = NEW_STRING(1));
	SET_STRING_ELT(ans_rv, 0, mkChar(Init.LastError.c_str()));
	Init.LastError.clear();
	UNPROTECT(1);
	return ans_rv;
}


/// get number of bytes and bits
/** \param ClassName   [in] the name of class
 *  \param out_nbit    [out] the number of bits
 *  \param err         [out] return TRUE if error occurs, otherwise FALSE
**/
DLLEXPORT void gds_Internal_Class(char **ClassName, int *out_nbit, int *err)
{
	CORETRY
		Init.CheckInit();

		*out_nbit = -1;

		// Class Name Mapping
		const char *nName;
		map<const char*, const char*, TInit::strCmp>::iterator it;
		it = Init.ClassMap.find(*ClassName);
		if (it != Init.ClassMap.end())
			nName = it->second;
		else
			throw ErrGDSFmt(string("Not support: ") + *ClassName);

		// mapping
		CdObjClassMgr::TdOnObjCreate OnCreate = dObjManager().NameToClass(nName);
		if (OnCreate)
		{
			CdObject *obj = OnCreate();
			if (dynamic_cast<CdContainer*>(obj))
			{
				*out_nbit = static_cast<CdContainer*>(obj)->BitOf();
			}
			delete obj;
		} else
			throw ErrGDSFmt(string("Not support: ") + *ClassName);

		*err = false;
	CORECATCH(*err = true)
}




// **********************************************************************
// **********************************************************************
// **********************************************************************

/// called by 'apply.gdsn'
/** \param gds_nodes   [in] a list of objects of 'gdsn' class
 *  \param margins     [in] margin indices starting from 1
 *  \param FUN         [in] a function applied marginally
 *  \param selection   [in] indicates selection
 *  \param as_is       [in] the type of returned value
 *  \param rho         [in] the environment variable
**/
DLLEXPORT SEXP gds_apply_call(SEXP gds_nodes, SEXP margins,
	SEXP FUN, SEXP selection, SEXP as_is, SEXP rho)
{
	bool has_error = false;
	SEXP rv_ans = R_NilValue;

	CORETRY

		// the total number of objects
		int nObject = Rf_length(gds_nodes);


		// ***********************************************************
		// gds_nodes, a list of data variables

		vector< CdSequenceX* > ObjList(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			CdGDSObj *Node = _NodeValidSEXP(VECTOR_ELT(gds_nodes, i));
			if (dynamic_cast<CdSequenceX*>(Node))
			{
				ObjList[i] = static_cast<CdSequenceX*>(Node);
			} else {
				throw ErrGDSFmt("'nodes[[%d]]' should be array-based!", i+1);
			}
		}


		// ***********************************************************
		// get information

		vector<int> DimCnt(nObject);
		vector< vector<CoreArray::Int32> > DLen(nObject);
		vector<TSVType> SVType(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			SVType[i] = ObjList[i]->SVType();
			if (!(COREARRAY_SV_INTEGER(SVType[i]) || COREARRAY_SV_FLOAT(SVType[i])))
				throw ErrGDSFmt("Only support numeric data currently.");	
			DimCnt[i] = ObjList[i]->DimCnt();
			DLen[i].resize(DimCnt[i]);
			ObjList[i]->GetDimLen(&(DLen[i][0]));
		}


		// ***********************************************************
		// margin should be integer

		vector<int> Margin(nObject);
		for (int i=0; i < nObject; i++)
		{
			Margin[i] = DimCnt[i] - INTEGER(margins)[i];
			if ((Margin[i] < 0) || (Margin[i] >= DimCnt[i]))
				throw ErrGDSFmt("'margin' is not valid according to 'nodes[[i]]'!", i+1);
		}


		// ***********************************************************
		// function
		if(!isFunction(FUN))
			throw ErrGDSFmt("'FUN' must be a function.");


		// ***********************************************************
		// selection

		vector< vector<CBOOL *> > array_sel(nObject);
		vector< vector< vector<CBOOL> > > sel_list(nObject);
		vector<CBOOL **> sel_ptr(nObject);

		if (!isNull(selection))
		{
			if (Rf_length(selection) != nObject)
				throw ErrGDSFmt("length(selection) should be equal to length(nodes).");

			for (int i=0; i < nObject; i++)
			{			
				SEXP SelItem = VECTOR_ELT(selection, i);

				if (!isNull(SelItem))
				{
					array_sel[i].resize(DimCnt[i]);
					sel_ptr[i] = &(array_sel[i][0]);
					sel_list[i].resize(DimCnt[i]);

					for (int j=0; j < DimCnt[i]; j++)
					{
						sel_list[i][j].resize(DLen[i][j]);
						array_sel[i][j] = &(sel_list[i][j][0]);
					}

					// check
					if (!isNewList(SelItem))
						throw ErrGDSFmt("'selection[[%d]]' must be NULL or a list.", i+1);
					if (Rf_length(SelItem) != DimCnt[i])
						throw ErrGDSFmt("length(selection[[%d]]) is not valid!", i+1);

					SEXP elmt = R_NilValue;
					for (R_len_t k = 0; k < Rf_length(SelItem); k ++)
					{
						int DimIdx = DimCnt[i] - k - 1;
						elmt = VECTOR_ELT(SelItem, k);
						if (isNull(elmt))
						{
							for (int j=0; j < DLen[i][DimIdx]; j++)
								array_sel[i][DimIdx][j] = true;
						} else if (isLogical(elmt))
						{
							if (Rf_length(elmt) != DLen[i][DimIdx])
								throw ErrGDSFmt("Invalid length of selection[[%d]][%d]!", i+1, k+1);
							int *p = LOGICAL(elmt);
							for (int j=0; j < DLen[i][DimIdx]; j++)
								array_sel[i][DimIdx][j] = (p[j]==TRUE);
						} else {
							throw ErrGDSFmt("The element of 'selection[[%d]]' should be a logical variable or NULL.",
								i + 1);
						}
					}
				} else {
					sel_ptr[i] = NULL;
				}
			}
		} else {
			// no selection
			for (int i=0; i < nObject; i++)
			{
				sel_ptr[i] = NULL;
			}
		}


		// ***********************************************************
		// as.is
		//     0: integer, 1: double, 2: character, 3: list, other: NULL
		int DatType;
		const char *as = CHAR(STRING_ELT(as_is, 0));
		if (strcmp(as, "integer") == 0)
			DatType = 0;
		else if (strcmp(as, "double") == 0)
			DatType = 1;
		else if (strcmp(as, "character") == 0)
			DatType = 2;
		else if (strcmp(as, "list") == 0)
			DatType = 3;
		else if (strcmp(as, "none") == 0)
			DatType = -1;
		else
			throw ErrGDSFmt("'as.is' is not valid!");


		// ***********************************************************
		// rho
		if (!isEnvironment(rho))
			throw ErrGDSFmt("'rho' should be an environment");


		// *************************************************************
		// initialize variables

		// array read object
		vector<CdArrayRead> Array(nObject);
		for (int i=0; i < nObject; i++)
		{
			if (COREARRAY_SV_INTEGER(SVType[i]))
				Array[i].Init(*ObjList[i], Margin[i], svInt32, sel_ptr[i], false);
			else if (COREARRAY_SV_FLOAT(SVType[i]))
				Array[i].Init(*ObjList[i], Margin[i], svFloat64, sel_ptr[i], false);
			else
				throw ErrGDSFmt("not support in apply.gdsn");
		}

		// check the margin
		CoreArray::Int32 MCnt = Array[0].Count();
		for (int i=1; i < nObject; i++)
		{
			if (Array[i].Count() != MCnt)
				throw ErrGDSFmt("nodes[[%d]] should have the same number of elements as nodes[[1]] marginally (margins[%d] = margins[1]).", i+1, i+1);
		}

		// allocate internal buffer uniformly
		Balance_ArrayRead_Buffer(&(Array[0]), Array.size());


		// used in UNPROTECT
		int nProtected = 0;

		// initialize param in FUN
		vector<void *> BufPtr(nObject);
		SEXP R_call_param = R_NilValue;
		SEXP tmp = R_NilValue;

		if (nObject > 1)
		{
			PROTECT(R_call_param = NEW_LIST(nObject));
			nProtected ++;
		}

		for (int i=0; i < nObject; i++)
		{
			if (COREARRAY_SV_INTEGER(SVType[i]))
			{
				if (gds_Is_R_Logical(*ObjList[i]))
				{
					PROTECT(tmp = NEW_LOGICAL(Array[i].MarginCount()));
				} else {
					PROTECT(tmp = NEW_INTEGER(Array[i].MarginCount()));
					nProtected += gds_Set_If_R_Factor(*ObjList[i], tmp);
				}
				nProtected ++;
				BufPtr[i] = INTEGER(tmp);
			} else if (COREARRAY_SV_FLOAT(SVType[i]))
			{
				PROTECT(tmp = NEW_NUMERIC(Array[i].MarginCount()));
				nProtected ++;
				BufPtr[i] = REAL(tmp);
			} else {
				tmp = R_NilValue;
				BufPtr[i] = NULL;
			}

			// init dim
			if (DimCnt[i] > 2)
			{
				SEXP dim;
				PROTECT(dim = NEW_INTEGER(DimCnt[i] - 1));
				nProtected ++;
				int I = 0;
				for (int k=DimCnt[i]-1; k >= 0; k--)
				{
					if (k != Margin[i])
						INTEGER(dim)[ I++ ] = Array[i].DimCntValid()[k];
				}
				SET_DIM(tmp, dim);
			}

			if (nObject > 1)
				SET_VECTOR_ELT(R_call_param, i, tmp);
		}

		if (nObject > 1)
		{
			// set name to R_call_param
			SET_NAMES(R_call_param, GET_NAMES(gds_nodes));
		} else {
			R_call_param = tmp;
		}


		// init return values
		// int DatType;  //< 0: integer, 1: double, 2: character, 3: list, other: NULL
		switch (DatType)
		{
		case 0:
			PROTECT(rv_ans = NEW_INTEGER(Array[0].Count()));
			nProtected ++;
			break;
		case 1:
			PROTECT(rv_ans = NEW_NUMERIC(Array[0].Count()));
			nProtected ++;
			break;
		case 2:
			PROTECT(rv_ans = NEW_CHARACTER(Array[0].Count()));
			nProtected ++;
			break;
		case 3:
			PROTECT(rv_ans = NEW_LIST(Array[0].Count()));
			nProtected ++;
			break;
		default:
			rv_ans = R_NilValue;
		}

		// init call
		SEXP R_fcall;
		PROTECT(R_fcall = LCONS(FUN,
			LCONS(R_call_param, LCONS(R_DotsSymbol, R_NilValue))));
		nProtected ++;

		SEXP val;
		int idx = 0;

		// for - loop
		while (!Array[0].Eof())
		{
			// read marginally
			for (int i=0; i < nObject; i++)
				Array[i].Read(BufPtr[i]);

			// call R function
			SEXP val = eval(R_fcall, rho);

			switch (DatType)
			{
			case 0:    // integer
				INTEGER(rv_ans)[idx] = INTEGER(AS_INTEGER(val))[0];
				break;
			case 1:    // double
				REAL(rv_ans)[idx] = REAL(AS_NUMERIC(val))[0];
				break;
			case 2:    // character
				SET_STRING_ELT(rv_ans, idx, STRING_ELT(AS_CHARACTER(val), 0));
				break;
			case 3:    // others
				if (NAMED(val) > 0)
				{
					// the object is bound to other symbol(s), need a copy
					val = duplicate(val);
				}
				SET_VECTOR_ELT(rv_ans, idx, val);
				break;
			}

			idx ++;
		}

		//
		UNPROTECT(nProtected);
	}
	catch (ErrAllocRead &E) {
		Init.LastError = erWriteOnly;
		has_error = true;
	}
	catch (exception &E) {
		Init.LastError = E.what();
		has_error = true;
	}
	catch (const char *E) {
		Init.LastError = E;
		has_error = true;
	}
	if (has_error)
		error(Init.LastError.c_str());

	return(rv_ans);
}


/// called by 'clusterApply.gdsn', return a selection (list)
/** \param gds_nodes   [in] a list of objects of 'gdsn' class
 *  \param margins     [in] margin indices starting from 1
 *  \param selection   [in] indicates selection
**/
DLLEXPORT SEXP gds_apply_create_selection(SEXP gds_nodes, SEXP margins,
	SEXP selection)
{
	bool has_error = false;
	SEXP rv_ans = R_NilValue;

	CORETRY

		// the total number of objects
		int nObject = Rf_length(gds_nodes);
		// used in UNPROTECT
		int nProtected = 0;

		// initialize returned value
		PROTECT(rv_ans = NEW_LIST(nObject));
		nProtected ++;


		// ***********************************************************
		// gds_nodes, a list of data variables

		vector< CdSequenceX* > ObjList(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			CdGDSObj *Node = _NodeValidSEXP(VECTOR_ELT(gds_nodes, i));
			if (dynamic_cast<CdSequenceX*>(Node))
			{
				ObjList[i] = static_cast<CdSequenceX*>(Node);
			} else {
				throw ErrGDSFmt("'node.names[[%d]]' should be array-based!", i+1);
			}
		}


		// ***********************************************************
		// get information

		vector<int> DimCnt(nObject);
		vector< vector<CoreArray::Int32> > DLen(nObject);
		vector<TSVType> SVType(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			SVType[i] = ObjList[i]->SVType();
			if (!(COREARRAY_SV_INTEGER(SVType[i]) || COREARRAY_SV_FLOAT(SVType[i])))
				throw ErrGDSFmt("Only support numeric data currently.");	
			DimCnt[i] = ObjList[i]->DimCnt();
			DLen[i].resize(DimCnt[i]);
			ObjList[i]->GetDimLen(&(DLen[i][0]));
		}


		// ***********************************************************
		// margin should be integer

		vector<int> Margin(nObject);
		for (int i=0; i < nObject; i++)
		{
			Margin[i] = DimCnt[i] - INTEGER(margins)[i];
			if ((Margin[i] < 0) || (Margin[i] >= DimCnt[i]))
				throw ErrGDSFmt("'margin' is not valid according to 'node.names[[i]]'!", i+1);
		}


		// ***********************************************************
		// selection

		vector< vector<CBOOL *> > array_sel(nObject);
		vector< vector< vector<CBOOL> > > sel_list(nObject);
		vector<CBOOL **> sel_ptr(nObject);

		if (!isNull(selection))
		{
			if (Rf_length(selection) != nObject)
				throw ErrGDSFmt("length(selection) should be equal to length(node).");

			for (int i=0; i < nObject; i++)
			{			
				SEXP SelItem = VECTOR_ELT(selection, i);

				if (!isNull(SelItem))
				{
					array_sel[i].resize(DimCnt[i]);
					sel_ptr[i] = &(array_sel[i][0]);
					sel_list[i].resize(DimCnt[i]);

					for (int j=0; j < DimCnt[i]; j++)
					{
						sel_list[i][j].resize(DLen[i][j]);
						array_sel[i][j] = &(sel_list[i][j][0]);
					}

					// check
					if (!isNewList(SelItem))
						throw ErrGDSFmt("'selection[[%d]]' must be NULL or a list.", i+1);
					if (Rf_length(SelItem) != DimCnt[i])
						throw ErrGDSFmt("length(selection[[%d]]) is not valid!", i+1);

					SEXP elmt = R_NilValue;
					for (R_len_t k = 0; k < Rf_length(SelItem); k ++)
					{
						int DimIdx = DimCnt[i] - k - 1;
						elmt = VECTOR_ELT(SelItem, k);
						if (isNull(elmt))
						{
							for (int j=0; j < DLen[i][DimIdx]; j++)
								array_sel[i][DimIdx][j] = true;
						} else if (isLogical(elmt))
						{
							if (Rf_length(elmt) != DLen[i][DimIdx])
								throw ErrGDSFmt("Invalid length of selection[[%d]][%d]!", i+1, k+1);
							int *p = LOGICAL(elmt);
							for (int j=0; j < DLen[i][DimIdx]; j++)
								array_sel[i][DimIdx][j] = (p[j]==TRUE);
						} else {
							throw ErrGDSFmt("The element of 'selection[[%d]]' should be a logical variable or NULL.",
								i + 1);
						}
					}
				} else {
					sel_ptr[i] = NULL;
				}
			}
		} else {
			// no selection
			for (int i=0; i < nObject; i++)
			{
				sel_ptr[i] = NULL;
				sel_list[i].resize(DimCnt[i]);
				for (int j=0; j < DimCnt[i]; j++)
					sel_list[i][j].resize(DLen[i][j], 1);  // 1 -- TRUE
			}
		}


		// *************************************************************
		// initialize variables

		// array read object
		vector<CdArrayRead> Array(nObject);
		for (int i=0; i < nObject; i++)
		{
			// do not allocate memory
			if (COREARRAY_SV_INTEGER(SVType[i]))
				Array[i].Init(*ObjList[i], Margin[i], svInt32, sel_ptr[i], false);
			else if (COREARRAY_SV_FLOAT(SVType[i]))
				Array[i].Init(*ObjList[i], Margin[i], svFloat64, sel_ptr[i], false);
			else
				throw ErrGDSFmt("not support in apply.gdsn");
		}

		// check the margin
		CoreArray::Int32 MCnt = Array[0].Count();
		for (int i=1; i < nObject; i++)
		{
			if (Array[i].Count() != MCnt)
			{
				throw ErrGDSFmt(
				"node[[%d]] should have the same number of elements as node[[1]] marginally (margin[%d] = margin[1]).",
				i+1, i+1);
			}
		}


		// *************************************************************
		// fill returned value
		for (int i=0; i < nObject; i++)
		{
			SEXP val, tmp;
			PROTECT(val = NEW_LIST(DimCnt[i]));
			nProtected ++;
			SET_VECTOR_ELT(rv_ans, i, val);

			for (int j=0; j < DimCnt[i]; j++)
			{
				PROTECT(tmp = NEW_LOGICAL(DLen[i][j]));
				nProtected ++;
				SET_VECTOR_ELT(val, DimCnt[i] - j - 1, tmp);

				for (int k=0; k < DLen[i][j]; k++)
					LOGICAL(tmp)[k] = sel_list[i][j][k];
			}
		}


		//
		UNPROTECT(nProtected);

	CORECATCH(has_error=true);
	if (has_error)
		error(Init.LastError.c_str());

	return(rv_ans);
}




// **********************************************************************
// **********************************************************************
// **********************************************************************


} // extern "C"
