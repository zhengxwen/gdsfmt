// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// R_GDS.cpp: Link C interface from the CoreArray's dynamic library "CoreGDS"
//
// Copyright (C) 2011 - 2014	Xiuwen Zheng [zhengx@u.washington.edu]
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
 *	\file     R_GDS.cpp
 *	\author   Xiuwen Zheng
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Link C interface from the CoreArray's dynamic library "CoreGDS"
 *	\details
**/


#ifndef _R_GDS_CPP_
#define _R_GDS_CPP_

#include <R_ext/Rdynload.h>

namespace CoreArray
{
	DL_FUNC fnptr_SysInit = NULL,
		fnptr_SysVersion = NULL,
		fnptr_SysVersionStr = NULL,
		fnptr_SysClassCount = NULL,
		fnptr_SysClassStruct = NULL,
		fnptr_SysPipeCount = NULL,
		fnptr_SysPipeStruct = NULL;

	DL_FUNC fnptr_FileOpen = NULL,
		fnptr_FileCreate = NULL,
		fnptr_FileClose = NULL,
		fnptr_FileSync = NULL,
		fnptr_FileName = NULL,
		fnptr_FileReadOnly = NULL,
		fnptr_FilePath = NULL,
		fnptr_FileSize = NULL,
		fnptr_FileTidyUp = NULL;

	DL_FUNC fnptr_NodeRoot = NULL,
		fnptr_NodeGDSFile = NULL,
		fnptr_NodeChildCount = NULL,
		fnptr_NodeChildFolder = NULL,
		fnptr_NodeChild = NULL,
		fnptr_NodePath = NULL,
		fnptr_NodeDelete = NULL,
		fnptr_NodeMoveTo = NULL,
		fnptr_NodeCopyTo = NULL,
		fnptr_NodeAddLabel = NULL,
		fnptr_NodeAddFolder = NULL,
		fnptr_NodeAddArray = NULL;

	DL_FUNC fnptr_NodeIsFolder = NULL,
		fnptr_NodeIsNull = NULL,
		fnptr_NodeIsContainer = NULL,
		fnptr_NodeIsSequence = NULL,
		fnptr_NodeIsVector = NULL,
		fnptr_NodeIsFStr = NULL,
		fnptr_NodeIsStreamContainer = NULL;

	DL_FUNC fnptr_NodeName = NULL,
		fnptr_NodeSetName = NULL,
		fnptr_NodeClassName = NULL,
		fnptr_NodeStreamInfo = NULL,
		fnptr_SetPackedMode = NULL,
		fnptr_NodeFree = NULL;

	DL_FUNC fnptr_AttrCount = NULL,
		fnptr_AttrNameIndex = NULL,
		fnptr_AttrIxAdd = NULL,
		fnptr_AttrIxName = NULL,
		fnptr_AttrIxSetName = NULL,
		fnptr_AttrIxStr = NULL,
		fnptr_AttrIxToStr = NULL,
		fnptr_AttrIxType = NULL,
		fnptr_AttrIxTypeStr = NULL;

	DL_FUNC fnptr_IterGetStart = NULL,
		fnptr_IterGetEnd = NULL,
		fnptr_IterGetHandle = NULL,
		fnptr_IterAdv = NULL,
		fnptr_IterAdvEx = NULL,
		fnptr_IterPrev = NULL,
		fnptr_IterPrevEx = NULL,
		fnptr_IterInt = NULL,
		fnptr_IterFloat = NULL,
		fnptr_IterStr = NULL,
		fnptr_IterIntTo = NULL,
		fnptr_IterFloatTo = NULL,
		fnptr_IterStrTo = NULL,
		fnptr_IterRData = NULL,
		fnptr_IterWData = NULL;

	DL_FUNC fnptr_SeqDimCnt = NULL,
		fnptr_SeqGetDim = NULL,
		fnptr_SeqGetCount = NULL,
		fnptr_SeqSVType = NULL,
		fnptr_SeqBitOf = NULL,
		fnptr_SeqIndexIter = NULL,
		fnptr_SeqFStrMaxLen = NULL;

	DL_FUNC fnptr_rData = NULL,
		fnptr_rDataEx = NULL,
		fnptr_wData = NULL,
		fnptr_AppendData = NULL,
		fnptr_AppendString = NULL,
		fnptr_Assign = NULL;

	DL_FUNC fnptr_NewContainer = NULL,
		fnptr_SaveContainer = NULL,
		fnptr_ContainerGetSize = NULL;

	DL_FUNC fnptr_DiagnBlockCount = NULL,
		fnptr_DiagnBlockInfo = NULL,
		fnptr_DiagnBlockSize = NULL,
		fnptr_DiagnUnusedBlock = NULL;

	DL_FUNC fnptr_GetNumberOfCPU = NULL,
		fnptr_GetL1CacheMemory = NULL,
		fnptr_GetL2CacheMemory = NULL;

	DL_FUNC fnptr_InitMutex = NULL,
		fnptr_DoneMutex = NULL,
		fnptr_LockMutex = NULL,
		fnptr_UnlockMutex = NULL,
		fnptr_InitSuspend = NULL,
		fnptr_DoneSuspend = NULL,
		fnptr_Suspend = NULL,
		fnptr_WakeUp = NULL,
		fnptr_DoBaseThread = NULL;

	DL_FUNC fnptr_ArrayRead_Init = NULL,
		fnptr_ArrayRead_Free = NULL,
		fnptr_ArrayRead_Read = NULL,
		fnptr_ArrayRead_Eof = NULL,
		fnptr_Balance_ArrayRead_Buffer = NULL;

	DL_FUNC fnptr_Is_R_Logical = NULL,
		fnptr_Is_R_Factor = NULL,
		fnptr_Set_If_R_Factor = NULL,
		fnptr_Read_SEXP = NULL;

	DL_FUNC fnptr_Error = NULL,
		fnptr_LastError = NULL;
}

#endif /* _R_GDS_CPP_ */
