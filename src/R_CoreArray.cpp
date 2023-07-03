// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// R_CoreArray.cpp: Export the C routines of CoreArray library
//
// Copyright (C) 2014-2022    Xiuwen Zheng
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

/**
 *	\file     R_CoreArray.cpp
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2014 - 2022
 *	\brief    Export the C routines of CoreArray library
 *	\details
**/

#define COREARRAY_GDSFMT_PACKAGE

#include <R_GDS_CPP.h>
#include <cstring>
#include <R_ext/Rdynload.h>
#include <vector>
#include <map>
#include <set>


namespace gdsfmt
{
	using namespace std;

	/// the number of preserved bytes for a pointer to a GDS object
	#define GDSFMT_NUM_BYTE_FOR_GDSOBJ    (4 + 16)


	/// a list of GDS files in the gdsfmt package
	COREARRAY_DLL_LOCAL PdGDSFile PKG_GDS_Files[GDSFMT_MAX_NUM_GDS_FILES];

	/// get the index in 'PKG_GDS_Files' for NULL
	COREARRAY_DLL_LOCAL int GetEmptyFileIndex(bool throw_error=true)
	{
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
		{
			if (PKG_GDS_Files[i] == NULL)
				return i;
		}
		if (throw_error)
		{
			throw ErrGDSFmt(
				"You have opened %d GDS files, and no more is allowed!",
				GDSFMT_MAX_NUM_GDS_FILES);
		}
		return -1;
	}

	/// get the index in 'PKG_GDS_Files' for file
	COREARRAY_DLL_LOCAL int GetFileIndex(PdGDSFile file, bool throw_error=true)
	{
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
		{
			if (PKG_GDS_Files[i] == file)
				return i;
		}
		if (throw_error)
			throw ErrGDSFmt("The GDS file is closed or uninitialized.");
		return -1;
	}


	/// a list of GDS objects
	COREARRAY_DLL_LOCAL vector<PdGDSObj> GDSFMT_GDSObj_List;

	/// mapping from GDS objects to indices
	COREARRAY_DLL_LOCAL map<PdGDSObj, int> GDSFMT_GDSObj_Map;


	/// initialization and finalization
	class COREARRAY_DLL_LOCAL CInitObject
	{
	public:
		/// initialization
		CInitObject()
		{
			memset(PKG_GDS_Files, 0, sizeof(PKG_GDS_Files));
			GDSFMT_GDSObj_List.reserve(1024);
		}

		/// finalization
		~CInitObject()
		{
			GDSFMT_GDSObj_List.clear();
			GDSFMT_GDSObj_Map.clear();

			for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
			{
				PdGDSFile file = PKG_GDS_Files[i];
				if (file != NULL)
				{
					try {
						PKG_GDS_Files[i] = NULL;
						delete file;
					}
					catch (...) { }
				}
			}
		}
	};

	static CInitObject InitObject;
}

using namespace std;
using namespace CoreArray;
using namespace CoreArray::Parallel;
using namespace gdsfmt;


