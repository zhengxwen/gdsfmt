// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// gdsfmt.cpp: R Interface to CoreArray Genomic Data Structure (GDS) Files
//
// Copyright (C) 2011-2024    Xiuwen Zheng
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

#include <string>
#include <set>
#include <map>
#include "R_GDS_CPP.h"
#include <Rdefines.h>
#include <R_ext/Rdynload.h>


namespace gdsfmt
{
	extern PdGDSFile PKG_GDS_Files[];
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

			ClassMap["bit24"] = TdTraits< CoreArray::UInt24 >::StreamName();
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

			ClassMap["sbit24"] = TdTraits< CoreArray::Int24 >::StreamName();
			ClassMap["sbit32"] = TdTraits< C_Int32          >::StreamName();
			ClassMap["sbit64"] = TdTraits< C_Int64          >::StreamName();

			ClassMap["sp.int"]   = TdTraits< TSpInt32 >::StreamName();
			ClassMap["sp.int8"]  = TdTraits< TSpInt8 >::StreamName();
			ClassMap["sp.int16"] = TdTraits< TSpInt16 >::StreamName();
			ClassMap["sp.int32"] = TdTraits< TSpInt32 >::StreamName();
			ClassMap["sp.int64"] = TdTraits< TSpInt64 >::StreamName();
			ClassMap["sp.uint8"]  = TdTraits< TSpUInt8 >::StreamName();
			ClassMap["sp.uint16"] = TdTraits< TSpUInt16 >::StreamName();
			ClassMap["sp.uint32"] = TdTraits< TSpUInt32 >::StreamName();
			ClassMap["sp.uint64"] = TdTraits< TSpUInt64 >::StreamName();


			// ==============================================================
			// Real number
			ClassMap["float32"] = TdTraits< C_Float32 >::StreamName();
			ClassMap["float64"] = TdTraits< C_Float64 >::StreamName();
			ClassMap["packedreal8"]   = TdTraits< TReal8  >::StreamName();
			ClassMap["packedreal8u"]  = TdTraits< TReal8u >::StreamName();
			ClassMap["packedreal16"]  = TdTraits< TReal16 >::StreamName();
			ClassMap["packedreal16u"] = TdTraits< TReal16u >::StreamName();
			ClassMap["packedreal24"]  = TdTraits< TReal24 >::StreamName();
			ClassMap["packedreal24u"] = TdTraits< TReal24u >::StreamName();
			ClassMap["packedreal32"]  = TdTraits< TReal32 >::StreamName();
			ClassMap["packedreal32u"] = TdTraits< TReal32u >::StreamName();
			ClassMap["sp.real"]    = TdTraits< TSpReal64 >::StreamName();
			ClassMap["sp.real32"]  = TdTraits< TSpReal32 >::StreamName();
			ClassMap["sp.real64"]  = TdTraits< TSpReal64 >::StreamName();


			// ==============================================================
			// String
			ClassMap["string"   ] = TdTraits< VARIABLE_LEN<C_UTF8>  >::StreamName();
			ClassMap["string16" ] = TdTraits< VARIABLE_LEN<C_UTF16> >::StreamName();
			ClassMap["string32" ] = TdTraits< VARIABLE_LEN<C_UTF32> >::StreamName();
			ClassMap["cstring"  ] = TdTraits< C_STRING<C_UTF8>  >::StreamName();
			ClassMap["cstring16"] = TdTraits< C_STRING<C_UTF16> >::StreamName();
			ClassMap["cstring32"] = TdTraits< C_STRING<C_UTF32> >::StreamName();
			ClassMap["fstring"  ] = TdTraits< FIXED_LEN<C_UTF8>  >::StreamName();
			ClassMap["fstring16"] = TdTraits< FIXED_LEN<C_UTF16> >::StreamName();
			ClassMap["fstring32"] = TdTraits< FIXED_LEN<C_UTF32> >::StreamName();


