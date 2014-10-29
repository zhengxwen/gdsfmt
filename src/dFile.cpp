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

#include "dFile.h"
#include <algorithm>


using namespace CoreArray;

/// GDS file version
#define COREARRAY_FILE_VERSION   COREARRAY_VERSION

/// GDS file prefix
#define COREARRAY_FILE_PREFIX    "COREARRAYx0A"



// =====================================================================
// CdGDSObj
// =====================================================================

// CdObjAttr

static const char *VAR_ATTRCNT  = "ATTRCNT";
static const char *VAR_ATTRLIST = "ATTRLIST";

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

CdAny &CdObjAttr::Add(const UTF16String &Name)
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

CdAny & CdObjAttr::operator[](const UTF16String &Name)
{
	vector<TdPair*>::iterator it = Find(Name);
	if (it == fList.end())
		throw ErrGDSObj(rsAttrName, UTF16ToUTF8(Name).c_str());
	return (*it)->val;
}

CdAny & CdObjAttr::operator[](int Index)
{
	return fList.at(Index)->val;
}

void CdObjAttr::Loading(CdReader &Reader, TdVersion Version)
{
	C_Int32 Cnt;
	Reader[VAR_ATTRCNT] >> Cnt;
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
	}

	if (Cnt > 0)
	{
		Reader[VAR_ATTRLIST].BeginStruct();
		for (int i=0; i < Cnt; i++)
		{
			TdPair *I = new TdPair;
			try {
				I->name = Reader.Storage().RpUTF16();
				Reader >> I->val;
			} catch (...) {
				delete I;
				break;
			}
			fList.push_back(I);
		}
		Reader.EndStruct();
	}
}

void CdObjAttr::Saving(CdWriter &Writer)
{
	C_Int32 Cnt = fList.size();
	Writer[VAR_ATTRCNT] << Cnt;
	if (Cnt > 0)
	{
		Writer[VAR_ATTRLIST].NewStruct();
		vector<TdPair*>::iterator it;
		for (it=fList.begin(); it != fList.end(); it++)
		{
			Writer.Storage().WpUTF16((*it)->name);
			Writer << (*it)->val;
		}
		Writer.EndStruct();
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

static const char *ERR_NO_NAME = "No name exists!";
static const char *ERR_DUP_NAME = "Duplicate name!";
static const char *ERR_MOVE_TO_ADD = "Please call 'AddObj' to add an object.";
static const char *ERR_NOT_SHARE_FILE = "'MoveTo' should be within the same GDS file.";
static const char *ERR_MOVE_TO_CHILD = "Cannot move to its sub folder.";

CdGDSObj::CdGDSObj(): CdObjMsg(), fAttr(*this)
{
	fFolder = NULL;
	fGDSStream = NULL;
	fChanged = false;
}

CdGDSObj::~CdGDSObj()
{
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
	throw ErrGDSObj(ERR_NO_NAME);
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

UTF16String CdGDSObj::FullName(const char *Delimiter) const
{
	return FullName(UTF16Text(Delimiter));
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
						throw ErrGDSObj(ERR_DUP_NAME);
					it->Name = NewName;
					fFolder->fChanged = true;
				}
				return;
			}
		}
	}
	throw ErrGDSObj(ERR_NO_NAME);
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
					throw ErrGDSObj(ERR_MOVE_TO_CHILD);
			}
			if ((fFolder!=&folder) && (this!=&folder))
			{
				vector<CdGDSFolder::TNode>::iterator it;
				it = fFolder->_FindObj(this);
				if (folder._HasName(it->Name))
					throw ErrGDSObj(ERR_DUP_NAME);
				folder.fList.push_back(*it);
				fFolder->fList.erase(it);
				fFolder->fChanged = folder.fChanged = true;
				fFolder = &folder;
			}
		} else
			throw ErrGDSObj(ERR_NOT_SHARE_FILE);
	} else
		throw ErrGDSObj(ERR_MOVE_TO_ADD);
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

void CdGDSObj::LoadStruct(CdReader &Reader, TdVersion Version)
{
	// call load function ::Loading
	CdObjMsg::LoadStruct(Reader, Version);

	// load attribute
	COREARRAY_READER_CALL_SILENT(
		fAttr.Loading(Reader, Version)
	);
}

void CdGDSObj::SaveStruct(CdWriter &Writer, bool IncludeName)
{
	// beginning ...
	Writer.BeginNameSpace();
	if (IncludeName)
	{
		TdVersion Version = SaveVersion();
		Writer.Storage() << C_UInt16(Version);
		Writer.WriteClassName(dName());
	}

	// call save function ::Saving
	this->Saving(Writer);

	// save attribute
	fAttr.Saving(Writer);

	// ending ...
	Writer.EndStruct();
	fChanged = false;
}

void CdGDSObj::SaveToBlockStream()
{
	#ifdef COREARRAY_CODE_DEBUG
	_CheckGDSStream();
	#endif

	if (fGDSStream)
	{
		CdWriter Writer(fGDSStream, &GDSFile()->Log());
		SaveStruct(Writer, IsWithClassName());
	}
}

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

void CdGDSObj::_GDSObjInitProc(CdObjClassMgr &Sender, CdObject *dObj,
	void *Data)
{
	if (dynamic_cast<CdGDSObj*>(dObj))
	{
		static_cast<CdGDSObj*>(dObj)->fGDSStream =
			(CdBlockStream*)Data;
	}
}