extern "C"
{

// ===========================================================================
// Internal

static bool flag_init_Matrix = false;


// predefined strings
static const char *ERR_WRITE_ONLY =
	"Writable only and please call 'readmode.gdsn()' before reading.";
static const UTF8String STR_LOGICAL = "R.logical";
static const UTF8String STR_CLASS = "R.class";
static const UTF8String STR_LEVELS = "R.levels";
static const UTF8String STR_FACTOR = "factor";


/// get the list element named str, or return NULL
COREARRAY_INLINE static SEXP GetListElement(SEXP list, const char *str)
{
	SEXP elmt = R_NilValue;
	SEXP names = getAttrib(list, R_NamesSymbol);
	R_len_t n = Rf_isNull(names) ? 0 : XLENGTH(list);
	for (R_len_t i = 0; i < n; i++)
	{
		if (strcmp(CHAR(STRING_ELT(names, i)), str) == 0)
		{
			elmt = VECTOR_ELT(list, i);
			break;
		}
	}
	return elmt;
}

/// get the index from a list
COREARRAY_INLINE static int GetIndexList(SEXP list, const char *str)
{
	SEXP names = getAttrib(list, R_NamesSymbol);
	R_len_t n = Rf_isNull(names) ? 0 : XLENGTH(list);
	for (R_len_t i = 0; i < n; i++)
	{
		if (strcmp(CHAR(STRING_ELT(names, i)), str) == 0)
			return i;
	}
	return -1;
}

static bool gds_verbose()
{
	int v = asLogical(GetOption1(install("gds.verbose")));
	return v == TRUE;
}

static void gdsfile_free(SEXP ptr_obj)
{
	// pointer
	void *ptr = R_ExternalPtrAddr(ptr_obj);
	if (!ptr) return;
	R_ClearExternalPtr(ptr_obj);
	// file ID
	SEXP ID = R_ExternalPtrProtected(ptr_obj);
	int i = Rf_asInteger(ID);
	if (TYPEOF(ID)==INTSXP && Rf_length(ID)>=1)
		INTEGER(ID)[0] = -1;
	if (0 <= i && i < GDSFMT_MAX_NUM_GDS_FILES)
	{
		void *save_ptr = PKG_GDS_Files[i];
		if (save_ptr != ptr) return;
		// verbose
		CdGDSFile *file = (CdGDSFile*)ptr;
		if (gds_verbose())
		{
			UTF8String fn = file->FileName();
			Rprintf("Close '%s'.\n", fn.c_str());
		}
		// close
		bool has_error = false;
		CORE_TRY
			GDS_File_Close(file);
		CORE_CATCH(has_error = true);
		if (has_error) error(GDS_GetError());
	}
}

COREARRAY_DLL_LOCAL SEXP new_gdsptr_obj(CdGDSFile *file, SEXP id, bool do_free)
{
	SEXP rv = R_MakeExternalPtr(file, R_NilValue, id);
	if (do_free)
	{
		PROTECT(rv);
		R_RegisterCFinalizerEx(rv, gdsfile_free, (Rboolean)FALSE);
		UNPROTECT(1);
	}
	return rv;
}


// ===========================================================================
/// check the validity of R SEXP
COREARRAY_INLINE static PdGDSObj CheckSEXPObject(SEXP Obj, bool Full)
{
	static const char *ERR_GDS_OBJ  =
		"Invalid GDS node object!";
	static const char *ERR_GDS_OBJ2 =
		"Invalid GDS node object (it was unloaded or deleted).";

	// check class
	SEXP Class = GET_CLASS(Obj);
	if (TYPEOF(Class) != STRSXP)
		throw ErrGDSFmt(ERR_GDS_OBJ);
	R_xlen_t i, n = XLENGTH(Class);
	for (i=0; i < n; i++)
	{
		if (strcmp(CHAR(STRING_ELT(Class, i)), "gdsn.class") == 0)
			break;
	}
	if (i >= n)
		throw ErrGDSFmt(ERR_GDS_OBJ);

	// check data type
	if (TYPEOF(Obj) != RAWSXP)
		throw ErrGDSFmt(ERR_GDS_OBJ);
	if (XLENGTH(Obj) != GDSFMT_NUM_BYTE_FOR_GDSOBJ)
		throw ErrGDSFmt(ERR_GDS_OBJ);

	Rbyte *p = RAW(Obj);
	int idx;
	PdGDSObj rv;
	memcpy(&idx, p            , sizeof(int));
	memcpy(&rv,  p+sizeof(int), sizeof(PdGDSObj));

	if (Full)
	{
		// check
		if ((idx < 0) || (idx >= (int)GDSFMT_GDSObj_List.size()))
			throw ErrGDSFmt(ERR_GDS_OBJ);
		if (rv == NULL)
			throw ErrGDSFmt(ERR_GDS_OBJ);
		if (GDSFMT_GDSObj_List[idx] != rv)
			throw ErrGDSFmt(ERR_GDS_OBJ2);
	}

	return rv;
}


// ===========================================================================
// R objects

/// convert "SEXP  --> (CdGDSFile*)"
COREARRAY_DLL_EXPORT PdGDSFile GDS_R_SEXP2File(SEXP File)
{
	// to register CoreArray classes and objects
	RegisterClass();

	SEXP gds_id  = GetListElement(File, "id");
	if (Rf_isNull(gds_id) || !Rf_isInteger(gds_id))
		throw ErrGDSFmt("The GDS object is invalid (wrong id).");

	SEXP gds_ptr = GetListElement(File, "ptr");
	if (TYPEOF(gds_ptr) != EXTPTRSXP)
		throw ErrGDSFmt("The GDS object is invalid (wrong ptr).");

	int id = Rf_asInteger(gds_id);
	if (id < 0)
		throw ErrGDSFmt("The GDS file is closed or uninitialized.");
	if (id >= GDSFMT_MAX_NUM_GDS_FILES)
		throw ErrGDSFmt("The GDS ID (%d) is invalid.", id);

	PdGDSFile file = PKG_GDS_Files[id];
	void *ptr = R_ExternalPtrAddr(gds_ptr);
	if (!file)
		throw ErrGDSFmt("The GDS file is closed or uninitialized.");
	if (ptr != file)
	{
		if (ptr)
			throw ErrGDSFmt("Invalid 'ptr' in the GDS object.");
		// else
		//	throw ErrGDSFmt("The GDS object is uninitialized.");
	}

	return file;
}

/// convert "SEXP  --> (CdGDSFolder*)"
COREARRAY_DLL_EXPORT PdGDSFolder GDS_R_SEXP2FileRoot(SEXP File)
{
	PdGDSFile file = GDS_R_SEXP2File(File);
	return &(file->Root());
}

/// convert "SEXP  -->  (CdGDSObj*)"
COREARRAY_DLL_EXPORT PdGDSObj GDS_R_SEXP2Obj(SEXP Obj, C_BOOL ReadOnly)
{
	PdGDSObj vObj = CheckSEXPObject(Obj, true);
	CdGDSFile *file = vObj->GDSFile();
	if (!file) return vObj;

	if (file->ReadOnly() && !ReadOnly)
		throw ErrGDSFmt("The GDS file is read-only.");
#ifdef COREARRAY_PLATFORM_UNIX
	if (file->GetProcessID() != GetCurrentProcessID())
	{
		// in forked process
		if (ReadOnly)
		{
			// reading
			if (file->IfSupportForking())
			{
				file->SetProcessID();
			} else {
				throw ErrGDSFmt("Not support forking, "
					"please open the GDS file with 'allow.fork=TRUE'.");
			}
		} else {
			throw ErrGDSFmt("Creating/Opening and writing "
				"the GDS file should be in the same process.");
		}
	}
#endif
	return vObj;
}

/// convert "(CdGDSObj*)  -->  SEXP"
COREARRAY_DLL_EXPORT SEXP GDS_R_Obj2SEXP(PdGDSObj Obj)
{
	static const char *ERR_OBJ2SEXP = "Internal error in 'GDS_R_Obj2SEXP'.";

	SEXP rv = R_NilValue;
	if (Obj != NULL)
	{
		PROTECT(rv = NEW_RAW(GDSFMT_NUM_BYTE_FOR_GDSOBJ));
		SET_CLASS(rv, mkString("gdsn.class"));
		Rbyte *p = RAW(rv);
		memset(p, 0, GDSFMT_NUM_BYTE_FOR_GDSOBJ);

		int idx;
		map<PdGDSObj, int>::iterator it = GDSFMT_GDSObj_Map.find(Obj);
		if (it != GDSFMT_GDSObj_Map.end())
		{
			idx = it->second;
			if ((idx < 0) || (idx >= (int)GDSFMT_GDSObj_List.size()))
				throw ErrGDSFmt(ERR_OBJ2SEXP);
			if (GDSFMT_GDSObj_List[idx] != Obj)
				throw ErrGDSFmt(ERR_OBJ2SEXP);
		} else {
			vector<PdGDSObj>::iterator it =
				find(GDSFMT_GDSObj_List.begin(), GDSFMT_GDSObj_List.end(),
				(PdGDSObj)NULL);
			if (it != GDSFMT_GDSObj_List.end())
			{
				idx = it - GDSFMT_GDSObj_List.begin();
				*it = Obj;
			} else {
				idx = GDSFMT_GDSObj_List.size();
				GDSFMT_GDSObj_List.push_back(Obj);
			}
			GDSFMT_GDSObj_Map[Obj] = idx;
		}
		memcpy(p            , &idx, sizeof(int));
		memcpy(p+sizeof(int), &Obj, sizeof(PdGDSObj));

		UNPROTECT(1);
	}
	return rv;
}

/// convert "SEXP (ObjSrc)  -->  SEXP (ObjDst)"
COREARRAY_DLL_EXPORT void GDS_R_Obj_SEXP2SEXP(SEXP ObjDst, SEXP ObjSrc)
{
	CheckSEXPObject(ObjDst, false);
	CheckSEXPObject(ObjSrc, true);
	memcpy(RAW(ObjDst), RAW(ObjSrc), GDSFMT_NUM_BYTE_FOR_GDSOBJ);
}

/// return true, if Obj is a logical object in R
COREARRAY_DLL_EXPORT C_BOOL GDS_R_Is_Logical(PdGDSObj Obj)
{
	return Obj->Attribute().HasName(STR_LOGICAL);
}

/// return true, if Obj is a factor variable
COREARRAY_DLL_EXPORT C_BOOL GDS_R_Is_Factor(PdGDSObj Obj)
{
	if (Obj->Attribute().HasName(STR_CLASS) &&
		Obj->Attribute().HasName(STR_LEVELS))
	{
		return (Obj->Attribute()[STR_CLASS].GetStr8() == STR_FACTOR);
	} else
		return false;
}


/// return 1 used in UNPROTECT and set levels in 'Val' if Obj is a factor in R
/// otherwise return 0, assuming Obj is a factor variable
static int GDS_R_Set_Factor(PdGDSObj Obj, SEXP Val)
{
	int nProtected = 0;
	CdObjAttr &Attr = Obj->Attribute();
	if (Attr[STR_LEVELS].IsArray())
	{
		const CdAny *p = Attr[STR_LEVELS].GetArray();
		C_UInt32 L = Attr[STR_LEVELS].GetArrayLength();
		SEXP levels = PROTECT(NEW_CHARACTER(L));
		nProtected ++;
		for (C_UInt32 i=0; i < L; i++)
		{
			UTF8String s = p[i].GetStr8();
			SET_STRING_ELT(levels, i, mkCharLenCE(&s[0], s.size(), CE_UTF8));
		}
		SET_LEVELS(Val, levels);
		SET_CLASS(Val, mkString("factor"));
	} else if (Attr[STR_LEVELS].IsString())
	{
		SEXP levels = PROTECT(NEW_CHARACTER(1));
		nProtected ++;
		UTF8String s = Attr[STR_LEVELS].GetStr8();
		SET_STRING_ELT(levels, 0, mkCharLenCE(&s[0], s.size(), CE_UTF8));
		SET_LEVELS(Val, levels);
		SET_CLASS(Val, mkString("factor"));
	}
	// output
	return nProtected;
}

/// return 1 used in UNPROTECT and set levels in 'Val' if Obj is a factor in R
/// otherwise return 0
COREARRAY_DLL_EXPORT int GDS_R_Set_IfFactor(PdGDSObj Obj, SEXP Val)
{
	int nProtected = 0;
	CdObjAttr &Attr = Obj->Attribute();
	if (Attr.HasName(STR_CLASS) && Attr.HasName(STR_LEVELS))
	{
		if (Attr[STR_CLASS].GetStr8() == STR_FACTOR)
			nProtected = GDS_R_Set_Factor(Obj, Val);
	}
	// output
	return nProtected;
}

/// return R type for logical and factor
COREARRAY_DLL_EXPORT int GDS_R_Is_ExtType(PdGDSObj Obj)
{
	CdObjAttr &Attr = Obj->Attribute();
	if (Attr.HasName(STR_LOGICAL))
	{
		return GDS_R_ExtType_Logical;
	} else if (Attr.HasName(STR_CLASS) && Attr.HasName(STR_LEVELS))
	{
		if (Attr[STR_CLASS].GetStr8() == STR_FACTOR)
			return GDS_R_ExtType_Factor;
	}
	return 0;
}


/// return an R data object from a GDS object, allowing raw-type data
COREARRAY_DLL_EXPORT SEXP GDS_R_Array_Read(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length,
	const C_BOOL *const Selection[], C_UInt32 UseMode)
{
	SEXP rv_ans = R_NilValue;
	int nProtected = 0;

	try
	{
		CdAbstractArray::TArrayDim St, Cnt;
		if (Start == NULL)
		{
			memset(St, 0, sizeof(St));
			Start = St;
		}
		if (Length == NULL)
		{
			Obj->GetDim(Cnt);
			Length = Cnt;
		}

		CdAbstractArray::TArrayDim ValidCnt;
		Obj->GetInfoSelection(Start, Length, Selection, NULL, NULL, ValidCnt);

		C_Int64 TotalCount;
		if (Obj->DimCnt() > 0)
		{
			TotalCount = 1;
			for (int i=0; i < Obj->DimCnt(); i++)
				TotalCount *= ValidCnt[i];
		} else {
			// defined in CoreArray, Obj->DimCnt() should be always > 0
			throw ErrGDSFmt("Internal error in 'GDS_R_Array_Read'.");
		}

		if (TotalCount > 0)
		{
			#ifndef R_XLEN_T_MAX
			if (TotalCount > TdTraits<R_xlen_t>::Max())
			{
				throw ErrGDSFmt(
				"No support of long vectors, please use 64-bit R with version >=3.0!");
			}
			#endif

			// if it is a sparse matrix
			if ((UseMode & GDS_R_READ_ALLOW_SP_MATRIX) && IsSparseArray(Obj) &&
				(Obj->DimCnt() <= 2))
			{
				CdSpExStruct *Sp = dynamic_cast<CdSpExStruct*>(Obj);
				if (!Sp)
					throw ErrGDSFmt("Unknown sparse array!");
				vector<int> sp_i, sp_p;
				vector<double> sp_x;
				int ncol, nrow;
				Sp->SpRead(Start[0], Start[1], Length[0], Length[1],
					Selection ? Selection[0] : NULL,
					Selection ? Selection[1] : NULL,
					sp_i, sp_p, sp_x, ncol, nrow);
				rv_ans = GDS_New_SpCMatrix(&sp_x[0], &sp_i[0], &sp_p[0],
					sp_x.size(), nrow, ncol);
				return rv_ans;
			}

			void *buffer;
			enum C_SVType SV;
			if (COREARRAY_SV_INTEGER(Obj->SVType()))
			{
				int ExtType = GDS_R_Is_ExtType(Obj);
				if (ExtType == GDS_R_ExtType_Logical)
				{
					PROTECT(rv_ans = NEW_LOGICAL(TotalCount));
					buffer = LOGICAL(rv_ans);
					SV = svInt32;
				} else {
					bool use_raw = false;
					if (UseMode & GDS_R_READ_ALLOW_RAW_TYPE)
						use_raw = (Obj->BitOf() <= 8) && ExtType==0;
					if (use_raw)
					{
						PROTECT(rv_ans = NEW_RAW(TotalCount));
						buffer = RAW(rv_ans);
						SV = svInt8;
					} else {
						PROTECT(rv_ans = NEW_INTEGER(TotalCount));
						if (ExtType == GDS_R_ExtType_Factor)
							nProtected += GDS_R_Set_Factor(Obj, rv_ans);
						buffer = INTEGER(rv_ans);
						SV = svInt32;
					}
				}
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
				SEXP dim = PROTECT(NEW_INTEGER(Obj->DimCnt()));
				nProtected ++;
				int I = 0, *p = INTEGER(dim);
				for (int k=Obj->DimCnt()-1; k >= 0; k--)
					p[I++] = ValidCnt[k];
				SET_DIM(rv_ans, dim);
			}

			if (buffer != NULL)
			{
				if (!Selection)
					Obj->ReadData(Start, Length, buffer, SV);
				else
					Obj->ReadDataEx(Start, Length, Selection, buffer, SV);
			} else {
				vector<UTF8String> strbuf(TotalCount);
				if (!Selection)
					Obj->ReadData(Start, Length, &strbuf[0], SV);
				else
					Obj->ReadDataEx(Start, Length, Selection, &strbuf[0], SV);
				for (size_t i=0; i < strbuf.size(); i++)
				{
					UTF8String &s = strbuf[i];
					SET_STRING_ELT(rv_ans, i,
						mkCharLenCE(&s[0], s.size(), CE_UTF8));
				}
			}
		} else {
			if (COREARRAY_SV_INTEGER(Obj->SVType()))
			{
				int ExtType = GDS_R_Is_ExtType(Obj);
				if (ExtType == GDS_R_ExtType_Logical)
				{
					PROTECT(rv_ans = NEW_LOGICAL(0));
				} else {
					bool use_raw = false;
					if (UseMode & GDS_R_READ_ALLOW_RAW_TYPE)
						use_raw = (Obj->BitOf() <= 8) && ExtType==0;
					if (use_raw)
					{
						PROTECT(rv_ans = NEW_RAW(0));
					} else {
						PROTECT(rv_ans = NEW_INTEGER(0));
						if (ExtType == GDS_R_ExtType_Factor)
							nProtected += GDS_R_Set_Factor(Obj, rv_ans);
					}
				}
			} else if (COREARRAY_SV_FLOAT(Obj->SVType()))
			{
				PROTECT(rv_ans = NEW_NUMERIC(0));
			} else if (COREARRAY_SV_STRING(Obj->SVType()))
			{
				PROTECT(rv_ans = NEW_CHARACTER(0));
			} else
				throw ErrGDSFmt("Invalid SVType of GDS object.");
			nProtected ++;

			// initialize dimension
			if (Obj->DimCnt() > 1)
			{
				SEXP dim = PROTECT(NEW_INTEGER(Obj->DimCnt()));
				nProtected ++;
				int I = 0, *p = INTEGER(dim);
				for (int k=Obj->DimCnt()-1; k >= 0; k--)
					p[I++] = ValidCnt[k];
				SET_DIM(rv_ans, dim);
			}
		}

		// unprotect the object
		if (nProtected > 0)
			UNPROTECT(nProtected);
	}
	catch (ErrAllocRead &E)
	{
		throw ErrGDSFmt(ERR_WRITE_ONLY);
	}
	catch (EZLibError &E)
	{
		throw ErrGDSFmt(ERR_WRITE_ONLY);
	}

	return rv_ans;
}

/// apply user-defined function margin by margin
/** \param Num         [in] the number of GDS objects
 *  \param ObjList     [in] a list of GDS objects
 *  \param Margins     [in  margin indices starting from 0 with C orders
 *  \param Selection   [in] indicating selection
 *  \param Func        [in] a user-defined function
 *  \param Param       [in] the parameter passed to the user-defined function
**/
COREARRAY_DLL_EXPORT void GDS_R_Apply(int Num, PdAbstractArray ObjList[],
	int Margins[], const C_BOOL *const * const Selection[],
	void (*InitFunc)(SEXP Argument, C_Int32 Count, PdArrayRead ReadObjList[],
		void *_Param),
	void (*LoopFunc)(SEXP Argument, C_Int32 Idx, void *_Param),
	void *Param, C_BOOL IncOrDec, C_UInt32 UseMode)
{
	if (Num <= 0)
		throw ErrGDSFmt("GDS_R_Apply: Invalid 'Num=%d'.", Num);
	if (!IncOrDec)
		throw ErrGDSFmt("GDS_R_Apply: The current implementation does not support 'IncOrDec=FALSE'.");

	// -----------------------------------------------------------
	// get dimension information

	vector<int> DimCnt(Num);
	vector< vector<C_Int32> > DLen(Num);
	vector<enum C_SVType> SVType(Num);
	vector<unsigned> BitOf(Num);
	// for -- loop
	for (int i=0; i < Num; i++)
	{
		SVType[i] = ObjList[i]->SVType();
		if (!(COREARRAY_SV_NUMERIC(SVType[i]) || COREARRAY_SV_STRING(SVType[i])))
			throw ErrGDSFmt("GDS_R_Apply: Not numeric or character-type data.");
		DimCnt[i] = ObjList[i]->DimCnt();
		DLen[i].resize(DimCnt[i]);
		ObjList[i]->GetDim(&(DLen[i][0]));
		BitOf[i] = ObjList[i]->BitOf();
	}

	// -----------------------------------------------------------
	// margin should be integer

	for (int i=0; i < Num; i++)
	{
		if ((Margins[i] < 0) || (Margins[i] >= DimCnt[i]))
			throw ErrGDSFmt("GDS_R_Apply: Invalid 'Margins[%d]'!", i);
	}

	// -----------------------------------------------------------
	// initialize variables

	// array read object
	vector<CdArrayRead> Array(Num);
	vector<PdArrayRead> ArrayList(Num);
	for (int i=0; i < Num; i++)
	{
		ArrayList[i] = &Array[i];
		if (COREARRAY_SV_INTEGER(SVType[i]))
		{
			enum C_SVType SV = svInt32;
			int ExtType = GDS_R_Is_ExtType(ObjList[i]);
			if (ExtType == 0)
			{
				if (UseMode & GDS_R_READ_ALLOW_RAW_TYPE)
					if (BitOf[i] <= 8) SV = svInt8;
			}
			Array[i].Init(*ObjList[i], Margins[i], SV,
				Selection[i], false);
		} else if (COREARRAY_SV_FLOAT(SVType[i]))
		{
			Array[i].Init(*ObjList[i], Margins[i], svFloat64,
				Selection[i], false);
		} else if (COREARRAY_SV_STRING(SVType[i]))
		{
			Array[i].Init(*ObjList[i], Margins[i], svStrUTF8,
				Selection[i], false);
		} else
			throw ErrGDSFmt("GDS_R_Apply: Not support data type.");
	}

	// check the margin
	C_Int32 MCnt = Array[0].Count();
	for (int i=1; i < Num; i++)
	{
		if (Array[i].Count() != MCnt)
		{
			throw ErrGDSFmt(
				"GDS_R_Apply: ObjList[%d] should have the same number of "
				"elements as ObjList[0] marginally (Margins[%d] = Margins[0]).",
				i, i);
		}
	}

	// allocate internal buffer uniformly
	Balance_ArrayRead_Buffer(&(Array[0]), Array.size());

	// used in UNPROTECT
	int nProtected = 0;

	// initialize buffer pointers
	vector<void *> BufPtr(Num);
	SEXP Func_Argument = R_NilValue;

	if (Num > 1)
	{
		PROTECT(Func_Argument = NEW_LIST(Num));
		nProtected ++;
	}

	for (int i=0; i < Num; i++)
	{
		SEXP tmp;
		if (COREARRAY_SV_INTEGER(SVType[i]))
		{
			int ExtType = GDS_R_Is_ExtType(ObjList[i]);
			if (ExtType == GDS_R_ExtType_Logical)
			{
				PROTECT(tmp = NEW_LOGICAL(Array[i].MarginCount()));
				BufPtr[i] = LOGICAL(tmp);
			} else {
				bool use_raw = false;
				if (UseMode & GDS_R_READ_ALLOW_RAW_TYPE)
					use_raw = (ObjList[i]->BitOf() <= 8) && ExtType==0;
				if (use_raw)
				{
					PROTECT(tmp = NEW_RAW(Array[i].MarginCount()));
					BufPtr[i] = RAW(tmp);
				} else {
					PROTECT(tmp = NEW_INTEGER(Array[i].MarginCount()));
					if (ExtType == GDS_R_ExtType_Factor)
						nProtected += GDS_R_Set_Factor(ObjList[i], tmp);
					BufPtr[i] = INTEGER(tmp);
				}
			}
			nProtected ++;
		} else if (COREARRAY_SV_FLOAT(SVType[i]))
		{
			PROTECT(tmp = NEW_NUMERIC(Array[i].MarginCount()));
			nProtected ++;
			BufPtr[i] = REAL(tmp);
		} else if (COREARRAY_SV_STRING(SVType[i]))
		{
			PROTECT(tmp = NEW_STRING(Array[i].MarginCount()));
			nProtected ++;
			BufPtr[i] = tmp;
		} else {
			tmp = R_NilValue;
			BufPtr[i] = NULL;
		}

		// initialize dimension
		if (DimCnt[i] > 2)
		{
			SEXP dim;
			PROTECT(dim = NEW_INTEGER(DimCnt[i] - 1));
			nProtected ++;
			int I = 0;
			for (int k=DimCnt[i]-1; k >= 0; k--)
			{
				if (k != Margins[i])
					INTEGER(dim)[ I++ ] = Array[i].DimCntValid()[k];
			}

			if (tmp != R_NilValue) SET_DIM(tmp, dim);
		}

		if (Num > 1)
			SET_VECTOR_ELT(Func_Argument, i, tmp);
		else
			Func_Argument = tmp;
	}

	try
	{
		vector<UTF8String> buffer_string;
	
		// call the initial user-defined function
		(*InitFunc)(Func_Argument, Array[0].Count(), &ArrayList[0], Param);

		// for - loop
		while (!Array[0].Eof())
		{
			C_Int32 Idx = Array[0].MarginIndex();

			// read marginally
			for (int i=0; i < Num; i++)
			{
				if (!COREARRAY_SV_STRING(SVType[i]))
				{
					Array[i].Read(BufPtr[i]);
				} else {
					C_Int64 n = Array[i].MarginCount();
					if (n > (C_Int64)buffer_string.size())
						buffer_string.resize(n);
					Array[i].Read(&buffer_string[0]);
					SEXP bufstr = (SEXP)BufPtr[i];
					for (C_Int64 i=0; i < n; i++)
					{
						UTF8String &s = buffer_string[i];
						SET_STRING_ELT(bufstr, i,
							mkCharLenCE(&s[0], s.size(), CE_UTF8));
					}
				}
			}

			// call the user-defined function
			(*LoopFunc)(Func_Argument, Idx, Param);
		}

		if (nProtected > 0)
			UNPROTECT(nProtected);
	}
	catch (ErrAllocRead &E)
	{
		throw ErrGDSFmt(ERR_WRITE_ONLY);
	}
	catch (EZLibError &E)
	{
		throw ErrGDSFmt(ERR_WRITE_ONLY);
	}
}


// append R data
COREARRAY_DLL_EXPORT void GDS_R_Append(PdAbstractArray Obj, SEXP Val)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val) && (TYPEOF(Val)!=RAWSXP))
		throw ErrGDSFmt("'Val' should be integer, numeric, character, logical or raw.");

	if (XLENGTH(Val) <= 0) return;

	int nProtected = 0;
	C_SVType sv = Obj->SVType();

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
		vector<UTF8String> buf(Len);
		for (R_xlen_t i=0; i < Len; i++)
		{
			SEXP s = STRING_ELT(Val, i);
			if (s != NA_STRING)
				buf[i] = UTF8Text(translateCharUTF8(s));
		}
		Obj->Append(&(buf[0]), Len, svStrUTF8);
	} else
		throw ErrGDSFmt("No support!");

	UNPROTECT(nProtected);
}


