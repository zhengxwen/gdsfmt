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
// Copyright (C) 2011-2015    Xiuwen Zheng
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

#define COREARRAY_GDSFMT_PACKAGE

#include "R_GDS_CPP.h"
#include <string>
#include <set>
#include <map>

#include <Rdefines.h>
#include <R_ext/Rdynload.h>


namespace gdsfmt
{
	extern PdGDSFile GDSFMT_GDS_Files[];
	extern int GetEmptyFileIndex(bool throw_error=true);
	extern int GetFileIndex(PdGDSFile file, bool throw_error=true);


	/// initialization and finalization
	class COREARRAY_DLL_LOCAL CInitNameObject
	{
	public:

		/// comparison of "const char *"
		struct strCmp {
			bool operator()(const char* s1, const char* s2) const
				{ return strcmp(s1, s2) < 0; }
		};

		/// mapping object
		map<const char*, const char*, strCmp> ClassMap;

		/// initialization
		CInitNameObject()
		{
			// ==============================================================
			// integer

			ClassMap["int8"  ] = TdTraits< C_Int8   >::StreamName();
			ClassMap["uint8" ] = TdTraits< C_UInt8  >::StreamName();
			ClassMap["int16" ] = TdTraits< C_Int16  >::StreamName();
			ClassMap["uint16"] = TdTraits< C_UInt16 >::StreamName();
			ClassMap["int32" ] = TdTraits< C_Int32  >::StreamName();
			ClassMap["uint32"] = TdTraits< C_UInt32 >::StreamName();
			ClassMap["int64" ] = TdTraits< C_Int64  >::StreamName();
			ClassMap["uint64"] = TdTraits< C_UInt64 >::StreamName();
			ClassMap["int24" ] = TdTraits< CoreArray::Int24  >::StreamName();
			ClassMap["uint24"] = TdTraits< CoreArray::UInt24 >::StreamName();

			ClassMap["bit1"] = TdTraits< CoreArray::BIT1 >::StreamName();
			ClassMap["bit2"] = TdTraits< CoreArray::BIT2 >::StreamName();
			ClassMap["bit3"] = TdTraits< CoreArray::BIT3 >::StreamName();
			ClassMap["bit4"] = TdTraits< CoreArray::BIT4 >::StreamName();
			ClassMap["bit5"] = TdTraits< CoreArray::BIT5 >::StreamName();
			ClassMap["bit6"] = TdTraits< CoreArray::BIT6 >::StreamName();
			ClassMap["bit7"] = TdTraits< CoreArray::BIT7 >::StreamName();
			ClassMap["bit8"] = TdTraits< C_UInt8         >::StreamName();

			ClassMap["bit9" ] = TdTraits< CoreArray::BIT9  >::StreamName();
			ClassMap["bit10"] = TdTraits< CoreArray::BIT10 >::StreamName();
			ClassMap["bit11"] = TdTraits< CoreArray::BIT11 >::StreamName();
			ClassMap["bit12"] = TdTraits< CoreArray::BIT12 >::StreamName();
			ClassMap["bit13"] = TdTraits< CoreArray::BIT13 >::StreamName();
			ClassMap["bit14"] = TdTraits< CoreArray::BIT14 >::StreamName();
			ClassMap["bit15"] = TdTraits< CoreArray::BIT15 >::StreamName();
			ClassMap["bit16"] = TdTraits< C_UInt16         >::StreamName();

			ClassMap["bit17"] = TdTraits< CoreArray::BIT17 >::StreamName();
			ClassMap["bit18"] = TdTraits< CoreArray::BIT18 >::StreamName();
			ClassMap["bit19"] = TdTraits< CoreArray::BIT19 >::StreamName();
			ClassMap["bit20"] = TdTraits< CoreArray::BIT20 >::StreamName();
			ClassMap["bit21"] = TdTraits< CoreArray::BIT21 >::StreamName();
			ClassMap["bit22"] = TdTraits< CoreArray::BIT22 >::StreamName();
			ClassMap["bit23"] = TdTraits< CoreArray::BIT23 >::StreamName();
			ClassMap["bit24"] = TdTraits< CoreArray::UInt24 >::StreamName();

			ClassMap["bit25"] = TdTraits< CoreArray::BIT25 >::StreamName();
			ClassMap["bit26"] = TdTraits< CoreArray::BIT26 >::StreamName();
			ClassMap["bit27"] = TdTraits< CoreArray::BIT27 >::StreamName();
			ClassMap["bit28"] = TdTraits< CoreArray::BIT28 >::StreamName();
			ClassMap["bit29"] = TdTraits< CoreArray::BIT29 >::StreamName();
			ClassMap["bit30"] = TdTraits< CoreArray::BIT30 >::StreamName();
			ClassMap["bit31"] = TdTraits< CoreArray::BIT31 >::StreamName();
			ClassMap["bit32"] = TdTraits< C_UInt32          >::StreamName();

			ClassMap["bit64"] = TdTraits< C_UInt64          >::StreamName();

			ClassMap["sbit2"] = TdTraits< CoreArray::SBIT2 >::StreamName();
			ClassMap["sbit3"] = TdTraits< CoreArray::SBIT3 >::StreamName();
			ClassMap["sbit4"] = TdTraits< CoreArray::SBIT4 >::StreamName();
			ClassMap["sbit5"] = TdTraits< CoreArray::SBIT5 >::StreamName();
			ClassMap["sbit6"] = TdTraits< CoreArray::SBIT6 >::StreamName();
			ClassMap["sbit7"] = TdTraits< CoreArray::SBIT7 >::StreamName();
			ClassMap["sbit8"] = TdTraits< C_Int8           >::StreamName();

			ClassMap["sbit9" ] = TdTraits< CoreArray::SBIT9  >::StreamName();
			ClassMap["sbit10"] = TdTraits< CoreArray::SBIT10 >::StreamName();
			ClassMap["sbit11"] = TdTraits< CoreArray::SBIT11 >::StreamName();
			ClassMap["sbit12"] = TdTraits< CoreArray::SBIT12 >::StreamName();
			ClassMap["sbit13"] = TdTraits< CoreArray::SBIT13 >::StreamName();
			ClassMap["sbit14"] = TdTraits< CoreArray::SBIT14 >::StreamName();
			ClassMap["sbit15"] = TdTraits< CoreArray::SBIT15 >::StreamName();
			ClassMap["sbit16"] = TdTraits< C_Int16           >::StreamName();

			ClassMap["sbit17"] = TdTraits< CoreArray::SBIT17 >::StreamName();
			ClassMap["sbit18"] = TdTraits< CoreArray::SBIT18 >::StreamName();
			ClassMap["sbit19"] = TdTraits< CoreArray::SBIT19 >::StreamName();
			ClassMap["sbit20"] = TdTraits< CoreArray::SBIT20 >::StreamName();
			ClassMap["sbit21"] = TdTraits< CoreArray::SBIT21 >::StreamName();
			ClassMap["sbit22"] = TdTraits< CoreArray::SBIT22 >::StreamName();
			ClassMap["sbit23"] = TdTraits< CoreArray::SBIT23 >::StreamName();
			ClassMap["sbit24"] = TdTraits< CoreArray::Int24 >::StreamName();

			ClassMap["sbit25"] = TdTraits< CoreArray::SBIT25 >::StreamName();
			ClassMap["sbit26"] = TdTraits< CoreArray::SBIT26 >::StreamName();
			ClassMap["sbit27"] = TdTraits< CoreArray::SBIT27 >::StreamName();
			ClassMap["sbit28"] = TdTraits< CoreArray::SBIT28 >::StreamName();
			ClassMap["sbit29"] = TdTraits< CoreArray::SBIT29 >::StreamName();
			ClassMap["sbit30"] = TdTraits< CoreArray::SBIT30 >::StreamName();
			ClassMap["sbit31"] = TdTraits< CoreArray::SBIT31 >::StreamName();
			ClassMap["sbit32"] = TdTraits< C_Int32          >::StreamName();

			ClassMap["sbit64"] = TdTraits< C_Int64          >::StreamName();


			// ==============================================================
			// Real number

			ClassMap["float32"] = TdTraits< C_Float32 >::StreamName();
			ClassMap["float64"] = TdTraits< C_Float64 >::StreamName();
			ClassMap["packedreal8"]  = TdTraits< TREAL8  >::StreamName();
			ClassMap["packedreal16"] = TdTraits< TREAL16 >::StreamName();
			ClassMap["packedreal32"] = TdTraits< TREAL32 >::StreamName();


			// ==============================================================
			// String

			ClassMap["string"   ] = TdTraits< VARIABLE_LENGTH<C_UTF8>  >::StreamName();
			ClassMap["string16" ] = TdTraits< VARIABLE_LENGTH<C_UTF16> >::StreamName();
			ClassMap["string32" ] = TdTraits< VARIABLE_LENGTH<C_UTF32> >::StreamName();
			ClassMap["fstring"  ] = TdTraits< FIXED_LENGTH<C_UTF8>  >::StreamName();
			ClassMap["fstring16"] = TdTraits< FIXED_LENGTH<C_UTF16> >::StreamName();
			ClassMap["fstring32"] = TdTraits< FIXED_LENGTH<C_UTF32> >::StreamName();


			// ==============================================================
			// R storage mode

			ClassMap["char"     ] = TdTraits< C_Int8  >::StreamName();
			ClassMap["raw"      ] = TdTraits< C_Int8  >::StreamName();
			ClassMap["int"      ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["integer"  ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["float"    ] = TdTraits< C_Float32 >::StreamName();
			ClassMap["numeric"  ] = TdTraits< C_Float64 >::StreamName();
			ClassMap["double"   ] = TdTraits< C_Float64 >::StreamName();
			ClassMap["character"] = TdTraits< VARIABLE_LENGTH<C_UTF8> >::StreamName();
			ClassMap["logical"  ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["factor"   ] = TdTraits< C_Int32 >::StreamName();

			ClassMap["list"] = "$FOLDER$";
			ClassMap["folder"] = "$FOLDER$";
		}
	};
	
	static CInitNameObject Init;
}


using namespace std;
using namespace CoreArray;
using namespace gdsfmt;


extern "C"
{

// error messages
static const char *ERR_NOT_FOLDER =
	"The GDS node is not a folder!";
static const char *ERR_READ_ONLY =
	"Read-only and please call 'compression.gdsn(node, \"\")' before writing.";


static SEXP mkStringUTF8(const char *s)
{
	SEXP rv = PROTECT(NEW_CHARACTER(1));
	SET_STRING_ELT(rv, 0, mkCharCE(s, CE_UTF8));
	UNPROTECT(1);
	return rv;
}

/// Get the list element named str, or return NULL
static SEXP GetListElement(SEXP list, const char *str)
{
	SEXP elmt = R_NilValue;
	SEXP names = getAttrib(list, R_NamesSymbol);
	R_xlen_t n = XLENGTH(list);
	for (R_xlen_t i=0; i < n; i++)
	{
		if (strcmp(CHAR(STRING_ELT(names, i)), str) == 0)
		{
			elmt = VECTOR_ELT(list, i);
			break;
		}
	}
	return elmt;
}

/// Get whether it is in a list
static bool IsElement(const char *s, const char *list[])
{
	while (*list)
	{
		if (strcmp(s, *list) == 0)
			return true;
		list ++;
	}
	return false;
}


extern SEXP gdsObjWriteAll(SEXP Node, SEXP Val, SEXP Check);
extern SEXP gdsObjSetDim(SEXP Node, SEXP DLen, SEXP Permute);

#include "gdsfmt_deprecated.h"


// ----------------------------------------------------------------------------
// File Operations
// ----------------------------------------------------------------------------

/// create a GDS file
/** \param FileName    [in] the file name
 *  \param AllowDup    [in] allow duplicate file
 *  \return
 *    $filename    the file name to be created
 *    $id          ID of GDS file, an integer, internal use
 *    $root        the root of hierachical structure
 *    $readonly	   whether it is read-only or not
**/
COREARRAY_DLL_EXPORT SEXP gdsCreateGDS(SEXP FileName, SEXP AllowDup)
{
	const char *fn = CHAR(STRING_ELT(FileName, 0));

	int allow_dup = Rf_asLogical(AllowDup);
	if (allow_dup == NA_LOGICAL)
		error("'allow.duplicate' must be TRUE or FALSE.");

	COREARRAY_TRY

		if (!allow_dup)
		{
			UTF8String FName = UTF8Text(fn);
			for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
			{
				if (GDSFMT_GDS_Files[i])
				{
					if (GDSFMT_GDS_Files[i]->FileName() == FName)
					{
						throw ErrGDSFmt(
							"The file '%s' has been created or opened.", fn);
					}
				}
			}
		}

		CdGDSFile *file = GDS_File_Create(fn);
		PROTECT(rv_ans = NEW_LIST(4));
			SET_ELEMENT(rv_ans, 0, FileName);
			SET_ELEMENT(rv_ans, 1, ScalarInteger(GetFileIndex(file)));
			SET_ELEMENT(rv_ans, 2, GDS_R_Obj2SEXP(&(file->Root())));
			SET_ELEMENT(rv_ans, 3, ScalarLogical(FALSE));
		UNPROTECT(1);
	
	COREARRAY_CATCH
}


/// open an existing GDS file
/** \param FileName    [in] the file name
 *  \param ReadOnly    [in] if TRUE, read-only
 *  \param AllowDup    [in] allow duplicate file
 *  \param AllowFork   [in] allow opening in a forked process
 *  \return
 *    $filename    the file name to be created
 *    $id          ID of GDS file, internal use
 *    $root        the root of hierachical structure
 *    $readonly	   whether it is read-only or not
**/
COREARRAY_DLL_EXPORT SEXP gdsOpenGDS(SEXP FileName, SEXP ReadOnly,
	SEXP AllowDup, SEXP AllowFork)
{
	const char *fn = CHAR(STRING_ELT(FileName, 0));

	int readonly = Rf_asLogical(ReadOnly);
	if (readonly == NA_LOGICAL)
		error("'readonly' must be TRUE or FALSE.");

	int allow_dup = Rf_asLogical(AllowDup);
	if (allow_dup == NA_LOGICAL)
		error("'allow.duplicate' must be TRUE or FALSE.");

	int allow_fork = Rf_asLogical(AllowFork);
	if (allow_fork == NA_LOGICAL)
		error("'allow.fork' must be TRUE or FALSE.");

	COREARRAY_TRY

		if (!allow_dup)
		{
			UTF8String FName = UTF8Text(fn);
			for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
			{
				if (GDSFMT_GDS_Files[i])
				{
					if (GDSFMT_GDS_Files[i]->FileName() == FName)
					{
						throw ErrGDSFmt(
							"The file '%s' has been created or opened.", fn);
					}
				}
			}
		}

		CdGDSFile *file = GDS_File_Open(fn, readonly, allow_fork);
		PROTECT(rv_ans = NEW_LIST(4));
			SET_ELEMENT(rv_ans, 0, FileName);
			SET_ELEMENT(rv_ans, 1, ScalarInteger(GetFileIndex(file)));
			SET_ELEMENT(rv_ans, 2, GDS_R_Obj2SEXP(&(file->Root())));
			SET_ELEMENT(rv_ans, 3, ScalarLogical(readonly));
		UNPROTECT(1);
	
	COREARRAY_CATCH
}


/// close the GDS file
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsCloseGDS(SEXP gdsfile)
{
	COREARRAY_TRY
		GDS_File_Close(GDS_R_SEXP2File(gdsfile));
	COREARRAY_CATCH
}


/// synchronize the GDS file
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsSyncGDS(SEXP gdsfile)
{
	COREARRAY_TRY
		GDS_R_SEXP2File(gdsfile)->SyncFile();
	COREARRAY_CATCH
}


/// detect whether the file has been opened
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsFileValid(SEXP gdsfile)
{
	COREARRAY_TRY
		GDS_R_SEXP2File(gdsfile);
	COREARRAY_CATCH
}


/// clean up fragments of a GDS file
/** \param FileName    [in] the file name
 *  \param Verbose     [in] if TRUE, show information
**/
COREARRAY_DLL_EXPORT SEXP gdsTidyUp(SEXP FileName, SEXP Verbose)
{
	const char *fn = CHAR(STRING_ELT(FileName, 0));

	int verbose_flag = Rf_asLogical(Verbose);
	if (verbose_flag == NA_LOGICAL)
		error("'verbose' must be TRUE or FALSE.");

	COREARRAY_TRY

		CdGDSFile file(fn, CdGDSFile::dmOpenReadWrite);
		if (verbose_flag == TRUE)
		{
			Rprintf("Clean up the fragments of GDS file:\n");
			Rprintf("\topen the file \"%s\" (size: %s).\n", fn,
				IntToStr(file.GetFileSize()).c_str());
			Rprintf("\t# of fragments in total: %d.\n",
				file.GetNumOfFragment());
			Rprintf("\tsave it to \"%s.tmp\".\n", fn);
		}
		file.TidyUp(false);
		if (verbose_flag == TRUE)
		{
			Rprintf("\trename \"%s.tmp\" (size: %s).\n", fn,
				IntToStr(file.GetFileSize()).c_str());
			Rprintf("\t# of fragments in total: %d.\n",
				file.GetNumOfFragment());
		}

	COREARRAY_CATCH
}


/// get all handles of opened GDS files
COREARRAY_DLL_EXPORT SEXP gdsGetConnection()
{
	COREARRAY_TRY

		int FileCnt = 0;
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
		{
			if (GDSFMT_GDS_Files[i])
				FileCnt ++;
		}

		int nProtected = 0;
		PROTECT(rv_ans = NEW_LIST(FileCnt));
		nProtected ++;

		FileCnt = 0;
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
		{
			PdGDSFile file = GDSFMT_GDS_Files[i];
			if (file)
			{
				SEXP handle;
				PROTECT(handle = NEW_LIST(4));
				nProtected ++;
				SET_ELEMENT(rv_ans, FileCnt, handle);
				FileCnt ++;

				SET_ELEMENT(handle, 0,
					mkString(RawText(file->FileName()).c_str()));
				SET_ELEMENT(handle, 1, ScalarInteger(i));
				SET_ELEMENT(handle, 2, GDS_R_Obj2SEXP(&(file->Root())));
				SET_ELEMENT(handle, 3,
					ScalarLogical(file->ReadOnly() ? TRUE : FALSE));
			}
		}

		UNPROTECT(nProtected);

	COREARRAY_CATCH
}


static map<TdGDSBlockID, string> diag_MapID;

static void diag_EnumObject(CdGDSObj &Obj)
{
	vector<const CdBlockStream*> LIST;

	string name = RawText(Obj.FullName());
	if (name.empty()) name = "/";

	diag_MapID[Obj.GDSStream()->ID()] = name + " $head$";
	Obj.GetOwnBlockStream(LIST);
	for (int j=0; j < (int)LIST.size(); j++)
		diag_MapID[LIST[j]->ID()] = name + " $data$";

	if (dynamic_cast<CdGDSFolder*>(&Obj))
	{
		CdGDSFolder &Folder = *static_cast<CdGDSFolder*>(&Obj);
		for (int i=0; i < Folder.NodeCount(); i++)
		{
			CdGDSObj *obj = Folder.ObjItem(i);
			diag_EnumObject(*obj);
		}
	}
}

/// synchronize a GDS file
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsDiagInfo(SEXP gdsfile)
{
	COREARRAY_TRY

		// get the GDS file
		CdGDSFile *tmp = GDS_R_SEXP2File(gdsfile);
		CdBlockCollection *file = (CdBlockCollection*)tmp;

		// load objects
		diag_MapID.clear();
		diag_EnumObject(tmp->Root());

		int nProtected = 0;

		PROTECT(rv_ans = NEW_LIST(2));
		nProtected ++;

		// ===============================================================
		// Stream List

		SEXP SList = PROTECT(NEW_LIST(5));
		nProtected ++;
		SET_ELEMENT(rv_ans, 0, SList);

		const vector<CdBlockStream*> &BL = file->BlockList();
		int n = BL.size() + 1;

		SEXP IDList    = PROTECT(NEW_INTEGER(n));
		SEXP TotalSize = PROTECT(NEW_NUMERIC(n));
		SEXP Capacity  = PROTECT(NEW_NUMERIC(n));
		SEXP nFragment = PROTECT(NEW_INTEGER(n));
		SEXP NameList  = PROTECT(NEW_CHARACTER(n));
		nProtected += 5;
		SET_ELEMENT(SList, 0, IDList);
		SET_ELEMENT(SList, 1, TotalSize);
		SET_ELEMENT(SList, 2, Capacity);
		SET_ELEMENT(SList, 3, nFragment);
		SET_ELEMENT(SList, 4, NameList);

		// Used stream info
		for (int i=0; i < n-1; i++)
		{
			INTEGER(IDList)[i] = BL[i]->ID();
			REAL(TotalSize)[i] = BL[i]->Size();
			REAL(Capacity)[i] = BL[i]->Capacity();
			INTEGER(nFragment)[i] = BL[i]->ListCount();
			SET_STRING_ELT(NameList, i,
				mkChar(diag_MapID[BL[i]->ID()].c_str()));
		}

		// Unused stream info
		{
			const CoreArray::CdBlockStream::TBlockInfo *s =
				file->UnusedBlock();
			int Cnt = 0;
			SIZE64 Size = 0;
			while (s) { Cnt++; Size += s->BlockSize; s = s->Next; }

			INTEGER(IDList)[n-1] = NA_INTEGER;
			REAL(TotalSize)[n-1] = Size;
			REAL(Capacity)[n-1] = Size;
			INTEGER(nFragment)[n-1] = Cnt;
			SET_STRING_ELT(NameList, n-1, mkChar("$unused$"));
		}

		// ===============================================================
		// Stream List

		n = tmp->Log().List().size();
		SEXP LogList = PROTECT(NEW_CHARACTER(n));
		nProtected ++;
		SET_ELEMENT(rv_ans, 1, LogList);

		for (int i=0; i < n; i++)
		{
			SET_STRING_ELT(LogList, i,
				mkChar(tmp->Log().List()[i].Msg.c_str()));
		}


		UNPROTECT(nProtected);

	COREARRAY_CATCH
}



// ----------------------------------------------------------------------------
// File Structure Operations
// ----------------------------------------------------------------------------

/// detect whether a node is valid
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeValid(SEXP Node)
{
	COREARRAY_TRY
		GDS_R_NodeValid(GDS_R_SEXP2Obj(Node), TRUE);
	COREARRAY_CATCH
}


/// get the number of variables in a folder
/** \param Node        [in] a GDS node
 *  \param Hidden      [in] whether include hidden variable(s)
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeChildCnt(SEXP Node, SEXP Hidden)
{
	int include_hidden = Rf_asLogical(Hidden);
	if (include_hidden == NA_LOGICAL)
		error("'include.hidden' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

		int Cnt = 0;
		if (dynamic_cast<CdGDSFolder*>(Obj))
		{
			CdGDSFolder *Dir = static_cast<CdGDSFolder*>(Obj);
			if (include_hidden)
			{
				Cnt = Dir->NodeCount();
			} else {
				for (int i=0; i < Dir->NodeCount(); i++)
				{
					CdGDSObj *Obj = Dir->ObjItemEx(i);
					if (Obj)
					{
						if (!Obj->Attribute().HasName(ASC16("R.invisible")))
							Cnt ++;
					}
				}
			}
		} else if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
		{
			CdGDSVirtualFolder *Dir = static_cast<CdGDSVirtualFolder*>(Obj);
			if (Dir->IsLoaded(true))
			{
				Cnt = Dir->NodeCount();
			} else {
				for (int i=0; i < Dir->NodeCount(); i++)
				{
					CdGDSObj *Obj = Dir->ObjItemEx(i);
					if (Obj)
					{
						if (!Obj->Attribute().HasName(ASC16("R.invisible")))
							Cnt ++;
					}
				}
			}
		}

		rv_ans = ScalarInteger(Cnt);

	COREARRAY_CATCH
}


/// get the name of a specified node
/** \param Node        [in] a GDS node
 *  \param FullName    [in] if TRUE, return the name with full path
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeName(SEXP Node, SEXP FullName)
{
	int full = Rf_asLogical(FullName);
	if (full == NA_LOGICAL)
		error("'fullname' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

		string nm;
		if (full == TRUE)
			nm = RawText(Obj->FullName());
		else
			nm = RawText(Obj->Name());
		rv_ans = mkStringUTF8(nm.c_str());

	COREARRAY_CATCH
}


/// enumerate the names of its child nodes
/** \param Node        [in] a GDS node
 *  \param Hidden      [in] whether include hidden variable(s)
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeEnumName(SEXP Node, SEXP Hidden)
{
	int include_hidden = Rf_asLogical(Hidden);
	if (include_hidden == NA_LOGICAL)
		error("'include.hidden' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

		CdGDSAbsFolder *Dir = dynamic_cast<CdGDSAbsFolder*>(Obj);
		if (Dir)
		{
			vector<string> List;
			for (int i=0; i < Dir->NodeCount(); i++)
			{
				CdGDSObj *Obj = Dir->ObjItemEx(i);
				if (Obj)
				{
					if (include_hidden)
					{
						List.push_back(RawText(Obj->Name()));
					} else {
						if (!Obj->Attribute().HasName(ASC16("R.invisible")))
							List.push_back(RawText(Obj->Name()));
					}
				}
			}

			PROTECT(rv_ans = NEW_STRING(List.size()));
			for (size_t i=0; i < List.size(); i++)
			{
				SET_STRING_ELT(rv_ans, i,
					mkCharCE(List[i].c_str(), CE_UTF8));
			}
			UNPROTECT(1);
		} else
			throw ErrGDSFmt(ERR_NOT_FOLDER);

	COREARRAY_CATCH
}


/// get the node with index or indices
/** \param Node        [in] a GDS node
 *  \param Path        [in] the full path of a specified node
 *  \param Index       [in] the index or indices of a specified node
 *  \param Silent      [in] return R_NilValue if fails and `Silent=TRUE'
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeIndex(SEXP Node, SEXP Path, SEXP Index,
	SEXP Silent)
{
	int silent_flag = Rf_asLogical(Silent);
	if (silent_flag == NA_LOGICAL)
		error("'silent' must be TRUE or FALSE.");
	int nProtected = 0;

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

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
				if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
				{
					string pn = RawText(Obj->FullName());
					if (pn.empty()) pn = "$ROOT$";
					throw ErrGDSFile("'%s' is not a folder.", pn.c_str());
				}
				CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);

				if (Rf_isInteger(Index))
				{
					int idx = INTEGER(Index)[i];
					if ((idx < 1) || (idx > Dir.NodeCount()))
					{
						string pn = RawText(Obj->FullName());
						if (pn.empty()) pn = "$ROOT$";
						throw ErrGDSFile("'%s' index[%d], out of range 1..%d.",
							pn.c_str(), idx, Dir.NodeCount());
					}
					Obj = Dir.ObjItem(idx - 1);
				} else if (Rf_isString(Index))
				{
					const char *nm = translateCharUTF8(STRING_ELT(Index, i));
					Obj = Dir.ObjItemEx(UTF16Text(nm));
					if (Obj == NULL)
					{
						string pn = RawText(Obj->FullName());
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

			if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			{
				string pn = RawText(Obj->FullName());
				if (pn.empty()) pn = "$ROOT$";
				throw ErrGDSFile("'%s' is not a folder.", pn.c_str());
			}

			CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
			const char *nm = translateCharUTF8(STRING_ELT(Path, 0));
			Obj = Dir.PathEx(UTF16Text(nm));
			if (!Obj && !silent_flag)
				throw ErrGDSObj("Invalid path \"%s\"!", nm);
		}

		if (Obj)
		{
			PROTECT(rv_ans = GDS_R_Obj2SEXP(Obj));
			nProtected ++;
		}

		UNPROTECT(nProtected);
		nProtected = 0;

	CORE_CATCH(rv_ans = R_NilValue; has_error = true);
	if (has_error)
	{
		if (silent_flag == TRUE)
		{
			if (nProtected > 0)
				UNPROTECT(nProtected);
		} else
			error(GDS_GetError());
	}

	return rv_ans;
}


/// get the description of a GDS node
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeObjDesp(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

		int nProtected = 0;
		SEXP tmp;
		PROTECT(rv_ans = NEW_LIST(14));
		nProtected ++;

			// 1: name
			SET_ELEMENT(rv_ans, 0,
				mkStringUTF8(RawText(Obj->Name()).c_str()));

			// 2: full name
			SET_ELEMENT(rv_ans, 1,
				mkStringUTF8(RawText(Obj->FullName()).c_str()));

			// 3: storage, the stream name of data field, such like "dInt32"
			SET_ELEMENT(rv_ans, 2, mkString(Obj->dName()));

			// 4: trait, the description of data field, such like "Int32"
			string s = Obj->dTraitName();
			if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
				s = RawText(((CdGDSVirtualFolder*)Obj)->LinkFileName());
			SET_ELEMENT(rv_ans, 3, mkString(s.c_str()));

			// 5: type (a factor)
			//   1  -- Label,    2 -- Folder,  3 -- VFolder,
			//   4  -- Raw,      5 -- Integer, 6 -- Factor,
			//   7  -- Logical,  8 -- Real,    9 -- String,
			//   10 -- Unknown
			int TypeInt;
			if (dynamic_cast<CdGDSLabel*>(Obj))
				TypeInt = 1;
			else if (dynamic_cast<CdGDSFolder*>(Obj))
				TypeInt = 2;
			else if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
				TypeInt = 3;
			else if (dynamic_cast<CdGDSStreamContainer*>(Obj))
				TypeInt = 4;
			else if (dynamic_cast<CdContainer*>(Obj))
			{
				CdContainer* nn = static_cast<CdContainer*>(Obj);
				C_SVType sv = nn->SVType();
				if (COREARRAY_SV_INTEGER(sv))
				{
					if (GDS_R_Is_Factor(Obj))
						TypeInt = 6;
					else if (GDS_R_Is_Logical(Obj))
						TypeInt = 7;
					else
						TypeInt = 5;
				} else if (COREARRAY_SV_FLOAT(sv))
					TypeInt = 8;
				else if (COREARRAY_SV_STRING(sv))
					TypeInt = 9;
				else
					TypeInt = 10;
			} else
				TypeInt = 10;
			SET_ELEMENT(rv_ans, 4, ScalarInteger(TypeInt));

			// 6: is.array
			SET_ELEMENT(rv_ans, 5,
				ScalarLogical(dynamic_cast<CdAbstractArray*>(Obj) ? TRUE : FALSE));

			// 7: dim, the dimension of data field
			// 8: encoder, the compression method: "", "ZIP"
			// 9: compress, the compression method: "", "ZIP.max"
			// 10: cpratio, data compression ratio, "NaN" indicates no compression
			if (dynamic_cast<CdAbstractArray*>(Obj))
			{
				CdAbstractArray *_Obj = (CdAbstractArray*)Obj;

				PROTECT(tmp = NEW_INTEGER(_Obj->DimCnt()));
				nProtected ++;
				SET_ELEMENT(rv_ans, 6, tmp);
				for (int i=0; i < _Obj->DimCnt(); i++)
					INTEGER(tmp)[i] = _Obj->GetDLen(_Obj->DimCnt()-i-1);

				SEXP encoder, coder, ratio;
				PROTECT(encoder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 7, encoder);
				SET_STRING_ELT(encoder, 0, mkChar(""));
				PROTECT(coder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 8, coder);
				SET_STRING_ELT(coder, 0, mkChar(""));

				PROTECT(ratio = NEW_NUMERIC(1));
				nProtected ++;
				SET_ELEMENT(rv_ans, 9, ratio);
				REAL(ratio)[0] = R_NaN;

				if (_Obj->PipeInfo())
				{
					SET_STRING_ELT(encoder, 0,
						mkCharCE(_Obj->PipeInfo()->Coder(), CE_UTF8));
					SET_STRING_ELT(coder, 0,
						mkCharCE(_Obj->PipeInfo()->CoderParam().c_str(),
						CE_UTF8));
					if (_Obj->PipeInfo()->StreamTotalIn() > 0)
					{
						REAL(ratio)[0] = (double)
							_Obj->PipeInfo()->StreamTotalOut() /
							_Obj->PipeInfo()->StreamTotalIn();
					}
				}
			} else if (dynamic_cast<CdGDSStreamContainer*>(Obj))
			{
				CdGDSStreamContainer *_Obj = (CdGDSStreamContainer*)Obj;

				PROTECT(tmp = NEW_NUMERIC(1));
				nProtected ++;
				SET_ELEMENT(rv_ans, 6, tmp);

				SEXP encoder, coder, ratio;
				PROTECT(encoder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 7, encoder);
				SET_STRING_ELT(encoder, 0, mkChar(""));
				PROTECT(coder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 8, coder);
				SET_STRING_ELT(coder, 0, mkChar(""));

				PROTECT(ratio = NEW_NUMERIC(1));
				nProtected ++;
				SET_ELEMENT(rv_ans, 9, ratio);
				REAL(ratio)[0] = R_NaN;

				if (_Obj->PipeInfo())
				{
					SET_STRING_ELT(encoder, 0,
						mkCharCE(_Obj->PipeInfo()->Coder(), CE_UTF8));
					SET_STRING_ELT(coder, 0,
						mkCharCE(_Obj->PipeInfo()->CoderParam().c_str(),
						CE_UTF8));
					if (_Obj->PipeInfo()->StreamTotalIn() > 0)
					{
						REAL(ratio)[0] = (double)
							_Obj->PipeInfo()->StreamTotalOut() /
							_Obj->PipeInfo()->StreamTotalIn();
					}
					REAL(tmp)[0] = _Obj->PipeInfo()->StreamTotalIn();
				} else
					REAL(tmp)[0] = _Obj->GetSize();
			}

			// 11: size
			double Size;
			if (dynamic_cast<CdContainer*>(Obj))
			{
				CdContainer* p = static_cast<CdContainer*>(Obj);
				p->Synchronize();
				Size = p->GDSStreamSize();
			} else {
				Size = R_NaN;
			}
			SET_ELEMENT(rv_ans, 10, ScalarReal(Size));

			// 12: good
			int GoodFlag;
			if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
			{
				CdGDSVirtualFolder *v = (CdGDSVirtualFolder*)Obj;
				GoodFlag = v->IsLoaded(true) ? TRUE : FALSE;
			} else if (dynamic_cast<CdGDSUnknown*>(Obj))
			{
				GoodFlag = FALSE;
			} else {
				GoodFlag = TRUE;
			}
			SET_ELEMENT(rv_ans, 11, ScalarLogical(GoodFlag));

			// 13: message
			PROTECT(tmp = NEW_STRING(1));
			nProtected ++;
			SET_ELEMENT(rv_ans, 12, tmp);
			if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
			{
				CdGDSVirtualFolder *v = (CdGDSVirtualFolder*)Obj;
				v->IsLoaded(true);
				SET_STRING_ELT(tmp, 0, mkChar(v->ErrMsg().c_str()));
			} else
				SET_STRING_ELT(tmp, 0, mkChar(""));

			// 14: param
			tmp = R_NilValue;
			if (dynamic_cast<CdPackedReal8*>(Obj) ||
				dynamic_cast<CdPackedReal16*>(Obj) ||
				dynamic_cast<CdPackedReal32*>(Obj))
			{
				PROTECT(tmp = NEW_LIST(2));
				SEXP nm = PROTECT(NEW_STRING(2));
				nProtected += 2;
				SET_STRING_ELT(nm, 0, mkChar("offset"));
				SET_STRING_ELT(nm, 1, mkChar("scale"));
				SET_NAMES(tmp, nm);

				if (dynamic_cast<CdPackedReal8*>(Obj))
				{
					CdPackedReal8 *v = static_cast<CdPackedReal8*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else if (dynamic_cast<CdPackedReal16*>(Obj))
				{
					CdPackedReal16 *v = static_cast<CdPackedReal16*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else {
					CdPackedReal32 *v = static_cast<CdPackedReal32*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				}
			} else if (dynamic_cast<CdFStr8*>(Obj) ||
				dynamic_cast<CdFStr16*>(Obj) ||
				dynamic_cast<CdFStr32*>(Obj))
			{
				PROTECT(tmp = NEW_LIST(1));
				SEXP nm = PROTECT(NEW_STRING(1));
				nProtected += 2;
				SET_STRING_ELT(nm, 0, mkChar("maxlen"));
				SET_NAMES(tmp, nm);

				if (dynamic_cast<CdFStr8*>(Obj))
				{
					SET_ELEMENT(tmp, 0, ScalarInteger(
						dynamic_cast<CdFStr8*>(Obj)->MaxLength()));
				} else if (dynamic_cast<CdFStr16*>(Obj))
				{
					SET_ELEMENT(tmp, 0, ScalarInteger(
						dynamic_cast<CdFStr16*>(Obj)->MaxLength()));
				} else {
					SET_ELEMENT(tmp, 0, ScalarInteger(
						dynamic_cast<CdFStr32*>(Obj)->MaxLength()));
				}
			}
			SET_ELEMENT(rv_ans, 13, tmp);

		UNPROTECT(nProtected);

	COREARRAY_CATCH
}


/// add a new node
/** \param Node        [in] a GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param Val         [in] the values
 *  \param Storage     [in] the mode of storage
 *  \param ValDim      [in] the dimension
 *  \param Compress    [in] the method of compression
 *  \param CloseZip    [in] if compressing data and TRUE, get into read mode after adding
 *  \param Check       [in] if TRUE, check data compatibility
 *  \param Replace     [in] if TRUE, replace the existing variable silently
 *  \param Param       [in] list(...), additional parameters
**/
COREARRAY_DLL_EXPORT SEXP gdsAddNode(SEXP Node, SEXP NodeName, SEXP Val,
	SEXP Storage, SEXP ValDim, SEXP Compress, SEXP CloseZip, SEXP Check,
	SEXP Replace, SEXP Param)
{
	static const char *ErrUnused =
		"Unused additional parameters (...) in 'add.gdsn'!";
	static const char *FixedString[] =
	{
		TdTraits< FIXED_LENGTH<C_UTF8>  >::StreamName(),
		TdTraits< FIXED_LENGTH<C_UTF16> >::StreamName(),
		TdTraits< FIXED_LENGTH<C_UTF32> >::StreamName(),
		NULL
	};
	static const char *PackedReal[] =
	{
		TdTraits< TREAL8  >::StreamName(),
		TdTraits< TREAL16 >::StreamName(),
		TdTraits< TREAL32 >::StreamName(),
		NULL
	};

	const char *nm  = translateCharUTF8(STRING_ELT(NodeName, 0));
	const char *stm = CHAR(STRING_ELT(Storage,  0));
	const char *cp  = CHAR(STRING_ELT(Compress, 0));
	if (!Rf_isNull(ValDim) && !Rf_isNumeric(ValDim))
		error("`valdim' should be NULL or a numeric vector");

	/// fixed-length string
	int FixStr_Len = 0;
	/// packed real number
	double FixedReal_Offset = R_NaN, FixedReal_Scale = R_NaN;

	map<const char*, const char*, CInitNameObject::strCmp>::iterator it;
	it = Init.ClassMap.find(stm);
	if (it != Init.ClassMap.end()) stm = it->second;

	if (IsElement(stm, FixedString))
	{
		// fixed-length characters
		SEXP val = GetListElement(Param, "maxlen");
		if (!Rf_isNull(val))
		{
			FixStr_Len = Rf_asInteger(val);
			if ((FixStr_Len==NA_INTEGER) || (FixStr_Len <= 0))
				error("'maxlen' should be a positive integer.");
			if (XLENGTH(Param) > 1) error(ErrUnused);
		} else {
			if (XLENGTH(Param) > 0) error(ErrUnused);
		}
	} else if (IsElement(stm, PackedReal))
	{
		// packed real number
		SEXP v1 = GetListElement(Param, "offset");
		SEXP v2 = GetListElement(Param, "scale");
		if (!Rf_isNull(v1) || !Rf_isNull(v2))
		{
			int n = 2;
			if (!Rf_isNull(v1))
			{
				FixedReal_Offset = Rf_asReal(v1);
				if (!R_FINITE(FixedReal_Offset))
					error("'offset' should not be NaN.");
			} else
				n--;
			if (!Rf_isNull(v2))
			{
				FixedReal_Scale = Rf_asReal(v2);
				if (!R_FINITE(FixedReal_Scale))
					error("'scale' should not be NaN.");
			} else
				n--;
			if (XLENGTH(Param) > n) error(ErrUnused);
		} else {
			if (XLENGTH(Param) > 0) error(ErrUnused);
		}
	} else {
		if (!Rf_isNull(Param))
		{
			if (XLENGTH(Param) > 0) error(ErrUnused);
		}
	}


	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);
		if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			throw ErrGDSFmt(ERR_NOT_FOLDER);

		CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
		int IdxReplace = -1;

		if (Rf_asLogical(Replace) == TRUE)
		{
			CdGDSObj *tmp = Dir.ObjItemEx(UTF16Text(nm));
			if (tmp)
			{
				IdxReplace = Dir.IndexObj(tmp);
				Dir.DeleteObj(tmp, true);
			}
		}

		// output GDS object
		PdGDSObj rv_obj = NULL;

		// class name mapping
		if (strcmp(stm, "NULL") != 0)
		{
			map<const char*, const char*, CInitNameObject::strCmp>::iterator it;
			it = Init.ClassMap.find(stm);
			if (it != Init.ClassMap.end()) stm = it->second;

			if (strcmp(stm, "$FOLDER$") != 0)
			{
				CdObjClassMgr::TdOnObjCreate OnCreate =
					dObjManager().NameToClass(stm);
				if (OnCreate)
				{
					CdObject *obj = (*OnCreate)();
					if (dynamic_cast<CdGDSObj*>(obj))
						rv_obj = static_cast<CdGDSObj*>(obj);
					else
						delete obj;
				}
			} else
				rv_obj = new CdGDSFolder();
		} else
			rv_obj = new CdGDSLabel();

		// check error
		if (rv_obj == NULL)
			throw ErrGDSFmt("Not support the storage mode '%s'.", stm);
		Dir.InsertObj(IdxReplace, UTF16Text(nm), rv_obj);
		if (dynamic_cast<CdGDSObjPipe*>(rv_obj))
			static_cast<CdGDSObjPipe*>(rv_obj)->SetPackedMode(cp);

		// output value
		rv_ans = GDS_R_Obj2SEXP(rv_obj);
		PROTECT(rv_ans);

		// if it is an array
		if (dynamic_cast<CdAbstractArray*>(rv_obj))
		{
			// set the compression mode
			CdAbstractArray *Obj = static_cast<CdAbstractArray*>(rv_obj);

			if (!Rf_isNull(Val))
			{
				if (Rf_isNumeric(Val) || Rf_isString(Val) ||
					Rf_isLogical(Val) || Rf_isFactor(Val) ||
					(TYPEOF(Val) == RAWSXP))
				{
					// check first
					if ((dynamic_cast<CdFStr8*>(rv_obj) ||
						dynamic_cast<CdFStr16*>(rv_obj) ||
						dynamic_cast<CdFStr32*>(rv_obj)) && Rf_isString(Val))
					{
						int MaxLen = FixStr_Len;
						R_xlen_t len = XLENGTH(Val);
						for (R_xlen_t i=0; i < len; i++)
						{
							const char *s = translateCharUTF8(STRING_ELT(Val, i));
							int L = strlen(s);
							if (L > MaxLen) MaxLen = L;
						}

						if (dynamic_cast<CdFStr8*>(rv_obj))
							static_cast<CdFStr8*>(rv_obj)->SetMaxLength(MaxLen);
						else if (dynamic_cast<CdFStr16*>(rv_obj))
							static_cast<CdFStr16*>(rv_obj)->SetMaxLength(MaxLen);
						else if (dynamic_cast<CdFStr32*>(rv_obj))
							static_cast<CdFStr32*>(rv_obj)->SetMaxLength(MaxLen);

					} else if (dynamic_cast<CdPackedReal8*>(rv_obj))
					{
						CdPackedReal8 *obj = static_cast<CdPackedReal8*>(rv_obj);
						if (R_FINITE(FixedReal_Offset))
							obj->SetOffset(FixedReal_Offset);
						if (R_FINITE(FixedReal_Scale))
							obj->SetScale(FixedReal_Scale);
					} else if (dynamic_cast<CdPackedReal16*>(rv_obj))
					{
						CdPackedReal16 *obj = static_cast<CdPackedReal16*>(rv_obj);
						if (R_FINITE(FixedReal_Offset))
							obj->SetOffset(FixedReal_Offset);
						if (R_FINITE(FixedReal_Scale))
							obj->SetScale(FixedReal_Scale);
					} else if (dynamic_cast<CdPackedReal32*>(rv_obj))
					{
						CdPackedReal32 *obj = static_cast<CdPackedReal32*>(rv_obj);
						if (R_FINITE(FixedReal_Offset))
							obj->SetOffset(FixedReal_Offset);
						if (R_FINITE(FixedReal_Scale))
							obj->SetScale(FixedReal_Scale);
					}

					// call write all
					gdsObjWriteAll(rv_ans, Val, Check);

					// close the compression
					if (Obj->PipeInfo() && (Rf_asLogical(CloseZip)==TRUE))
						Obj->CloseWriter();

					// set dimensions
					if (!Rf_isNull(ValDim))
						gdsObjSetDim(rv_ans, ValDim, ScalarLogical(FALSE));
				} else
					throw ErrGDSFmt("Not support `val'.");
			} else {
				// set dimensions
				if (Rf_isNull(ValDim))
					ValDim = ScalarInteger(0);
				gdsObjSetDim(rv_ans, ValDim, ScalarLogical(FALSE));
			}
		}

		UNPROTECT(1);

	COREARRAY_CATCH
}


/// add a new (virtual) folder
/** \param Node        [in] a GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param Type        [in] the type of folder
 *  \param GDS_fn      [in] the file name of an existing GDS file
 *  \param Replace     [in] if TRUE, replace the existing variable silently
**/
COREARRAY_DLL_EXPORT SEXP gdsAddFolder(SEXP Node, SEXP NodeName, SEXP Type,
	SEXP GDS_fn, SEXP Replace)
{
	const char *nm = translateCharUTF8(STRING_ELT(NodeName, 0));
	const char *tp = CHAR(STRING_ELT(Type, 0));
	const char *fn = NULL;
	if (strcmp(tp, "virtual") == 0)
		fn = CHAR(STRING_ELT(GDS_fn, 0));

	int replace_flag = Rf_asLogical(Replace);
	if (replace_flag == NA_LOGICAL)
		error("'replace' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);
		if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			throw ErrGDSFmt(ERR_NOT_FOLDER);

		CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
		int IdxReplace = -1;
		if (replace_flag)
		{
			CdGDSObj *tmp = Dir.ObjItemEx(UTF16Text(nm));
			if (tmp)
			{
				IdxReplace = Dir.IndexObj(tmp);
				Dir.DeleteObj(tmp, true);
			}
		}

		PdGDSObj vObj = NULL;
		if (strcmp(tp, "directory") == 0)
		{
			vObj = Dir.AddFolder(UTF16Text(nm));
		} else if (strcmp(tp, "virtual") == 0)
		{
			CdGDSVirtualFolder *F = new CdGDSVirtualFolder;
			Dir.InsertObj(IdxReplace, UTF16Text(nm), F);
			F->SetLinkFile(UTF8Text(fn));
			vObj = F;
		} else
			throw ErrGDSFmt("Invalid 'type = %s'.", tp);

		rv_ans = GDS_R_Obj2SEXP(vObj);

	COREARRAY_CATCH
}


/// add a new node with a GDS file
/** \param Node        [in] a GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param FileName    [in] the name of input file
 *  \param Compress    [in] the method of compression
 *  \param Replace     [in] if TRUE, replace the existing variable silently
**/
COREARRAY_DLL_EXPORT SEXP gdsAddFile(SEXP Node, SEXP NodeName, SEXP FileName,
	SEXP Compress, SEXP Replace)
{
	const char *nm = translateCharUTF8(STRING_ELT(NodeName, 0));
	const char *fn = CHAR(STRING_ELT(FileName, 0));
	const char *cp = CHAR(STRING_ELT(Compress, 0));

	int replace_flag = Rf_asLogical(Replace);
	if (replace_flag == NA_LOGICAL)
		error("'replace' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);
		if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			throw ErrGDSFmt(ERR_NOT_FOLDER);

		CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
		int IdxReplace = -1;
		if (replace_flag)
		{
			CdGDSObj *tmp = Dir.ObjItemEx(UTF16Text(nm));
			if (tmp)
			{
				IdxReplace = Dir.IndexObj(tmp);
				Dir.DeleteObj(tmp, true);
			}
		}

		TdAutoRef<CdBufStream> file(new CdBufStream(
			new CdFileStream(fn, CdFileStream::fmOpenRead)));
		CdGDSStreamContainer *vObj = new CdGDSStreamContainer();
		vObj->SetPackedMode(cp);
		Dir.InsertObj(IdxReplace, UTF16Text(nm), vObj);
		vObj->CopyFrom(*file.get());
		vObj->CloseWriter();

		rv_ans = GDS_R_Obj2SEXP(vObj);

	COREARRAY_CATCH
}


/// get the file from a file node
/** \param Node        [in] a GDS node
 *  \param OutFileName [in] the name for output file
**/
COREARRAY_DLL_EXPORT SEXP gdsGetFile(SEXP Node, SEXP OutFileName)
{
	const char *fn = CHAR(STRING_ELT(OutFileName, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);
		if (!dynamic_cast<CdGDSStreamContainer*>(Obj))
			throw ErrGDSFmt("It is not a stream container!");

		CdGDSStreamContainer *_Obj = static_cast<CdGDSStreamContainer*>(Obj);
		TdAutoRef<CdBufStream> file(new CdBufStream(
			new CdFileStream(fn, CdFileStream::fmCreate)));
		_Obj->CopyTo(*file.get());

	COREARRAY_CATCH
}


/// delete a node
/** \param Node        [in] a GDS node
 *  \param Force       [in] if TRUE, remove a folder no matter whether it is empty
**/
COREARRAY_DLL_EXPORT SEXP gdsDeleteNode(SEXP Node, SEXP Force)
{
	int force_flag = Rf_asLogical(Force);
	if (force_flag == NA_LOGICAL)
		error("'force' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);
		if (Obj->Folder())
			Obj->Folder()->DeleteObj(Obj, force_flag);
		else
			throw ErrGDSFmt("Can not delete the root.");

	COREARRAY_CATCH
}


/// rename a node
/** \param Node        [in] a GDS node
 *  \param NewName     [in] the new name
**/
COREARRAY_DLL_EXPORT SEXP gdsRenameNode(SEXP Node, SEXP NewName)
{
	const char *nm = translateCharUTF8(STRING_ELT(NewName, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);
		Obj->SetName(UTF16Text(nm));

	COREARRAY_CATCH
}




// ----------------------------------------------------------------------------
// Attribute Operations
// ----------------------------------------------------------------------------

/// get the attribute(s) of a GDS node
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsGetAttr(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

		if (Obj->Attribute().Count() > 0)
		{
			int nProtected = 0;
			PROTECT(rv_ans = NEW_LIST(Obj->Attribute().Count()));
			nProtected ++;

			// the values
			for (int i=0; i < (int)Obj->Attribute().Count(); i++)
			{
				const CdAny *p = &(Obj->Attribute()[i]);
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
						{
							SET_STRING_ELT(tmp, i,
								mkCharCE(RawText(p->GetStr8()).c_str(),
								CE_UTF8));
						}
					} else if (p->IsBool())
					{
						PROTECT(tmp = NEW_LOGICAL(Cnt));
						nProtected ++;
						for (R_xlen_t i=0; i < Cnt; i++, p++)
							LOGICAL(tmp)[i] = p->GetBool() ? TRUE : FALSE;
					}
				}
				SET_ELEMENT(rv_ans, i, tmp);
			}

			// the values
			SEXP nlist;
			PROTECT(nlist = NEW_STRING(Obj->Attribute().Count()));
			nProtected ++;
			for (int i=0; i < (int)Obj->Attribute().Count(); i++)
			{
				SET_STRING_ELT(nlist, i, mkCharCE(
					RawText(Obj->Attribute().Names(i)).c_str(), CE_UTF8));
			}
			SET_NAMES(rv_ans, nlist);

			UNPROTECT(nProtected);
		}

	COREARRAY_CATCH
}


/// set an attribute
/** \param Node        [in] a GDS node
 *  \param Name        [in] the name of attribute
 *  \param Val         [in] the value
**/
COREARRAY_DLL_EXPORT SEXP gdsPutAttr(SEXP Node, SEXP Name, SEXP Val)
{
	const char *nm = translateCharUTF8(STRING_ELT(Name, 0));
	if (!Rf_isNull(Val) && !Rf_isInteger(Val) && !Rf_isReal(Val) &&
			!Rf_isString(Val) && !Rf_isLogical(Val))
		error("Unsupported type!");
	if (!Rf_isNull(Val) && (Rf_length(Val) <= 0))
		error("The length of values should be > 0.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);

		CdAny *p;
		if (Obj->Attribute().HasName(UTF16Text(nm)))
			p = &(Obj->Attribute()[UTF16Text(nm)]);
		else
			p = &(Obj->Attribute().Add(UTF16Text(nm)));

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
				SEXP s = STRING_ELT(Val, 0);
				if (s == NA_STRING)
					warning("Missing character is converted to \"NA\".");
				p->SetStr8(UTF8Text(translateCharUTF8(s)));
			} else {
				bool warn = true;
				p->SetArray(Rf_length(Val));
				for (R_xlen_t i=0; i < Rf_length(Val); i++)
				{
					SEXP s = STRING_ELT(Val, i);
					if ((s == NA_STRING) && warn)
					{
						warning("Missing characters are converted to \"NA\".");
						warn = false;
					}
					p->GetArray()[i].SetStr8(UTF8Text(translateCharUTF8(s)));
				}
			}
		} else if (Rf_isLogical(Val))
		{
			if (Rf_length(Val) == 1)
			{
				p->SetBool(LOGICAL(Val)[0] == TRUE);
			} else {
				p->SetArray(Rf_length(Val));
				for (R_xlen_t i=0; i < Rf_length(Val); i++)
					p->GetArray()[i].SetBool(LOGICAL(Val)[i] == TRUE);
			}
		}

	COREARRAY_CATCH
}


/// delete an attribute
/** \param Node        [in] a GDS node
 *  \param Name        [in] the name of attribute
**/
COREARRAY_DLL_EXPORT SEXP gdsDeleteAttr(SEXP Node, SEXP Name)
{
	const char *nm = translateCharUTF8(STRING_ELT(Name, 0));
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);
		Obj->Attribute().Delete(UTF16Text(nm));

	COREARRAY_CATCH
}




// ----------------------------------------------------------------------------
// Data Operations
// ----------------------------------------------------------------------------

/// read data from a node
/** \param Node        [in] a GDS node
 *  \param Start       [in] the starting position
 *  \param Count       [in] the count of each dimension
 *  \param Simplify    [in] if TRUE, convert to a vector if possible
 *  \param UseRaw      [in] if TRUE, use RAW if possible
**/
COREARRAY_DLL_EXPORT SEXP gdsObjReadData(SEXP Node, SEXP Start, SEXP Count,
	SEXP Simplify, SEXP UseRaw)
{
	if (!Rf_isNull(Start) && !Rf_isNumeric(Start))
		error("`start' should be numeric.");
	if (!Rf_isNull(Count) && !Rf_isNumeric(Count))
		error("`count' should be numeric.");
	if ((Rf_isNull(Start) && !Rf_isNull(Count)) ||
			(!Rf_isNull(Start) && Rf_isNull(Count)))
		error("`start' and 'count' should be both NULL.");

	// "auto", "none", "force"
	const char *simplify_text = CHAR(STRING_ELT(Simplify, 0));

	int use_raw_flag = Rf_asLogical(UseRaw);
	if (use_raw_flag == NA_LOGICAL)
		error("'useraw' must be TRUE or FALSE.");

	// check
	GDS_R_NodeValid_SEXP(Node, TRUE);
	// GDS object
	CdAbstractArray *Obj =
		dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node));
	if (Obj == NULL)
		error("There is no data field.");

	CdAbstractArray::TArrayDim DStart, DLen;
	C_Int32 *pDS=NULL, *pDL=NULL;
	if (!Rf_isNull(Start) && !Rf_isNull(Count))
	{
		int Len = Obj->DimCnt();
		CdAbstractArray::TArrayDim DCnt;
		Obj->GetDim(DCnt);

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

	// read data
	COREARRAY_TRY
		rv_ans = GDS_R_Array_Read(Obj, pDS, pDL, NULL,
			(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
	CORE_CATCH(has_error = true);
	if (has_error) error(GDS_GetError());

	// to simplify
	if (strcmp(simplify_text, "auto") == 0)
	{
		PROTECT(rv_ans);
		SEXP dim = getAttrib(rv_ans, R_DimSymbol);
		if (!Rf_isNull(dim))
		{
			int Num_GreaterOne = 0;
			for (int i=0; i < XLENGTH(dim); i++)
			{
				if (INTEGER(dim)[i] > 1)
					Num_GreaterOne ++;
			}
			if (Num_GreaterOne <= 1)
				setAttrib(rv_ans, R_DimSymbol, R_NilValue);
		}
		UNPROTECT(1);
	} else if (strcmp(simplify_text, "force") == 0)
	{
		PROTECT(rv_ans);
		setAttrib(rv_ans, R_DimSymbol, R_NilValue);
		UNPROTECT(1);
	}

	return rv_ans;
}


/// read data from a node with a selection
/** \param Node        [in] a GDS node
 *  \param Selection   [in] the logical variable of selection
 *  \param Simplify    [in] if TRUE, convert to a vector if possible
 *  \param UseRaw      [in] if TRUE, use RAW if possible
**/
COREARRAY_DLL_EXPORT SEXP gdsObjReadExData(SEXP Node, SEXP Selection,
	SEXP Simplify, SEXP UseRaw)
{
	// "auto", "none", "force"
	const char *simplify_text = CHAR(STRING_ELT(Simplify, 0));

	int use_raw_flag = Rf_asLogical(UseRaw);
	if (use_raw_flag == NA_LOGICAL)
		error("'useraw' must be TRUE or FALSE.");

	COREARRAY_TRY

		// check
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

		// GDS object
		CdAbstractArray *_Obj = dynamic_cast<CdAbstractArray*>(Obj);
		if (_Obj == NULL)
			throw ErrGDSFmt("There is no data field.");

		vector< vector<C_BOOL> > Select;

		if (Rf_isVectorList(Selection))
		{
			if (XLENGTH(Selection) != _Obj->DimCnt())
				throw ErrGDSFmt("The dimension of `sel' is not correct.");

			Select.resize(_Obj->DimCnt());
			for (int i=0; i < XLENGTH(Selection); i++)
			{
				SEXP tmp = VECTOR_ELT(Selection, i);
				if (Rf_isLogical(tmp))
				{
					int k = _Obj->DimCnt() - i - 1;
					R_xlen_t Len = _Obj->GetDLen(k);
					if (XLENGTH(tmp) != Len)
					{
						throw ErrGDSFmt(
							"The length of `sel[[%d]]' is not correct.", i+1);
					}
					vector<C_BOOL> &ss = Select[k];
					ss.resize(_Obj->GetDLen(k));
					for (R_xlen_t j=0; j < Len; j++)
						ss[j] = (LOGICAL(tmp)[j] == TRUE);
				} else {
					throw ErrGDSFmt(
						"`sel[[%d]]' should be a logical variable.", i+1);
				}
			}
		} else
			throw ErrGDSFmt("`sel' should be a list or a logical variable.");

		vector<C_BOOL*> SelList(_Obj->DimCnt());
		for (int i=0; i < _Obj->DimCnt(); i++)
			SelList[i] = &(Select[i][0]);

		// read data
		rv_ans = GDS_R_Array_Read(_Obj, NULL, NULL, &(SelList[0]),
			(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));

		// to simplify
		if (strcmp(simplify_text, "auto") == 0)
		{
			PROTECT(rv_ans);
			SEXP dim = getAttrib(rv_ans, R_DimSymbol);
			if (!Rf_isNull(dim))
			{
				int Num_GreaterOne = 0;
				for (int i=0; i < XLENGTH(dim); i++)
				{
					if (INTEGER(dim)[i] > 1)
						Num_GreaterOne ++;
				}
				if (Num_GreaterOne <= 1)
					setAttrib(rv_ans, R_DimSymbol, R_NilValue);
			}
			UNPROTECT(1);
		} else if (strcmp(simplify_text, "force") == 0)
		{
			PROTECT(rv_ans);
			setAttrib(rv_ans, R_DimSymbol, R_NilValue);
			UNPROTECT(1);
		}

	COREARRAY_CATCH
}


/// append data to a node
/** \param Node        [in] a GDS node
 *  \param Val         [in] the values
 *  \param Check       [in] if TRUE check
**/
COREARRAY_DLL_EXPORT SEXP gdsObjAppend(SEXP Node, SEXP Val, SEXP Check)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val) && (TYPEOF(Val)!=RAWSXP))
		error("`val' should be integer, numeric, character, logical or raw.");
	
	int check_flag = Rf_asLogical(Check);
	if (check_flag == NA_LOGICAL)
		error("'check' must be TRUE or FALSE.");

	COREARRAY_TRY

		// check
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);

