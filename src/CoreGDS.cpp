// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// CoreGDS.cpp: Export the C interface of CoreArray library
//
// Copyright (C) 2007 - 2015	Xiuwen Zheng
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
 *	\file     CoreGDS.cpp
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2015
 *	\brief    Export the C interface of CoreArray library
 *	\details
**/


#include "CoreGDS.h"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <map>


namespace CoreArray
{
	// Initial Object

	class COREARRAY_DLL_DEFAULT TInit
	{
	public:
		std::vector<CdGDSFile*> Files;
		string LastError;

		~TInit()
		{
			std::vector<CdGDSFile*>::iterator i;
			for (i=Files.begin(); i != Files.end(); i++)
			{
				try {
					delete *i;
					*i = NULL;
				} catch (...) { }
			}
		}
	};

	static TInit Init;
}

using namespace std;
using namespace CoreArray;
using namespace CoreArray::Parallel;


extern "C"
{

/// the last error message
extern string R_CoreArray_Error_Msg;


#define CORETRY			try {
#define CORECATCH(x)	} \
	catch (exception &E) { \
		R_CoreArray_Error_Msg = E.what(); \
		return x; \
	} \
	catch (const char *E) { \
		R_CoreArray_Error_Msg = E; \
		return x; \
	}
#define CORECATCH2(cmd, x)	} \
	catch (exception &E) { \
		R_CoreArray_Error_Msg = E.what(); \
		cmd; \
		return x; \
	} \
	catch (const char *E) { \
		R_CoreArray_Error_Msg = E; \
		cmd; \
		return x; \
	}
#define COREFINALLY(x)	} \
	catch (...) { x; throw; } \
	x;


// Functions for CoreArray System

COREARRAY_DLL_EXPORT void gds_SysInit()
{
	RegisterClass();
}

COREARRAY_DLL_EXPORT int gds_SysVersion()
{
	return COREARRAY_VERSION;
}

COREARRAY_DLL_EXPORT const char *gds_SysVersionStr()
{
	return COREARRAY_VERSION_STR;
}

COREARRAY_DLL_EXPORT int gds_SysClassCount()
{
	CORETRY
		return dObjManager().ClassMap().size();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_SysClassStruct(int Index, const char **OutName,
	const char **OutDesp, int *OutClassType)
{
	CORETRY
		if ((Index < 0) || (Index >= (int)dObjManager().ClassMap().size()))
			throw ErrCoreArray("Invalid class index (%d).", Index);

		map<const char *, CdObjClassMgr::_ClassStruct, CdObjClassMgr::_strCmp>::
			const_iterator it = dObjManager().ClassMap().begin();
		for (int i=0; i < Index; i++)
			it++;
		*OutName = it->first;
		*OutDesp = it->second.Desp;
		*OutClassType = it->second.CType;
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_SysPipeCount()
{
	CORETRY
		return dStreamPipeMgr.RegList().size();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_SysPipeStruct(int Index, const char **OutCoder,
	const char **OutDesp)
{
	CORETRY
		if ((Index < 0) || (Index >= (int)dStreamPipeMgr.RegList().size()))
			throw ErrCoreArray("Invalid pipe index (%d).", Index);
		CdPipeMgrItem * const &it = dStreamPipeMgr.RegList().at(Index);
		*OutCoder = it->Coder();
		*OutDesp = it->Description();
		return true;
	CORECATCH(false);
}


// Functions for File

COREARRAY_DLL_EXPORT PdGDSFile gds_FileOpen(const char *FileName, bool ReadOnly)
{
	PdGDSFile rv = new CdGDSFile;
	CORETRY
		rv->LoadFile(FileName, ReadOnly);
		Init.Files.push_back(rv);
		return rv;
	CORECATCH2(delete rv, NULL);
}

COREARRAY_DLL_EXPORT PdGDSFile gds_FileCreate(const char *FileName)
{
	PdGDSFile rv = new CdGDSFile;
	CORETRY
		rv->SaveAsFile(FileName);
		Init.Files.push_back(rv);
		return rv;
	CORECATCH2(delete rv, NULL);
}

COREARRAY_DLL_EXPORT bool gds_FileClose(PdGDSFile Handle)
{
	CORETRY
	vector<CdGDSFile*>::iterator i;
		i = find(Init.Files.begin(), Init.Files.end(), Handle);
		if (i == Init.Files.end())
			throw ErrCoreArray("Invalid File Handle.");
		Init.Files.erase(i);
		delete Handle;
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_FileSync(PdGDSFile Handle)
{
	CORETRY
		Handle->SyncFile();
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_FileName(PdGDSFile Handle, char *OutStr,
	size_t OutBufLen)
{
	CORETRY
		string s = RawText(Handle->FileName());
		if (OutStr)
			strncpy(OutStr, s.c_str(), OutBufLen);
		return s.length();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_FileReadOnly(PdGDSFile Handle)
{
	return Handle->ReadOnly();
}

COREARRAY_DLL_EXPORT PdGDSObj gds_FilePath(PdGDSFile Handle,
	const char *Path)
{
	CORETRY
		if ((Path==NULL) || (strcmp(Path, "")==0) ||
				(strcmp(Path, "/")==0) || (strcmp(Path, "//")==0))
			return &Handle->Root();
		else
			return Handle->Root().Path(ASC16(Path));
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT C_Int64 gds_FileSize(PdGDSFile Handle)
{
	CORETRY
		return Handle->GetFileSize();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_FileTidyUp(PdGDSFile Handle)
{
	CORETRY
		Handle->TidyUp(false);
		return true;
	CORECATCH(false);
}


// Functions for File Structure

COREARRAY_DLL_EXPORT PdGDSFolder gds_NodeRoot(PdGDSFile Handle)
{
	CORETRY
		return &Handle->Root();
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT PdGDSFile gds_NodeGDSFile(PdGDSObj Node)
{
	CORETRY
	return Node->GDSFile();
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT int gds_NodeChildCount(PdGDSFolder Node)
{
	CORETRY
	return Node->NodeCount();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT PdGDSFolder gds_NodeChildFolder(PdGDSFolder Node, int Index)
{
	CORETRY
	return &Node->DirItem(Index);
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT PdGDSObj gds_NodeChild(PdGDSFolder Node, int Index)
{
	CORETRY
		return Node->ObjItem(Index);
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT PdGDSObj gds_NodePath(PdGDSFolder Node, const char *Path)
{
	CORETRY
		return Node->Path(ASC16(Path));
	CORECATCH(NULL);
}

/// remove a GDS node
COREARRAY_DLL_EXPORT bool gds_NodeDelete(PdGDSObj Node)
{
	CORETRY
		if (Node->Folder())
		{
			Node->Folder()->DeleteObj(Node);
		} else
			throw ErrCoreArray("Cann't delete the root.");
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_NodeMoveTo(PdGDSObj Source, PdGDSFolder Dest)
{
	CORETRY
	Source->MoveTo(*Dest);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_NodeCopyTo(PdGDSObj Source, PdGDSFolder Dest)
{
	CORETRY
		UTF16String n = Source->Name();
		if (Dest->ObjItemEx(n))
			throw ErrCoreArray("The target folder has a node with the same name!");
		PdGDSObj rv = Source->NewOne();
		Dest->AddObj(n, rv);
		if (dynamic_cast<CdGDSFolder*>(Source))
		{
			dynamic_cast<CdGDSFolder*>(rv)->AssignOneEx(
				*dynamic_cast<CdGDSFolder*>(Source));
		} else
			rv->AssignOne(*Source);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_NodeAddLabel(PdGDSFolder Folder,
	const char *Name)
{
	CORETRY
		Folder->AddObj(ASC16(Name), NULL);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT PdGDSFolder gds_NodeAddFolder(PdGDSFolder Folder,
	const char *Name)
{
	CORETRY
		return (CdGDSFolder*)(Folder->AddFolder(ASC16(Name)));
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT PdAbstractArray gds_NodeAddArray(PdGDSFolder Folder,
	const char *Name, const char *TypeName, int DimCnt)
{
	return NULL;
/*
	CORETRY
		CdObjClassMgr::TdOnObjCreate OnObj =
			dObjManager().NameToClass(TypeName);
		PdAbstractArray rv = NULL;
		if (OnObj)
		{
			CdObject *obj = OnObj();
			if (dynamic_cast<CdAbstractArray*>(obj))
			{
				rv = static_cast<CdAbstractArray*>(obj);
				for (int i=0; i < DimCnt; i++)
					rv->AddDim(0);
			} else {
				delete obj;
				Init.LastError = string("No type name ") + TypeName;
				return NULL;
			}
		} else {
			Init.LastError = string("No type name ") + TypeName;
			return NULL;
		}
		if (rv)
		{
			try {
				Folder->AddObj(ASC16(Name), rv);
			} catch (...) {
				delete rv;
				throw;
			}
		}
		return rv;
	CORECATCH(NULL);
*/
}


// Functions for File Node

COREARRAY_DLL_EXPORT bool gds_NodeIsFolder(PdGDSObj Node)
{
	return (dynamic_cast<CdGDSFolder*>(Node) != NULL);
}

COREARRAY_DLL_EXPORT bool gds_NodeIsNull(PdGDSObj Node)
{
	return (dynamic_cast<CdGDSLabel*>(Node) != NULL);
}

COREARRAY_DLL_EXPORT bool gds_NodeIsContainer(PdGDSObj Node)
{
	return (dynamic_cast<PdContainer>(Node) != NULL);
}

COREARRAY_DLL_EXPORT bool gds_NodeIsSequence(PdGDSObj Node)
{
	return (dynamic_cast<CdAbstractArray*>(Node) != NULL);
}

COREARRAY_DLL_EXPORT bool gds_NodeIsVector(PdGDSObj Node)
{
	return (dynamic_cast<CdAllocArray*>(Node) != NULL);
}

COREARRAY_DLL_EXPORT bool gds_NodeIsFStr(PdGDSObj Node)
{
	return false;
/*	return (dynamic_cast<CdFStr8*>(Node) != NULL) ||
		(dynamic_cast<CdFStr16*>(Node) != NULL) ||
		(dynamic_cast<CdFStr32*>(Node) != NULL);
*/
}

COREARRAY_DLL_EXPORT bool gds_NodeIsStreamContainer(PdGDSObj Node)
{
	return (dynamic_cast<CdGDSStreamContainer*>(Node) != NULL);
}

COREARRAY_DLL_EXPORT int gds_NodeName(PdGDSObj Node, char *OutStr,
	size_t OutBufLen, bool FullName)
{
	CORETRY
		string n;
		if (FullName)
			n = RawText(Node->FullName());
		else
			n = RawText(Node->Name());
		if (OutStr)
			strncpy(OutStr, n.c_str(), OutBufLen);
		return n.length();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_NodeSetName(PdGDSObj Node, char *NewName)
{
	CORETRY
	Node->SetName(ASC16(NewName));
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_NodeClassName(PdGDSObj Node, char *OutStr,
	size_t OutBufLen)
{
	CORETRY
		string n;
		n = Node->dName();
		if (OutStr)
			strncpy(OutStr, n.c_str(), OutBufLen);
		return n.length();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_NodeStreamInfo(PdGDSObj Node, C_Int64 *TotalIn,
	C_Int64 *TotalOut, const char **StreamDesp)
{
	CORETRY
/*		if (Node->PipeInfo())
		{
			*TotalIn = Node->PipeInfo()->StreamTotalIn();
			*TotalOut = Node->PipeInfo()->StreamTotalOut();
			*StreamDesp = Node->PipeInfo()->Coder();
		} else {
			*TotalIn = *TotalOut = -1;
			*StreamDesp = "";
		}
*/		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_SetPackedMode(PdGDSObj Node, const char *Mode)
{
	CORETRY
		if (dynamic_cast<PdContainer>(Node))
			static_cast<PdContainer>(Node)->SetPackedMode(Mode);
		else
			if (dynamic_cast<CdGDSStreamContainer*>(Node))
				static_cast<CdGDSStreamContainer*>(Node)->SetPackedMode(Mode);
			else
				throw ErrCoreArray("No 'SetPackedMode' function!");
		return true;
	CORECATCH(false);
}


// Functions for Specific Node

#ifndef COREARRAY_USING_R
COREARRAY_DLL_EXPORT TdCombine2View *gds_NodeCombineView(PdAbstractArray *List,
	bool *Trans, int ListCnt)
{
	CORETRY
		return new TdCombine2View(List, Trans, ListCnt);
	CORECATCH(NULL);
}
#endif // COREARRAY_USING_R

COREARRAY_DLL_EXPORT bool gds_NodeFree(PdGDSObj Node)
{
	CORETRY
		if (Node)
			Node->Release();
		return true;
	CORECATCH(false);
}


// Functions for File Node -- TdObjAttr

COREARRAY_DLL_EXPORT int gds_AttrCount(PdGDSObj Node)
{
	CORETRY
		return Node->Attribute().Count();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT int gds_AttrNameIndex(PdGDSObj Node, const char *Name)
{
	CORETRY
		return Node->Attribute().IndexName(ASC16(Name));
	CORECATCH(-2);
}

COREARRAY_DLL_EXPORT bool gds_AttrIxAdd(PdGDSObj Node, const char *Name,
	const char *Value)
{
	CORETRY
		CdAny &D = Node->Attribute().Add(UTF16Text(Name));
		D.Assign(UTF8Text(Value));
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_AttrIxName(PdGDSObj Node, int Index,
	char *OutStr, size_t OutBufLen)
{
	CORETRY
		string n = RawText(Node->Attribute().Names(Index));
		if (OutStr)
			strncpy(OutStr, n.c_str(), OutBufLen);
		return n.length();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_AttrIxSetName(PdGDSObj Node, int Index,
	const char *NewName)
{
	CORETRY
		Node->Attribute().SetName(Index, ASC16(NewName));
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_AttrIxStr(PdGDSObj Node, int Index, char *OutStr,
	size_t OutBufLen)
{
	CORETRY
		CdAny &D = Node->Attribute()[Index];
		string n = RawText(D.GetStr8());
		if (OutStr)
			strncpy(OutStr, n.c_str(), OutBufLen);
		return n.length();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_AttrIxToStr(PdGDSObj Node, int Index, const char *Str)
{
	CORETRY
		CdAny &D = Node->Attribute()[Index];
		if (RawText(D.GetStr8()) != Str)
		{
			D.Assign(UTF8Text(Str));
			Node->Attribute().Changed();
		}
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_AttrIxType(PdGDSObj Node, int Index)
{
	CORETRY
		CdAny &D = Node->Attribute()[Index];
		return D.Type();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT const char *gds_AttrIxTypeStr(PdGDSObj Node, int Index)
{
	CORETRY
		CdAny &D = Node->Attribute()[Index];
		return D.dvtNames(D.Type());
	CORECATCH(NULL);
}


// Functions for CdContainer - TdIterator

COREARRAY_DLL_EXPORT bool gds_IterGetStart(PdContainer Node, PdIterator Out)
{
	CORETRY
		*Out = Node->IterBegin();
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_IterGetEnd(PdContainer Node, PdIterator Out)
{
	CORETRY
		*Out = Node->IterEnd();
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT PdContainer gds_IterGetHandle(PdIterator I)
{
	return I->Handler;
}

COREARRAY_DLL_EXPORT bool gds_IterAdv(PdIterator I)
{
	CORETRY
		++ (*I);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_IterAdvEx(PdIterator I, const ssize_t offset)
{
	CORETRY
		(*I) += offset;
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_IterPrev(PdIterator I)
{
	CORETRY
		-- (*I);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_IterPrevEx(PdIterator I, const ssize_t offset)
{
	CORETRY
		(*I) -= offset;
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_IterInt(PdIterator I)
{
	CORETRY
		return I->GetInteger();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT double gds_IterFloat(PdIterator I)
{
	CORETRY
		return I->GetFloat();
	CORECATCH(NaN);
}

COREARRAY_DLL_EXPORT int gds_IterStr(PdIterator I, char *OutStr,
	size_t OutBufLen)
{
	CORETRY
		string s = RawText(I->GetString());
		if (OutStr)
			strncpy(OutStr, s.c_str(), OutBufLen);
		return s.length();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_IterIntTo(PdIterator I, int val)
{
	CORETRY
		I->SetInteger(val);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_IterFloatTo(PdIterator I, double val)
{
	CORETRY
		I->SetFloat(val);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_IterStrTo(PdIterator I, const char *Str)
{
	CORETRY
		I->SetString(UTF16Text(Str));
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT size_t gds_IterRData(PdIterator I, void *OutBuf, size_t Cnt, C_SVType OutSV)
{
	CORETRY
		I->ReadData(OutBuf, Cnt, OutSV);
		return Cnt;
	CORECATCH((size_t)-1);
}

COREARRAY_DLL_EXPORT size_t gds_IterWData(PdIterator I, const void *InBuf, size_t Cnt, C_SVType InSV)
{
	CORETRY
		I->WriteData(InBuf, Cnt, InSV);
		return Cnt;
	CORECATCH((size_t)-1);
}



// Functions for CdAbstractArray

COREARRAY_DLL_EXPORT int gds_SeqDimCnt(PdAbstractArray Obj)
{
	CORETRY
		return Obj->DimCnt();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_SeqGetDim(PdAbstractArray Obj, int *OutBuf)
{
	CORETRY
		CdAbstractArray::TArrayDim buf;
		Obj->GetDim(buf);
		int cnt = Obj->DimCnt();
		C_Int32 *s = buf;
		for (; cnt > 0; cnt--) *OutBuf++ = *s++;
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT C_Int64 gds_SeqGetCount(PdAbstractArray Obj)
{
	CORETRY
		return Obj->TotalCount();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT int gds_SeqSVType(PdAbstractArray Obj)
{
	CORETRY
		return Obj->SVType();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT int gds_SeqBitOf(PdAbstractArray Obj)
{
	CORETRY
		return Obj->BitOf();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT bool gds_SeqIndexIter(PdAbstractArray Obj, int *Index,
	PdIterator Out)
{
	CORETRY
		CdAbstractArray::TArrayDim buf;
		C_Int32 *p = buf;
		for (int cnt=Obj->DimCnt(); cnt > 0; cnt--)
			*p++ = *Index++;
		*Out = Obj->Iterator(buf);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT int gds_SeqFStrMaxLen(PdAbstractArray Obj)
{
	return 0;
/*
	CORETRY
		if (dynamic_cast<CdFStr8*>(Obj))
			return static_cast<CdFStr8*>(Obj)->MaxLength();
		else if (dynamic_cast<CdFStr16*>(Obj))
			return static_cast<CdFStr16*>(Obj)->MaxLength();
		else if (dynamic_cast<CdFStr32*>(Obj))
			return static_cast<CdFStr32*>(Obj)->MaxLength();
		else {
			Init.LastError = "Unknown Type of String Array!";
			return -1;
		}
	CORECATCH(-1);
*/
}

// CdAbstractArray -- read

COREARRAY_DLL_EXPORT bool gds_rData(PdAbstractArray Obj, C_Int32 const* Start,
	C_Int32 const* Length, void *OutBuf, C_SVType OutSV)
{
	CORETRY
		Obj->ReadData(Start, Length, OutBuf, OutSV);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_rDataEx(PdAbstractArray Obj, C_Int32 const* Start,
	C_Int32 const* Length, const C_BOOL *const Selection[], void *OutBuf,
	C_SVType OutSV)
{
	CORETRY
		Obj->ReadDataEx(Start, Length, Selection, OutBuf, OutSV);
		return true;
	CORECATCH(false);
}

// CdAbstractArray -- write

COREARRAY_DLL_EXPORT bool gds_wData(PdAbstractArray Obj, C_Int32 const* Start,
	C_Int32 const* Length, const void *InBuf, C_SVType InSV)
{
	CORETRY
		Obj->WriteData(Start, Length, InBuf, InSV);
		return true;
	CORECATCH(false);
}


// CdAbstractArray -- append

COREARRAY_DLL_EXPORT bool gds_AppendData(PdAbstractArray Obj, int Cnt,
	const void *InBuf, C_SVType InSV)
{
	if (Cnt < 0) return false;
	CORETRY
		Obj->Append(InBuf, Cnt, InSV);
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT bool gds_AppendString(PdAbstractArray Obj, int Cnt,
	const char *buffer[])
{
	if (Cnt < 0) return false;
	CORETRY
		if (Cnt > 1)
		{
			UTF8String *Buf = new UTF8String[Cnt];
			UTF8String *d = Buf;
			const char **s = buffer;
			for (int i=0; i < Cnt; i++)
				*d++ = UTF8Text(*s++);
			Obj->Append(Buf, Cnt, svStrUTF8);
			delete [] Buf;
		} if (Cnt == 1)
		{
			UTF8String s = UTF8Text(buffer[0]);
			Obj->Append(&s, 1, svStrUTF8);
		}
		return true;
	CORECATCH(false);
}


// CdAbstractArray -- Assign

COREARRAY_DLL_EXPORT bool gds_Assign(PdAbstractArray dest_obj,
	PdAbstractArray src_obj, bool append)
{
	CORETRY
		dest_obj->AssignOneEx(*src_obj, append);
		return true;
	CORECATCH(false);
}



// Functions for CdGDSStreamContainer

COREARRAY_DLL_EXPORT PdGDSStreamContainer gds_NewContainer(PdGDSFolder Folder,
	const char *Name, const char *InputFile, const char *PackMode)
{
	CORETRY
		TdAutoRef<CdBufStream> file(new CdBufStream(
			new CdFileStream(InputFile, CdFileStream::fmOpenRead)));
		PdGDSStreamContainer rv = new CdGDSStreamContainer();
		rv->SetPackedMode(PackMode);
		Folder->AddObj(ASC16(Name), rv);
		rv->CopyFrom(*file.get());
		rv->CloseWriter();
		return rv;
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT bool gds_SaveContainer(PdGDSStreamContainer Container,
	const char *OutputFile)
{
	CORETRY
		TdAutoRef<CdBufStream> file(new CdBufStream(
			new CdFileStream(OutputFile, CdFileStream::fmCreate)));
		Container->CopyTo(*file.get());
		return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT C_Int64 gds_ContainerGetSize(PdGDSStreamContainer Container)
{
	CORETRY
		return Container->GetSize();
	CORECATCH(-1);
}


// Diagnosis

COREARRAY_DLL_EXPORT int gds_DiagnBlockCount(PdGDSFile Handle)
{
	CORETRY
		CdBlockCollection *H = (CdBlockCollection*)Handle;
		return H->BlockList().size();
	CORECATCH(-1);
}

COREARRAY_DLL_EXPORT PdBlockStream_BlockInfo const gds_DiagnBlockInfo(
	PdGDSFile Handle, int Index)
{
	CORETRY
		CdBlockCollection *H = (CdBlockCollection*)Handle;
		return (PdBlockStream_BlockInfo)(H->BlockList()[Index]->List());
	CORECATCH(NULL);
}

COREARRAY_DLL_EXPORT bool gds_DiagnBlockSize(PdGDSFile Handle, int Index,
	C_Int64 &size, C_Int64 &capacity)
{
	CORETRY
	CdBlockCollection *H = (CdBlockCollection*)Handle;
		size = H->BlockList()[Index]->Size();
		capacity = H->BlockList()[Index]->Capacity();
	return true;
	CORECATCH(false);
}

COREARRAY_DLL_EXPORT PdBlockStream_BlockInfo const gds_DiagnUnusedBlock(
	PdGDSFile Handle)
{
	CORETRY
		CdBlockCollection *H = (CdBlockCollection*)Handle;
		return H->UnusedBlock();
	CORECATCH(NULL);
}


// Functions for Error
COREARRAY_DLL_EXPORT const char *gds_Error()
{
	return Init.LastError.c_str();
}

COREARRAY_DLL_EXPORT string *gds_LastError()
{
	return &Init.LastError;
}



// ******************************************************************
// ****  the functions for machine configuration
//

/// Return NaN
COREARRAY_DLL_EXPORT float conf_F32_NaN()
{
	return NaN;
}
/// Return NaN
COREARRAY_DLL_EXPORT double conf_F64_NaN()
{
	return NaN;
}

/// Return infinity
COREARRAY_DLL_EXPORT float conf_F32_Inf()
{
	return Infinity;
}
/// Return infinity
COREARRAY_DLL_EXPORT double conf_F64_Inf()
{
	return Infinity;
}

/// Return negative infinity
COREARRAY_DLL_EXPORT float conf_F32_NegInf()
{
	return NegInfinity;
}
/// Return negative infinity
COREARRAY_DLL_EXPORT double conf_F64_NegInf()
{
	return NegInfinity;
}

/// Return whether it is a finite number
COREARRAY_DLL_EXPORT bool conf_IsFinite32(float val)
{
	return IsFinite(val);
}
/// Return whether it is a finite number
COREARRAY_DLL_EXPORT bool conf_IsFinite64(double val)
{
	return IsFinite(val);
}

/// Return the number of available CPU cores in the system
/** return -1, if unable to determine. **/
COREARRAY_DLL_EXPORT int conf_GetNumberOfCPU()
{
	return Mach::GetCPU_NumOfCores();
}

/// Return the size in byte of level-1 cache memory
/** return -1, if unable to determine. **/
COREARRAY_DLL_EXPORT int conf_GetL1CacheMemory()
{
	return Mach::GetCPU_LevelCache(1);
}

/// Return the size in byte of level-2 cache memory
/** return -1, if unable to determine. **/
COREARRAY_DLL_EXPORT int conf_GetL2CacheMemory()
{
	return Mach::GetCPU_LevelCache(2);
}



// ******************************************************************
// ****	 the functions for parellel computing
//

// thread mutex

/// create a mutex object
COREARRAY_DLL_EXPORT PdThreadMutex plc_InitMutex()
{
	CORETRY
		return new CdThreadMutex;
	CORECATCH(NULL);
}
/// destroy the mutex object
COREARRAY_DLL_EXPORT bool plc_DoneMutex(PdThreadMutex obj)
{
	CORETRY
		if (obj) delete obj;
		return true;
	CORECATCH(false);
}
/// lock the mutex object
COREARRAY_DLL_EXPORT bool plc_LockMutex(PdThreadMutex obj)
{
	CORETRY
		if (obj) obj->Lock();
		return true;
	CORECATCH(false);
}
/// unlock the mutex object
COREARRAY_DLL_EXPORT bool plc_UnlockMutex(PdThreadMutex obj)
{
	CORETRY
		if (obj) obj->Unlock();
		return true;
	CORECATCH(false);
}

// thread suspending object

/// initialize a thread suspending object
COREARRAY_DLL_EXPORT PdThreadsSuspending plc_InitSuspend()
{
	CORETRY
		return new CdThreadsSuspending;
	CORECATCH(NULL);
}
/// destroy the thread suspending object
COREARRAY_DLL_EXPORT bool plc_DoneSuspend(PdThreadsSuspending obj)
{
	CORETRY
		if (obj) delete obj;
		return true;
	CORECATCH(false);
}
/// suspend the thread suspending object
COREARRAY_DLL_EXPORT bool plc_Suspend(PdThreadsSuspending obj)
{
	CORETRY
		if (obj) obj->Suspend();
		return true;
	CORECATCH(false);
}
/// wakeup the thread suspending object
COREARRAY_DLL_EXPORT bool plc_WakeUp(PdThreadsSuspending obj)
{
	CORETRY
		if (obj) obj->WakeUp();
		return true;
	CORECATCH(false);
}


static CParallelBase _ParallelBase;

/// parallel computing
COREARRAY_DLL_EXPORT bool plc_DoBaseThread(void (*Proc)(PdThread, int, void*),
	void *param, int nThread)
{
	CORETRY
		_ParallelBase.SetNumThread(nThread);
		_ParallelBase.RunThreads((CParallelBase::TProc)Proc, param);
		return true;
	CORECATCH(false);
}


// ******************************************************************
// ****	 the functions for block read
//

/// read an array-oriented object margin by margin
COREARRAY_DLL_EXPORT PdArrayRead gds_ArrayRead_Init(PdAbstractArray Obj,
	int Margin, C_SVType SVType, const C_BOOL *const Selection[],
	bool buf_if_need)
{
	CORETRY
		PdArrayRead rv = new CdArrayRead;
		rv->Init(*Obj, Margin, SVType, Selection, buf_if_need);
		return rv;
	CORECATCH(NULL);
}

/// free a 'CdArrayRead' object
COREARRAY_DLL_EXPORT bool gds_ArrayRead_Free(PdArrayRead Obj)
{
	CORETRY
		if (Obj) delete Obj;
		return true;
	CORECATCH(false);
}

/// read data
COREARRAY_DLL_EXPORT bool gds_ArrayRead_Read(PdArrayRead Obj, void *Buffer)
{
	CORETRY
		Obj->Read(Buffer);
		return true;
	CORECATCH(false);
}

/// return true, if it is of the end
COREARRAY_DLL_EXPORT bool gds_ArrayRead_Eof(PdArrayRead Obj)
{
	CORETRY
		return Obj->Eof();
	CORECATCH(true);
}

/// reallocate the buffer with specified size with respect to array
COREARRAY_DLL_EXPORT bool gds_Balance_ArrayRead_Buffer(PdArrayRead array[],
	int n, C_Int64 buffer_size)
{
	CORETRY
		Balance_ArrayRead_Buffer(array, n, buffer_size);
		return true;
	CORECATCH(false);
}

} // extern "C"