// append R data with a range
COREARRAY_DLL_EXPORT void GDS_R_AppendEx(PdAbstractArray Obj, SEXP Val,
	size_t Start, size_t Count)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val) && (TYPEOF(Val)!=RAWSXP))
		throw ErrGDSFmt("'Val' should be integer, numeric, character, logical or raw.");

	size_t N = XLENGTH(Val);
	if ((Start >= N) || (Start+Count > N))
		throw ErrGDSFmt("'GDS_R_AppendEx', out of range.");

	if ((N <= 0) || (Count <= 0)) return;

	int nProtected = 0;
	C_SVType sv = Obj->SVType();

	if (COREARRAY_SV_INTEGER(sv))
	{
		PROTECT(Val = Rf_coerceVector(Val, INTSXP));
		nProtected ++;
		Obj->Append(INTEGER(Val)+Start, Count, svInt32);
	} else if (COREARRAY_SV_FLOAT(sv))
	{
		PROTECT(Val = Rf_coerceVector(Val, REALSXP));
		nProtected ++;
		Obj->Append(REAL(Val)+Start, Count, svFloat64);
	} else if (COREARRAY_SV_STRING(sv))
	{
		PROTECT(Val = Rf_coerceVector(Val, STRSXP));
		nProtected ++;
		vector<UTF8String> buf(Count);
		for (size_t i=0; i < Count; i++)
		{
			SEXP s = STRING_ELT(Val, Start + i);
			if (s != NA_STRING)
				buf[i] = UTF8Text(translateCharUTF8(s));
		}
		Obj->Append(&(buf[0]), Count, svStrUTF8);
	} else
		throw ErrGDSFmt("No support!");

	UNPROTECT(nProtected);
}


