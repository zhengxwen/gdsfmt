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
 *	\author   Xiuwen Zheng
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Export the C interface of CoreArray library
 *	\details
**/


#include <CoreDEF.h>
#include <dType.h>
#include <dPlatform.h>
#include <dBase.h>
#include <dSeq.h>
#include <dParallel.h>

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
	// Error Object

	class ErrCoreGDS: public ErrCoreArray
	{
	public:
		ErrCoreGDS() {};
		ErrCoreGDS(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		ErrCoreGDS(const std::string &msg) { fMessage = msg; }
	};


	// Stream Events

	typedef void (*OnGDSStreamEvent)(void *Data, Int64 size, int *Stop);

	class TdStreamEvent: public CdObjMsg
	{
	public:
		OnGDSStreamEvent evBegin, evRunning, evEnd;
		void *Data;

		void OnDo(CdObjMsg *Sender, Int32 Msg, void *data)
		{
			Int64 *p = (Int64*)data;
			int Stop = false;
			switch (Msg)
			{
				case mcBeginLoad: case mcBeginSave:
					if (evBegin)
						evBegin(Data, *p, &Stop);
					break;
				case mcLoading: case mcSaving:
					if (evRunning)
						evRunning(Data, *p, &Stop);
					break;
				case mcLoaded: case mcSaved:
					if (evEnd)
						evEnd(Data, *p, &Stop);
					break;
			}
			if (Stop)
				throw ErrCoreGDS("The user stops!");
		}
	};


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


#ifndef COREARRAY_R_LINK
	// TdCombine2View

	class TdCombine2View: public CdSequenceX
	{
	public:
		TdCombine2View(CdSequenceX **List, bool *Trans, int Cnt)
		{
			fColCnt = fRowCnt = 0;
			TItem I;
			for (int i=0; i < Cnt; i++)
			{
				I.Obj = List[i];
				I.Trans = List[i];
				I.Row = 1; I.Col = fColCnt;
				switch (I.Obj->DimCnt())
				{
					case 0:
						if (fRowCnt < 2) fRowCnt = 2;
						fColCnt++;
						break;
					case 1:
						if (fRowCnt < I.Obj->Count()+1)
							fRowCnt = I.Obj->Count() + 1;
						fColCnt++;
						break;
					default:
						Int32 nRow = I.Obj->GetDLen(I.Obj->DimCnt()-2);
						Int32 nCol = I.Obj->GetDLen(I.Obj->DimCnt()-1);
						if (!I.Trans)
						{
							if (fRowCnt < nRow+1) fRowCnt = nRow+1;
							fColCnt += nCol;
						} else {
							if (fRowCnt < nCol+1) fRowCnt = nCol+1;
							fColCnt += nRow;
						}
				}
				fList.push_back(I);
			}
		}

		virtual void AddDim(Int32 NewDimLen = -1) {}
		virtual void DeleteDim() {}
		virtual void SetDimLen(const Int32 *Lens, size_t LenCnt) {}
		virtual void SetDLen(int DimIndex, Int32 Value) {}
		virtual void Append(void const* Buffer, ssize_t Cnt, TSVType InSV) {}
		virtual void AppendIter(TdIterator &iter, ssize_t Cnt) {}
		virtual void Clear() {}

		virtual UTF16String Name() const { return UTF16String(); }

		virtual int DimCnt() const { return 2; }
		virtual void GetDimLen(Int32 *Dims) const
			{ Dims[0] = fRowCnt; Dims[1] = fColCnt; }

		virtual Int32 GetDLen(int DimIndex) const
		{
			switch (DimIndex)
			{
				case 0: return fRowCnt;
				case 1: return fColCnt;
			}
			return 0;
		}
		virtual TdIterator Iterator(Int32 const* DimIndex)
		{
			TdIterator rv;
			rv.Handler = this;
			IterRow(rv) = DimIndex[0];
			IterCol(rv) = DimIndex[1];
			return rv;
		}
		virtual TdIterator atStart()
		{
			TdIterator rv;
			rv.Handler = this;
			IterRow(rv) = IterCol(rv) = 0;
			return rv;
		}
		virtual TdIterator atEnd()
		{
			TdIterator rv;
			rv.Handler = this;
			IterRow(rv) = fRowCnt;
			IterCol(rv) = 0;
			return rv;
		}

	protected:
		struct TItem
		{
			CdSequenceX *Obj;
			Int32 Row, Col;
			bool Trans;
			TItem() { Obj = NULL; Row = Col = 0; Trans = false; }
		};
		std::vector<TItem> fList;
		Int32 fColCnt, fRowCnt;

		virtual UTF16String _toStr(TdIterator &it)
		{
			TdIterator I;
			CdSequenceX *obj = xFindTitle(IterRow(it), IterCol(it));
			if (obj)
				return obj->Name();
			obj = xFind(IterRow(it), IterCol(it), I);
			if (obj)
				return I.toStr();
			else
				return UTF16String();
		}

		virtual void _StrTo(TdIterator &it, const UTF16String &v)
		{
			TdIterator I;
			CdSequenceX *obj = xFind(IterRow(it), IterCol(it), I);
			if (obj) I.StrTo(v);
		}

		virtual void _Advance(TdIterator &it)
		{
			Int32 &val = IterCol(it);
			if ((++val) >= fColCnt)
				{ val = 0; ++IterRow(it); }
		}
		virtual void _Previous(TdIterator &it)
		{
			Int32 &val = IterCol(it);
			if ((--val) < 0)
				{ val = fColCnt; --IterCol(it); }
		}
		virtual bool _isStart(const TdIterator &it)
			{ return (IterRow(it)==0) && (IterCol(it)==0); }
		virtual bool _isEnd(const TdIterator &it)
			{ return (IterRow(it)<0) || (IterRow(it)>=fRowCnt) ||
				(IterCol(it)<0) || (IterCol(it)>=fColCnt); }
		virtual bool _isEqual(TdIterator &it1, TdIterator &it2)
			{ return (IterRow(it1)==IterRow(it2)) && (IterCol(it1)==IterCol(it2)); }

	private:
		virtual Int64 _toInt(TdIterator &it) { return 0; }
		virtual long double _toFloat(TdIterator &it) { return 0; }
		virtual void _IntTo(TdIterator &it, const Int64 v) {}
		virtual void _FloatTo(TdIterator &it, const LongFloat v) {}
		virtual void _Assign(TdIterator &it, TdIterator &its) {}
		virtual int _Compare(TdIterator &it1, TdIterator &it2) { return 0; }
		virtual void _LoadIter(TdIterator &it, CdSerial &Reader) {}
		virtual void _SaveIter(TdIterator &it, CdSerial &Writer) {}
		virtual void _InitIter(TdIterator &it, ssize_t Len) {}
		virtual void _DoneIter(TdIterator &it, ssize_t Len) {}
		virtual void _Swap(TdIterator &it1, TdIterator &it2) {}
		virtual void KeepInStream(CdSerial &Reader, void * Data) {}

		inline Int32 &IterRow(TdIterator &I)
			{ return *static_cast<Int32*>((void*)&I.VByte[0]); }
		inline const Int32 &IterRow(const TdIterator &I)
			{ return *static_cast<Int32*>((void*)&I.VByte[0]); }
		inline Int32 &IterCol(TdIterator &I)
			{ return *static_cast<Int32*>((void*)&I.VByte[4]); }
		inline const Int32 &IterCol(const TdIterator &I)
			{ return *static_cast<Int32*>((void*)&I.VByte[4]); }

		CdSequenceX *xFindTitle(Int32 vRow, Int32 vCol)
		{
			std::vector<TItem>::const_iterator it;
			for (it=fList.begin(); it != fList.end(); it++)
				if ((it->Row==vRow+1) && (it->Col==vCol))
					return it->Obj;
			return NULL;
		}
		CdSequenceX *xFind(Int32 vRow, Int32 vCol, TdIterator &OutIter)
		{
			std::vector<TItem>::const_iterator it;
			for (it=fList.begin(); it != fList.end(); it++)
			{
				int DCnt = it->Obj->DimCnt();
				switch (DCnt)
				{
					case 0:
						if ((it->Row==vRow) && (it->Col==vCol))
						{
							OutIter = it->Obj->atStart();
							return it->Obj;
						}
						break;
					case 1:
						if ((it->Col==vCol) && (it->Row<=vRow) &&
							(vRow < it->Row+it->Obj->Count()))
						{
							Int32 I = vRow - it->Row;
							OutIter = it->Obj->Iterator(&I);
							return it->Obj;
						}
						break;
					default:
						Int32 nRow = it->Obj->GetDLen(DCnt-2);
						Int32 nCol = it->Obj->GetDLen(DCnt-1);
						if (!it->Trans)
						{
							if ((it->Col<=vCol) && (it->Row<=vRow) &&
								(vCol < it->Col+nCol) && (vRow < it->Row+nRow))
							{
								Int32 st[2] = {vRow - it->Row, vCol - it->Col};
								OutIter = it->Obj->Iterator(st);
								return it->Obj;
							}
						} else {
							if ((it->Col<=vCol) && (it->Row<=vRow) &&
								(vCol < it->Col+nRow) && (vRow < it->Row+nCol))
							{
								Int32 st[2] = {vCol - it->Col, vRow - it->Row};
								OutIter = it->Obj->Iterator(st);
								return it->Obj;
							}
						}
				}
			}
			return NULL;
		}
	};

#endif // COREARRAY_R_LINK

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
				throw ErrCoreGDS("Invalid class index (%d).", Index);

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
				throw ErrCoreGDS("Invalid pipe index (%d).", Index);
			CdPipeMgrItem * const &it = dStreamPipeMgr.RegList().at(Index);
			*OutCoder = it->Coder();
			*OutDesp = it->Description();
			return true;
		CORECATCH(false);
	}


	// Functions for File

	COREARRAY_DLLEXPORT CdGDSFile *gds_FileOpen(const char *FileName, bool ReadOnly)
	{
		CdGDSFile *rv = new CdGDSFile;
		CORETRY
			rv->LoadFile(FileName, ReadOnly);
			Init.Files.push_back(rv);
			return rv;
		CORECATCH2(delete rv, NULL);
	}

	COREARRAY_DLLEXPORT CdGDSFile *gds_FileCreate(const char *FileName)
	{
		CdGDSFile *rv = new CdGDSFile;
		CORETRY
			rv->SaveAsFile(FileName);
			Init.Files.push_back(rv);
			return rv;
		CORECATCH2(delete rv, NULL);
	}

	COREARRAY_DLLEXPORT bool gds_FileClose(CdGDSFile *Handle)
	{
		CORETRY
		vector<CdGDSFile*>::iterator i;
			i = find(Init.Files.begin(), Init.Files.end(), Handle);
			if (i == Init.Files.end())
				throw ErrCoreGDS("Invalid File Handle.");
			Init.Files.erase(i);
			delete Handle;
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_FileSync(CdGDSFile *Handle)
	{
		CORETRY
			Handle->SyncFile();
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT int gds_FileName(CdGDSFile *Handle, char *OutStr, int OutBufLen)
	{
		CORETRY
			string s = UTF16toUTF8(Handle->FileName());
			if (OutStr)
				strncpy(OutStr, s.c_str(), OutBufLen);
			return s.length();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_FileReadOnly(CdGDSFile *Handle)
	{
		return Handle->ReadOnly();
	}

	COREARRAY_DLLEXPORT CdGDSObj *gds_FilePath(CdGDSFile *Handle,
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

	COREARRAY_DLLEXPORT Int64 gds_FileSize(CdGDSFile *Handle)
	{
		CORETRY
			return Handle->GetFileSize();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_FileTidyUp(CdGDSFile *Handle, bool deep)
	{
		CORETRY
			Handle->TidyUp(deep);
			return true;
		CORECATCH(false);
	}


	// Functions for File Structure

	COREARRAY_DLLEXPORT CdGDSFolder *gds_NodeRoot(CdGDSFile *Handle)
	{
		CORETRY
			return &Handle->Root();
		CORECATCH(NULL);
	}

	COREARRAY_DLLEXPORT CdGDSFile *gds_NodeGDSFile(CdGDSObj *Obj)
	{
		CORETRY
		return Obj->GDSFile();
		CORECATCH(NULL);
	}

	COREARRAY_DLLEXPORT int gds_NodeChildCount(CdGDSFolder *Node)
	{
		CORETRY
		return Node->NodeCount();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT CdGDSFolder *gds_NodeChildFolder(CdGDSFolder *Node, int Index)
	{
		CORETRY
		return &Node->DirItem(Index);
		CORECATCH(NULL);
	}

	COREARRAY_DLLEXPORT CdGDSObj *gds_NodeChild(CdGDSFolder *Node, int Index)
	{
		CORETRY
			return Node->ObjItem(Index);
		CORECATCH(NULL);
	}

	COREARRAY_DLLEXPORT CdGDSObj *gds_NodePath(CdGDSFolder *Node, const char *Path)
	{
		CORETRY
			return Node->Path(T(Path));
		CORECATCH(NULL);
	}

	/// remove a GDS node
	COREARRAY_DLLEXPORT bool gds_NodeDelete(CdGDSObj *Node)
	{
		CORETRY
			if (Node->Folder())
			{
				Node->Folder()->DeleteObj(Node);
			} else
				throw ErrCoreGDS("Cann't delete the root.");
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_NodeMoveTo(CdGDSObj *Source, CdGDSFolder *Dest)
	{
		CORETRY
		Source->MoveTo(*Dest);
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_NodeCopyTo(CdGDSObj *Source, CdGDSFolder *Dest)
	{
		CORETRY
			UTF16String n = Source->Name();
			if (Dest->ObjItemEx(n))
				throw ErrCoreGDS("The target folder has a node with the same name!");
			CdGDSObj *rv = Source->NewOne();
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

	COREARRAY_DLLEXPORT bool gds_NodeAddLabel(CdGDSFolder *Folder,
		const char *Name)
	{
		CORETRY
			Folder->AddObj(T(Name), NULL);
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT CdGDSFolder *gds_NodeAddFolder(CdGDSFolder *Folder,
		const char *Name)
	{
		CORETRY
			return (CdGDSFolder*)(Folder->AddFolder(T(Name)));
		CORECATCH(NULL);
	}

	COREARRAY_DLLEXPORT CdSequenceX *gds_NodeAddArray(CdGDSFolder *Folder,
		const char *Name, const char *TypeName, int DimCnt)
	{
		CORETRY
			CdObjClassMgr::TdOnObjCreate OnObj =
				dObjManager().NameToClass(TypeName);
			CdSequenceX *rv = NULL;
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

	COREARRAY_DLLEXPORT bool gds_NodeIsFolder(CdGDSObj *Node)
	{
		return (dynamic_cast<CdGDSFolder*>(Node) != NULL);
	}

	COREARRAY_DLLEXPORT bool gds_NodeIsNull(CdGDSObj *Node)
	{
		return (dynamic_cast<CdGDSLabel*>(Node) != NULL);
	}

	COREARRAY_DLLEXPORT bool gds_NodeIsContainer(CdGDSObj *Node)
	{
		return (dynamic_cast<CdContainer*>(Node) != NULL);
	}

	COREARRAY_DLLEXPORT bool gds_NodeIsSequence(CdGDSObj *Node)
	{
		return (dynamic_cast<CdSequenceX*>(Node) != NULL);
	}

	COREARRAY_DLLEXPORT bool gds_NodeIsVector(CdGDSObj *Node)
	{
		return (dynamic_cast<CdVectorX*>(Node) != NULL);
	}

	COREARRAY_DLLEXPORT bool gds_NodeIsFStr(CdGDSObj *Node)
	{
		return (dynamic_cast<CdFStr8*>(Node) != NULL) ||
			(dynamic_cast<CdFStr16*>(Node) != NULL) ||
			(dynamic_cast<CdFStr32*>(Node) != NULL);
	}

	COREARRAY_DLLEXPORT bool gds_NodeIsStreamContainer(CdGDSObj *Node)
	{
		return (dynamic_cast<CdGDSStreamContainer*>(Node) != NULL);
	}

	COREARRAY_DLLEXPORT int gds_NodeName(CdGDSObj *Node, char *OutStr,
		int OutBufLen, bool FullName)
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

	COREARRAY_DLLEXPORT bool gds_NodeSetName(CdGDSObj *Node, char *NewName)
	{
		CORETRY
		Node->SetName(T(NewName));
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT int gds_NodeClassName(CdGDSObj *Node, char *OutStr,
		int OutBufLen)
	{
		CORETRY
			string n;
			n = Node->dName();
			if (OutStr)
				strncpy(OutStr, n.c_str(), OutBufLen);
			return n.length();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_NodeStreamInfo(CdGDSObj *Node, Int64 *TotalIn,
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

	COREARRAY_DLLEXPORT bool gds_SetPackedMode(CdGDSObj *Node, const char *Mode)
	{
		CORETRY
			if (dynamic_cast<CdContainer*>(Node))
				static_cast<CdContainer*>(Node)->SetPackedMode(Mode);
			else
				if (dynamic_cast<CdGDSStreamContainer*>(Node))
					static_cast<CdGDSStreamContainer*>(Node)->SetPackedMode(Mode);
				else
					throw ErrCoreGDS("No 'SetPackedMode' function!");
			return true;
		CORECATCH(false);
	}


	// Functions for Specific Node

#ifndef COREARRAY_R_LINK
	COREARRAY_DLLEXPORT TdCombine2View *gds_NodeCombineView(CdSequenceX **List,
		bool *Trans, int ListCnt)
	{
		CORETRY
			return new TdCombine2View(List, Trans, ListCnt);
		CORECATCH(NULL);
	}
#endif // COREARRAY_R_LINK

	COREARRAY_DLLEXPORT bool gds_NodeFree(CdGDSObj *Node)
	{
		CORETRY
			Node->Release();
			return true;
		CORECATCH(false);
	}


	// Functions for File Node -- TdObjAttr

	COREARRAY_DLLEXPORT int gds_AttrCount(CdGDSObj *Node)
	{
		CORETRY
			return Node->Attribute().Count();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT int gds_AttrNameIndex(CdGDSObj *Node, const char *Name)
	{
		CORETRY
			return Node->Attribute().IndexName(T(Name));
		CORECATCH(-2);
	}

	COREARRAY_DLLEXPORT bool gds_AttrIxAdd(CdGDSObj *Node, const char *Name,
		const char *Value)
	{
		CORETRY
			TdsAny &D = Node->Attribute().Add(T(Name));
			D.Assign(Value);
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT int gds_AttrIxName(CdGDSObj *Node, int Index, char *OutStr,
		int OutBufLen)
	{
		CORETRY
			string n = UTF16toUTF8(Node->Attribute().Names(Index));
			if (OutStr)
				strncpy(OutStr, n.c_str(), OutBufLen);
			return n.length();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_AttrIxSetName(CdGDSObj *Node, int Index,
		const char *NewName)
	{
		CORETRY
			Node->Attribute().SetName(Index, T(NewName));
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT int gds_AttrIxStr(CdGDSObj *Node, int Index, char *OutStr,
		int OutBufLen)
	{
		CORETRY
			TdsAny &D = Node->Attribute()[Index];
			string n = D.GetStr8();
			if (OutStr)
				strncpy(OutStr, n.c_str(), OutBufLen);
			return n.length();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_AttrIxToStr(CdGDSObj *Node, int Index, const char *Str)
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

	COREARRAY_DLLEXPORT int gds_AttrIxType(CdGDSObj *Node, int Index)
	{
		CORETRY
			TdsAny &D = Node->Attribute()[Index];
			return D.Type();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT const char *gds_AttrIxTypeStr(CdGDSObj *Node, int Index)
	{
		CORETRY
			TdsAny &D = Node->Attribute()[Index];
			return D.dvtNames(D.Type());
		CORECATCH(NULL);
	}


	// Functions for CdContainer - TdIterator

	COREARRAY_DLLEXPORT bool gds_IterGetStart(CdContainer *Node, TdIterator &Out)
	{
		CORETRY
			Out = Node->atStart();
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_IterGetEnd(CdContainer *Node, TdIterator &Out)
	{
		CORETRY
			Out = Node->atEnd();
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT CdContainer* gds_IterGetHandle(TdIterator &Iter)
	{
		return Iter.Handler;
	}

	COREARRAY_DLLEXPORT bool gds_IterAdv(TdIterator &Iter)
	{
		CORETRY
			++ Iter;
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_IterAdvEx(TdIterator &Iter, const ssize_t offset)
	{
		CORETRY
			Iter += offset;
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_IterPrev(TdIterator &Iter)
	{
		CORETRY
			-- Iter;
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_IterPrevEx(TdIterator &Iter, const ssize_t offset)
	{
		CORETRY
			Iter -= offset;
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT int gds_IterInt(TdIterator &Iter)
	{
		CORETRY
			return Iter.toInt();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT double gds_IterFloat(TdIterator &Iter)
	{
		CORETRY
			return Iter.toFloat();
		CORECATCH(NaN);
	}

	COREARRAY_DLLEXPORT int gds_IterStr(TdIterator &Iter, char *OutStr, int OutBufLen)
	{
		CORETRY
			UTF8String s = UTF16toUTF8(Iter.toStr());
			if (OutStr)
				strncpy(OutStr, s.c_str(), OutBufLen);
			return s.length();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_IterIntTo(TdIterator &Iter, int val)
	{
		CORETRY
			Iter.IntTo(val);
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_IterFloatTo(TdIterator &Iter, double val)
	{
		CORETRY
			Iter.FloatTo(val);
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_IterStrTo(TdIterator &Iter, const char *Str)
	{
		CORETRY
			Iter.StrTo(PChartoUTF16(Str));
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT size_t gds_IterRData(TdIterator &Iter, void *OutBuf, size_t Cnt, TSVType OutSV)
	{
		CORETRY
			return Iter.rData(OutBuf, Cnt, OutSV);
		CORECATCH((size_t)-1);
	}

	COREARRAY_DLLEXPORT size_t gds_IterWData(TdIterator &Iter, const void *InBuf, size_t Cnt, TSVType InSV)
	{
		CORETRY
			return Iter.wData(InBuf, Cnt, InSV);
		CORECATCH((size_t)-1);
	}



	// Functions for CdSequenceX

	COREARRAY_DLLEXPORT int gds_SeqDimCnt(CdSequenceX *Obj)
	{
		CORETRY
			return Obj->DimCnt();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_SeqGetDim(CdSequenceX *Obj, int *OutBuf)
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

	COREARRAY_DLLEXPORT Int64 gds_SeqGetCount(CdSequenceX *Obj)
	{
		CORETRY
			return Obj->Count();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT int gds_SeqSVType(CdSequenceX *Obj)
	{
		CORETRY
			return Obj->SVType();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT int gds_SeqBitOf(CdSequenceX *Obj)
	{
		CORETRY
			return Obj->BitOf();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT bool gds_SeqIndexIter(CdSequenceX *Obj, int *Index, TdIterator *Out)
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

	COREARRAY_DLLEXPORT bool gds_SeqLoad(CdSequenceX *Obj, const char *FileName,
		void *Data = NULL,
		OnGDSStreamEvent evBegin = NULL,
		OnGDSStreamEvent evRunning = NULL,
		OnGDSStreamEvent evEnd = NULL,
		int TimeInterval = -1)
	{
		CORETRY
			TdDefParamText op;
			if (TimeInterval <= 0)
				op.TimeInterval = TimeInterval;

			TdStreamEvent event;
			event.evBegin = evBegin;
			event.evRunning = evRunning;
			event.evEnd = evEnd;
			event.Data = Data;

			Obj->AddMsgEx(&event, &TdStreamEvent::OnDo);
			CORETRY
				Obj->LoadFromText(FileName, &op);
			COREFINALLY(Obj->RemoveMsgEx(&event, &TdStreamEvent::OnDo));

			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_SeqSave(CdSequenceX *Obj, const char *FileName,
		void *Data = NULL,
		OnGDSStreamEvent evBegin = NULL,
		OnGDSStreamEvent evRunning = NULL,
		OnGDSStreamEvent evEnd = NULL,
		int TimeInterval = -1)
	{
		CORETRY
			TdDefParamText op;
			if (TimeInterval <= 0)
				op.TimeInterval = TimeInterval;

			TdStreamEvent event;
			event.evBegin = evBegin;
			event.evRunning = evRunning;
			event.evEnd = evEnd;
			event.Data = Data;

			Obj->AddMsgEx(&event, &TdStreamEvent::OnDo);
			CORETRY
				Obj->SaveToText(FileName, &op);
			COREFINALLY(Obj->RemoveMsgEx(&event, &TdStreamEvent::OnDo));
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT int gds_SeqFStrMaxLen(CdSequenceX *Obj)
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

	COREARRAY_DLLEXPORT bool gds_rData(CdSequenceX *obj, Int32 const* Start,
		Int32 const* Length, void *OutBuf, TSVType OutSV)
	{
		CORETRY
			obj->rData(Start, Length, OutBuf, OutSV);
			return true;
		CORECATCH(false);
	}
	COREARRAY_DLLEXPORT bool gds_rDataEx(CdSequenceX *obj, Int32 const* Start,
		Int32 const* Length, const CBOOL *const Selection[], void *OutBuf,
		TSVType OutSV)
	{
		CORETRY
			obj->rDataEx(Start, Length, Selection, OutBuf, OutSV);
			return true;
		CORECATCH(false);
	}

	// CdSequenceX -- write

	COREARRAY_DLLEXPORT bool gds_wData(CdSequenceX *obj, Int32 const* Start,
		Int32 const* Length, const void *InBuf, TSVType InSV)
	{
		CORETRY
			obj->wData(Start, Length, InBuf, InSV);
			return true;
		CORECATCH(false);
	}


	// CdSequenceX -- append

	COREARRAY_DLLEXPORT bool gds_AppendData(CdSequenceX *obj, int Cnt, const void *InBuf, TSVType InSV)
	{
		if (Cnt < 0) return false;
		CORETRY
			obj->Append(InBuf, Cnt, InSV);
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT bool gds_AppendString(CdSequenceX *obj, int Cnt, const char *buffer[])
	{
		if (Cnt < 0) return false;
		CORETRY
			UTF8String *Buf = new UTF8String[Cnt];
			UTF8String *d = Buf;
			const char **s = buffer;
			for (int i=0; i < Cnt; i++) *d++ = *s++;
			obj->Append(Buf, Cnt, svStrUTF8);
			delete [] Buf;
			return true;
		CORECATCH(false);
	}


	// CdSequenceX -- Assign

	COREARRAY_DLLEXPORT bool gds_Assign(CdSequenceX *dest_obj, CdSequenceX *src_obj, bool append)
	{
		CORETRY
			dest_obj->AssignOne(*src_obj, append);
			return true;
		CORECATCH(false);
	}



	// Functions for CdGDSStreamContainer

	COREARRAY_DLLEXPORT CdGDSStreamContainer *gds_NewContainer(CdGDSFolder *Folder,
		const char *Name, const char *InputFile, const char *PackMode)
	{
		CORETRY
			TdAutoRef<CBufdStream> file(new CBufdStream(
				new CdFileStream(InputFile, CdFileStream::fmOpenRead)));
			CdGDSStreamContainer *rv = new CdGDSStreamContainer();
			rv->SetPackedMode(PackMode);
			Folder->AddObj(T(Name), rv);
			rv->CopyFrom(*file.get());
			rv->CloseWriter();
			return rv;
		CORECATCH(NULL);
	}

	COREARRAY_DLLEXPORT bool gds_SaveContainer(CdGDSStreamContainer *Container,
		const char *OutputFile)
    {
		CORETRY
			TdAutoRef<CBufdStream> file(new CBufdStream(
				new CdFileStream(OutputFile, CdFileStream::fmCreate)));
			Container->CopyTo(*file.get());
			return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT Int64 gds_ContainerGetSize(CdGDSStreamContainer *Container)
    {
		CORETRY
			return Container->GetSize();
		CORECATCH(-1);
	}


	// Diagnosis

	COREARRAY_DLLEXPORT int gds_DiagnBlockCount(CdGDSFile *Handle)
	{
		CORETRY
			CdBlockCollection *H = (CdBlockCollection*)Handle;
		    return H->BlockList().size();
		CORECATCH(-1);
	}

	COREARRAY_DLLEXPORT const CdBlockStream::TBlockInfo *gds_DiagnBlockInfo(
		CdGDSFile *Handle, int Index)
	{
		CORETRY
			CdBlockCollection *H = (CdBlockCollection*)Handle;
			return H->BlockList()[Index]->List();
		CORECATCH(NULL);
	}

	COREARRAY_DLLEXPORT bool gds_DiagnBlockSize(CdGDSFile *Handle, int Index,
		Int64 &size, Int64 &capacity)
	{
		CORETRY
		CdBlockCollection *H = (CdBlockCollection*)Handle;
			size = H->BlockList()[Index]->Size();
			capacity = H->BlockList()[Index]->Capacity();
		return true;
		CORECATCH(false);
	}

	COREARRAY_DLLEXPORT const CdBlockStream::TBlockInfo *gds_DiagnUnusedBlock(
		CdGDSFile *Handle)
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
	COREARRAY_DLLEXPORT CdThreadMutex* plc_InitMutex()
	{
		CORETRY
			return new CdThreadMutex;
		CORECATCH(NULL);
	}
	/// destroy the mutex object
	COREARRAY_DLLEXPORT bool plc_DoneMutex(CdThreadMutex* obj)
	{
		CORETRY
			if (obj) delete obj;
			return true;
		CORECATCH(false);
	}
	/// lock the mutex object
	COREARRAY_DLLEXPORT bool plc_LockMutex(CdThreadMutex* obj)
	{
		CORETRY
			if (obj) obj->Lock();
			return true;
		CORECATCH(false);
	}
	/// unlock the mutex object
	COREARRAY_DLLEXPORT bool plc_UnlockMutex(CdThreadMutex* obj)
	{
		CORETRY
			if (obj) obj->Unlock();
			return true;
		CORECATCH(false);
	}

	// thread suspending object

	/// initialize a thread suspending object
	COREARRAY_DLLEXPORT CdThreadsSuspending* plc_InitSuspend()
	{
		CORETRY
			return new CdThreadsSuspending;
		CORECATCH(NULL);
	}
	/// destroy the thread suspending object
	COREARRAY_DLLEXPORT bool plc_DoneSuspend(CdThreadsSuspending* obj)
	{
		CORETRY
			if (obj) delete obj;
			return true;
		CORECATCH(false);
	}
	/// suspend the thread suspending object
	COREARRAY_DLLEXPORT bool plc_Suspend(CdThreadsSuspending* obj)
	{
		CORETRY
			if (obj) obj->Suspend();
			return true;
		CORECATCH(false);
	}
	/// wakeup the thread suspending object
	COREARRAY_DLLEXPORT bool plc_WakeUp(CdThreadsSuspending* obj)
	{
		CORETRY
			if (obj) obj->WakeUp();
			return true;
		CORECATCH(false);
	}


	static CParallelBase _ParallelBase;

	/// parallel computing
	COREARRAY_DLLEXPORT bool plc_DoBaseThread(void (*Proc)(CdThread *, int, void*),
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
	COREARRAY_DLLEXPORT CdArrayRead* gds_ArrayRead_Init(CdSequenceX *Obj,
		int Margin, TSVType SVType, const CBOOL *const Selection[],
		bool buf_if_need=true)
	{
		CORETRY
			CdArrayRead *rv = new CdArrayRead;
			rv->Init(*Obj, Margin, SVType, Selection, buf_if_need);
			return rv;
		CORECATCH(NULL);
	}

	/// free a 'CdArrayRead' object
	COREARRAY_DLLEXPORT bool gds_ArrayRead_Free(CdArrayRead *Obj)
	{
		CORETRY
			if (Obj) delete Obj;
			return true;
		CORECATCH(false);
	}

	/// read data
	COREARRAY_DLLEXPORT bool gds_ArrayRead_Read(CdArrayRead *Obj, void *Buffer)
	{
		CORETRY
			Obj->Read(Buffer);
			return true;
		CORECATCH(false);
	}

	/// return true, if it is of the end
	COREARRAY_DLLEXPORT bool gds_ArrayRead_Eof(CdArrayRead *Obj)
	{
		CORETRY
			return Obj->Eof();
		CORECATCH(true);
	}

	/// reallocate the buffer with specified size with respect to array
	COREARRAY_DLLEXPORT bool gds_Balance_ArrayRead_Buffer(CdArrayRead *array[],
		int n, Int64 buffer_size=-1)
	{
		CORETRY
			Balance_ArrayRead_Buffer(array, n, buffer_size);
			return true;
		CORECATCH(false);
	}

} // extern "C"
