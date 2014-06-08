// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dFile.cpp: Functions and classes for CoreArray Genomic Data Structure (GDS)
//
// Copyright (C) 2013	Xiuwen Zheng
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

#include <dFile.h>
#include <algorithm>
#include <cstdio>


namespace CoreArray
{
	// CdStreamPipeMgr

	/// The pipe for reading data from a ZIP stream
	class CdZIPReadPipe: public CdStreamPipe
	{
	protected:
		virtual CdStream* InitPipe(CBufdStream *Filter)
		{
			fStream = Filter->Stream();
			if (dynamic_cast<CdSerial*>(Filter))
			{
				fPStream = new CdZIPInflate(dynamic_cast<CdSerial*>(Filter)->
					BlockStream());
			} else
				fPStream = new CdZIPInflate(Filter->Stream());
			return fPStream;
		}
		virtual CdStream* FreePipe()
		{
			fPStream->Release();
			return fStream;
		}

	private:
		CdStream* fStream;
		CdZIPInflate* fPStream;
	};


	/// The pipe for writing data to a ZIP stream
	class CdZIPWritePipe: public CdStreamPipe
	{
	public:
		CdZIPWritePipe(CdZIPDeflate::TZLevel vLevel,
			TdCompressRemainder &vRemainder): fRemainder(vRemainder)
			{ fLevel = vLevel; }

		COREARRAY_INLINE CdZIPDeflate::TZLevel Level() const { return fLevel; };
		COREARRAY_INLINE CdStream* Stream() const { return fStream; };
		COREARRAY_INLINE CdStream* StreamZIP() const { return fPStream; };

	protected:
		virtual CdStream* InitPipe(CBufdStream *Filter)
		{
			fStream = Filter->Stream();
			fPStream = new CdZIPDeflate(fStream, fLevel);
			fPStream->PtrExtRec = &fRemainder;
			return fPStream;
		}
		virtual CdStream* FreePipe()
		{
			fPStream->Release();
			return fStream;
		}

	private:
		CdStream *fStream;
		CdZIPDeflate *fPStream;
		CdZIPDeflate::TZLevel fLevel;
		TdCompressRemainder &fRemainder;
	};


	const char *ZIPStrings[4] =
		{ "ZIP", "ZIP.fast", "ZIP.default", "ZIP.max" };
	const CdZIPDeflate::TZLevel ZIPLevels[4] =
		{ CdZIPDeflate::zcDefault, CdZIPDeflate::zcFastest,
		  CdZIPDeflate::zcDefault, CdZIPDeflate::zcMax };

	class CdPipeZIP: public CdPipeMgrItem
	{
	public:
		CdPipeZIP(CdZIPDeflate::TZLevel level = CdZIPDeflate::zcDefault)
			{ vLevel = level; fSizeInfo_Ptr = -1; }

		virtual CdPipeMgrItem *NewOne()
			{ return new CdPipeZIP(vLevel); }
		virtual const char *Coder() const
			{ return "ZIP"; };
		virtual const char *Description() const
			{ return "Version: " ZLIB_VERSION; }
		virtual bool Equal(const char *Mode) const
			{ return Which(Mode, ZIPStrings, 4) >= 0; }
		virtual void PushReadPipe(CBufdStream &buf)
			{ buf.PushPipe(new CdZIPReadPipe); }
		virtual void PushWritePipe(CBufdStream &buf)
			{ buf.PushPipe(new CdZIPWritePipe(vLevel, fRemainder)); }
		virtual void PopPipe(CBufdStream &buf)
			{ buf.PopPipe(); }
		virtual bool WriteMode(CBufdStream &buf) const
			{ return dynamic_cast<CdZIPDeflate*>(buf.Stream())!=NULL; }
		virtual void ClosePipe(CBufdStream &buf)
		{
			if (dynamic_cast<CdZIPDeflate*>(buf.Stream()))
            	static_cast<CdZIPDeflate*>(buf.Stream())->Close();
        }

	protected:
		virtual CdPipeMgrItem *Match(const char *Mode)
		{
			int i = Which(Mode, ZIPStrings, 4);
			if (i >= 0)
				return new CdPipeZIP(ZIPLevels[i]);
			else
				return NULL;
		}
		virtual bool GetStreamInfo(CBufdStream *buf)
		{
			TdPtr64 in, out;

			if (buf)
			{
				if (dynamic_cast<CdZIPDeflate*>(buf->Stream()))
				{
					CdZIPDeflate *s = static_cast<CdZIPDeflate*>(buf->Stream());
					in = s->TotalIn();
					out = s->TotalOut() + (s->HaveClosed() ? 0 : s->Pending());
				} else
					return false;
			} else
				in = out = 0;
			if ((in!=fStreamTotalIn) || (out!=fStreamTotalOut))
			{
				fStreamTotalIn = in; fStreamTotalOut = out;
				return true;
			} else
				return false;
		}
		virtual void UpdateStreamInfo(CdStream &Stream)
		{
			if (fSizeInfo_Ptr >= 0)
			{
				Int64 Ary[2] = { fStreamTotalIn, fStreamTotalOut };
				Stream.SetPosition(fSizeInfo_Ptr);
                Stream.WriteBuffer((void*)Ary, sizeof(Ary));
            }
		}
		virtual void LoadStream(CdSerial &Reader, TdVersion Version)
		{
			Int64 Ary[2];
			if (Reader["PIPE_SIZE"].rShortBuf(Ary, 2))
			{
				fSizeInfo_Ptr = Reader.Position();
				fStreamTotalIn = Ary[0];
				fStreamTotalOut = Ary[1];
			} else {
				fSizeInfo_Ptr = -1;
				fStreamTotalIn = fStreamTotalOut = -1;
			}
			UInt8 I = 0;
			Reader["PIPE_LEVEL"] >> I;
			if (I > 3)
                throw ErrGDSObj("Invalid 'PIPE_LEVEL %d'", I);
			vLevel = (CdZIPDeflate::TZLevel)I;
		}
		virtual void SaveStream(CdSerial &Writer)
		{
        	UpdateStreamSize();
			Int64 Ary[2] = { fStreamTotalIn, fStreamTotalOut };
			Writer["PIPE_SIZE"].wShortBuf(Ary, 2);
			fSizeInfo_Ptr = Writer.Position() - sizeof(Ary);
			UInt8 I = vLevel;
			Writer["PIPE_LEVEL"] << I;
		}