struct COREARRAY_DLL_LOCAL char_ptr_less
{
	bool operator ()(const char *s1, const char *s2) const
	{
		return (strcmp(s1, s2) < 0);
	}
};

/// is.element
COREARRAY_DLL_EXPORT void GDS_R_Is_Element(PdAbstractArray Obj, SEXP SetEL,
	C_BOOL Out[])
{
	R_xlen_t Len = XLENGTH(SetEL);
	int nProtected = 0;
	set<int> SetInt;
	set<double> SetFloat;
	set<const char *, char_ptr_less> SetString;

	// determine data type
	C_SVType ObjSV = Obj->SVType();
	if (COREARRAY_SV_INTEGER(ObjSV))
	{
		PROTECT(SetEL = Rf_coerceVector(SetEL, INTSXP));
		nProtected ++;
		int *p = INTEGER(SetEL);
		for (R_xlen_t i=0; i < Len; i++)
			SetInt.insert(*p++);
	} else if (COREARRAY_SV_FLOAT(ObjSV))
	{
		PROTECT(SetEL = Rf_coerceVector(SetEL, REALSXP));
		nProtected ++;
		double *p = REAL(SetEL);
		for (R_xlen_t i=0; i < Len; i++)
			SetFloat.insert(*p++);
	} else if (COREARRAY_SV_STRING(ObjSV))
	{
		PROTECT(SetEL = Rf_coerceVector(SetEL, STRSXP));
		nProtected ++;
		for (R_xlen_t i=0; i < Len; i++)
			SetString.insert(translateCharUTF8(STRING_ELT(SetEL, i)));
	} else
		throw ErrGDSFmt("Invalid SVType of array-oriented object.");

	// set values
	const int n_size = 4096;
	C_BOOL *pL = Out;
	CdIterator it = Obj->IterBegin();
	C_Int64 TotalCount = Obj->TotalCount();

	if (COREARRAY_SV_INTEGER(ObjSV))
	{
		int buffer[n_size];
		while (TotalCount > 0)
		{
			int n = (TotalCount >= n_size) ? n_size : TotalCount;
			it.ReadData(buffer, n, svInt32);
			for (int i=0; i < n; i++, pL++)
				*pL = SetInt.count(buffer[i]) ? TRUE : FALSE;
			TotalCount -= n;
		}
	} else if (COREARRAY_SV_FLOAT(ObjSV))
	{
		double buffer[n_size];
		while (TotalCount > 0)
		{
			int n = (TotalCount >= n_size) ? n_size : TotalCount;
			it.ReadData(buffer, n, svFloat64);
			for (int i=0; i < n; i++, pL++)
				*pL = SetFloat.count(buffer[i]) ? TRUE : FALSE;
			TotalCount -= n;
		}
	} else if (COREARRAY_SV_STRING(ObjSV))
	{
		UTF8String buffer[n_size];
		while (TotalCount > 0)
		{
			int n = (TotalCount >= n_size) ? n_size : TotalCount;
			it.ReadData(buffer, n, svStrUTF8);
			for (int i=0; i < n; i++, pL++)
				*pL = SetString.count(buffer[i].c_str()) ? TRUE : FALSE;
			TotalCount -= n;
		}
	}

	UNPROTECT(nProtected);
}



