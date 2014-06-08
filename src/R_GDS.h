// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// R_GDS.h: Link C interface from the CoreArray's dynamic library "CoreGDS"
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
 *	\file     R_GDS.h
 *	\author   Xiuwen Zheng
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Link C interface from the CoreArray's dynamic library "CoreGDS"
 *	\details
**/

#ifndef _R_GDS_H_
#define _R_GDS_H_

#ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
#endif

#include <R.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>

// to avoid the conflict with C++
#ifdef length
#  undef length
#endif


#include <CoreDEF.h>
#include <dType.h>
#include <dString.h>

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>


namespace CoreArray
{
	/// Error Macro
	#define _COREARRAY_ERRMACRO_(x) { \
		va_list args; va_start(args, x); \
		Init(x, args); \
		va_end(args); \
	}

	/// The root class of exception for CoreArray library
	class ErrCoreArray: public std::exception
	{
	public:
		ErrCoreArray() {};
		ErrCoreArray(const char *fmt, ...)
			{ _COREARRAY_ERRMACRO_(fmt); }
		ErrCoreArray(const std::string &msg)
			{ fMessage = msg; }

		virtual const char *what() const throw()
			{ return fMessage.c_str(); }
		virtual ~ErrCoreArray() throw() {};

	protected:
		std::string fMessage;

		void Init(const char *fmt, va_list arglist)
		{
			char buf[1024];
			vsnprintf(buf, sizeof(buf), fmt, arglist);
			fMessage = buf;
		}
	};


	// ******************************************************************
	// ****  the functions for a GDS file
	//

	/// the interface for CoreArray library

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
	/// the pointer to a stream container
	typedef void* PdGDSStreamContainer;

	/// Iterator for CoreArray array-oriented container
	/** sizeof(TdIterator) = 32 **/
	struct TdIterator
	{
        /// the handler of this iterator
		PdContainer* Handler;
		unsigned char VByte[32 - sizeof(PdContainer)];
	};

	typedef TdIterator* PdIterator;


	typedef void* PdBlockStream_BlockInfo;


	// ******************************************************************
	// ****  the functions for parellel computing
	//

	/// the class of mutex object
	typedef void* PdThreadMutex;

	/// the class of suspending object
	typedef void* PdThreadsSuspending;

	/// the class of thread object
	typedef void* PdThread;


	// ******************************************************************
	// ****	 the functions for block read
	//

	typedef void* PdArrayRead;



	// ******************************************************************
	// ******************************************************************
	// ******************************************************************

	static const char *gds_name = "gdsfmt";

	extern DL_FUNC fnptr_SysInit, fnptr_SysVersion, fnptr_SysVersionStr,
		fnptr_SysClassCount, fnptr_SysClassStruct, fnptr_SysPipeCount,
		fnptr_SysPipeStruct;

	extern DL_FUNC fnptr_FileOpen, fnptr_FileCreate, fnptr_FileClose,
		fnptr_FileSync, fnptr_FileName, fnptr_FileReadOnly,
		fnptr_FilePath, fnptr_FileSize, fnptr_FileTidyUp;

	extern DL_FUNC fnptr_NodeRoot, fnptr_NodeGDSFile, fnptr_NodeChildCount,
		fnptr_NodeChildFolder, fnptr_NodeChild, fnptr_NodePath,
		fnptr_NodeDelete, fnptr_NodeMoveTo, fnptr_NodeCopyTo,
		fnptr_NodeAddLabel, fnptr_NodeAddFolder, fnptr_NodeAddArray;

	extern DL_FUNC fnptr_NodeIsFolder, fnptr_NodeIsNull, fnptr_NodeIsContainer,
		fnptr_NodeIsSequence, fnptr_NodeIsVector, fnptr_NodeIsFStr,
		fnptr_NodeIsStreamContainer;

	extern DL_FUNC fnptr_NodeName, fnptr_NodeSetName, fnptr_NodeClassName,
		fnptr_NodeStreamInfo, fnptr_SetPackedMode, fnptr_NodeFree;

	extern DL_FUNC fnptr_AttrCount, fnptr_AttrNameIndex, fnptr_AttrIxAdd,
		fnptr_AttrIxName, fnptr_AttrIxSetName, fnptr_AttrIxStr,
		fnptr_AttrIxToStr, fnptr_AttrIxType, fnptr_AttrIxTypeStr;

	extern DL_FUNC fnptr_IterGetStart, fnptr_IterGetEnd, fnptr_IterGetHandle,
		fnptr_IterAdv, fnptr_IterAdvEx, fnptr_IterPrev,
		fnptr_IterPrevEx, fnptr_IterInt, fnptr_IterFloat,
		fnptr_IterStr, fnptr_IterIntTo, fnptr_IterFloatTo,
		fnptr_IterStrTo, fnptr_IterRData, fnptr_IterWData;

	extern DL_FUNC fnptr_SeqDimCnt, fnptr_SeqGetDim, fnptr_SeqGetCount,
		fnptr_SeqSVType, fnptr_SeqBitOf, fnptr_SeqIndexIter,
		fnptr_SeqFStrMaxLen;

	extern DL_FUNC fnptr_rData, fnptr_rDataEx, fnptr_wData,
		fnptr_AppendData, fnptr_AppendString, fnptr_Assign;

	extern DL_FUNC fnptr_NewContainer, fnptr_SaveContainer,
		fnptr_ContainerGetSize;

	extern DL_FUNC fnptr_DiagnBlockCount, fnptr_DiagnBlockInfo,
		fnptr_DiagnBlockSize, fnptr_DiagnUnusedBlock;

	extern DL_FUNC fnptr_GetNumberOfCPU, fnptr_GetL1CacheMemory,
		fnptr_GetL2CacheMemory;

	extern DL_FUNC fnptr_InitMutex, fnptr_DoneMutex, fnptr_LockMutex,
		fnptr_UnlockMutex, fnptr_InitSuspend, fnptr_DoneSuspend,
		fnptr_Suspend, fnptr_WakeUp, fnptr_DoBaseThread;

	extern DL_FUNC fnptr_ArrayRead_Init, fnptr_ArrayRead_Free,
		fnptr_ArrayRead_Read, fnptr_ArrayRead_Eof,
		fnptr_Balance_ArrayRead_Buffer;

	extern DL_FUNC fnptr_Is_R_Logical, fnptr_Is_R_Factor,
		fnptr_Set_If_R_Factor, fnptr_Read_SEXP;

	extern DL_FUNC fnptr_Error,fnptr_LastError;



	// ******************************************************************
	// ****  Functions for CoreArray System
	//

	R_INLINE static void gds_SysInit()
	{
		typedef void (*TYPE)();
		if (!fnptr_SysInit)
			fnptr_SysInit = R_GetCCallable(gds_name, "gds_SysInit");
		(*TYPE(fnptr_SysInit))();
	}

	R_INLINE static int gds_SysVersion()
	{
		typedef int (*TYPE)();
		if (!fnptr_SysVersion)
			fnptr_SysVersion = R_GetCCallable(gds_name, "gds_SysVersion");
		return (*TYPE(fnptr_SysVersion))();
	}

	R_INLINE static const char *gds_SysVersionStr()
	{
		typedef const char* (*TYPE)();
		if (!fnptr_SysVersionStr)
			fnptr_SysVersionStr = R_GetCCallable(gds_name, "gds_SysVersionStr");
		return (*TYPE(fnptr_SysVersionStr))();
	}

	R_INLINE static int gds_SysClassCount()
	{
		typedef int (*TYPE)();
		if (!fnptr_SysClassCount)
			fnptr_SysClassCount = R_GetCCallable(gds_name, "gds_SysClassCount");
		return (*TYPE(fnptr_SysClassCount))();
	}

	R_INLINE static bool gds_SysClassStruct(int Index, const char **OutName,
		const char **OutDesp, int *OutClassType)
	{
		typedef bool (*TYPE)(int, const char **, const char **, int *);
		if (!fnptr_SysClassStruct)
			fnptr_SysClassStruct = R_GetCCallable(gds_name, "gds_SysClassStruct");
		return (*TYPE(fnptr_SysClassStruct))(Index, OutName, OutDesp, OutClassType);
	}
	
	R_INLINE static int gds_SysPipeCount()
	{
		typedef int (*TYPE)();
		if (!fnptr_SysPipeCount)
			fnptr_SysPipeCount = R_GetCCallable(gds_name, "gds_SysPipeCount");
		return (*TYPE(fnptr_SysPipeCount))();
	}

	R_INLINE static bool gds_SysPipeStruct(int Index, const char **OutCoder,
		const char **OutDesp)
	{
		typedef bool (*TYPE)(int, const char **, const char **);
		if (!fnptr_SysPipeStruct)
			fnptr_SysPipeStruct = R_GetCCallable(gds_name, "gds_SysPipeStruct");
		return (*TYPE(fnptr_SysPipeStruct))(Index, OutCoder, OutDesp);
	}


	// Functions for File

	R_INLINE static PdGDSFile gds_FileOpen(const char *FileName, bool ReadOnly)
	{
		typedef PdGDSFile (*TYPE)(const char *, bool);
		if (!fnptr_FileOpen)
			fnptr_FileOpen = R_GetCCallable(gds_name, "gds_FileOpen");
		return (*TYPE(fnptr_FileOpen))(FileName, ReadOnly);
	}

	R_INLINE static PdGDSFile gds_FileCreate(const char *FileName)
	{
		typedef PdGDSFile (*TYPE)(const char *);
		if (!fnptr_FileCreate)
			fnptr_FileCreate = R_GetCCallable(gds_name, "gds_FileCreate");
		return (*TYPE(fnptr_FileCreate))(FileName);
	}

	R_INLINE static bool gds_FileClose(PdGDSFile Handle)
	{
		typedef bool (*TYPE)(PdGDSFile);
		if (!fnptr_FileClose)
			fnptr_FileClose = R_GetCCallable(gds_name, "gds_FileClose");
		return (*TYPE(fnptr_FileClose))(Handle);
	}

	R_INLINE static bool gds_FileSync(PdGDSFile Handle)
	{
		typedef bool (*TYPE)(PdGDSFile);
		if (!fnptr_FileSync)
			fnptr_FileSync = R_GetCCallable(gds_name, "gds_FileSync");
		return (*TYPE(fnptr_FileSync))(Handle);
	}

	R_INLINE static int gds_FileName(PdGDSFile Handle, char *OutStr,
		size_t OutBufLen)
	{
		typedef int (*TYPE)(PdGDSFile, char *, size_t);
		if (!fnptr_FileName)
			fnptr_FileName = R_GetCCallable(gds_name, "gds_FileName");
		return (*TYPE(fnptr_FileName))(Handle, OutStr, OutBufLen);
	}

	R_INLINE static bool gds_FileReadOnly(PdGDSFile Handle)
	{
		typedef bool (*TYPE)(PdGDSFile);
		if (!fnptr_FileReadOnly)
			fnptr_FileReadOnly = R_GetCCallable(gds_name, "gds_FileReadOnly");
		return (*TYPE(fnptr_FileReadOnly))(Handle);
	}

	R_INLINE static PdGDSObj gds_FilePath(PdGDSFile Handle, const char *Path)
	{
		typedef PdGDSObj (*TYPE)(PdGDSFile, const char *);
		if (!fnptr_FilePath)
			fnptr_FilePath = R_GetCCallable(gds_name, "gds_FilePath");
		return (*TYPE(fnptr_FilePath))(Handle, Path);
	}

	R_INLINE static Int64 gds_FileSize(PdGDSFile Handle)
	{
		typedef Int64 (*TYPE)(PdGDSFile);
		if (!fnptr_FileSize)
			fnptr_FileSize = R_GetCCallable(gds_name, "gds_FileSize");
		return (*TYPE(fnptr_FileSize))(Handle);
	}

	R_INLINE static bool gds_FileTidyUp(PdGDSFile Handle)
	{
		typedef Int64 (*TYPE)(PdGDSFile);
		if (!fnptr_FileTidyUp)
			fnptr_FileTidyUp = R_GetCCallable(gds_name, "gds_FileTidyUp");
		return (*TYPE(fnptr_FileTidyUp))(Handle);
	}


	// Functions for File Structure

	R_INLINE static PdGDSFolder gds_NodeRoot(PdGDSFile Handle)
	{
		typedef PdGDSFolder (*TYPE)(PdGDSFile);
		if (!fnptr_NodeRoot)
			fnptr_NodeRoot = R_GetCCallable(gds_name, "gds_NodeRoot");
		return (*TYPE(fnptr_NodeRoot))(Handle);
	}

	R_INLINE static PdGDSFile gds_NodeGDSFile(PdGDSObj Node)
	{
		typedef PdGDSFile (*TYPE)(PdGDSObj);
		if (!fnptr_NodeGDSFile)
			fnptr_NodeGDSFile = R_GetCCallable(gds_name, "gds_NodeGDSFile");
		return (*TYPE(fnptr_NodeGDSFile))(Node);
	}

	R_INLINE static int gds_NodeChildCount(PdGDSFolder Node)
	{
		typedef int (*TYPE)(PdGDSFolder);
		if (!fnptr_NodeChildCount)
			fnptr_NodeChildCount = R_GetCCallable(gds_name, "gds_NodeChildCount");
		return (*TYPE(fnptr_NodeChildCount))(Node);
	}

	R_INLINE static PdGDSFolder gds_NodeChildFolder(PdGDSFolder Node, int Index)
	{
		typedef PdGDSFolder (*TYPE)(PdGDSFolder, int);
		if (!fnptr_NodeChildFolder)
			fnptr_NodeChildFolder = R_GetCCallable(gds_name, "gds_NodeChildFolder");
		return (*TYPE(fnptr_NodeChildFolder))(Node, Index);
	}

	R_INLINE static PdGDSObj gds_NodeChild(PdGDSFolder Node, int Index)
	{
		typedef PdGDSObj (*TYPE)(PdGDSFolder, int);
		if (!fnptr_NodeChild)
			fnptr_NodeChild = R_GetCCallable(gds_name, "gds_NodeChild");
		return (*TYPE(fnptr_NodeChild))(Node, Index);
	}

	R_INLINE static PdGDSObj gds_NodePath(PdGDSFolder Node, const char *Path)
	{
		typedef PdGDSObj (*TYPE)(PdGDSFolder, const char *);
		if (!fnptr_NodePath)
			fnptr_NodePath = R_GetCCallable(gds_name, "gds_NodePath");
		return (*TYPE(fnptr_NodePath))(Node, Path);
	}

	R_INLINE static bool gds_NodeDelete(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeDelete)
			fnptr_NodeDelete = R_GetCCallable(gds_name, "gds_NodeDelete");
		return (*TYPE(fnptr_NodeDelete))(Node);
	}

	R_INLINE static bool gds_NodeMoveTo(PdGDSObj Source, PdGDSFolder Dest)
	{
		typedef bool (*TYPE)(PdGDSObj, PdGDSFolder);
		if (!fnptr_NodeMoveTo)
			fnptr_NodeMoveTo = R_GetCCallable(gds_name, "gds_NodeMoveTo");
		return (*TYPE(fnptr_NodeMoveTo))(Source, Dest);
	}

	R_INLINE static bool gds_NodeCopyTo(PdGDSObj Source, PdGDSFolder Dest)
	{
		typedef bool (*TYPE)(PdGDSObj, PdGDSFolder);
		if (!fnptr_NodeCopyTo)
			fnptr_NodeCopyTo = R_GetCCallable(gds_name, "gds_NodeCopyTo");
		return (*TYPE(fnptr_NodeCopyTo))(Source, Dest);
	}

	R_INLINE static bool gds_NodeAddLabel(PdGDSFolder Folder, const char *Name)
	{
		typedef bool (*TYPE)(PdGDSFolder, const char *);
		if (!fnptr_NodeAddLabel)
			fnptr_NodeAddLabel = R_GetCCallable(gds_name, "gds_NodeAddLabel");
		return (*TYPE(fnptr_NodeAddLabel))(Folder, Name);
	}

	R_INLINE static PdGDSFolder gds_NodeAddFolder(PdGDSFolder Folder,
		const char *Name)
	{
		typedef PdGDSFolder (*TYPE)(PdGDSFolder, const char *);
		if (!fnptr_NodeAddFolder)
			fnptr_NodeAddFolder = R_GetCCallable(gds_name, "gds_NodeAddFolder");
		return (*TYPE(fnptr_NodeAddFolder))(Folder, Name);
	}

	R_INLINE static PdSequenceX gds_NodeAddArray(PdGDSFolder Folder,
		const char *Name, const char *TypeName, int DimCnt)
	{
		typedef PdSequenceX (*TYPE)(PdGDSFolder, const char *, const char *, int);
		if (!fnptr_NodeAddArray)
			fnptr_NodeAddArray = R_GetCCallable(gds_name, "gds_NodeAddArray");
		return (*TYPE(fnptr_NodeAddArray))(Folder, Name, TypeName, DimCnt);
	}

	R_INLINE static bool gds_NodeIsFolder(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeIsFolder)
			fnptr_NodeIsFolder = R_GetCCallable(gds_name, "gds_NodeIsFolder");
		return (*TYPE(fnptr_NodeIsFolder))(Node);
	}

	R_INLINE static bool gds_NodeIsNull(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeIsNull)
			fnptr_NodeIsNull = R_GetCCallable(gds_name, "gds_NodeIsNull");
		return (*TYPE(fnptr_NodeIsNull))(Node);
	}

	R_INLINE static bool gds_NodeIsContainer(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeIsContainer)
			fnptr_NodeIsContainer = R_GetCCallable(gds_name, "gds_NodeIsContainer");
		return (*TYPE(fnptr_NodeIsContainer))(Node);
	}

	R_INLINE static bool gds_NodeIsSequence(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeIsSequence)
			fnptr_NodeIsSequence = R_GetCCallable(gds_name, "gds_NodeIsSequence");
		return (*TYPE(fnptr_NodeIsSequence))(Node);
	}

	R_INLINE static bool gds_NodeIsVector(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeIsVector)
			fnptr_NodeIsVector = R_GetCCallable(gds_name, "gds_NodeIsVector");
		return (*TYPE(fnptr_NodeIsVector))(Node);
	}

	R_INLINE static bool gds_NodeIsFStr(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeIsFStr)
			fnptr_NodeIsFStr = R_GetCCallable(gds_name, "gds_NodeIsFStr");
		return (*TYPE(fnptr_NodeIsFStr))(Node);
	}

	R_INLINE static bool gds_NodeIsStreamContainer(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeIsStreamContainer)
			fnptr_NodeIsStreamContainer = R_GetCCallable(gds_name, "gds_NodeIsStreamContainer");
		return (*TYPE(fnptr_NodeIsStreamContainer))(Node);
	}

	R_INLINE static int gds_NodeName(PdGDSObj Node, char *OutStr,
		size_t OutBufLen, bool FullName)
	{
		typedef int (*TYPE)(PdGDSObj, char *, size_t, bool);
		if (!fnptr_NodeName)
			fnptr_NodeName = R_GetCCallable(gds_name, "gds_NodeName");
		return (*TYPE(fnptr_NodeName))(Node, OutStr, OutBufLen, FullName);
	}

	R_INLINE static bool gds_NodeSetName(PdGDSObj Node, char *NewName)
	{
		typedef bool (*TYPE)(PdGDSObj, char *);
		if (!fnptr_NodeSetName)
			fnptr_NodeSetName = R_GetCCallable(gds_name, "gds_NodeSetName");
		return (*TYPE(fnptr_NodeSetName))(Node, NewName);
	}

	R_INLINE static int gds_NodeClassName(PdGDSObj Node, char *OutStr,
		size_t OutBufLen)
	{
		typedef int (*TYPE)(PdGDSObj, char *, size_t);
		if (!fnptr_NodeClassName)
			fnptr_NodeClassName = R_GetCCallable(gds_name, "gds_NodeClassName");
		return (*TYPE(fnptr_NodeClassName))(Node, OutStr, OutBufLen);
	}

	R_INLINE static bool gds_NodeStreamInfo(PdGDSObj Node, Int64 *TotalIn,
		Int64 *TotalOut, const char **StreamDesp)
	{
		typedef bool (*TYPE)(PdGDSObj, Int64 *, Int64 *, const char **);
		if (!fnptr_NodeStreamInfo)
			fnptr_NodeStreamInfo = R_GetCCallable(gds_name, "gds_NodeStreamInfo");
		return (*TYPE(fnptr_NodeStreamInfo))(Node, TotalIn, TotalOut, StreamDesp);
	}

	R_INLINE static bool gds_SetPackedMode(PdGDSObj Node, const char *Mode)
	{
		typedef bool (*TYPE)(PdGDSObj, const char *);
		if (!fnptr_SetPackedMode)
			fnptr_SetPackedMode = R_GetCCallable(gds_name, "gds_SetPackedMode");
		return (*TYPE(fnptr_SetPackedMode))(Node, Mode);
	}

	R_INLINE static bool gds_NodeFree(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_NodeFree)
			fnptr_NodeFree = R_GetCCallable(gds_name, "gds_NodeFree");
		return (*TYPE(fnptr_NodeFree))(Node);
	}


	// Functions for File Node -- TdObjAttr

	R_INLINE static int gds_AttrCount(PdGDSObj Node)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_AttrCount)
			fnptr_AttrCount = R_GetCCallable(gds_name, "gds_AttrCount");
		return (*TYPE(fnptr_AttrCount))(Node);
	}

	R_INLINE static int gds_AttrNameIndex(PdGDSObj Node, const char *Name)
	{
		typedef int (*TYPE)(PdGDSObj, const char *);
		if (!fnptr_AttrNameIndex)
			fnptr_AttrNameIndex = R_GetCCallable(gds_name, "gds_AttrNameIndex");
		return (*TYPE(fnptr_AttrNameIndex))(Node, Name);
	}

	R_INLINE static bool gds_AttrIxAdd(PdGDSObj Node, const char *Name,
		const char *Value)
	{
		typedef bool (*TYPE)(PdGDSObj, const char *, const char *);
		if (!fnptr_AttrIxAdd)
			fnptr_AttrIxAdd = R_GetCCallable(gds_name, "gds_AttrIxAdd");
		return (*TYPE(fnptr_AttrIxAdd))(Node, Name, Value);
	}

	R_INLINE static int gds_AttrIxName(PdGDSObj Node, int Index, char *OutStr,
		size_t OutBufLen)
	{
		typedef int (*TYPE)(PdGDSObj, int, char *, size_t);
		if (!fnptr_AttrIxName)
			fnptr_AttrIxName = R_GetCCallable(gds_name, "gds_AttrIxName");
		return (*TYPE(fnptr_AttrIxName))(Node, Index, OutStr, OutBufLen);
	}

	R_INLINE static bool gds_AttrIxSetName(PdGDSObj Node, int Index,
		const char *NewName)
	{
		typedef bool (*TYPE)(PdGDSObj, int, const char *);
		if (!fnptr_AttrIxSetName)
			fnptr_AttrIxSetName = R_GetCCallable(gds_name, "gds_AttrIxSetName");
		return (*TYPE(fnptr_AttrIxSetName))(Node, Index, NewName);
	}

	R_INLINE static int gds_AttrIxStr(PdGDSObj Node, int Index, char *OutStr,
		size_t OutBufLen)
	{
		typedef int (*TYPE)(PdGDSObj, int, char *, size_t);
		if (!fnptr_AttrIxStr)
			fnptr_AttrIxStr = R_GetCCallable(gds_name, "gds_AttrIxStr");
		return (*TYPE(fnptr_AttrIxStr))(Node, Index, OutStr, OutBufLen);
	}

	R_INLINE static bool gds_AttrIxToStr(PdGDSObj Node, int Index,
		const char *Str)
	{
		typedef bool (*TYPE)(PdGDSObj, int, const char *);
		if (!fnptr_AttrIxToStr)
			fnptr_AttrIxToStr = R_GetCCallable(gds_name, "gds_AttrIxToStr");
		return (*TYPE(fnptr_AttrIxToStr))(Node, Index, Str);
	}

	R_INLINE static int gds_AttrIxType(PdGDSObj Node, int Index)
	{
		typedef int (*TYPE)(PdGDSObj, int);
		if (!fnptr_AttrIxType)
			fnptr_AttrIxType = R_GetCCallable(gds_name, "gds_AttrIxType");
		return (*TYPE(fnptr_AttrIxType))(Node, Index);
	}

	R_INLINE static const char *gds_AttrIxTypeStr(PdGDSObj Node, int Index)
	{
		typedef const char* (*TYPE)(PdGDSObj, int);
		if (!fnptr_AttrIxTypeStr)
			fnptr_AttrIxTypeStr = R_GetCCallable(gds_name, "gds_AttrIxTypeStr");
		return (*TYPE(fnptr_AttrIxTypeStr))(Node, Index);
	}


	// Functions for CdContainer - TdIterator

	R_INLINE static bool gds_IterGetStart(PdContainer Node, PdIterator Out)
	{
		typedef bool (*TYPE)(PdContainer, PdIterator);
		if (!fnptr_IterGetStart)
			fnptr_IterGetStart = R_GetCCallable(gds_name, "gds_IterGetStart");
		return (*TYPE(fnptr_IterGetStart))(Node, Out);
	}

	R_INLINE static bool gds_IterGetEnd(PdContainer Node, PdIterator Out)
	{
		typedef bool (*TYPE)(PdContainer, PdIterator);
		if (!fnptr_IterGetEnd)
			fnptr_IterGetEnd = R_GetCCallable(gds_name, "gds_IterGetEnd");
		return (*TYPE(fnptr_IterGetEnd))(Node, Out);
	}

	R_INLINE static PdContainer gds_IterGetHandle(PdIterator I)
	{
		typedef PdContainer (*TYPE)(PdIterator);
		if (!fnptr_IterGetHandle)
			fnptr_IterGetHandle = R_GetCCallable(gds_name, "gds_IterGetHandle");
		return (*TYPE(fnptr_IterGetHandle))(I);
	}

	R_INLINE static bool gds_IterAdv(PdIterator I)
	{
		typedef bool (*TYPE)(PdIterator);
		if (!fnptr_IterAdv)
			fnptr_IterAdv = R_GetCCallable(gds_name, "gds_IterAdv");
		return (*TYPE(fnptr_IterAdv))(I);
	}

	R_INLINE static bool gds_IterAdvEx(PdIterator I, const ssize_t offset)
	{
		typedef bool (*TYPE)(PdIterator, const ssize_t);
		if (!fnptr_IterAdvEx)
			fnptr_IterAdvEx = R_GetCCallable(gds_name, "gds_IterAdvEx");
		return (*TYPE(fnptr_IterAdvEx))(I, offset);
	}

	R_INLINE static bool gds_IterPrev(PdIterator I)
	{
		typedef bool (*TYPE)(PdIterator);
		if (!fnptr_IterPrev)
			fnptr_IterPrev = R_GetCCallable(gds_name, "gds_IterPrev");
		return (*TYPE(fnptr_IterPrev))(I);
	}

	R_INLINE static bool gds_IterPrevEx(PdIterator I, const ssize_t offset)
	{
		typedef bool (*TYPE)(PdIterator, const ssize_t);
		if (!fnptr_IterPrevEx)
			fnptr_IterPrevEx = R_GetCCallable(gds_name, "gds_IterPrevEx");
		return (*TYPE(fnptr_IterPrevEx))(I, offset);
	}

	R_INLINE static int gds_IterInt(PdIterator I)
	{
		typedef int (*TYPE)(PdIterator);
		if (!fnptr_IterInt)
			fnptr_IterInt = R_GetCCallable(gds_name, "gds_IterInt");
		return (*TYPE(fnptr_IterInt))(I);
	}

	R_INLINE static double gds_IterFloat(PdIterator I)
	{
		typedef double (*TYPE)(PdIterator);
		if (!fnptr_IterFloat)
			fnptr_IterFloat = R_GetCCallable(gds_name, "gds_IterFloat");
		return (*TYPE(fnptr_IterFloat))(I);
	}

	R_INLINE static int gds_IterStr(PdIterator I, char *OutStr,
		size_t OutBufLen)
	{
		typedef int (*TYPE)(PdIterator, char *, size_t);
		if (!fnptr_IterStr)
			fnptr_IterStr = R_GetCCallable(gds_name, "gds_IterStr");
		return (*TYPE(fnptr_IterStr))(I, OutStr, OutBufLen);
	}

	R_INLINE static bool gds_IterIntTo(PdIterator I, int val)
	{
		typedef bool (*TYPE)(PdIterator, int);
		if (!fnptr_IterIntTo)
			fnptr_IterIntTo = R_GetCCallable(gds_name, "gds_IterIntTo");
		return (*TYPE(fnptr_IterIntTo))(I, val);
	}

	R_INLINE static bool gds_IterFloatTo(PdIterator I, double val)
	{
		typedef bool (*TYPE)(PdIterator, double);
		if (!fnptr_IterFloatTo)
			fnptr_IterFloatTo = R_GetCCallable(gds_name, "gds_IterFloatTo");
		return (*TYPE(fnptr_IterFloatTo))(I, val);
	}

	R_INLINE static bool gds_IterStrTo(PdIterator I, const char *Str)
	{
		typedef bool (*TYPE)(PdIterator, const char *);
		if (!fnptr_IterStrTo)
			fnptr_IterStrTo = R_GetCCallable(gds_name, "gds_IterStrTo");
		return (*TYPE(fnptr_IterStrTo))(I, Str);
	}

	R_INLINE static size_t gds_IterRData(PdIterator I, void *OutBuf,
		size_t Cnt, TSVType OutSV)
	{
		typedef size_t (*TYPE)(PdIterator, void *, size_t, TSVType);
		if (!fnptr_IterRData)
			fnptr_IterRData = R_GetCCallable(gds_name, "gds_IterRData");
		return (*TYPE(fnptr_IterRData))(I, OutBuf, Cnt, OutSV);
	}

	R_INLINE static size_t gds_IterWData(PdIterator I, const void *InBuf,
		size_t Cnt, TSVType InSV)
	{
		typedef size_t (*TYPE)(PdIterator, const void *, size_t, TSVType);
		if (!fnptr_IterWData)
			fnptr_IterWData = R_GetCCallable(gds_name, "gds_IterWData");
		return (*TYPE(fnptr_IterWData))(I, InBuf, Cnt, InSV);
	}


	// Functions for CdSequenceX

	R_INLINE static int gds_SeqDimCnt(PdSequenceX Obj)
	{
		typedef int (*TYPE)(PdSequenceX);
		if (!fnptr_SeqDimCnt)
			fnptr_SeqDimCnt = R_GetCCallable(gds_name, "gds_SeqDimCnt");
		return (*TYPE(fnptr_SeqDimCnt))(Obj);
	}

	R_INLINE static bool gds_SeqGetDim(PdSequenceX Obj, int *OutBuf)
	{
		typedef bool (*TYPE)(PdSequenceX, int *);
		if (!fnptr_SeqGetDim)
			fnptr_SeqGetDim = R_GetCCallable(gds_name, "gds_SeqGetDim");
		return (*TYPE(fnptr_SeqGetDim))(Obj, OutBuf);
	}

	R_INLINE static Int64 gds_SeqGetCount(PdSequenceX Obj)
	{
		typedef Int64 (*TYPE)(PdSequenceX);
		if (!fnptr_SeqGetCount)
			fnptr_SeqGetCount = R_GetCCallable(gds_name, "gds_SeqGetCount");
		return (*TYPE(fnptr_SeqGetCount))(Obj);
	}

	R_INLINE static int gds_SeqSVType(PdSequenceX Obj)
	{
		typedef int (*TYPE)(PdSequenceX);
		if (!fnptr_SeqSVType)
			fnptr_SeqSVType = R_GetCCallable(gds_name, "gds_SeqSVType");
		return (*TYPE(fnptr_SeqSVType))(Obj);
	}

	R_INLINE static int gds_SeqBitOf(PdSequenceX Obj)
	{
		typedef int (*TYPE)(PdSequenceX);
		if (!fnptr_SeqBitOf)
			fnptr_SeqBitOf = R_GetCCallable(gds_name, "gds_SeqBitOf");
		return (*TYPE(fnptr_SeqBitOf))(Obj);
	}

	R_INLINE static bool gds_SeqIndexIter(PdSequenceX Obj, int *Index,
		PdIterator Out)
	{
		typedef bool (*TYPE)(PdSequenceX, int *, PdIterator);
		if (!fnptr_SeqIndexIter)
			fnptr_SeqIndexIter = R_GetCCallable(gds_name, "gds_SeqIndexIter");
		return (*TYPE(fnptr_SeqIndexIter))(Obj, Index, Out);
	}

	R_INLINE static int gds_SeqFStrMaxLen(PdSequenceX Obj)
	{
		typedef int (*TYPE)(PdSequenceX);
		if (!fnptr_SeqFStrMaxLen)
			fnptr_SeqFStrMaxLen = R_GetCCallable(gds_name, "gds_SeqFStrMaxLen");
		return (*TYPE(fnptr_SeqFStrMaxLen))(Obj);
	}

	// CdSequenceX -- read
	R_INLINE static bool gds_rData(PdSequenceX Obj, CoreArray::Int32 const* Start,
		CoreArray::Int32 const* Length, void *OutBuf, TSVType OutSV)
	{
		typedef bool (*TYPE)(PdSequenceX, CoreArray::Int32 const *,
			CoreArray::Int32 const *, void *, TSVType);
		if (!fnptr_rData)
			fnptr_rData = R_GetCCallable(gds_name, "gds_rData");
		return (*TYPE(fnptr_rData))(Obj, Start, Length, OutBuf, OutSV);
	}

	R_INLINE static bool gds_rDataEx(PdSequenceX Obj, CoreArray::Int32 const* Start,
		CoreArray::Int32 const* Length, const CBOOL *const Selection[],
		void *OutBuf, TSVType OutSV)
	{
		typedef bool (*TYPE)(PdSequenceX, CoreArray::Int32 const *,
			CoreArray::Int32 const *, const CBOOL *const [], void *, TSVType);
		if (!fnptr_rDataEx)
			fnptr_rDataEx = R_GetCCallable(gds_name, "gds_rDataEx");
		return (*TYPE(fnptr_rDataEx))(Obj, Start, Length, Selection, OutBuf, OutSV);
	}

	// CdSequenceX -- write
	R_INLINE static bool gds_wData(PdSequenceX Obj, CoreArray::Int32 const* Start,
		CoreArray::Int32 const* Length, const void *InBuf, TSVType InSV)
	{
		typedef bool (*TYPE)(PdSequenceX, CoreArray::Int32 const *,
			CoreArray::Int32 const *, const void *, TSVType);
		if (!fnptr_wData)
			fnptr_wData = R_GetCCallable(gds_name, "gds_wData");
		return (*TYPE(fnptr_wData))(Obj, Start, Length, InBuf, InSV);
	}

	// CdSequenceX -- append
	R_INLINE static bool gds_AppendData(PdSequenceX Obj, int Cnt,
		const void *InBuf, TSVType InSV)
	{
		typedef bool (*TYPE)(PdSequenceX, int, const void *, TSVType);
		if (!fnptr_AppendData)
			fnptr_AppendData = R_GetCCallable(gds_name, "gds_AppendData");
		return (*TYPE(fnptr_AppendData))(Obj, Cnt, InBuf, InSV);
	}

	R_INLINE static bool gds_AppendString(PdSequenceX Obj, int Cnt,
		const char *buffer[])
	{
		typedef bool (*TYPE)(PdSequenceX, int, const char * []);
		if (!fnptr_AppendString)
			fnptr_AppendString = R_GetCCallable(gds_name, "gds_AppendString");
		return (*TYPE(fnptr_AppendString))(Obj, Cnt, buffer);
	}

	R_INLINE static bool gds_AppendString(PdSequenceX Obj, const char *val)
	{
		return gds_AppendString(Obj, 1, &val);
	}

	// CdSequenceX -- Assign
	R_INLINE static bool gds_Assign(PdSequenceX dest_obj, PdSequenceX src_obj,
		bool append)
	{
		typedef bool (*TYPE)(PdSequenceX, PdSequenceX, bool);
		if (!fnptr_Assign)
			fnptr_Assign = R_GetCCallable(gds_name, "gds_Assign");
		return (*TYPE(fnptr_Assign))(dest_obj, src_obj, append);
	}


	// Functions for CdGDSStreamContainer

	R_INLINE static PdGDSStreamContainer gds_NewContainer(PdGDSFolder Folder,
		const char *Name, const char *InputFile, const char *PackMode)
	{
		typedef PdGDSStreamContainer (*TYPE)(PdGDSFolder, const char *,
			const char *, const char *);
		if (!fnptr_NewContainer)
			fnptr_NewContainer = R_GetCCallable(gds_name, "gds_NewContainer");
		return (*TYPE(fnptr_NewContainer))(Folder, Name, InputFile, PackMode);
	}

	R_INLINE static bool gds_SaveContainer(PdGDSStreamContainer Container,
		const char *OutputFile)
	{
		typedef bool (*TYPE)(PdGDSStreamContainer, const char *);
		if (!fnptr_SaveContainer)
			fnptr_SaveContainer = R_GetCCallable(gds_name, "gds_SaveContainer");
		return (*TYPE(fnptr_SaveContainer))(Container, OutputFile);
	}

	R_INLINE static Int64 gds_ContainerGetSize(PdGDSStreamContainer Container)
	{
		typedef Int64 (*TYPE)(PdGDSStreamContainer);
		if (!fnptr_ContainerGetSize)
			fnptr_ContainerGetSize = R_GetCCallable(gds_name, "gds_ContainerGetSize");
		return (*TYPE(fnptr_ContainerGetSize))(Container);
	}


	// Diagnosis

	R_INLINE static int gds_DiagnBlockCount(PdGDSFile Handle)
	{
		typedef int (*TYPE)(PdGDSFile);
		if (!fnptr_DiagnBlockCount)
			fnptr_DiagnBlockCount = R_GetCCallable(gds_name, "gds_DiagnBlockCount");
		return (*TYPE(fnptr_DiagnBlockCount))(Handle);
	}

	R_INLINE static PdBlockStream_BlockInfo const gds_DiagnBlockInfo(
		PdGDSFile Handle, int Index)
	{
		typedef PdBlockStream_BlockInfo const (*TYPE)(PdGDSFile, int);
		if (!fnptr_DiagnBlockInfo)
			fnptr_DiagnBlockInfo = R_GetCCallable(gds_name, "gds_DiagnBlockInfo");
		return (*TYPE(fnptr_DiagnBlockInfo))(Handle, Index);
	}

	R_INLINE static bool gds_DiagnBlockSize(PdGDSFile Handle, int Index,
		Int64 &size, Int64 &capacity)
	{
		typedef bool (*TYPE)(PdGDSFile, int, Int64 &, Int64 &);
		if (!fnptr_DiagnBlockSize)
			fnptr_DiagnBlockSize = R_GetCCallable(gds_name, "gds_DiagnBlockSize");
		return (*TYPE(fnptr_DiagnBlockSize))(Handle, Index, size, capacity);
	}

	R_INLINE static PdBlockStream_BlockInfo const gds_DiagnUnusedBlock(
		PdGDSFile Handle)
	{
		typedef PdBlockStream_BlockInfo const (*TYPE)(PdGDSFile);
		if (!fnptr_DiagnUnusedBlock)
			fnptr_DiagnUnusedBlock = R_GetCCallable(gds_name, "gds_DiagnUnusedBlock");
		return (*TYPE(fnptr_DiagnUnusedBlock))(Handle);
	}



	// ******************************************************************
	// ****  the functions for machine configuration
	//

	// Return the number of available CPU cores in the system
	R_INLINE static int conf_GetNumberOfCPU()
	{
		typedef int (*TYPE)();
		if (!fnptr_GetNumberOfCPU)
			fnptr_GetNumberOfCPU = R_GetCCallable(gds_name, "conf_GetNumberOfCPU");
		return (*TYPE(fnptr_GetNumberOfCPU))();
	}

	/// Return the size in byte of level-1 cache memory
	R_INLINE static int conf_GetL1CacheMemory()
	{
		typedef int (*TYPE)();
		if (!fnptr_GetL1CacheMemory)
			fnptr_GetL1CacheMemory = R_GetCCallable(gds_name, "conf_GetL1CacheMemory");
		return (*TYPE(fnptr_GetL1CacheMemory))();
	}

	/// Return the size in byte of level-2 cache memory
    R_INLINE static int conf_GetL2CacheMemory()
	{
		typedef int (*TYPE)();
		if (!fnptr_GetL2CacheMemory)
			fnptr_GetL2CacheMemory = R_GetCCallable(gds_name, "conf_GetL2CacheMemory");
		return (*TYPE(fnptr_GetL2CacheMemory))();
	}



	// ******************************************************************
	// ****	 the functions for parellel computing
	//

	// thread mutex

	/// create a mutex object
	R_INLINE static PdThreadMutex plc_InitMutex()
	{
		typedef PdThreadMutex (*TYPE)();
		if (!fnptr_InitMutex)
			fnptr_InitMutex = R_GetCCallable(gds_name, "plc_InitMutex");
		return (*TYPE(fnptr_InitMutex))();
	}

	/// destroy the mutex object
	R_INLINE static bool plc_DoneMutex(PdThreadMutex obj)
	{
		typedef bool (*TYPE)(PdThreadMutex);
		if (!fnptr_DoneMutex)
			fnptr_DoneMutex = R_GetCCallable(gds_name, "plc_DoneMutex");
		return (*TYPE(fnptr_DoneMutex))(obj);
	}

	/// lock the mutex object
	R_INLINE static bool plc_LockMutex(PdThreadMutex obj)
	{
		typedef bool (*TYPE)(PdThreadMutex);
		if (!fnptr_LockMutex)
			fnptr_LockMutex = R_GetCCallable(gds_name, "plc_LockMutex");
		return (*TYPE(fnptr_LockMutex))(obj);
	}

	/// unlock the mutex object
	R_INLINE static bool plc_UnlockMutex(PdThreadMutex obj)
	{
		typedef bool (*TYPE)(PdThreadMutex);
		if (!fnptr_UnlockMutex)
			fnptr_UnlockMutex = R_GetCCallable(gds_name, "plc_UnlockMutex");
		return (*TYPE(fnptr_UnlockMutex))(obj);
	}

	// thread suspending object

	/// initialize a thread suspending object
	R_INLINE static PdThreadsSuspending plc_InitSuspend()
	{
		typedef PdThreadsSuspending (*TYPE)();
		if (!fnptr_InitSuspend)
			fnptr_InitSuspend = R_GetCCallable(gds_name, "plc_InitSuspend");
		return (*TYPE(fnptr_InitSuspend))();
	}

	/// destroy the thread suspending object
	R_INLINE static bool plc_DoneSuspend(PdThreadsSuspending obj)
	{
		typedef bool (*TYPE)(PdThreadsSuspending);
		if (!fnptr_DoneSuspend)
			fnptr_DoneSuspend = R_GetCCallable(gds_name, "plc_DoneSuspend");
		return (*TYPE(fnptr_DoneSuspend))(obj);
	}

	/// suspend the thread suspending object
	R_INLINE static bool plc_Suspend(PdThreadsSuspending obj)
	{
		typedef bool (*TYPE)(PdThreadsSuspending);
		if (!fnptr_Suspend)
			fnptr_Suspend = R_GetCCallable(gds_name, "plc_Suspend");
		return (*TYPE(fnptr_Suspend))(obj);
	}

	/// wakeup the thread suspending object
	R_INLINE static bool plc_WakeUp(PdThreadsSuspending obj)
	{
		typedef bool (*TYPE)(PdThreadsSuspending);
		if (!fnptr_WakeUp)
			fnptr_WakeUp = R_GetCCallable(gds_name, "plc_WakeUp");
		return (*TYPE(fnptr_WakeUp))(obj);
	}

	/// parallel computing
	R_INLINE static bool plc_DoBaseThread(void (*Proc)(PdThread, int, void*),
		void *param, int nThread)
	{
		typedef bool (*TYPE)(void (*)(PdThread, int, void*), void *, int);
		if (!fnptr_DoBaseThread)
			fnptr_DoBaseThread = R_GetCCallable(gds_name, "plc_DoBaseThread");
		return (*TYPE(fnptr_DoBaseThread))(Proc, param, nThread);
	}



	// ******************************************************************
	// ****	 the functions for block read
	//

	/// read an array-oriented object margin by margin
	R_INLINE static PdArrayRead gds_ArrayRead_Init(PdSequenceX Obj,
		int Margin, TSVType SVType, const CBOOL *const Selection[],
		bool buf_if_need=true)
	{
		typedef PdArrayRead (*TYPE)(PdSequenceX, int, TSVType,
			const CBOOL *const Selection[], bool);
		if (!fnptr_ArrayRead_Init)
			fnptr_ArrayRead_Init = R_GetCCallable(gds_name, "gds_ArrayRead_Init");
		return (*TYPE(fnptr_ArrayRead_Init))(Obj, Margin, SVType, Selection, buf_if_need);
	}

	/// free a 'CdArrayRead' object
	R_INLINE static bool gds_ArrayRead_Free(PdArrayRead Obj)
	{
		typedef bool (*TYPE)(PdArrayRead);
		if (!fnptr_ArrayRead_Free)
			fnptr_ArrayRead_Free = R_GetCCallable(gds_name, "gds_ArrayRead_Free");
		return (*TYPE(fnptr_ArrayRead_Free))(Obj);
	}

	/// read data
	R_INLINE static bool gds_ArrayRead_Read(PdArrayRead Obj, void *Buffer)
	{
		typedef bool (*TYPE)(PdArrayRead, void *);
		if (!fnptr_ArrayRead_Read)
			fnptr_ArrayRead_Read = R_GetCCallable(gds_name, "gds_ArrayRead_Read");
		return (*TYPE(fnptr_ArrayRead_Read))(Obj, Buffer);
	}

	/// return true, if it is of the end
	R_INLINE static bool gds_ArrayRead_Eof(PdArrayRead Obj)
	{
		typedef bool (*TYPE)(PdArrayRead);
		if (!fnptr_ArrayRead_Eof)
			fnptr_ArrayRead_Eof = R_GetCCallable(gds_name, "gds_ArrayRead_Eof");
		return (*TYPE(fnptr_ArrayRead_Eof))(Obj);
	}

	/// reallocate the buffer with specified size with respect to array
	R_INLINE static bool gds_Balance_ArrayRead_Buffer(PdArrayRead array[],
		int n, Int64 buffer_size=-1)
	{
		typedef bool (*TYPE)(PdArrayRead[], int, Int64);
		if (!fnptr_Balance_ArrayRead_Buffer)
			fnptr_Balance_ArrayRead_Buffer = R_GetCCallable(gds_name, "gds_Balance_ArrayRead_Buffer");
		return (*TYPE(fnptr_Balance_ArrayRead_Buffer))(array, n, buffer_size);
	}



	// ******************************************************************
	// ****  the functions for error messages
	//

	/// return true, if Obj is a logical object in R
	R_INLINE static bool gds_Is_R_Logical(PdGDSObj Obj)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_Is_R_Logical)
			fnptr_Is_R_Logical = R_GetCCallable(gds_name, "gds_Is_R_Logical");
		return (*TYPE(fnptr_Is_R_Logical))(Obj);
	}

	/// return true, if Obj is a factor variable
	R_INLINE static bool gds_Is_R_Factor(PdGDSObj Obj)
	{
		typedef bool (*TYPE)(PdGDSObj);
		if (!fnptr_Is_R_Factor)
			fnptr_Is_R_Factor = R_GetCCallable(gds_name, "gds_Is_R_Factor");
		return (*TYPE(fnptr_Is_R_Factor))(Obj);
	}

	/// return 1, if Obj is a factor object in R; otherwise return 0
	R_INLINE static int gds_Set_If_R_Factor(PdGDSObj Obj, SEXP val)
	{
		typedef int (*TYPE)(PdGDSObj, SEXP);
		if (!fnptr_Set_If_R_Factor)
			fnptr_Set_If_R_Factor = R_GetCCallable(gds_name, "gds_Set_If_R_Factor");
		return (*TYPE(fnptr_Set_If_R_Factor))(Obj, val);
	}

	/// read data and return a SEXP object
	R_INLINE static SEXP gds_Read_SEXP(PdSequenceX Obj,
		CoreArray::Int32 const* Start, CoreArray::Int32 const* Length,
		const CBOOL *const Selection[])
	{
		typedef SEXP (*TYPE)(PdSequenceX, CoreArray::Int32 const*,
			CoreArray::Int32 const*, const CBOOL *const []);
		if (!fnptr_Read_SEXP)
			fnptr_Read_SEXP = R_GetCCallable(gds_name, "gds_Read_SEXP");
		return (*TYPE(fnptr_Read_SEXP))(Obj, Start, Length, Selection);
	}

	/// get the last error message
	R_INLINE static const char *gds_Error()
	{
		typedef const char* (*TYPE)();
		if (!fnptr_Error)
			fnptr_Error = R_GetCCallable(gds_name, "gds_Error");
		return (*TYPE(fnptr_Error))();
	}

	/// get the last error message
	R_INLINE static std::string *gds_LastError()
	{
		typedef std::string* (*TYPE)();
		if (!fnptr_LastError)
			fnptr_LastError = R_GetCCallable(gds_name, "gds_LastError");
		return (*TYPE(fnptr_LastError))();
	}




	// ******************************************************************
	// ******************************************************************
	// ******************************************************************

	/// automatic mutex object
	struct TdAutoMutex
	{
		PdThreadMutex mutex;
		TdAutoMutex(PdThreadMutex m)
			{ mutex = m; if (m) plc_LockMutex(m); }
		~TdAutoMutex()
			{ if (mutex) plc_UnlockMutex(mutex); }

		inline void Reset(PdThreadMutex m)
		{
			if (m != mutex)
			{
				if (mutex) plc_UnlockMutex(mutex);
				mutex = m;
				if (m) plc_LockMutex(m);
			}
		}
	};


	/// the class of read array
	class CArrayRead
	{
	public:
		CArrayRead(PdSequenceX Obj, int Margin, TSVType SVType,
			const CBOOL *const Selection[], bool buf_if_need=true)
		{
			_Obj = gds_ArrayRead_Init(Obj, Margin, SVType, Selection, buf_if_need);
			if (!_Obj)
				throw ErrCoreArray("Error 'initialize CArrayRead'.");
		}
		~CArrayRead() { gds_ArrayRead_Free(_Obj); }

		/// read data
		bool Read(void *Buffer)
		{
			return gds_ArrayRead_Read(_Obj, Buffer);
		}

		/// return true, if it is of the end
		bool Eof()
		{
			return gds_ArrayRead_Eof(_Obj);
		}

	protected:
		PdArrayRead _Obj;
	};
}

#endif /* _R_GDS_H_ */