	private:
		CdZIPDeflate::TZLevel vLevel;
		TdPtr64 fSizeInfo_Ptr;
	};


	CdStreamPipeMgr dStreamPipeMgr;
}

using namespace std;
using namespace CoreArray;


// CdPipeMgrItem

CdPipeMgrItem::CdPipeMgrItem()
{
	fOwner = NULL;
	fStreamTotalIn = fStreamTotalOut = -1;
}

CdPipeMgrItem::~CdPipeMgrItem() {}

void CdPipeMgrItem::UpdateStreamSize()
{
	if (fOwner)
		fOwner->GetPipeInfo();
}

void CdPipeMgrItem::LoadStream(CdSerial &Reader, TdVersion Version) {}

void CdPipeMgrItem::SaveStream(CdSerial &Writer) {}

int CdPipeMgrItem::Which(const char *txt, const char **Strs, int nStrs)
{
	for (int i=0; i < nStrs; i++, Strs++)
	{
		if (EqualStrNoCase(txt, *Strs))
			return i;
	}
	return -1;
}

bool CdPipeMgrItem::EqualStrNoCase(const char *s1, const char *s2)
{
	for (;*s1 || *s2; s1++, s2++)
		if (toupper(*s1) != toupper(*s2))
			return false;
	return true;
}

// CdStreamPipeMgr

CdStreamPipeMgr::CdStreamPipeMgr()
{
	Register(new CdPipeZIP);
}

CdStreamPipeMgr::~CdStreamPipeMgr()
{
	vector<CdPipeMgrItem*>::iterator it;
	for (it = fRegList.begin(); it != fRegList.end(); it++)
		delete *it;
}

void CdStreamPipeMgr::Register(CdPipeMgrItem *vNewPipe)
{
	if (vNewPipe)
		fRegList.push_back(vNewPipe);
}


CdPipeMgrItem *CdStreamPipeMgr::Match(CdGDSObj &Obj, const char *Mode)
{
	vector<CdPipeMgrItem*>::iterator it;
	for (it = fRegList.begin(); it != fRegList.end(); it++)
	{
		CdPipeMgrItem *rv = (*it)->Match(Mode);
		if (rv)
		{
        	rv->fOwner = &Obj;
			return rv;
        }
	}
	return NULL;
}


// CdObjAttr

static const char *rsAttrName = "No Attribute Name ('%s').";
static const char *rsAttrNameExist = "Attribute '%s' has existed.";

CdObjAttr::CdObjAttr(CdGDSObj &vOwner): fOwner(vOwner) {}

CdObjAttr::~CdObjAttr()
{
	vector<TdPair*>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		delete *it;
}

void CdObjAttr::Assign(CdObjAttr &Source)
{
	/// \todo Assign procedure
	Clear();
	for (int i=0; i < (int)Source.Count(); i++)
		Add(Source.Names(i)) = Source[i];
    Changed();
}

TdsAny & CdObjAttr::Add(const UTF16String &Name)
{
	xValidateName(Name);
	vector<TdPair*>::iterator it = Find(Name);
	if (it == fList.end())
	{
		TdPair *I = new TdPair;
		I->name = Name;
		fList.push_back(I);
		Changed();
		return I->val;
	} else
		throw ErrGDSObj(rsAttrNameExist, UTF16toUTF8(Name).c_str());
}

int CdObjAttr::IndexName(const UTF16String &Name)
{
	vector<TdPair*>::iterator it = Find(Name);
	if (it != fList.end())
    	return it - fList.begin();
	else
		return -1;
}

void CdObjAttr::Delete(const UTF16String &Name)
{
	vector<TdPair*>::iterator it = Find(Name);
	if (it == fList.end())
		throw ErrGDSObj(rsAttrName, UTF16toUTF8(Name).c_str());
	fList.erase(it);
	Changed();
}

void CdObjAttr::Delete(int Index)
{
	fList.at(Index); // check range
    fList.erase(fList.begin() + Index);
	Changed();
}

void CdObjAttr::Clear()
{
	if (!fList.empty())
	{
		fList.clear();
		Changed();
	}
}

void CdObjAttr::Changed()
{
	this->fOwner.fChanged = true;
}

TdsAny & CdObjAttr::operator[](const UTF16String &Name)
{
	vector<TdPair*>::iterator it = Find(Name);
	if (it == fList.end())
		throw ErrGDSObj(rsAttrName, UTF16toUTF8(Name).c_str());
	return (*it)->val;
}

TdsAny & CdObjAttr::operator[](int Index)
{
	return fList.at(Index)->val;
}

void CdObjAttr::LoadAfter(CdSerial &Reader, const TdVersion Version)
{
	Int32 Cnt;
	if (Reader["ATTRCNT"] >> Cnt)
	{
		fList.clear();
		if (Cnt > 0)
		{
			if (Reader["ATTRLIST"].rBuffer())
			{
				for (int i=0; i < Cnt; i++)
				{
					TdPair *I = new TdPair;
					try {
						Reader >> I->name;
						Reader >> I->val;
					} catch (...) {
						delete I;
						break;
					}
					fList.push_back(I);
				}
				Reader.rEndStruct();
			} else
				throw ErrGDSObj("CdObjAttr Load Error!");
		}
	} else
		fList.clear();
}

void CdObjAttr::SaveAfter(CdSerial &Writer)
{
	Int32 Cnt = fList.size();
	Writer["ATTRCNT"] << Cnt;
	if (Cnt > 0)
	{
		Writer["ATTRLIST"].wBuffer();
		vector<TdPair*>::iterator it;
		for (it=fList.begin(); it != fList.end(); it++)
		{
			Writer << (*it)->name;
			Writer << (*it)->val;
		}
		Writer.wEndStruct();
	}
}

vector<CdObjAttr::TdPair*>::iterator CdObjAttr::Find(const UTF16String &Name)
{
	vector<TdPair*>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if ((*it)->name == Name) break;
	return it;
}

void CdObjAttr::SetName(const UTF16String &OldName, const UTF16String &NewName)
{
	xValidateName(NewName);
	vector<TdPair*>::iterator it = Find(OldName);
	if (it == fList.end())
		throw ErrGDSObj(rsAttrName, UTF16toUTF8(OldName).c_str());
	if (OldName != NewName)
	{
		if (HasName(NewName) >= 0)
			throw ErrGDSObj(rsAttrNameExist, UTF16toUTF8(NewName).c_str());
		(*it)->name = NewName;
		Changed();
	}
}