		// GDS object
		CdAbstractArray *_Obj = dynamic_cast<CdAbstractArray*>(Obj);
		if (_Obj == NULL)
			throw ErrGDSFmt("There is no data field.");

		int nProtected = 0;
		C_SVType sv = _Obj->SVType();

		if (COREARRAY_SV_INTEGER(sv))
		{
			PROTECT(Val = Rf_coerceVector(Val, INTSXP));
			nProtected ++;
			_Obj->Append(INTEGER(Val), XLENGTH(Val), svInt32);
		} else if (COREARRAY_SV_FLOAT(sv))
		{
			PROTECT(Val = Rf_coerceVector(Val, REALSXP));
			nProtected ++;
			_Obj->Append(REAL(Val), XLENGTH(Val), svFloat64);
		} else if (COREARRAY_SV_STRING(sv))
		{
			PROTECT(Val = Rf_coerceVector(Val, STRSXP));
			nProtected ++;
			R_xlen_t Len = XLENGTH(Val);
			if (check_flag)
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
			vector<UTF8String> buf(Len);
			for (R_xlen_t i=0; i < Len; i++)
			{
				SEXP s = STRING_ELT(Val, i);
				if (s != NA_STRING)
					buf[i] = UTF8Text(translateCharUTF8(s));
			}
			_Obj->Append(&(buf[0]), Len, svStrUTF8);
		} else
			throw ErrGDSFmt("Not support!");