			// ==============================================================
			// R storage mode
			ClassMap["char"     ] = TdTraits< C_Int8  >::StreamName();
			ClassMap["raw"      ] = TdTraits< C_Int8  >::StreamName();
			ClassMap["int"      ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["integer"  ] = TdTraits< C_Int32 >::StreamName();
			ClassMap["vl_int"   ] = TdTraits< TVL_Int >::StreamName();
			ClassMap["vl_uint"  ] = TdTraits< TVL_UInt >::StreamName();
			ClassMap["float"    ] = TdTraits< C_Float32 >::StreamName();
			ClassMap["single"   ] = TdTraits< C_Float32 >::StreamName();
			ClassMap["numeric"  ] = TdTraits< C_Float64 >::StreamName();
			ClassMap["double"   ] = TdTraits< C_Float64 >::StreamName();
			ClassMap["character"] = TdTraits< VARIABLE_LEN<C_UTF8> >::StreamName();
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
static const char *ERR_NO_DATA =
	"There is no data field.";

// predefined string
static const UTF8String STR_INVISIBLE = "R.invisible";


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
	R_len_t n = Rf_isNull(names) ? 0 : XLENGTH(list);
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


/// strcmp without case sensitivity
static inline int str_icmp(const char *p1, const char *p2)
{
	unsigned char *s1 = (unsigned char *) p1;
	unsigned char *s2 = (unsigned char *) p2;
	unsigned char c1, c2;
	do
	{
		c1 = (unsigned char) toupper((int)*s1++);
		c2 = (unsigned char) toupper((int)*s2++);
		if (c1 == '\0')
		{
			return c1 - c2;
		}
	}
	while (c1 == c2);

	return c1 - c2;
}

/// Get whether it is in a list
static bool IsElement(const char *s, const char *list[])
{
	while (*list)
	{
		if (str_icmp(s, *list) == 0)
			return true;
		list ++;
	}
	return false;
}

/// file size to a string
static string fmt_size(double b)
{
	static const double TB = 1024.0*1024*1024*1024;
	static const double GB = 1024.0*1024*1024;
	static const double MB = 1024.0*1024;
	static const double KB = 1024.0;

	char s[256];
	if (b >= TB)
		FmtText(s, sizeof(s), "%.1fT", b/TB);
	else if (b >= GB)
		FmtText(s, sizeof(s), "%.1fG", b/GB);
	else if (b >= MB)
		FmtText(s, sizeof(s), "%.1fM", b/MB);
	else if (b >= KB)
		FmtText(s, sizeof(s), "%.1fK", b/KB);
	else
		FmtText(s, sizeof(s), "%gB", b);

	return string(s);
}


extern SEXP new_gdsptr_obj(CdGDSFile *file, SEXP id, bool do_free);
extern SEXP gdsObjWriteAll(SEXP Node, SEXP Val, SEXP Check);
extern SEXP gdsObjSetDim(SEXP Node, SEXP DLen, SEXP Permute);


// ----------------------------------------------------------------------------
// File Operations
// ----------------------------------------------------------------------------

/// Create a GDS file
/** \param FileName    [in] the file name
 *  \param AllowDup    [in] allow duplicate file
 *  \return
 *    $filename    file name
 *    $id          ID of GDS file, internal use
 *    $ptr         pointer to GDS file, internal use
 *    $root        root of hierarchical structure
 *    $readonly	   whether it is read-only or not
**/
COREARRAY_DLL_EXPORT SEXP gdsCreateGDS(SEXP FileName, SEXP AllowDup)
{
	// filename
	SEXP fs = STRING_ELT(FileName, 0);
	if (fs == NA_STRING) error("'filename' should not be NA.");
	const char *fn = CHAR(fs);
	// allow.duplicate
	int allow_dup = Rf_asLogical(AllowDup);
	if (allow_dup == NA_LOGICAL) error("'allow.duplicate' must be TRUE or FALSE.");

	COREARRAY_TRY
		// check duplicate file
		if (!allow_dup)
		{
			UTF8String FName = UTF8Text(fn);
			for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
			{
				if (PKG_GDS_Files[i])
				{
					if (PKG_GDS_Files[i]->FileName() == FName)
					{
						throw ErrGDSFmt(
							"The file '%s' has been created or opened.", fn);
					}
				}
			}
		}
		// create file and return R object
		CdGDSFile *file = GDS_File_Create(fn);
		PROTECT(rv_ans = NEW_LIST(5));
			SET_ELEMENT(rv_ans, 0, FileName);
			SEXP ID = ScalarInteger(GetFileIndex(file));
			SET_ELEMENT(rv_ans, 1, ID);
			SET_ELEMENT(rv_ans, 2, new_gdsptr_obj(file, ID, true));
			SET_ELEMENT(rv_ans, 3, GDS_R_Obj2SEXP(&(file->Root())));
			SET_ELEMENT(rv_ans, 4, ScalarLogical(FALSE));
		UNPROTECT(1);
	COREARRAY_CATCH
}


/// Open an existing GDS file
/** \param FileName    [in] the file name
 *  \param ReadOnly    [in] if TRUE, read-only
 *  \param AllowDup    [in] allow duplicate file
 *  \param AllowFork   [in] allow opening in a forked process
 *  \param AllowError  [in] allow errors in the GDS file to recover data
 *  \return
 *    $filename    file name
 *    $id          ID of GDS file, internal use
 *    $ptr         pointer to GDS file, internal use
 *    $root        root of hierarchical structure
 *    $readonly	   whether it is read-only or not
**/
COREARRAY_DLL_EXPORT SEXP gdsOpenGDS(SEXP FileName, SEXP ReadOnly,
	SEXP AllowDup, SEXP AllowFork, SEXP AllowError)
{
	// filename
	SEXP fs = STRING_ELT(FileName, 0);
	if (fs == NA_STRING) error("'filename' should not be NA.");
	const char *fn = CHAR(fs);
	// readonly
	int readonly = Rf_asLogical(ReadOnly);
	if (readonly == NA_LOGICAL) error("'readonly' must be TRUE or FALSE.");
	// allow.duplicate
	int allow_dup = Rf_asLogical(AllowDup);
	if (allow_dup == NA_LOGICAL) error("'allow.duplicate' must be TRUE or FALSE.");
	// allow.fork
	int allow_fork = Rf_asLogical(AllowFork);
	if (allow_fork == NA_LOGICAL) error("'allow.fork' must be TRUE or FALSE.");
	// allow.error
	int allow_error = Rf_asLogical(AllowError);
	if (allow_error == NA_LOGICAL) error("'allow.error' must be TRUE or FALSE.");

	COREARRAY_TRY
		// check duplicate file
		if (!allow_dup)
		{
			UTF8String FName = UTF8Text(fn);
			for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
			{
				if (PKG_GDS_Files[i])
				{
					if (PKG_GDS_Files[i]->FileName() == FName)
						throw ErrGDSFmt("The file '%s' has been created or opened.", fn);
				}
			}
		}
		// open file and return R object
		CdGDSFile *file = GDS_File_Open(fn, readonly, allow_fork, allow_error);
		PROTECT(rv_ans = NEW_LIST(5));
			SET_ELEMENT(rv_ans, 0, FileName);
			SEXP ID = ScalarInteger(GetFileIndex(file));
			SET_ELEMENT(rv_ans, 1, ID);
			SET_ELEMENT(rv_ans, 2, new_gdsptr_obj(file, ID, true));
			SET_ELEMENT(rv_ans, 3, GDS_R_Obj2SEXP(&(file->Root())));
			SET_ELEMENT(rv_ans, 4, ScalarLogical(readonly));
		UNPROTECT(1);
	COREARRAY_CATCH
}


/// Close the GDS file
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsCloseGDS(SEXP gdsfile)
{
	COREARRAY_TRY
		GDS_File_Close(GDS_R_SEXP2File(gdsfile));
		SEXP gds_id  = GetListElement(gdsfile, "id");
		INTEGER(gds_id)[0] = -1;
		SEXP gds_ptr = GetListElement(gdsfile, "ptr");
		R_SetExternalPtrAddr(gds_ptr, NULL);
	COREARRAY_CATCH
}


/// Synchronize the GDS file
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsSyncGDS(SEXP gdsfile)
{
	COREARRAY_TRY
		GDS_R_SEXP2File(gdsfile)->SyncFile();
	COREARRAY_CATCH
}


/// Reopen the GDS file if needed and return TRUE
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsReopenGDS(SEXP gdsfile)
{
	COREARRAY_TRY
		C_BOOL rv = GDS_File_Reopen(gdsfile);
		rv_ans = ScalarLogical(rv ? TRUE : FALSE);
	COREARRAY_CATCH
}


/// Get the file size and check the file handler
/** \param gdsfile     [in] the GDS file object
**/
COREARRAY_DLL_EXPORT SEXP gdsFileSize(SEXP gdsfile)
{
	COREARRAY_TRY
		PdGDSFile file = GDS_R_SEXP2File(gdsfile);
		rv_ans = ScalarReal(file->GetFileSize());
	COREARRAY_CATCH
}


/// Clean up fragments of a GDS file
/** \param FileName    [in] the file name
 *  \param Verbose     [in] if TRUE, show information
**/
COREARRAY_DLL_EXPORT SEXP gdsTidyUp(SEXP FileName, SEXP Verbose)
{
	const char *fn = R_ExpandFileName(CHAR(STRING_ELT(FileName, 0)));

	int verbose_flag = Rf_asLogical(Verbose);
	if (verbose_flag == NA_LOGICAL)
		error("'verbose' must be TRUE or FALSE.");

	COREARRAY_TRY

		CdGDSFile file(fn, CdGDSFile::dmOpenReadWrite);
		C_Int64 old_s = file.GetFileSize();
		if (verbose_flag == TRUE)
		{
			Rprintf("Clean up the fragments of GDS file:\n");
			Rprintf("    open the file '%s' (%s)\n", fn, fmt_size(old_s).c_str());
			Rprintf("    # of fragments: %d\n", file.GetNumOfFragment());
			Rprintf("    save to '%s.tmp'\n", fn);
		}
		file.TidyUp(false);
		if (verbose_flag == TRUE)
		{
			C_Int64 new_s = file.GetFileSize();
			Rprintf("    rename '%s.tmp' (%s, reduced: %s)\n", fn,
				fmt_size(new_s).c_str(), fmt_size(old_s-new_s).c_str());
			Rprintf("    # of fragments: %d\n", file.GetNumOfFragment());
		}

	COREARRAY_CATCH
}


/// Get all handles of opened GDS files
COREARRAY_DLL_EXPORT SEXP gdsShowFile(SEXP CloseAll)
{
	int closeall = Rf_asLogical(CloseAll);
	if (closeall == NA_LOGICAL)
		error("'closeall' must be TRUE or FALSE.");

	COREARRAY_TRY

		int FileCnt = 0;
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
			if (PKG_GDS_Files[i]) FileCnt ++;

		rv_ans = PROTECT(NEW_LIST(3));
		SEXP fn = NEW_CHARACTER(FileCnt);
		SET_ELEMENT(rv_ans, 0, fn);
		SEXP rd = NEW_LOGICAL(FileCnt);
		SET_ELEMENT(rv_ans, 1, rd);
		SEXP st = NEW_CHARACTER(FileCnt);
		SET_ELEMENT(rv_ans, 2, st);

		int k = 0;
		for (int i=0; i < GDSFMT_MAX_NUM_GDS_FILES; i++)
		{
			PdGDSFile file = PKG_GDS_Files[i];
			if (file)
			{
				SET_STRING_ELT(fn, k, mkChar(RawText(file->FileName()).c_str()));
				LOGICAL(rd)[k] = file->ReadOnly() ? TRUE : FALSE;
				if (closeall)
				{
					SET_STRING_ELT(st, k, mkChar("closed"));
					GDS_File_Close(file);
				} else {
					SET_STRING_ELT(st, k, mkChar("open"));
				}
				k ++;
			}
		}

		UNPROTECT(1);

	COREARRAY_CATCH
}


static map<TdGDSBlockID, string> diag_MapID;

static void diag_EnumObject(CdGDSObj &Obj)
{
	vector<const CdBlockStream*> LIST;

	string name = Obj.FullName();
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


/// Diagnose a GDS file
/** \param gdsfile     [in] the GDS file object
 *  \param logonly     [in] TRUE, return log only
**/
COREARRAY_DLL_EXPORT SEXP gdsDiagInfo(SEXP gdsfile, SEXP log_only)
{
	int logonly = Rf_asLogical(log_only);
	if (logonly == NA_LOGICAL) error("'log.only' must be TRUE or FALSE.");

	COREARRAY_TRY

		// get the GDS file
		CdGDSFile *tmp = GDS_R_SEXP2File(gdsfile);
		CdBlockCollection *file = (CdBlockCollection*)tmp;
		int nProtected = 0;

		if (logonly == 0)
		{
			// ====  Stream List  ====

			// load objects
			diag_MapID.clear();
			diag_EnumObject(tmp->Root());
			PROTECT(rv_ans = NEW_LIST(2));
			nProtected ++;
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
				SET_STRING_ELT(NameList, i, mkChar(diag_MapID[BL[i]->ID()].c_str()));
			}

			// Unused stream info
			const CoreArray::CdBlockStream::TBlockInfo *s = file->UnusedBlock();
			int Cnt = 0;
			SIZE64 Size = 0;
			while (s) { Cnt++; Size += s->BlockSize; s = s->Next; }

			INTEGER(IDList)[n-1] = NA_INTEGER;
			REAL(TotalSize)[n-1] = Size;
			REAL(Capacity)[n-1] = Size;
			INTEGER(nFragment)[n-1] = Cnt;
			SET_STRING_ELT(NameList, n-1, mkChar("$unused$"));
		}

		// ====  Log  ====
		int n = tmp->Log().List().size();
		SEXP LogList = PROTECT(NEW_CHARACTER(n));
		nProtected ++;
		for (int i=0; i < n; i++)
		{
			CdLogRecord::TdItem &I = tmp->Log().List()[i];
			string s = string(I.TypeStr()) + ": " + I.Msg;
			SET_STRING_ELT(LogList, i, mkChar(s.c_str()));
		}

		if (logonly == 0)
			SET_ELEMENT(rv_ans, 1, LogList);
		else
			rv_ans = LogList;

		UNPROTECT(nProtected);

	COREARRAY_CATCH
}


/// Diagnose a GDS node: a sub routine
static SEXP _SEXP_Block(const CdBlockStream *Stream, int &nProtected)
{
	SEXP ans = PROTECT(NEW_LIST(3));

	SET_ELEMENT(ans, 0, ScalarReal(Stream->GetSize()));
	int n = Stream->ListCount();
	SEXP Offset = PROTECT(NEW_NUMERIC(n));
	SET_ELEMENT(ans, 1, Offset);
	SEXP Size = PROTECT(NEW_NUMERIC(n));
	SET_ELEMENT(ans, 2, Size);

	const CdBlockStream::TBlockInfo *p = Stream->List();
	for (int i=0; i < n; i++)
	{
		REAL(Offset)[i] = p->StreamStart;
		REAL(Size)[i] = p->BlockSize;
		p = p->Next;
	}

	SEXP nm = PROTECT(NEW_CHARACTER(3));
	SET_STRING_ELT(nm, 0, mkChar("total_size"));
	SET_STRING_ELT(nm, 1, mkChar("chunk_offset"));
	SET_STRING_ELT(nm, 2, mkChar("chunk_size"));
	SET_NAMES(ans, nm);

	nProtected += 4;
	return ans;
}

/// Diagnose a GDS node
/** \param node     [in] the GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsDiagInfo2(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);

		const CdBlockStream *Body = Obj->GDSStream();
		vector<const CdBlockStream*> Data;
		Obj->GetOwnBlockStream(Data);

		// random access stream?
		int nRA = 0;
		vector<SIZE64> RA_RawSize, RA_CmpSize;
		if (dynamic_cast<CdAllocArray*>(Obj))
		{
			CdAllocator &alloc = dynamic_cast<CdAllocArray*>(Obj)->Allocator();
			if (alloc.BufStream())
			{
				CdStream *s = alloc.BufStream()->Stream();
				if (dynamic_cast<CdZDecoder_RA*>(s))
				{
					dynamic_cast<CdZDecoder_RA*>(s)->GetBlockInfo(
						RA_RawSize, RA_CmpSize);
					nRA = 1;
			#ifndef COREARRAY_NO_LZ4
				} else if (dynamic_cast<CdLZ4Decoder_RA*>(s))
				{
					dynamic_cast<CdLZ4Decoder_RA*>(s)->GetBlockInfo(
						RA_RawSize, RA_CmpSize);
					nRA = 1;
			#endif
			#ifndef COREARRAY_NO_LZMA
				} else if (dynamic_cast<CdXZDecoder_RA*>(s))
				{
					dynamic_cast<CdXZDecoder_RA*>(s)->GetBlockInfo(
						RA_RawSize, RA_CmpSize);
					nRA = 1;
			#endif
				}
			}
		}

		int nProtected = 0;
		PROTECT(rv_ans = NEW_LIST(Data.size() + 1 + nRA));
		nProtected ++;

		// head
		SET_ELEMENT(rv_ans, 0, _SEXP_Block(Body, nProtected));

		// data, ...
		for (int j=0; j < (int)Data.size(); j++)
			SET_ELEMENT(rv_ans, j+1, _SEXP_Block(Data[j], nProtected));

		// random-access stream
		if (nRA > 0)
		{
			SEXP RA = PROTECT(NEW_LIST(2));
			nProtected ++;
			SEXP x = NEW_NUMERIC(RA_RawSize.size());
			SET_ELEMENT(RA, 0, x);
			for (int i=0; i < (int)RA_RawSize.size(); i++)
				REAL(x)[i] = RA_RawSize[i];
			SEXP y = NEW_NUMERIC(RA_CmpSize.size());
			SET_ELEMENT(RA, 1, y);
			for (int i=0; i < (int)RA_CmpSize.size(); i++)
				REAL(y)[i] = RA_CmpSize[i];
			SET_ELEMENT(rv_ans, Data.size()+1, RA);

			SEXP nm = PROTECT(NEW_CHARACTER(2));
			nProtected ++;
			SET_STRING_ELT(nm, 0, mkChar("raw"));
			SET_STRING_ELT(nm, 1, mkChar("compression"));
			SET_NAMES(RA, nm);
		}

		// names
		SEXP nm = PROTECT(NEW_CHARACTER(XLENGTH(rv_ans)));
		nProtected ++;
		SET_STRING_ELT(nm, 0, mkChar("head"));
		if (Data.size() > 1)
		{
			for (int j=1; j <= (int)Data.size(); j++)
				SET_STRING_ELT(nm, j+1, mkChar(Format("data%d", j).c_str()));
		} else if (Data.size() == 1)
			SET_STRING_ELT(nm, 1, mkChar("data"));
		if (nRA)
			SET_STRING_ELT(nm, Data.size()+1, mkChar("ra_stream"));

		SET_NAMES(rv_ans, nm);
		UNPROTECT(nProtected);

	COREARRAY_CATCH
}



// ----------------------------------------------------------------------------
// File Structure Operations
// ----------------------------------------------------------------------------

/// Detect whether a node is valid
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeValid(SEXP Node)
{
	COREARRAY_TRY
		GDS_R_SEXP2Obj(Node, TRUE);
	COREARRAY_CATCH
}


/// Get the number of variables in a folder
/** \param Node        [in] a GDS node
 *  \param Hidden      [in] whether include hidden variable(s)
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeChildCnt(SEXP Node, SEXP Hidden)
{
	int include_hidden = Rf_asLogical(Hidden);
	if (include_hidden == NA_LOGICAL)
		error("'include.hidden' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);

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
						if (!Obj->GetHidden() &&
							!Obj->Attribute().HasName(STR_INVISIBLE))
						{
							Cnt ++;
						}
					}
				}
			}
		} else if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
		{
			CdGDSVirtualFolder *Dir = static_cast<CdGDSVirtualFolder*>(Obj);
			if (Dir->IsLoaded(true))
			{
				if (include_hidden)
				{
					Cnt = Dir->NodeCount();
				} else {
					for (int i=0; i < Dir->NodeCount(); i++)
					{
						CdGDSObj *Obj = Dir->ObjItemEx(i);
						if (Obj)
						{
							if (!Obj->GetHidden() &&
								!Obj->Attribute().HasName(STR_INVISIBLE))
							{
								Cnt ++;
							}
						}
					}
				}
			}
		}

		rv_ans = ScalarInteger(Cnt);

	COREARRAY_CATCH
}


/// Get the name of a specified node
/** \param Node        [in] a GDS node
 *  \param FullName    [in] if TRUE, return the name with full path
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeName(SEXP Node, SEXP FullName)
{
	int full = Rf_asLogical(FullName);
	if (full == NA_LOGICAL)
		error("'fullname' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		string nm;
		if (full == TRUE)
			nm = Obj->FullName();
		else
			nm = Obj->Name();
		rv_ans = mkStringUTF8(nm.c_str());

	COREARRAY_CATCH
}


/// Enumerate the names of its child nodes (static)
static void gds_ls_name(CdGDSAbsFolder *dir, bool recursive, bool hidden,
	bool include_dir, string name, vector<string> &list)
{
	for (int i=0; i < dir->NodeCount(); i++)
	{
		CdGDSObj *obj = dir->ObjItemEx(i);
		if (obj)
		{
			if (hidden)
			{
				CdGDSAbsFolder *dir_obj = dynamic_cast<CdGDSAbsFolder*>(obj);
				string nm = obj->Name();
				if (name != "") nm = name + "/" + nm;
				if (include_dir || !dir_obj)
					list.push_back(nm);
				if (recursive && dir_obj)
				{
					gds_ls_name(dir_obj, recursive, hidden, include_dir,
						nm, list);
				}
			} else {
				if (!obj->GetHidden() &&
					!obj->Attribute().HasName(STR_INVISIBLE))
				{
					CdGDSAbsFolder *dir_obj = dynamic_cast<CdGDSAbsFolder*>(obj);
					string nm = obj->Name();
					if (name != "") nm = name + "/" + nm;
					if (include_dir || !dir_obj)
						list.push_back(nm);
					if (recursive && dir_obj)
					{
						gds_ls_name(dir_obj, recursive, hidden, include_dir,
							nm, list);
					}
				}
			}
		}
	}
}

/// Enumerate the names of its child nodes
/** \param Node        [in] a GDS node
 *  \param Hidden      [in] whether include hidden variable(s)
 *  \param Recursive   [in] whether recursively include the sub folder(s)
 *  \param IncludeDir  [in] whether include the sub folder(s)
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeEnumName(SEXP Node, SEXP Hidden,
	SEXP Recursive, SEXP IncludeDir)
{
	int hidden = Rf_asLogical(Hidden);
	if (hidden == NA_LOGICAL)
		error("'include.hidden' must be TRUE or FALSE.");

	int recursive = Rf_asLogical(Recursive);
	if (recursive == NA_LOGICAL)
		error("'recursive' must be TRUE or FALSE.");

	int include_dir = Rf_asLogical(IncludeDir);
	if (include_dir == NA_LOGICAL)
		error("'include.dirs' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		CdGDSAbsFolder *Dir = dynamic_cast<CdGDSAbsFolder*>(Obj);

		if (Dir)
		{
			vector<string> List;
			gds_ls_name(Dir, recursive, hidden, include_dir, "", List);
			PROTECT(rv_ans = NEW_STRING(List.size()));
			for (size_t i=0; i < List.size(); i++)
			{
				SET_STRING_ELT(rv_ans, i, mkCharCE(List[i].c_str(), CE_UTF8));
			}
			UNPROTECT(1);
		} else {
			rv_ans = NEW_STRING(0);
		}

	COREARRAY_CATCH
}


/// Get the node with index or indices
/** \param Node        [in] a GDS node
 *  \param Path        [in] the full path of a specified node
 *  \param Index       [in] the index or indices of a specified node
 *  \param Silent      [in] return R_NilValue if fails and 'Silent=TRUE'
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeIndex(SEXP Node, SEXP Path, SEXP Index,
	SEXP Silent)
{
	int silent_flag = Rf_asLogical(Silent);
	if (silent_flag == NA_LOGICAL)
		error("'silent' must be TRUE or FALSE.");
	int nProtected = 0;

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);

		if (Rf_isNull(Path))
		{
			// "path=NULL, index=..."

			if (!Rf_isNumeric(Index) && !Rf_isString(Index))
				throw ErrGDSFile("'index' should numeric values or characters.");
			if (Rf_isReal(Index))
			{
				PROTECT(Index = Rf_coerceVector(Index, INTSXP));
				nProtected ++;
			}

			for (int i=0; i < XLENGTH(Index); i++)
			{
				if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
				{
					string pn = Obj->FullName();
					if (pn.empty()) pn = "$ROOT$";
					throw ErrGDSFile("'%s' is not a folder.", pn.c_str());
				}
				CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);

				if (Rf_isInteger(Index))
				{
					int idx = INTEGER(Index)[i];
					if ((idx < 1) || (idx > Dir.NodeCount()))
					{
						string pn = Dir.FullName();
						if (pn.empty()) pn = "$ROOT$";
						throw ErrGDSFile("'%s' index[%d], out of range 1..%d.",
							pn.c_str(), idx, Dir.NodeCount());
					}
					Obj = Dir.ObjItem(idx - 1);
				} else if (Rf_isString(Index))
				{
					const char *nm = translateCharUTF8(STRING_ELT(Index, i));
					Obj = Dir.ObjItemEx(nm);
					if (Obj == NULL)
					{
						string pn = Dir.FullName();
						if (pn.empty()) pn = "$ROOT$";
						throw ErrGDSFile("'%s' has no node of '%s'.",
							pn.c_str(), nm);
					}
				}
			}
		} else {
			// "path=..., index=NULL"

			if (!Rf_isNull(Index))
				throw ErrGDSFile("'index' should be NULL if 'path' is specified.");
			if (!Rf_isString(Path))
				throw ErrGDSFile("'path' should be character-type.");
			if (XLENGTH(Path) != 1)
				throw ErrGDSFile("Please use '/' as a separator.");

			if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			{
				string pn = Obj->FullName();
				if (pn.empty()) pn = "$ROOT$";
				throw ErrGDSFile("'%s' is not a folder.", pn.c_str());
			}

			CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
			const char *nm = translateCharUTF8(STRING_ELT(Path, 0));
			Obj = Dir.PathEx(nm);
			if (!Obj && !silent_flag)
				throw ErrGDSObj("No such GDS node \"%s\"!", nm);
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
			error("%s", GDS_GetError());
	}

	return rv_ans;
}


/// Get the folder node which contains the specified node
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsGetFolder(SEXP Node)
{
	COREARRAY_TRY
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		Obj = Obj->Folder();
		if (Obj) rv_ans = GDS_R_Obj2SEXP(Obj);
	COREARRAY_CATCH
}


/// Get the description of a GDS node
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsNodeObjDesp(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		int nProtected = 0;
		SEXP tmp;
		PROTECT(rv_ans = NEW_LIST(16));
		nProtected ++;

			// 1: name
			SET_ELEMENT(rv_ans, 0, mkStringUTF8(Obj->Name().c_str()));
			// 2: full name
			SET_ELEMENT(rv_ans, 1, mkStringUTF8(Obj->FullName().c_str()));
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

			// 7: is.sparse
			SET_ELEMENT(rv_ans, 6,
				ScalarLogical(dynamic_cast<CdSpExStruct*>(Obj) ? TRUE : FALSE));

			// 8: dim, the dimension of data field
			// 9: encoder, the compression method: "", "ZIP"
			// 10: compress, the compression method: "", "ZIP.max"
			// 11: cpratio, data compression ratio, "NaN" indicates no compression
			if (dynamic_cast<CdAbstractArray*>(Obj))
			{
				CdAbstractArray *_Obj = (CdAbstractArray*)Obj;

				PROTECT(tmp = NEW_INTEGER(_Obj->DimCnt()));
				nProtected ++;
				SET_ELEMENT(rv_ans, 7, tmp);
				for (int i=0; i < _Obj->DimCnt(); i++)
					INTEGER(tmp)[i] = _Obj->GetDLen(_Obj->DimCnt()-i-1);

				SEXP encoder, coder, ratio;
				PROTECT(encoder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 8, encoder);
				SET_STRING_ELT(encoder, 0, mkChar(""));
				PROTECT(coder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 9, coder);
				SET_STRING_ELT(coder, 0, mkChar(""));

				PROTECT(ratio = NEW_NUMERIC(1));
				nProtected ++;
				SET_ELEMENT(rv_ans, 10, ratio);
				REAL(ratio)[0] = R_NaN;

				if (_Obj->PipeInfo())
				{
					SET_STRING_ELT(encoder, 0,
						mkChar(_Obj->PipeInfo()->Coder()));
					SET_STRING_ELT(coder, 0,
						mkChar(_Obj->PipeInfo()->CoderParam().c_str()));
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
				SET_ELEMENT(rv_ans, 7, tmp);

				SEXP encoder, coder, ratio;
				PROTECT(encoder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 8, encoder);
				SET_STRING_ELT(encoder, 0, mkChar(""));
				PROTECT(coder = NEW_STRING(1)); nProtected ++;
				SET_ELEMENT(rv_ans, 9, coder);
				SET_STRING_ELT(coder, 0, mkChar(""));

				PROTECT(ratio = NEW_NUMERIC(1));
				nProtected ++;
				SET_ELEMENT(rv_ans, 10, ratio);
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

			// 12: size
			double Size = R_NaN;
			if (dynamic_cast<CdContainer*>(Obj))
			{
				CdContainer* p = static_cast<CdContainer*>(Obj);
				p->Synchronize();
				Size = p->GDSStreamSize();
			} else if (dynamic_cast<CdGDSStreamContainer*>(Obj))
			{
				CdGDSStreamContainer *_Obj = (CdGDSStreamContainer*)Obj;
				if (_Obj->PipeInfo())
					Size = _Obj->PipeInfo()->StreamTotalIn();
				else
					Size = _Obj->GetSize();
			}
			SET_ELEMENT(rv_ans, 11, ScalarReal(Size));

			// 13: good
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
			SET_ELEMENT(rv_ans, 12, ScalarLogical(GoodFlag));

			// 14: hidden
			int hidden_flag = Obj->GetHidden() ||
				Obj->Attribute().HasName(STR_INVISIBLE);
			SET_ELEMENT(rv_ans, 13, ScalarLogical(hidden_flag));

			// 15: message
			PROTECT(tmp = NEW_STRING(1));
			nProtected ++;
			SET_ELEMENT(rv_ans, 14, tmp);
			if (dynamic_cast<CdGDSVirtualFolder*>(Obj))
			{
				CdGDSVirtualFolder *v = (CdGDSVirtualFolder*)Obj;
				v->IsLoaded(true);
				SET_STRING_ELT(tmp, 0, mkChar(v->ErrMsg().c_str()));
			} else
				SET_STRING_ELT(tmp, 0, mkChar(""));

			// 16: param
			tmp = R_NilValue;
			if (IsPackedReal(Obj))
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
				} else if (dynamic_cast<CdPackedReal8U*>(Obj))
				{
					CdPackedReal8U *v = static_cast<CdPackedReal8U*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else if (dynamic_cast<CdPackedReal16*>(Obj))
				{
					CdPackedReal16 *v = static_cast<CdPackedReal16*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else if (dynamic_cast<CdPackedReal16U*>(Obj))
				{
					CdPackedReal16U *v = static_cast<CdPackedReal16U*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else if (dynamic_cast<CdPackedReal24*>(Obj))
				{
					CdPackedReal24 *v = static_cast<CdPackedReal24*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else if (dynamic_cast<CdPackedReal24U*>(Obj))
				{
					CdPackedReal24U *v = static_cast<CdPackedReal24U*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else if (dynamic_cast<CdPackedReal32*>(Obj))
				{
					CdPackedReal32 *v = static_cast<CdPackedReal32*>(Obj);
					SET_ELEMENT(tmp, 0, ScalarReal(v->Offset()));
					SET_ELEMENT(tmp, 1, ScalarReal(v->Scale()));
				} else {
					CdPackedReal32U *v = static_cast<CdPackedReal32U*>(Obj);
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
			SET_ELEMENT(rv_ans, 15, tmp);

		UNPROTECT(nProtected);

	COREARRAY_CATCH
}


/// Add a new node
/** \param Node        [in] a GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param Val         [in] the values
 *  \param Storage     [in] the mode of storage
 *  \param ValDim      [in] the dimension
 *  \param Compress    [in] the method of compression
 *  \param CloseZip    [in] if compressing data and TRUE, get into read mode after adding
 *  \param Check       [in] if TRUE, check data compatibility
 *  \param Replace     [in] if TRUE, replace the existing variable silently
 *  \param Visible     [in] if TRUE, visible or hidden
 *  \param Param       [in] list(...), additional parameters
**/
COREARRAY_DLL_EXPORT SEXP gdsAddNode(SEXP Node, SEXP NodeName, SEXP Val,
	SEXP Storage, SEXP ValDim, SEXP Compress, SEXP CloseZip, SEXP Check,
	SEXP Replace, SEXP Visible, SEXP Param)
{
	static const char *ERR_UNUSED =
		"Unused additional parameters (...) in 'add.gdsn'!";
	static const char *FixedString[] =
	{
		TdTraits< FIXED_LEN<C_UTF8>  >::StreamName(),
		TdTraits< FIXED_LEN<C_UTF16> >::StreamName(),
		TdTraits< FIXED_LEN<C_UTF32> >::StreamName(),
		NULL
	};
	static const char *PackedReal[] =
	{
		TdTraits< TReal8  >::StreamName(),
		TdTraits< TReal8u >::StreamName(),
		TdTraits< TReal16 >::StreamName(),
		TdTraits< TReal16u >::StreamName(),
		TdTraits< TReal24 >::StreamName(),
		TdTraits< TReal24u >::StreamName(),
		TdTraits< TReal32 >::StreamName(),
		TdTraits< TReal32u >::StreamName(),
		NULL
	};

	const char *nm  = translateCharUTF8(STRING_ELT(NodeName, 0));
	const char *stm = CHAR(STRING_ELT(Storage,  0));
	const char *cp  = CHAR(STRING_ELT(Compress, 0));
	if (!Rf_isNull(ValDim) && !Rf_isNumeric(ValDim))
		error("'valdim' should be NULL or a numeric vector");

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
			if (XLENGTH(Param) > 1) error("%s", ERR_UNUSED);
		} else {
			if (XLENGTH(Param) > 0) error("%s", ERR_UNUSED);
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
			if (XLENGTH(Param) > n) error("%s", ERR_UNUSED);
		} else {
			if (XLENGTH(Param) > 0) error("%s", ERR_UNUSED);
		}
	} else {
		if (!Rf_isNull(Param))
		{
			if (XLENGTH(Param) > 0) error("%s", ERR_UNUSED);
		}
	}


	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			throw ErrGDSFmt(ERR_NOT_FOLDER);
		CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
		int IdxReplace = -1;