// =====================================================================
// Stream Manage Pipe
// =====================================================================

static const char* VAR_PIPE_SIZE   = "PIPE_SIZE";
static const char* VAR_PIPE_LEVEL  = "PIPE_LEVEL";
static const char *VAR_PIPE_BKSIZE = "PIPE_BKSIZE";


// =====================================================================
// ZIP Pipe
// =====================================================================

namespace CoreArray
{
	/// The pipe for reading data from a ZIP stream
	class COREARRAY_DLL_DEFAULT CdZIPReadPipe: public CdStreamPipe
	{
	public:
		CdZIPReadPipe(): CdStreamPipe() {}

	protected:
		virtual CdStream *InitPipe(CdBufStream *BufStream)
		{
			fStream = BufStream->Stream();
			fPStream = new CdZInflate(*BufStream->Stream());
			return fPStream;
		}
		virtual CdStream *FreePipe()
		{
			if (fPStream) fPStream->Release();
			return fStream;
		}

	private:
		CdStream *fStream;
		CdZInflate *fPStream;
	};


	/// The pipe for writing data to a ZIP stream
	class COREARRAY_DLL_DEFAULT CdZIPWritePipe: public CdStreamPipe
	{
	public:
		CdZIPWritePipe(CdZDeflate::TZLevel vLevel,
				TdCompressRemainder &vRemainder):
			CdStreamPipe(), fRemainder(vRemainder) { fLevel = vLevel; }

		COREARRAY_INLINE CdZDeflate::TZLevel Level() const { return fLevel; }
		COREARRAY_INLINE CdStream *Stream() const { return fStream; }
		COREARRAY_INLINE CdStream *StreamZIP() const { return fPStream; }

	protected:
		virtual CdStream *InitPipe(CdBufStream *BufStream)
		{
			fStream = BufStream->Stream();
			fPStream = new CdZDeflate(*fStream, fLevel);
			fPStream->PtrExtRec = &fRemainder;
			return fPStream;
		}
		virtual CdStream *FreePipe()
		{
			if (fPStream) fPStream->Release();
			return fStream;
		}

	private:
		CdStream *fStream;
		CdZDeflate *fPStream;
		CdZDeflate::TZLevel fLevel;
		TdCompressRemainder &fRemainder;
	};


	static const int NUM_ZIP_LEVEL = 5;
	static const char *ZIP_Strings[NUM_ZIP_LEVEL] =
		{ "ZIP", "ZIP.fast", "ZIP.default", "ZIP.max", "ZIP.none" };
	static const CdZDeflate::TZLevel ZIPLevels[NUM_ZIP_LEVEL] =
		{ CdZDeflate::zcDefault, CdZDeflate::zcFastest,
		  CdZDeflate::zcDefault, CdZDeflate::zcMax,
		  CdZDeflate::zcNone };

	class COREARRAY_DLL_DEFAULT CdPipeZIP: public CdPipeMgrItem
	{
	private:
		CdZDeflate::TZLevel vLevel;
		SIZE64 vSizeInfo_Ptr;

	public:
		CdPipeZIP(CdZDeflate::TZLevel level = CdZDeflate::zcDefault):
				CdPipeMgrItem()
			{ vLevel = level; vSizeInfo_Ptr = -1; }

