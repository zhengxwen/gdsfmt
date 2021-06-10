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
// Copyright (C) 2014-2021    Xiuwen Zheng
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
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2014 - 2021
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

	/// Version of R package gdsfmt: v1.28.0
	#define GDSFMT_R_VERSION       0x011C00


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
	/// the class of condition object
	typedef void* PdThreadCondition;
	/// the class of suspending object
	typedef void* PdThreadsSuspending;
	/// the class of thread object
	typedef void* PdThread;

	/// the class of block read
	typedef void* PdArrayRead;


	/// the iterator for CoreArray array-oriented container
	struct CdIterator
	{
		void *Allocator;
		C_Int64 Ptr;
		void *Container;
	};

	/// the pointer to an iterator
	typedef struct CdIterator *PdIterator;


	#endif  // COREARRAY_GDSFMT_PACKAGE
	// ]] ********



	// ==================================================================
	// ==================================================================

	/// the maximum number of GDS files
	#define GDSFMT_MAX_NUM_GDS_FILES     1024

	/// the maximun number of dimensions in GDS array
	#define GDS_MAX_NUM_DIMENSION        256

	/// the default mode of R data type, used in GDS_R_Array_Read
	#define GDS_R_READ_DEFAULT_MODE      0x00
	/// the mode of R data allowing raw type, used in GDS_R_Array_Read
	#define GDS_R_READ_ALLOW_RAW_TYPE    0x01
	/// the mode of R data allowing sparse matrix, used in GDS_R_Array_Read (requiring >= v1.23.6)
	#define GDS_R_READ_ALLOW_SP_MATRIX   0x10



	// ==================================================================
	// R objects

	// return from GDS_R_Is_ExtType (requiring >= v1.27.5)
	const int GDS_R_ExtType_Logical = 1;
	const int GDS_R_ExtType_Factor  = 2;

	/// convert "SEXP  --> (CdGDSFile*)"
	extern PdGDSFile GDS_R_SEXP2File(SEXP File);
	/// convert "SEXP  --> (CdGDSFolder*)" (requiring >= v1.5.4)
	extern PdGDSFolder GDS_R_SEXP2FileRoot(SEXP File);
	/// convert "SEXP  --> (CdGDSObj*)", true for read-only
	extern PdGDSObj GDS_R_SEXP2Obj(SEXP Obj, C_BOOL ReadOnly);
	/// convert "(CdGDSObj*)  -->  SEXP"
	extern SEXP GDS_R_Obj2SEXP(PdGDSObj Obj);
	/// convert "SEXP (ObjSrc)  -->  SEXP (ObjDst)", (requiring >= v1.5.8)
	extern void GDS_R_Obj_SEXP2SEXP(SEXP ObjDst, SEXP ObjSrc);
	/// return true, if Obj is a logical object in R
	extern C_BOOL GDS_R_Is_Logical(PdGDSObj Obj);
	/// return true, if Obj is a factor variable
	extern C_BOOL GDS_R_Is_Factor(PdGDSObj Obj);
	/// return 1 for logical, 2 for factor, 0 otherwise (requiring >= v1.27.5)
	extern int GDS_R_Is_ExtType(PdGDSObj Obj);
	/// return 1 used in UNPROTECT and set levels in 'Val' if Obj is a factor in R; otherwise return 0
	extern int GDS_R_Set_IfFactor(PdGDSObj Obj, SEXP Val);
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
	/// append R data
	extern void GDS_R_Append(PdAbstractArray Obj, SEXP Val);
	/// append R data with a range
	extern void GDS_R_AppendEx(PdAbstractArray Obj, SEXP Val, size_t Start,
		size_t Count);
	/// return whether the elements in SetEL
	extern void GDS_R_Is_Element(PdAbstractArray Obj, SEXP SetEL, C_BOOL Out[]);


	// ==================================================================
	// File structure

	/// create a GDS file
	extern PdGDSFile GDS_File_Create(const char *FileName);
	/// open an existing GDS file (the argument 'AllowError' requires >= 1.23.4)
	extern PdGDSFile GDS_File_Open(const char *FileName, C_BOOL ReadOnly,
		C_BOOL ForkSupport, C_BOOL AllowError);
	/// close the GDS file
	extern void GDS_File_Close(PdGDSFile File);
	/// synchronize the GDS file
	extern void GDS_File_Sync(PdGDSFile File);
	/// reopen the GDS file based on R object if needed (requiring >= 1.23.9)
	extern C_BOOL GDS_File_Reopen(SEXP GDSObj);
	/// get the root folder of a GDS file
	extern PdGDSFolder GDS_File_Root(PdGDSFile File);
	/// get the GDS file from a GDS node
	extern PdGDSFile GDS_Node_File(PdGDSObj Node);
	/// load a GDS node and get the internal ID from Path (requiring >= 1.23.5), return true if update node and id
	extern C_BOOL GDS_Node_Load(PdGDSObj Node, int NodeID, const char *Path,
		PdGDSFile File, PdGDSObj *OutNode, int *OutNodeID);
	/// unload the GDS node (requiring >= 1.23.5)
	extern void GDS_Node_Unload(PdGDSObj Node);
	/// delete the GDS variable (requiring >= 1.5.9)
	extern void GDS_Node_Delete(PdGDSObj Node, C_BOOL Force);
	/// get the class name of a GDS node
	extern void GDS_Node_GetClassName(PdGDSObj Node, char *OutStr, size_t OutSize);
	/// get the number of nodes in the folder
	extern int GDS_Node_ChildCount(PdGDSFolder Node);
	/// get a GDS file specified by a path
	extern PdGDSObj GDS_Node_Path(PdGDSFolder Node, const char *Path,
		C_BOOL MustExist);



	// ==================================================================
	// Attribute functions

	/// get the number of attributes
	extern int GDS_Attr_Count(PdGDSObj Node);
	/// get the attribute index with the name
	extern int GDS_Attr_Name2Index(PdGDSObj Node, const char *Name);



	// ==================================================================
	// CdAbstractArray methods

	/// get the number of dimensions
	extern int GDS_Array_DimCnt(PdAbstractArray Obj);
	/// get the dimension size
	extern void GDS_Array_GetDim(PdAbstractArray Obj, C_Int32 OutBuffer[],
		size_t N_Buf);
	/// get the total number of elements
	extern C_Int64 GDS_Array_GetTotalCount(PdAbstractArray Obj);
	/// get data type
	extern enum C_SVType GDS_Array_GetSVType(PdAbstractArray Obj);
	/// get the number of bits for an element
	extern unsigned GDS_Array_GetBitOf(PdAbstractArray Obj);
	/// read data
	/** \param Obj         GDS array object
	 *  \param Start       the starting positions (from ZERO), it could be NULL
	 *  \param Length      the lengths of each dimension, it could be NULL
	 *  \param OutBuffer   the pointer to the output buffer
	 *  \param OutSV       data type of output buffer
	**/
	extern void *GDS_Array_ReadData(PdAbstractArray Obj, const C_Int32 *Start,
		const C_Int32 *Length, void *OutBuf, enum C_SVType OutSV);
	/// read data with selection
	/** \param Obj         GDS array object
	 *  \param Start       the starting positions (from ZERO), it could be NULL
	 *  \param Length      the lengths of each dimension, it could be NULL
	 *  \param Selection   the array of selection, it could be NULL
	 *  \param OutBuffer   the pointer to the output buffer
	 *  \param OutSV       data type of output buffer
	**/
	extern void *GDS_Array_ReadDataEx(PdAbstractArray Obj, const C_Int32 *Start,
		const C_Int32 *Length, const C_BOOL *const Selection[], void *OutBuf,
		enum C_SVType OutSV);
	/// write data
	/** \param Obj         GDS array object
	 *  \param Start       the starting positions (from ZERO), it could be NULL
	 *  \param Length      the lengths of each dimension, it could be NULL
	 *  \param InBuf       the pointer to the input buffer
	 *  \param InSV        data type of input buffer
	**/
	extern const void *GDS_Array_WriteData(PdAbstractArray Obj,
		const C_Int32 *Start, const C_Int32 *Length, const void *InBuf,
		enum C_SVType InSV);
	/// append data
	extern const void *GDS_Array_AppendData(PdAbstractArray Obj, ssize_t Cnt,
		const void *InBuf, enum C_SVType InSV);
	/// append a string
	extern void GDS_Array_AppendString(PdAbstractArray Obj, const char *Text);
	/// append a string with maximum number of bytes
	extern void GDS_Array_AppendStrLen(PdAbstractArray Obj, const char *Text,
		size_t Len);



	// ==================================================================
	// TdIterator methods

	/// get the start iterator
	extern void GDS_Iter_GetStart(PdContainer Node, PdIterator Out);
	/// get the next iterator after the last one
	extern void GDS_Iter_GetEnd(PdContainer Node, PdIterator Out);
	/// get the GDS object with the iterator
	extern PdContainer GDS_Iter_GetHandle(PdIterator I);
	/// offset the iterator
	extern void GDS_Iter_Offset(PdIterator I, C_Int64 Offset);
	/// get the iterator according to offset from the beginning (requiring >= 1.5.11)
	extern void GDS_Iter_Position(PdContainer Node, PdIterator Out, C_Int64 Offset);
	/// get an integer according to the iterator
	extern C_Int64 GDS_Iter_GetInt(PdIterator I);
	/// get a numeric value according to the iterator
	extern C_Float64 GDS_Iter_GetFloat(PdIterator I);
	/// get a string according to the iterator
	extern void GDS_Iter_GetStr(PdIterator I, char *Out, size_t Size);
	/// set an integer according to the iterator
	extern void GDS_Iter_SetInt(PdIterator I, C_Int64 Val);
	/// set a numeric value according to the iterator
	extern void GDS_Iter_SetFloat(PdIterator I, C_Float64 Val);
	/// set a string according to the iterator
	extern void GDS_Iter_SetStr(PdIterator I, const char *Str);
	/// read data from the iterator
	extern void *GDS_Iter_RData(PdIterator I, void *OutBuf, size_t Cnt,
		enum C_SVType OutSV);
	/// read data from the iterator with a selection (requiring >= v1.5.11)
	extern void *GDS_Iter_RDataEx(PdIterator I, void *OutBuf, size_t Cnt,
		enum C_SVType OutSV, const C_BOOL Selection[]);
	/// write data to the iterator
	extern const void *GDS_Iter_WData(PdIterator I, const void *InBuf,
		size_t Cnt, enum C_SVType InSV);



	// ==================================================================
	// Error functions

	/// get the error message
	extern const char *GDS_GetError();
	/// set the error message
	extern void GDS_SetError(const char *Msg);



	// ==================================================================
	// Functions for parallel computing

	/// initialize the mutex object
	extern PdThreadMutex GDS_Parallel_InitMutex();
	/// finalize the mutex object
	extern void GDS_Parallel_DoneMutex(PdThreadMutex Obj);
	/// lock the mutex object
	extern void GDS_Parallel_LockMutex(PdThreadMutex Obj);
	/// lock the mutex object
	extern void GDS_Parallel_LockMutex(PdThreadMutex Obj);
	/// attempt to lock the mutex object, return true if succeed
	extern C_BOOL GDS_Parallel_TryLockMutex(PdThreadMutex Obj);
	/// unlock the mutex object
	extern void GDS_Parallel_UnlockMutex(PdThreadMutex Obj);

	/// initialize the condition object
	extern PdThreadCondition GDS_Parallel_InitCondition();
	/// finalize the condition object
	extern void GDS_Parallel_DoneCondition(PdThreadCondition Obj);
	/// signal a condition object
	extern void GDS_Parallel_SignalCondition(PdThreadCondition Obj);
	/// broadcast a condition object
	extern void GDS_Parallel_BroadcastCondition(PdThreadCondition Obj);
	/// wait for a condition object to become signaled
	extern void GDS_Parallel_WaitCondition(PdThreadCondition Obj, PdThreadMutex Mutex);

	/// initialize the suspending object
	extern PdThreadsSuspending GDS_Parallel_InitSuspend();
	/// finalize the suspending object
	extern void GDS_Parallel_DoneSuspend(PdThreadsSuspending Obj);
	/// suspend the object
	extern void GDS_Parallel_Suspend(PdThreadsSuspending Obj);
	/// wake up the object
	extern void GDS_Parallel_WakeUp(PdThreadsSuspending Obj);

	/// run the function with multiple threads
	extern void GDS_Parallel_RunThreads(
		void (*Proc)(PdThread, int, void*), void *Param, int nThread);



	// ==================================================================
	// Functions for machine

	/// return the number of available (logical) cores in the system
	extern int GDS_Mach_GetNumOfCores();
	/// return the size in byte of level-n cache memory
	extern C_UInt64 GDS_Mach_GetCPULevelCache(int level);



	// ==================================================================
	// Functions for reading block by block

	/// initialize the array object
	extern PdArrayRead GDS_ArrayRead_Init(PdAbstractArray Obj,
		int Margin, enum C_SVType SVType, const C_BOOL *const Selection[],
		C_BOOL buf_if_need);
	/// finalize the array object
	extern void GDS_ArrayRead_Free(PdArrayRead Obj);
	/// read data from the array object
	extern void GDS_ArrayRead_Read(PdArrayRead Obj, void *Buffer);
	/// return 1 if it is the end
	extern C_BOOL GDS_ArrayRead_Eof(PdArrayRead Obj);
	/// balance the buffers of multiple array objects according to the total buffer size
	extern void GDS_ArrayRead_BalanceBuffer(PdArrayRead array[], int n,
		C_Int64 buffer_size);


	// ==================================================================
	// External packages (requiring >= v1.23.6)

	/// load the Matrix package
	extern C_BOOL GDS_Load_Matrix();
	/// create a dgCMatrix R object, length(x)=length(i)=n_x, length(p)=ncol+1
	extern SEXP GDS_New_SpCMatrix(const double *x, const int *i, const int *p,
		int n_x, int nrow, int ncol);



	// ==================================================================

	#ifndef COREARRAY_GDSFMT_PACKAGE

	/// initialize GDS interface
	extern void Init_GDS_Routines();

	#endif  // COREARRAY_GDSFMT_PACKAGE


#ifdef __cplusplus
}
#endif

#endif /* _HEADER_R_GDS_ */