void CdObjAttr::SetName(int Index, const UTF16String &NewName)
{
	TdPair &p = *fList.at(Index); // check range
	xValidateName(NewName);
	if (p.name != NewName)
	{
		if (HasName(NewName) >= 0)
			throw ErrGDSObj(rsAttrNameExist, UTF16toUTF8(NewName).c_str());
		p.name = NewName;
		Changed();
	}
}

void CdObjAttr::xValidateName(const UTF16String &name)
{
	if (name.empty())
        throw ErrGDSObj("Invalid name: ZERO length.");
}


// CdGDSObj

static const char *erNoNameExist = "No name exists!";
static const char *erDupName = "Duplicate name!";
static const char *erMoveToAdd = "Please call 'AddObj' to add an object.";
static const char *erNotShareCollection = "'MoveTo' should be within the same GDS file.";
static const char *erMoveToChild = "Cannot move to its sub folder.";
static const char *erLoseVariable = "The variable (%s) loses.";

CdGDSObj::CdGDSObj(CdGDSFolder *vFolder): fAttr(*this)
{
	fFolder = vFolder;
	fGDSStream = NULL; fPipeInfo = NULL;
	fChanged = false;
}

CdGDSObj::~CdGDSObj()
{
	if (fPipeInfo) delete fPipeInfo;
	fGDSStream->Release();
}

CdGDSObj *CdGDSObj::NewOne(void *Param)
{
	return new CdGDSObj;
}

void CdGDSObj::AssignOne(CdGDSObj &Source, void *Param)
{
    fAttr.Assign(Source.Attribute());
}

UTF16String CdGDSObj::Name() const
{
	if (fFolder)
	{
		vector<CdGDSFolder::TItem>::const_iterator it;
		for (it = fFolder->fList.begin(); it != fFolder->fList.end(); it++)
		{
			if (it->Obj == this)
				return it->Name;
		}
	}
	throw ErrGDSObj(erNoNameExist);
}

UTF16String CdGDSObj::FullName(const UTF16String &Delimiter) const
{
	UTF16String rv = Name();
	CdGDSFolder *p = fFolder;
	if (p != NULL)
	{
		while (p->fFolder)
		{
			rv = p->Name() + Delimiter + rv;
			p = p->fFolder;
		}
	}
	return rv;
}

void CdGDSObj::SetName(const UTF16String &NewName)
{
	if (fFolder)
	{
		vector<CdGDSFolder::TItem>::iterator it;
		for (it = fFolder->fList.begin(); it != fFolder->fList.end(); it++)
		{
			if (it->Obj == this)
			{
				if (it->Name != NewName)
				{
					if (fFolder->_HasName(NewName))
						throw ErrGDSObj(erDupName);
					it->Name = NewName;
					fFolder->fChanged = true;
				}
				return;
			}
		}
	}
	throw ErrGDSObj(erNoNameExist);
}

void CdGDSObj::MoveTo(CdGDSFolder &folder)
{
	if (fGDSStream && folder.GDSStream() && fFolder)
	{
		CdBlockCollection *n1 = &fGDSStream->Collection();
		CdBlockCollection *n2 = &folder.GDSStream()->Collection();
		if (n1 == n2)
		{
			if (dynamic_cast<CdGDSFolder*>(this))
			{
				if (static_cast<CdGDSFolder*>(this)->HasChild(&folder))
					throw ErrGDSObj(erMoveToChild);
			}
			if ((fFolder!=&folder) && (this!=&folder))
			{
				vector<CdGDSFolder::TItem>::iterator it;
				it = fFolder->_FindObj(this);
				if (folder._HasName(it->Name))
					throw ErrGDSObj(erDupName);
				folder.fList.push_back(*it);
				fFolder->fList.erase(it);
				fFolder->fChanged = folder.fChanged = true;
				fFolder = &folder;
			}
		} else
			throw ErrGDSObj(erNotShareCollection);
	} else
		throw ErrGDSObj(erMoveToAdd);
}

void CdGDSObj::Synchronize()
{
	if (fChanged)
		SaveToBlockStream();
}

CdGDSFile *CdGDSObj::GDSFile()
{
	if (fGDSStream)
	{
		CdBlockCollection *collect = &fGDSStream->Collection();
		CdGDSFile *rv = (CdGDSFile*)(collect);
		return rv;
	} else
		return NULL;
}

void CdGDSObj::LoadBefore(CdSerial &Reader, TdVersion Version)
{
	// PipeInfo
	if (fPipeInfo) delete fPipeInfo;
	fPipeInfo = NULL;

	UTF8String coder;
	if (Reader["PIPE"] >> coder)
	{
		fPipeInfo = dStreamPipeMgr.Match(*this, coder.c_str());
		if (fPipeInfo==NULL)
        	throw ErrGDSObj("Invalid pipe coder: %s", coder.c_str());
		fPipeInfo->LoadStream(Reader, Version);
	}
}

void CdGDSObj::LoadAfter(CdSerial &Reader, TdVersion Version)
{
	CdObjMsg::LoadAfter(Reader, Version);
	fAttr.LoadAfter(Reader, Version);
}

void CdGDSObj::SaveBefore(CdSerial &Writer)
{
	if (fPipeInfo)
	{
		Writer["PIPE"] << fPipeInfo->Coder();
		fPipeInfo->SaveStream(Writer);
	}
}

void CdGDSObj::SaveAfter(CdSerial &Writer)
{
	CdObjMsg::SaveAfter(Writer);
	fAttr.SaveAfter(Writer);
}

void CdGDSObj::SaveStruct(CdSerial &Writer, bool IncludeName)
{
	CdObjMsg::SaveStruct(Writer, IncludeName);
	Writer.Truncate();
	fChanged = false;
}

void CdGDSObj::SaveToBlockStream()
{
	#ifdef COREARRAY_DEBUG_CODE
	_CheckGDSStream();
	#endif
	if (fGDSStream)
	{
		TdAutoRef<CdSerial> Writer(new CdSerial(fGDSStream));
		SaveStruct(*Writer, true);
	}
}

