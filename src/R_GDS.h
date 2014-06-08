// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// R_GDS.h: C interface to gdsfmt dynamic library
//
// Copyright (C) 2014	Xiuwen Zheng [zhengx@u.washington.edu]
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
 *	\file     R_GDS.h
 *	\author   Xiuwen Zheng
 *	\version  1.0
 *	\date     2014
 *	\brief    C interface to gdsfmt dynamic library
 *	\details
**/

#ifndef _HEADER_R_GDS_
#define _HEADER_R_GDS_

#include <dType.h>

#ifdef COREARRAY_GDSFMT_PACKAGE
#   include <CoreArray.h>
using namespace CoreArray;
#endif

#define STRICT_R_HEADERS
#include <R.h>
#include <Rdefines.h>

// to avoid the conflict with C++
#ifdef length
#   undef length
#endif

#ifdef Realloc
#   undef Realloc
#endif

// R_XLEN_T_MAX is defined, R >= v3.0
#ifndef R_XLEN_T_MAX
#   define R_xlen_t	R_len_t
#   define XLENGTH	Rf_length
#endif


#ifdef __cplusplus
extern "C" {
#endif

	// ==================================================================

	#define GDSFMT_PACKAGE_VERSION    1.05


	// [[ ********
	#ifndef COREARRAY_GDSFMT_PACKAGE

	// ====  the interface for CoreArray library  ====

	/// the pointer to a GDS file
	typedef void* PdGDSFile;
	/// the pionter to a GDS node
	typedef void* PdGDSObj;
	/// the pointer to a GDS folder
	typedef void* PdGDSFolder;
	/// the pointer to a container object
	typedef void* PdContainer;
	/// the pointer to a sequence object
	typedef void* PdSequenceX;

	/// the class of mutex object
	typedef void* PdThreadMutex;
	/// the class of suspending object
	typedef void* PdThreadsSuspending;
	/// the class of thread object
	typedef void* PdThread;

	/// the class of block read
	typedef void* PdArrayRead;


	/// Iterator for CoreArray array-oriented container
	/** sizeof(TdIterator) = 32 **/
	struct TdIterator
	{
		C_UInt8 VByte[32];
	};

	/// The pointer to an iterator
	typedef struct TdIterator* PdIterator;


	#endif  // COREARRAY_GDSFMT_PACKAGE
	// ]] ********



	// ==================================================================
	// ==================================================================

	/// the number of preserved integers for a pointer to a GDS object
	#define GDSFMT_NUM_INT_FOR_OBJECT    4

	/// the maximum number of GDS files
	#define GDSFMT_MAX_NUM_GDS_FILES     256




	// ==================================================================
	// R objects

	/// convert "SEXP  --> (CdGDSObj*)"
	extern PdGDSObj GDS_R_SEXP2Obj(SEXP Obj);
	/// convert "(CdGDSObj*)  -->  SEXP"
	extern SEXP GDS_R_Obj2SEXP(PdGDSObj Obj);
	/// detect whether a node is valid, true for reading
	extern void GDS_R_NodeValid(PdGDSObj Obj, C_BOOL ReadOrWrite);
	/// detect whether a node is valid, true for reading, not throw exception
	extern void GDS_R_NodeValid_SEXP(SEXP Obj, C_BOOL ReadOrWrite);
	/// return true, if Obj is a logical object in R
	extern C_BOOL GDS_R_Is_Logical(PdGDSObj Obj);
	/// return true, if Obj is a factor variable
	extern C_BOOL GDS_R_Is_Factor(PdGDSObj Obj);
	/// return 1 used in UNPROTECT and set levels in 'val' if Obj is a factor in R; otherwise return 0
	extern int GDS_R_Set_IfFactor(PdGDSObj Obj, SEXP val);
	/// return an R data object from a GDS object
	extern SEXP GDS_R_Array_Read(PdSequenceX Obj, C_Int32 const* Start,
		C_Int32 const* Length, const C_BOOL *const Selection[]);
	/// apply user-defined function margin by margin
	extern void GDS_R_Apply(int Num, PdSequenceX ObjList[],
		int Margins[], const C_BOOL *const * const Selection[],
		void (*InitFunc)(SEXP Argument, C_Int32 Count,
			PdArrayRead ReadObjList[], void *_Param),
		void (*LoopFunc)(SEXP Argument, C_Int32 Idx, void *_Param),
		void *Param, C_BOOL IncOrDec);
	/// is.element
	extern void GDS_R_Is_Element(PdSequenceX Obj, SEXP SetEL,
		C_BOOL Out[], size_t n_bool);


	// ==================================================================
	// functions for file structure

	extern PdGDSFile GDS_File_Create(const char *FileName);
	extern PdGDSFile GDS_File_Open(const char *FileName, C_BOOL ReadOnly,
		C_BOOL ForkSupport);
	extern void GDS_File_Close(PdGDSFile File);
	extern void GDS_File_Sync(PdGDSFile File);
	extern PdGDSFolder GDS_File_Root(PdGDSFile File);

	extern PdGDSFile GDS_Node_File(PdGDSObj Node);
	extern void GDS_Node_GetClassName(PdGDSObj Node, char *Out, size_t OutSize);
	extern int GDS_Node_ChildCount(PdGDSFolder Node);
	extern PdGDSObj GDS_Node_Path(PdGDSFolder Node, const char *Path,
		C_BOOL MustExist);


	// ==================================================================
	// functions for attributes

	extern int GDS_Attr_Count(PdGDSObj Node);
	extern int GDS_Attr_Name2Index(PdGDSObj Node, const char *Name);


	// ==================================================================
	// functions for CdSequenceX

	extern int GDS_Seq_DimCnt(PdSequenceX Obj);
	extern void GDS_Seq_GetDim(PdSequenceX Obj, C_Int32 OutBuffer[],
		size_t N_Buf);
	extern C_Int64 GDS_Seq_GetTotalCount(PdSequenceX Obj);
	extern enum C_SVType GDS_Seq_GetSVType(PdSequenceX Obj);
	extern unsigned GDS_Seq_GetBitOf(PdSequenceX Obj);
	extern void GDS_Seq_rData(PdSequenceX Obj, C_Int32 const* Start,
		C_Int32 const* Length, void *OutBuf, enum C_SVType OutSV);
	extern void GDS_Seq_rDataEx(PdSequenceX Obj, C_Int32 const* Start,
		C_Int32 const* Length, const C_BOOL *const Selection[], void *OutBuf,
		enum C_SVType OutSV);
	extern void GDS_Seq_wData(PdSequenceX Obj, C_Int32 const* Start,
		C_Int32 const* Length, const void *InBuf, enum C_SVType InSV);
	extern void GDS_Seq_AppendData(PdSequenceX Obj, ssize_t Cnt,
		const void *InBuf, enum C_SVType InSV);
	extern void GDS_Seq_AppendString(PdSequenceX Obj, const char *Text);


	// ==================================================================
	// functions for TdIterator

	extern void GDS_Iter_GetStart(PdContainer Node, PdIterator Out);
	extern void GDS_Iter_GetEnd(PdContainer Node, PdIterator Out);
	extern PdContainer GDS_Iter_GetHandle(PdIterator I);
	extern void GDS_Iter_Offset(PdIterator I, C_Int64 Offset);
	extern C_Int64 GDS_Iter_GetInt(PdIterator I);
	extern C_Float64 GDS_Iter_GetFloat(PdIterator I);
	extern void GDS_Iter_GetStr(PdIterator I, char *Out, size_t Size);
	extern void GDS_Iter_SetInt(PdIterator I, C_Int64 Val);
	extern void GDS_Iter_SetFloat(PdIterator I, C_Float64 Val);
	extern void GDS_Iter_SetStr(PdIterator I, const char *Str);
	extern size_t GDS_Iter_RData(PdIterator I, void *OutBuf, size_t Cnt,
		enum C_SVType OutSV);
	extern size_t GDS_Iter_WData(PdIterator I, const void *InBuf,
		size_t Cnt, enum C_SVType InSV);


	// ==================================================================
	// functions for error

	extern const char *GDS_GetError();
	extern void GDS_SetError(const char *Msg);


	// ==================================================================
	// functions for parallel computing

	extern PdThreadMutex GDS_Parallel_InitMutex();
	extern void GDS_Parallel_DoneMutex(PdThreadMutex Obj);
	extern void GDS_Parallel_LockMutex(PdThreadMutex Obj);
	extern void GDS_Parallel_UnlockMutex(PdThreadMutex Obj);
	extern PdThreadsSuspending GDS_Parallel_InitSuspend();
	extern void GDS_Parallel_DoneSuspend(PdThreadsSuspending Obj);
	extern void GDS_Parallel_Suspend(PdThreadsSuspending Obj);
	extern void GDS_Parallel_WakeUp(PdThreadsSuspending Obj);
	extern void GDS_Parallel_RunThreads(
		void (*Proc)(PdThread, int, void*), void *Param, int nThread);


	// ==================================================================
	// functions for machine

	/// Return the number of available (logical) cores in the system
	extern int GDS_Mach_GetNumOfCores();
	/// Return the size in byte of level-n cache memory
	extern C_UInt64 GDS_Mach_GetCPULevelCache(int level);


	// ==================================================================
	// functions for reading block by block

	extern PdArrayRead GDS_ArrayRead_Init(PdSequenceX Obj,
		int Margin, enum C_SVType SVType, const C_BOOL *const Selection[],
		C_BOOL buf_if_need);
	extern void GDS_ArrayRead_Free(PdArrayRead Obj);
	extern void GDS_ArrayRead_Read(PdArrayRead Obj, void *Buffer);
	extern C_BOOL GDS_ArrayRead_Eof(PdArrayRead Obj);
	extern void GDS_ArrayRead_BalanceBuffer(PdArrayRead array[],
		int n, C_Int64 buffer_size);



	// ==================================================================

	#ifndef COREARRAY_GDSFMT_PACKAGE

	/// initialize GDS interface
	extern void Init_GDS_Routines();

	#endif  // COREARRAY_GDSFMT_PACKAGE


#ifdef __cplusplus
}
#endif

#endif /* _HEADER_R_GDS_ */
