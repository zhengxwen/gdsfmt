// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// gdsfmt_init.cpp: the initialization of R interface
//
// Copyright (C) 2014	Xiuwen Zheng
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

// TODO: double check 'ClearDim' in futures

#include <CoreArray.h>
#include <CoreGDS.h>

#include <Rdefines.h>
#include <R_ext/Rdynload.h>


using namespace std;
using namespace CoreArray;


extern "C"
{

extern bool gds_Is_R_Logical(CdGDSObj &Obj);
extern bool gds_Is_R_Factor(CdGDSObj &Obj);
extern int gds_Set_If_R_Factor(CdGDSObj &Obj, SEXP val);
extern SEXP gds_Read_SEXP(CdSequenceX *Obj, CoreArray::Int32 const* Start,
	CoreArray::Int32 const* Length, const CBOOL *const Selection[]);


// ------------------------------------------------------------------------

void R_init_gdsfmt(DllInfo *info)
{
	const char *pkg_name = "gdsfmt";

	#define REG(name)	R_RegisterCCallable(pkg_name, #name, (DL_FUNC)name)

	REG(gds_SysInit);
	REG(gds_SysVersion);
	REG(gds_SysVersionStr);
	REG(gds_SysClassCount);
	REG(gds_SysClassStruct);
	REG(gds_SysPipeCount);
	REG(gds_SysPipeStruct);

	REG(gds_FileOpen);
	REG(gds_FileCreate);
	REG(gds_FileClose);
	REG(gds_FileSync);
	REG(gds_FileName);
	REG(gds_FileReadOnly);
	REG(gds_FilePath);
	REG(gds_FileSize);
	REG(gds_FileTidyUp);

	REG(gds_NodeRoot);
	REG(gds_NodeGDSFile);
	REG(gds_NodeChildCount);
	REG(gds_NodeChildFolder);
	REG(gds_NodeChild);
	REG(gds_NodePath);
	REG(gds_NodeDelete);
	REG(gds_NodeMoveTo);
	REG(gds_NodeCopyTo);
	REG(gds_NodeAddLabel);
	REG(gds_NodeAddFolder);
	REG(gds_NodeAddArray);

	REG(gds_NodeIsFolder);
	REG(gds_NodeIsNull);
	REG(gds_NodeIsContainer);
	REG(gds_NodeIsSequence);
	REG(gds_NodeIsVector);
	REG(gds_NodeIsFStr);
	REG(gds_NodeIsStreamContainer);

	REG(gds_NodeName);
	REG(gds_NodeSetName);
	REG(gds_NodeClassName);
	REG(gds_NodeStreamInfo);
	REG(gds_SetPackedMode);
	REG(gds_NodeFree);

	REG(gds_AttrCount);
	REG(gds_AttrNameIndex);
	REG(gds_AttrIxAdd);
	REG(gds_AttrIxName);
	REG(gds_AttrIxSetName);
	REG(gds_AttrIxStr);
	REG(gds_AttrIxToStr);
	REG(gds_AttrIxType);
	REG(gds_AttrIxTypeStr);

	REG(gds_IterGetStart);
	REG(gds_IterGetEnd);
	REG(gds_IterGetHandle);
	REG(gds_IterAdv);
	REG(gds_IterAdvEx);
	REG(gds_IterPrev);
	REG(gds_IterPrevEx);
	REG(gds_IterInt);
	REG(gds_IterFloat);
	REG(gds_IterStr);
	REG(gds_IterIntTo);
	REG(gds_IterFloatTo);
	REG(gds_IterStrTo);
	REG(gds_IterRData);
	REG(gds_IterWData);

	REG(gds_SeqDimCnt);
	REG(gds_SeqGetDim);
	REG(gds_SeqGetCount);
	REG(gds_SeqSVType);
	REG(gds_SeqBitOf);
	REG(gds_SeqIndexIter);
	REG(gds_SeqFStrMaxLen);

	REG(gds_rData);
	REG(gds_rDataEx);
	REG(gds_wData);
	REG(gds_AppendData);
	REG(gds_AppendString);
	REG(gds_Assign);

	REG(gds_NewContainer);
	REG(gds_SaveContainer);
	REG(gds_ContainerGetSize);

	REG(gds_DiagnBlockCount);
	REG(gds_DiagnBlockInfo);
	REG(gds_DiagnBlockSize);
	REG(gds_DiagnUnusedBlock);

	REG(conf_GetNumberOfCPU);
	REG(conf_GetL1CacheMemory);
	REG(conf_GetL2CacheMemory);

	REG(plc_InitMutex);
	REG(plc_DoneMutex);
	REG(plc_LockMutex);
	REG(plc_UnlockMutex);
	REG(plc_InitSuspend);
	REG(plc_DoneSuspend);
	REG(plc_Suspend);
	REG(plc_WakeUp);
	REG(plc_DoBaseThread);

	REG(gds_ArrayRead_Init);
	REG(gds_ArrayRead_Free);
	REG(gds_ArrayRead_Read);
	REG(gds_ArrayRead_Eof);
	REG(gds_Balance_ArrayRead_Buffer);

	REG(gds_Is_R_Logical);
	REG(gds_Is_R_Factor);
	REG(gds_Set_If_R_Factor);
	REG(gds_Read_SEXP);
	REG(gds_Error);
	REG(gds_LastError);
}

} // extern "C"
