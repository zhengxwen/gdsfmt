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

#include <dFile.h>
#include <algorithm>
#include <cstdio>


namespace CoreArray
{
	// CdStreamPipeMgr

	/// The pipe for reading data from a ZIP stream
	class COREARRAY_DLL_DEFAULT CdZIPReadPipe: public CdStreamPipe
	{
	public:
		CdZIPReadPipe(): CdStreamPipe() {}

	protected:
		virtual CdStream* InitPipe(CdBufStream *Filter)
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
			if (fPStream)
				fPStream->Release();
			return fStream;
		}

	private:
		CdStream* fStream;
		CdZIPInflate* fPStream;
	};


	/// The pipe for writing data to a ZIP stream
	class COREARRAY_DLL_DEFAULT CdZIPWritePipe: public CdStreamPipe
	{
	public:
		CdZIPWritePipe(CdZIPDeflate::TZLevel vLevel,
			TdCompressRemainder &vRemainder):
			CdStreamPipe(), fRemainder(vRemainder)
			{ fLevel = vLevel; }

		COREARRAY_INLINE CdZIPDeflate::TZLevel Level() const { return fLevel; };
		COREARRAY_INLINE CdStream* Stream() const { return fStream; };
		COREARRAY_INLINE CdStream* StreamZIP() const { return fPStream; };

	protected:
		virtual CdStream* InitPipe(CdBufStream *Filter)
		{
			fStream = Filter->Stream();
			fPStream = new CdZIPDeflate(fStream, fLevel);
			fPStream->PtrExtRec = &fRemainder;
			return fPStream;
		}
		virtual CdStream* FreePipe()
		{
			if (fPStream)
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

	class COREARRAY_DLL_DEFAULT CdPipeZIP: public CdPipeMgrItem
	{
	public:
		CdPipeZIP(CdZIPDeflate::TZLevel level = CdZIPDeflate::zcDefault):
			CdPipeMgrItem()
			{ vLevel = level; fSizeInfo_Ptr = -1; }

		virtual CdPipeMgrItem *NewOne()
			{ return new CdPipeZIP(vLevel); }
		virtual const char *Coder() const
			{ return "ZIP"; };
		virtual const char *Description() const
			{ return "Version: " ZLIB_VERSION; }
		virtual bool Equal(const char *Mode) const
			{ return Which(Mode, ZIPStrings, 4) >= 0; }
		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZIPReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZIPWritePipe(vLevel, fRemainder)); }
		virtual void PopPipe(CdBufStream &buf)
			{ buf.PopPipe(); }
		virtual bool WriteMode(CdBufStream &buf) const
			{ return dynamic_cast<CdZIPDeflate*>(buf.Stream())!=NULL; }
		virtual void ClosePipe(CdBufStream &buf)
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
		virtual bool GetStreamInfo(CdBufStream *buf)
		{
			SIZE64 in, out;

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
				C_Int64 Ary[2] = { fStreamTotalIn, fStreamTotalOut };
				Stream.SetPosition(fSizeInfo_Ptr);
                Stream.WriteBuffer((void*)Ary, sizeof(Ary));
            }
		}
		virtual void LoadStream(CdSerial &Reader, TdVersion Version)
		{
			C_Int64 Ary[2];
			if (Reader["PIPE_SIZE"].rShortBuf(Ary, 2))
			{
				fSizeInfo_Ptr = Reader.Position();
				fStreamTotalIn = Ary[0];
				fStreamTotalOut = Ary[1];
			} else {
				fSizeInfo_Ptr = -1;
				fStreamTotalIn = fStreamTotalOut = -1;
			}
			C_UInt8 I = 0;
			Reader["PIPE_LEVEL"] >> I;
			if (I > 3)
                throw ErrGDSObj("Invalid 'PIPE_LEVEL %d'", I);
			vLevel = (CdZIPDeflate::TZLevel)I;
		}
		virtual void SaveStream(CdSerial &Writer)
		{
        	UpdateStreamSize();
			C_Int64 Ary[2] = { fStreamTotalIn, fStreamTotalOut };
			Writer["PIPE_SIZE"].wShortBuf(Ary, 2);
			fSizeInfo_Ptr = Writer.Position() - sizeof(Ary);
			C_UInt8 I = vLevel;
			Writer["PIPE_LEVEL"] << I;
		}

	private:
		CdZIPDeflate::TZLevel vLevel;
		SIZE64 fSizeInfo_Ptr;
	};


	CdStreamPipeMgr dStreamPipeMgr;
}

using namespace std;
using namespace CoreArray;


// CdPipeMgrItem

CdPipeMgrItem::CdPipeMgrItem(): CdAbstractItem()
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

CdStreamPipeMgr::CdStreamPipeMgr(): CdAbstractManager()
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