		/// Create a new CdPipeMgrItem object
		virtual CdPipeMgrItem *NewOne()
		{
			return new CdPipeZIP(vLevel);
		}
		/// Return the name of coder stored in stream
		virtual const char *Coder() const
		{
			return "ZIP";
		}
		/// Return the description of coder
		virtual const char *Description() const
		{
			return "zlib_" ZLIB_VERSION;
		}
		/// Return whether or not Mode is self
		virtual bool Equal(const char *Mode) const
		{
			int i = Which(Mode, ZIP_Strings, NUM_ZIP_LEVEL);
			if (i >= 0)
				return (ZIPLevels[i]==vLevel);
			else
				return false;
		}
		/// Return coder with parameters
		virtual string CoderParam()
		{
			switch (vLevel)
			{
				case CdZDeflate::zcNone:    return "ZIP.none";
				case CdZDeflate::zcFastest: return "ZIP.fast";
				case CdZDeflate::zcDefault: return "ZIP.default";
				case CdZDeflate::zcMax:     return "ZIP.max";
				default:                    return "ZIP";
			}
		}

		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZIPReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZIPWritePipe(vLevel, fRemainder)); }
		virtual void PopPipe(CdBufStream &buf)
			{ buf.PopPipe(); }
		virtual bool WriteMode(CdBufStream &buf) const
			{ return dynamic_cast<CdZDeflate*>(buf.Stream())!=NULL; }
		virtual void ClosePipe(CdBufStream &buf)
		{
			CdZDeflate *s = dynamic_cast<CdZDeflate*>(buf.Stream());
			if (s) s->Close();
        }

		virtual bool GetStreamInfo(CdBufStream *buf)
		{
			SIZE64 in, out;
			if (buf)
			{
				CdZDeflate *s = dynamic_cast<CdZDeflate*>(buf->Stream());
				if (s)
				{
					in = s->TotalIn();
					out = s->TotalOut() + (s->HaveClosed() ? 0 : s->Pending());
				} else
					return false;
			} else
				in = out = 0;
			if ((in!=fStreamTotalIn) || (out!=fStreamTotalOut))
			{
				fStreamTotalIn = in;
				fStreamTotalOut = out;
				return true;
			} else
				return false;
		}
	protected:

		virtual CdPipeMgrItem *Match(const char *Mode)
		{
			int i = Which(Mode, ZIP_Strings, NUM_ZIP_LEVEL);
			if (i >= 0)
				return new CdPipeZIP(ZIPLevels[i]);
			else
				return NULL;
		}
		virtual void UpdateStreamInfo(CdStream &Stream)
		{
			if (vSizeInfo_Ptr >= 0)
			{
				BYTE_LE<CdStream> S(Stream);
				S.SetPosition(vSizeInfo_Ptr);
				S << fStreamTotalIn << fStreamTotalOut;
            }
		}
		virtual void LoadStream(CdReader &Reader, TdVersion Version)
		{
			if (Reader.HaveProperty(VAR_PIPE_SIZE))
			{
				vSizeInfo_Ptr = Reader.PropPosition(VAR_PIPE_SIZE);
				C_Int64 Ary[2];
				Reader[VAR_PIPE_SIZE].GetShortRec(Ary, 2);
				fStreamTotalIn = Ary[0];
				fStreamTotalOut = Ary[1];
			} else {
				vSizeInfo_Ptr = -1;
				fStreamTotalIn = fStreamTotalOut = -1;
			}
			if (Reader.HaveProperty(VAR_PIPE_LEVEL))
			{
				C_UInt8 I = 0;
				Reader[VAR_PIPE_LEVEL] >> I;
				if (I > 3)
				{
					// Since zcNone=0, zcFastest=1, zcDefault=2, zcMax=3
            	    throw ErrGDSObj("Invalid 'PIPE_LEVEL %d'", I);
            	}
				vLevel = (CdZDeflate::TZLevel)I;
			} else
				vLevel = CdZDeflate::zcNone;
		}
		virtual void SaveStream(CdWriter &Writer)
		{
        	UpdateStreamSize();
			C_Int64 Ary[2] = { fStreamTotalIn, fStreamTotalOut };
			Writer[VAR_PIPE_SIZE].NewShortRec(Ary, 2);
			vSizeInfo_Ptr = Writer.PropPosition(VAR_PIPE_SIZE);
			Writer[VAR_PIPE_LEVEL] << C_UInt8(vLevel);
		}
	};
}


// =====================================================================
// ZIP Pipe with the support of random access
// =====================================================================

namespace CoreArray
{
	/// The pipe for reading data from a ZIP stream
	class COREARRAY_DLL_DEFAULT CdZRAReadPipe: public CdStreamPipe
	{
	public:
		CdZRAReadPipe(): CdStreamPipe() {}

	protected:
		virtual CdStream *InitPipe(CdBufStream *BufStream)
		{
			fStream = BufStream->Stream();
			fPStream = new CdZRA_Inflate(*BufStream->Stream());
			return fPStream;
		}
		virtual CdStream *FreePipe()
		{
			if (fPStream) fPStream->Release();
			return fStream;
		}

	private:
		CdStream *fStream;
		CdZInflate *fPStream;
	};


	/// The pipe for writing data to a ZIP stream
	class COREARRAY_DLL_DEFAULT CdZRAWritePipe: public CdStreamPipe
	{
	public:
		CdZRAWritePipe(CdZDeflate::TZLevel vLevel,
				CdZRA_Deflate::TRABlockSize vBSize,
				TdCompressRemainder &vRemainder):
			CdStreamPipe(), fRemainder(vRemainder)
		{
			fLevel = vLevel; fBSize = vBSize;
		}

		inline CdZDeflate::TZLevel Level() const { return fLevel; };
		inline CdZRA_Deflate::TRABlockSize BSize() const { return fBSize; }
		inline CdStream *Stream() const { return fStream; }
		inline CdStream *StreamZIP() const { return fPStream; }

	protected:
		virtual CdStream *InitPipe(CdBufStream *BufStream)
		{
			fStream = BufStream->Stream();
			fPStream = new CdZRA_Deflate(*fStream, fLevel, fBSize);
			fPStream->PtrExtRec = &fRemainder;
			return fPStream;
		}
		virtual CdStream *FreePipe()
		{
			if (fPStream) fPStream->Release();
			return fStream;
		}

	private:
		CdStream *fStream;
		CdZDeflate *fPStream;
		CdZDeflate::TZLevel fLevel;
		CdZRA_Deflate::TRABlockSize fBSize;
		TdCompressRemainder &fRemainder;
	};


	/// ZIP string with the support of random access
	static const char *ZRA_Strings[NUM_ZIP_LEVEL] =
		{ "ZIP_RA", "ZIP_RA.fast", "ZIP_RA.default", "ZIP_RA.max", "ZIP_RA.none" };

	/// 
	#define ZRA_STR_BSIZE_NUM    7
	/// Block size string according to CdZRA_Deflate::TRABlockSize
	static const char *ZRA_Str_BSize[ZRA_STR_BSIZE_NUM] =
		{ "16K", "32K", "64K", "128K", "256K", "512K", "1M" };


	class COREARRAY_DLL_DEFAULT CdPipeZRA: public CdPipeMgrItem
	{
	private:
		CdZDeflate::TZLevel vLevel;
		CdZRA_Deflate::TRABlockSize vBSize;
		SIZE64 vSizeInfo_Ptr;