// ===========================================================================
// functions for file structure

/// create a GDS file
COREARRAY_DLL_EXPORT PdGDSFile GDS_File_Create(const char *FileName)
{
	// to register CoreArray classes and objects
	RegisterClass();

	int gds_idx = GetEmptyFileIndex();
	PdGDSFile file = NULL;

	try {
		file = new CdGDSFile;
		file->SaveAsFile(FileName);
		PKG_GDS_Files[gds_idx] = file;
	}
	catch (std::exception &E) {
		if (file) delete file;
		throw;
	}
	catch (const char *E) {
		if (file) delete file;
		throw;
	}
	catch (...) {
		if (file) delete file;
		throw;
	}
	return file;
}

/// open an existing GDS file
COREARRAY_DLL_EXPORT PdGDSFile GDS_File_Open(const char *FileName,
	C_BOOL ReadOnly, C_BOOL ForkSupport, C_BOOL AllowError)
{
	static const char *INFO_LOG = "Log:";
	static const char *INFO_SP = "  ";
	static const char *INFO_NEW_CMD =
		"Consider using 'openfn.gds(, allow.error=TRUE)'.";

	// to register CoreArray classes and objects
	RegisterClass();

	int gds_idx = GetEmptyFileIndex();
	PdGDSFile file = NULL;

	try {
		file = new CdGDSFile;
		if (!ForkSupport)
			file->LoadFile(FileName, ReadOnly, AllowError);
		else
			file->LoadFileFork(FileName, ReadOnly, AllowError);

		PKG_GDS_Files[gds_idx] = file;
	}
	catch (std::exception &E) {
		string Msg = E.what();
		if ((file!=NULL) && !file->Log().List().empty())
		{
			Msg.append(sLineBreak).append(INFO_LOG);
			for (size_t i=0; i < file->Log().List().size(); i++)
			{
				Msg.append(sLineBreak).append(INFO_SP);
				Msg.append(RawText(file->Log().List()[i].Msg));
			}
			if (!AllowError)
				Msg.append(sLineBreak).append(INFO_SP).append(INFO_NEW_CMD);
		}
		if (file) delete file;
		throw ErrGDSFmt(Msg);
	}
	catch (const char *E) {
		string Msg = E;
		if ((file!=NULL) && !file->Log().List().empty())
		{
			Msg.append(sLineBreak).append(INFO_LOG);
			for (size_t i=0; i < file->Log().List().size(); i++)
			{
				Msg.append(sLineBreak).append(INFO_SP);
				Msg.append(RawText(file->Log().List()[i].Msg));
			}
			if (!AllowError)
				Msg.append(sLineBreak).append(INFO_SP).append(INFO_NEW_CMD);
		}
		if (file) delete file;
		throw ErrGDSFmt(Msg);
	}
	catch (...) {
		if (file) delete file;
		throw;
	}
	return file;
}

/// close the GDS file
COREARRAY_DLL_EXPORT void GDS_File_Close(PdGDSFile File)
{
	int gds_idx = GetFileIndex(File, false);
	if (gds_idx >= 0)
	{
		PKG_GDS_Files[gds_idx] = NULL;

		// delete GDS objects in GDSFMT_GDSObj_List and GDSFMT_GDSObj_Map
		vector<PdGDSObj>::iterator p = GDSFMT_GDSObj_List.begin();
		for (; p != GDSFMT_GDSObj_List.end(); p++)
		{
			if (*p != NULL)
			{
				// for a virtual folder
				PdGDSObj Obj = *p;
				PdGDSFolder Folder = Obj->Folder();
				while (Folder != NULL)
				{
					Obj = Folder;
					Folder = Obj->Folder();
				}
				// Obj is the root, and then get the GDS file
				if (Obj->GDSFile() == File)
				{
					GDSFMT_GDSObj_Map.erase(*p);
					*p = NULL;
				}
			}
		}
	}
	if (File) delete File;
}

/// synchronize the GDS file
COREARRAY_DLL_EXPORT void GDS_File_Sync(PdGDSFile File)
{
	File->SyncFile();
}

/// reopen the GDS file if needed
COREARRAY_DLL_EXPORT C_BOOL GDS_File_Reopen(SEXP GDSObj)
{
	// information
	static const char *ERR_CLASS = "Invalid gds.class: no '%s'.";
	static const char *VAR_FN  = "filename";
	static const char *VAR_ID  = "id";
	static const char *VAR_PTR = "ptr";
	static const char *VAR_RD  = "readonly";
	static const char *VAR_RT  = "root";
	// pointer
	int i_ptr = GetIndexList(GDSObj, VAR_PTR);
	if (i_ptr < 0)
		throw ErrGDSFmt(ERR_CLASS, VAR_PTR);
	SEXP gds_ptr = VECTOR_ELT(GDSObj, i_ptr);
	if (TYPEOF(gds_ptr) != EXTPTRSXP)
		throw ErrGDSFmt("Invalid gds.class$ptr");
	if (R_ExternalPtrAddr(gds_ptr) == NULL)
	{
		// file name
		int i_fn = GetIndexList(GDSObj, VAR_FN);
		if (i_fn < 0)
			throw ErrGDSFmt(ERR_CLASS, VAR_FN);
		SEXP fn_obj = VECTOR_ELT(GDSObj, i_fn);
		if (TYPEOF(fn_obj) != STRSXP)
			throw ErrGDSFmt("Invalid gds.class$filename");
		const char *fn = CHAR(STRING_ELT(fn_obj, 0));
		// id
		int i_id = GetIndexList(GDSObj, VAR_ID);
		if (i_id < 0)
			throw ErrGDSFmt(ERR_CLASS, VAR_ID);
		// readonly
		int i_rd = GetIndexList(GDSObj, VAR_RD);
		if (i_rd < 0)
			throw ErrGDSFmt(ERR_CLASS, VAR_RD);
		int readonly = Rf_asLogical(VECTOR_ELT(GDSObj, i_rd));
		// root
		int i_rt = GetIndexList(GDSObj, VAR_RT);
		if (i_rt < 0)
			throw ErrGDSFmt(ERR_CLASS, VAR_RT);
		// open
		CdGDSFile *file = GDS_File_Open(fn, readonly, TRUE, FALSE);
		SEXP ID = ScalarInteger(GetFileIndex(file));
		SET_ELEMENT(GDSObj, i_id, ID);
		SET_ELEMENT(GDSObj, i_ptr, new_gdsptr_obj(file, ID, true));
		SET_ELEMENT(GDSObj, i_rt, GDS_R_Obj2SEXP(&(file->Root())));
		// output
		return TRUE;
	} else
		return FALSE;
}

