// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// R_CoreArray.cpp: Export the C routines of CoreArray allowing C++ exceptions
//
// Copyright (C) 2014-2015    Xiuwen Zheng
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

/**
 *	\file     R_CoreArray.cpp
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2014 - 2015
 *	\brief    Export the C routines of CoreArray allowing C++ exceptions
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
	COREARRAY_DLL_LOCAL PdGDSFile GDSFMT_GDS_Files[GDSFMT_MAX_NUM_GDS_FILES];

	/// get the index in 'GDSFMT_GDS_Files' for NULL
	COREARRAY_DLL_LOCAL int GetEmptyFileIndex(bool throw_error=true)
	{
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
		{
			if (GDSFMT_GDS_Files[i] == NULL)
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

	/// get the index in 'GDSFMT_GDS_Files' for file
	COREARRAY_DLL_LOCAL int GetFileIndex(PdGDSFile file, bool throw_error=true)
	{
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
		{
			if (GDSFMT_GDS_Files[i] == file)
				return i;
		}
		if (throw_error)
		{
			throw ErrGDSFmt(
				"The GDS file has been closed, or invalid.");
		}
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
			memset(GDSFMT_GDS_Files, 0, sizeof(GDSFMT_GDS_Files));
			GDSFMT_GDSObj_List.reserve(1024);
		}

		/// finalization
		~CInitObject()
		{
			GDSFMT_GDSObj_List.clear();
			GDSFMT_GDSObj_Map.clear();

			for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
			{
				PdGDSFile file = GDSFMT_GDS_Files[i];
				if (file != NULL)
				{
					try {
						GDSFMT_GDS_Files[i] = NULL;
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

static const char *ERR_WRITE_ONLY =
	"Writable only and please call 'readmode.gdsn' before reading.";


/// get the list element named str, or return NULL
COREARRAY_INLINE static SEXP GetListElement(SEXP list, const char *str)
{
	SEXP elmt = R_NilValue;
	SEXP names = getAttrib(list, R_NamesSymbol);
	for (R_len_t i = 0; i < XLENGTH(list); i++)
	{
		if (strcmp(CHAR(STRING_ELT(names, i)), str) == 0)
		{
			elmt = VECTOR_ELT(list, i);
			break;
		}
	}
	return elmt;
}

/// check the validity of R SEXP
COREARRAY_INLINE static PdGDSObj CheckSEXPObject(SEXP Obj, bool Full)
{
	static const char *ERR_GDS_OBJ  =
		"Invalid GDS node object!";
	static const char *ERR_GDS_OBJ2 =
		"Invalid GDS node object (it was closed or deleted).";

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

	SEXP gds_id = GetListElement(File, "id");
	if (Rf_isNull(gds_id) || !Rf_isInteger(gds_id))
		throw ErrGDSFmt("The GDS object is invalid.");

	int id = Rf_asInteger(gds_id);
	if ((id < 0) || (id >= GDSFMT_MAX_NUM_GDS_FILES))
		throw ErrGDSFmt("The GDS ID (%d) is invalid.", id);

	PdGDSFile file = GDSFMT_GDS_Files[id];
	if (file == NULL)
		throw ErrGDSFmt("The GDS file has been closed.");

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
	return Obj->Attribute().HasName(ASC16("R.logical"));
}

/// return true, if Obj is a factor variable
COREARRAY_DLL_EXPORT C_BOOL GDS_R_Is_Factor(PdGDSObj Obj)
{
	if (Obj->Attribute().HasName(ASC16("R.class")) &&
		Obj->Attribute().HasName(ASC16("R.levels")))
	{
		return (RawText(Obj->Attribute()[ASC16("R.class")].GetStr8())
			== "factor");
	} else
		return false;
}

/// return 1 used in UNPROTECT and set levels in 'val' if Obj is a factor in R
/// otherwise return 0
COREARRAY_DLL_EXPORT int GDS_R_Set_IfFactor(PdGDSObj Obj, SEXP val)
{
	int nProtected = 0;

	if (Obj->Attribute().HasName(ASC16("R.class")) &&
		Obj->Attribute().HasName(ASC16("R.levels")))
	{
		if (RawText(Obj->Attribute()[ASC16("R.class")].GetStr8()) == "factor")
		{
			if (Obj->Attribute()[ASC16("R.levels")].IsArray())
			{
				const CdAny *p =
					Obj->Attribute()[ASC16("R.levels")].GetArray();
				C_UInt32 L =
					Obj->Attribute()[ASC16("R.levels")].GetArrayLength();

				SEXP levels;
				PROTECT(levels = NEW_CHARACTER(L));
				nProtected ++;
				for (C_UInt32 i=0; i < L; i++)
				{
					SET_STRING_ELT(levels, i, mkCharCE(
						RawText(p[i].GetStr8()).c_str(), CE_UTF8));
				}

				SET_LEVELS(val, levels);
				SET_CLASS(val, mkString("factor"));

			} else if (Obj->Attribute()[ASC16("R.levels")].IsString())
			{
				SEXP levels;
				PROTECT(levels = NEW_CHARACTER(1));
				nProtected ++;
				SET_STRING_ELT(levels, 0, mkCharCE(
					RawText(Obj->Attribute()[ASC16("R.levels")].GetStr8()).c_str(),
					CE_UTF8));

				SET_LEVELS(val, levels);
				SET_CLASS(val, mkString("factor"));
			}
		}
	}

	// output
	return nProtected;
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

			void *buffer;
			enum C_SVType SV;
			if (COREARRAY_SV_INTEGER(Obj->SVType()))
			{
				if (GDS_R_Is_Logical(Obj))
				{
					PROTECT(rv_ans = NEW_LOGICAL(TotalCount));
					buffer = LOGICAL(rv_ans);
					SV = svInt32;
				} else {
					bool use_raw = false;
					if (UseMode & GDS_R_READ_ALLOW_RAW_TYPE)
						use_raw = (Obj->BitOf() <= 8);
					if (use_raw)
					{
						PROTECT(rv_ans = NEW_RAW(TotalCount));
						buffer = RAW(rv_ans);
						SV = svInt8;
					} else {
						PROTECT(rv_ans = NEW_INTEGER(TotalCount));
						nProtected += GDS_R_Set_IfFactor(Obj, rv_ans);
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
					SET_STRING_ELT(rv_ans, i,
						mkCharCE(RawText(strbuf[i]).c_str(), CE_UTF8));
				}
			}
		} else {
			if (COREARRAY_SV_INTEGER(Obj->SVType()))
			{
				if (GDS_R_Is_Logical(Obj))
				{
					PROTECT(rv_ans = NEW_LOGICAL(0));
				} else {
					PROTECT(rv_ans = NEW_INTEGER(0));
					nProtected += GDS_R_Set_IfFactor(Obj, rv_ans);
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
			if (!GDS_R_Is_Logical(ObjList[i]))
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
			if (GDS_R_Is_Logical(ObjList[i]))
			{
				PROTECT(tmp = NEW_LOGICAL(Array[i].MarginCount()));
				BufPtr[i] = INTEGER(tmp);
			} else {
				bool use_raw = false;
				if (UseMode & GDS_R_READ_ALLOW_RAW_TYPE)
					use_raw = (BitOf[i] <= 8);
				if (use_raw)
				{
					PROTECT(tmp = NEW_RAW(Array[i].MarginCount()));
					BufPtr[i] = RAW(tmp);
				} else {
					PROTECT(tmp = NEW_INTEGER(Array[i].MarginCount()));
					BufPtr[i] = INTEGER(tmp);
				}
				nProtected += GDS_R_Set_IfFactor(ObjList[i], tmp);
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
						SET_STRING_ELT(bufstr, i,
							mkCharCE(RawText(buffer_string[i]).c_str(), CE_UTF8));
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
				*pL = SetString.count(RawText(buffer[i]).c_str()) ? TRUE : FALSE;
			TotalCount -= n;
		}
	}

	UNPROTECT(nProtected);
}



// ===========================================================================
// functions for file structure

COREARRAY_DLL_EXPORT PdGDSFile GDS_File_Create(const char *FileName)
{
	// to register CoreArray classes and objects
	RegisterClass();

	int gds_idx = GetEmptyFileIndex();
	PdGDSFile file = NULL;

	try {
		file = new CdGDSFile;
		file->SaveAsFile(FileName);
		GDSFMT_GDS_Files[gds_idx] = file;
	}
	catch (exception &E) {
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

COREARRAY_DLL_EXPORT PdGDSFile GDS_File_Open(const char *FileName,
	C_BOOL ReadOnly, C_BOOL ForkSupport)
{
	// to register CoreArray classes and objects
	RegisterClass();

	int gds_idx = GetEmptyFileIndex();
	PdGDSFile file = NULL;

	try {
		file = new CdGDSFile;
		if (!ForkSupport)
			file->LoadFile(FileName, ReadOnly);
		else
			file->LoadFileFork(FileName, ReadOnly);

		GDSFMT_GDS_Files[gds_idx] = file;
	}
	catch (exception &E) {
		string Msg = E.what();
		if ((file!=NULL) && !file->Log().List().empty())
		{
			Msg.append(sLineBreak);
			Msg.append("Log:");
			for (size_t i=0; i < file->Log().List().size(); i++)
			{
				Msg.append(sLineBreak);
				Msg.append(RawText(file->Log().List()[i].Msg));
			}
		}
		if (file) delete file;
		throw ErrGDSFmt(Msg);
	}
	catch (const char *E) {
		string Msg = E;
		if ((file!=NULL) && !file->Log().List().empty())
		{
			Msg.append(sLineBreak);
			Msg.append("Log:");
			for (size_t i=0; i < file->Log().List().size(); i++)
			{
				Msg.append(sLineBreak);
				Msg.append(RawText(file->Log().List()[i].Msg));
			}
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

COREARRAY_DLL_EXPORT void GDS_File_Close(PdGDSFile File)
{
	int gds_idx = GetFileIndex(File, false);
	if (gds_idx >= 0)
	{
		GDSFMT_GDS_Files[gds_idx] = NULL;

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

COREARRAY_DLL_EXPORT void GDS_File_Sync(PdGDSFile File)
{
	File->SyncFile();
}

COREARRAY_DLL_EXPORT PdGDSFolder GDS_File_Root(PdGDSFile File)
{
	return &File->Root();
}

COREARRAY_DLL_EXPORT PdGDSFile GDS_Node_File(PdGDSObj Node)
{
	return Node->GDSFile();
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
		return Node->Path(UTF16Text(Path));
	else
		return Node->PathEx(UTF16Text(Path));
}


// ===========================================================================
// functions for attributes

COREARRAY_DLL_EXPORT int GDS_Attr_Count(PdGDSObj Node)
{
	return Node->Attribute().Count();
}

COREARRAY_DLL_EXPORT int GDS_Attr_Name2Index(PdGDSObj Node, const char *Name)
{
	return Node->Attribute().IndexName(ASC16(Name));
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

COREARRAY_DLL_EXPORT void GDS_Array_ReadData(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length, void *OutBuf,
	enum C_SVType OutSV)
{
	Obj->ReadData(Start, Length, OutBuf, OutSV);
}

COREARRAY_DLL_EXPORT void GDS_Array_ReadDataEx(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length,
	const C_BOOL *const Selection[], void *OutBuf, enum C_SVType OutSV)
{
	Obj->ReadDataEx(Start, Length, Selection, OutBuf, OutSV);
}

COREARRAY_DLL_EXPORT void GDS_Array_WriteData(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length, const void *InBuf,
	enum C_SVType InSV)
{
	Obj->WriteData(Start, Length, InBuf, InSV);
}

COREARRAY_DLL_EXPORT void GDS_Array_AppendData(PdAbstractArray Obj,
	ssize_t Cnt, const void *InBuf, enum C_SVType InSV)
{
	Obj->Append(InBuf, Cnt, InSV);
}

COREARRAY_DLL_EXPORT void GDS_Array_AppendString(PdAbstractArray Obj,
	const char *Text)
{
	UTF8String val = UTF8Text(Text);
	Obj->Append(&val, 1, svStrUTF8);
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

COREARRAY_DLL_EXPORT void GDS_Iter_RData(PdIterator I, void *OutBuf,
	size_t Cnt, enum C_SVType OutSV)
{
	I->ReadData(OutBuf, Cnt, OutSV);
}

COREARRAY_DLL_EXPORT void GDS_Iter_RDataEx(PdIterator I, void *OutBuf,
	size_t Cnt, enum C_SVType OutSV, const C_BOOL Selection[])
{
	I->ReadDataEx(OutBuf, Cnt, OutSV, Selection);
}

COREARRAY_DLL_EXPORT void GDS_Iter_WData(PdIterator I, const void *InBuf,
	size_t Cnt, enum C_SVType InSV)
{
	I->WriteData(InBuf, Cnt, InSV);
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
		{
			R_CoreArray_Error_Msg = Msg;
		}
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

/// unlock the mutex object
COREARRAY_DLL_EXPORT void GDS_Parallel_UnlockMutex(PdThreadMutex Obj)
{
	if (Obj) Obj->Unlock();
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
// initialize the package 'gdsfmt'

extern COREARRAY_DLL_LOCAL void R_Init_RegCallMethods(DllInfo *info);

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
	REG(GDS_R_Set_IfFactor);
	REG(GDS_R_Array_Read);
	REG(GDS_R_Apply);
	REG(GDS_R_Is_Element);

	// functions for file structure
	REG(GDS_File_Create);
	REG(GDS_File_Open);
	REG(GDS_File_Close);
	REG(GDS_File_Sync);
	REG(GDS_File_Root);

	REG(GDS_Node_File);
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
	REG(GDS_Parallel_UnlockMutex);
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
}

} // extern "C"