void CdGDSObj::GetPipeInfo() {}

void CdGDSObj::_CheckGDSStream()
{
	if (!fGDSStream)
		throw ErrGDSObj("GDSStream should not be NULL.");
}

void CdGDSObj::_RaiseInvalidAssign(const string &msg)
{
	if (msg.empty())
		throw ErrGDSObj("Invalid Assign operation.");
	else
		throw ErrGDSObj("Invalid Assign operation (%s).", msg.c_str());
}

void CdGDSObj::_GDSObjInitProc(CdObjClassMgr &Sender, CdObject *dObj, void *Data)
{
	if (dynamic_cast<CdGDSObj*>(dObj))
		static_cast<CdGDSObj*>(dObj)->fGDSStream = (CdBlockStream*)Data;
}


// CdGDSFolder

static const char *erNameExist = "The name \"%s\" exists.";
static const char *erFolderItem = "Invalid index %d.";
static const char *erFolderName = "Invalid name \"%s\".";
static const char *erNoFolderName = "There is not a folder named \"%s\".";
static const char *erObjItem = "Invalid index %d.";
static const char *erInvalidObj = "Invalid object!";
static const char *erInvalidCombine = "The object has been combined with a GDS file!";
static const char *erInvalidPath = "Invalid path \"%s\"!";

CdGDSFolder::CdGDSFolder(CdGDSFolder *vFolder): CdGDSObj(vFolder) {}

CdGDSFolder::~CdGDSFolder()
{
    _Clear();
}

CdGDSObj *CdGDSFolder::NewOne(void *Param)
{
    return new CdGDSFolder;
}

void CdGDSFolder::AssignOneEx(CdGDSFolder &Source)
{
	AssignOne(Source);
	for (int i=0; i < (int)Source.Count(); i++)
	{
		CdGDSObj *obj = Source.ObjItem(i);
		CdGDSObj *cp = obj->NewOne();
        AddObj(obj->Name(), cp);
		if (dynamic_cast<CdGDSFolder*>(obj))
		{
			dynamic_cast<CdGDSFolder*>(cp)->AssignOneEx(
            	*dynamic_cast<CdGDSFolder*>(obj));
		} else
        	cp->AssignOne(*obj);
	}
}

CdGDSFolder &CdGDSFolder::AddFolder(const UTF16String &Name)
{
	_CheckGDSStream();

	if (_HasName(Name))
		throw ErrGDSObj(erNameExist, UTF16toUTF8(Name).c_str());

	CdGDSFolder *rv = new CdGDSFolder(this);
	rv->fGDSStream = fGDSStream->Collection().NewBlockStream();
	rv->fGDSStream->AddRef();
	rv->fChanged = true;
	rv->AddRef();

	TItem I;
	I.Name = Name; I.Obj = rv;
	I.StreamID = rv->fGDSStream->ID();
	I.SetFolder();
	fList.push_back(I);
	fChanged = true;
	return *rv;
}

CdGDSObj *CdGDSFolder::AddObj(const UTF16String &Name, CdGDSObj *val)
{
	return InsertObj(-1, Name, val);
}

CdGDSObj *CdGDSFolder::InsertObj(int index, const UTF16String &Name,
	CdGDSObj *val)
{
	if ((index < -1) || (index > (int)fList.size()))
		throw ErrGDSObj("CdGDSFolder::InsertObj, invalid 'index' %d.", index);

	_CheckGDSStream();

	if (_HasName(Name))
		throw ErrGDSObj(erNameExist, UTF16toUTF8(Name).c_str());

	TItem I;
	if (val == NULL)
	{
		val = new CdGDSNull(this);
		I.SetEmpty();
	} else if (dynamic_cast<CdGDSFolder*>(val))
	{
		I.SetFolder();
	} else if (dynamic_cast<CdGDSNull*>(val))
	{
    	I.SetEmpty();
	}

	if (val->fGDSStream == NULL)
	{
		val->fGDSStream = fGDSStream->Collection().NewBlockStream();
		val->fGDSStream->AddRef();
		I.StreamID = val->fGDSStream->ID();
		val->SaveToBlockStream();
		val->AddRef();
	} else
		throw ErrGDSObj(erInvalidCombine);
	val->fFolder = this;

	I.Name = Name; I.Obj = val;
	if (index < 0)
		fList.push_back(I);
	else
		fList.insert(fList.begin()+index, I);
	fChanged = true;

	return val;
}

void CdGDSFolder::DeleteObj(int Index, bool force)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(erObjItem, Index);

	vector<TItem>::iterator it = fList.begin() + Index;
	if (it->Obj != NULL)
	{
		CdBlockStream *stream = it->Obj->fGDSStream;

		// check whether it is a folder
		if (dynamic_cast<CdGDSFolder*>(it->Obj))
		{
			CdGDSFolder *folder = static_cast<CdGDSFolder*>(it->Obj);
			if (!force && (folder->Count()>0))
			{
				throw ErrGDSObj(
					"Please delete the item(s) in the folder before removing it.");
			}
			folder->ClearObj(force);
		}

	#ifdef COREARRAY_DEBUG_CODE
		if (it->Obj->Release() != 0)
		{
			throw ErrGDSObj(
				"Internal Error: Object 'Release()' should return ZERO.");
		}
	#else
		it->Obj->Release();
	#endif
		if (fGDSStream && stream)
			fGDSStream->Collection().DeleteBlockStream(stream->ID());
	}
    fList.erase(it);

	fChanged = true;
}

void CdGDSFolder::DeleteObj(CdGDSObj *val, bool force)
{
	if (val == NULL) return;

	vector<CdGDSFolder::TItem>::iterator it;
	int Index = 0;
	for (it = fList.begin(); it != fList.end(); it++, Index++)
	{
		if (it->Obj == val)
		{
			DeleteObj(Index, force);
			return;
		}
	}
	throw ErrGDSObj();
}

void CdGDSFolder::ClearObj(bool force)
{
	vector<CdGDSObj *> lst;
	for (size_t i=0; i < fList.size(); i++)
		lst.push_back(ObjItem(i));

	for (size_t i=0; i < lst.size(); i++)
		DeleteObj(lst[i], force);	
}