		UNPROTECT(nProtected);

		if (_Obj->PipeInfo())
			_Obj->PipeInfo()->UpdateStreamSize();

		if (check_flag)
		{
			if (dynamic_cast<CdAllocArray*>(Obj))
			{
				CdAllocArray *_Obj = static_cast<CdAllocArray*>(Obj);
				if (_Obj->TotalArrayCount() != _Obj->TotalCount())
					warning("Not a complete subset of data.");
			}
		}
	}
	catch (ErrAllocWrite &E) {
		GDS_SetError(ERR_READ_ONLY);
		has_error = true;

	COREARRAY_CATCH
}


/// append data of a GDS node to a node
/** \param Node        [in] a GDS node
 *  \param Val         [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsObjAppend2(SEXP Node, SEXP Val)
{
	COREARRAY_TRY

		PdGDSObj Dest = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Dest, FALSE);

		PdGDSObj Source = GDS_R_SEXP2Obj(Val);
		GDS_R_NodeValid(Source, TRUE);

		if (dynamic_cast<CdContainer*>(Dest))
		{
			static_cast<CdContainer*>(Dest)->AssignOneEx(*Source, true);
		} else
			throw ErrGDSFmt("Not support!");
	}
	catch (ErrAllocWrite &E) {
		GDS_SetError(ERR_READ_ONLY);
		has_error = true;

	COREARRAY_CATCH
}


/// write data to a node
/** \param Node        [in] a GDS node
 *  \param Val         [in] the input values
 *  \param Check
**/
COREARRAY_DLL_EXPORT SEXP gdsObjWriteAll(SEXP Node, SEXP Val, SEXP Check)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val) && (TYPEOF(Val)!=RAWSXP))
		error("`val' should be integer, numeric, character, logical or raw.");

	int check_flag = Rf_asLogical(Check);
	if (check_flag == NA_LOGICAL)
		error("'check' must be TRUE or FALSE.");

	// check
	GDS_R_NodeValid_SEXP(Node, FALSE);
	// GDS object
	CdAbstractArray *Obj = dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node));
	if (Obj == NULL)
		error("There is no data field.");

	int nProtected = 0;
	C_SVType ObjSV = Obj->SVType();

	if (COREARRAY_SV_INTEGER(ObjSV))
	{
		if ((TYPEOF(Val) != RAWSXP) && (TYPEOF(Val) != INTSXP))
		{
			PROTECT(Val = Rf_coerceVector(Val, INTSXP));
			nProtected ++;
		}
	} else if (COREARRAY_SV_FLOAT(ObjSV))
	{
		PROTECT(Val = Rf_coerceVector(Val, REALSXP));
		nProtected ++;
	} else if (COREARRAY_SV_STRING(ObjSV))
	{
		PROTECT(Val = Rf_coerceVector(Val, STRSXP));
		nProtected ++;
	} else
		error("Not support!");

	COREARRAY_TRY

		int DDimCnt;
		CdAbstractArray::TArrayDim DDim;
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

		DDim[0] = 0;
		Obj->ResetDim(DDim, DDimCnt);

		if (COREARRAY_SV_INTEGER(ObjSV))
		{
			if (TYPEOF(Val) != RAWSXP)
				Obj->Append(INTEGER(Val), XLENGTH(Val), svInt32);
			else
				Obj->Append(RAW(Val), XLENGTH(Val), svInt8);
		} else if (COREARRAY_SV_FLOAT(ObjSV))
		{
			Obj->Append(REAL(Val), XLENGTH(Val), svFloat64);
		} else if (COREARRAY_SV_STRING(ObjSV))
		{
			R_xlen_t Len = XLENGTH(Val);
			if (check_flag == TRUE)
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
			vector<UTF8String> buf(Len);
			for (R_xlen_t i=0; i < Len; i++)
			{
				SEXP s = STRING_ELT(Val, i);
				if (s != NA_STRING)
					buf[i] = UTF8Text(translateCharUTF8(s));
			}
			Obj->Append(&(buf[0]), Len, svStrUTF8);
		} else
			throw ErrGDSFmt("Not support!");

		UNPROTECT(nProtected);
	}
	catch (ErrAllocWrite &E) {
		GDS_SetError(ERR_READ_ONLY);
		has_error = true;

	COREARRAY_CATCH
}


