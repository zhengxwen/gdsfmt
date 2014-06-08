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
 *	\file     CoreGDS.cpp
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Export the C interface of CoreArray library
 *	\details
**/


#include <CoreArray.h>
#include <CoreGDS.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#if !defined(COREARRAY_WINDOWS)
#   undef COREARRAY_DLLEXPORT
#   define COREARRAY_DLLEXPORT
#endif


namespace CoreArray
{
	// Initial Object

	class TInit
	{
	public:
		std::vector<CdGDSFile*> Files;
		std::string LastError;

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

#define CORETRY			try {
#define CORECATCH(x)	} \
	catch (exception &E) { \
		Init.LastError = E.what(); \
		return x; \
	} \
	catch (const char *E) { \
		Init.LastError = E; \
		return x; \
	}
#define CORECATCH2(cmd, x)	} \
	catch (exception &E) { \
		Init.LastError = E.what(); \
		cmd; \
		return x; \
	} \
	catch (const char *E) { \
		Init.LastError = E; \
		cmd; \
		return x; \
	}
#define COREFINALLY(x)	} \
	catch (...) { x; throw; } \
	x;


// Functions for CoreArray System

COREARRAY_DLLEXPORT void gds_SysInit()
{
	RegisterClass();
}

COREARRAY_DLLEXPORT int gds_SysVersion()
{
	return COREARRAY_VERSION;
}

COREARRAY_DLLEXPORT const char *gds_SysVersionStr()
{
	return COREARRAY_VERSION_STR;
}

COREARRAY_DLLEXPORT int gds_SysClassCount()
{
	CORETRY
		return dObjManager().ClassMap().size();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_SysClassStruct(int Index, const char **OutName,
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

COREARRAY_DLLEXPORT int gds_SysPipeCount()
{
	CORETRY
		return dStreamPipeMgr.RegList().size();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_SysPipeStruct(int Index, const char **OutCoder,
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

COREARRAY_DLLEXPORT PdGDSFile gds_FileOpen(const char *FileName, bool ReadOnly)
{
	PdGDSFile rv = new CdGDSFile;
	CORETRY
		rv->LoadFile(FileName, ReadOnly);
		Init.Files.push_back(rv);
		return rv;
	CORECATCH2(delete rv, NULL);
}

COREARRAY_DLLEXPORT PdGDSFile gds_FileCreate(const char *FileName)
{
	PdGDSFile rv = new CdGDSFile;
	CORETRY
		rv->SaveAsFile(FileName);
		Init.Files.push_back(rv);
		return rv;
	CORECATCH2(delete rv, NULL);
}

COREARRAY_DLLEXPORT bool gds_FileClose(PdGDSFile Handle)
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

COREARRAY_DLLEXPORT bool gds_FileSync(PdGDSFile Handle)
{
	CORETRY
		Handle->SyncFile();
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT int gds_FileName(PdGDSFile Handle, char *OutStr,
	size_t OutBufLen)
{
	CORETRY
		string s = UTF16toUTF8(Handle->FileName());
		if (OutStr)
			strncpy(OutStr, s.c_str(), OutBufLen);
		return s.length();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_FileReadOnly(PdGDSFile Handle)
{
	return Handle->ReadOnly();
}

COREARRAY_DLLEXPORT PdGDSObj gds_FilePath(PdGDSFile Handle,
	const char *Path)
{
	CORETRY
		if ((Path==NULL) || (strcmp(Path, "")==0) ||
				(strcmp(Path, "/")==0) || (strcmp(Path, "//")==0))
			return &Handle->Root();
		else
			return Handle->Root().Path(T(Path));
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT Int64 gds_FileSize(PdGDSFile Handle)
{
	CORETRY
		return Handle->GetFileSize();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_FileTidyUp(PdGDSFile Handle)
{
	CORETRY
		Handle->TidyUp(false);
		return true;
	CORECATCH(false);
}


// Functions for File Structure

COREARRAY_DLLEXPORT PdGDSFolder gds_NodeRoot(PdGDSFile Handle)
{
	CORETRY
		return &Handle->Root();
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT PdGDSFile gds_NodeGDSFile(PdGDSObj Node)
{
	CORETRY
	return Node->GDSFile();
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT int gds_NodeChildCount(PdGDSFolder Node)
{
	CORETRY
	return Node->NodeCount();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT PdGDSFolder gds_NodeChildFolder(PdGDSFolder Node, int Index)
{
	CORETRY
	return &Node->DirItem(Index);
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT PdGDSObj gds_NodeChild(PdGDSFolder Node, int Index)
{
	CORETRY
		return Node->ObjItem(Index);
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT PdGDSObj gds_NodePath(PdGDSFolder Node, const char *Path)
{
	CORETRY
		return Node->Path(T(Path));
	CORECATCH(NULL);
}

/// remove a GDS node
COREARRAY_DLLEXPORT bool gds_NodeDelete(PdGDSObj Node)
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

COREARRAY_DLLEXPORT bool gds_NodeMoveTo(PdGDSObj Source, PdGDSFolder Dest)
{
	CORETRY
	Source->MoveTo(*Dest);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_NodeCopyTo(PdGDSObj Source, PdGDSFolder Dest)
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

COREARRAY_DLLEXPORT bool gds_NodeAddLabel(PdGDSFolder Folder,
	const char *Name)
{
	CORETRY
		Folder->AddObj(T(Name), NULL);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT PdGDSFolder gds_NodeAddFolder(PdGDSFolder Folder,
	const char *Name)
{
	CORETRY
		return (CdGDSFolder*)(Folder->AddFolder(T(Name)));
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT PdSequenceX gds_NodeAddArray(PdGDSFolder Folder,
	const char *Name, const char *TypeName, int DimCnt)
{
	CORETRY
		CdObjClassMgr::TdOnObjCreate OnObj =
			dObjManager().NameToClass(TypeName);
		PdSequenceX rv = NULL;
		if (OnObj)
		{
			CdObject *obj = OnObj();
			if (dynamic_cast<CdSequenceX*>(obj))
			{
				rv = static_cast<CdSequenceX*>(obj);
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
				Folder->AddObj(T(Name), rv);
			} catch (...) {
				delete rv;
				throw;
			}
		}
		return rv;
	CORECATCH(NULL);
}


// Functions for File Node

COREARRAY_DLLEXPORT bool gds_NodeIsFolder(PdGDSObj Node)
{
	return (dynamic_cast<CdGDSFolder*>(Node) != NULL);
}

COREARRAY_DLLEXPORT bool gds_NodeIsNull(PdGDSObj Node)
{
	return (dynamic_cast<CdGDSLabel*>(Node) != NULL);
}

COREARRAY_DLLEXPORT bool gds_NodeIsContainer(PdGDSObj Node)
{
	return (dynamic_cast<PdContainer>(Node) != NULL);
}

COREARRAY_DLLEXPORT bool gds_NodeIsSequence(PdGDSObj Node)
{
	return (dynamic_cast<CdSequenceX*>(Node) != NULL);
}

COREARRAY_DLLEXPORT bool gds_NodeIsVector(PdGDSObj Node)
{
	return (dynamic_cast<CdVectorX*>(Node) != NULL);
}

COREARRAY_DLLEXPORT bool gds_NodeIsFStr(PdGDSObj Node)
{
	return (dynamic_cast<CdFStr8*>(Node) != NULL) ||
		(dynamic_cast<CdFStr16*>(Node) != NULL) ||
		(dynamic_cast<CdFStr32*>(Node) != NULL);
}

COREARRAY_DLLEXPORT bool gds_NodeIsStreamContainer(PdGDSObj Node)
{
	return (dynamic_cast<CdGDSStreamContainer*>(Node) != NULL);
}

COREARRAY_DLLEXPORT int gds_NodeName(PdGDSObj Node, char *OutStr,
	size_t OutBufLen, bool FullName)
{
	CORETRY
		string n;
		if (FullName)
			n = UTF16toUTF8(Node->FullName());
		else
			n = UTF16toUTF8(Node->Name());
		if (OutStr)
			strncpy(OutStr, n.c_str(), OutBufLen);
		return n.length();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_NodeSetName(PdGDSObj Node, char *NewName)
{
	CORETRY
	Node->SetName(T(NewName));
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT int gds_NodeClassName(PdGDSObj Node, char *OutStr,
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

COREARRAY_DLLEXPORT bool gds_NodeStreamInfo(PdGDSObj Node, Int64 *TotalIn,
	Int64 *TotalOut, const char **StreamDesp)
{
	CORETRY
		if (Node->PipeInfo())
		{
			*TotalIn = Node->PipeInfo()->StreamTotalIn();
			*TotalOut = Node->PipeInfo()->StreamTotalOut();
			*StreamDesp = Node->PipeInfo()->Coder();
		} else {
			*TotalIn = *TotalOut = -1;
			*StreamDesp = "";
		}
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_SetPackedMode(PdGDSObj Node, const char *Mode)
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

#ifndef COREARRAY_R_LINK
COREARRAY_DLLEXPORT TdCombine2View *gds_NodeCombineView(PdSequenceX *List,
	bool *Trans, int ListCnt)
{
	CORETRY
		return new TdCombine2View(List, Trans, ListCnt);
	CORECATCH(NULL);
}
#endif // COREARRAY_R_LINK

COREARRAY_DLLEXPORT bool gds_NodeFree(PdGDSObj Node)
{
	CORETRY
		if (Node)
			Node->Release();
		return true;
	CORECATCH(false);
}


// Functions for File Node -- TdObjAttr

COREARRAY_DLLEXPORT int gds_AttrCount(PdGDSObj Node)
{
	CORETRY
		return Node->Attribute().Count();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT int gds_AttrNameIndex(PdGDSObj Node, const char *Name)
{
	CORETRY
		return Node->Attribute().IndexName(T(Name));
	CORECATCH(-2);
}

COREARRAY_DLLEXPORT bool gds_AttrIxAdd(PdGDSObj Node, const char *Name,
	const char *Value)
{
	CORETRY
		TdsAny &D = Node->Attribute().Add(T(Name));
		D.Assign(Value);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT int gds_AttrIxName(PdGDSObj Node, int Index,
	char *OutStr, size_t OutBufLen)
{
	CORETRY
		string n = UTF16toUTF8(Node->Attribute().Names(Index));
		if (OutStr)
			strncpy(OutStr, n.c_str(), OutBufLen);
		return n.length();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_AttrIxSetName(PdGDSObj Node, int Index,
	const char *NewName)
{
	CORETRY
		Node->Attribute().SetName(Index, T(NewName));
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT int gds_AttrIxStr(PdGDSObj Node, int Index, char *OutStr,
	size_t OutBufLen)
{
	CORETRY
		TdsAny &D = Node->Attribute()[Index];
		string n = D.GetStr8();
		if (OutStr)
			strncpy(OutStr, n.c_str(), OutBufLen);
		return n.length();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_AttrIxToStr(PdGDSObj Node, int Index, const char *Str)
{
	CORETRY
		TdsAny &D = Node->Attribute()[Index];
		if (D.GetStr8() != Str)
		{
			D.Assign(Str);
			Node->Attribute().Changed();
		}
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT int gds_AttrIxType(PdGDSObj Node, int Index)
{
	CORETRY
		TdsAny &D = Node->Attribute()[Index];
		return D.Type();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT const char *gds_AttrIxTypeStr(PdGDSObj Node, int Index)
{
	CORETRY
		TdsAny &D = Node->Attribute()[Index];
		return D.dvtNames(D.Type());
	CORECATCH(NULL);
}


// Functions for CdContainer - TdIterator

COREARRAY_DLLEXPORT bool gds_IterGetStart(PdContainer Node, PdIterator Out)
{
	CORETRY
		*Out = Node->atStart();
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_IterGetEnd(PdContainer Node, PdIterator Out)
{
	CORETRY
		*Out = Node->atEnd();
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT PdContainer gds_IterGetHandle(PdIterator I)
{
	return I->Handler;
}

COREARRAY_DLLEXPORT bool gds_IterAdv(PdIterator I)
{
	CORETRY
		++ (*I);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_IterAdvEx(PdIterator I, const ssize_t offset)
{
	CORETRY
		(*I) += offset;
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_IterPrev(PdIterator I)
{
	CORETRY
		-- (*I);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_IterPrevEx(PdIterator I, const ssize_t offset)
{
	CORETRY
		(*I) -= offset;
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT int gds_IterInt(PdIterator I)
{
	CORETRY
		return I->toInt();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT double gds_IterFloat(PdIterator I)
{
	CORETRY
		return I->toFloat();
	CORECATCH(NaN);
}

COREARRAY_DLLEXPORT int gds_IterStr(PdIterator I, char *OutStr,
	size_t OutBufLen)
{
	CORETRY
		UTF8String s = UTF16toUTF8(I->toStr());
		if (OutStr)
			strncpy(OutStr, s.c_str(), OutBufLen);
		return s.length();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_IterIntTo(PdIterator I, int val)
{
	CORETRY
		I->IntTo(val);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_IterFloatTo(PdIterator I, double val)
{
	CORETRY
		I->FloatTo(val);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_IterStrTo(PdIterator I, const char *Str)
{
	CORETRY
		I->StrTo(PChartoUTF16(Str));
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT size_t gds_IterRData(PdIterator I, void *OutBuf, size_t Cnt, TSVType OutSV)
{
	CORETRY
		return I->rData(OutBuf, Cnt, OutSV);
	CORECATCH((size_t)-1);
}

COREARRAY_DLLEXPORT size_t gds_IterWData(PdIterator I, const void *InBuf, size_t Cnt, TSVType InSV)
{
	CORETRY
		return I->wData(InBuf, Cnt, InSV);
	CORECATCH((size_t)-1);
}



// Functions for CdSequenceX

COREARRAY_DLLEXPORT int gds_SeqDimCnt(PdSequenceX Obj)
{
	CORETRY
		return Obj->DimCnt();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_SeqGetDim(PdSequenceX Obj, int *OutBuf)
{
	CORETRY
		CdSequenceX::TSeqDimBuf buf;
		Obj->GetDimLen(buf);
		int cnt = Obj->DimCnt();
		Int32 *s = buf;
		for (; cnt > 0; cnt--) *OutBuf++ = *s++;
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT Int64 gds_SeqGetCount(PdSequenceX Obj)
{
	CORETRY
		return Obj->Count();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT int gds_SeqSVType(PdSequenceX Obj)
{
	CORETRY
		return Obj->SVType();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT int gds_SeqBitOf(PdSequenceX Obj)
{
	CORETRY
		return Obj->BitOf();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT bool gds_SeqIndexIter(PdSequenceX Obj, int *Index,
	PdIterator Out)
{
	CORETRY
		CdSequenceX::TSeqDimBuf buf;
		Int32 *p = buf;
		for (int cnt=Obj->DimCnt(); cnt > 0; cnt--)
			*p++ = *Index++;
		*Out = Obj->Iterator(buf);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT int gds_SeqFStrMaxLen(PdSequenceX Obj)
{
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
}

// CdSequenceX -- read

COREARRAY_DLLEXPORT bool gds_rData(PdSequenceX Obj, Int32 const* Start,
	Int32 const* Length, void *OutBuf, TSVType OutSV)
{
	CORETRY
		Obj->rData(Start, Length, OutBuf, OutSV);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_rDataEx(PdSequenceX Obj, Int32 const* Start,
	Int32 const* Length, const CBOOL *const Selection[], void *OutBuf,
	TSVType OutSV)
{
	CORETRY
		Obj->rDataEx(Start, Length, Selection, OutBuf, OutSV);
		return true;
	CORECATCH(false);
}

// CdSequenceX -- write

COREARRAY_DLLEXPORT bool gds_wData(PdSequenceX Obj, Int32 const* Start,
	Int32 const* Length, const void *InBuf, TSVType InSV)
{
	CORETRY
		Obj->wData(Start, Length, InBuf, InSV);
		return true;
	CORECATCH(false);
}


// CdSequenceX -- append

COREARRAY_DLLEXPORT bool gds_AppendData(PdSequenceX Obj, int Cnt,
	const void *InBuf, TSVType InSV)
{
	if (Cnt < 0) return false;
	CORETRY
		Obj->Append(InBuf, Cnt, InSV);
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT bool gds_AppendString(PdSequenceX Obj, int Cnt,
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
				*d++ = *s++;
			Obj->Append(Buf, Cnt, svStrUTF8);
			delete [] Buf;
		} if (Cnt == 1)
		{
			UTF8String s = buffer[0];
			Obj->Append(&s, 1, svStrUTF8);
		}
		return true;
	CORECATCH(false);
}


// CdSequenceX -- Assign

COREARRAY_DLLEXPORT bool gds_Assign(PdSequenceX dest_obj,
	PdSequenceX src_obj, bool append)
{
	CORETRY
		dest_obj->AssignOneEx(*src_obj, append);
		return true;
	CORECATCH(false);
}



// Functions for CdGDSStreamContainer

COREARRAY_DLLEXPORT PdGDSStreamContainer gds_NewContainer(PdGDSFolder Folder,
	const char *Name, const char *InputFile, const char *PackMode)
{
	CORETRY
		TdAutoRef<CBufdStream> file(new CBufdStream(
			new CdFileStream(InputFile, CdFileStream::fmOpenRead)));
		PdGDSStreamContainer rv = new CdGDSStreamContainer();
		rv->SetPackedMode(PackMode);
		Folder->AddObj(T(Name), rv);
		rv->CopyFrom(*file.get());
		rv->CloseWriter();
		return rv;
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT bool gds_SaveContainer(PdGDSStreamContainer Container,
	const char *OutputFile)
{
	CORETRY
		TdAutoRef<CBufdStream> file(new CBufdStream(
			new CdFileStream(OutputFile, CdFileStream::fmCreate)));
		Container->CopyTo(*file.get());
		return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT Int64 gds_ContainerGetSize(PdGDSStreamContainer Container)
{
	CORETRY
		return Container->GetSize();
	CORECATCH(-1);
}


// Diagnosis

COREARRAY_DLLEXPORT int gds_DiagnBlockCount(PdGDSFile Handle)
{
	CORETRY
		CdBlockCollection *H = (CdBlockCollection*)Handle;
		return H->BlockList().size();
	CORECATCH(-1);
}

COREARRAY_DLLEXPORT PdBlockStream_BlockInfo const gds_DiagnBlockInfo(
	PdGDSFile Handle, int Index)
{
	CORETRY
		CdBlockCollection *H = (CdBlockCollection*)Handle;
		return (PdBlockStream_BlockInfo)(H->BlockList()[Index]->List());
	CORECATCH(NULL);
}

COREARRAY_DLLEXPORT bool gds_DiagnBlockSize(PdGDSFile Handle, int Index,
	Int64 &size, Int64 &capacity)
{
	CORETRY
	CdBlockCollection *H = (CdBlockCollection*)Handle;
		size = H->BlockList()[Index]->Size();
		capacity = H->BlockList()[Index]->Capacity();
	return true;
	CORECATCH(false);
}

COREARRAY_DLLEXPORT PdBlockStream_BlockInfo const gds_DiagnUnusedBlock(
	PdGDSFile Handle)
{
	CORETRY
		CdBlockCollection *H = (CdBlockCollection*)Handle;
		return H->UnusedBlock();
	CORECATCH(NULL);
}


// Functions for Error
COREARRAY_DLLEXPORT const char *gds_Error()
{
	return Init.LastError.c_str();
}

COREARRAY_DLLEXPORT string *gds_LastError()
{
	return &Init.LastError;
}



// ******************************************************************
// ****  the functions for machine configuration
//

/// Return NaN
COREARRAY_DLLEXPORT float conf_F32_NaN()
{
	return NaN;
}
/// Return NaN
COREARRAY_DLLEXPORT double conf_F64_NaN()
{
	return NaN;
}

/// Return infinity
COREARRAY_DLLEXPORT float conf_F32_Inf()
{
	return Infinity;
}
/// Return infinity
COREARRAY_DLLEXPORT double conf_F64_Inf()
{
	return Infinity;
}

/// Return negative infinity
COREARRAY_DLLEXPORT float conf_F32_NegInf()
{
	return NegInfinity;
}
/// Return negative infinity
COREARRAY_DLLEXPORT double conf_F64_NegInf()
{
	return NegInfinity;
}

/// Return whether it is a finite number
COREARRAY_DLLEXPORT bool conf_IsFinite32(float val)
{
	return IsFinite(val);
}
/// Return whether it is a finite number
COREARRAY_DLLEXPORT bool conf_IsFinite64(double val)
{
	return IsFinite(val);
}

/// Return the number of available CPU cores in the system
/** return -1, if unable to determine. **/
COREARRAY_DLLEXPORT int conf_GetNumberOfCPU()
{
	return Mach::GetNumberOfCPU();
}

/// Return the size in byte of level-1 cache memory
/** return -1, if unable to determine. **/
COREARRAY_DLLEXPORT int conf_GetL1CacheMemory()
{
	return Mach::GetL1CacheMemory();
}

/// Return the size in byte of level-2 cache memory
/** return -1, if unable to determine. **/
COREARRAY_DLLEXPORT int conf_GetL2CacheMemory()
{
	return Mach::GetL2CacheMemory();
}



// ******************************************************************
// ****	 the functions for parellel computing
//

// thread mutex

/// create a mutex object
COREARRAY_DLLEXPORT PdThreadMutex plc_InitMutex()
{
	CORETRY
		return new CdThreadMutex;
	CORECATCH(NULL);
}
/// destroy the mutex object
COREARRAY_DLLEXPORT bool plc_DoneMutex(PdThreadMutex obj)
{
	CORETRY
		if (obj) delete obj;
		return true;
	CORECATCH(false);
}
/// lock the mutex object
COREARRAY_DLLEXPORT bool plc_LockMutex(PdThreadMutex obj)
{
	CORETRY
		if (obj) obj->Lock();
		return true;
	CORECATCH(false);
}
/// unlock the mutex object
COREARRAY_DLLEXPORT bool plc_UnlockMutex(PdThreadMutex obj)
{
	CORETRY
		if (obj) obj->Unlock();
		return true;
	CORECATCH(false);
}

// thread suspending object

/// initialize a thread suspending object
COREARRAY_DLLEXPORT PdThreadsSuspending plc_InitSuspend()
{
	CORETRY
		return new CdThreadsSuspending;
	CORECATCH(NULL);
}
/// destroy the thread suspending object
COREARRAY_DLLEXPORT bool plc_DoneSuspend(PdThreadsSuspending obj)
{
	CORETRY
		if (obj) delete obj;
		return true;
	CORECATCH(false);
}
/// suspend the thread suspending object
COREARRAY_DLLEXPORT bool plc_Suspend(PdThreadsSuspending obj)
{
	CORETRY
		if (obj) obj->Suspend();
		return true;
	CORECATCH(false);
}
/// wakeup the thread suspending object
COREARRAY_DLLEXPORT bool plc_WakeUp(PdThreadsSuspending obj)
{
	CORETRY
		if (obj) obj->WakeUp();
		return true;
	CORECATCH(false);
}


static CParallelBase _ParallelBase;

/// parallel computing
COREARRAY_DLLEXPORT bool plc_DoBaseThread(void (*Proc)(PdThread, int, void*),
	void *param, int nThread)
{
	CORETRY
		_ParallelBase.SetnThread(nThread);
		_ParallelBase.DoThreads((CParallelBase::TProc)Proc, param);
		return true;
	CORECATCH(false);
}


// ******************************************************************
// ****	 the functions for block read
//

/// read an array-oriented object margin by margin
COREARRAY_DLLEXPORT PdArrayRead gds_ArrayRead_Init(PdSequenceX Obj,
	int Margin, TSVType SVType, const CBOOL *const Selection[],
	bool buf_if_need)
{
	CORETRY
		PdArrayRead rv = new CdArrayRead;
		rv->Init(*Obj, Margin, SVType, Selection, buf_if_need);
		return rv;
	CORECATCH(NULL);
}

/// free a 'CdArrayRead' object
COREARRAY_DLLEXPORT bool gds_ArrayRead_Free(PdArrayRead Obj)
{
	CORETRY
		if (Obj) delete Obj;
		return true;
	CORECATCH(false);
}

/// read data
COREARRAY_DLLEXPORT bool gds_ArrayRead_Read(PdArrayRead Obj, void *Buffer)
{
	CORETRY
		Obj->Read(Buffer);
		return true;
	CORECATCH(false);
}

/// return true, if it is of the end
COREARRAY_DLLEXPORT bool gds_ArrayRead_Eof(PdArrayRead Obj)
{
	CORETRY
		return Obj->Eof();
	CORECATCH(true);
}

/// reallocate the buffer with specified size with respect to array
COREARRAY_DLLEXPORT bool gds_Balance_ArrayRead_Buffer(PdArrayRead array[],
	int n, Int64 buffer_size)
{
	CORETRY
		Balance_ArrayRead_Buffer(array, n, buffer_size);
		return true;
	CORECATCH(false);
}

} // extern "C"
