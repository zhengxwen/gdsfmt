// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// R_GDS2.h: C interface to gdsfmt dynamic library
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
 *	\file     R_GDS2.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2014 - 2020
 *	\brief    C interface to gdsfmt dynamic library
 *	\details
**/


#ifndef _R_GDS_C_FILE_
#define _R_GDS_C_FILE_

#include <R_GDS.h>
#include <R_ext/Rdynload.h>

#ifdef __cplusplus
extern "C" {
#endif


// ===========================================================================
// R objects

typedef PdGDSFile (*Type_R_SEXP2File)(SEXP);
static Type_R_SEXP2File func_R_SEXP2File = NULL;
COREARRAY_DLL_LOCAL PdGDSFile GDS_R_SEXP2File(SEXP File)
{
	return (*func_R_SEXP2File)(File);
}

typedef PdGDSFolder (*Type_R_SEXP2FileRoot)(SEXP);
static Type_R_SEXP2FileRoot func_R_SEXP2FileRoot = NULL;
COREARRAY_DLL_LOCAL PdGDSFolder GDS_R_SEXP2FileRoot(SEXP File)
{
	return (*func_R_SEXP2FileRoot)(File);
}

typedef PdGDSObj (*Type_R_SEXP2Obj)(SEXP, C_BOOL);
static Type_R_SEXP2Obj func_R_SEXP2Obj = NULL;
COREARRAY_DLL_LOCAL PdGDSObj GDS_R_SEXP2Obj(SEXP Obj, C_BOOL ReadOnly)
{
	return (*func_R_SEXP2Obj)(Obj, ReadOnly);
}

typedef SEXP (*Type_R_Obj2SEXP)(PdGDSObj);
static Type_R_Obj2SEXP func_R_Obj2SEXP = NULL;
COREARRAY_DLL_LOCAL SEXP GDS_R_Obj2SEXP(PdGDSObj Obj)
{
	return (*func_R_Obj2SEXP)(Obj);
}

typedef void (*Type_R_Obj_SEXP2SEXP)(SEXP, SEXP);
static Type_R_Obj_SEXP2SEXP func_R_Obj_SEXP2SEXP = NULL;
COREARRAY_DLL_LOCAL void GDS_R_Obj_SEXP2SEXP(SEXP ObjDst, SEXP ObjSrc)
{
	(*func_R_Obj_SEXP2SEXP)(ObjDst, ObjSrc);
}

typedef C_BOOL (*Type_R_Is_Logical)(PdGDSObj);
static Type_R_Is_Logical func_R_Is_Logical = NULL;
COREARRAY_DLL_LOCAL C_BOOL GDS_R_Is_Logical(PdGDSObj Obj)
{
	return (*func_R_Is_Logical)(Obj);
}

typedef C_BOOL (*Type_R_Is_Factor)(PdGDSObj);
static Type_R_Is_Factor func_R_Is_Factor = NULL;
COREARRAY_DLL_LOCAL C_BOOL GDS_R_Is_Factor(PdGDSObj Obj)
{
	return (*func_R_Is_Factor)(Obj);
}

typedef int (*Type_R_Is_ExtType)(PdGDSObj);
static Type_R_Is_ExtType func_R_Is_ExtType = NULL;
COREARRAY_DLL_EXPORT int GDS_R_Is_ExtType(PdGDSObj Obj)
{
	return (*func_R_Is_ExtType)(Obj);
}

typedef int (*Type_R_Set_IfFactor)(PdGDSObj, SEXP);
static Type_R_Set_IfFactor func_R_Set_IfFactor = NULL;
COREARRAY_DLL_LOCAL int GDS_R_Set_IfFactor(PdGDSObj Obj, SEXP Val)
{
	return (*func_R_Set_IfFactor)(Obj, Val);
}

typedef SEXP (*Type_R_Array_Read)(PdAbstractArray, const C_Int32 *,
	const C_Int32 *, const C_BOOL *const [], C_UInt32);
static Type_R_Array_Read func_R_Array_Read = NULL;
COREARRAY_DLL_LOCAL SEXP GDS_R_Array_Read(PdAbstractArray Obj,
	const C_Int32 *Start, const C_Int32 *Length,
	const C_BOOL *const Selection[], C_UInt32 UseMode)
{
	return (*func_R_Array_Read)(Obj, Start, Length, Selection, UseMode);
}

typedef void (*Type_R_Apply)(int, PdAbstractArray [], int [],
	const C_BOOL *const * const [],
	void (*)(SEXP, C_Int32, PdArrayRead [], void *),
	void (*)(SEXP, C_Int32, void *), void *, C_BOOL, C_UInt32 UseMode);
static Type_R_Apply func_R_Apply = NULL;
COREARRAY_DLL_LOCAL void GDS_R_Apply(int Num, PdAbstractArray ObjList[],
	int Margins[], const C_BOOL *const * const Selection[],
	void (*InitFunc)(SEXP Argument, C_Int32 Count, PdArrayRead ReadObjList[],
		void *_Param),
	void (*LoopFunc)(SEXP Argument, C_Int32 Idx, void *_Param),
	void *Param, C_BOOL IncOrDec, C_UInt32 UseMode)
{
	(*func_R_Apply)(Num, ObjList, Margins, Selection, InitFunc, LoopFunc,
		Param, IncOrDec, UseMode);
}

typedef void (*Type_R_Append)(PdAbstractArray, SEXP);
static Type_R_Append func_R_Append = NULL;
COREARRAY_DLL_LOCAL void GDS_R_Append(PdAbstractArray Obj, SEXP Data)
{
	(*func_R_Append)(Obj, Data);
}

typedef void (*Type_R_AppendEx)(PdAbstractArray, SEXP, size_t, size_t);
static Type_R_AppendEx func_R_AppendEx = NULL;
COREARRAY_DLL_LOCAL void GDS_R_AppendEx(PdAbstractArray Obj, SEXP Data,
	size_t Start, size_t Count)
{
	(*func_R_AppendEx)(Obj, Data, Start, Count);
}

typedef void (*Type_R_Is_Element)(PdAbstractArray, SEXP, C_BOOL[]);
static Type_R_Is_Element func_R_Is_Element = NULL;
COREARRAY_DLL_LOCAL void GDS_R_Is_Element(PdAbstractArray Obj, SEXP SetEL,
	C_BOOL Out[])
{
	(*func_R_Is_Element)(Obj, SetEL, Out);
}



// ===========================================================================
// File structure

typedef PdGDSFile (*Type_File_Create)(const char *);
static Type_File_Create func_File_Create = NULL;
COREARRAY_DLL_LOCAL PdGDSFile GDS_File_Create(const char *FileName)
{
	return (*func_File_Create)(FileName);
}

typedef PdGDSFile (*Type_File_Open)(const char *, C_BOOL, C_BOOL, C_BOOL);
static Type_File_Open func_File_Open = NULL;
COREARRAY_DLL_LOCAL PdGDSFile GDS_File_Open(const char *FileName, C_BOOL ReadOnly,
	C_BOOL ForkSupport, C_BOOL AllowError)
{
	return (*func_File_Open)(FileName, ReadOnly, ForkSupport, AllowError);
}

typedef void (*Type_File_Close)(PdGDSFile);
static Type_File_Close func_File_Close = NULL;
COREARRAY_DLL_LOCAL void GDS_File_Close(PdGDSFile File)
{
	(*func_File_Close)(File);
}

typedef void (*Type_File_Sync)(PdGDSFile);
static Type_File_Sync func_File_Sync = NULL;
COREARRAY_DLL_LOCAL void GDS_File_Sync(PdGDSFile File)
{
	(*func_File_Sync)(File);
}

typedef C_BOOL (*Type_File_Reopen)(SEXP);
static Type_File_Reopen func_File_Reopen = NULL;
COREARRAY_DLL_LOCAL C_BOOL GDS_File_Reopen(SEXP GDSObj)
{
	return (*func_File_Reopen)(GDSObj);
}

typedef PdGDSFolder (*Type_File_Root)(PdGDSFile);
static Type_File_Root func_File_Root = NULL;
COREARRAY_DLL_LOCAL PdGDSFolder GDS_File_Root(PdGDSFile File)
{
	return (*func_File_Root)(File);
}

typedef PdGDSFile (*Type_Node_File)(PdGDSObj);
static Type_Node_File func_Node_File = NULL;
COREARRAY_DLL_LOCAL PdGDSFile GDS_Node_File(PdGDSObj Node)
{
	return (*func_Node_File)(Node);
}

typedef void (*Type_Node_Delete)(PdGDSObj, C_BOOL);
static Type_Node_Delete func_Node_Delete = NULL;
COREARRAY_DLL_LOCAL void GDS_Node_Delete(PdGDSObj Node, C_BOOL Force)
{
	(*func_Node_Delete)(Node, Force);
}

typedef C_BOOL (*Type_Node_Load)(PdGDSObj, int, const char*, PdGDSFile, PdGDSObj*, int*);
static Type_Node_Load func_Node_Load = NULL;
COREARRAY_DLL_LOCAL C_BOOL GDS_Node_Load(PdGDSObj Node, int NodeID,
	const char *Path, PdGDSFile File, PdGDSObj *OutNode, int *OutNodeID)
{
	return (*func_Node_Load)(Node, NodeID, Path, File, OutNode, OutNodeID);
}

typedef void (*Type_Node_Unload)(PdGDSObj);
static Type_Node_Unload func_Node_Unload = NULL;
COREARRAY_DLL_LOCAL void GDS_Node_Unload(PdGDSObj Node)
{
	(*func_Node_Unload)(Node);
}

typedef void (*Type_Node_GetClassName)(PdGDSObj, char *, size_t);
static Type_Node_GetClassName func_Node_GetClassName = NULL;
COREARRAY_DLL_LOCAL void GDS_Node_GetClassName(PdGDSObj Node, char *Out,
	size_t OutSize)
{
	(*func_Node_GetClassName)(Node, Out, OutSize);
}

typedef int (*Type_Node_ChildCount)(PdGDSFolder);
static Type_Node_ChildCount func_Node_ChildCount = NULL;
COREARRAY_DLL_LOCAL int GDS_Node_ChildCount(PdGDSFolder Node)
{
	return (*func_Node_ChildCount)(Node);
}

typedef PdGDSObj (*Type_Node_Path)(PdGDSFolder, const char *, C_BOOL);
static Type_Node_Path func_Node_Path = NULL;
COREARRAY_DLL_LOCAL PdGDSObj GDS_Node_Path(PdGDSFolder Node,
	const char *Path, C_BOOL MustExist)
{
	return (*func_Node_Path)(Node, Path, MustExist);
}


// ===========================================================================
// functions for attributes

typedef int (*Type_Attr_Count)(PdGDSObj);
static Type_Attr_Count func_Attr_Count = NULL;
COREARRAY_DLL_EXPORT int GDS_Attr_Count(PdGDSObj Node)
{
	return (*func_Attr_Count)(Node);
}

typedef int (*Type_Attr_Name2Index)(PdGDSObj, const char *);
static Type_Attr_Name2Index func_Attr_Name2Index = NULL;
COREARRAY_DLL_EXPORT int GDS_Attr_Name2Index(PdGDSObj Node, const char *Name)
{
	return (*func_Attr_Name2Index)(Node, Name);
}


// ===========================================================================
// functions for CdAbstractArray

typedef int (*Type_Array_DimCnt)(PdAbstractArray);
static Type_Array_DimCnt func_Array_DimCnt = NULL;
COREARRAY_DLL_LOCAL int GDS_Array_DimCnt(PdAbstractArray Obj)
{
	return (*func_Array_DimCnt)(Obj);
}

typedef void (*Type_Array_GetDim)(PdAbstractArray, C_Int32 [], size_t);
static Type_Array_GetDim func_Array_GetDim = NULL;
COREARRAY_DLL_LOCAL void GDS_Array_GetDim(PdAbstractArray Obj,
	C_Int32 OutBuffer[], size_t N_Buf)
{
	(*func_Array_GetDim)(Obj, OutBuffer, N_Buf);
}

typedef C_Int64 (*Type_Array_GetTotalCount)(PdAbstractArray);
static Type_Array_GetTotalCount func_Array_GetTotalCount = NULL;
COREARRAY_DLL_LOCAL C_Int64 GDS_Array_GetTotalCount(PdAbstractArray Obj)
{
	return (*func_Array_GetTotalCount)(Obj);
}

typedef enum C_SVType (*Type_Array_GetSVType)(PdAbstractArray);
static Type_Array_GetSVType func_Array_GetSVType = NULL;
COREARRAY_DLL_LOCAL enum C_SVType GDS_Array_GetSVType(PdAbstractArray Obj)
{
	return (*func_Array_GetSVType)(Obj);
}

typedef unsigned (*Type_Array_GetBitOf)(PdAbstractArray);
static Type_Array_GetBitOf func_Array_GetBitOf = NULL;
COREARRAY_DLL_LOCAL unsigned GDS_Array_GetBitOf(PdAbstractArray Obj)
{
	return (*func_Array_GetBitOf)(Obj);
}

typedef void* (*Type_Array_ReadData)(PdAbstractArray, C_Int32 const *,
	C_Int32 const *, void *, enum C_SVType);
static Type_Array_ReadData func_Array_ReadData = NULL;
COREARRAY_DLL_LOCAL void *GDS_Array_ReadData(PdAbstractArray Obj,
	C_Int32 const* Start, C_Int32 const* Length, void *OutBuf,
	enum C_SVType OutSV)
{
	return (*func_Array_ReadData)(Obj, Start, Length, OutBuf, OutSV);
}

typedef void* (*Type_Array_ReadDataEx)(PdAbstractArray, C_Int32 const *,
	C_Int32 const *, const C_BOOL *const [], void *, enum C_SVType OutSV);
static Type_Array_ReadDataEx func_Array_ReadDataEx = NULL;
COREARRAY_DLL_LOCAL void *GDS_Array_ReadDataEx(PdAbstractArray Obj,
	C_Int32 const* Start, C_Int32 const* Length,
	const C_BOOL *const Selection[], void *OutBuf, enum C_SVType OutSV)
{
	return (*func_Array_ReadDataEx)(Obj, Start, Length, Selection, OutBuf, OutSV);
}

typedef const void* (*Type_Array_WriteData)(PdAbstractArray, C_Int32 const *,
	C_Int32 const *, const void *, enum C_SVType);
static Type_Array_WriteData func_Array_WriteData = NULL;
COREARRAY_DLL_LOCAL const void *GDS_Array_WriteData(PdAbstractArray Obj,
	C_Int32 const* Start, C_Int32 const* Length, const void *InBuf,
	enum C_SVType InSV)
{
	return (*func_Array_WriteData)(Obj, Start, Length, InBuf, InSV);
}

typedef const void* (*Type_Array_AppendData)(PdAbstractArray, ssize_t, const void *,
	enum C_SVType);
static Type_Array_AppendData func_Array_AppendData = NULL;
COREARRAY_DLL_LOCAL const void *GDS_Array_AppendData(PdAbstractArray Obj, ssize_t Cnt,
	const void *InBuf, enum C_SVType InSV)
{
	return (*func_Array_AppendData)(Obj, Cnt, InBuf, InSV);
}

typedef void (*Type_Array_AppendString)(PdAbstractArray, const char *);
static Type_Array_AppendString func_Array_AppendString = NULL;
COREARRAY_DLL_LOCAL void GDS_Array_AppendString(PdAbstractArray Obj,
	const char *Text)
{
	(*func_Array_AppendString)(Obj, Text);
}

typedef void (*Type_Array_AppendStrLen)(PdAbstractArray, const char *, size_t);
static Type_Array_AppendStrLen func_Array_AppendStrLen = NULL;
COREARRAY_DLL_LOCAL void GDS_Array_AppendStrLen(PdAbstractArray Obj,
	const char *Text, size_t Len)
{
	(*func_Array_AppendStrLen)(Obj, Text, Len);
}



// ===========================================================================
// functions for TdIterator

typedef void (*Type_Iter_GetStart)(PdContainer, PdIterator);
static Type_Iter_GetStart func_Iter_GetStart = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_GetStart(PdContainer Node, PdIterator Out)
{
	(*func_Iter_GetStart)(Node, Out);
}

typedef void (*Type_Iter_GetEnd)(PdContainer, PdIterator);
static Type_Iter_GetEnd func_Iter_GetEnd = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_GetEnd(PdContainer Node, PdIterator Out)
{
	(*func_Iter_GetEnd)(Node, Out);
}

typedef PdContainer (*Type_Iter_GetHandle)(PdIterator);
static Type_Iter_GetHandle func_Iter_GetHandle = NULL;
COREARRAY_DLL_LOCAL PdContainer GDS_Iter_GetHandle(PdIterator I)
{
	return (*func_Iter_GetHandle)(I);
}

typedef void (*Type_Iter_Offset)(PdIterator, C_Int64);
static Type_Iter_Offset func_Iter_Offset = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_Offset(PdIterator I, C_Int64 Offset)
{
	(*func_Iter_Offset)(I, Offset);
}

typedef void (*Type_Iter_Position)(PdContainer, PdIterator, C_Int64);
static Type_Iter_Position func_Iter_Position = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_Position(PdContainer Node, PdIterator Out, C_Int64 Offset)
{
	(*func_Iter_Position)(Node, Out, Offset);
}

typedef C_Int64 (*Type_Iter_GetInt)(PdIterator);
static Type_Iter_GetInt func_Iter_GetInt = NULL;
COREARRAY_DLL_LOCAL C_Int64 GDS_Iter_GetInt(PdIterator I)
{
	return (*func_Iter_GetInt)(I);
}

typedef C_Float64 (*Type_Iter_GetFloat)(PdIterator);
static Type_Iter_GetFloat func_Iter_GetFloat = NULL;
COREARRAY_DLL_LOCAL C_Float64 GDS_Iter_GetFloat(PdIterator I)
{
	return (*func_Iter_GetFloat)(I);
}

typedef void (*Type_Iter_GetStr)(PdIterator, char *, size_t);
static Type_Iter_GetStr func_Iter_GetStr = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_GetStr(PdIterator I, char *Out, size_t Size)
{
	(*func_Iter_GetStr)(I, Out, Size);
}

typedef void (*Type_Iter_SetInt)(PdIterator, C_Int64);
static Type_Iter_SetInt func_Iter_SetInt = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_SetInt(PdIterator I, C_Int64 Val)
{
	(*func_Iter_SetInt)(I, Val);
}

typedef void (*Type_Iter_SetFloat)(PdIterator, C_Float64);
static Type_Iter_SetFloat func_Iter_SetFloat = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_SetFloat(PdIterator I, C_Float64 Val)
{
	(*func_Iter_SetFloat)(I, Val);
}

typedef void (*Type_Iter_SetStr)(PdIterator, const char *);
static Type_Iter_SetStr func_Iter_SetStr = NULL;
COREARRAY_DLL_LOCAL void GDS_Iter_SetStr(PdIterator I, const char *Str)
{
	(*func_Iter_SetStr)(I, Str);
}

typedef void* (*Type_Iter_RData)(PdIterator, void *, size_t, enum C_SVType);
static Type_Iter_RData func_Iter_RData = NULL;
COREARRAY_DLL_LOCAL void *GDS_Iter_RData(PdIterator I, void *OutBuf,
	size_t Cnt, enum C_SVType OutSV)
{
	return (*func_Iter_RData)(I, OutBuf, Cnt, OutSV);
}

typedef void* (*Type_Iter_RDataEx)(PdIterator, void *, size_t, enum C_SVType, const C_BOOL*);
static Type_Iter_RDataEx func_Iter_RDataEx = NULL;
COREARRAY_DLL_LOCAL void *GDS_Iter_RDataEx(PdIterator I, void *OutBuf,
	size_t Cnt, enum C_SVType OutSV, const C_BOOL Selection[])
{
	return (*func_Iter_RDataEx)(I, OutBuf, Cnt, OutSV, Selection);
}

typedef const void* (*Type_Iter_WData)(PdIterator, const void *, size_t, enum C_SVType);
static Type_Iter_WData func_Iter_WData = NULL;
COREARRAY_DLL_LOCAL const void *GDS_Iter_WData(PdIterator I, const void *InBuf,
	size_t Cnt, enum C_SVType InSV)
{
	return (*func_Iter_WData)(I, InBuf, Cnt, InSV);
}



// ===========================================================================
// functions for error

typedef const char *(*Type_GetError)();
static Type_GetError func_GetError = NULL;
COREARRAY_DLL_LOCAL const char *GDS_GetError()
{
	return (*func_GetError)();
}

typedef void (*Type_SetError)(const char *);
static Type_SetError func_SetError = NULL;
COREARRAY_DLL_LOCAL void GDS_SetError(const char *Msg)
{
	(*func_SetError)(Msg);
}



// ===========================================================================
// functions for parallel computing

typedef PdThreadMutex (*Type_Parallel_InitMutex)();
static Type_Parallel_InitMutex func_Parallel_InitMutex = NULL;
COREARRAY_DLL_LOCAL PdThreadMutex GDS_Parallel_InitMutex()
{
	return (*func_Parallel_InitMutex)();
}

typedef void (*Type_Parallel_DoneMutex)(PdThreadMutex);
static Type_Parallel_DoneMutex func_Parallel_DoneMutex = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_DoneMutex(PdThreadMutex Obj)
{
	(*func_Parallel_DoneMutex)(Obj);
}

typedef void (*Type_Parallel_LockMutex)(PdThreadMutex);
static Type_Parallel_LockMutex func_Parallel_LockMutex = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_LockMutex(PdThreadMutex Obj)
{
	(*func_Parallel_LockMutex)(Obj);
}

typedef C_BOOL (*Type_Parallel_TryLockMutex)(PdThreadMutex);
static Type_Parallel_TryLockMutex func_Parallel_TryLockMutex = NULL;
COREARRAY_DLL_LOCAL C_BOOL GDS_Parallel_TryLockMutex(PdThreadMutex Obj)
{
	return (*func_Parallel_TryLockMutex)(Obj);
}

typedef void (*Type_Parallel_UnlockMutex)(PdThreadMutex);
static Type_Parallel_UnlockMutex func_Parallel_UnlockMutex = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_UnlockMutex(PdThreadMutex Obj)
{
	(*func_Parallel_UnlockMutex)(Obj);
}

typedef PdThreadCondition (*Type_Parallel_InitCondition)();
static Type_Parallel_InitCondition func_Parallel_InitCondition = NULL;
COREARRAY_DLL_LOCAL PdThreadCondition GDS_Parallel_InitCondition()
{
	return (*func_Parallel_InitCondition)();
}

typedef void (*Type_Parallel_Condition)(PdThreadCondition);
static Type_Parallel_Condition func_Parallel_DoneCondition = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_DoneCondition(PdThreadCondition Obj)
{
	(*func_Parallel_DoneCondition)(Obj);
}

static Type_Parallel_Condition func_Parallel_SignalCondition = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_SignalCondition(PdThreadCondition Obj)
{
	(*func_Parallel_SignalCondition)(Obj);
}

static Type_Parallel_Condition func_Parallel_BroadcastCondition = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_BroadcastCondition(PdThreadCondition Obj)
{
	(*func_Parallel_BroadcastCondition)(Obj);
}

typedef void (*Type_Parallel_WaitCondition)(PdThreadCondition, PdThreadMutex);
static Type_Parallel_WaitCondition func_Parallel_WaitCondition = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_WaitCondition(PdThreadCondition Obj,
	PdThreadMutex Mutex)
{
	(*func_Parallel_WaitCondition)(Obj, Mutex);
}

typedef PdThreadsSuspending (*Type_Parallel_InitSuspend)();
static Type_Parallel_InitSuspend func_Parallel_InitSuspend = NULL;
COREARRAY_DLL_LOCAL PdThreadsSuspending GDS_Parallel_InitSuspend()
{
	return (*func_Parallel_InitSuspend)();
}

typedef void (*Type_Parallel_DoneSuspend)(PdThreadsSuspending);
static Type_Parallel_DoneSuspend func_Parallel_DoneSuspend = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_DoneSuspend(PdThreadsSuspending Obj)
{
	(*func_Parallel_DoneSuspend)(Obj);
}

typedef void (*Type_Parallel_Suspend)(PdThreadsSuspending);
static Type_Parallel_Suspend func_Parallel_Suspend = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_Suspend(PdThreadsSuspending Obj)
{
	(*func_Parallel_Suspend)(Obj);
}

typedef void (*Type_Parallel_WakeUp)(PdThreadsSuspending);
static Type_Parallel_WakeUp func_Parallel_WakeUp = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_WakeUp(PdThreadsSuspending Obj)
{
	(*func_Parallel_WakeUp)(Obj);
}

typedef void (*Type_Parallel_RunThreads)(void (*)(PdThread, int, void*),
	void *, int);
static Type_Parallel_RunThreads func_Parallel_RunThreads = NULL;
COREARRAY_DLL_LOCAL void GDS_Parallel_RunThreads(
	void (*Proc)(PdThread, int, void*), void *Param, int nThread)
{
	(*func_Parallel_RunThreads)(Proc, Param, nThread);
}


// ===========================================================================
// functions for machine

typedef int (*Type_Mach_GetNumOfCores)();
static Type_Mach_GetNumOfCores func_Mach_GetNumOfCores = NULL;
COREARRAY_DLL_EXPORT int GDS_Mach_GetNumOfCores()
{
	return (*func_Mach_GetNumOfCores)();
}

typedef size_t (*Type_Mach_GetCPULevelCache)(int);
static Type_Mach_GetCPULevelCache func_Mach_GetCPULevelCache = NULL;
COREARRAY_DLL_EXPORT C_UInt64 GDS_Mach_GetCPULevelCache(int level)
{
	return (*func_Mach_GetCPULevelCache)(level);
}

// ===========================================================================
// functions for reading block by block

typedef PdArrayRead (*Type_ArrayRead_Init)(PdAbstractArray, int, enum C_SVType,
	const C_BOOL *const [], C_BOOL);
static Type_ArrayRead_Init func_ArrayRead_Init = NULL;
COREARRAY_DLL_LOCAL PdArrayRead GDS_ArrayRead_Init(PdAbstractArray Obj,
	int Margin, enum C_SVType SVType, const C_BOOL *const Selection[],
	C_BOOL buf_if_need)
{
	return (*func_ArrayRead_Init)(Obj, Margin, SVType, Selection, buf_if_need);
}

typedef void (*Type_ArrayRead_Free)(PdArrayRead);
static Type_ArrayRead_Free func_ArrayRead_Free = NULL;
COREARRAY_DLL_LOCAL void GDS_ArrayRead_Free(PdArrayRead Obj)
{
	(*func_ArrayRead_Free)(Obj);
}

typedef void (*Type_ArrayRead_Read)(PdArrayRead, void *);
static Type_ArrayRead_Read func_ArrayRead_Read = NULL;
COREARRAY_DLL_LOCAL void GDS_ArrayRead_Read(PdArrayRead Obj, void *Buffer)
{
	(*func_ArrayRead_Read)(Obj, Buffer);
}

typedef C_BOOL (*Type_ArrayRead_Eof)(PdArrayRead);
static Type_ArrayRead_Eof func_ArrayRead_Eof = NULL;
COREARRAY_DLL_LOCAL C_BOOL GDS_ArrayRead_Eof(PdArrayRead Obj)
{
	return (*func_ArrayRead_Eof)(Obj);
}

typedef void (*Type_ArrayRead_BalanceBuffer)(PdArrayRead[], int, C_Int64);
static Type_ArrayRead_BalanceBuffer func_ArrayRead_BalanceBuffer = NULL;
COREARRAY_DLL_LOCAL void GDS_ArrayRead_BalanceBuffer(PdArrayRead array[],
	int n, C_Int64 buffer_size)
{
	(*func_ArrayRead_BalanceBuffer)(array, n, buffer_size);
}

typedef C_BOOL (*Type_Load_Matrix)();
static Type_Load_Matrix func_Load_Matrix = NULL;
COREARRAY_DLL_LOCAL C_BOOL GDS_Load_Matrix()
{
	return (*func_Load_Matrix)();
}

typedef SEXP (*Type_New_SpCMatrix)(const double *x, const int *i, const int *p,
	int n_x, int nrow, int ncol);
static Type_New_SpCMatrix func_New_SpCMatrix = NULL;
COREARRAY_DLL_LOCAL SEXP GDS_New_SpCMatrix(const double *x, const int *i,
	const int *p, int n_x, int nrow, int ncol)
{
	return (*func_New_SpCMatrix)(x, i, p, n_x, nrow, ncol);
}



// ===========================================================================

/// initialize the GDS routines
void Init_GDS_Routines()
{
	static const char *PKG_GDSFMT = "gdsfmt";

	#define LOAD(var, name)    { \
	    DL_FUNC f = R_GetCCallable(PKG_GDSFMT, name); \
		memcpy(&var, &f, sizeof(f)); \
	}

	// R objects
	LOAD(func_R_SEXP2File, "GDS_R_SEXP2File");
	LOAD(func_R_SEXP2FileRoot, "GDS_R_SEXP2FileRoot");
	LOAD(func_R_SEXP2Obj, "GDS_R_SEXP2Obj");
	LOAD(func_R_Obj2SEXP, "GDS_R_Obj2SEXP");
	LOAD(func_R_Obj_SEXP2SEXP, "GDS_R_Obj_SEXP2SEXP");
	LOAD(func_R_Is_Logical, "GDS_R_Is_Logical");
	LOAD(func_R_Is_Factor, "GDS_R_Is_Factor");
	LOAD(func_R_Is_ExtType, "GDS_R_Is_ExtType");
	LOAD(func_R_Set_IfFactor, "GDS_R_Set_IfFactor");
	LOAD(func_R_Array_Read, "GDS_R_Array_Read");
	LOAD(func_R_Apply, "GDS_R_Apply");
	LOAD(func_R_Append, "GDS_R_Append");
	LOAD(func_R_AppendEx, "GDS_R_AppendEx");
	LOAD(func_R_Is_Element, "GDS_R_Is_Element");

	// File structure
	LOAD(func_File_Create, "GDS_File_Create");
	LOAD(func_File_Open, "GDS_File_Open");
	LOAD(func_File_Close, "GDS_File_Close");
	LOAD(func_File_Sync, "GDS_File_Sync");
	LOAD(func_File_Reopen, "GDS_File_Reopen");
	LOAD(func_File_Root, "GDS_File_Root");

	LOAD(func_Node_File, "GDS_Node_File");
	LOAD(func_Node_Load, "GDS_Node_Load");
	LOAD(func_Node_Unload, "GDS_Node_Unload");
	LOAD(func_Node_Delete, "GDS_Node_Delete");
	LOAD(func_Node_GetClassName, "GDS_Node_GetClassName");
	LOAD(func_Node_ChildCount, "GDS_Node_ChildCount");
	LOAD(func_Node_Path, "GDS_Node_Path");

	LOAD(func_Attr_Count, "GDS_Attr_Count");
	LOAD(func_Attr_Name2Index, "GDS_Attr_Name2Index");

	LOAD(func_Array_DimCnt, "GDS_Array_DimCnt");
	LOAD(func_Array_GetDim, "GDS_Array_GetDim");
	LOAD(func_Array_GetTotalCount, "GDS_Array_GetTotalCount");
	LOAD(func_Array_GetSVType, "GDS_Array_GetSVType");
	LOAD(func_Array_GetBitOf, "GDS_Array_GetBitOf");
	LOAD(func_Array_ReadData, "GDS_Array_ReadData");
	LOAD(func_Array_ReadDataEx, "GDS_Array_ReadDataEx");
	LOAD(func_Array_WriteData, "GDS_Array_WriteData");
	LOAD(func_Array_AppendData, "GDS_Array_AppendData");
	LOAD(func_Array_AppendString, "GDS_Array_AppendString");
	LOAD(func_Array_AppendStrLen, "GDS_Array_AppendStrLen");

	LOAD(func_Iter_GetStart, "GDS_Iter_GetStart");
	LOAD(func_Iter_GetEnd, "GDS_Iter_GetEnd");
	LOAD(func_Iter_GetHandle, "GDS_Iter_GetHandle");
	LOAD(func_Iter_Offset, "GDS_Iter_Offset");
	LOAD(func_Iter_Position, "GDS_Iter_Position");
	LOAD(func_Iter_GetInt, "GDS_Iter_GetInt");
	LOAD(func_Iter_GetFloat, "GDS_Iter_GetFloat");
	LOAD(func_Iter_GetStr, "GDS_Iter_GetStr");
	LOAD(func_Iter_SetInt, "GDS_Iter_SetInt");
	LOAD(func_Iter_SetFloat, "GDS_Iter_SetFloat");
	LOAD(func_Iter_SetStr, "GDS_Iter_SetStr");
	LOAD(func_Iter_RData, "GDS_Iter_RData");
	LOAD(func_Iter_RDataEx, "GDS_Iter_RDataEx");
	LOAD(func_Iter_WData, "GDS_Iter_WData");

	LOAD(func_GetError, "GDS_GetError");
	LOAD(func_SetError, "GDS_SetError");

	LOAD(func_Parallel_InitMutex, "GDS_Parallel_InitMutex");
	LOAD(func_Parallel_DoneMutex, "GDS_Parallel_DoneMutex");
	LOAD(func_Parallel_LockMutex, "GDS_Parallel_LockMutex");
	LOAD(func_Parallel_TryLockMutex, "GDS_Parallel_TryLockMutex");
	LOAD(func_Parallel_UnlockMutex, "GDS_Parallel_UnlockMutex");

	LOAD(func_Parallel_InitCondition, "GDS_Parallel_InitCondition");
	LOAD(func_Parallel_DoneCondition, "GDS_Parallel_DoneCondition");
	LOAD(func_Parallel_SignalCondition, "GDS_Parallel_SignalCondition");
	LOAD(func_Parallel_BroadcastCondition, "GDS_Parallel_BroadcastCondition");
	LOAD(func_Parallel_WaitCondition, "GDS_Parallel_WaitCondition");

	LOAD(func_Parallel_InitSuspend, "GDS_Parallel_InitSuspend");
	LOAD(func_Parallel_DoneSuspend, "GDS_Parallel_DoneSuspend");
	LOAD(func_Parallel_Suspend, "GDS_Parallel_Suspend");
	LOAD(func_Parallel_WakeUp, "GDS_Parallel_WakeUp");
	LOAD(func_Parallel_RunThreads, "GDS_Parallel_RunThreads");

	LOAD(func_Mach_GetNumOfCores, "GDS_Mach_GetNumOfCores");
	LOAD(func_Mach_GetCPULevelCache, "GDS_Mach_GetCPULevelCache");

	LOAD(func_ArrayRead_Init, "GDS_ArrayRead_Init");
	LOAD(func_ArrayRead_Free, "GDS_ArrayRead_Free");
	LOAD(func_ArrayRead_Read, "GDS_ArrayRead_Read");
	LOAD(func_ArrayRead_Eof, "GDS_ArrayRead_Eof");
	LOAD(func_ArrayRead_BalanceBuffer, "GDS_ArrayRead_BalanceBuffer");

	LOAD(func_Load_Matrix, "GDS_Load_Matrix");
	LOAD(func_New_SpCMatrix, "GDS_New_SpCMatrix");
}


#ifdef __cplusplus
}
#endif

#endif /* _R_GDS_C_FILE_ */