CdObjAttr::CdObjAttr(CdGDSObj &vOwner): CdObject(), fOwner(vOwner)
{ }

CdObjAttr::~CdObjAttr()
{
	vector<TdPair*>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		TdPair *p = *it;
		*it = NULL;
		delete p;
	}
}

void CdObjAttr::Assign(CdObjAttr &Source)
{
	/// \todo Assign procedure
	Clear();
	for (int i=0; i < (int)Source.Count(); i++)
		Add(Source.Names(i)) = Source[i];
    Changed();
}

TdsAny &CdObjAttr::Add(const UTF16String &Name)
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
		throw ErrGDSObj(rsAttrNameExist, UTF16ToUTF8(Name).c_str());
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
		throw ErrGDSObj(rsAttrName, UTF16ToUTF8(Name).c_str());
	TdPair *p = *it;
	*it = NULL;
	fList.erase(it);
	delete p;
	Changed();
}

void CdObjAttr::Delete(int Index)
{
	fList.at(Index); // check range
	TdPair *p = fList[Index];
	fList[Index] = NULL;
    fList.erase(fList.begin() + Index);
	delete p;
	Changed();
}

void CdObjAttr::Clear()
{
	if (!fList.empty())
	{
		vector<TdPair*>::iterator it;
		for (it = fList.begin(); it != fList.end(); it++)
		{
			TdPair *p = *it;
			*it = NULL;
			delete p;
		}
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
		throw ErrGDSObj(rsAttrName, UTF16ToUTF8(Name).c_str());
	return (*it)->val;
}

TdsAny & CdObjAttr::operator[](int Index)
{
	return fList.at(Index)->val;
}

void CdObjAttr::LoadAfter(CdSerial &Reader, const TdVersion Version)
{
	C_Int32 Cnt;
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
	C_Int32 Cnt = fList.size();
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
	{
		if ((*it)->name == Name)
			break;
	}
	return it;
}

void CdObjAttr::SetName(const UTF16String &OldName, const UTF16String &NewName)
{
	xValidateName(NewName);
	vector<TdPair*>::iterator it = Find(OldName);
	if (it == fList.end())
		throw ErrGDSObj(rsAttrName, UTF16ToUTF8(OldName).c_str());
	if (OldName != NewName)
	{
		if (HasName(NewName) >= 0)
			throw ErrGDSObj(rsAttrNameExist, UTF16ToUTF8(NewName).c_str());
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
			throw ErrGDSObj(rsAttrNameExist, UTF16ToUTF8(NewName).c_str());
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

CdGDSObj::CdGDSObj(): CdObjMsg(), fAttr(*this)
{
	fFolder = NULL;
	fGDSStream = NULL;
	fPipeInfo = NULL;
	fChanged = false;
}

CdGDSObj::~CdGDSObj()
{
	if (fPipeInfo)
		delete fPipeInfo;
	if (fGDSStream)
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
		vector<CdGDSFolder::TNode>::const_iterator it;
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
		vector<CdGDSFolder::TNode>::iterator it;
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
				vector<CdGDSFolder::TNode>::iterator it;
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

void CdGDSObj::GetOwnBlockStream(vector<const CdBlockStream*> &Out)
{
	Out.clear();
}

CdGDSFile *CdGDSObj::GDSFile()
{
	if (fGDSStream)
	{
		CdBlockCollection *collect = &fGDSStream->Collection();
		// static_cast, not dynamic_cast
		// because 'cannot cast protected base class'
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
	#ifdef COREARRAY_CODE_DEBUG
	_CheckGDSStream();
	#endif

	if (fGDSStream)
	{
		TdAutoRef<CdSerial> Writer(new CdSerial(fGDSStream));
		SaveStruct(*Writer, IsWithClassName());
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


// CdGDSLabel

CdGDSObj *CdGDSLabel::NewOne(void *Param)
{
	return new CdGDSLabel;
}


// CdGDSFolder

static const char *erNameExist = "The GDS node \"%s\" exists.";
static const char *erFolderItem = "Invalid index %d.";
static const char *erFolderName = "Invalid node name \"%s\".";
static const char *erNoFolderName = "There is not a folder named \"%s\".";
static const char *erObjItem = "Invalid index %d.";
static const char *erInvalidCombine = "The object has been combined with a GDS file!";
static const char *erInvalidPath = "The GDS node \"%s\" does not exist.";


CdGDSFolder::TNode::TNode()
{
	Obj = NULL;
	StreamID = 0;
	Flag = FLAG_TYPE_CLASS;
	_pos = 0;
}

bool CdGDSFolder::TNode::IsFlagType(C_UInt32 val) const
{
	return (Flag & FLAG_TYPE_MASK) == (val & FLAG_TYPE_MASK);
}

void CdGDSFolder::TNode::SetFlagType(C_UInt32 val)
{
	Flag &= ~FLAG_TYPE_MASK;
	Flag |= (val & FLAG_TYPE_MASK);
}


CdGDSFolder::~CdGDSFolder()
{
    _ClearFolder();
}

CdGDSObj *CdGDSFolder::NewOne(void *Param)
{
    return new CdGDSFolder;
}

void CdGDSFolder::AssignOneEx(CdGDSFolder &Source)
{
	AssignOne(Source);
	for (int i=0; i < Source.NodeCount(); i++)
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

CdGDSObj *CdGDSFolder::AddFolder(const UTF16String &Name)
{
	_CheckGDSStream();

	if (_HasName(Name))
		throw ErrGDSObj(erNameExist, UTF16ToUTF8(Name).c_str());

	CdGDSFolder *rv = new CdGDSFolder;
	rv->fFolder = this;
	rv->fGDSStream = fGDSStream->Collection().NewBlockStream();
	rv->fGDSStream->AddRef();
	rv->fChanged = true;
	rv->AddRef();

	TNode I;
	I.Name = Name; I.Obj = rv;
	I.StreamID = rv->fGDSStream->ID();
	I.SetFlagType(CdGDSFolder::TNode::FLAG_TYPE_FOLDER);
	fList.push_back(I);
	fChanged = true;

	return rv;
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
		throw ErrGDSObj(erNameExist, UTF16ToUTF8(Name).c_str());

	TNode I;
	if (val == NULL)
	{
		val = new CdGDSLabel;
		I.SetFlagType(CdGDSFolder::TNode::FLAG_TYPE_LABEL);
	} else if (dynamic_cast<CdGDSLabel*>(val))
	{
		I.SetFlagType(CdGDSFolder::TNode::FLAG_TYPE_LABEL);
	} else if (dynamic_cast<CdGDSFolder*>(val))
	{
		I.SetFlagType(CdGDSFolder::TNode::FLAG_TYPE_FOLDER);
	} else if (dynamic_cast<CdGDSVirtualFolder*>(val))
	{
		I.SetFlagType(CdGDSFolder::TNode::FLAG_TYPE_VIRTUAL_FOLDER);
	} else if (dynamic_cast<CdGDSStreamContainer*>(val))
	{
		I.SetFlagType(CdGDSFolder::TNode::FLAG_TYPE_STREAM);
	}

	val->fFolder = this;

	if (val->fGDSStream == NULL)
	{
		val->fGDSStream = fGDSStream->Collection().NewBlockStream();
		val->fGDSStream->AddRef();
		I.StreamID = val->fGDSStream->ID();
		val->SaveToBlockStream();
		val->AddRef();
	} else
		throw ErrGDSObj(erInvalidCombine);

	I.Name = Name; I.Obj = val;
	if (index < 0)
		fList.push_back(I);
	else
		fList.insert(fList.begin()+index, I);
	fChanged = true;

	return val;
}

void CdGDSFolder::MoveTo(int Index, int NewPos)
{
	if ((Index < -1) || (Index >= (int)fList.size()))
		throw ErrGDSObj("CdGDSFolder::MoveTo, invalid 'Index' %d.", Index);
	if ((NewPos < -1) || (NewPos >= (int)fList.size()))
		throw ErrGDSObj("CdGDSFolder::MoveTo, invalid 'NewPos' %d.", NewPos);

	if (Index != NewPos)
	{
		TNode ND = fList[Index];
		if (NewPos >= (int)fList.size()-1)
		{
			fList.erase(fList.begin() + Index);
			fList.push_back(ND);
		} else {
			fList.erase(fList.begin() + Index);
			fList.insert(fList.begin() + NewPos, ND);
		}

		fChanged = true;
	}
}

void CdGDSFolder::DeleteObj(int Index, bool force)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(erObjItem, Index);

	vector<TNode>::iterator it = fList.begin() + Index;
	if (it->Obj != NULL)
	{
		CdBlockStream *stream = it->Obj->fGDSStream;

		vector<const CdBlockStream*> BL;
		it->Obj->GetOwnBlockStream(BL);

		// check whether it is a folder
		if (dynamic_cast<CdGDSFolder*>(it->Obj))
		{
			CdGDSFolder *folder = static_cast<CdGDSFolder*>(it->Obj);
			if (!force && (folder->NodeCount()>0))
			{
				throw ErrGDSObj(
					"Please delete the item(s) in the folder before removing it.");
			}
			folder->ClearObj(force);
		}

	#ifdef COREARRAY_CODE_DEBUG
		if (it->Obj->Release() != 0)
		{
			throw ErrGDSObj(
				"Internal Error: Object 'Release()' should return ZERO.");
		}
	#else
		it->Obj->Release();
	#endif

		if (fGDSStream)
		{
			if (stream)
				fGDSStream->Collection().DeleteBlockStream(stream->ID());

			vector<const CdBlockStream*>::iterator it;
			for (it=BL.begin(); it != BL.end(); it++)
			{
				fGDSStream->Collection().DeleteBlockStream((*it)->ID());
			}
		}
	}
    fList.erase(it);

	fChanged = true;
}

void CdGDSFolder::DeleteObj(CdGDSObj *val, bool force)
{
	if (val == NULL) return;

	vector<CdGDSFolder::TNode>::iterator it;
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
	CdGDSFolder::TNode &I = fList[Index];
	_LoadItem(I);
	if (dynamic_cast<CdGDSFolder*>(I.Obj))
		return *static_cast<CdGDSFolder*>(I.Obj);
	else
    	throw ErrGDSObj(erNoFolderName, UTF16ToUTF8(I.Name).c_str());
}

CdGDSFolder & CdGDSFolder::DirItem(const UTF16String &Name)
{
	CdGDSFolder::TNode &I = _NameItem(Name);
	_LoadItem(I);
	if (dynamic_cast<CdGDSFolder*>(I.Obj))
		return *static_cast<CdGDSFolder*>(I.Obj);
	else
    	throw ErrGDSObj(erNoFolderName, UTF16ToUTF8(I.Name).c_str());
}

CdGDSObj * CdGDSFolder::ObjItem(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(erObjItem, Index);
	CdGDSFolder::TNode &I = fList[Index];
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj * CdGDSFolder::ObjItem(const UTF16String &Name)
{
	CdGDSFolder::TNode &I = _NameItem(Name);
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj * CdGDSFolder::ObjItemEx(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		return NULL;
	CdGDSFolder::TNode &I = fList[Index];
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj * CdGDSFolder::ObjItemEx(const UTF16String &Name)
{
	vector<CdGDSFolder::TNode>::iterator it;
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
		throw ErrGDSObj(erInvalidPath, UTF16ToUTF8(FullName).c_str());
	return rv;
}

CdGDSObj * CdGDSFolder::PathEx(const UTF16String &FullName)
{
	static const UTF16 delimit = '/';
	const UTF16 *p = FullName.c_str();

	CdGDSObj *rv = this;
	while ((*p) && (rv))
	{
		if (!dynamic_cast<CdGDSAbsFolder*>(rv))
			return NULL;
		if (*p == delimit) p ++;

		const UTF16 *s = p;
		while ((*p != delimit) && (*p != 0))
			p ++;
		if (s == p)
			return rv;
		rv = ((CdGDSAbsFolder*)rv)->ObjItemEx(UTF16String(s, p));
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

	vector<TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		if (it->Obj == Obj) return true;
		if (dynamic_cast<CdGDSAbsFolder*>(it->Obj) && SubFolder)
		{
			if (dynamic_cast<CdGDSAbsFolder*>(it->Obj)->HasChild(Obj))
				return true;
        }
	}
	return false;
}

int CdGDSFolder::NodeCount()
{
	return fList.size();
}

void CdGDSFolder::LoadAfter(CdSerial &Reader, TdVersion Version)
{
	fList.clear();

	C_Int32 L = 0;
	Reader["DIRCNT"] >> L;

	if (L > 0)
	{
		if (!Reader["DIRLIST"].rBuffer())
        	throw ErrGDSObj(erLoseVariable, "DIRLIST");
		for (C_Int32 k = 0; k < L; k++)
		{
			TNode I;
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
	C_Int32 L = fList.size();
	Writer["DIRCNT"] << L;

	if (L > 0)
	{
		Writer["DIRLIST"].wBuffer();
		{
			vector<TNode>::iterator it;
			for (it = fList.begin(); it != fList.end(); it++)
			{
				Writer.wBeginNameSpace();
				Writer["ID"] << it->StreamID;
				it->_pos = Writer.Position() - TdBlockID::Size;
				Writer["FLAG"] << it->Flag;
				Writer["NAME"] << it->Name;
				Writer.wEndStruct();
			}
		}
		Writer.wEndStruct();
	}

	fAttr.SaveAfter(Writer);
}

void CdGDSFolder::_ClearFolder()
{
	vector<CdGDSFolder::TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		if (it->Obj)
		{
		#ifdef COREARRAY_CODE_DEBUG
			if (it->Obj->Release() != 0)
				throw ErrGDSObj("Object Release() should return ZERO.");
		#else
			it->Obj->Release();
		#endif
		}
	}
	fList.clear();
}

bool CdGDSFolder::_HasName(const UTF16String &Name)
{
	vector<CdGDSFolder::TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Name == Name)
			return true;
	return false;
}

CdGDSFolder::TNode &CdGDSFolder::_NameItem(const UTF16String &Name)
{
	vector<CdGDSFolder::TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Name == Name)
			return *it;
	throw ErrGDSObj(erFolderName, UTF16ToUTF8(Name).c_str());
}

void CdGDSFolder::_LoadItem(TNode &I)
{
	if (I.Obj == NULL)
	{
		#ifdef COREARRAY_CODE_DEBUG
		_CheckGDSStream();
		#endif

		TdAutoRef<CdSerial> Reader(new CdSerial(
			fGDSStream->Collection()[I.StreamID]));

		if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_LABEL))
		{
			// it is a label
			CdGDSLabel *vObj = new CdGDSLabel;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader->rBeginNameSpace();
			_INTERNAL::CdObject_LoadStruct(*vObj, *Reader, 0x100);
			Reader->rEndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_FOLDER))
		{
			// it is a GDS folder
			CdGDSFolder *vObj = new CdGDSFolder;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader->rBeginNameSpace();
			vObj->LoadStruct(*Reader, 0x100);
			Reader->rEndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_VIRTUAL_FOLDER))
		{
			// it is a virtual folder linking to another GDS file
			CdGDSVirtualFolder *vObj = new CdGDSVirtualFolder;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader->rBeginNameSpace();
			vObj->LoadStruct(*Reader, 0x100);
			Reader->rEndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_STREAM))
		{
			// it is a stream container
			CdGDSStreamContainer *vObj = new CdGDSStreamContainer;
			vObj->fFolder = this;
			I.Obj = vObj;

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

			Reader->rBeginNameSpace();
			vObj->LoadStruct(*Reader, 0x100);
			Reader->rEndStruct();

		} else {
			// it is a class object
			CdObjRef *obj = NULL;

			try{
				obj =
					fGDSStream->Collection().ClassMgr()->
					ToObj(*Reader, _GDSObjInitProc, fGDSStream, false);
			}
			catch (exception &E)
			{
				I.Obj = new CdGDSUnknown;
				I.Obj->fFolder = this;
				I.Obj->AddRef();
				throw;
			}

			if (dynamic_cast<CdGDSObj*>(obj))
			{
				I.Obj = static_cast<CdGDSObj*>(obj);
				I.Obj->fFolder = this;
				I.Obj->fGDSStream = dynamic_cast<CdBlockStream*>(Reader->Stream());
				I.Obj->fGDSStream->AddRef();
			} else {
				if (obj) delete obj;
				throw ErrGDSObj(
					"Invalid GDS object (it should be inherited from CdGDSObj).");
			}
		}

		I.Obj->AddRef();
	}
}

void CdGDSFolder::_UpdateAll()
{
	if (fChanged)
		SaveToBlockStream();

	vector<CdGDSFolder::TNode>::iterator it;
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

vector<CdGDSFolder::TNode>::iterator CdGDSFolder::_FindObj(CdGDSObj *Obj)
{
	vector<TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Obj == Obj) return it;
	return fList.end();
}


// CdGDSVirtualFolder

static const char *erFailToLink = "Fail to link the GDS file '%s'.";

CdGDSVirtualFolder::CdGDSVirtualFolder(): CdGDSAbsFolder()
{
	fLinkFile = NULL;
	fHasTried = true;
}

CdGDSVirtualFolder::~CdGDSVirtualFolder()
{
	if (fLinkFile)
	{
		delete fLinkFile;
		fLinkFile = NULL;
	}
}

bool CdGDSVirtualFolder::IsLoaded(bool Silent)
{
	if (!fHasTried)
	{
		fHasTried = true;

		CdGDSFile *file = GDSFile();
		UTF8String fn = UTF16ToUTF8(file->FileName());

		// remove the file name from 'fn'
		int i = (int)fn.size() - 1;
		for (; i >= 0; i--)
		{
			if ((fn[i]=='/') || (fn[i]=='\\'))
				break;
		}
		fn.resize(i+1);
		fn.append(fLinkFileName);

		// open the linking GDS file
		CdGDSFile *f = new CdGDSFile;
		try {
			f->LoadFile(fn.c_str(), file->ReadOnly());
			f->fRoot.fFolder = fFolder;
			f->fRoot.fVFolder = this;
		}
		catch (exception &E)
		{
			fErrMsg = E.what();
			delete f; f = NULL;
			if (!Silent)
				throw;
		}

		fLinkFile = f;
	}

	return (fLinkFile != NULL);
}

void CdGDSVirtualFolder::_CheckLinked()
{
	if (!IsLoaded(false))
		throw ErrGDSObj(erFailToLink, fLinkFileName.c_str());
}

void CdGDSVirtualFolder::SetLinkFile(const UTF8String &FileName)
{
	if (FileName != fLinkFileName)
	{
		if (fLinkFile)
		{
			CdGDSFile *file = fLinkFile;
			fLinkFile = NULL;
			delete file;
		}

		fLinkFileName = FileName;
		fHasTried = false;
		fChanged = true;
		fErrMsg.clear();
	}
}

CdGDSObj *CdGDSVirtualFolder::AddFolder(const UTF16String &Name)
{
	_CheckLinked();
	return fLinkFile->Root().AddFolder(Name);
}

CdGDSObj *CdGDSVirtualFolder::AddObj(const UTF16String &Name, CdGDSObj *val)
{
	_CheckLinked();
	return fLinkFile->Root().AddObj(Name, val);
}

CdGDSObj *CdGDSVirtualFolder::InsertObj(int index, const UTF16String &Name,
	CdGDSObj *val)
{
	_CheckLinked();
	return fLinkFile->Root().AddObj(Name, val);
}

void CdGDSVirtualFolder::MoveTo(int Index, int NewPos)
{
	_CheckLinked();
	fLinkFile->Root().MoveTo(Index, NewPos);
}

void CdGDSVirtualFolder::DeleteObj(int Index, bool force)
{
	_CheckLinked();
	fLinkFile->Root().DeleteObj(Index, force);
}

void CdGDSVirtualFolder::DeleteObj(CdGDSObj *val, bool force)
{
	_CheckLinked();
	fLinkFile->Root().DeleteObj(val, force);
}

void CdGDSVirtualFolder::ClearObj(bool force)
{
	_CheckLinked();
	fLinkFile->Root().ClearObj(force);
}

CdGDSObj *CdGDSVirtualFolder::ObjItem(int Index)
{
	_CheckLinked();
	return fLinkFile->Root().ObjItem(Index);
}

CdGDSObj *CdGDSVirtualFolder::ObjItem(const UTF16String &Name)
{
	_CheckLinked();
	return fLinkFile->Root().ObjItem(Name);
}

CdGDSObj *CdGDSVirtualFolder::ObjItemEx(int Index)
{
	_CheckLinked();
	return fLinkFile->Root().ObjItemEx(Index);
}

CdGDSObj *CdGDSVirtualFolder::ObjItemEx(const UTF16String &Name)
{
	_CheckLinked();
	return fLinkFile->Root().ObjItemEx(Name);
}

CdGDSObj *CdGDSVirtualFolder::Path(const UTF16String &FullName)
{
	_CheckLinked();
	return fLinkFile->Root().Path(FullName);
}

CdGDSObj *CdGDSVirtualFolder::PathEx(const UTF16String &FullName)
{
	_CheckLinked();
	return fLinkFile->Root().PathEx(FullName);
}

int CdGDSVirtualFolder::IndexObj(CdGDSObj *Obj)
{
	_CheckLinked();
	return fLinkFile->Root().IndexObj(Obj);
}

bool CdGDSVirtualFolder::HasChild(CdGDSObj *Obj, bool SubFolder)
{
	_CheckLinked();
	return fLinkFile->Root().HasChild(Obj, SubFolder);
}

int CdGDSVirtualFolder::NodeCount()
{
	_CheckLinked();
	return fLinkFile->Root().NodeCount();
}

void CdGDSVirtualFolder::LoadAfter(CdSerial &Reader, const TdVersion Version)
{
	UTF8String fn;
	Reader["FILENAME"] >> fn;
	CdGDSAbsFolder::LoadAfter(Reader, Version);

	SetLinkFile(fn);
	fChanged = false;
}

void CdGDSVirtualFolder::SaveAfter(CdSerial &Writer)
{
	Writer["FILENAME"] << fLinkFileName;
	CdGDSAbsFolder::SaveAfter(Writer);
}

void CdGDSVirtualFolder::Synchronize()
{
	CdGDSAbsFolder::Synchronize();
	if (fLinkFile)
		fLinkFile->fRoot.Synchronize();
}


// CdGDSStreamContainer

CdGDSStreamContainer::CdGDSStreamContainer(): CdGDSObjNoCName()
{
	fBufStream = new CdBufStream(new CdMemoryStream);
	fBufStream->AddRef();
	vAlloc_Stream = NULL;
	fNeedUpdate = false;
	vAllocID = 0;
	vAlloc_Ptr = 0;
}

CdGDSStreamContainer::~CdGDSStreamContainer()
{
	CloseWriter();
	if (fBufStream)
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

		if (fBufStream)
			fBufStream->Release();
		fBufStream = new CdBufStream(fGDSStream->Collection()[vAllocID]);
		fBufStream->AddRef();

		if (fPipeInfo)
        	fPipeInfo->PushReadPipe(*fBufStream);
	}
	#ifdef COREARRAY_CODE_DEBUG
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
		vAlloc_Ptr = Writer.Position() - TdBlockID::Size;
	}
	#ifdef COREARRAY_CODE_DEBUG
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
		#ifdef COREARRAY_CODE_DEBUG
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

			TdAutoRef<CdBufStream> M(new CdBufStream(vAlloc_Stream));
			if (fPipeInfo)
				fPipeInfo->PushWritePipe(*M);
			M->CopyFrom(*fBufStream);

			if (fBufStream)
				fBufStream->Release();
			fBufStream = M.get();
			fBufStream->AddRef();
		}
	}
}