CdGDSFolder & CdGDSFolder::DirItem(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(erFolderItem, Index);
	CdGDSFolder::TItem &I = fList[Index];
	_LoadItem(I);
	if (dynamic_cast<CdGDSFolder*>(I.Obj))
		return *static_cast<CdGDSFolder*>(I.Obj);
	else
    	throw ErrGDSObj(erNoFolderName, UTF16toUTF8(I.Name).c_str());
}

CdGDSFolder & CdGDSFolder::DirItem(const UTF16String &Name)
{
	CdGDSFolder::TItem &I = _NameItem(Name);
	_LoadItem(I);
	if (dynamic_cast<CdGDSFolder*>(I.Obj))
		return *static_cast<CdGDSFolder*>(I.Obj);
	else
    	throw ErrGDSObj(erNoFolderName, UTF16toUTF8(I.Name).c_str());
}

CdGDSObj * CdGDSFolder::ObjItem(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(erObjItem, Index);
	CdGDSFolder::TItem &I = fList[Index];
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj * CdGDSFolder::ObjItem(const UTF16String &Name)
{
	CdGDSFolder::TItem &I = _NameItem(Name);
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj * CdGDSFolder::ObjItemEx(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		return NULL;
	CdGDSFolder::TItem &I = fList[Index];
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj * CdGDSFolder::ObjItemEx(const UTF16String &Name)
{
	vector<CdGDSFolder::TItem>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		if (it->Name == Name)
		{
			_LoadItem(*it);
			return it->Obj;
		}
	}
	return NULL;
}

CdGDSObj * CdGDSFolder::Path(const UTF16String &FullName)
{
	CdGDSObj *rv = PathEx(FullName);
	if (!rv)
		throw ErrGDSObj(erInvalidPath, UTF16toUTF8(FullName).c_str());
	return rv;
}

CdGDSObj * CdGDSFolder::PathEx(const UTF16String &FullName)
{
	static const UTF16 delimit = '/';
	const UTF16 *p = FullName.c_str();

	if ((p[0] == delimit) && (p[1] == 0))
		return this;

	CdGDSObj *rv = this;
	while (*p)
	{
		if (!dynamic_cast<CdGDSFolder*>(rv))
			{ rv = NULL; break; }
		if (*p == delimit) p++;

		const UTF16 *s = p;
		while ((*p != delimit) && (*p != 0))
			p++;
		if (s == p)
			{ rv = NULL; break; }
		rv = static_cast<CdGDSFolder*>(rv)->
			ObjItemEx(UTF16String(s, p));
		if (rv == NULL) break;
	}
	return rv;
}

void CdGDSFolder::SplitPath(const UTF16String &FullName, UTF16String &Path,
	UTF16String &Name)
{
	static const UTF16 delimit = '/';
	size_t pos = FullName.find(delimit);
	if (pos == UTF16String::npos)
	{
		Path.clear();
		Name = FullName;
	} else {
		Path = FullName.substr(0, pos);
        Name = FullName.substr(pos+1, FullName.size()-pos-1);
    }
}

int CdGDSFolder::IndexObj(CdGDSObj *Obj)
{
	vector<CdGDSObj*> lst;
	for (size_t i=0; i < fList.size(); i++)
	{
		if (Obj == ObjItem(i))
			return i;
	}
	return -1;
}

bool CdGDSFolder::HasChild(CdGDSObj *Obj, bool SubFolder)
{
	if (Obj == NULL) return false;

	vector<TItem>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		if (it->Obj == Obj) return true;
		if (it->IsFolder() && SubFolder)
		{
			if (dynamic_cast<CdGDSFolder*>(it->Obj)->HasChild(Obj))
				return true;
        }
	}
	return false;
}

void CdGDSFolder::LoadAfter(CdSerial &Reader, TdVersion Version)
{
	fList.clear();

	Int32 L = 0;
	Reader["DIRCNT"] >> L;

	if (L > 0)
	{
		if (!Reader["DIRLIST"].rBuffer())
        	throw ErrGDSObj(erLoseVariable, "DIRLIST");
		for (Int32 k = 0; k < L; k++)
		{
			TItem I;
			Reader.rBeginNameSpace();
			{
				Reader.rInitNameSpace();
				Reader["ID"] >> I.StreamID;
				Reader["FLAG"] >> I.Flag;
				Reader["NAME"] >> I.Name;
			}
			Reader.rEndStruct();

			fList.push_back(I);
		}
		Reader.rEndStruct();
	}

	fAttr.LoadAfter(Reader, Version);
}

void CdGDSFolder::SaveAfter(CdSerial &Writer)
{
	Int32 L = fList.size();
	Writer["DIRCNT"] << L;

	if (L > 0)
	{
		Writer["DIRLIST"].wBuffer();
		{
			vector<TItem>::iterator it;
			for (it = fList.begin(); it != fList.end(); it++)
			{
				Writer.wBeginNameSpace();
				Writer["ID"] << it->StreamID;
				it->_pos = Writer.Position() - TdBlockID::size;
				Writer["FLAG"] << it->Flag;
				Writer["NAME"] << it->Name;
				Writer.wEndStruct();
			}
		}
		Writer.wEndStruct();
	}

	fAttr.SaveAfter(Writer);
}

void CdGDSFolder::SaveToBlockStream()
{
	#ifdef COREARRAY_DEBUG_CODE
	_CheckGDSStream();
	#endif
	if (fGDSStream)
	{
		TdAutoRef<CdSerial> Writer(new CdSerial(fGDSStream));
		SaveStruct(*Writer, false);
	}
}

void CdGDSFolder::_Clear()
{
	vector<CdGDSFolder::TItem>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		#ifdef COREARRAY_DEBUG_CODE
		if (it->Obj->Release() != 0)
			throw ErrGDSObj("Object Release() should return ZERO.");
		#else
		it->Obj->Release();
		#endif
		it->Obj = NULL;
	}
	fList.clear();
}

bool CdGDSFolder::_HasName(const UTF16String &Name)
{
	vector<CdGDSFolder::TItem>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Name == Name)
			return true;
	return false;
}

CdGDSFolder::TItem &CdGDSFolder::_NameItem(const UTF16String &Name)
{
	vector<CdGDSFolder::TItem>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Name == Name)
			return *it;
	throw ErrGDSObj(erFolderName, UTF16toUTF8(Name).c_str());
}