		if (Rf_asLogical(Replace) == TRUE)
		{
			CdGDSObj *tmp = Dir.ObjItemEx(nm);
			if (tmp)
			{
				IdxReplace = Dir.IndexObj(tmp);
				GDS_Node_Delete(tmp, TRUE);
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
			throw ErrGDSFmt("No support of the storage mode '%s'.", stm);

		Dir.InsertObj(IdxReplace, nm, rv_obj);

		// hidden flag
		if (Rf_asLogical(Visible) != TRUE)
		{
			// compatible:
			// only available for >= gdsfmt_1.5.16
			rv_obj->SetHidden(true);
			// only 'R.invisible' can be recognized by all versions of gdsfmt
			rv_obj->Attribute().Add(STR_INVISIBLE);
		}

		// data compression mode
		if (dynamic_cast<CdGDSObjPipe*>(rv_obj))
		{
			CdGDSObjPipe *obj = static_cast<CdGDSObjPipe*>(rv_obj);
			obj->SetPackedMode(cp);
			// check compression parameters
			if (obj->PipeInfo())
			{
				if (strcmp(obj->PipeInfo()->Coder(), "LZMA_ra") == 0)
				{
					string s = obj->PipeInfo()->CoderParam();
					if (s.find("16K") != string::npos)
						warning("LZMA_ra:16K is almost equivalent to LZMA_ra:32K.");
				}
			}
		}

		// output value
		rv_ans = GDS_R_Obj2SEXP(rv_obj);
		PROTECT(rv_ans);

		// if it is an array
		if (dynamic_cast<CdAbstractArray*>(rv_obj))
		{
			// set the compression mode
			CdAbstractArray *Obj = static_cast<CdAbstractArray*>(rv_obj);

			// check first
			if ((dynamic_cast<CdFStr8*>(rv_obj) ||
				dynamic_cast<CdFStr16*>(rv_obj) ||
				dynamic_cast<CdFStr32*>(rv_obj)) && Rf_isString(Val))
			{
				int MaxLen = FixStr_Len;
				if (!Rf_isNull(Val))
				{
					R_xlen_t len = XLENGTH(Val);
					for (R_xlen_t i=0; i < len; i++)
					{
						const char *s = translateCharUTF8(STRING_ELT(Val, i));
						int L = strlen(s);
						if (L > MaxLen) MaxLen = L;
					}
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
			} else if (dynamic_cast<CdPackedReal8U*>(rv_obj))
			{
				CdPackedReal8U *obj = static_cast<CdPackedReal8U*>(rv_obj);
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
			} else if (dynamic_cast<CdPackedReal16U*>(rv_obj))
			{
				CdPackedReal16U *obj = static_cast<CdPackedReal16U*>(rv_obj);
				if (R_FINITE(FixedReal_Offset))
					obj->SetOffset(FixedReal_Offset);
				if (R_FINITE(FixedReal_Scale))
					obj->SetScale(FixedReal_Scale);
			} else if (dynamic_cast<CdPackedReal24*>(rv_obj))
			{
				CdPackedReal24 *obj = static_cast<CdPackedReal24*>(rv_obj);
				if (R_FINITE(FixedReal_Offset))
					obj->SetOffset(FixedReal_Offset);
				if (R_FINITE(FixedReal_Scale))
					obj->SetScale(FixedReal_Scale);
			} else if (dynamic_cast<CdPackedReal24U*>(rv_obj))
			{
				CdPackedReal24U *obj = static_cast<CdPackedReal24U*>(rv_obj);
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
			} else if (dynamic_cast<CdPackedReal32U*>(rv_obj))
			{
				CdPackedReal32U *obj = static_cast<CdPackedReal32U*>(rv_obj);
				if (R_FINITE(FixedReal_Offset))
					obj->SetOffset(FixedReal_Offset);
				if (R_FINITE(FixedReal_Scale))
					obj->SetScale(FixedReal_Scale);
			}

			if (!Rf_isNull(Val))
			{
				if (Rf_isNumeric(Val) || Rf_isString(Val) ||
					Rf_isLogical(Val) || Rf_isFactor(Val) ||
					(TYPEOF(Val) == RAWSXP))
				{
					// call write all
					gdsObjWriteAll(rv_ans, Val, Check);

					// close the compression
					if (Obj->PipeInfo() && (Rf_asLogical(CloseZip)==TRUE))
						Obj->CloseWriter();

					// set dimensions
					if (!Rf_isNull(ValDim))
						gdsObjSetDim(rv_ans, ValDim, ScalarLogical(FALSE));
				} else
					throw ErrGDSFmt("No support of 'val'.");
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


/// Add a new (virtual) folder
/** \param Node        [in] a GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param Type        [in] the type of folder
 *  \param GDS_fn      [in] the file name of an existing GDS file
 *  \param Replace     [in] if TRUE, replace the existing variable silently
 *  \param Visible     [in] if TRUE, visible or hidden
**/
COREARRAY_DLL_EXPORT SEXP gdsAddFolder(SEXP Node, SEXP NodeName, SEXP Type,
	SEXP GDS_fn, SEXP Replace, SEXP Visible)
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

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			throw ErrGDSFmt(ERR_NOT_FOLDER);

		CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
		int IdxReplace = -1;
		if (replace_flag)
		{
			CdGDSObj *tmp = Dir.ObjItemEx(nm);
			if (tmp)
			{
				IdxReplace = Dir.IndexObj(tmp);
				GDS_Node_Delete(tmp, TRUE);
			}
		}

		PdGDSObj vObj = NULL;
		if (strcmp(tp, "directory") == 0)
		{
			vObj = Dir.AddFolder(nm);
		} else if (strcmp(tp, "virtual") == 0)
		{
			CdGDSVirtualFolder *F = new CdGDSVirtualFolder;
			Dir.InsertObj(IdxReplace, nm, F);
			F->SetLinkFile(UTF8Text(fn));
			vObj = F;
		} else
			throw ErrGDSFmt("Invalid 'type = %s'.", tp);

		// hidden flag
		if (Rf_asLogical(Visible) != TRUE)
		{
			// compatible:
			// only available for >= gdsfmt_1.5.16
			vObj->SetHidden(true);
			// only 'R.invisible' can be recognized by all versions of gdsfmt
			vObj->Attribute().Add(STR_INVISIBLE);
		}

		rv_ans = GDS_R_Obj2SEXP(vObj);

	COREARRAY_CATCH
}


/// Add a new node with a GDS file
/** \param Node        [in] a GDS node
 *  \param NodeName    [in] the name of a new node
 *  \param FileName    [in] the name of input file
 *  \param Compress    [in] the method of compression
 *  \param Replace     [in] if TRUE, replace the existing variable silently
 *  \param Visible     [in] if TRUE, visible or hidden
**/
COREARRAY_DLL_EXPORT SEXP gdsAddFile(SEXP Node, SEXP NodeName, SEXP FileName,
	SEXP Compress, SEXP Replace, SEXP Visible)
{
	const char *nm = translateCharUTF8(STRING_ELT(NodeName, 0));
	const char *fn = CHAR(STRING_ELT(FileName, 0));
	const char *cp = CHAR(STRING_ELT(Compress, 0));

	int replace_flag = Rf_asLogical(Replace);
	if (replace_flag == NA_LOGICAL)
		error("'replace' must be TRUE or FALSE.");

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		if (!dynamic_cast<CdGDSAbsFolder*>(Obj))
			throw ErrGDSFmt(ERR_NOT_FOLDER);

		CdGDSAbsFolder &Dir = *((CdGDSAbsFolder*)Obj);
		int IdxReplace = -1;
		if (replace_flag)
		{
			CdGDSObj *tmp = Dir.ObjItemEx(nm);
			if (tmp)
			{
				IdxReplace = Dir.IndexObj(tmp);
				GDS_Node_Delete(tmp, TRUE);
			}
		}

		TdAutoRef<CdBufStream> file(new CdBufStream(
			new CdFileStream(fn, CdFileStream::fmOpenRead)));
		CdGDSStreamContainer *vObj = new CdGDSStreamContainer();
		vObj->SetPackedMode(cp);
		Dir.InsertObj(IdxReplace, nm, vObj);
		vObj->CopyFromBuf(*file.get());
		vObj->CloseWriter();

		// hidden flag
		if (Rf_asLogical(Visible) != TRUE)
		{
			// compatible:
			// only available for >= gdsfmt_1.5.16
			vObj->SetHidden(true);
			// only 'R.invisible' can be recognized by all versions of gdsfmt
			vObj->Attribute().Add(STR_INVISIBLE);
		}

		rv_ans = GDS_R_Obj2SEXP(vObj);

	COREARRAY_CATCH
}


/// Get the file from a file node
/** \param Node        [in] a GDS node
 *  \param OutFileName [in] the name for output file
**/
COREARRAY_DLL_EXPORT SEXP gdsGetFile(SEXP Node, SEXP OutFileName)
{
	const char *fn = CHAR(STRING_ELT(OutFileName, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		if (!dynamic_cast<CdGDSStreamContainer*>(Obj))
			throw ErrGDSFmt("It is not a stream container!");

		CdGDSStreamContainer *_Obj = static_cast<CdGDSStreamContainer*>(Obj);
		TdAutoRef<CdBufStream> file(new CdBufStream(
			new CdFileStream(fn, CdFileStream::fmCreate)));
		_Obj->CopyTo(*file.get());

	COREARRAY_CATCH
}


/// Delete a node
/** \param Node        [in] a GDS node
 *  \param Force       [in] if TRUE, remove a folder no matter whether it is empty
**/
COREARRAY_DLL_EXPORT SEXP gdsDeleteNode(SEXP Node, SEXP Force)
{
	int force_flag = Rf_asLogical(Force);
	if (force_flag == NA_LOGICAL)
		error("'force' must be TRUE or FALSE.");

	COREARRAY_TRY
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		GDS_Node_Delete(Obj, force_flag);
	COREARRAY_CATCH
}


/// Unload a node
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsUnloadNode(SEXP Node)
{
	COREARRAY_TRY
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		GDS_Node_Unload(Obj);
	COREARRAY_CATCH
}


/// Rename a node
/** \param Node        [in] a GDS node
 *  \param NewName     [in] the new name
**/
COREARRAY_DLL_EXPORT SEXP gdsRenameNode(SEXP Node, SEXP NewName)
{
	const char *nm = translateCharUTF8(STRING_ELT(NewName, 0));

	COREARRAY_TRY
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		Obj->SetName(nm);
	COREARRAY_CATCH
}




// ----------------------------------------------------------------------------
// Attribute Operations
// ----------------------------------------------------------------------------

/// Get the attribute(s) of a GDS node
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsGetAttr(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
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
								mkCharCE(p->GetStr8().c_str(), CE_UTF8));
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


/// Set an attribute
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

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);

		CdAny *p;
		if (Obj->Attribute().HasName(nm))
		{
			p = &(Obj->Attribute()[nm]);
			Obj->Attribute().Changed();
		} else
			p = &(Obj->Attribute().Add(nm));

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


/// Set an attribute
/** \param Node        [in] a GDS node
 *  \param Source      [in] the source GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsPutAttr2(SEXP Node, SEXP Source)
{
	COREARRAY_TRY
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		PdGDSObj Src = GDS_R_SEXP2Obj(Source, TRUE);
		Obj->Attribute().Assign(Src->Attribute());
	COREARRAY_CATCH
}


/// Delete attribute(s)
/** \param Node        [in] a GDS node
 *  \param Name        [in] the name of attribute
**/
COREARRAY_DLL_EXPORT SEXP gdsDeleteAttr(SEXP Node, SEXP Name)
{
	COREARRAY_TRY
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		size_t num = XLENGTH(Name);
		for (size_t i=0; i < num; i++)
		{
			const char *nm = translateCharUTF8(STRING_ELT(Name, i));
			Obj->Attribute().Delete(nm);
		}
	COREARRAY_CATCH
}




// ----------------------------------------------------------------------------
// Data Operations
// ----------------------------------------------------------------------------

/// Read data from a node
/** \param Node        [in] a GDS node
 *  \param Start       [in] the starting position
 *  \param Count       [in] the count of each dimension
 *  \param Simplify    [in] convert to a vector if possible
 *  \param UseRaw      [in] if TRUE, use RAW if possible
 *  \param ValList     [in] a list of '.value' and '.substitute'
 *  \param Sparse      [in] if TRUE, return sparse matrix if possible
**/
COREARRAY_DLL_EXPORT SEXP gdsObjReadData(SEXP Node, SEXP Start, SEXP Count,
	SEXP Simplify, SEXP UseRaw, SEXP ValList, SEXP Sparse)
{
	extern SEXP gdsDataFmt(SEXP Result, SEXP Simplify, SEXP ValList);

	if (!Rf_isNull(Start) && !Rf_isNumeric(Start))
		error("'start' should be numeric.");
	if (!Rf_isNull(Count) && !Rf_isNumeric(Count))
		error("'count' should be numeric.");
	if ((Rf_isNull(Start) && !Rf_isNull(Count)) ||
			(!Rf_isNull(Start) && Rf_isNull(Count)))
		error("'start' and 'count' should be both NULL.");

	int use_raw_flag = Rf_asLogical(UseRaw);
	if (use_raw_flag == NA_LOGICAL)
		error("'.useraw' must be TRUE or FALSE.");

	int use_sparse = Rf_asLogical(Sparse);
	if (use_sparse == NA_LOGICAL)
		error("'.sparse' must be TRUE or FALSE.");

	// GDS object
	CdAbstractArray *Obj;
	{
		bool has_error = false;
		CORE_TRY
			Obj = dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node, TRUE));
			if (Obj == NULL)
				throw ErrGDSFmt(ERR_NO_DATA);
		CORE_CATCH(has_error = true);
		if (has_error) error("%s", GDS_GetError());
	}

	CdAbstractArray::TArrayDim DStart, DLen;
	C_Int32 *pDS=NULL, *pDL=NULL;
	if (!Rf_isNull(Start) && !Rf_isNull(Count))
	{
		int Len = Obj->DimCnt();
		CdAbstractArray::TArrayDim DCnt;
		Obj->GetDim(DCnt);

		PROTECT(Start = Rf_coerceVector(Start, INTSXP));
		if (XLENGTH(Start) != Len)
			error("The length of 'start' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Start)[i];
			if ((v < 1) || (v > DCnt[Len-i-1]))
				error("'start' is invalid.");
			DStart[Len-i-1] = v-1;
		}
		pDS = DStart;

		PROTECT(Count = Rf_coerceVector(Count, INTSXP));
		if (XLENGTH(Count) != Len)
			error("The length of 'count' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Count)[i];
			if (v == -1)
				v = DCnt[Len-i-1] - DStart[Len-i-1];
			if ((v < 0) || ((DStart[Len-i-1]+v) > DCnt[Len-i-1]))
				error("'count' is invalid.");
			DLen[Len-i-1] = v;
		}
		pDL = DLen;

		UNPROTECT(2);
	}

	// read data
	COREARRAY_TRY

		rv_ans = GDS_R_Array_Read(Obj, pDS, pDL, NULL,
			GDS_R_READ_DEFAULT_MODE |
			(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0) |
			(use_sparse ? GDS_R_READ_ALLOW_SP_MATRIX : 0));
		gdsDataFmt(rv_ans, Simplify, ValList);

	COREARRAY_CATCH
}


/// Read data from a node with a selection
/** \param Node        [in] a GDS node
 *  \param Selection   [in] the logical variable of selection
 *  \param UseRaw      [in] if TRUE, use RAW if possible
 *  \param Index       [out]
 *  \param Sparse      [in] if TRUE, return sparse matrix if possible
**/
COREARRAY_DLL_EXPORT SEXP gdsObjReadExData(SEXP Node, SEXP Selection,
	SEXP UseRaw, SEXP Index, SEXP Sparse)
{
	int use_raw_flag = Rf_asLogical(UseRaw);
	if (use_raw_flag == NA_LOGICAL)
		error("'.useraw' must be TRUE or FALSE.");

	int use_sparse = Rf_asLogical(Sparse);
	if (use_sparse == NA_LOGICAL)
		error("'.sparse' must be TRUE or FALSE.");

	COREARRAY_TRY

		// GDS object
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
		CdAbstractArray *_Obj = dynamic_cast<CdAbstractArray*>(Obj);
		if (_Obj == NULL)
			throw ErrGDSFmt(ERR_NO_DATA);

		int nProtected = 0;
		vector< vector<C_BOOL> > Select;
		SEXP MatIdx = VECTOR_ELT(Index, 0);

		if (Rf_isVectorList(Selection))
		{
			if (XLENGTH(Selection) != _Obj->DimCnt())
				throw ErrGDSFmt("The dimension of 'sel' is not correct.");

			Select.resize(_Obj->DimCnt());
			for (R_xlen_t i=0; i < XLENGTH(Selection); i++)
			{
				SEXP tmp = VECTOR_ELT(Selection, i);
				int k = _Obj->DimCnt() - i - 1;
				if (Rf_isLogical(tmp))
				{
					R_xlen_t Len = _Obj->GetDLen(k);
					if (XLENGTH(tmp) != Len)
					{
						throw ErrGDSFmt(
							"The length of 'sel[[%d]]' is not correct.", i+1);
					}
					Select[k].resize(Len);
					ValCvtArray<C_BOOL, C_Int32>(&(Select[k][0]), LOGICAL(tmp), Len);

				} else if (Rf_isInteger(tmp) || Rf_isReal(tmp))
				{
					if (Rf_isReal(tmp))
					{
						tmp = PROTECT(AS_INTEGER(tmp));
						nProtected ++;
					}

					R_xlen_t Len = _Obj->GetDLen(k);
					Select[k].resize(Len);
					C_BOOL *pSel = &(Select[k][0]);

					// if length(tmp)=0, get NA_INTEGER
					int first_value = Rf_asInteger(tmp);
					if ((first_value == NA_INTEGER) || (first_value >= 0))
					{
						// NA and/or positive subscripts
						memset(pSel, FALSE, Len);
						bool increasing = true;
						int last = -1;
						int *p = INTEGER(tmp);
						for (R_xlen_t n=XLENGTH(tmp); n > 0; n--)
						{
							int I = *p ++;
							if (I != NA_INTEGER)
							{
								if ((I == 0) || (I > Len))
								{
									throw ErrGDSFmt(
										"Invalid index in 'sel[[%d]]'", i + 1);
								} else if (I < 0)
								{
									throw ErrGDSFmt(
										"'sel[[%d]]': positive subscripts should "
										"not be mixed with negative subscripts.",
										i + 1);
								}
								if (I <= last) increasing = false;
								last = I;
								pSel[I - 1] = TRUE;
							} else
								increasing = false;
						}
						if (!increasing)
						{
							if (Rf_isNull(MatIdx))
							{
								MatIdx = PROTECT(NEW_LIST(_Obj->DimCnt() + 2));
								nProtected ++;
								SET_VECTOR_ELT(Index, 0, MatIdx);
							}
							SET_VECTOR_ELT(MatIdx, i+1, tmp);
						}
					} else {
						// NA and/or positive subscripts
						memset(pSel, TRUE, Len);
						int *p = INTEGER(tmp);
						for (R_xlen_t n=XLENGTH(tmp); n > 0; n--)
						{
							int I = *p ++;
							if ((I >= 0) || (I == NA_INTEGER))
							{
								throw ErrGDSFmt(
									"'sel[[%d]]': negative subscripts should "
									"not be mixed with positive subscripts, NA and 0.",
									i + 1);
							} else if (I < -Len)
							{
								throw ErrGDSFmt(
									"Invalid index in 'sel[[%d]]'", i + 1);
							} else
								pSel[-I - 1] = FALSE;
						}
					}

				} else if (Rf_isNull(tmp))
				{
					R_xlen_t Len = _Obj->GetDLen(k);
					Select[k].resize(Len);
					memset(&(Select[k][0]), TRUE, Len);
				} else {
					throw ErrGDSFmt(
						"'sel[[%d]]' should be a logical variable or NULL.",
						i+1);
				}
			}
		} else
			throw ErrGDSFmt("'sel' should be a list or a logical variable.");

		// set the selection
		vector<C_BOOL*> SelList(_Obj->DimCnt());
		for (int i=0; i < _Obj->DimCnt(); i++)
			SelList[i] = &(Select[i][0]);

		// read data
		rv_ans = GDS_R_Array_Read(_Obj, NULL, NULL, &(SelList[0]),
			GDS_R_READ_DEFAULT_MODE |
			(use_raw_flag ? GDS_R_READ_ALLOW_RAW_TYPE : 0) |
			(use_sparse ? GDS_R_READ_ALLOW_SP_MATRIX : 0));

		// set the variable 'idx' in `readex.gdsn()`
		if (!Rf_isNull(MatIdx))
		{
			SET_VECTOR_ELT(MatIdx, 0, rv_ans);
			vector<int> BufIdx;
			for (R_xlen_t i=1; i <= XLENGTH(Selection); i++)
			{
				// assign the correct index according to the subset
				int k = _Obj->DimCnt() - i;
				size_t SIZE = _Obj->GetDLen(k);
				BufIdx.resize(SIZE);
				int *p = &BufIdx[0];
				C_BOOL *s = SelList[k];
				int v = 0;
				for (; SIZE > 0; SIZE--)
				{
					if (*s++) *p = (++v);
					p ++;
				}
				SEXP sel = VECTOR_ELT(MatIdx, i);
				if (!Rf_isNull(sel))
				{
					SEXP II = PROTECT(NEW_INTEGER(XLENGTH(sel)));
					nProtected ++;
					int *p = INTEGER(II);
					int *s = INTEGER(sel);
					for (size_t n=XLENGTH(sel); n > 0; n--)
					{
						if (*s != NA_INTEGER)
							*p++ = BufIdx[*s - 1];
						else
							*p++ = NA_INTEGER;
						s ++;
					}
					SET_VECTOR_ELT(MatIdx, i, II);
				} else {
					SET_VECTOR_ELT(MatIdx, i, ScalarLogical(TRUE));
				}
			}

			// drop=FALSE, in `[`
			int last = XLENGTH(MatIdx) - 1;
			SET_VECTOR_ELT(MatIdx, last, ScalarLogical(FALSE));

			SEXP nm = PROTECT(NEW_STRING(last+1));
			nProtected ++;
			SEXP s = mkChar("");
			for (int i=0; i < last; i++)
				SET_STRING_ELT(nm, i, s);
			SET_STRING_ELT(nm, last, mkChar("drop"));
			SET_NAMES(MatIdx, nm);
		}

		UNPROTECT(nProtected);

	COREARRAY_CATCH
}


/// Reformat data
/** \param Data        [in] the data returned from gdsObjReadExData etc
 *  \param ValList     [in] a list of '.value' and '.substitute'
 *  \param st          [in] the start index for '.value'
**/
COREARRAY_DLL_LOCAL void _GDS_DataFmt(SEXP Data, SEXP ValList, size_t st)
{
	int nProtected = 0;
	PROTECT(Data); nProtected ++;

	// the original values in Result
	SEXP Value = VECTOR_ELT(ValList, st+0);
	// the values replaced
	SEXP ValReplaced = VECTOR_ELT(ValList, st+1);

	if (!Rf_isNull(Value))
	{
		R_xlen_t nVal    = XLENGTH(Value);
		R_xlen_t nValRep = XLENGTH(ValReplaced);
		if ((nValRep != 1) && (nValRep != nVal))
			error("`length(.substitute)` must be ONE or `length(.value)`.");

		if (Rf_isS4(Data))
		{
			if (Rf_inherits(Data, "dgCMatrix"))
				Data = GET_SLOT(Data, mkString("x"));
			else
				error("Unknown type for replacing values.");
		}

		#define REPLACE_HEADER(SEXP_TYPE, TYPE, FUNC)    \
			if (TYPEOF(Value) != SEXP_TYPE) \
			{ \
				Value = PROTECT(coerceVector(Value, SEXP_TYPE)); \
				nProtected ++; \
				SET_VECTOR_ELT(ValList, st+0, Value); \
			} \
			if (TYPEOF(ValReplaced) != SEXP_TYPE) \
			{ \
				ValReplaced = PROTECT(coerceVector(ValReplaced, SEXP_TYPE)); \
				nProtected ++; \
				SET_VECTOR_ELT(ValList, st+1, ValReplaced); \
			} \
			TYPE *pValue = FUNC(Value); \
			TYPE *pValRep = FUNC(ValReplaced); \
			TYPE *p = FUNC(Data); \
			for (R_xlen_t n = XLENGTH(Data); n > 0; n--, p++) \
			{ \
				TYPE *s = pValue; \
				R_xlen_t k = 0; \
				for (TYPE I = *p; k < nVal; k++)

		#define REPLACE_END    \
					if (*s++ == I) break; \
				if (k < nVal) \
					*p = (nValRep <= 1) ? pValRep[0] : pValRep[k]; \
			}

		#define REPLACE(SEXP_TYPE, TYPE, FUNC)    \
			REPLACE_HEADER(SEXP_TYPE, TYPE, FUNC) \
			REPLACE_END

		if (Rf_isInteger(Data))
		{
			REPLACE(INTSXP, int, INTEGER)
		} else if (Rf_isLogical(Data))
		{
			REPLACE(LGLSXP, int, LOGICAL)
		} else if (IS_RAW(Data))
		{
			REPLACE(RAWSXP, Rbyte, RAW)
		} else if (Rf_isReal(Data))
		{
			REPLACE_HEADER(REALSXP, double, REAL)
					if (EqaulFloat(*s++, I)) break;
				if (k < nVal)
					*p = (nValRep <= 1) ? pValRep[0] : pValRep[k];
			}
		} else if (IS_CHARACTER(Data))
		{
			if (TYPEOF(Value) != STRSXP)
			{
				Value = PROTECT(coerceVector(Value, STRSXP));
				nProtected ++;
				SET_VECTOR_ELT(ValList, st+0, Value);
			}
			if (TYPEOF(ValReplaced) != STRSXP)
			{
				ValReplaced = PROTECT(coerceVector(ValReplaced, STRSXP));
				nProtected ++;
				SET_VECTOR_ELT(ValList, st+1, ValReplaced);
			}
			SEXP ValRep1 = STRING_ELT(ValReplaced, 0);
			R_xlen_t n = XLENGTH(Data);
			for (R_xlen_t i=0; i < n; i++)
			{
				SEXP I = STRING_ELT(Data, i);
				R_xlen_t k = 0;
				for (; k < nVal; k++)
				{
					SEXP X = STRING_ELT(Value, k);
					if ((I != NA_STRING) && (X != NA_STRING))
					{
						if (strcmp(CHAR(I), CHAR(X)) == 0)
							break;
					} else if (I == X)
						break;
				}
				if (k < nVal)
				{
					SET_STRING_ELT(Data, i, (nValRep <= 1) ?
						ValRep1 : STRING_ELT(ValReplaced, k));
				}
			}
		}

	} else if (!Rf_isNull(ValReplaced))
	{
		error("'.substitute' must be NULL if '.value' is NULL.");
	}

	UNPROTECT(nProtected);
}

/// Reformat data
/** \param Data        [in] the data returned from gdsObjReadExData etc
 *  \param Simplify    [in] convert to a vector if possible
 *  \param ValList     [in] a list of '.value' and '.substitute'
**/
COREARRAY_DLL_EXPORT SEXP gdsDataFmt(SEXP Data, SEXP Simplify, SEXP ValList)
{
	const char *s = CHAR(STRING_ELT(Simplify, 0));
	if (strcmp(s, "auto") == 0)
	{
		SEXP dim = getAttrib(Data, R_DimSymbol);
		if (!Rf_isNull(dim))
		{
			int Num_GreaterOne = 0;
			int *pD = INTEGER(dim);
			for (R_xlen_t n=XLENGTH(dim); n > 0; n--)
			{
				if (*pD++ > 1)
					Num_GreaterOne ++;
			}
			if (Num_GreaterOne <= 1)
			{
				setAttrib(Data, R_DimSymbol, R_NilValue);
			} else {
				int NoOneLength = XLENGTH(dim);
				pD = INTEGER(dim) + NoOneLength - 1;
				for (int n=NoOneLength; n > 0; n--)
				{
					if ((*pD--) == 1)
						NoOneLength --;
					else
						break;
				}
				if ((NoOneLength < XLENGTH(dim)) && (NoOneLength > 0))
				{
					SEXP dm = NEW_INTEGER(NoOneLength);
					memcpy(INTEGER(dm), INTEGER(dim), NoOneLength*sizeof(int));
					setAttrib(Data, R_DimSymbol, dm);
				}
			}
		}
	} else if (strcmp(s, "force") == 0)
	{
		setAttrib(Data, R_DimSymbol, R_NilValue);
	}

	_GDS_DataFmt(Data, ValList, 0);
	return Data;
}


/// Append data to a node
/** \param Node        [in] a GDS node
 *  \param Val         [in] the values
 *  \param Check       [in] if TRUE check
**/
COREARRAY_DLL_EXPORT SEXP gdsObjAppend(SEXP Node, SEXP Val, SEXP Check)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val) && (TYPEOF(Val)!=RAWSXP))
		error("'val' should be integer, numeric, character, logical or raw.");
	