	public:
		CdPipeZRA(): CdPipeMgrItem()
		{
			vLevel = CdZDeflate::zcDefault;
			vBSize = CdZRA_Deflate::bs64K;
			vSizeInfo_Ptr = -1;
		}
		CdPipeZRA(CdZDeflate::TZLevel level, CdZRA_Deflate::TRABlockSize bs):
			CdPipeMgrItem()
		{
			vLevel = level;
			vBSize = bs;
			vSizeInfo_Ptr = -1;
		}

		/// Create a new CdPipeMgrItem object
		virtual CdPipeMgrItem *NewOne()
		{
			return new CdPipeZRA(vLevel, vBSize);
		}
		/// Return the name of coder stored in stream
		virtual const char *Coder() const
		{
			return "ZIP_RA";
		}
		/// Return the description of coder
		virtual const char *Description() const
		{
			return "zlib_" ZLIB_VERSION "(chunk + random access)";
		}
		/// Return whether or not Mode is self
		virtual bool Equal(const char *Mode) const
		{
			string s = Mode;
			size_t pos = s.find(':');
			if (pos != string::npos)
			{
				Mode += (pos + 1); s.resize(pos);
			} else
				Mode = "256K";

			int i = Which(s.c_str(), ZRA_Strings, NUM_ZIP_LEVEL);
			int j = Which(Mode, ZRA_Str_BSize, ZRA_STR_BSIZE_NUM);
			if ((i >= 0) && (j>=0))
			{
				return (vLevel == ZIPLevels[i]) &&
					(vBSize == (CdZRA_Deflate::TRABlockSize)j);
			} else
				return false;
		}
		/// Return coder with parameters
		virtual string CoderParam()
		{
			string s;
			switch (vLevel)
			{
				case CdZDeflate::zcNone:    s = "ZIP_RA.none";
				case CdZDeflate::zcFastest: s = "ZIP_RA.fast";
				case CdZDeflate::zcDefault: s = "ZIP_RA.default";
				case CdZDeflate::zcMax:     s = "ZIP_RA.max";
				default:                    s = "ZIP_RA";
			}
			s.append(":");
			s.append(ZRA_Str_BSize[vBSize]);
			return s;
		}

		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZRAReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZRAWritePipe(vLevel, vBSize, fRemainder)); }
		virtual void PopPipe(CdBufStream &buf)
			{ buf.PopPipe(); }
		virtual bool WriteMode(CdBufStream &buf) const
			{ return dynamic_cast<CdZRA_Deflate*>(buf.Stream())!=NULL; }
		virtual void ClosePipe(CdBufStream &buf)
		{
			CdZRA_Deflate *s = dynamic_cast<CdZRA_Deflate*>(buf.Stream());
			if (s) s->Close();
        }

		virtual bool GetStreamInfo(CdBufStream *buf)
		{
			SIZE64 in, out;
			if (buf)
			{
				CdZRA_Deflate *s = dynamic_cast<CdZRA_Deflate*>(buf->Stream());
				if (s)
				{
					in = s->TotalIn();
					out = s->TotalOut() + (s->HaveClosed() ? 0 : s->Pending());
				} else
					return false;
			} else
				in = out = 0;
			if ((in!=fStreamTotalIn) || (out!=fStreamTotalOut))
			{
				fStreamTotalIn = in;
				fStreamTotalOut = out;
				return true;
			} else
				return false;
		}
	protected:

		virtual CdPipeMgrItem *Match(const char *Mode)
		{
			string s = Mode;
			size_t pos = s.find(':');
			if (pos != string::npos)
			{
				Mode += (pos + 1); s.resize(pos);
			} else
				Mode = "256K";

			int i = Which(s.c_str(), ZRA_Strings, NUM_ZIP_LEVEL);
			int j = Which(Mode, ZRA_Str_BSize, ZRA_STR_BSIZE_NUM);
			if ((i >= 0) && (j>=0))
			{
				return new CdPipeZRA(ZIPLevels[i],
					(CdZRA_Deflate::TRABlockSize)j);
			} else
				return NULL;
		}
		virtual void UpdateStreamInfo(CdStream &Stream)
		{
			if (vSizeInfo_Ptr >= 0)
			{
				BYTE_LE<CdStream> S(Stream);
				S.SetPosition(vSizeInfo_Ptr);
				S << fStreamTotalIn << fStreamTotalOut;
            }
		}
		virtual void LoadStream(CdReader &Reader, TdVersion Version)
		{
			// Pipe size of input and output
			if (Reader.HaveProperty(VAR_PIPE_SIZE))
			{
				vSizeInfo_Ptr = Reader.PropPosition(VAR_PIPE_SIZE);
				C_Int64 Ary[2];
				Reader[VAR_PIPE_SIZE].GetShortRec(Ary, 2);
				fStreamTotalIn = Ary[0];
				fStreamTotalOut = Ary[1];
			} else {
				vSizeInfo_Ptr = -1;
				fStreamTotalIn = fStreamTotalOut = -1;
			}
			// Pipe compression level
			if (Reader.HaveProperty(VAR_PIPE_LEVEL))
			{
				C_UInt8 I = 0;
				Reader[VAR_PIPE_LEVEL] >> I;
				if (I > 3)
				{
					// Since zcNone=0, zcFastest=1, zcDefault=2, zcMax=3
            	    throw ErrGDSObj("Invalid 'PIPE_LEVEL %d'", I);
            	}
				vLevel = (CdZDeflate::TZLevel)I;
			} else
				vLevel = CdZDeflate::zcNone;
			// Pipe compressed block size
			if (Reader.HaveProperty(VAR_PIPE_BKSIZE))
			{
				C_UInt8 I = 0;
				Reader[VAR_PIPE_BKSIZE] >> I;
				if (I > CdZRA_Deflate::bs1M)
				{
					// Since zcNone=0, zcFastest=1, zcDefault=2, zcMax=3
            	    throw ErrGDSObj("Invalid 'PIPE_BKSIZE %d'", I);
            	}
				vBSize = (CdZRA_Deflate::TRABlockSize)I;
			} else
				vBSize = CdZRA_Deflate::bs64K;
		}

		virtual void SaveStream(CdWriter &Writer)
		{
        	UpdateStreamSize();
			C_Int64 Ary[2] = { fStreamTotalIn, fStreamTotalOut };
			Writer[VAR_PIPE_SIZE].NewShortRec(Ary, 2);
			vSizeInfo_Ptr = Writer.PropPosition(VAR_PIPE_SIZE);
			Writer[VAR_PIPE_LEVEL] << C_UInt8(vLevel);
			Writer[VAR_PIPE_BKSIZE] << C_UInt8(vBSize);
		}
	};
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