/// get the root folder of a GDS file
COREARRAY_DLL_EXPORT PdGDSFolder GDS_File_Root(PdGDSFile File)
{
	return &File->Root();
}

/// get the GDS file from a GDS node
COREARRAY_DLL_EXPORT PdGDSFile GDS_Node_File(PdGDSObj Node)
{
	return Node->GDSFile();
}

COREARRAY_DLL_EXPORT C_BOOL GDS_Node_Load(PdGDSObj Node, int NodeID,
	const char *Path, PdGDSFile File, PdGDSObj *OutNode, int *OutNodeID)
{
	if (NodeID < 0)
	{
		if (!Node)
			Node = File->Root().Path(Path);
		map<PdGDSObj, int>::iterator it = GDSFMT_GDSObj_Map.find(Node);
		if (it != GDSFMT_GDSObj_Map.end())
		{
			if (OutNode) *OutNode = Node;
			if (OutNodeID) *OutNodeID = it->second;
		} else {
			vector<PdGDSObj>::iterator it =
				find(GDSFMT_GDSObj_List.begin(), GDSFMT_GDSObj_List.end(),
				(PdGDSObj)NULL);
			if (it != GDSFMT_GDSObj_List.end())
			{
				NodeID = it - GDSFMT_GDSObj_List.begin();
				*it = Node;
			} else {
				NodeID = GDSFMT_GDSObj_List.size();
				GDSFMT_GDSObj_List.push_back(Node);
			}
			GDSFMT_GDSObj_Map[Node] = NodeID;
			if (OutNode) *OutNode = Node;
			if (OutNodeID) *OutNodeID = NodeID;
		}
		return TRUE;
	}

	PdGDSObj Obj = NULL;
	if (NodeID < (int)GDSFMT_GDSObj_List.size())
		Obj = GDSFMT_GDSObj_List[NodeID];
	if ((Obj != Node) || !Node) // need update
	{
		if (!Obj)
		{
			Node = File->Root().Path(Path);
			vector<PdGDSObj>::iterator it =
				find(GDSFMT_GDSObj_List.begin(), GDSFMT_GDSObj_List.end(),
				(PdGDSObj)NULL);
			if (it != GDSFMT_GDSObj_List.end())
			{
				NodeID = it - GDSFMT_GDSObj_List.begin();
				*it = Node;
			} else {
				NodeID = GDSFMT_GDSObj_List.size();
				GDSFMT_GDSObj_List.push_back(Node);
			}
			GDSFMT_GDSObj_Map[Node] = NodeID;
		} else {
			Node = Obj;
		}
		if (OutNode) *OutNode = Node;
		if (OutNodeID) *OutNodeID = NodeID;
		return TRUE;
	} else
		return FALSE;
}

COREARRAY_DLL_EXPORT void GDS_Node_Unload(PdGDSObj Node)
{
	if (Node != NULL)
	{
		if (Node->Folder())
			Node->Folder()->UnloadObj(Node);
		else
			throw ErrGDSFmt("Can not unload the root.");

		// delete GDS objects in GDSFMT_GDSObj_List and GDSFMT_GDSObj_Map
		vector<PdGDSObj>::iterator p = GDSFMT_GDSObj_List.begin();
		for (; p != GDSFMT_GDSObj_List.end(); p++)
			if (*p == Node) *p = NULL;
		GDSFMT_GDSObj_Map.erase(Node);
	}
}

COREARRAY_DLL_EXPORT void GDS_Node_Delete(PdGDSObj Node, C_BOOL Force)
{
	if (Node != NULL)
	{
		vector<C_BOOL> DeleteArray;
		if (dynamic_cast<CdGDSAbsFolder*>(Node))
		{
			DeleteArray.resize(GDSFMT_GDSObj_List.size(), FALSE);
			size_t idx = 0;
			vector<PdGDSObj>::iterator p = GDSFMT_GDSObj_List.begin();
			for (; p != GDSFMT_GDSObj_List.end(); p++)
			{
				if (*p != NULL)
				{
					if (static_cast<CdGDSAbsFolder*>(Node)->HasChild(*p, true))
						DeleteArray[idx] = TRUE;
				}
				idx ++;
			}
		}

		if (Node->Folder())
			Node->Folder()->DeleteObj(Node, Force != FALSE);
		else
			throw ErrGDSFmt("Can not delete the root.");

		// delete GDS objects in GDSFMT_GDSObj_List and GDSFMT_GDSObj_Map
		vector<PdGDSObj>::iterator p = GDSFMT_GDSObj_List.begin();
		for (; p != GDSFMT_GDSObj_List.end(); p++)
			if (*p == Node) *p = NULL;
		GDSFMT_GDSObj_Map.erase(Node);

		if (!DeleteArray.empty())
		{
			size_t idx = 0;
			vector<C_BOOL>::iterator p = DeleteArray.begin();
			for (; p != DeleteArray.end(); p++)
			{
				if (*p)
				{
					PdGDSObj &Obj = GDSFMT_GDSObj_List[idx];
					GDSFMT_GDSObj_Map.erase(Obj);
					Obj = NULL;
				}
				idx ++;
			}
		}
	}
}

COREARRAY_DLL_EXPORT void GDS_Node_GetClassName(PdGDSObj Node, char *Out,
	size_t OutSize)
{
	string nm = Node->dName();
	if (Out)
		strncpy(Out, nm.c_str(), OutSize);
}

COREARRAY_DLL_EXPORT int GDS_Node_ChildCount(PdGDSFolder Node)
{
	return Node->NodeCount();
}

COREARRAY_DLL_EXPORT PdGDSObj GDS_Node_Path(PdGDSFolder Node,
	const char *Path, C_BOOL MustExist)
{
	if (MustExist)
		return Node->Path(Path);
	else
		return Node->PathEx(Path);
}


// ===========================================================================
// functions for attributes

COREARRAY_DLL_EXPORT int GDS_Attr_Count(PdGDSObj Node)
{
	return Node->Attribute().Count();
}

COREARRAY_DLL_EXPORT int GDS_Attr_Name2Index(PdGDSObj Node, const char *Name)
{
	return Node->Attribute().IndexName(Name);
}


// ===========================================================================
// functions for CdAbstractArray

COREARRAY_DLL_EXPORT int GDS_Array_DimCnt(PdAbstractArray Obj)
{
	return Obj->DimCnt();
}

COREARRAY_DLL_EXPORT void GDS_Array_GetDim(PdAbstractArray Obj,
	C_Int32 OutBuffer[], size_t N_Buf)
{
	if (Obj->DimCnt() > (int)N_Buf)
		throw ErrCoreArray("Insufficient buffer in 'GDS_Array_GetDim'.");
	Obj->GetDim(OutBuffer);
}

COREARRAY_DLL_EXPORT C_Int64 GDS_Array_GetTotalCount(PdAbstractArray Obj)
{
	return Obj->TotalCount();
}

COREARRAY_DLL_EXPORT enum C_SVType GDS_Array_GetSVType(PdAbstractArray Obj)
{
	return Obj->SVType();
}

COREARRAY_DLL_EXPORT unsigned GDS_Array_GetBitOf(PdAbstractArray Obj)
{
	return Obj->BitOf();
}

COREARRAY_DLL_EXPORT void *GDS_Array_ReadData(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length, void *OutBuf,
	enum C_SVType OutSV)
{
	return Obj->ReadData(Start, Length, OutBuf, OutSV);
}

COREARRAY_DLL_EXPORT void *GDS_Array_ReadDataEx(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length,
	const C_BOOL *const Selection[], void *OutBuf, enum C_SVType OutSV)
{
	return Obj->ReadDataEx(Start, Length, Selection, OutBuf, OutSV);
}

COREARRAY_DLL_EXPORT const void *GDS_Array_WriteData(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length, const void *InBuf,
	enum C_SVType InSV)
{
	return Obj->WriteData(Start, Length, InBuf, InSV);
}

COREARRAY_DLL_EXPORT const void *GDS_Array_AppendData(PdAbstractArray Obj,
	ssize_t Cnt, const void *InBuf, enum C_SVType InSV)
{
	return Obj->Append(InBuf, Cnt, InSV);
}

COREARRAY_DLL_EXPORT void GDS_Array_AppendString(PdAbstractArray Obj,
	const char *Text)
{
	UTF8String Val = Text;
	Obj->Append(&Val, 1, svStrUTF8);
}