/// write data to a node
/** \param Node        [in] a GDS node
 *  \param Val         [in] the values
 *  \param Start       [in] the starting positions
 *  \param Count       [in] the count
 *  \param Check       [in] if TRUE, check any(is.na(val)) if val is character
**/
COREARRAY_DLL_EXPORT SEXP gdsObjWriteData(SEXP Node, SEXP Val,
	SEXP Start, SEXP Count, SEXP Check)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val) && (TYPEOF(Val)!=RAWSXP))
		error("`val' should be integer, numeric, character, logical or raw.");
	if (!Rf_isNull(Start) && !Rf_isNumeric(Start))
		error("`start' should be numeric.");
	if (!Rf_isNull(Count) && !Rf_isNumeric(Count))
		error("`count' should be numeric.");
	if ((Rf_isNull(Start) && !Rf_isNull(Count)) ||
			(!Rf_isNull(Start) && Rf_isNull(Count)))
		error("`start' and 'count' should be both NULL.");
	if (!Rf_isLogical(Check) || (XLENGTH(Check) <= 0))
		error("`check' should be a logical variable.");

	// check
	GDS_R_NodeValid_SEXP(Node, FALSE);
	// GDS object
	CdAbstractArray *Obj = dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node));
	if (Obj == NULL)
		error("There is no data field.");

	CdAbstractArray::TArrayDim DStart, DLen;
	if (!Rf_isNull(Start) && !Rf_isNull(Count))
	{
		int Len = Obj->DimCnt();
		CdAbstractArray::TArrayDim DCnt;
		Obj->GetDim(DCnt);

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

		C_Int64 Cnt = 1;
		for (int i=0; i < Len; i++)
			Cnt *= DLen[i];
		if (Cnt != (C_Int64)Rf_length(Val))
			error("Invalid length of dimension of 'val'.");
	}

	COREARRAY_TRY

		int nProtected = 0;
		C_SVType ObjSV = Obj->SVType();

		if (COREARRAY_SV_INTEGER(ObjSV))
		{
			if (TYPEOF(Val) != RAWSXP)
			{
				PROTECT(Val = Rf_coerceVector(Val, INTSXP));
				nProtected ++;
				Obj->WriteData(DStart, DLen, INTEGER(Val), svInt32);
			} else {
				Obj->WriteData(DStart, DLen, RAW(Val), svInt8);
			}
		} else if (COREARRAY_SV_FLOAT(ObjSV))
		{
			PROTECT(Val = Rf_coerceVector(Val, REALSXP));
			nProtected ++;
			Obj->WriteData(DStart, DLen, REAL(Val), svFloat64);
		} else if (COREARRAY_SV_STRING(ObjSV))
		{
			PROTECT(Val = Rf_coerceVector(Val, STRSXP));
			nProtected ++;
			R_xlen_t Len = XLENGTH(Val);
			if (Rf_asLogical(Check) == TRUE)
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
			vector<UTF8String> buf(Len);
			for (R_xlen_t i=0; i < Len; i++)
			{
				SEXP s = STRING_ELT(Val, i);
				if (s != NA_STRING)
					buf[i] = UTF8Text(translateCharUTF8(s));
			}
			Obj->WriteData(DStart, DLen, &(buf[0]), svStrUTF8);
		} else
			throw ErrGDSFmt("Not support!");

		UNPROTECT(nProtected);
	}
	catch (ErrAllocWrite &E) {
		GDS_SetError(ERR_READ_ONLY);
		has_error = true;

	COREARRAY_CATCH
}