void CdGDSFolder::_LoadItem(TItem &I)
{
	if (I.Obj == NULL)
	{
		#ifdef COREARRAY_DEBUG_CODE
		_CheckGDSStream();
		#endif
		TdAutoRef<CdSerial> Reader(new CdSerial(
			fGDSStream->Collection()[I.StreamID]));

		if (I.IsFolder())
		{
			I.Obj = new CdGDSFolder(this);
			CdGDSFolder *vObj = static_cast<CdGDSFolder*>(I.Obj);

			Reader->rBeginNameSpace();
			vObj->LoadStruct(*Reader, 0x100);
			Reader->rEndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
			vObj->fGDSStream->AddRef();
		} else if (I.IsEmpty())
		{
			I.Obj = new CdGDSNull(this);
			CdGDSNull *vObj = static_cast<CdGDSNull*>(I.Obj);

			Reader->rBeginNameSpace();
			_Internal_::CdObject_LoadStruct(*vObj, *Reader, 0x100);
			Reader->rEndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
			vObj->fGDSStream->AddRef();
		} else {
			CdObjRef *obj =
				fGDSStream->Collection().ClassMgr()->
				toObj(*Reader, _GDSObjInitProc, fGDSStream);
			if (dynamic_cast<CdGDSObj*>(obj))
			{
				I.Obj = static_cast<CdGDSObj*>(obj);
				I.Obj->fFolder = this;
				I.Obj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
				I.Obj->fGDSStream->AddRef();
			} else {
				if (obj) delete obj;
				throw ErrGDSObj(erInvalidObj);
			}
		}

		I.Obj->AddRef();
	}
}

void CdGDSFolder::_UpdateAll()
{
	if (fChanged)
		SaveToBlockStream();

	vector<CdGDSFolder::TItem>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		if (it->Obj)
		{
			if (dynamic_cast<CdGDSFolder*>(it->Obj))
				static_cast<CdGDSFolder*>(it->Obj)->_UpdateAll();
			else
				if (it->Obj->fChanged) it->Obj->SaveToBlockStream();
		}
	}
}

vector<CdGDSFolder::TItem>::iterator CdGDSFolder::_FindObj(CdGDSObj *Obj)
{
	vector<TItem>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Obj == Obj) return it;
	return fList.end();
}


// CdGDSNull

CdGDSNull::CdGDSNull(CdGDSFolder *vFolder): CdGDSObj(vFolder) {}

CdGDSObj *CdGDSNull::NewOne(void *Param)
{
	return new CdGDSNull();
}

void CdGDSNull::SaveToBlockStream()
{
	#ifdef COREARRAY_DEBUG_CODE
	_CheckGDSStream();
	#endif
	if (fGDSStream)
	{
		TdAutoRef<CdSerial> Writer(new CdSerial(fGDSStream));
		SaveStruct(*Writer, false);
	}
}


// CdGDSStreamContainer

CdGDSStreamContainer::CdGDSStreamContainer(CdGDSFolder *vFolder)
{
	fBufStream = new CBufdStream(new CdMemoryStream);
	fBufStream->AddRef();
	vAlloc_Stream = NULL;
	fNeedUpdate = false;
	vAllocID = 0;
	vAlloc_Ptr = 0;
}

CdGDSStreamContainer::~CdGDSStreamContainer()
{
	CloseWriter();
	fBufStream->Release();
}

char const* CdGDSStreamContainer::dName()
{
	return "dStream";
}

char const* CdGDSStreamContainer::dTraitName()
{
	return "Stream";
}

CdGDSObj *CdGDSStreamContainer::NewOne(void *Param)
{
	CdGDSStreamContainer *rv = new CdGDSStreamContainer;
	if (fPipeInfo)
		rv->fPipeInfo = fPipeInfo->NewOne();
	return rv;
}

void CdGDSStreamContainer::AssignOne(CdGDSObj &Source, void *Param)
{
	CdGDSObj::AssignOne(Source, Param);
	if (dynamic_cast<CdGDSStreamContainer*>(&Source))
	{
		CdGDSStreamContainer &S = *static_cast<CdGDSStreamContainer*>(&Source);
		S.CloseWriter();
		S.CopyTo(*BufStream());
		CloseWriter();
	}
}

void CdGDSStreamContainer::LoadAfter(CdSerial &Reader, TdVersion Version)
{
	if (fGDSStream != NULL)
	{
		vAllocID = 0;
		Reader["DATA"] >> vAllocID;
		vAlloc_Ptr = Reader.NamePosition("DATA");

        fBufStream->Release();
		fBufStream = new CBufdStream(fGDSStream->Collection()[vAllocID]);
		fBufStream->AddRef();

		if (fPipeInfo)
        	fPipeInfo->PushReadPipe(*fBufStream);
	}
	#ifdef COREARRAY_DEBUG_CODE
	else
		throw ErrGDSStreamContainer("This object should be combined with a GDS file!");
	#endif

	CdGDSObj::LoadAfter(Reader, Version);
}

void CdGDSStreamContainer::SaveAfter(CdSerial &Writer)
{
	if (fGDSStream != NULL)
	{
		TdBlockID Entry = 0;
		if (vAlloc_Stream)
			Entry = vAlloc_Stream->ID();
		Writer["DATA"] << Entry;
		vAlloc_Ptr = Writer.Position() - TdBlockID::size;
	}
	#ifdef COREARRAY_DEBUG_CODE
	else
		throw ErrGDSStreamContainer("This object should be combined with a GDS file!");
	#endif

	CdGDSObj::SaveAfter(Writer);
}

void CdGDSStreamContainer::SaveStruct(CdSerial &Writer, bool IncludeName)
{
	CdGDSObj::SaveStruct(Writer, IncludeName);

	if ((vAlloc_Stream==NULL) && (fGDSStream!=NULL))
	{
		#ifdef COREARRAY_DEBUG_CODE
		_CheckGDSStream();
		if (vAlloc_Ptr <= 0)
			throw ErrGDSStreamContainer("Invalid vAlloc_Ptr.");
		#endif
		{
			// Modify TdBlockID
			vAlloc_Stream = fGDSStream->Collection().NewBlockStream();
			Writer.SetPosition(vAlloc_Ptr);
			Writer << TdBlockID(vAlloc_Stream->ID());
			Writer.FlushWrite();

			TdAutoRef<CBufdStream> M(new CBufdStream(vAlloc_Stream));
			if (fPipeInfo)
				fPipeInfo->PushWritePipe(*M);
			M->CopyFrom(*fBufStream);

			fBufStream->Release();
			fBufStream = M.get();
			fBufStream->AddRef();
		}
	}
}