SIZE64 CdGDSStreamContainer::GetSize()
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
					TdAutoRef<CdBufStream> Buf(new CdBufStream(Temp.get()));
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
				if (fBufStream)
					fBufStream->Release();
				fBufStream = new CdBufStream(vAlloc_Stream);
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

			if (fBufStream)
				fBufStream->Release();
			vAlloc_Stream->SetPosition(0);
			fBufStream = new CdBufStream(vAlloc_Stream);
        	fBufStream->AddRef();
			if (fPipeInfo)
				fPipeInfo->PushReadPipe(*fBufStream);
		}
	}
}

void CdGDSStreamContainer::CopyFrom(CdBufStream &Source, SIZE64 Count)
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
		N = (Count >= (SIZE64)sizeof(Buffer)) ? (ssize_t)sizeof(Buffer) : Count;
		Source.Read((void*)Buffer, N);
		fBufStream->Write((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyFrom(CdStream &Source, SIZE64 Count)
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
		N = (Count >= (SIZE64)sizeof(Buffer)) ? (ssize_t)sizeof(Buffer) : Count;
		Source.ReadBuffer((void*)Buffer, N);
		fBufStream->Write((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyTo(CdBufStream &Dest, SIZE64 Count)
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
		N = (Count >= (SIZE64)sizeof(Buffer)) ? (ssize_t)sizeof(Buffer) : Count;
		fBufStream->Read((void*)Buffer, N);
		Dest.Write((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyTo(CdStream &Dest, SIZE64 Count)
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
		N = (Count >= (SIZE64)sizeof(Buffer))? (ssize_t)sizeof(Buffer) : Count;
		fBufStream->Read((void*)Buffer, N);
		Dest.WriteBuffer((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::GetOwnBlockStream(vector<const CdBlockStream*> &Out)
{
	Out.clear();
	if (vAlloc_Stream) Out.push_back(vAlloc_Stream);
}


// CdGDSRoot

CdGDSRoot::CdGDSRoot(): CdGDSFolder()
{
	fVFolder = NULL;
}

UTF16String CdGDSRoot::Name() const
{
	if (fVFolder)
	{
		return fVFolder->Name();
	} else {
		return UTF16String();
	}
}

void CdGDSRoot::SetName(const UTF16String &NewName)
{
	if (fVFolder)
	{
		fVFolder->SetName(NewName);
	} else {
		throw ErrGDSFile(
			"The root of a GDS file is not allowed to have a name.");
	}
}



// CdGDSFile

static const char *erGDSInvalidOpenMode = "Invalid open mode in CdGDSFile.";
static const char *erInvalidPrefix = "Invalid prefix of stream!";
static const char *erEntryError = "Invalid entry point(%d).";
static const char *erSaveStream = "Should save it to a GDS file first!";

#ifdef COREARRAY_CODE_DEBUG
static const char *erFStream = "fStream should be NULL.";
#endif

const char *CdGDSFile::GDSFilePrefix()
{
	return COREARRAY_FILE_PREFIX;
}

void CdGDSFile::_Init()
{
	fVersion = COREARRAY_FILE_VERSION;
	fRoot.AddRef();
	fCodeStart = strlen(GDSFilePrefix()) +
		sizeof(TdVersion) + TdBlockID::Size;
	fReadOnly = false;
	fLog = new CdLogRecord; fLog->AddRef();
	fprocess_id = GetCurrentProcessID();
}

CdGDSFile::CdGDSFile(): CdBlockCollection() { _Init(); }

CdGDSFile::CdGDSFile(const UTF16String &fn, TdOpenMode Mode):
	CdBlockCollection()
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

CdGDSFile::CdGDSFile(const char *fn, TdOpenMode Mode):
	CdBlockCollection()
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
	if (fLog)
	{
	#ifdef COREARRAY_CODE_DEBUG
		if (fLog->Release() != 0)
			throw ErrSerial("Log::Release() should return 0 here.");
	#else
		fLog->Release();
	#endif
	}
}

void CdGDSFile::LoadStream(CdStream* Stream, bool ReadOnly)
{
	// Initialize
	CloseFile();
	fLog->List().clear();
	fReadOnly = ReadOnly;

	// Check the prefix
	const size_t L = strlen(GDSFilePrefix());
	vector<char> buf(L);
	Stream->ReadBuffer((void*)&buf[0], L);
	if (memcmp((void*)GDSFilePrefix(), (void*)&buf[0], L) !=0)
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
		_INTERNAL::CdObject_LoadStruct(fRoot, *Reader, fVersion);
		Reader->rEndStruct();
	} else
		throw ErrGDSFile(erEntryError, Entry.get());
}

void CdGDSFile::SaveStream(CdStream *Stream)
{
	#ifdef COREARRAY_CODE_DEBUG
	if (fStream != NULL)
		throw ErrGDSFile(erFStream);
	#endif

	// Save Prefix
	const size_t L = strlen(GDSFilePrefix());
	Stream->WriteBuffer((void*)GDSFilePrefix(), L);

	// Save Ver
	Stream->wUInt8(fVersion & 0xFF);
    Stream->wUInt8(fVersion >> 8);

	SIZE64 _EntryPos = Stream->Position();
	*Stream << TdBlockID(0);

	CdBlockCollection::WriteStream(Stream);
	fRoot.fGDSStream = NewBlockStream();
	fRoot.fGDSStream->AddRef();
	SIZE64 _NewPos = Stream->Position();

	Stream->SetPosition(_EntryPos);
	*Stream << fRoot.fGDSStream->ID();
	Stream->SetPosition(_NewPos);

	fRoot.SaveToBlockStream();
}

void CdGDSFile::LoadFile(const UTF16String &fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdFileStream(
		UTF16ToUTF8(fn).c_str(),
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = fn;
}

void CdGDSFile::LoadFile(const char *fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdFileStream(
		fn,
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = UTF8ToUTF16(fn);
}

void CdGDSFile::LoadFileFork(const char *fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdForkFileStream(
		fn,
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = UTF8ToUTF16(fn);
}

void CdGDSFile::SyncFile()
{
	if (fStream == NULL)
		throw ErrGDSFile(erSaveStream);
	fRoot._UpdateAll();
}

void CdGDSFile::SaveAsFile(const UTF16String &fn)
{
	TdAutoRef<CdStream> F(new CdFileStream(UTF16ToUTF8(fn).c_str(),
		CdFileStream::fmCreate));
	fFileName = fn;
	SaveStream(F.get());
}

void CdGDSFile::SaveAsFile(const char *fn)
{
	TdAutoRef<CdStream> F(new CdFileStream(fn, CdFileStream::fmCreate));
	fFileName = UTF8ToUTF16(fn);
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
		TdAutoRef<CdStream> F(new CdFileStream(UTF16ToUTF8(fn).c_str(),
			CdFileStream::fmCreate));

		// Save Prefix
		const size_t L = strlen(GDSFilePrefix());
		F->WriteBuffer((void*)GDSFilePrefix(), L);

		// Save Version
		F->wUInt8(fVersion & 0xFF);
		F->wUInt8(fVersion >> 8);

		// Save Entry ID
		(*F) << fRoot.fGDSStream->ID();

		// for-loop for all stream blocks
		for (int i=0; i < (int)fBlockList.size(); i++)
		{
			TdPosType bSize = fBlockList[i]->Size();
			TdPosType sSize = (2*TdPosType::Size +
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
	DuplicateFile(PCharToUTF16(fn), deep);
}

void CdGDSFile::CloseFile()
{
	if (fStream)
	{
		SyncFile();
		fFileName.clear();
		fLog->List().clear();
		fRoot.Attribute().Clear();
		fRoot._ClearFolder();
		if (fRoot.fGDSStream)
		{
			// todo: check
			fRoot.fGDSStream->Release();
			fRoot.fGDSStream = NULL;
		}
		CdBlockCollection::Clear();
    }
}

void CdGDSFile::TidyUp(bool deep)
{
	bool TempReadOnly = fReadOnly;
	UTF16String fn, f;
	fn = fFileName;
	f = fn + UTF8ToUTF16(".tmp");
	DuplicateFile(f, deep);
	CloseFile();
	remove(UTF16ToUTF8(fn).c_str());
	rename(UTF16ToUTF8(f).c_str(), UTF16ToUTF8(fn).c_str());
	LoadFile(fn, TempReadOnly);
}

bool CdGDSFile::_HaveModify(CdGDSFolder *folder)
{
	if (folder->fChanged) return true;

	vector<CdGDSFolder::TNode>::iterator it;
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

SIZE64 CdGDSFile::GetFileSize()
{
    return fStreamSize;
}

int CdGDSFile::GetNumOfFragment()
{
	return CdBlockCollection::NumOfFragment();
}

bool CdGDSFile::IfSupportForking()
{
	return (dynamic_cast<CdForkFileStream*>(fStream) != NULL);
}

TProcessID CdGDSFile::GetProcessID()
{
	return fprocess_id;
}

void CdGDSFile::SetProcessID()
{
	fprocess_id = GetCurrentProcessID();
}