/// assign data to a GDS variable
/** \param dest_obj    [in] a GDS node
 *  \param src_obj     [in] the number of dimension
 *  \param append      [in] the starting positions
**/
COREARRAY_DLL_EXPORT SEXP gdsAssign(SEXP Dest, SEXP Src, SEXP Append)
{
	int append_flag = Rf_asLogical(Append);
	if (append_flag == NA_LOGICAL)
		error("'append' must be TRUE or FALSE.");

	COREARRAY_TRY

		// check
		PdGDSObj DstObj = GDS_R_SEXP2Obj(Dest);
		GDS_R_NodeValid(DstObj, FALSE);
		PdGDSObj SrcObj = GDS_R_SEXP2Obj(Src);
		GDS_R_NodeValid(SrcObj, TRUE);

		// assign
		if (dynamic_cast<CdContainer*>(DstObj))
		{
			static_cast<CdContainer*>(DstObj)->AssignOneEx(*SrcObj,
				append_flag);
		} else {
			DstObj->AssignOne(*SrcObj);
		}

	COREARRAY_CATCH
}


/// set the dimension of data to a node
/** \param Node        [in] a GDS node
 *  \param DLen        [in] the new sizes of dimension
 *  \param Permute     [in] if TRUE, rearrange the elements
**/
COREARRAY_DLL_EXPORT SEXP gdsObjSetDim(SEXP Node, SEXP DLen, SEXP Permute)
{
	// check
	GDS_R_NodeValid_SEXP(Node, FALSE);
	// GDS object
	CdAbstractArray *Obj = dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node));
	if (Obj == NULL)
		error("There is no data field.");
	// permute
	int permute_flag = Rf_asLogical(Permute);
	if (permute_flag == NA_INTEGER)
		error("'permute' must be TRUE or FALSE.");

	PROTECT(DLen = Rf_coerceVector(DLen, INTSXP));
	const size_t ndim = XLENGTH(DLen);
	if (ndim > 0)
	{
		for (size_t i=0; i < ndim-1; i++)
		{
			int v = INTEGER(DLen)[i];
			if (v == NA_INTEGER)
				error("Except the last entry, no NA is allowed in 'valdim'.");
			else if (v <= 0)
				error("Except the last entry, 'valdim[*]' should be > 0.");
		}
		int v = INTEGER(DLen)[ndim-1];
		if ((v != NA_INTEGER) && (v < 0))
			error("The last entry of 'valdim' should be >= 0.");
	} else
		error("The number of dimensions (valdim) should be > 0.");

	COREARRAY_TRY

		if (ndim <= 0)
		{
			throw ErrGDSFmt("length(valdim) should be > 0.");
		} else if (ndim > CdAbstractArray::MAX_ARRAY_DIM)
		{
			throw ErrGDSFmt(
				"The total number of dimensions should not be greater than %d.",
				CdAbstractArray::MAX_ARRAY_DIM);
		} else {
			CdAbstractArray::TArrayDim Dim;
			for (size_t i=0; i < ndim; i++)
			{
				int v = INTEGER(DLen)[ndim - i - 1];
				if ((v == NA_INTEGER) || (v < 0)) v = 0;
				Dim[i] = v;
			}

			if (permute_flag)
			{
				for (int i=ndim-1; i >= 0; i--)
					Obj->SetDLen(i, Dim[i]);
			} else {
				Obj->ResetDim(Dim, ndim);
			}
		}

		UNPROTECT(1);

	COREARRAY_CATCH
}