TdPtr64 CdGDSStreamContainer::GetSize()
{
	if (fPipeInfo)
		return fPipeInfo->StreamTotalIn();
	else
    	return fBufStream->GetSize();
}

void CdGDSStreamContainer::SetPackedMode(const char *Mode)
{
	static const char *erMethod = "Invalid compression method '%s'.";

	if (fPipeInfo ? (!fPipeInfo->Equal(Mode)) : true)
	{
		if (vAlloc_Stream && fGDSStream)
		{
			Synchronize();

			if (fPipeInfo) delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (*Mode!=0))
				throw ErrGDSStreamContainer(erMethod, Mode);

			{
				TdAutoRef<CdTempStream> Temp(new CdTempStream(""));
				{
					TdAutoRef<CBufdStream> Buf(new CBufdStream(Temp.get()));
					Buf->CopyFrom(*fBufStream);
					if (fPipeInfo)
						_GetStreamPipeInfo(Buf.get(), true);
				}
				vAlloc_Stream->SetPosition(0);
				vAlloc_Stream->SetSizeOnly(0);
				vAlloc_Stream->CopyFrom(*Temp);
				vAlloc_Stream->SetPosition(0);
			}
			// New Filter
			{
                fBufStream->Release();
				fBufStream = new CBufdStream(vAlloc_Stream);
            	fBufStream->AddRef();
				if (fPipeInfo)
					fPipeInfo->PushReadPipe(*fBufStream);
			}
			// Save self
			SaveToBlockStream();
		} else {
			if (fPipeInfo)
				delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (*Mode!=0))
				throw ErrGDSStreamContainer(erMethod, Mode);
		}
	}
}

void CdGDSStreamContainer::CloseWriter()
{
	fBufStream->OnFlush.Clear();
	fBufStream->FlushWrite();
	if (fPipeInfo && vAlloc_Stream)
	{
		if (fPipeInfo->WriteMode(*fBufStream))
		{
			fPipeInfo->ClosePipe(*fBufStream);
			if (_GetStreamPipeInfo(fBufStream, false))
				_UpdateStreamPipeInfo(*fGDSStream);

			fBufStream->Release();
			vAlloc_Stream->SetPosition(0);
			fBufStream = new CBufdStream(vAlloc_Stream);
        	fBufStream->AddRef();
			if (fPipeInfo)
				fPipeInfo->PushReadPipe(*fBufStream);
		}
	}
}