COREARRAY_DLL_EXPORT void GDS_Array_AppendStrLen(PdAbstractArray Obj,
	const char *Text, size_t Len)
{
	UTF8String Val = UTF8String(Text, Len);
	Obj->Append(&Val, 1, svStrUTF8);
}



// ===========================================================================
// Functions for CdContainer - CdIterator

COREARRAY_DLL_EXPORT void GDS_Iter_GetStart(PdContainer Node, PdIterator Out)
{
	*Out = Node->IterBegin();
}

COREARRAY_DLL_EXPORT void GDS_Iter_GetEnd(PdContainer Node, PdIterator Out)
{
	*Out = Node->IterEnd();
}

COREARRAY_DLL_EXPORT PdContainer GDS_Iter_GetHandle(PdIterator I)
{
	return I->Handler;
}

COREARRAY_DLL_EXPORT void GDS_Iter_Offset(PdIterator I, C_Int64 Offset)
{
	*I += Offset;
}

COREARRAY_DLL_EXPORT void GDS_Iter_Position(PdContainer Node, PdIterator Out, C_Int64 Offset)
{
	*Out = Node->IterBegin();
	*Out += Offset;
}

COREARRAY_DLL_EXPORT C_Int64 GDS_Iter_GetInt(PdIterator I)
{
	return I->GetInteger();
}

COREARRAY_DLL_EXPORT C_Float64 GDS_Iter_GetFloat(PdIterator I)
{
	return I->GetFloat();
}

COREARRAY_DLL_EXPORT void GDS_Iter_GetStr(PdIterator I, char *Out, size_t Size)
{
	string s = RawText(I->GetString());
	if (Out)
		strncpy(Out, s.c_str(), Size);
}

COREARRAY_DLL_EXPORT void GDS_Iter_SetInt(PdIterator I, C_Int64 Val)
{
	I->SetInteger(Val);
}

COREARRAY_DLL_EXPORT void GDS_Iter_SetFloat(PdIterator I, C_Float64 Val)
{
	I->SetFloat(Val);
}

COREARRAY_DLL_EXPORT void GDS_Iter_SetStr(PdIterator I, const char *Str)
{
	I->SetString(UTF16Text(Str));
}

COREARRAY_DLL_EXPORT void *GDS_Iter_RData(PdIterator I, void *OutBuf,
	size_t Cnt, enum C_SVType OutSV)
{
	return I->ReadData(OutBuf, Cnt, OutSV);
}

COREARRAY_DLL_EXPORT void *GDS_Iter_RDataEx(PdIterator I, void *OutBuf,
	size_t Cnt, enum C_SVType OutSV, const C_BOOL Selection[])
{
	return I->ReadDataEx(OutBuf, Cnt, OutSV, Selection);
}

COREARRAY_DLL_EXPORT const void *GDS_Iter_WData(PdIterator I, const void *InBuf,
	size_t Cnt, enum C_SVType InSV)
{
	return I->WriteData(InBuf, Cnt, InSV);
}


// ===========================================================================
// functions for error

/// the last error message
COREARRAY_DLL_LOCAL string R_CoreArray_Error_Msg;

COREARRAY_DLL_EXPORT const char *GDS_GetError()
{
	return R_CoreArray_Error_Msg.c_str();
}

COREARRAY_DLL_EXPORT void GDS_SetError(const char *Msg)
{
	if (Msg)
	{
		if (Msg != GDS_GetError())
			R_CoreArray_Error_Msg = Msg;
	} else {
		R_CoreArray_Error_Msg.clear();
	}
}



// ===========================================================================
// functions for parallel computing

/// create a mutex object
COREARRAY_DLL_EXPORT PdThreadMutex GDS_Parallel_InitMutex()
{
	return new CdThreadMutex;
}

/// destroy the mutex object
COREARRAY_DLL_EXPORT void GDS_Parallel_DoneMutex(PdThreadMutex Obj)
{
	if (Obj) delete Obj;
}

/// lock the mutex object
COREARRAY_DLL_EXPORT void GDS_Parallel_LockMutex(PdThreadMutex Obj)
{
	if (Obj) Obj->Lock();
}

/// attempt to lock the mutex object, return true if succeed
COREARRAY_DLL_EXPORT C_BOOL GDS_Parallel_TryLockMutex(PdThreadMutex Obj)
{
	return Obj->TryLock() ? 1 : 0;
}

/// unlock the mutex object
COREARRAY_DLL_EXPORT void GDS_Parallel_UnlockMutex(PdThreadMutex Obj)
{
	if (Obj) Obj->Unlock();
}

/// initialize the condition object
COREARRAY_DLL_EXPORT PdThreadCondition GDS_Parallel_InitCondition()
{
	return new CdThreadCondition;
}

/// finalize the condition object
COREARRAY_DLL_EXPORT void GDS_Parallel_DoneCondition(PdThreadCondition Obj)
{
	if (Obj) delete Obj;
}

/// signal a condition object
COREARRAY_DLL_EXPORT void GDS_Parallel_SignalCondition(PdThreadCondition Obj)
{
	Obj->Signal();
}

/// broadcast a condition object
COREARRAY_DLL_EXPORT void GDS_Parallel_BroadcastCondition(PdThreadCondition Obj)
{
	Obj->Broadcast();
}

/// wait for a condition object to become signaled
COREARRAY_DLL_EXPORT void GDS_Parallel_WaitCondition(PdThreadCondition Obj,
	PdThreadMutex Mutex)
{
	Obj->Wait(*Mutex);
}

/// initialize a thread suspending object
COREARRAY_DLL_EXPORT PdThreadsSuspending GDS_Parallel_InitSuspend()
{
	return new CdThreadsSuspending;
}

/// destroy the thread suspending object
COREARRAY_DLL_EXPORT void GDS_Parallel_DoneSuspend(PdThreadsSuspending Obj)
{
	if (Obj) delete Obj;
}

/// suspend the thread suspending object
COREARRAY_DLL_EXPORT void GDS_Parallel_Suspend(PdThreadsSuspending Obj)
{
	if (Obj) Obj->Suspend();
}

/// wake up the thread suspending object
COREARRAY_DLL_EXPORT void GDS_Parallel_WakeUp(PdThreadsSuspending Obj)
{
	if (Obj) Obj->WakeUp();
}


/// the object with multiple threads for parallel computing
static CParallelBase R_CoreArray_ParallelBase;

/// parallel computing
COREARRAY_DLL_EXPORT void GDS_Parallel_RunThreads(
	void (*Proc)(PdThread, int, void*), void *Param, int nThread)
{
	R_CoreArray_ParallelBase.SetNumThread(nThread);
	R_CoreArray_ParallelBase.RunThreads((CParallelBase::TProc)Proc, Param);
}


// ===========================================================================
/// functions for machine

// Return the number of available CPU cores in the system
COREARRAY_DLL_EXPORT int GDS_Mach_GetNumOfCores()
{
	return Mach::GetCPU_NumOfCores();
}

/// Return the size in byte of level-1 cache memory
COREARRAY_DLL_EXPORT C_UInt64 GDS_Mach_GetCPULevelCache(int level)
{
	return Mach::GetCPU_LevelCache(level);
}


// ===========================================================================
// functions for reading block by block

/// read an array-oriented object margin by margin
COREARRAY_DLL_EXPORT PdArrayRead GDS_ArrayRead_Init(PdAbstractArray Obj,
	int Margin, enum C_SVType SVType, const C_BOOL *const Selection[],
	C_BOOL buf_if_need)
{
	PdArrayRead rv = new CdArrayRead;
	rv->Init(*Obj, Margin, SVType, Selection, buf_if_need);
	return rv;
}

/// free a 'CdArrayRead' object
COREARRAY_DLL_EXPORT void GDS_ArrayRead_Free(PdArrayRead Obj)
{
	if (Obj) delete Obj;
}

/// read data
COREARRAY_DLL_EXPORT void GDS_ArrayRead_Read(PdArrayRead Obj, void *Buffer)
{
	Obj->Read(Buffer);
}

/// return true, if it is of the end
COREARRAY_DLL_EXPORT C_BOOL GDS_ArrayRead_Eof(PdArrayRead Obj)
{
	return Obj->Eof();
}

/// reallocate the buffer with specified size with respect to array
COREARRAY_DLL_EXPORT void GDS_ArrayRead_BalanceBuffer(PdArrayRead array[],
	int n, C_Int64 buffer_size)
{
	Balance_ArrayRead_Buffer(array, n, buffer_size);
}



// ===========================================================================
// External packages

static SEXP LANG_LOAD_LIB_MATRIX = NULL;
static SEXP LANG_NEW_SP_MATRIX = NULL;