/// set a new compression mode
/** \param Node        [in] a GDS node
 *  \param Compress    [in] the compression mode
**/
COREARRAY_DLL_EXPORT SEXP gdsObjCompress(SEXP Node, SEXP Compress)
{
	const char *cp = CHAR(STRING_ELT(Compress, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);

		if (dynamic_cast<CdContainer*>(Obj))
		{
			static_cast<CdContainer*>(Obj)->SetPackedMode(cp);
		} else if (dynamic_cast<CdGDSStreamContainer*>(Obj))
		{
			static_cast<CdGDSStreamContainer*>(Obj)->SetPackedMode(cp);
		} else
			throw ErrGDSFmt("Not allow compression / decompression.");

	COREARRAY_CATCH
}


/// close the compression mode if possible
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsObjCompressClose(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);

		if (dynamic_cast<CdContainer*>(Obj))
		{
			static_cast<CdContainer*>(Obj)->CloseWriter();
		}

	COREARRAY_CATCH
}


/// Caching the data associated with a GDS variable
/** \param node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsCache(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, TRUE);

		if (dynamic_cast<CdContainer*>(Obj))
		{
			static_cast<CdContainer*>(Obj)->Caching();
		} else
			warning("The GDS node does not support caching.");

	COREARRAY_CATCH
}


/// Caching the data associated with a GDS variable
/** \param Node        [in] a GDS node
 *  \param NewNode     [in] the node of a new location
 *  \param RelPos      [in] 1: "after", 2: "before", 3: "replace"
**/
COREARRAY_DLL_EXPORT SEXP gdsMoveTo(SEXP Node, SEXP NewNode, SEXP RelPos)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(Obj, FALSE);

		PdGDSObj NObj = GDS_R_SEXP2Obj(NewNode);
		GDS_R_NodeValid(NObj, TRUE);

		if (Obj->Folder() == NObj->Folder())
		{
			int i_Obj  = Obj->Folder()->IndexObj(Obj);
			int i_NObj = NObj->Folder()->IndexObj(NObj);
			if (i_Obj != i_NObj)
			{
				int relpos = INTEGER(RelPos)[0];
				if ((relpos == 1) || (relpos == 3))
				{
					if (i_Obj <= i_NObj)
						Obj->Folder()->MoveTo(i_Obj, i_NObj);
					else
						Obj->Folder()->MoveTo(i_Obj, i_NObj+1);
				} else if (relpos == 2)
				{
					if (i_Obj <= i_NObj)
						Obj->Folder()->MoveTo(i_Obj, i_NObj-1);
					else
						Obj->Folder()->MoveTo(i_Obj, i_NObj);
				} else
					throw ErrGDSFmt("Invalid 'relpos'!");
			}
		} else {
			throw ErrGDSFmt(
				"'node' and 'new.loc.node' should be in the same folder.");
		}

	COREARRAY_CATCH
}


struct COREARRAY_DLL_LOCAL char_ptr_less
{
	bool operator ()(const char *s1, const char *s2) const
	{
		return (strcmp(s1, s2) < 0);
	}
};