void CdGDSStreamContainer::CopyFrom(CBufdStream &Source, TdPtr64 Count)
{
	char Buffer[0x10000];
	ssize_t N;

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	while (Count > 0)
	{
		N = (Count >= (TdPtr64)sizeof(Buffer)) ? (ssize_t)sizeof(Buffer) : Count;
		Source.Read((void*)Buffer, N);
		fBufStream->Write((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyFrom(CdStream &Source, TdPtr64 Count)
{
	char Buffer[0x10000];
	ssize_t N;

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	while (Count > 0)
	{
		N = (Count >= (TdPtr64)sizeof(Buffer)) ? (ssize_t)sizeof(Buffer) : Count;
		Source.ReadBuffer((void*)Buffer, N);
		fBufStream->Write((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyTo(CBufdStream &Dest, TdPtr64 Count)
{
	char Buffer[0x10000];
	ssize_t N;

	if (Count < 0)
	{
		fBufStream->SetPosition(0);
		Dest.SetPosition(0);
		Count = GetSize();
	}
	while (Count > 0)
	{
		N = (Count >= (TdPtr64)sizeof(Buffer)) ? (ssize_t)sizeof(Buffer) : Count;
		fBufStream->Read((void*)Buffer, N);
		Dest.Write((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyTo(CdStream &Dest, TdPtr64 Count)
{
	char Buffer[0x10000];
	ssize_t N;

	if (Count < 0)
	{
		fBufStream->SetPosition(0);
		Dest.SetPosition(0);
		Count = GetSize();
	}
	while (Count > 0)
	{
		N = (Count >= (TdPtr64)sizeof(Buffer))? (ssize_t)sizeof(Buffer) : Count;
		fBufStream->Read((void*)Buffer, N);
		Dest.WriteBuffer((void*)Buffer, N);
		Count -= N;
	}
}


// CdGDSFile

static const char *erGDSInvalidOpenMode = "Invalid open mode in CdGDSFile.";
static const char *erInvalidPrefix = "Invalid prefix of stream!";
static const char *erEntryError = "Invalid entry point(%d).";
static const char *erSaveStream = "Should save it to a GDS file first!";

#ifdef COREARRAY_DEBUG_CODE
static const char *erFStream = "fStream should be NULL.";
#endif

void CdGDSFile::_Init()
{
	fRoot.AddRef();
	fPrefix = "COREARRAYx0A";
	fCodeStart = strlen(fPrefix) + sizeof(fVersion) + TdBlockID::size;
	fVersion = COREARRAY_VERSION;
	fReadOnly = false;
	fLog = new CdLogRecord; fLog->AddRef();
}

CdGDSFile::CdGDSFile() { _Init(); }

CdGDSFile::CdGDSFile(const UTF16String &fn, TdOpenMode Mode)
{
	_Init();
	switch (Mode)
	{
		case dmCreate:
			SaveAsFile(fn); break;
		case dmOpenRead:
			LoadFile(fn, true); break;
		case dmOpenReadWrite:
			LoadFile(fn, false); break;
		default:
			throw ErrGDSFile(erGDSInvalidOpenMode);
	}
}

CdGDSFile::CdGDSFile(const char *fn, TdOpenMode Mode)
{
	_Init();
	switch (Mode)
	{
		case dmCreate:
			SaveAsFile(fn); break;
		case dmOpenRead:
			LoadFile(fn, true); break;
		case dmOpenReadWrite:
			LoadFile(fn, false); break;
		default:
			throw ErrGDSFile(erGDSInvalidOpenMode);
	}
}

CdGDSFile::~CdGDSFile()
{
	CloseFile();
#ifdef COREARRAY_DEBUG_CODE
	if (fLog->Release() != 0)
		throw Err_dFilter("Log::Release() should return 0 here.");
#else
	fLog->Release();
#endif
}

void CdGDSFile::LoadStream(CdStream* Stream, bool ReadOnly)
{
	// Initialize
	CloseFile();
	fLog->List().clear();
	fReadOnly = ReadOnly;

	// Check the prefix
	size_t L = strlen(fPrefix);
	auto_ptr<char> buf(new char[L]);
	Stream->ReadBuffer((void*)(buf.get()), L);
	if (memcmp((void*)fPrefix, (void*)(buf.get()), L) !=0)
		throw ErrGDSFile(erInvalidPrefix);

	// Load Version
	fVersion = Stream->rUInt8();
	fVersion |= Stream->rUInt8() << 8;

	// The entry of stream ID
	TdBlockID Entry;
	*Stream >> Entry;

	// To identify the block stream
	CdBlockCollection::LoadStream(Stream, ReadOnly);

	if (HaveID(Entry))
	{
		fRoot.fGDSStream = (*this)[Entry];
		fRoot.fGDSStream->AddRef();

		TdAutoRef<CdSerial> Reader(new CdSerial(fRoot.fGDSStream));
		Reader->rBeginNameSpace();
		_Internal_::CdObject_LoadStruct(fRoot, *Reader, fVersion);
		Reader->rEndStruct();
	} else
		throw ErrGDSFile(erEntryError, Entry.get());
}

void CdGDSFile::SaveStream(CdStream *Stream)
{
	#ifdef COREARRAY_DEBUG_CODE
	if (fStream != NULL)
		throw ErrGDSFile(erFStream);
	#endif

	// Save Prefix
	Stream->WriteBuffer((void*)fPrefix, strlen(fPrefix));

	// Save Ver
	Stream->wUInt8(fVersion & 0xFF);
    Stream->wUInt8(fVersion >> 8);

	TdPtr64 _EntryPos = Stream->Position();
	*Stream << TdBlockID(0);

	CdBlockCollection::WriteStream(Stream);
	fRoot.fGDSStream = NewBlockStream();
	fRoot.fGDSStream->AddRef();
	TdPtr64 _NewPos = Stream->Position();

	Stream->SetPosition(_EntryPos);
	*Stream << fRoot.fGDSStream->ID();
	Stream->SetPosition(_NewPos);

	fRoot.SaveToBlockStream();
}

void CdGDSFile::LoadFile(const UTF16String &fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdFileStream(UTF16toUTF8(fn).c_str(),
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = fn;
}

void CdGDSFile::LoadFile(const char *fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdFileStream(fn,
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = UTF8toUTF16(fn);
}

void CdGDSFile::SyncFile()
{
	if (fStream == NULL)
		throw ErrGDSFile(erSaveStream);
	fRoot._UpdateAll();
}

void CdGDSFile::SaveAsFile(const UTF16String &fn)
{
	TdAutoRef<CdStream> F(new CdFileStream(UTF16toUTF8(fn).c_str(),
		CdFileStream::fmCreate));
	fFileName = fn;
	SaveStream(F.get());
}

void CdGDSFile::SaveAsFile(const char *fn)
{
	TdAutoRef<CdStream> F(new CdFileStream(fn, CdFileStream::fmCreate));
	fFileName = UTF8toUTF16(fn);
	SaveStream(F.get());
}

void CdGDSFile::DuplicateFile(const UTF16String &fn, bool deep)
{
	if (deep)
	{
		CdGDSFile file(fn, CdGDSFile::dmCreate);
		file.Root().AssignOneEx(Root());
	} else {
		// create a new file
		TdAutoRef<CdStream> F(new CdFileStream(UTF16toUTF8(fn).c_str(),
			CdFileStream::fmCreate));

		// Save Prefix
		F->WriteBuffer((void*)fPrefix, strlen(fPrefix));
		// Save Version
		F->wUInt8(fVersion & 0xFF);
		F->wUInt8(fVersion >> 8);
		// Save Entry ID
		(*F) << fRoot.fGDSStream->ID();

		// for-loop for all stream blocks
		for (int i=0; i < (int)fBlockList.size(); i++)
		{
			TdPosType bSize = fBlockList[i]->Size();
			TdPosType sSize = (2*TdPosType::size +
				CdBlockStream::TBlockInfo::HeadSize + bSize) |
				GDS_STREAM_POS_MASK_HEAD_BIT;
			TdPosType sNext = 0;
			(*F) << sSize << sNext << fBlockList[i]->ID() << bSize;
			F->CopyFrom(*fBlockList[i]);
		}
	}
}

void CdGDSFile::DuplicateFile(const char *fn, bool deep)
{
	DuplicateFile(PChartoUTF16(fn), deep);
}

void CdGDSFile::CloseFile()
{
	if (fStream)
	{
		SyncFile();
		fFileName.clear();
		fLog->List().clear();
		fRoot._Clear();
		fRoot.fGDSStream->Release();
		fRoot.fGDSStream = NULL;
		CdBlockCollection::Clear();
    }
}

void CdGDSFile::TidyUp(bool deep)
{
	UTF16String fn, f;
	fn = fFileName;
	f = fn + UTF8toUTF16(".tmp");
	DuplicateFile(f, deep);
	CloseFile();
	remove(UTF16toUTF8(fn).c_str());
	rename(UTF16toUTF8(f).c_str(), UTF16toUTF8(fn).c_str());
	LoadFile(fn, fReadOnly);
}

bool CdGDSFile::_HaveModify(CdGDSFolder *folder)
{
	if (folder->fChanged) return true;

	vector<CdGDSFolder::TItem>::iterator it;
	for (it = folder->fList.begin(); it != folder->fList.end(); it++)
	{
		if (it->Obj)
		{
			if (dynamic_cast<CdGDSFolder*>(it->Obj))
			{
				if (_HaveModify(static_cast<CdGDSFolder*>(it->Obj)))
					return true;
			} else
				if (it->Obj->fChanged) return true;
		}
	}
	return false;
}

bool CdGDSFile::Modified()
{
	return _HaveModify(&fRoot);
}

TdPtr64 CdGDSFile::GetFileSize()
{
    return fStreamSize;
}

int CdGDSFile::GetNumOfFragment()
{
	return CdBlockCollection::NumOfFragment();
}
