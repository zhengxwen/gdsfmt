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
 *	\file     R_GDS.h
 *	\author   Xiuwen Zheng
 *	\version  1.0
 *	\date     2014 - 2015
 *	\brief    C interface to gdsfmt dynamic library
 *	\details
**/

#ifndef _HEADER_R_GDS_
#define _HEADER_R_GDS_

#include <dType.h>

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
#   ifdef COREARRAY_GDSFMT_PACKAGE
#       include <CoreArray.h>
        using namespace CoreArray;
#   endif
extern "C" {
#endif

	// ==================================================================

	/// Version of R package gdsfmt: v1.3.7
	#define GDSFMT_R_VERSION       0x010307


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
	typedef void* PdAbstractArray;

	/// the class of mutex object
	typedef void* PdThreadMutex;
	/// the class of suspending object
	typedef void* PdThreadsSuspending;
	/// the class of thread object
	typedef void* PdThread;

	/// the class of block read
	typedef void* PdArrayRead;


	/// Iterator for CoreArray array-oriented container
	struct CdIterator
	{
		void *Allocator;
		C_Int64 Ptr;
		void *Container;
	};

	/// The pointer to an iterator
	typedef struct CdIterator *PdIterator;


	#endif  // COREARRAY_GDSFMT_PACKAGE
	// ]] ********



	// ==================================================================
	// ==================================================================

	/// the number of preserved integers for a pointer to a GDS object
	#define GDSFMT_NUM_INT_FOR_OBJECT    4

	/// the maximum number of GDS files
	#define GDSFMT_MAX_NUM_GDS_FILES     256

	/// To specify the mode of R data type, used in GDS_R_Array_Read
	#define GDS_R_READ_ALLOW_RAW_TYPE    0x01



	// ==================================================================
	// R objects

	/// convert "SEXP  --> (CdGDSFile*)"
	extern PdGDSFile GDS_R_SEXP2File(SEXP File);
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
	/// return an R data object from a GDS object, allowing raw-type data
	extern SEXP GDS_R_Array_Read(PdAbstractArray Obj, const C_Int32 *Start,
		const C_Int32 *Length, const C_BOOL *const Selection[],
		C_UInt32 UseMode);
	/// apply user-defined function margin by margin
	extern void GDS_R_Apply(int Num, PdAbstractArray ObjList[],
		int Margins[], const C_BOOL *const * const Selection[],
		void (*InitFunc)(SEXP Argument, C_Int32 Count,
			PdArrayRead ReadObjList[], void *_Param),
		void (*LoopFunc)(SEXP Argument, C_Int32 Idx, void *_Param),
		void *Param, C_BOOL IncOrDec, C_UInt32 UseMode);

	/// is.element
	extern void GDS_R_Is_Element(PdAbstractArray Obj, SEXP SetEL,
		C_BOOL Out[], size_t n_bool);



	// ==================================================================
	// File structure

	/// create a GDS file
	extern PdGDSFile GDS_File_Create(const char *FileName);
	/// open an existing GDS file
	extern PdGDSFile GDS_File_Open(const char *FileName, C_BOOL ReadOnly,
		C_BOOL ForkSupport);
	/// close the GDS file
	extern void GDS_File_Close(PdGDSFile File);
	/// synchronize the GDS file
	extern void GDS_File_Sync(PdGDSFile File);

	/// get the root folder of a GDS file
	extern PdGDSFolder GDS_File_Root(PdGDSFile File);
	/// get the GDS file from a GDS node
	extern PdGDSFile GDS_Node_File(PdGDSObj Node);
	/// get the class name of a GDS node
	extern void GDS_Node_GetClassName(PdGDSObj Node, char *Out, size_t OutSize);
	/// get the number of nodes in the folder
	extern int GDS_Node_ChildCount(PdGDSFolder Node);
	/// get a GDS file specified by a path
	extern PdGDSObj GDS_Node_Path(PdGDSFolder Node, const char *Path,
		C_BOOL MustExist);



	// ==================================================================
	// functions for attributes

	extern int GDS_Attr_Count(PdGDSObj Node);
	extern int GDS_Attr_Name2Index(PdGDSObj Node, const char *Name);



	// ==================================================================
	// functions for CdAbstractArray

	extern int GDS_Array_DimCnt(PdAbstractArray Obj);
	extern void GDS_Array_GetDim(PdAbstractArray Obj, C_Int32 OutBuffer[],
		size_t N_Buf);
	extern C_Int64 GDS_Array_GetTotalCount(PdAbstractArray Obj);
	extern enum C_SVType GDS_Array_GetSVType(PdAbstractArray Obj);
	extern unsigned GDS_Array_GetBitOf(PdAbstractArray Obj);
	extern void GDS_Array_ReadData(PdAbstractArray Obj, const C_Int32 *Start,
		const C_Int32 *Length, void *OutBuf, enum C_SVType OutSV);
	extern void GDS_Array_ReadDataEx(PdAbstractArray Obj, const C_Int32 *Start,
		const C_Int32 *Length, const C_BOOL *const Selection[], void *OutBuf,
		enum C_SVType OutSV);
	extern void GDS_Array_WriteData(PdAbstractArray Obj, const C_Int32 *Start,
		const C_Int32 *Length, const void *InBuf, enum C_SVType InSV);
	extern void GDS_Array_AppendData(PdAbstractArray Obj, ssize_t Cnt,
		const void *InBuf, enum C_SVType InSV);
	extern void GDS_Array_AppendString(PdAbstractArray Obj, const char *Text);



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
	extern void GDS_Iter_RData(PdIterator I, void *OutBuf, size_t Cnt,
		enum C_SVType OutSV);
	extern void GDS_Iter_WData(PdIterator I, const void *InBuf,
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

	extern PdArrayRead GDS_ArrayRead_Init(PdAbstractArray Obj,
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




	// ==================================================================
	/// compatible with <= gdsfmt_1.0.5
	// ==================================================================

	#define PdSequenceX            PdAbstractArray
	#define TdIterator             CdIterator

	#define GDS_Seq_DimCnt         GDS_Array_DimCnt
	#define GDS_Seq_GetDim         GDS_Array_GetDim
	#define GDS_Seq_GetTotalCount  GDS_Array_GetTotalCount
	#define GDS_Seq_GetSVType      GDS_Array_GetSVType
	#define GDS_Seq_GetBitOf       GDS_Array_GetBitOf
	#define GDS_Seq_rData          GDS_Array_ReadData
	#define GDS_Seq_rDataEx        GDS_Array_ReadDataEx
	#define GDS_Seq_wData          GDS_Array_WriteData
	#define GDS_Seq_AppendData     GDS_Array_AppendData
	#define GDS_Seq_AppendString   GDS_Array_AppendString
	#define GDS_Mach_GetNumOfCPU   GDS_Mach_GetNumOfCores


#ifdef __cplusplus
}
#endif

#endif /* _HEADER_R_GDS_ */