/// Return a vector indicating whether the elements in a specified set
/** \param Node        [in] a GDS node
 *  \param SetEL       [in] a set of elements
**/
COREARRAY_DLL_EXPORT SEXP gdsIsElement(SEXP Node, SEXP SetEL)
{
	COREARRAY_TRY

		// GDS object
		PdGDSObj tmp = GDS_R_SEXP2Obj(Node);
		GDS_R_NodeValid(tmp, TRUE);
		CdAbstractArray *Obj = dynamic_cast<CdAbstractArray*>(tmp);
		if (Obj)
		{
			R_xlen_t Len = XLENGTH(SetEL);
			int nProtected = 0;
			set<int> SetInt;
			set<double> SetFloat;
			set<const char *, char_ptr_less> SetString;

			// check total number
			C_Int64 TotalCount = Obj->TotalCount();
			#ifndef R_XLEN_T_MAX
			if (TotalCount > TdTraits<R_xlen_t>::Max())
				throw ErrGDSFmt("No support of long vectors, please use 64-bit R with version >=3.0!");
			#endif

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

			// allocate memory
			PROTECT(rv_ans = NEW_LOGICAL(TotalCount));
			nProtected ++;

			// set values
			const int n_size = 4096;
			int *pL = LOGICAL(rv_ans);
			CdIterator it = Obj->IterBegin();
			
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
					{
						*pL = SetString.count(RawText(buffer[i]).c_str()) ?
							TRUE : FALSE;
					}
					TotalCount -= n;
				}
			}

			// set dimension
			if (Obj->DimCnt() > 1)
			{
				CdAbstractArray::TArrayDim DCnt;
				Obj->GetDim(DCnt);
				SEXP dim;
				PROTECT(dim = NEW_INTEGER(Obj->DimCnt()));
				nProtected ++;
				for (int i=0; i < Obj->DimCnt(); i++)
					INTEGER(dim)[Obj->DimCnt()-i-1] = DCnt[i];
				SET_DIM(rv_ans, dim);
			}

			UNPROTECT(nProtected);

		} else
			throw ErrGDSFmt("There is no data field.");

	COREARRAY_CATCH
}


/// get the last error message
COREARRAY_DLL_EXPORT SEXP gdsLastErrGDS()
{
	SEXP rv_ans = mkString(GDS_GetError());
	GDS_SetError(NULL);

	return rv_ans;
}

