// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// CoreGDS.h: The header for exporting the C interface of CoreArray library
//
// Copyright (C) 2007 - 2014	Xiuwen Zheng
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
 *	\file     CoreGDS.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Export the C interface of CoreArray library
 *	\details
**/

#ifndef _COREGDS_H_
#define _COREGDS_H_


#include <CoreArray.h>
#include <string>

using namespace CoreArray;
using namespace CoreArray::Parallel;

extern "C"
{
	// Functions for CoreArray System

	extern void gds_SysInit();
	extern int gds_SysVersion();
	extern const char *gds_SysVersionStr();
	extern int gds_SysClassCount();
	extern bool gds_SysClassStruct(int Index, const char **OutName,
		const char **OutDesp, int *OutClassType);
	extern int gds_SysPipeCount();
	extern bool gds_SysPipeStruct(int Index, const char **OutCoder,
		const char **OutDesp);


	// Functions for File

	extern PdGDSFile gds_FileOpen(const char *FileName, bool ReadOnly);
	extern PdGDSFile gds_FileCreate(const char *FileName);
	extern bool gds_FileClose(PdGDSFile Handle);
	extern bool gds_FileSync(PdGDSFile Handle);
	extern int gds_FileName(PdGDSFile Handle, char *OutStr, size_t OutBufLen);
	extern bool gds_FileReadOnly(PdGDSFile Handle);
	extern PdGDSObj gds_FilePath(PdGDSFile Handle, const char *Path);
	extern Int64 gds_FileSize(PdGDSFile Handle);
	extern bool gds_FileTidyUp(PdGDSFile Handle);


	// Functions for File Structure

	extern PdGDSFolder gds_NodeRoot(PdGDSFile Handle);
	extern PdGDSFile gds_NodeGDSFile(PdGDSObj Node);
	extern int gds_NodeChildCount(PdGDSFolder Node);
	extern PdGDSFolder gds_NodeChildFolder(PdGDSFolder Node, int Index);
	extern PdGDSObj gds_NodeChild(PdGDSFolder Node, int Index);
	extern PdGDSObj gds_NodePath(PdGDSFolder Node, const char *Path);
	extern bool gds_NodeDelete(PdGDSObj Node);
	extern bool gds_NodeMoveTo(PdGDSObj Source, PdGDSFolder Dest);
	extern bool gds_NodeCopyTo(PdGDSObj Source, PdGDSFolder Dest);
	extern bool gds_NodeAddLabel(PdGDSFolder Folder, const char *Name);
	extern PdGDSFolder gds_NodeAddFolder(PdGDSFolder Folder,
		const char *Name);
	extern PdSequenceX gds_NodeAddArray(PdGDSFolder Folder,
		const char *Name, const char *TypeName, int DimCnt);

	extern bool gds_NodeIsFolder(PdGDSObj Node);
	extern bool gds_NodeIsNull(PdGDSObj Node);
	extern bool gds_NodeIsContainer(PdGDSObj Node);
	extern bool gds_NodeIsSequence(PdGDSObj Node);
	extern bool gds_NodeIsVector(PdGDSObj Node);
	extern bool gds_NodeIsFStr(PdGDSObj Node);
	extern bool gds_NodeIsStreamContainer(PdGDSObj Node);

	extern int gds_NodeName(PdGDSObj Node, char *OutStr,
		size_t OutBufLen, bool FullName);
	extern bool gds_NodeSetName(PdGDSObj Node, char *NewName);
	extern int gds_NodeClassName(PdGDSObj Node, char *OutStr,
		size_t OutBufLen);
	extern bool gds_NodeStreamInfo(PdGDSObj Node, Int64 *TotalIn,
		Int64 *TotalOut, const char **StreamDesp);
	extern bool gds_SetPackedMode(PdGDSObj Node, const char *Mode);
	extern bool gds_NodeFree(PdGDSObj Node);


	// Functions for File Node -- TdObjAttr

	extern int gds_AttrCount(PdGDSObj Node);
	extern int gds_AttrNameIndex(PdGDSObj Node, const char *Name);
	extern bool gds_AttrIxAdd(PdGDSObj Node, const char *Name,
		const char *Value);
	extern int gds_AttrIxName(PdGDSObj Node, int Index, char *OutStr,
		size_t OutBufLen);
	extern bool gds_AttrIxSetName(PdGDSObj Node, int Index,
		const char *NewName);
	extern int gds_AttrIxStr(PdGDSObj Node, int Index, char *OutStr,
		size_t OutBufLen);
	extern bool gds_AttrIxToStr(PdGDSObj Node, int Index, const char *Str);
	extern int gds_AttrIxType(PdGDSObj Node, int Index);
	extern const char *gds_AttrIxTypeStr(PdGDSObj Node, int Index);


	// Functions for CdContainer - TdIterator

	extern bool gds_IterGetStart(PdContainer Node, PdIterator Out);
	extern bool gds_IterGetEnd(PdContainer Node, PdIterator Out);
	extern PdContainer gds_IterGetHandle(PdIterator I);
	extern bool gds_IterAdv(PdIterator I);
	extern bool gds_IterAdvEx(PdIterator I, const ssize_t offset);
	extern bool gds_IterPrev(PdIterator I);
	extern bool gds_IterPrevEx(PdIterator I, const ssize_t offset);
	extern int gds_IterInt(PdIterator I);
	extern double gds_IterFloat(PdIterator I);
	extern int gds_IterStr(PdIterator I, char *OutStr, size_t OutBufLen);
	extern bool gds_IterIntTo(PdIterator I, int val);
	extern bool gds_IterFloatTo(PdIterator I, double val);
	extern bool gds_IterStrTo(PdIterator I, const char *Str);
	extern size_t gds_IterRData(PdIterator I, void *OutBuf, size_t Cnt,
		TSVType OutSV);
	extern size_t gds_IterWData(PdIterator I, const void *InBuf,
		size_t Cnt, TSVType InSV);


	// Functions for CdSequenceX

	extern int gds_SeqDimCnt(PdSequenceX Obj);
	extern bool gds_SeqGetDim(PdSequenceX Obj, int *OutBuf);
	extern Int64 gds_SeqGetCount(PdSequenceX Obj);
	extern int gds_SeqSVType(PdSequenceX Obj);
	extern int gds_SeqBitOf(PdSequenceX Obj);
	extern bool gds_SeqIndexIter(PdSequenceX Obj, int *Index, PdIterator Out);
	extern int gds_SeqFStrMaxLen(PdSequenceX Obj);

	// CdSequenceX -- read
	extern bool gds_rData(PdSequenceX Obj, CoreArray::Int32 const* Start,
		CoreArray::Int32 const* Length, void *OutBuf, TSVType OutSV);
	extern bool gds_rDataEx(PdSequenceX Obj, CoreArray::Int32 const* Start,
		CoreArray::Int32 const* Length, const CBOOL *const Selection[],
		void *OutBuf, TSVType OutSV);

	// CdSequenceX -- write
	extern bool gds_wData(PdSequenceX Obj, CoreArray::Int32 const* Start,
		CoreArray::Int32 const* Length, const void *InBuf, TSVType InSV);

	// CdSequenceX -- append
	extern bool gds_AppendData(PdSequenceX Obj, int Cnt, const void *InBuf,
		TSVType InSV);
	extern bool gds_AppendString(PdSequenceX Obj, int Cnt,
		const char *buffer[]);

	// CdSequenceX -- Assign
	extern bool gds_Assign(PdSequenceX dest_obj, PdSequenceX src_obj,
		bool append);


	// Functions for CdGDSStreamContainer

	extern PdGDSStreamContainer gds_NewContainer(PdGDSFolder Folder,
		const char *Name, const char *InputFile, const char *PackMode);
	extern bool gds_SaveContainer(PdGDSStreamContainer Container,
		const char *OutputFile);
	extern Int64 gds_ContainerGetSize(PdGDSStreamContainer Container);


	// Diagnosis

	extern int gds_DiagnBlockCount(PdGDSFile Handle);
	extern PdBlockStream_BlockInfo const gds_DiagnBlockInfo(
		PdGDSFile Handle, int Index);
	extern bool gds_DiagnBlockSize(PdGDSFile Handle, int Index,
		Int64 &size, Int64 &capacity);
	extern PdBlockStream_BlockInfo const gds_DiagnUnusedBlock(
		PdGDSFile Handle);


	// ******************************************************************
	// ****  the functions for machine configuration
	//

	// Return NaN
	extern float conf_F32_NaN();
	extern double conf_F64_NaN();

	// Return infinity
	extern float conf_F32_Inf();
	extern double conf_F64_Inf();

	// Return negative infinity
	extern float conf_F32_NegInf();
	extern double conf_F64_NegInf();

	// Return whether it is a finite number
	extern bool conf_IsFinite32(float val);
	extern bool conf_IsFinite64(double val);

	// Return the number of available CPU cores in the system
	extern int conf_GetNumberOfCPU();

	/// Return the size in byte of level-1 cache memory
	extern int conf_GetL1CacheMemory();

	/// Return the size in byte of level-2 cache memory
    extern int conf_GetL2CacheMemory();



	// ******************************************************************
	// ****	 the functions for parellel computing
	//

	// thread mutex

	/// create a mutex object
	extern PdThreadMutex plc_InitMutex();
	/// destroy the mutex object
	extern bool plc_DoneMutex(PdThreadMutex obj);
	/// lock the mutex object
	extern bool plc_LockMutex(PdThreadMutex obj);
	/// unlock the mutex object
	extern bool plc_UnlockMutex(PdThreadMutex obj);

	// thread suspending object

	/// initialize a thread suspending object
	extern PdThreadsSuspending plc_InitSuspend();
	/// destroy the thread suspending object
	extern bool plc_DoneSuspend(PdThreadsSuspending obj);
	/// suspend the thread suspending object
	extern bool plc_Suspend(PdThreadsSuspending obj);
	/// wakeup the thread suspending object
	extern bool plc_WakeUp(PdThreadsSuspending obj);

	/// parallel computing
	extern bool plc_DoBaseThread(void (*Proc)(PdThread, int, void*),
		void *param, int nThread);


	// ******************************************************************
	// ****	 the functions for block read
	//

	/// read an array-oriented object margin by margin
	extern PdArrayRead gds_ArrayRead_Init(PdSequenceX Obj,
		int Margin, TSVType SVType, const CBOOL *const Selection[],
		bool buf_if_need=true);
	/// free a 'CdArrayRead' object
	extern bool gds_ArrayRead_Free(PdArrayRead Obj);

	/// read data
	extern bool gds_ArrayRead_Read(PdArrayRead Obj, void *Buffer);
	/// return true, if it is of the end
	extern bool gds_ArrayRead_Eof(PdArrayRead Obj);
	/// reallocate the buffer with specified size with respect to array
	extern bool gds_Balance_ArrayRead_Buffer(PdArrayRead array[],
		int n, Int64 buffer_size=-1);


	// ******************************************************************
	// ****	 error functions
	//

	/// get the last error message
	extern const char *gds_Error();

	/// get the last error message
	extern std::string *gds_LastError();

} // extern "C"

#endif /* _COREGDS_H_ */