void CdPipeMgrItem::LoadStream(CdReader &Reader, TdVersion Version) {}

void CdPipeMgrItem::SaveStream(CdWriter &Writer) {}

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

CdStreamPipeMgr CoreArray::dStreamPipeMgr;

CdStreamPipeMgr::CdStreamPipeMgr(): CdAbstractManager()
{
	Register(new CdPipeZIP);
	Register(new CdPipeZRA);
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


CdPipeMgrItem *CdStreamPipeMgr::Match(CdGDSObjPipe &Obj, const char *Mode)
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


// =====================================================================
// CdGDSObjPipe
// =====================================================================

static const char *VAR_PIPE = "PIPE";
static const char *ERR_PIPE_CODER = "Invalid pipe coder: %s";

CdGDSObjPipe::CdGDSObjPipe(): CdGDSObj()
{
	fPipeInfo = NULL;
}

CdGDSObjPipe::~CdGDSObjPipe()
{
	if (fPipeInfo)
		delete fPipeInfo;
}

void CdGDSObjPipe::AssignOne(CdGDSObj &Source, void *Param)
{
	// TODO:
    fAttr.Assign(Source.Attribute());
}

void CdGDSObjPipe::Loading(CdReader &Reader, TdVersion Version)
{
	// clear Pipe
	if (fPipeInfo) delete fPipeInfo;
	fPipeInfo = NULL;

	// load Pipe object
	if (Reader.HaveProperty(VAR_PIPE))
	{
		UTF8String Coder;
		Reader[VAR_PIPE] >> Coder;
		{
			fPipeInfo = dStreamPipeMgr.Match(*this, RawText(Coder).c_str());
			if (fPipeInfo==NULL)
    	    	throw ErrGDSObj(ERR_PIPE_CODER, RawText(Coder).c_str());
			fPipeInfo->LoadStream(Reader, Version);
		}
	}
}

void CdGDSObjPipe::Saving(CdWriter &Writer)
{
	CdGDSObj::Saving(Writer);
	if (fPipeInfo)
	{
		Writer[VAR_PIPE] << UTF8Text(fPipeInfo->Coder());
		fPipeInfo->SaveStream(Writer);
	}
}

void CdGDSObjPipe::GetPipeInfo() {}



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
		val->AddRef();
		val->SaveToBlockStream();
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
	static const C_UTF16 delimit = '/';
	const C_UTF16 *p = FullName.c_str();

	CdGDSObj *rv = this;
	while ((*p) && (rv))
	{
		if (!dynamic_cast<CdGDSAbsFolder*>(rv))
			return NULL;
		if (*p == delimit) p ++;

		const C_UTF16 *s = p;
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
	static const C_UTF16 delimit = '/';
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

static const char *VAR_DIRCNT = "DIRCNT";
static const char *VAR_DIRLIST = "DIRLIST";
static const char *VAR_DIR_ID = "ID";
static const char *VAR_DIR_FLAG = "FLAG";
static const char *VAR_DIR_NAME = "NAME";

void CdGDSFolder::Loading(CdReader &Reader, TdVersion Version)
{
	// Load directory inforamtion
	fList.clear();
	C_Int32 L = 0;
	Reader[VAR_DIRCNT] >> L;

	if (L > 0)
	{
		Reader[VAR_DIRLIST].BeginStruct();
		for (C_Int32 k = 0; k < L; k++)
		{
			TNode I;
			Reader.BeginNameSpace();
			{
				Reader[VAR_DIR_ID]   >> I.StreamID;
				Reader[VAR_DIR_FLAG] >> I.Flag;
				Reader[VAR_DIR_NAME] >> I.Name;
			}
			Reader.EndStruct();

			fList.push_back(I);
		}
		Reader.EndStruct();
	}

	// Load the attribute
	CdGDSAbsFolder::Loading(Reader, Version);
}

void CdGDSFolder::Saving(CdWriter &Writer)
{
	C_Int32 L = fList.size();
	Writer[VAR_DIRCNT] << L;

	if (L > 0)
	{
		Writer[VAR_DIRLIST].NewStruct();
		{
			vector<TNode>::iterator it;
			for (it = fList.begin(); it != fList.end(); it++)
			{
				Writer.BeginNameSpace();
				Writer[VAR_DIR_ID] << it->StreamID;
				it->_pos = Writer.PropPosition(VAR_DIR_ID);
				Writer[VAR_DIR_FLAG] << it->Flag;
				Writer[VAR_DIR_NAME] << it->Name;
				Writer.EndStruct();
			}
		}
		Writer.EndStruct();
	}

	// Save the attribute
	CdGDSAbsFolder::Saving(Writer);
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

		CdReader Reader(fGDSStream->Collection()[I.StreamID],
			&GDSFile()->Log());

		if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_LABEL))
		{
			// it is a label
			CdGDSLabel *vObj = new CdGDSLabel;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader.BeginNameSpace();
			_INTERNAL::CdObject_LoadStruct(*vObj, Reader, 0x100);
			Reader.EndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(&Reader.Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_FOLDER))
		{
			// it is a GDS folder
			CdGDSFolder *vObj = new CdGDSFolder;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader.BeginNameSpace();
			vObj->LoadStruct(Reader, 0x100);
			Reader.EndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(&Reader.Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_VIRTUAL_FOLDER))
		{
			// it is a virtual folder linking to another GDS file
			CdGDSVirtualFolder *vObj = new CdGDSVirtualFolder;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader.BeginNameSpace();
			vObj->LoadStruct(Reader, 0x100);
			Reader.EndStruct();

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(&Reader.Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_STREAM))
		{
			// it is a stream container
			CdGDSStreamContainer *vObj = new CdGDSStreamContainer;
			vObj->fFolder = this;
			I.Obj = vObj;

			vObj->fGDSStream = dynamic_cast<CdBlockStream*>(&Reader.Stream());
			/// todo: check
			vObj->fGDSStream->AddRef();

			Reader.BeginNameSpace();
			vObj->LoadStruct(Reader, 0x100);
			Reader.EndStruct();

		} else {

			// it is a class object
			CdObjRef *obj = NULL;

			try{
				obj =
					fGDSStream->Collection().ClassMgr()->
					ToObj(Reader, _GDSObjInitProc, fGDSStream, false);
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
				I.Obj->fGDSStream = dynamic_cast<CdBlockStream*>(
					&Reader.Stream());
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
		UTF8String fn = file->FileName();

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

static const char *VAR_FILENAME = "FILENAME";

void CdGDSVirtualFolder::Loading(CdReader &Reader, TdVersion Version)
{
	UTF8String fn;
	Reader[VAR_FILENAME] >> fn;
	CdGDSAbsFolder::Loading(Reader, Version);

	SetLinkFile(fn);
	fChanged = false;
}

void CdGDSVirtualFolder::Saving(CdWriter &Writer)
{
	Writer[VAR_FILENAME] << fLinkFileName;
	CdGDSAbsFolder::Saving(Writer);
}

void CdGDSVirtualFolder::Synchronize()
{
	CdGDSAbsFolder::Synchronize();
	if (fLinkFile)
		fLinkFile->fRoot.Synchronize();
}


// CdGDSStreamContainer

static const char *VAR_DATA = "DATA";

CdGDSStreamContainer::CdGDSStreamContainer(): CdGDSObjPipe()
{
	fBufStream = new CdBufStream(new CdMemoryStream);
	fBufStream->AddRef();
	vAllocStream = NULL;
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

const char *CdGDSStreamContainer::dName()
{
	return "dStream";
}

const char *CdGDSStreamContainer::dTraitName()
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

void CdGDSStreamContainer::Loading(CdReader &Reader, TdVersion Version)
{
	CdGDSObjPipe::Loading(Reader, Version);

	if (fGDSStream != NULL)
	{
		vAllocID = 0;
		Reader[VAR_DATA] >> vAllocID;
		vAlloc_Ptr = Reader.PropPosition(VAR_DATA);

		if (fBufStream)
			fBufStream->Release();
		fBufStream = new CdBufStream(fGDSStream->Collection()[vAllocID]);
		fBufStream->AddRef();

		if (fPipeInfo)
        	fPipeInfo->PushReadPipe(*fBufStream);
	} else {
		throw ErrGDSStreamContainer(
		"'CdGDSStreamContainer' object should be combined with a GDS file!");
	}
}

void CdGDSStreamContainer::Saving(CdWriter &Writer)
{
	CdGDSObjPipe::Saving(Writer);

	if (fGDSStream != NULL)
	{
		if (vAllocStream == NULL)
		{
			_CheckGDSStream();
			vAllocStream = fGDSStream->Collection().NewBlockStream();

			if (fBufStream) fBufStream->Release();
			fBufStream = new CdBufStream(vAllocStream);
			fBufStream->AddRef();
			if (fPipeInfo)
				fPipeInfo->PushWritePipe(*fBufStream);
		}
		TdGDSBlockID Entry = vAllocStream->ID();
		Writer[VAR_DATA] << Entry;
		vAlloc_Ptr = Writer.PropPosition(VAR_DATA);
	} else {
		throw ErrGDSStreamContainer(
		"'CdGDSStreamContainer' object should be combined with a GDS file!");
	}
}

SIZE64 CdGDSStreamContainer::GetSize()
{
	if (fPipeInfo)
		return fPipeInfo->StreamTotalIn();
	else
    	return fBufStream->GetSize();
}

static const char *ERR_PACKED_MODE = "Invalid packed/compression method '%s'.";
static const char *ERR_READONLY = "The GDS file is read-only!";

void CdGDSStreamContainer::SetPackedMode(const char *Mode)
{
	if (fGDSStream && fGDSStream->ReadOnly())
		throw ErrGDSStreamContainer(ERR_READONLY);

	if (fPipeInfo ? (!fPipeInfo->Equal(Mode)) : true)
	{
		if (vAllocStream && fGDSStream && (vAllocStream->GetSize()>0))
		{
			Synchronize();

			// total size
			SIZE64 TotalSize = GetSize();

			if (fPipeInfo) delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (strcmp(Mode, "")!=0))
				throw ErrGDSStreamContainer(ERR_PACKED_MODE, Mode);

			{
				// automatically release the temporary stream
				TdAutoRef<CdBufStream> Output(new CdBufStream(new CdTempStream));
				CopyTo(*Output, TotalSize);
				Output.get()->FlushWrite();

				// input
				vAllocStream->SetPosition(0);
				vAllocStream->SetSizeOnly(0);
				TdAutoRef<CdBufStream> Input(new CdBufStream(vAllocStream));
				if (fPipeInfo)
					fPipeInfo->PushWritePipe(*Input);

				// copy
				Input.get()->CopyFrom(*Output);
				Input.get()->FlushWrite();
				if (fPipeInfo)
				{
					fPipeInfo->ClosePipe(*Input);
					fPipeInfo->GetStreamInfo(Input.get());
				}
			}

			vAllocStream->SetPosition(0);
			if (fBufStream)
				fBufStream->Release();
			fBufStream = new CdBufStream(vAllocStream);
			fBufStream->AddRef();
			if (fPipeInfo)
				fPipeInfo->PushReadPipe(*fBufStream);

			// save, since PipeInfo has been changed.
			SaveToBlockStream();
		} else {
			if (fPipeInfo)
				delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (strcmp(Mode, "")!=0))
				throw ErrGDSStreamContainer(ERR_PACKED_MODE, Mode);
		}
	}
}

void CdGDSStreamContainer::CloseWriter()
{
	fBufStream->OnFlush.Clear();
	fBufStream->FlushWrite();
	if (fPipeInfo && vAllocStream)
	{
		if (fPipeInfo->WriteMode(*fBufStream))
		{
			fPipeInfo->ClosePipe(*fBufStream);
			if (_GetStreamPipeInfo(fBufStream, false))
				_UpdateStreamPipeInfo(*fGDSStream);

			if (fBufStream)
				fBufStream->Release();
			vAllocStream->SetPosition(0);
			fBufStream = new CdBufStream(vAllocStream);
        	fBufStream->AddRef();
			if (fPipeInfo)
				fPipeInfo->PushReadPipe(*fBufStream);
		}
	}
}

void CdGDSStreamContainer::CopyFrom(CdBufStream &Source, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	while (Count > 0)
	{
		ssize_t N = (Count <= (int)sizeof(Buffer)) ? Count : sizeof(Buffer);
		Source.ReadData((void*)Buffer, N);
		fBufStream->WriteData((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyFrom(CdStream &Source, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	while (Count > 0)
	{
		ssize_t N = (Count <= (int)sizeof(Buffer)) ? Count : sizeof(Buffer);
		Source.ReadData((void*)Buffer, N);
		fBufStream->WriteData((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyTo(CdBufStream &Dest, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];

	if (Count < 0)
	{
		fBufStream->SetPosition(0);
		Dest.SetPosition(0);
		Count = GetSize();
	}
	while (Count > 0)
	{
		ssize_t N = (Count <= (int)sizeof(Buffer)) ? Count : sizeof(Buffer);
		fBufStream->ReadData((void*)Buffer, N);
		Dest.WriteData((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::CopyTo(CdStream &Dest, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];

	if (Count < 0)
	{
		fBufStream->SetPosition(0);
		Dest.SetPosition(0);
		Count = GetSize();
	}
	while (Count > 0)
	{
		ssize_t N = (Count <= (int)sizeof(Buffer)) ? Count : sizeof(Buffer);
		fBufStream->ReadData((void*)Buffer, N);
		Dest.WriteData((void*)Buffer, N);
		Count -= N;
	}
}

void CdGDSStreamContainer::GetOwnBlockStream(vector<const CdBlockStream*> &Out)
{
	Out.clear();
	if (vAllocStream) Out.push_back(vAllocStream);
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



// =====================================================================
// CdGDSUnknown
// =====================================================================

CdGDSUnknown::CdGDSUnknown(): CdGDSObjNoCName()
{ }

void CdGDSUnknown::SaveStruct(CdWriter &Writer, bool IncludeName)
{
	// do nothing!
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
		sizeof(TdVersion) + GDS_BLOCK_ID_SIZE;
	fReadOnly = false;
	fLog = new CdLogRecord; fLog->AddRef();
	fprocess_id = GetCurrentProcessID();
}

CdGDSFile::CdGDSFile(): CdBlockCollection() { _Init(); }

CdGDSFile::CdGDSFile(const UTF8String &fn, TdOpenMode Mode):
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

void CdGDSFile::LoadStream(CdStream *Stream, bool ReadOnly)
{
	// Initialize
	CloseFile();
	fLog->List().clear();
	fReadOnly = ReadOnly;

	// Check the prefix
	const size_t L = strlen(GDSFilePrefix());
	vector<char> buf(L);
	Stream->ReadData((void*)&buf[0], L);
	if (memcmp((void*)GDSFilePrefix(), (void*)&buf[0], L) !=0)
		throw ErrGDSFile(erInvalidPrefix);

	// Load Version
	fVersion = Stream->R8b();
	fVersion |= Stream->R8b() << 8;

	// The entry of stream ID
	TdGDSBlockID Entry;
	BYTE_LE<CdStream>(Stream) >> Entry;

	// To identify the block stream
	CdBlockCollection::LoadStream(Stream, ReadOnly);

	if (HaveID(Entry))
	{
		fRoot.fGDSStream = (*this)[Entry];
		fRoot.fGDSStream->AddRef();

		CdReader Reader(fRoot.fGDSStream, &Log());
		Reader.BeginNameSpace();
		_INTERNAL::CdObject_LoadStruct(fRoot, Reader, fVersion);
		Reader.EndStruct();
	} else
		throw ErrGDSFile(erEntryError, Entry.Get());
}

void CdGDSFile::SaveStream(CdStream *Stream)
{
	#ifdef COREARRAY_CODE_DEBUG
	if (fStream != NULL)
		throw ErrGDSFile(erFStream);
	#endif

	// Save Prefix
	const size_t L = strlen(GDSFilePrefix());
	Stream->WriteData((void*)GDSFilePrefix(), L);

	// Save Ver
	Stream->W8b(fVersion & 0xFF);
    Stream->W8b(fVersion >> 8);

	SIZE64 _EntryPos = Stream->Position();
	BYTE_LE<CdStream>(Stream) << TdGDSBlockID(0);

	CdBlockCollection::WriteStream(Stream);
	fRoot.fGDSStream = NewBlockStream();
	fRoot.fGDSStream->AddRef();
	SIZE64 _NewPos = Stream->Position();

	Stream->SetPosition(_EntryPos);
	BYTE_LE<CdStream>(Stream) << fRoot.fGDSStream->ID();
	Stream->SetPosition(_NewPos);

	fRoot.SaveToBlockStream();
}

void CdGDSFile::LoadFile(const UTF8String &fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdFileStream(RawText(fn).c_str(),
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = fn;
}

void CdGDSFile::LoadFile(const char *fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdFileStream(fn,
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = UTF8Text(fn);
}

void CdGDSFile::LoadFileFork(const char *fn, bool ReadOnly)
{
	TdAutoRef<CdStream> F(new CdForkFileStream(fn,
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly);
	fFileName = UTF8Text(fn);
}

void CdGDSFile::SyncFile()
{
	if (fStream == NULL)
		throw ErrGDSFile(erSaveStream);
	fRoot._UpdateAll();
}

void CdGDSFile::SaveAsFile(const UTF8String &fn)
{
	TdAutoRef<CdStream> F(new CdFileStream(RawText(fn).c_str(),
		CdFileStream::fmCreate));
	fFileName = fn;
	SaveStream(F.get());
}

void CdGDSFile::SaveAsFile(const char *fn)
{
	TdAutoRef<CdStream> F(new CdFileStream(fn, CdFileStream::fmCreate));
	fFileName = UTF8Text(fn);
	SaveStream(F.get());
}

void CdGDSFile::DuplicateFile(const UTF8String &fn, bool deep)
{
	if (deep)
	{
		CdGDSFile file(fn, CdGDSFile::dmCreate);
		file.Root().AssignOneEx(Root());
	} else {
		// create a new file
		TdAutoRef<CdStream> F(new CdFileStream(RawText(fn).c_str(),
			CdFileStream::fmCreate));

		// Save Prefix
		const size_t L = strlen(GDSFilePrefix());
		F->WriteData((void*)GDSFilePrefix(), L);

		// Save Version
		F->W8b(fVersion & 0xFF);
		F->W8b(fVersion >> 8);

		// Save Entry ID
		BYTE_LE<CdStream>(*F) << fRoot.fGDSStream->ID();

		// for-loop for all stream blocks
		for (int i=0; i < (int)fBlockList.size(); i++)
		{
			TdGDSPos bSize = fBlockList[i]->Size();
			TdGDSPos sSize = (2*GDS_POS_SIZE +
				CdBlockStream::TBlockInfo::HeadSize + bSize) |
				GDS_STREAM_POS_MASK_HEAD_BIT;
			TdGDSPos sNext = 0;
			BYTE_LE<CdStream>(*F) <<
				sSize << sNext << fBlockList[i]->ID() << bSize;
			F->CopyFrom(*fBlockList[i]);
		}
	}
}

void CdGDSFile::DuplicateFile(const char *fn, bool deep)
{
	DuplicateFile(UTF8Text(fn), deep);
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
	UTF8String fn, f;
	fn = fFileName;
	f = fn + ASC(".tmp");
	DuplicateFile(f, deep);
	CloseFile();

	remove(RawText(fn).c_str());
	rename(RawText(f).c_str(), RawText(fn).c_str());
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