/// load the Matrix package
COREARRAY_DLL_EXPORT C_BOOL GDS_Load_Matrix()
{
	if (!flag_init_Matrix)
	{
		int error = 0;
		// LANG_LOAD_LIB_MATRIX = quote(require("Matrix", quietly=TRUE))
		SEXP rv = R_tryEval(LANG_LOAD_LIB_MATRIX, R_GlobalEnv, &error);
		if (error) return FALSE;
		flag_init_Matrix = (Rf_asLogical(rv) == TRUE);
	}
	return flag_init_Matrix;
}

inline static void check_load_pkg_matrix()
{
	if (!flag_init_Matrix)
	{
		if (!GDS_Load_Matrix())
			error("Fail to load the Matrix package!");
	}
}

inline static void set_lang_epr(SEXP epr, SEXP x, SEXP i, SEXP p, SEXP dm)
{
	// new("dgCMatrix", x=x, i=i, p=p, Dim=dm)
	SETCADDR(epr, x);   // parameter: x
	SETCADDDR(epr, i);  // parameter: i
	SETCAD4R(epr, p);   // parameter: p
	SEXP v = CDR(CDR(CDR(CDR(CDR(epr)))));
	SETCAR(v, dm);      // parameter: Dim
}

inline static SEXP sexp_dim(int nrow, int ncol)
{
	SEXP var_dm = PROTECT(NEW_INTEGER(2));
	int *pm = INTEGER(var_dm);
	pm[0] = nrow; pm[1] = ncol;
	UNPROTECT(1);
	return var_dm;
}

/// create a dgCMatrix R object
COREARRAY_DLL_EXPORT SEXP GDS_New_SpCMatrix(const double *x, const int *i,
	const int *p, int n_x, int nrow, int ncol)
{
	check_load_pkg_matrix();
	// LANG_NEW_SP_MATRIX = quote(new("dgCMatrix", x=x, i=i, p=p, Dim=dm))
	SEXP epr = PROTECT(LANG_NEW_SP_MATRIX);
	// parameter: x
	SEXP var_x = PROTECT(NEW_NUMERIC(n_x));
	memcpy(REAL(var_x), x, sizeof(double)*n_x);
	// parameter: i
	SEXP var_i = PROTECT(NEW_INTEGER(n_x));
	memcpy(INTEGER(var_i), i, sizeof(int)*n_x);
	// parameter: p
	SEXP var_p = PROTECT(NEW_INTEGER(ncol+1));
	memcpy(INTEGER(var_p), p, sizeof(int)*(ncol+1));
	// parameter: Dim
	SEXP var_dm = PROTECT(sexp_dim(nrow, ncol));
	set_lang_epr(epr, var_x, var_i, var_p, var_dm);
	// call
	SEXP rv = PROTECT(eval(epr, R_GlobalEnv));
	// free
	set_lang_epr(epr, R_NilValue, R_NilValue, R_NilValue, R_NilValue);
	UNPROTECT(6);
	// output
	return rv;
}

/// create a dgCMatrix R object using x, i & p (requiring >= v1.31.1)
COREARRAY_DLL_EXPORT SEXP GDS_New_SpCMatrix2(SEXP x, SEXP i, SEXP p,
	int nrow, int ncol)
{
	check_load_pkg_matrix();
	// LANG_NEW_SP_MATRIX = quote(new("dgCMatrix", x=x, i=i, p=p, Dim=dm))
	SEXP epr = PROTECT(LANG_NEW_SP_MATRIX);
	set_lang_epr(epr, x, i, p, sexp_dim(nrow, ncol));
	// call
	SEXP rv = PROTECT(eval(epr, R_GlobalEnv));
	// free
	set_lang_epr(epr, R_NilValue, R_NilValue, R_NilValue, R_NilValue);
	UNPROTECT(2);
	// output
	return rv;
}



// ===========================================================================
// initialize the package 'gdsfmt'

extern COREARRAY_DLL_LOCAL void R_Init_RegCallMethods(DllInfo *info);

COREARRAY_DLL_EXPORT SEXP gdsInitPkg(SEXP lang_var)
{
	LANG_LOAD_LIB_MATRIX = VECTOR_ELT(lang_var, 0);
	LANG_NEW_SP_MATRIX = VECTOR_ELT(lang_var, 1);
	return R_NilValue;
}

void R_init_gdsfmt(DllInfo *info)
{
	R_Init_RegCallMethods(info);

	static const char *pkg_name = "gdsfmt";
	#define REG(nm)    \
		R_RegisterCCallable(pkg_name, #nm, (DL_FUNC)&nm)
	#define REG2(nm, fn)    \
		R_RegisterCCallable(pkg_name, nm, (DL_FUNC)&fn)

	// R objects
	REG(GDS_R_SEXP2File);
	REG(GDS_R_SEXP2FileRoot);
	REG(GDS_R_SEXP2Obj);
	REG(GDS_R_Obj2SEXP);
	REG(GDS_R_Obj_SEXP2SEXP);
	REG(GDS_R_Is_Logical);
	REG(GDS_R_Is_Factor);
	REG(GDS_R_Is_ExtType);
	REG(GDS_R_Set_IfFactor);
	REG(GDS_R_Array_Read);
	REG(GDS_R_Apply);
	REG(GDS_R_Append);
	REG(GDS_R_AppendEx);
	REG(GDS_R_Is_Element);

	// functions for file structure
	REG(GDS_File_Create);
	REG(GDS_File_Open);
	REG(GDS_File_Close);
	REG(GDS_File_Sync);
	REG(GDS_File_Reopen);
	REG(GDS_File_Root);

	REG(GDS_Node_File);
	REG(GDS_Node_Unload);
	REG(GDS_Node_Load);
	REG(GDS_Node_Delete);
	REG(GDS_Node_GetClassName);
	REG(GDS_Node_ChildCount);
	REG(GDS_Node_Path);

	// functions for attributes
	REG(GDS_Attr_Count);
	REG(GDS_Attr_Name2Index);

	// functions for CdAbstractArray
	REG(GDS_Array_DimCnt);
	REG(GDS_Array_GetDim);
	REG(GDS_Array_GetTotalCount);
	REG(GDS_Array_GetSVType);
	REG(GDS_Array_GetBitOf);
	REG(GDS_Array_ReadData);
	REG(GDS_Array_ReadDataEx);
	REG(GDS_Array_WriteData);
	REG(GDS_Array_AppendData);
	REG(GDS_Array_AppendString);
	REG(GDS_Array_AppendStrLen);

	// functions for CdIterator
	REG(GDS_Iter_GetStart);
	REG(GDS_Iter_GetEnd);
	REG(GDS_Iter_GetHandle);
	REG(GDS_Iter_Offset);
	REG(GDS_Iter_Position);
	REG(GDS_Iter_GetInt);
	REG(GDS_Iter_GetFloat);
	REG(GDS_Iter_GetStr);
	REG(GDS_Iter_SetInt);
	REG(GDS_Iter_SetFloat);
	REG(GDS_Iter_SetStr);
	REG(GDS_Iter_RData);
	REG(GDS_Iter_RDataEx);
	REG(GDS_Iter_WData);

	// functions for error
	REG(GDS_GetError);
	REG(GDS_SetError);

	// functions for parallel computing
	REG(GDS_Parallel_InitMutex);
	REG(GDS_Parallel_DoneMutex);
	REG(GDS_Parallel_LockMutex);
	REG(GDS_Parallel_TryLockMutex);
	REG(GDS_Parallel_UnlockMutex);
	REG(GDS_Parallel_InitCondition);
	REG(GDS_Parallel_DoneCondition);
	REG(GDS_Parallel_SignalCondition);
	REG(GDS_Parallel_BroadcastCondition);
	REG(GDS_Parallel_WaitCondition);
	REG(GDS_Parallel_InitSuspend);
	REG(GDS_Parallel_DoneSuspend);
	REG(GDS_Parallel_Suspend);
	REG(GDS_Parallel_WakeUp);
	REG(GDS_Parallel_RunThreads);

	// functions for machine
	REG(GDS_Mach_GetNumOfCores);
	REG(GDS_Mach_GetCPULevelCache);

	// functions for reading block by block
	REG(GDS_ArrayRead_Init);
	REG(GDS_ArrayRead_Free);
	REG(GDS_ArrayRead_Read);
	REG(GDS_ArrayRead_Eof);
	REG(GDS_ArrayRead_BalanceBuffer);

	/// Matrix package
	REG(GDS_Load_Matrix);
	REG(GDS_New_SpCMatrix);
	REG(GDS_New_SpCMatrix2);
}

} // extern "C"