	int check_flag = Rf_asLogical(Check);
	if (check_flag == NA_LOGICAL)
		error("'check' must be TRUE or FALSE.");

	COREARRAY_TRY

		// GDS object
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		CdAbstractArray *_Obj = dynamic_cast<CdAbstractArray*>(Obj);
		if (_Obj == NULL)
			throw ErrGDSFmt(ERR_NO_DATA);

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
			throw ErrGDSFmt("No support!");

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


/// Append data of a GDS node to a node
/** \param Node        [in] a GDS node
 *  \param Src         [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsObjAppend2(SEXP Node, SEXP Src)
{
	COREARRAY_TRY

		PdGDSObj Dest = GDS_R_SEXP2Obj(Node, FALSE);
		PdGDSObj Source = GDS_R_SEXP2Obj(Src, TRUE);

		if (dynamic_cast<CdAbstractArray*>(Dest))
		{
			CdContainer *Array = static_cast<CdContainer*>(Source);
			C_Int64 Count = Array->TotalCount();
			CdIterator I = Array->IterBegin();
			static_cast<CdAbstractArray*>(Dest)->AppendIter(I, Count);
		} else
			throw ErrGDSFmt("No support of GDS node!");
	}
	catch (ErrAllocWrite &E) {
		GDS_SetError(ERR_READ_ONLY);
		has_error = true;

	COREARRAY_CATCH
}


/// Write data to a node
/** \param Node        [in] a GDS node
 *  \param Val         [in] the input values
 *  \param Check
**/
COREARRAY_DLL_EXPORT SEXP gdsObjWriteAll(SEXP Node, SEXP Val, SEXP Check)
{
	if (!Rf_isNumeric(Val) && !Rf_isString(Val) && !Rf_isLogical(Val) &&
			!Rf_isFactor(Val) && (TYPEOF(Val)!=RAWSXP))
		error("'val' should be integer, numeric, character, logical or raw.");

	int check_flag = Rf_asLogical(Check);
	if (check_flag == NA_LOGICAL)
		error("'check' must be TRUE or FALSE.");

	// GDS object
	CdAbstractArray *Obj;
	{
		bool has_error = false;
		CORE_TRY
			Obj = dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node, FALSE));
			if (Obj == NULL)
				throw ErrGDSFmt(ERR_NO_DATA);
		CORE_CATCH(has_error = true);
		if (has_error) error("%s", GDS_GetError());
	}

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
		error("No support!");

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
					"The length of 'val' should not be greater than %d.",
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
			throw ErrGDSFmt("No support!");

		UNPROTECT(nProtected);
	}
	catch (ErrAllocWrite &E) {
		GDS_SetError(ERR_READ_ONLY);
		has_error = true;

	COREARRAY_CATCH
}