/// initialize the gds machine list
COREARRAY_DLL_EXPORT SEXP gdsSystem()
{
	COREARRAY_TRY

		int nProtect = 0;
		PROTECT(rv_ans = NEW_LIST(9));
		nProtect ++;
		SEXP nm = PROTECT(NEW_CHARACTER(9));
		nProtect ++;
		SET_NAMES(rv_ans, nm);

		// the number of logical cores
		SET_ELEMENT(rv_ans, 0, ScalarInteger(Mach::GetCPU_NumOfCores()));
		SET_STRING_ELT(nm, 0, mkChar("num.logical.core"));

		// memory cache
		string s;
		for (int i=0; i <= 4; i++)
		{
			C_UInt64 S = Mach::GetCPU_LevelCache(i);
			if (S < INT_MAX)
			{
				if (S <= 0) S = NA_INTEGER;
				SET_ELEMENT(rv_ans, i+1, ScalarInteger(S));
			} else {
				double ss = (S <= 0) ? R_NaN : (double)S;
				SET_ELEMENT(rv_ans, i+1, ScalarReal(ss));
			}
			switch (i)
			{
				case 0:  s = "l1i.cache.size"; break;
				case 1:  s = "l1d.cache.size"; break;
				default: s = Format("l%d.cache.size", i);
			}
			SET_STRING_ELT(nm, i+1, mkChar(s.c_str()));
		}

		// Compression encoder
		int n = dStreamPipeMgr.RegList().size();
		SEXP Encoder = PROTECT(NEW_CHARACTER(2*n));
		nProtect ++;
		SET_ELEMENT(rv_ans, 6, Encoder);
		SET_STRING_ELT(nm, 6, mkChar("compression.encoder"));
		for (int i=0; i < n; i++)
		{
			const CdPipeMgrItem *p = dStreamPipeMgr.RegList()[i];
			SET_STRING_ELT(Encoder, 2*i+0, mkChar(p->Coder()));
			SET_STRING_ELT(Encoder, 2*i+1, mkChar(p->Description()));
		}	


		// Compression encoder
		vector<string> ss;
	#ifdef COREARRAY_SIMD_SSE
		ss.push_back("SSE");
	#endif
	#ifdef COREARRAY_SIMD_SSE2
		ss.push_back("SSE2");
	#endif
	#ifdef COREARRAY_SIMD_SSE3
		ss.push_back("SSE3");
	#endif
	#ifdef COREARRAY_SIMD_SSE4_1
		ss.push_back("SSE4.1");
	#endif
	#ifdef COREARRAY_SIMD_SSE4_2
		ss.push_back("SSE4.2");
	#endif
	#ifdef COREARRAY_SIMD_AVX
		ss.push_back("AVX");
	#endif
	#ifdef COREARRAY_SIMD_AVX2
		ss.push_back("AVX2");
	#endif
	#ifdef COREARRAY_SIMD_FMA
		ss.push_back("FMA");
	#endif
	#ifdef COREARRAY_SIMD_FMA4
		ss.push_back("FMA4");
	#endif
		SEXP SIMD = PROTECT(NEW_CHARACTER(ss.size()));
		nProtect ++;
		SET_ELEMENT(rv_ans, 7, SIMD);
		SET_STRING_ELT(nm, 7, mkChar("compiler.flag"));
		for (int i=0; i < (int)ss.size(); i++)
			SET_STRING_ELT(SIMD, i, mkChar(ss[i].c_str()));

		// class list
		RegisterClass();
		vector<string> key, desp;
		dObjManager().ClassList(key, desp);
		SEXP Key = PROTECT(NEW_CHARACTER(key.size()));
		nProtect ++;
		SEXP Desp = PROTECT(NEW_CHARACTER(desp.size()));
		nProtect ++;
		SEXP CL = PROTECT(NEW_LIST(2));
		nProtect ++;
		SET_ELEMENT(rv_ans, 8, CL);
		SET_STRING_ELT(nm, 8, mkChar("class.list"));
		SET_ELEMENT(CL, 0, Key);
		SET_ELEMENT(CL, 1, Desp);
		for (int i=0; i < (int)key.size(); i++)
		{
			SET_STRING_ELT(Key, i, mkChar(key[i].c_str()));
			SET_STRING_ELT(Desp, i, mkChar(desp[i].c_str()));
		}

		UNPROTECT(nProtect);

	COREARRAY_CATCH
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

static int ApplyStartIndex = 0;

COREARRAY_DLL_EXPORT SEXP gdsApplySetStart(SEXP Idx)
{
	ApplyStartIndex = asInteger(Idx);
	return R_NilValue;
}


struct COREARRAY_DLL_LOCAL TApplyStruct
{
	SEXP R_Nodes;         ///< R SEXP objects
	SEXP R_Fun;           ///< R SEXP user-defined function
	SEXP R_Func_Call;     ///< R SEXP object for 'eval'
	SEXP R_Rho;           ///< the environment variable
	SEXP R_AccIdx;        ///< R SEXP accumulated index
	SEXP R_MarIdx;        ///< R SEXP marginal index
	SEXP RV_List;         ///< the returned SEXP object
	int DatType;          ///< 0: none, 1: list, 2: integer, 3: double,
	                      //   4: character, 5: logical, 6: raw, 7: gdsnode
	int AccIdx;           ///< the accumulated index
	CdAbstractArray **pTarget;  ///< target GDS node(s)
	int nTarget;                ///< the number of target GDS nodes
	int nProtected;       ///< nProtected used UNPROTECT
};

static void _apply_initfunc(SEXP Argument, C_Int32 Count,
	PdArrayRead ReadObj[], void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;

	if (p->R_AccIdx != NULL)
	{
		p->R_Func_Call = LCONS(p->R_Fun, LCONS(p->R_AccIdx,
			LCONS(Argument, LCONS(R_DotsSymbol, R_NilValue))));
	} else if (p->R_MarIdx != NULL)
	{
		p->R_Func_Call = LCONS(p->R_Fun, LCONS(p->R_MarIdx,
			LCONS(Argument, LCONS(R_DotsSymbol, R_NilValue))));
	} else {
		p->R_Func_Call = LCONS(p->R_Fun,
			LCONS(Argument, LCONS(R_DotsSymbol, R_NilValue)));
	}

	PROTECT(p->R_Func_Call);
	(p->nProtected) ++;

	// set name to Argument
	SET_NAMES(Argument, GET_NAMES(p->R_Nodes));

	// init return values
	// 0: none, 1: list, 2: integer, 3: double,
	// 4: character, 5: logical, 6: raw, 7: gdsnode
	switch (p->DatType)
	{
	case 1:
		PROTECT(p->RV_List = NEW_LIST(Count));
		(p->nProtected) ++;
		break;
	case 2:
		PROTECT(p->RV_List = NEW_INTEGER(Count));
		(p->nProtected) ++;
		break;
	case 3:
		PROTECT(p->RV_List = NEW_NUMERIC(Count));
		(p->nProtected) ++;
		break;
	case 4:
		PROTECT(p->RV_List = NEW_CHARACTER(Count));
		(p->nProtected) ++;
		break;
	case 5:
		PROTECT(p->RV_List = NEW_LOGICAL(Count));
		(p->nProtected) ++;
		break;
	case 6:
		PROTECT(p->RV_List = NEW_RAW(Count));
		(p->nProtected) ++;
		break;
	default:
		p->RV_List = R_NilValue;
	}
}

inline static void _apply_param_index(TApplyStruct *p, C_Int32 MarginIdx)
{
	// index
	if (p->R_AccIdx != NULL)
		INTEGER(p->R_AccIdx)[0] = p->AccIdx + ApplyStartIndex;
	if (p->R_MarIdx != NULL)
		INTEGER(p->R_MarIdx)[0] = MarginIdx + 1;
}

static void _apply_func_none(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	eval(p->R_Func_Call, p->R_Rho);

	p->AccIdx ++;
}

static void _apply_func_list(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	// the object is bound to other symbol(s), need a copy
	SET_VECTOR_ELT(p->RV_List, p->AccIdx, duplicate(val));

	p->AccIdx ++;
}

static void _apply_func_integer(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	INTEGER(p->RV_List)[p->AccIdx] = Rf_asInteger(val);
	p->AccIdx ++;
}

static void _apply_func_double(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	REAL(p->RV_List)[p->AccIdx] = Rf_asReal(val);
	p->AccIdx ++;
}

static void _apply_func_char(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	SET_STRING_ELT(p->RV_List, p->AccIdx, Rf_asChar(val));
	p->AccIdx ++;
}

static void _apply_func_logical(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	LOGICAL(p->RV_List)[p->AccIdx] = Rf_asLogical(val);
	p->AccIdx ++;
}

static void _apply_func_raw(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	RAW(p->RV_List)[p->AccIdx] = Rf_asInteger(val);
	p->AccIdx ++;
}


inline static void _apply_func_gds_append(CdAbstractArray *Obj, SEXP val)
{
	ssize_t xl = XLENGTH(val);

	if (isInteger(val))
	{
		Obj->Append(INTEGER(val), xl, svInt32);
	} else if (isReal(val))
	{
		Obj->Append(REAL(val), xl, svFloat64);
	} else if (isString(val))
	{
		PROTECT(val);
		const ssize_t SIZE = 256;
		UTF8String buf[SIZE];
		ssize_t idx = 0;
		while (xl > 0)
		{
			ssize_t L = (xl <= SIZE) ? xl : SIZE;
			xl -= L;
			for (ssize_t i=0; i < L; i++)
			{
				SEXP s = STRING_ELT(val, idx++);
				buf[i] = UTF8Text(translateCharUTF8(s));
			}
			Obj->Append(buf, L, svStrUTF8);
		}
		UNPROTECT(1);
	} else if (TYPEOF(val) == RAWSXP)
	{
		Obj->Append(RAW(val), xl, svInt8);
	} else if (!isNull(val))
	{
		throw ErrGDSFmt("the returned value from the user-defined function "
			"should be numeric, character or NULL.");
	}		
}

static void _apply_func_gdsnode(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	if (!isNull(val))
	{
		if (p->nTarget == 1)
		{
			_apply_func_gds_append(p->pTarget[0], val);
		} else {
			int n = XLENGTH(val);
			if (p->nTarget != n)
			{
				throw ErrGDSFmt("the returned value from the user-defined "
					"function should be a list of %d size.", n);
			}
			for (int i=0; i < n; i++)
			{
				_apply_func_gds_append(p->pTarget[i], VECTOR_ELT(val, i));
			}
		}
	}

	p->AccIdx ++;
}


/// called by 'apply.gdsn'
/** \param gds_nodes   [in] a list of objects of 'gdsn' class
 *  \param margins     [in] margin indices starting from 1
 *  \param FUN         [in] a function applied marginally
 *  \param selection   [in] indicates selection
 *  \param as_is       [in] the type of returned value
 *  \param var_index   [in] 1: none, 2: relative, 3: absolute
 *  \param rho         [in] the environment variable
**/
COREARRAY_DLL_EXPORT SEXP gdsApplyCall(SEXP gds_nodes, SEXP margins,
	SEXP FUN, SEXP selection, SEXP as_is, SEXP var_index, SEXP use_raw,
	SEXP target_node, SEXP rho)
{
	int use_raw_flag = Rf_asLogical(use_raw);
	if (use_raw_flag == NA_LOGICAL)
		error("'useraw' must be TRUE or FALSE.");

	const char *asRes = CHAR(STRING_ELT(as_is, 0));
	const char *varIdx = CHAR(STRING_ELT(var_index, 0));

	COREARRAY_TRY

		// the total number of objects
		int nObject = Rf_length(gds_nodes);

		// -----------------------------------------------------------
		// gds_nodes, a list of data variables

		vector<PdAbstractArray> ObjList(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			// check
			PdGDSObj Node = GDS_R_SEXP2Obj(VECTOR_ELT(gds_nodes, i));
			GDS_R_NodeValid(Node, TRUE);
			if (dynamic_cast<PdAbstractArray>(Node))
			{
				ObjList[i] = static_cast<PdAbstractArray>(Node);
			} else {
				throw ErrGDSFmt(
					"'nodes[[%d]]' should be array-oriented data!",
					i + 1);
			}
		}

		// target GDS node
		vector<CdAbstractArray*> Targets;
		if (!Rf_isNull(target_node))
		{
			size_t n = XLENGTH(target_node);
			Targets.resize(n);
			for (size_t i=0; i < n; i++)
			{
				PdGDSObj obj = GDS_R_SEXP2Obj(VECTOR_ELT(target_node, i));
				GDS_R_NodeValid(obj, FALSE);
				if (dynamic_cast<CdAbstractArray*>(obj))
					Targets[i] = static_cast<CdAbstractArray*>(obj);
				else
					throw ErrGDSFmt("'target.node[[%d]]' should be array-oriented!", (int)i);
			}
		}


		// -----------------------------------------------------------
		// get information

		vector<int> DimCnt(nObject);
		vector< vector<C_Int32> > DLen(nObject);
		vector<C_SVType> SVType(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			SVType[i] = ObjList[i]->SVType();
			if ( !COREARRAY_SV_NUMERIC(SVType[i]) &&
				!COREARRAY_SV_STRING(SVType[i]) )
			{
				throw ErrGDSFmt(
					"Only support numeric and character data currently.");	
			}
			DimCnt[i] = ObjList[i]->DimCnt();
			DLen[i].resize(DimCnt[i]);
			ObjList[i]->GetDim(&(DLen[i][0]));
		}

		// -----------------------------------------------------------
		// margin should be integer

		vector<int> Margin(nObject);
		for (int i=0; i < nObject; i++)
		{
			Margin[i] = DimCnt[i] - INTEGER(margins)[i];
			if ((Margin[i] < 0) || (Margin[i] >= DimCnt[i]))
			{
				throw ErrGDSFmt(
					"'margin' is not valid according to 'nodes[[i]]'!", i+1);
			}
		}

		// -----------------------------------------------------------
		// function
		if(!isFunction(FUN))
			throw ErrGDSFmt("'FUN' must be a function.");

		// -----------------------------------------------------------
		// selection

		vector< vector<C_BOOL *> > array_sel(nObject);
		vector< vector< vector<C_BOOL> > > sel_list(nObject);
		vector<C_BOOL **> sel_ptr(nObject);

		if (!isNull(selection))
		{
			if (Rf_length(selection) != nObject)
			{
				throw ErrGDSFmt(
					"length(selection) should be equal to length(nodes).");
			}

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

		// -----------------------------------------------------------
		// rho
		if (!isEnvironment(rho))
			throw ErrGDSFmt("'rho' should be an environment");

		// -----------------------------------------------------------
		// initialize data structure
		TApplyStruct a_struct;
		a_struct.R_Nodes = gds_nodes;
		a_struct.R_Fun = FUN;
		a_struct.R_Func_Call = NULL;
		a_struct.R_Rho = rho;
		a_struct.RV_List = rv_ans;
		a_struct.AccIdx = 0;
		a_struct.pTarget = NULL;
		a_struct.nTarget = 0;
		a_struct.nProtected = 0;

		if (strcmp(varIdx, "none") == 0)
		{
			a_struct.R_AccIdx = NULL;
			a_struct.R_MarIdx = NULL;
		} else if (strcmp(varIdx, "relative") == 0)
		{
			a_struct.R_AccIdx = PROTECT(NEW_INTEGER(1));
			a_struct.nProtected ++;
			a_struct.R_MarIdx = NULL;
		} else if (strcmp(varIdx, "absolute") == 0)
		{
			a_struct.R_AccIdx = NULL;
			a_struct.R_MarIdx = PROTECT(NEW_INTEGER(1));
			a_struct.nProtected ++;
		} else
			throw ErrGDSFmt("'var.index' is not valid!");


		// -----------------------------------------------------------
		// as.is
		// 0: none, 1: list, 2: integer, 3: double,
		// 4: character, 5: logical, 6: raw, 7: gdsnode
		if (strcmp(asRes, "none") == 0)
		{
			a_struct.DatType = 0;
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_none, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else if (strcmp(asRes, "list") == 0)
		{
			a_struct.DatType = 1;
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_list, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else if (strcmp(asRes, "integer") == 0)
		{
			a_struct.DatType = 2;
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_integer, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else if (strcmp(asRes, "double") == 0)
		{
			a_struct.DatType = 3;
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_double, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else if (strcmp(asRes, "character") == 0)
		{
			a_struct.DatType = 4;
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_char, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else if (strcmp(asRes, "logical") == 0)
		{
			a_struct.DatType = 5;
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_logical, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else if (strcmp(asRes, "raw") == 0)
		{
			a_struct.DatType = 6;
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_raw, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else if (strcmp(asRes, "gdsnode") == 0)
		{
			a_struct.DatType = 7;
			a_struct.pTarget = &Targets[0];
			a_struct.nTarget = Targets.size();
			GDS_R_Apply(nObject, &ObjList[0], &Margin[0], &sel_ptr[0],
				_apply_initfunc, _apply_func_gdsnode, &a_struct, TRUE,
				(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0));
		} else
			throw ErrGDSFmt("'as.is' is not valid!");

		// unprotect
		if (a_struct.nProtected > 0)
			UNPROTECT(a_struct.nProtected);

		// output
		rv_ans = a_struct.RV_List;

	COREARRAY_CATCH
}


/// called by 'clusterApply.gdsn', return a selection (list)
/** \param gds_nodes   [in] a list of objects of 'gdsn' class
 *  \param margins     [in] margin indices starting from 1
 *  \param selection   [in] indicates selection
**/
COREARRAY_DLL_EXPORT SEXP gdsApplyCreateSelection(SEXP gds_nodes,
	SEXP margins, SEXP selection)
{
	COREARRAY_TRY

		// the total number of objects
		int nObject = Rf_length(gds_nodes);
		// used in UNPROTECT
		int nProtected = 0;

		// initialize returned value
		PROTECT(rv_ans = NEW_LIST(nObject));
		nProtected ++;

		// -----------------------------------------------------------
		// gds_nodes, a list of data variables

		vector<PdAbstractArray> ObjList(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			// check
			PdGDSObj Node = GDS_R_SEXP2Obj(VECTOR_ELT(gds_nodes, i));
			GDS_R_NodeValid(Node, TRUE);

			if (dynamic_cast<PdAbstractArray>(Node))
			{
				ObjList[i] = static_cast<PdAbstractArray>(Node);
			} else {
				throw ErrGDSFmt(
					"'node.names[[%d]]' should be array-based!", i + 1);
			}
		}

		// -----------------------------------------------------------
		// get information

		vector<int> DimCnt(nObject);
		vector< vector<C_Int32> > DLen(nObject);
		vector<C_SVType> SVType(nObject);
		// for -- loop
		for (int i=0; i < nObject; i++)
		{
			SVType[i] = ObjList[i]->SVType();
			if (!(COREARRAY_SV_NUMERIC(SVType[i]) || COREARRAY_SV_STRING(SVType[i])))
			{
				throw ErrGDSFmt(
					"Only support numeric and character data currently.");	
			}
			DimCnt[i] = ObjList[i]->DimCnt();
			DLen[i].resize(DimCnt[i]);
			ObjList[i]->GetDim(&(DLen[i][0]));
		}

		// -----------------------------------------------------------
		// margin should be integer

		vector<int> Margin(nObject);
		for (int i=0; i < nObject; i++)
		{
			Margin[i] = DimCnt[i] - INTEGER(margins)[i];
			if ((Margin[i] < 0) || (Margin[i] >= DimCnt[i]))
			{
				throw ErrGDSFmt(
					"'margin' is not valid according to 'node.names[[i]]'!",
					i + 1);
			}
		}

		// -----------------------------------------------------------
		// selection

		vector< vector<C_BOOL *> > array_sel(nObject);
		vector< vector< vector<C_BOOL> > > sel_list(nObject);
		vector<C_BOOL **> sel_ptr(nObject);

		if (!isNull(selection))
		{
			if (Rf_length(selection) != nObject)
			{
				throw ErrGDSFmt(
					"length(selection) should be equal to length(node).");
			}

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

		// -----------------------------------------------------------
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
			else if (COREARRAY_SV_STRING(SVType[i]))
				Array[i].Init(*ObjList[i], Margin[i], svStrUTF8, sel_ptr[i], false);
			else
				throw ErrGDSFmt("not support in apply.gdsn");
		}

		// check the margin
		C_Int32 MCnt = Array[0].Count();
		for (int i=1; i < nObject; i++)
		{
			if (Array[i].Count() != MCnt)
			{
				throw ErrGDSFmt(
				"node[[%d]] should have the same number of elements as node[[1]] marginally (margin[%d] = margin[1]).",
				i + 1, i + 1);
			}
		}


		// -----------------------------------------------------------
		// set returned value
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

	COREARRAY_CATCH
}


/// get number of bytes and bits
/** \param ClassName   [in] the name of class
 *  \param out_nbit    [out] the number of bits
 *  \param err         [out] return TRUE if error occurs, otherwise FALSE
**/
COREARRAY_DLL_EXPORT SEXP gds_test_Class(SEXP ClassName)
{
	const char *cn = CHAR(STRING_ELT(ClassName, 0));

	COREARRAY_TRY

		// register classes if not
		RegisterClass();

		// the returned value
		int out_nbit = -1;

		// Class Name Mapping
		const char *nName;
		map<const char*, const char*, CInitNameObject::strCmp>::iterator it =
			Init.ClassMap.find(cn);
		if (it != Init.ClassMap.end())
			nName = it->second;
		else
			throw ErrGDSFmt("No support of '%s'.", cn);

		// mapping
		CdObjClassMgr::TdOnObjCreate OnCreate =
			dObjManager().NameToClass(nName);
		if (OnCreate)
		{
			CdObject *obj = OnCreate();
			if (dynamic_cast<CdContainer*>(obj))
			{
				out_nbit = static_cast<CdContainer*>(obj)->BitOf();
			}
			delete obj;
		} else
			throw ErrGDSFmt("No support of '%s'.", cn);

		rv_ans = ScalarInteger(out_nbit);

	COREARRAY_CATCH
}


COREARRAY_DLL_LOCAL void R_Init_RegCallMethods(DllInfo *info)
{
	#define CALL(name, num)    { #name, (DL_FUNC)&name, num }

	static R_CallMethodDef callMethods[] =
	{
		CALL(gdsCreateGDS, 2),    CALL(gdsOpenGDS, 4),
		CALL(gdsCloseGDS, 1),     CALL(gdsSyncGDS, 1),
		CALL(gdsTidyUp, 2),       CALL(gdsGetConnection, 0),
		CALL(gdsDiagInfo, 1),     CALL(gdsFileValid, 1),

		CALL(gdsNodeChildCnt, 2),    CALL(gdsNodeName, 2),
		CALL(gdsRenameNode, 2),      CALL(gdsNodeEnumName, 2),
		CALL(gdsNodeIndex, 4),       CALL(gdsNodeObjDesp, 1),
		CALL(gdsAddNode, 10),        CALL(gdsAddFolder, 5),
		CALL(gdsAddFile, 5),         CALL(gdsGetFile, 2),
		CALL(gdsDeleteNode, 2),      CALL(gdsNodeValid, 1),
		CALL(gdsAssign, 3),          CALL(gdsMoveTo, 3),
		CALL(gdsCache, 1),

		CALL(gdsPutAttr, 3),         CALL(gdsGetAttr, 1),
		CALL(gdsDeleteAttr, 2),

		CALL(gdsObjCompress, 2),     CALL(gdsObjCompressClose, 1),
		CALL(gdsObjSetDim, 3),
		CALL(gdsObjAppend, 3),       CALL(gdsObjAppend2, 2),
		CALL(gdsObjReadData, 5),     CALL(gdsObjReadExData, 4),
		CALL(gdsObjWriteAll, 3),     CALL(gdsObjWriteData, 5),
	
		CALL(gdsApplySetStart, 1),   CALL(gdsApplyCall, 9),
		CALL(gdsApplyCreateSelection, 3),

		CALL(gdsIsElement, 2),       CALL(gdsLastErrGDS, 0),
		CALL(gdsSystem, 0),

		{ NULL, NULL, 0 }
	};

	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

} // extern "C"