/// Write data to a node
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
		error("'val' should be integer, numeric, character, logical or raw.");
	if (!Rf_isNull(Start) && !Rf_isNumeric(Start))
		error("'start' should be numeric.");
	if (!Rf_isNull(Count) && !Rf_isNumeric(Count))
		error("'count' should be numeric.");
	if ((Rf_isNull(Start) && !Rf_isNull(Count)) ||
			(!Rf_isNull(Start) && Rf_isNull(Count)))
		error("'start' and 'count' should be both NULL.");
	if (!Rf_isLogical(Check) || (XLENGTH(Check) <= 0))
		error("'check' should be a logical variable.");

	// GDS object
	CdAbstractArray *Obj;
	{
		bool has_error = false;
		CORE_TRY
			Obj = dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node, FALSE));
			if (Obj == NULL)
				throw ErrGDSFmt(ERR_NO_DATA);
		CORE_CATCH(has_error = true);
		if (has_error) error("%s", GDS_GetError());
	}

	CdAbstractArray::TArrayDim DStart, DLen;
	if (!Rf_isNull(Start) && !Rf_isNull(Count))
	{
		int Len = Obj->DimCnt();
		CdAbstractArray::TArrayDim DCnt;
		Obj->GetDim(DCnt);

		PROTECT(Start = Rf_coerceVector(Start, INTSXP));
		if (XLENGTH(Start) != Len)
			error("The length of 'start' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Start)[i];
			if ((v < 1) || (v > DCnt[Len-i-1]))
				error("'start' is invalid.");
			DStart[Len-i-1] = v-1;
		}

		PROTECT(Count = Rf_coerceVector(Count, INTSXP));
		if (XLENGTH(Count) != Len)
			error("The length of 'count' is invalid.");
		for (int i=0; i < Len; i++)
		{
			int v = INTEGER(Count)[i];
			if (v == -1) v = DCnt[Len-i-1];
			if ((v < 0) || ((DStart[Len-i-1]+v) > DCnt[Len-i-1]))
				error("'count' is invalid.");
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
			throw ErrGDSFmt("No support!");

		UNPROTECT(nProtected);
	}
	catch (ErrAllocWrite &E) {
		GDS_SetError(ERR_READ_ONLY);
		has_error = true;

	COREARRAY_CATCH
}


/// Assign data to a GDS variable
/** \param Dest        [in] a GDS node
 *  \param Src         [in] the number of dimension
**/
COREARRAY_DLL_EXPORT SEXP gdsAssign(SEXP Dest, SEXP Src)
{
	COREARRAY_TRY
		PdGDSObj DstObj = GDS_R_SEXP2Obj(Dest, FALSE);
		PdGDSObj SrcObj = GDS_R_SEXP2Obj(Src, TRUE);
		DstObj->Assign(*SrcObj, true);
	COREARRAY_CATCH
}


/// Set the dimension of data to a node
/** \param Node        [in] a GDS node
 *  \param DLen        [in] the new sizes of dimension
 *  \param Permute     [in] if TRUE, rearrange the elements
**/
COREARRAY_DLL_EXPORT SEXP gdsObjSetDim(SEXP Node, SEXP DLen, SEXP Permute)
{
	// permute
	int permute_flag = Rf_asLogical(Permute);
	if (permute_flag == NA_INTEGER)
		error("'permute' must be TRUE or FALSE.");

	// GDS object
	CdAbstractArray *Obj;
	{
		bool has_error = false;
		CORE_TRY
			Obj = dynamic_cast<CdAbstractArray*>(GDS_R_SEXP2Obj(Node, FALSE));
			if (Obj == NULL)
				throw ErrGDSFmt(ERR_NO_DATA);
		CORE_CATCH(has_error = true);
		if (has_error) error("%s", GDS_GetError());
	}

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


/// Set a new compression mode
/** \param Node        [in] a GDS node
 *  \param Compress    [in] the compression mode
**/
COREARRAY_DLL_EXPORT SEXP gdsObjCompress(SEXP Node, SEXP Compress)
{
	const char *cp = CHAR(STRING_ELT(Compress, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);

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


/// Close the compression mode if possible
/** \param Node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsObjCompressClose(SEXP Node)
{
	COREARRAY_TRY
		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		if (dynamic_cast<CdContainer*>(Obj))
			static_cast<CdContainer*>(Obj)->CloseWriter();
	COREARRAY_CATCH
}


/// Caching the data associated with a GDS variable
/** \param node        [in] a GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsCache(SEXP Node)
{
	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, TRUE);
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
 *  \param RelPos      [in] "after", "before", "replace", "replace+rename"
**/
COREARRAY_DLL_EXPORT SEXP gdsMoveTo(SEXP Node, SEXP LocNode, SEXP RelPos)
{
	const char *S = CHAR(STRING_ELT(RelPos, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		PdGDSObj LObj = GDS_R_SEXP2Obj(LocNode, TRUE);

		if (Obj->Folder() == LObj->Folder())
		{
			int i_Obj  = Obj->Folder()->IndexObj(Obj);
			int i_LObj = LObj->Folder()->IndexObj(LObj);
			if (i_Obj != i_LObj)
			{
				if ((strcmp(S, "after")==0) || (strcmp(S, "replace")==0) ||
					(strcmp(S, "replace+rename")==0))
				{
					if (i_Obj <= i_LObj)
						Obj->Folder()->MoveTo(i_Obj, i_LObj);
					else
						Obj->Folder()->MoveTo(i_Obj, i_LObj+1);
					if (strcmp(S, "replace") == 0)
					{
						GDS_Node_Delete(LObj, TRUE);
						GDS_R_Obj_SEXP2SEXP(LocNode, Node);
					} else if (strcmp(S, "replace+rename") == 0)
					{
						UTF8String nm = LObj->Name();
						GDS_Node_Delete(LObj, TRUE);
						Obj->SetName(nm);
						GDS_R_Obj_SEXP2SEXP(LocNode, Node);
					}
				} else if (strcmp(S, "before")==0)
				{
					if (i_Obj <= i_LObj)
						Obj->Folder()->MoveTo(i_Obj, i_LObj-1);
					else
						Obj->Folder()->MoveTo(i_Obj, i_LObj);
				} else
					throw ErrGDSFmt("Invalid 'relpos'!");
			}
		} else {
			throw ErrGDSFmt(
				"'node' and 'new.loc.node' should be in the same folder.");
		}

	COREARRAY_CATCH
}


/// Caching the data associated with a GDS variable
/** \param Node        [in] a GDS folder
 *  \param Name        [in] the name to be added
 *  \param Source      [in] the source GDS node
**/
COREARRAY_DLL_EXPORT SEXP gdsCopyTo(SEXP Node, SEXP Name, SEXP Source)
{
	const char *nm = CHAR(STRING_ELT(Name, 0));

	COREARRAY_TRY

		PdGDSObj Obj = GDS_R_SEXP2Obj(Node, FALSE);
		PdGDSObj SObj = GDS_R_SEXP2Obj(Source, TRUE);

		if (dynamic_cast<CdGDSAbsFolder*>(Obj))
		{
			if (dynamic_cast<CdGDSAbsFolder*>(SObj))
			{
				if (static_cast<CdGDSAbsFolder*>(SObj)->HasChild(Obj, true))
				{
					throw ErrGDSFmt(
						"Should not copy the node(s) into its sub folder.");
				}
			}
			CdGDSAbsFolder *Folder = static_cast<CdGDSAbsFolder*>(Obj);
			UTF8String s = nm;
			if (Folder->ObjItemEx(s) == NULL)
			{
				CdGDSObj *obj = Folder->AddObj(s, SObj->NewObject());
				obj->Assign(*SObj, true);
			} else
				throw ErrGDSFmt("Copy error: '%s' has exited.", nm);
		} else
			throw ErrGDSFmt("'node' should be a folder.");

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
		PdGDSObj tmp = GDS_R_SEXP2Obj(Node, TRUE);
		CdAbstractArray *Obj = dynamic_cast<CdAbstractArray*>(tmp);
		if (Obj)
		{
			// allocate memory
			size_t n = Obj->TotalCount();
			PROTECT(rv_ans = NEW_LOGICAL(n));

			int *pL = LOGICAL(rv_ans);
			C_BOOL *pB = (C_BOOL*)pL;
			GDS_R_Is_Element(Obj, SetEL, pB);

			pL += n; pB += n;
			for (; n > 0; n--) *(--pL) = *(--pB);

			// set dimension
			if (Obj->DimCnt() > 1)
			{
				CdAbstractArray::TArrayDim DCnt;
				Obj->GetDim(DCnt);
				const int ndim = Obj->DimCnt();
				SEXP dim = PROTECT(NEW_INTEGER(ndim));
				for (int i=0; i < ndim; i++)
					INTEGER(dim)[ndim-i-1] = DCnt[i];
				SET_DIM(rv_ans, dim);
				UNPROTECT(1);
			}

			UNPROTECT(1);

		} else
			throw ErrGDSFmt(ERR_NO_DATA);

	COREARRAY_CATCH
}


/// Test whether it is a sparse array or not
COREARRAY_DLL_EXPORT SEXP gdsIsSparse(SEXP Node)
{
	COREARRAY_TRY
		// GDS object
		PdGDSObj p = GDS_R_SEXP2Obj(Node, TRUE);
		rv_ans = ScalarLogical(dynamic_cast<CdSpExStruct*>(p) ? TRUE : FALSE);
	COREARRAY_CATCH
}


/// Test whether a path exists or not
COREARRAY_DLL_EXPORT SEXP gdsExistPath(SEXP Node, SEXP Path)
{
	COREARRAY_TRY
		// GDS object
		PdGDSObj p = GDS_R_SEXP2Obj(Node, TRUE);
		if (!dynamic_cast<CdGDSAbsFolder*>(p))
			throw ErrGDSFile("The node is not a folder.");
		CdGDSAbsFolder *folder = dynamic_cast<CdGDSAbsFolder*>(p);
		const int n = Rf_length(Path);
		rv_ans = PROTECT(NEW_LOGICAL(n));
		for (int i=0; i < n; i++)
		{
			SEXP s = STRING_ELT(Path, i);
			LOGICAL(rv_ans)[i] =
				folder->PathEx(translateCharUTF8(s)) ? TRUE : FALSE;
		}
		UNPROTECT(1);
	COREARRAY_CATCH
}


/// Get the last error message
COREARRAY_DLL_EXPORT SEXP gdsLastErrGDS()
{
	SEXP rv_ans = mkString(GDS_GetError());
	GDS_SetError(NULL);
	return rv_ans;
}


/// Get GDS system information
COREARRAY_DLL_EXPORT SEXP gdsSystem()
{
	COREARRAY_TRY

		int nProtect = 0;
		PROTECT(rv_ans = NEW_LIST(10));
		nProtect ++;
		SEXP nm = PROTECT(NEW_CHARACTER(10));
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
		SEXP Encoder = PROTECT(NEW_CHARACTER(4*n));
		nProtect ++;
		SET_ELEMENT(rv_ans, 6, Encoder);
		SET_STRING_ELT(nm, 6, mkChar("compression.encoder"));
		for (int i=0; i < n; i++)
		{
			const CdPipeMgrItem *p = dStreamPipeMgr.RegList()[i];
			SET_STRING_ELT(Encoder, 4*i+0, mkChar(p->Coder()));
			SET_STRING_ELT(Encoder, 4*i+1, mkChar(p->Description()));
			string s = p->CoderOptString();
			SET_STRING_ELT(Encoder, 4*i+2, mkChar(s.c_str()));
			s = p->ExtOptString();
			SET_STRING_ELT(Encoder, 4*i+3, mkChar(s.c_str()));
		}	

		// compiler
		SEXP Compiler = PROTECT(NEW_CHARACTER(2));
		nProtect ++;
		SET_ELEMENT(rv_ans, 7, Compiler);
		SET_STRING_ELT(nm, 7, mkChar("compiler"));
	#ifdef __VERSION__
		SET_STRING_ELT(Compiler, 0, mkChar(__VERSION__));
	#endif
	#ifdef __GNUC__
		char buf_compiler[128] = { 0 };
		#ifndef __GNUC_PATCHLEVEL__
		#   define __GNUC_PATCHLEVEL__    0
		#endif
		snprintf(buf_compiler, sizeof(buf_compiler), "GNUG_v%d.%d.%d",
			__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
		SET_STRING_ELT(Compiler, 1, mkChar(buf_compiler));
	#endif

		// compiler flags
		vector<string> ss;
	#ifdef COREARRAY_REGISTER_BIT64
		ss.push_back("64-bit");
	#else
		ss.push_back("32-bit");
	#endif
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
	#ifdef COREARRAY_SIMD_AVX512F
		ss.push_back("AVX512F");
	#endif
	#ifdef COREARRAY_SIMD_AVX512BW
		ss.push_back("AVX512BW");
	#endif
	#ifdef COREARRAY_SIMD_AVX512CD
		ss.push_back("AVX512CD");
	#endif
	#ifdef COREARRAY_SIMD_AVX512DQ
		ss.push_back("AVX512DQ");
	#endif
	#ifdef COREARRAY_SIMD_AVX512VL
		ss.push_back("AVX512VL");
	#endif
	#ifdef COREARRAY_SIMD_FMA
		ss.push_back("FMA");
	#endif
	#ifdef COREARRAY_SIMD_FMA4
		ss.push_back("FMA4");
	#endif
	#ifdef COREARRAY_POPCNT
		ss.push_back("POPCNT");
	#endif
		SEXP SIMD = PROTECT(NEW_CHARACTER(ss.size()));
		nProtect ++;
		SET_ELEMENT(rv_ans, 8, SIMD);
		SET_STRING_ELT(nm, 8, mkChar("compiler.flag"));
		for (int i=0; i < (int)ss.size(); i++)
			SET_STRING_ELT(SIMD, i, mkChar(ss[i].c_str()));

		// class list
		RegisterClass();
		vector<string> key, desp;
		dObjManager().GetClassDesp(key, desp);
		SEXP Key = PROTECT(NEW_CHARACTER(key.size()));
		nProtect ++;
		SEXP Desp = PROTECT(NEW_CHARACTER(desp.size()));
		nProtect ++;
		SEXP CL = PROTECT(NEW_LIST(2));
		nProtect ++;
		SET_ELEMENT(rv_ans, 9, CL);
		SET_STRING_ELT(nm, 9, mkChar("class.list"));
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
	ApplyStartIndex = Rf_asInteger(Idx);
	return R_NilValue;
}


struct COREARRAY_DLL_LOCAL TApplyStruct
{
	SEXP R_Nodes;         ///< R SEXP objects
	SEXP R_Fun;           ///< R SEXP user-defined function
	SEXP R_Func_Call;     ///< R SEXP object for 'eval'
	SEXP R_Rho;           ///< the environment variable
	SEXP R_ValList;       ///< the list of '.value' and '.val.list'
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

COREARRAY_INLINE static void _apply_param_index(SEXP Argument,
	TApplyStruct *p, C_Int32 MarginIdx)
{
	// index
	if (p->R_AccIdx != NULL)
		INTEGER(p->R_AccIdx)[0] = p->AccIdx + ApplyStartIndex;
	if (p->R_MarIdx != NULL)
		INTEGER(p->R_MarIdx)[0] = MarginIdx + 1;

	if (p->R_ValList)
	{
		if (TYPEOF(Argument) == VECSXP)
		{
			R_xlen_t n = XLENGTH(Argument);
			for (R_xlen_t i=0; i < n; i++)
				_GDS_DataFmt(VECTOR_ELT(Argument, i), p->R_ValList, i << 1);
		} else {
			_GDS_DataFmt(Argument, p->R_ValList, 0);
		}
	}
}

static void _apply_func_none(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

	// call R function
	eval(p->R_Func_Call, p->R_Rho);

	p->AccIdx ++;
}

static void _apply_func_list(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	// the object is bound to other symbol(s), need a copy
	SET_VECTOR_ELT(p->RV_List, p->AccIdx, duplicate(val));

	p->AccIdx ++;
}

static void _apply_func_integer(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	INTEGER(p->RV_List)[p->AccIdx] = Rf_asInteger(val);
	p->AccIdx ++;
}

static void _apply_func_double(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	REAL(p->RV_List)[p->AccIdx] = Rf_asReal(val);
	p->AccIdx ++;
}

static void _apply_func_char(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	SET_STRING_ELT(p->RV_List, p->AccIdx, Rf_asChar(val));
	p->AccIdx ++;
}

static void _apply_func_logical(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	LOGICAL(p->RV_List)[p->AccIdx] = Rf_asLogical(val);
	p->AccIdx ++;
}

static void _apply_func_raw(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

	// call R function
	SEXP val = eval(p->R_Func_Call, p->R_Rho);

	RAW(p->RV_List)[p->AccIdx] = Rf_asInteger(val);
	p->AccIdx ++;
}


inline static void _apply_func_gds_append(CdAbstractArray *Obj, SEXP val)
{
	ssize_t xl = XLENGTH(val);
	switch (TYPEOF(val))
	{
	case INTSXP:
		Obj->Append(INTEGER(val), xl, svInt32);
		break;
	case REALSXP:
		Obj->Append(REAL(val), xl, svFloat64);
		break;
	case RAWSXP:
		Obj->Append(RAW(val), xl, svInt8);
		break;
	case STRSXP:
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
			break;
		}
	default:
		if (!isNull(val))
		{
			throw ErrGDSFmt("the returned value from the user-defined function "
				"should be numeric, character or NULL.");
		}
	}
}

static void _apply_func_gdsnode(SEXP Argument, C_Int32 MarginIdx, void *_Param)
{
	TApplyStruct *p = (TApplyStruct*)_Param;
	_apply_param_index(Argument, p, MarginIdx);

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
				_apply_func_gds_append(
					p->pTarget[i], VECTOR_ELT(val, i));
			}
		}
	}

	p->AccIdx ++;
}


/// Called by 'apply.gdsn'
/** \param gds_nodes   [in] a list of objects of 'gdsn' class
 *  \param margins     [in] margin indices starting from 1
 *  \param FUN         [in] a function applied marginally
 *  \param selection   [in] indicates selection
 *  \param as_is       [in] the type of returned value
 *  \param var_index   [in] 1: none, 2: relative, 3: absolute
 *  \param target_node [in] target.node
 *  \param rho         [in] the environment variable
 *  \param use_raw     [in] whether use RAW to represent data
 *  \param ValList     [in] a list of '.value' and '.substitute'
**/
COREARRAY_DLL_EXPORT SEXP gdsApplyCall(SEXP gds_nodes, SEXP margins,
	SEXP FUN, SEXP selection, SEXP as_is, SEXP var_index, SEXP target_node,
	SEXP rho, SEXP use_raw, SEXP ValList)
{
	int use_raw_flag = Rf_asLogical(use_raw);
	if (use_raw_flag == NA_LOGICAL)
		error("'.useraw' must be TRUE or FALSE.");

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
			PdGDSObj Node = GDS_R_SEXP2Obj(VECTOR_ELT(gds_nodes, i), TRUE);
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
				PdGDSObj obj = GDS_R_SEXP2Obj(VECTOR_ELT(target_node, i), FALSE);
				if (dynamic_cast<CdAbstractArray*>(obj))
				{
					Targets[i] = static_cast<CdAbstractArray*>(obj);
				} else {
					throw ErrGDSFmt(
						"'target.node[[%d]]' should be array-oriented!",
						(int)i + 1);
				}
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

		if (isNull(VECTOR_ELT(ValList,0)) && isNull(VECTOR_ELT(ValList,1)))
		{
			a_struct.R_ValList = NULL;
		} else {
			if (nObject <= 1)
			{
				a_struct.R_ValList = ValList;
			} else {
				a_struct.R_ValList = PROTECT(NEW_LIST(nObject*2));
				a_struct.nProtected ++;
				for (int i=0; i < nObject; i++)
				{
					SET_VECTOR_ELT(a_struct.R_ValList, i*2 + 0,
						VECTOR_ELT(ValList, 0));
					SET_VECTOR_ELT(a_struct.R_ValList, i*2 + 1,
						VECTOR_ELT(ValList, 1));
				}
			}
		}

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


/// Called by 'clusterApply.gdsn', return a selection (list)
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
			PdGDSObj Node = GDS_R_SEXP2Obj(VECTOR_ELT(gds_nodes, i), TRUE);

			if (dynamic_cast<PdAbstractArray>(Node))
			{
				ObjList[i] = static_cast<PdAbstractArray>(Node);
			} else {
				throw ErrGDSFmt(
					"'node.names[[%d]]' should be array-based!", i+1);
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


/// format
COREARRAY_DLL_EXPORT SEXP gdsFmtSize(SEXP size_in_byte)
{
	const R_len_t n = XLENGTH(size_in_byte);
	size_in_byte = PROTECT(Rf_coerceVector(size_in_byte, REALSXP));
	SEXP rv_ans = PROTECT(NEW_CHARACTER(n));

	for (R_len_t i=0; i < n; i++)
	{
		double b = REAL(size_in_byte)[i];
		SET_STRING_ELT(rv_ans, i, mkChar(fmt_size(b).c_str()));
	}
	UNPROTECT(2);

	return rv_ans;
}


/// Get number of bytes and bits
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

	extern SEXP gdsDigest(SEXP, SEXP, SEXP);
	extern SEXP gdsSummary(SEXP);

	static R_CallMethodDef callMethods[] =
	{
		CALL(gdsCreateGDS, 2),          CALL(gdsOpenGDS, 5),
		CALL(gdsCloseGDS, 1),           CALL(gdsSyncGDS, 1),
		CALL(gdsTidyUp, 2),             CALL(gdsShowFile, 1),
		CALL(gdsDiagInfo, 2),           CALL(gdsDiagInfo2, 1),
		CALL(gdsFileSize, 1),

		CALL(gdsNodeChildCnt, 2),       CALL(gdsNodeName, 2),
		CALL(gdsRenameNode, 2),         CALL(gdsNodeEnumName, 4),
		CALL(gdsNodeIndex, 4),          CALL(gdsGetFolder, 1),
		CALL(gdsNodeObjDesp, 1),
		CALL(gdsAddNode, 11),           CALL(gdsAddFolder, 6),
		CALL(gdsAddFile, 6),            CALL(gdsGetFile, 2),
		CALL(gdsDeleteNode, 2),         CALL(gdsUnloadNode, 1),
		CALL(gdsNodeValid, 1),
		CALL(gdsAssign, 2),             CALL(gdsMoveTo, 3),
		CALL(gdsCopyTo, 3),             CALL(gdsCache, 1),

		CALL(gdsPutAttr, 3),            CALL(gdsPutAttr2, 2),
		CALL(gdsGetAttr, 1),            CALL(gdsDeleteAttr, 2),

		CALL(gdsObjCompress, 2),        CALL(gdsObjCompressClose, 1),
		CALL(gdsObjSetDim, 3),
		CALL(gdsObjAppend, 3),          CALL(gdsObjAppend2, 2),
		CALL(gdsObjReadData, 7),        CALL(gdsObjReadExData, 5),
		CALL(gdsObjWriteAll, 3),        CALL(gdsObjWriteData, 5),
		CALL(gdsDataFmt, 3),
	
		CALL(gdsApplySetStart, 1),      CALL(gdsApplyCall, 10),
		CALL(gdsApplyCreateSelection, 3),

		CALL(gdsIsElement, 2),          CALL(gdsLastErrGDS, 0),
		CALL(gdsSystem, 0),             CALL(gdsDigest, 3),
		CALL(gdsFmtSize, 1),            CALL(gdsSummary, 1),
		CALL(gdsReopenGDS, 1),

		{ NULL, NULL, 0 }
	};

	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}

} // extern "C"
