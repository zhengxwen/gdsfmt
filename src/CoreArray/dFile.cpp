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
// Copyright (C) 2007-2020    Xiuwen Zheng
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

/// GDS file prefix, SHOULD NOT BE CHANGED
#define COREARRAY_FILE_PREFIX    "COREARRAYx0A"


static const char *ERR_OBJ_RELEASE = "%s(), Release() should return ZERO.";


// =====================================================================
// CdGDSObj
// =====================================================================

// CdObjAttr

static const char *VAR_ATTRCNT  = "ATTRCNT";
static const char *VAR_ATTRLIST = "ATTRLIST";

static const char *ERR_ATTR_NAME = "No Attribute Name ('%s').";
static const char *ERR_ATTR_NAME_EXIST = "Attribute '%s' has existed.";
static const char *ERR_ATTR_INVALID_NAME = "Invalid zero-length name.";

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
	Clear();
	const size_t n = Source.Count();
	fList.reserve(n);
	for (size_t i=0; i < n; i++)
	{
		TdPair *I = new TdPair;
		I->name = Source.Names(i);
		fList.push_back(I);
		Changed();
		I->val = Source[i];
	}
}

CdAny &CdObjAttr::Add(const UTF8String &Name)
{
	_ValidateName(Name);
	vector<TdPair*>::iterator it = _Find(Name);
	if (it == fList.end())
	{
		TdPair *I = new TdPair;
		I->name = Name;
		fList.push_back(I);
		Changed();
		return I->val;
	} else
		throw ErrGDSObj(ERR_ATTR_NAME_EXIST, Name.c_str());
}

int CdObjAttr::IndexName(const UTF8String &Name)
{
	vector<TdPair*>::iterator it = _Find(Name);
	if (it != fList.end())
    	return it - fList.begin();
	else
		return -1;
}

bool CdObjAttr::HasName(const UTF8String &Name)
{
	return (IndexName(Name) >= 0);
}

void CdObjAttr::Delete(const UTF8String &Name)
{
	vector<TdPair*>::iterator it = _Find(Name);
	if (it == fList.end())
		throw ErrGDSObj(ERR_ATTR_NAME, Name.c_str());
	TdPair *p = *it;
	*it = NULL;
	fList.erase(it);
	delete p;
	Changed();
}

void CdObjAttr::Delete(int Index)
{
	TdPair *p = fList.at(Index);
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

CdAny & CdObjAttr::operator[](const UTF8String &Name)
{
	vector<TdPair*>::iterator it = _Find(Name);
	if (it == fList.end())
		throw ErrGDSObj(ERR_ATTR_NAME, Name.c_str());
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
				I->name = UTF16ToUTF8(Reader.Storage().RpUTF16()); // TODO
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
			Writer.Storage().WpUTF16(UTF8ToUTF16((*it)->name)); // TODO
			Writer << (*it)->val;
		}
		Writer.EndStruct();
	}
}

vector<CdObjAttr::TdPair*>::iterator CdObjAttr::_Find(const UTF8String &Name)
{
	vector<TdPair*>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		if ((*it)->name == Name)
			break;
	}
	return it;
}

void CdObjAttr::SetName(const UTF8String &OldName, const UTF8String &NewName)
{
	_ValidateName(NewName);
	vector<TdPair*>::iterator it = _Find(OldName);
	if (it == fList.end())
		throw ErrGDSObj(ERR_ATTR_NAME, OldName.c_str());
	if (OldName != NewName)
	{
		if (HasName(NewName))
			throw ErrGDSObj(ERR_ATTR_NAME_EXIST, NewName.c_str());
		(*it)->name = NewName;
		Changed();
	}
}

void CdObjAttr::SetName(int Index, const UTF8String &NewName)
{
	TdPair &p = *fList.at(Index); // check range
	_ValidateName(NewName);
	if (p.name != NewName)
	{
		if (HasName(NewName))
			throw ErrGDSObj(ERR_ATTR_NAME_EXIST, NewName.c_str());
		p.name = NewName;
		Changed();
	}
}

void CdObjAttr::_ValidateName(const UTF8String &name)
{
	if (name.empty())
        throw ErrGDSObj(ERR_ATTR_INVALID_NAME);
}


// CdGDSObj

static const char *ERR_NO_NAME      = "No name exists!";
static const char *ERR_NO_OBJECT    = "No parent folder.";
static const char *ERR_DUP_NAME     = "Duplicate name!";
static const char *ERR_MOVE_TO_ADD  = "Please call 'AddObj' to add an object.";
static const char *ERR_SAME_FILE    = "'MoveTo' should be within the same GDS file.";
static const char *ERR_MOVETO_CHILD = "Cannot move to its sub folder.";
static const char *ERR_GDS_READONLY = "The GDS file is read-only.";
static const char *ERR_GDSStream    = "%s: GDSStream should not be NULL.";

static const char *ERR_INVALID_ASSIGN  = "Invalid assignment.";
static const char *ERR_INVALID_ASSIGN2 = "Invalid assignment to '%s' from '%s'.";
static const char *ERR_INVALID_ASSIGN3 = "Invalid assignment to '%s'.";
static const char *ERR_INVALID_ASSIGN4 = "Invalid assignment from '%s'.";

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

void CdGDSObj::AssignAttribute(CdGDSObj &Source)
{
	fAttr.Assign(Source.Attribute());
}

UTF8String CdGDSObj::Name() const
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

UTF8String CdGDSObj::FullName() const
{
	const UTF8String Delimiter = "/";
	UTF8String rv = Name();
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

void CdGDSObj::SetName(const UTF8String &NewName)
{
	if (fFolder)
	{
		vector<CdGDSFolder::TNode>::iterator it = fFolder->FindObj(this);
		if (it != fFolder->fList.end())
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
		throw ErrGDSObj(ERR_NO_OBJECT);
	}
	throw ErrGDSObj(ERR_NO_NAME);
}

bool CdGDSObj::GetHidden() const
{
	if (fFolder)
	{
		vector<CdGDSFolder::TNode>::const_iterator it =
			fFolder->FindObj(this);
		if (it != fFolder->fList.end())
		{
			return (it->Flag & CdGDSFolder::TNode::FLAG_ATTR_HIDDEN) != 0;
		}
		throw ErrGDSObj(ERR_NO_OBJECT);
	}
	return false;
}

void CdGDSObj::SetHidden(bool hidden)
{
	if (fFolder)
	{
		vector<CdGDSFolder::TNode>::iterator it = fFolder->FindObj(this);
		if (it != fFolder->fList.end())
		{
			bool flag = (it->Flag & CdGDSFolder::TNode::FLAG_ATTR_HIDDEN) != 0;
			if (flag != hidden)
			{
				if (hidden)
					it->Flag |= CdGDSFolder::TNode::FLAG_ATTR_HIDDEN;
				else
					it->Flag &= ~CdGDSFolder::TNode::FLAG_ATTR_HIDDEN;
				fFolder->fChanged = true;
			}
			return;
		}
		throw ErrGDSObj(ERR_NO_OBJECT);
	}
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
				if (static_cast<CdGDSFolder*>(this)->HasChild(&folder, true))
					throw ErrGDSObj(ERR_MOVETO_CHILD);
			}
			if ((fFolder!=&folder) && (this!=&folder))
			{
				vector<CdGDSFolder::TNode>::iterator it;
				it = fFolder->FindObj(this);
				if (folder._HasName(it->Name))
					throw ErrGDSObj(ERR_DUP_NAME);
				folder.fList.push_back(*it);
				fFolder->fList.erase(it);
				fFolder->fChanged = folder.fChanged = true;
				fFolder = &folder;
			}
		} else
			throw ErrGDSObj(ERR_SAME_FILE);
	} else
		throw ErrGDSObj(ERR_MOVE_TO_ADD);
}

void CdGDSObj::Synchronize()
{
	if (fChanged)
		SaveToBlockStream();
}

void CdGDSObj::GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
{
	Out.clear();
}

void CdGDSObj::GetOwnBlockStream(vector<CdStream*> &Out)
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
	// call the function 'Loading'
	CdObjMsg::LoadStruct(Reader, Version);
	// load attributes
	COREARRAY_READER_CALL_SILENT(fAttr.Loading(Reader, Version));
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
	_CheckGDSStream();
	if (fGDSStream)
	{
		CdWriter Writer(fGDSStream, &GDSFile()->Log());
		SaveStruct(Writer, IsWithClassName());
	}
}

#ifdef COREARRAY_CODE_USING_LOG
string CdGDSObj::LogValue()
{
	if (fFolder)
		return FullName();
	else
		return "/";
}
#endif

void CdGDSObj::_CheckWritable()
{
	CdGDSFile *file = GDSFile();
	if (file && file->ReadOnly())
		throw ErrGDSObj(ERR_GDS_READONLY);
}

void CdGDSObj::_CheckGDSStream()
{
	if (!fGDSStream)
		throw ErrGDSObj(ERR_GDSStream, "CdGDSObj");
}

void CdGDSObj::RaiseInvalidAssign(const char *ThisClass, CdGDSObj *Source)
{
	const char *SourceClass = NULL;
	if (Source)
	{
		SourceClass = Source->dName();
		if (SourceClass && (SourceClass[0]==0))
			SourceClass = NULL;
	}

	if (ThisClass)
	{
		if (SourceClass)
			throw ErrGDSObj(ERR_INVALID_ASSIGN2, ThisClass, SourceClass);
		else
			throw ErrGDSObj(ERR_INVALID_ASSIGN3, ThisClass);
	} else {
		if (SourceClass)
			throw ErrGDSObj(ERR_INVALID_ASSIGN4, SourceClass);
		else
			throw ErrGDSObj(ERR_INVALID_ASSIGN);
	}
}

void CdGDSObj::_GDSObjInitProc(CdObjClassMgr &Sender, CdObject *Obj, void *Data)
{
	if (dynamic_cast<CdGDSObj*>(Obj))
		static_cast<CdGDSObj*>(Obj)->fGDSStream = (CdBlockStream*)Data;
}



// =====================================================================
// Stream Manage Pipe
// =====================================================================

namespace CoreArray
{
	static const char *VAR_PIPE_SIZE   = "PIPE_SIZE";
	static const char *VAR_PIPE_LEVEL  = "PIPE_LEVEL";
	static const char *VAR_PIPE_BKSIZE = "PIPE_BKSIZE";

	static const CdRecodeStream::TLevel CompressionLevels[] =
	{
		CdRecodeStream::clMin,
		CdRecodeStream::clFast,
		CdRecodeStream::clDefault,
		CdRecodeStream::clMax,
		CdRecodeStream::clUltra,     // ultra  (LZMA 512MiB)
		CdRecodeStream::clUltraMax,  // ultra_max  (LZMA 1536MiB)
		CdRecodeStream::clDefault
	};

	static const char *RA_Str_BSize[] =
	{
		"16K", "32K", "64K", "128K",
		"256K", "512K", "1M", "2M",
		"4M", "8M", NULL
	};

	/// The pipe for writing data to a compressed stream
	template<typename CLASS>
		class COREARRAY_DLL_DEFAULT CdWritePipe: public CdStreamPipe
	{
	public:
		CdWritePipe(CdRecodeStream::TLevel vLevel,
				TdCompressRemainder &vRemainder):
			CdStreamPipe(), fRemainder(vRemainder)
		{
			fLevel = vLevel;
		}

	protected:
		CdStream *fStream;
		CLASS *fPStream;
		CdRecodeStream::TLevel fLevel;
		TdCompressRemainder &fRemainder;

		virtual CdStream *InitPipe(CdBufStream *BufStream)
		{
			fStream = BufStream->Stream();
			fPStream = new CLASS(*fStream, fLevel);
			fPStream->PtrExtRec = &fRemainder;
			return fPStream;
		}
		virtual CdStream *FreePipe()
		{
			if (fPStream) { fPStream->Release(); fPStream = NULL; }
			return fStream;
		}
	};

	/// The pipe for writing data to a compressed stream
	template<typename CLASS, typename BSIZE>
		class COREARRAY_DLL_DEFAULT CdWritePipe2: public CdStreamPipe
	{
	public:
		CdWritePipe2(CdRecodeStream::TLevel vLevel, BSIZE bs,
				TdCompressRemainder &vRemainder):
			CdStreamPipe(), fRemainder(vRemainder)
		{
			fLevel = vLevel;
			fBSize = bs;
		}

	protected:
		CdStream *fStream;
		CLASS *fPStream;
		CdRecodeStream::TLevel fLevel;
		BSIZE fBSize;
		TdCompressRemainder &fRemainder;

		virtual CdStream *InitPipe(CdBufStream *BufStream)
		{
			fStream = BufStream->Stream();
			fPStream = new CLASS(*fStream, fLevel, fBSize);
			fPStream->PtrExtRec = &fRemainder;
			return fPStream;
		}
		virtual CdStream *FreePipe()
		{
			if (fPStream) { fPStream->Release(); fPStream = NULL; }
			return fStream;
		}
	};


	/// The pipe system with a template
	template<int MaxBVal, int DefBVal, typename BSIZE,
		typename CLASS, typename TYPE>
		class COREARRAY_DLL_DEFAULT CdPipe: public CdPipeMgrItem2
	{
	private:
		SIZE64 vSizeInfo_Ptr;

	public:
		CdPipe(): CdPipeMgrItem2()
		{
			fLevel = CdRecodeStream::clDefault;
			fBlockSize = (BSIZE)DefBVal;
			vSizeInfo_Ptr = -1;
		}

		virtual CdPipeMgrItem *New()
		{
			CdPipe<MaxBVal, DefBVal, BSIZE, CLASS, TYPE> *rv = new TYPE();
			rv->fCoderIndex = fCoderIndex;
			rv->fParamIndex = fParamIndex;
			rv->fLevel = fLevel;
			rv->fBlockSize = fBlockSize;
			return rv;
		}

		virtual void PopPipe(CdBufStream &buf)
		{
			buf.PopPipe();
		}

		virtual bool WriteMode(CdBufStream &buf) const
		{
			return (dynamic_cast<CLASS*>(buf.Stream()) != NULL);
		}

		virtual void ClosePipe(CdBufStream &buf)
		{
			CLASS *s = dynamic_cast<CLASS*>(buf.Stream());
			if (s) s->Close();
        }

		virtual bool GetStreamInfo(CdBufStream *buf)
		{
			SIZE64 in, out;
			if (buf)
			{
				CLASS *s = dynamic_cast<CLASS*>(buf->Stream());
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
		CdRecodeStream::TLevel fLevel;
		BSIZE fBlockSize;

		virtual CdPipeMgrItem *Match(const char *Mode) const
		{
			int ic, ip;
			ParseMode(Mode, ic, ip);
			if (ic >= 0)
			{
				CdPipe<MaxBVal, DefBVal, BSIZE, CLASS, TYPE> *rv = new TYPE();
				rv->fLevel = CompressionLevels[ic];
				if (ip < 0) ip = DefBVal;
				rv->fBlockSize = (BSIZE)ip;
				rv->fCoderIndex = rv->fLevel;
				rv->fParamIndex = ip;
				return rv;
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
			// pipe size
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

			// pipe level
			if (Reader.HaveProperty(VAR_PIPE_LEVEL))
			{
				C_UInt8 I = 0;
				Reader[VAR_PIPE_LEVEL] >> I;
				if (I > 5)
				{
					// Since clNone=0, clFast=1, clDefault=2, clMax=3
					// clUltra=4, clUltraMax=5
					throw ErrGDSObj("Invalid 'PIPE_LEVEL %d'", I);
				}
				fLevel = (CdRecodeStream::TLevel)I;
			} else
				fLevel = CdRecodeStream::clUnknown;
			fCoderIndex = (int)fLevel;

			// pipe block size
			if (MaxBVal > 0)
			{
				if (Reader.HaveProperty(VAR_PIPE_BKSIZE))
				{
					C_UInt8 I = 0;
					Reader[VAR_PIPE_BKSIZE] >> I;
					if ((int)I > MaxBVal)
	            	    throw ErrGDSObj("Invalid 'PIPE_BKSIZE %d'", I);
					fBlockSize = (BSIZE)I;
				} else
					fBlockSize = (BSIZE)(-1);
				fParamIndex = (int)fBlockSize;
			}
		}
		virtual void SaveStream(CdWriter &Writer)
		{
			UpdateStreamSize();
			C_Int64 Ary[2] = { fStreamTotalIn, fStreamTotalOut };
			Writer[VAR_PIPE_SIZE].NewShortRec(Ary, 2);
			vSizeInfo_Ptr = Writer.PropPosition(VAR_PIPE_SIZE);
			Writer[VAR_PIPE_LEVEL] << C_UInt8(fLevel);
			if (MaxBVal > 0)
				Writer[VAR_PIPE_BKSIZE] << C_UInt8(fBlockSize);
		}
	};
}


namespace CoreArray
{
	// =====================================================================
	// ZIP: ZIP Pipe
	// =====================================================================

	typedef CdStreamPipe2<CdZDecoder> CdZIPReadPipe;
	typedef CdWritePipe<CdZEncoder> CdZIPWritePipe;

	static const char *ZIP_Strings[] =
	{
		"ZIP.min", "ZIP.fast", "ZIP.def", "ZIP.max", "", "", "ZIP", NULL
	};

	class COREARRAY_DLL_DEFAULT CdPipeZIP:
		public CdPipe<0, -1, int, CdZEncoder, CdPipeZIP>
	{
	public:
		virtual const char *Coder() const
			{ return "ZIP"; }
		virtual const char *Description() const
			{ return "zlib_" ZLIB_VERSION; }
		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZIPReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZIPWritePipe(fLevel, fRemainder)); }

	protected:
		virtual const char **CoderList() const { return ZIP_Strings; }
		virtual const char **ParamList() const { return NULL; }
	};


	// =====================================================================
	// ZRA: ZIP Pipe with the support of random access
	// =====================================================================

	typedef CdStreamPipe2<CdZDecoder_RA> CdZRAReadPipe;
	typedef CdWritePipe2<CdZEncoder_RA, CdRAAlgorithm::TBlockSize> CdZRAWritePipe;

	static const char *ZRA_Strings[] =
	{
		"ZIP_RA.min", "ZIP_RA.fast", "ZIP_RA.def", "ZIP_RA.max",
		"", "", "ZIP_RA", NULL
	};

	class COREARRAY_DLL_DEFAULT CdPipeZRA:
		public CdPipe<CdRAAlgorithm::raLast, CdRAAlgorithm::raDefault,
		CdRAAlgorithm::TBlockSize, CdZEncoder_RA, CdPipeZRA>
	{
	public:
		virtual const char *Coder() const
			{ return "ZIP_ra"; }
		virtual const char *Description() const
			{ return "zlib_" ZLIB_VERSION " (random access)"; }
		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZRAReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdZRAWritePipe(fLevel, fBlockSize, fRemainder)); }

	protected:
		virtual const char **CoderList() const { return ZRA_Strings; }
		virtual const char **ParamList() const { return RA_Str_BSize; }
	};


	// =====================================================================
	// LZ4: LZ4 Pipe
	// =====================================================================

#ifndef COREARRAY_NO_LZ4

	typedef CdStreamPipe2<CdLZ4Decoder> CdLZ4ReadPipe;
	typedef CdWritePipe2<CdLZ4Encoder, CdBaseLZ4Stream::TLZ4Chunk> CdLZ4WritePipe;

	static const char *LZ4_Strings[] =
	{
		"LZ4.min", "LZ4.fast", "LZ4.hc", "LZ4.max", "", "", "LZ4", NULL
	};
	static const char *LZ4_Str_BSize[] =
		{ "64K", "256K", "1M", "4M", NULL };

	class COREARRAY_DLL_DEFAULT CdPipeLZ4:
		public CdPipe<CdBaseLZ4Stream::bsLast, CdBaseLZ4Stream::bsDefault,
		CdBaseLZ4Stream::TLZ4Chunk, CdLZ4Encoder, CdPipeLZ4>
	{
	public:
		virtual const char *Coder() const
			{ return "LZ4"; }
		virtual const char *Description() const
		{
			static char LZ4_TEXT[] = "lz4_v?.?.?";
			LZ4_TEXT[5] = '0' + LZ4_VERSION_MAJOR;
			LZ4_TEXT[7] = '0' + LZ4_VERSION_MINOR;
			LZ4_TEXT[9] = '0' + LZ4_VERSION_RELEASE;
			return LZ4_TEXT;
		}
		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdLZ4ReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdLZ4WritePipe(fLevel, fBlockSize, fRemainder)); }

	protected:
		virtual const char **CoderList() const { return LZ4_Strings; }
		virtual const char **ParamList() const { return LZ4_Str_BSize; }
	};


	// =====================================================================
	// LZ4: LZ4 Pipe with random access
	// =====================================================================

	typedef CdStreamPipe2<CdLZ4Decoder_RA> CdLZ4RAReadPipe;
	typedef CdWritePipe2<CdLZ4Encoder_RA, CdRAAlgorithm::TBlockSize> CdLZ4RAWritePipe;

	static const char *LZ4RA_Strings[] =
	{
		"LZ4_RA.min", "LZ4_RA.fast", "LZ4_RA.hc", "LZ4_RA.max",
		"", "", "LZ4_RA", NULL
	};

	class COREARRAY_DLL_DEFAULT CdPipeLZ4RA:
		public CdPipe<CdRAAlgorithm::raLast, CdRAAlgorithm::raDefault,
		CdRAAlgorithm::TBlockSize, CdLZ4Encoder_RA, CdPipeLZ4RA>
	{
	public:
		virtual const char *Coder() const
			{ return "LZ4_ra"; }
		virtual const char *Description() const
		{
			static char LZ4_TEXT[] = "lz4_v?.?.? (random access)";
			LZ4_TEXT[5] = '0' + LZ4_VERSION_MAJOR;
			LZ4_TEXT[7] = '0' + LZ4_VERSION_MINOR;
			LZ4_TEXT[9] = '0' + LZ4_VERSION_RELEASE;
			return LZ4_TEXT;
		}
		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdLZ4RAReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdLZ4RAWritePipe(fLevel, fBlockSize, fRemainder)); }

	protected:
		virtual const char **CoderList() const { return LZ4RA_Strings; }
		virtual const char **ParamList() const { return RA_Str_BSize; }
	};

#endif


	// =====================================================================
	// XZ: xz stream
	// =====================================================================

#ifndef COREARRAY_NO_LZMA

	typedef CdStreamPipe2<CdXZDecoder> CdXZReadPipe;
	typedef CdWritePipe<CdXZEncoder> CdXZWritePipe;

	static const char *XZ_Strings[] =
	{
		"LZMA.min", "LZMA.fast", "LZMA.def", "LZMA.max",
		"LZMA.ultra", "LZMA.ultra_max", "LZMA", NULL
	};

	class COREARRAY_DLL_DEFAULT CdPipeXZ:
		public CdPipe<0, -1, int, CdXZEncoder, CdPipeXZ>
	{
	public:
		virtual const char *Coder() const
			{ return "LZMA"; }
		virtual const char *Description() const
			{ return "xz_" LZMA_VERSION_STRING; }
		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdXZReadPipe); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdXZWritePipe(fLevel, fRemainder)); }

	protected:
		virtual const char **CoderList() const { return XZ_Strings; }
		virtual const char **ParamList() const { return NULL; }
	};


	// =====================================================================
	// XZ_RA: XZ Pipe with the support of random access
	// =====================================================================

	typedef CdStreamPipe2<CdXZDecoder_RA> CdXZReadPipe_RA;
	typedef CdWritePipe2<CdXZEncoder_RA, CdRAAlgorithm::TBlockSize> CdXZWritePipe_RA;

	static const char *XZ_RA_Strings[] =
	{
		"LZMA_RA.min", "LZMA_RA.fast", "LZMA_RA.def", "LZMA_RA.max",
		"LZMA_RA.ultra", "LZMA_RA.ultra_max", "LZMA_RA", NULL
	};

	class COREARRAY_DLL_DEFAULT CdPipeXZ_RA:
		public CdPipe<CdRAAlgorithm::raLast, CdRAAlgorithm::raDefault,
		CdRAAlgorithm::TBlockSize, CdXZEncoder_RA, CdPipeXZ_RA>
	{
	public:
		virtual const char *Coder() const
			{ return "LZMA_ra"; }
		virtual const char *Description() const
			{ return "xz_" LZMA_VERSION_STRING " (random access)"; }
		virtual void PushReadPipe(CdBufStream &buf)
			{ buf.PushPipe(new CdXZReadPipe_RA); }
		virtual void PushWritePipe(CdBufStream &buf)
			{ buf.PushPipe(new CdXZWritePipe_RA(fLevel, fBlockSize, fRemainder)); }

	protected:
		virtual const char **CoderList() const { return XZ_RA_Strings; }
		virtual const char **ParamList() const { return RA_Str_BSize; }
	};

#endif

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

void CdPipeMgrItem::LoadStream(CdReader &Reader, TdVersion Version) { }

void CdPipeMgrItem::SaveStream(CdWriter &Writer) { }

bool CdPipeMgrItem::EqualText(const char *s1, const char *s2)
{
	for (;*s1 || *s2; s1++, s2++)
	{
		if (toupper(*s1) != toupper(*s2))
			return false;
	}
	return true;
}


// CdPipeMgrItem2

CdPipeMgrItem2::CdPipeMgrItem2(): CdPipeMgrItem()
{
	fCoderIndex = fParamIndex = -1;
}

string CdPipeMgrItem2::CoderOptString() const
{
	string rv;
	const char **ss = CoderList();
	for (; *ss; ss++)
	{
		if (strlen(*ss) > 0)
		{
			if (!rv.empty()) rv.append(", ");
			rv.append(*ss);
		}
	}
	return rv;
}

string CdPipeMgrItem2::ExtOptString() const
{
	string rv;
	const char **ss = ParamList();
	if (ss)
	{
		for (; *ss; ss++)
		{
			if (strlen(*ss) > 0)
			{
				if (!rv.empty()) rv.append(", ");
				rv.append(":");
				rv.append(*ss);
			}
		}
	}
	return rv;
}

bool CdPipeMgrItem2::Equal(const char *Mode) const
{
	int ic, ip;
	ParseMode(Mode, ic, ip);
	if (fCoderIndex >= 0)
		return (fCoderIndex == ic) && (fParamIndex == ip);
	else
		return false;
}

string CdPipeMgrItem2::CoderParam() const
{
	string ans;
	if (fCoderIndex >= 0)
		ans.append(CoderList()[fCoderIndex]);
	if (fParamIndex >= 0)
	{
		ans.append(":");
		ans.append(ParamList()[fParamIndex]);
	}
	return ans;
}

void CdPipeMgrItem2::ParseMode(const char *Mode, int &IdxCoder,
	int &IdxParam) const
{
	IdxCoder = IdxParam = -1;

	string s = Mode;
	size_t pos = s.find(':');
	if (pos != string::npos)
	{
		s.resize(pos);
		Mode += (pos + 1);
	} else
		Mode = NULL;

	const char **ss = CoderList();
	for (int i=0; *ss != NULL; ss++, i++)
	{
		if (strlen(*ss)>0 && EqualText(s.c_str(), *ss))
		{
			IdxCoder = i;
			break;
		}
	}
	if (IdxCoder < 0) return;

	ss = ParamList();
	if ((ss != NULL) && (Mode != NULL))
	{
		for (int i=0; *ss != NULL; ss++, i++)
		{
			if (strlen(*ss)>0 && EqualText(Mode, *ss))
			{
				IdxParam = i;
				break;
			}
		}
		if (IdxParam < 0) IdxCoder = -1;
	}
}



// CdStreamPipeMgr

CdStreamPipeMgr CoreArray::dStreamPipeMgr;

CdStreamPipeMgr::CdStreamPipeMgr(): CdAbstractManager()
{
	Register(new CdPipeZIP);
	Register(new CdPipeZRA);
#ifndef COREARRAY_NO_LZ4
	Register(new CdPipeLZ4);
	Register(new CdPipeLZ4RA);
#endif
#ifndef COREARRAY_NO_LZMA
	Register(new CdPipeXZ);
	Register(new CdPipeXZ_RA);
#endif
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
	{
		delete fPipeInfo;
		fPipeInfo = NULL;
	}
}

CdGDSObjPipe *CdGDSObjPipe::AssignPipe(CdGDSObjPipe &Source)
{
	if (fPipeInfo)
	{
		delete fPipeInfo;
		fPipeInfo = NULL;
	}
	if (Source.fPipeInfo)
		fPipeInfo = Source.fPipeInfo->New();
	fChanged = true;
	return this;
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

CdGDSObj *CdGDSLabel::NewObject()
{
	return new CdGDSLabel;
}

void CdGDSLabel::Assign(CdGDSObj &Source, bool Full)
{
	if (dynamic_cast<CdGDSLabel*>(&Source))
	{
		if (Full)
			AssignAttribute(Source);
	} else
		RaiseInvalidAssign("CdGDSLabel", &Source);
}


// =====================================================================
// CdGDSFolder
// =====================================================================

static const char *ERR_NAME_EXIST   = "The GDS node \"%s\" exists.";
static const char *ERR_NAME_INVALID = "The GDS node name \"%s\" should not contain '/' or '\x0'.";
static const char *ERR_FOLDER_ITEM  = "Invalid index %d.";
static const char *ERR_FOLDER_NAME  = "Invalid node name \"%s\".";
static const char *ERR_NO_FOLDER    = "There is not a folder named \"%s\".";
static const char *ERR_OBJ_INDEX    = "Invalid object index %d in the folder.";
static const char *ERR_INVALID_ASSOC = "The object has been associated with a GDS file!";
static const char *ERR_INVALID_PATH  = "The GDS node \"%s\" does not exist.";
static const char *ERR_INVALID_INDEX = "%s(), invalid 'Index' %d.";

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

bool CdGDSFolder::TNode::IsFlagAttr(C_UInt32 val) const
{
	return (Flag & FLAG_ATTR_MASK) == (val & FLAG_ATTR_MASK);
}

void CdGDSFolder::TNode::SetFlagAttr(C_UInt32 val)
{
	Flag &= ~FLAG_ATTR_MASK;
	Flag |= (val & FLAG_ATTR_MASK);
}


CdGDSFolder::~CdGDSFolder()
{
    _ClearFolder();
}

CdGDSObj *CdGDSFolder::NewObject()
{
    return new CdGDSFolder;
}

void CdGDSFolder::Assign(CdGDSObj &Source, bool Full)
{
	if (dynamic_cast<CdGDSAbsFolder*>(&Source))
	{
		if (Full)
			AssignAttribute(Source);
		AssignFolder(*static_cast<CdGDSAbsFolder*>(&Source));
	} else
		RaiseInvalidAssign("CdGDSFolder", &Source);
}

void CdGDSFolder::AssignFolder(CdGDSAbsFolder &Source)
{
	for (int i=0; i < Source.NodeCount(); i++)
	{
		CdGDSObj *obj = Source.ObjItem(i);
		CdGDSObj *dst = obj->NewObject();
		AddObj(obj->Name(), dst);
		dst->Assign(*obj, true);
	}
}

CdGDSObj *CdGDSFolder::AddFolder(const UTF8String &Name)
{
	_CheckWritable();
	_CheckGDSStream();

	if (!_ValidName(Name))
		throw ErrGDSObj(ERR_NAME_INVALID, Name.c_str());
	if (_HasName(Name))
		throw ErrGDSObj(ERR_NAME_EXIST, Name.c_str());

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

CdGDSObj *CdGDSFolder::AddObj(const UTF8String &Name, CdGDSObj *val)
{
	return InsertObj(-1, Name, val);
}

CdGDSObj *CdGDSFolder::InsertObj(int index, const UTF8String &Name,
	CdGDSObj *val)
{
	static const char *ERR_INSERT_DIFF_OWNER =
		"CdGDSFolder::InsertObj, 'val' has a different owner.";

	if ((index < -1) || (index > (int)fList.size()))
		throw ErrGDSObj(ERR_INVALID_INDEX, "CdGDSFolder::InsertObj", index);
	if (val && (val->fFolder!=NULL) && (val->fFolder!=this))
		throw ErrGDSObj(ERR_INSERT_DIFF_OWNER);

	_CheckWritable();
	_CheckGDSStream();

	if (!_ValidName(Name))
		throw ErrGDSObj(ERR_NAME_INVALID, Name.c_str());
	if (_HasName(Name))
		throw ErrGDSObj(ERR_NAME_EXIST, Name.c_str());

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
		throw ErrGDSObj(ERR_INVALID_ASSOC);

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
	static const char *ERR_MOVETO_INVALID_NEWPOS =
		"CdGDSFolder::MoveTo, invalid 'NewPos' %d.";

	if ((Index < -1) || (Index >= (int)fList.size()))
		throw ErrGDSObj(ERR_INVALID_INDEX, "CdGDSFolder::MoveTo", Index);
	if ((NewPos < -1) || (NewPos >= (int)fList.size()))
		throw ErrGDSObj(ERR_MOVETO_INVALID_NEWPOS, NewPos);
	_CheckWritable();

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

void CdGDSFolder::UnloadObj(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(ERR_OBJ_INDEX, Index);

	vector<TNode>::iterator it = fList.begin() + Index;
	if (it->Obj)
	{
		static const char *ERR_IS_FOLDER = "Not allowed to unload a folder.";
		static const char *ERR_UNLOAD = "Fails to unload %p.";

		if (dynamic_cast<CdGDSAbsFolder*>(it->Obj))
			throw ErrGDSObj(ERR_IS_FOLDER);
	#ifdef COREARRAY_CODE_DEBUG
		if (it->Obj->Release() != 0)
			throw ErrGDSObj(ERR_UNLOAD, (void*)(it->Obj));
	#else
		it->Obj->Release();
	#endif
		it->Obj = NULL;
	}
}

void CdGDSFolder::UnloadObj(CdGDSObj *val)
{
	if (val == NULL) return;
	vector<CdGDSFolder::TNode>::iterator it;
	int Index = 0;
	for (it = fList.begin(); it != fList.end(); it++, Index++)
	{
		if (it->Obj == val)
		{
			UnloadObj(Index);
			return;
		}
	}
	throw ErrGDSObj();
}

void CdGDSFolder::DeleteObj(int Index, bool force)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(ERR_OBJ_INDEX, Index);
	_CheckWritable();

	vector<TNode>::iterator it = fList.begin() + Index;
	_LoadItem(*it);

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
				static const char *ERR_REMOVE_FIRST =
					"Please delete the item(s) in the folder before removing it.";
				throw ErrGDSObj(ERR_REMOVE_FIRST);
			}
			folder->ClearObj(force);
		}

	#ifdef COREARRAY_CODE_DEBUG
		if (it->Obj->Release() != 0)
			throw ErrGDSObj(ERR_OBJ_RELEASE, "CdGDSFolder::DeleteObj");
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
	_CheckWritable();

	vector<CdGDSObj *> lst;
	for (size_t i=0; i < fList.size(); i++)
		lst.push_back(ObjItem(i));

	for (size_t i=0; i < lst.size(); i++)
		DeleteObj(lst[i], force);	
}

CdGDSFolder & CdGDSFolder::DirItem(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(ERR_FOLDER_ITEM, Index);
	CdGDSFolder::TNode &I = fList[Index];
	_LoadItem(I);
	if (dynamic_cast<CdGDSFolder*>(I.Obj))
		return *static_cast<CdGDSFolder*>(I.Obj);
	else
    	throw ErrGDSObj(ERR_NO_FOLDER, I.Name.c_str());
}

CdGDSFolder & CdGDSFolder::DirItem(const UTF8String &Name)
{
	CdGDSFolder::TNode &I = _NameItem(Name);
	_LoadItem(I);
	if (dynamic_cast<CdGDSFolder*>(I.Obj))
		return *static_cast<CdGDSFolder*>(I.Obj);
	else
    	throw ErrGDSObj(ERR_NO_FOLDER, I.Name.c_str());
}

CdGDSObj *CdGDSFolder::ObjItem(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		throw ErrGDSObj(ERR_OBJ_INDEX, Index);
	CdGDSFolder::TNode &I = fList[Index];
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj *CdGDSFolder::ObjItem(const UTF8String &Name)
{
	CdGDSFolder::TNode &I = _NameItem(Name);
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj *CdGDSFolder::ObjItemEx(int Index)
{
	if ((Index < 0) || (Index >= (int)fList.size()))
		return NULL;
	CdGDSFolder::TNode &I = fList[Index];
	_LoadItem(I);
	return I.Obj;
}

CdGDSObj *CdGDSFolder::ObjItemEx(const UTF8String &Name)
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

CdGDSObj *CdGDSFolder::Path(const UTF8String &FullName)
{
	CdGDSObj *rv = PathEx(FullName);
	if (!rv)
		throw ErrGDSObj(ERR_INVALID_PATH, FullName.c_str());
	return rv;
}

CdGDSObj *CdGDSFolder::PathEx(const UTF8String &FullName)
{
	const char delimit = '/';
	const char *p = FullName.c_str();

	CdGDSObj *rv = this;
	while ((*p) && (rv))
	{
		if (!dynamic_cast<CdGDSAbsFolder*>(rv))
			return NULL;
		if (*p == delimit) p ++;

		const char *s = p;
		while ((*p != delimit) && (*p != 0))
			p ++;
		if (s == p)
			return rv;
		rv = ((CdGDSAbsFolder*)rv)->ObjItemEx(UTF8String(s, p));
	}

	return rv;
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

bool CdGDSFolder::HasChild(CdGDSObj *Obj, bool Recursive)
{
	if (Obj == NULL) return false;

	vector<TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
	{
		if (it->Obj == Obj) return true;
		if (dynamic_cast<CdGDSAbsFolder*>(it->Obj) && Recursive)
		{
			if (dynamic_cast<CdGDSAbsFolder*>(it->Obj)->HasChild(Obj, Recursive))
				return true;
        }
	}
	return false;
}

int CdGDSFolder::NodeCount()
{
	return fList.size();
}

static const char *VAR_DIRCNT   = "DIRCNT";
static const char *VAR_DIRLIST  = "DIRLIST";
static const char *VAR_DIR_ID   = "ID";
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
				throw ErrGDSObj(ERR_OBJ_RELEASE, "CdGDSFolder::_ClearFolder");
		#else
			it->Obj->Release();
		#endif
		}
	}
	fList.clear();
}

bool CdGDSFolder::_HasName(const UTF8String &Name)
{
	vector<CdGDSFolder::TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Name == Name)
			return true;
	return false;
}

bool CdGDSFolder::_ValidName(const UTF8String &Name)
{
	for (size_t i=0; i < Name.size(); i++)
	{
		char ch = Name[i];
		if (ch=='/' || ch=='\x0') return false;
	}
	return true;
}

CdGDSFolder::TNode &CdGDSFolder::_NameItem(const UTF8String &Name)
{
	vector<CdGDSFolder::TNode>::iterator it;
	for (it = fList.begin(); it != fList.end(); it++)
		if (it->Name == Name)
			return *it;
	throw ErrGDSObj(ERR_FOLDER_NAME, Name.c_str());
}

void CdGDSFolder::_LoadItem(TNode &I)
{
	static const char *ERR_INVALID_GDS_OBJ =
		"Invalid GDS object (it should be inherited from CdGDSObj).";

	if (I.Obj == NULL)
	{
		_CheckGDSStream();
		CdBlockStream *IStream = fGDSStream->Collection()[I.StreamID];
		CdReader Reader(IStream, &GDSFile()->Log());

		if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_LABEL))
		{
			// it is a label
			CdGDSLabel *vObj = new CdGDSLabel;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader.BeginNameSpace();
			_INTERNAL::CdObject_LoadStruct(*vObj, Reader, 0x100);
			Reader.EndStruct();

			/// todo: check
			vObj->fGDSStream = IStream;
			IStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_FOLDER))
		{
			// it is a GDS folder
			CdGDSFolder *vObj = new CdGDSFolder;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader.BeginNameSpace();
			vObj->LoadStruct(Reader, COREARRAY_CLASS_VERSION);
			Reader.EndStruct();

			/// todo: check
			vObj->fGDSStream = IStream;
			IStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_VIRTUAL_FOLDER))
		{
			// it is a virtual folder linking to another GDS file
			CdGDSVirtualFolder *vObj = new CdGDSVirtualFolder;
			vObj->fFolder = this;
			I.Obj = vObj;

			Reader.BeginNameSpace();
			vObj->LoadStruct(Reader, COREARRAY_CLASS_VERSION);
			Reader.EndStruct();

			/// todo: check
			vObj->fGDSStream = IStream;
			IStream->AddRef();

		} else if (I.IsFlagType(CdGDSFolder::TNode::FLAG_TYPE_STREAM))
		{
			// it is a stream container
			CdGDSStreamContainer *vObj = new CdGDSStreamContainer;
			vObj->fFolder = this;
			I.Obj = vObj;

			/// todo: check
			vObj->fGDSStream = IStream;
			IStream->AddRef();

			Reader.BeginNameSpace();
			vObj->LoadStruct(Reader, COREARRAY_CLASS_VERSION);
			Reader.EndStruct();

		} else {
			// it is a class object
			CdObjRef *obj = NULL;

			try{
				obj = fGDSStream->Collection().ClassMgr()->
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
				I.Obj->fGDSStream = IStream;
				IStream->AddRef();
			} else {
				if (obj) delete obj;
				throw ErrGDSObj(ERR_INVALID_GDS_OBJ);
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
			{
				static_cast<CdGDSFolder*>(it->Obj)->_UpdateAll();
			} else {
				it->Obj->Synchronize();
			}
		}
	}
}

vector<CdGDSFolder::TNode>::iterator CdGDSFolder::FindObj(CdGDSObj *Obj)
{
	vector<TNode>::iterator it  = fList.begin();
	vector<TNode>::iterator end = fList.end();
	for (; it != end; it++)
	{
		if (it->Obj == Obj) break;
	}
	return it;
}

vector<CdGDSFolder::TNode>::const_iterator CdGDSFolder::FindObj(
	const CdGDSObj *Obj) const
{
	vector<TNode>::const_iterator it  = fList.begin();
	vector<TNode>::const_iterator end = fList.end();
	for (; it != end; it++)
	{
		if (it->Obj == Obj) break;
	}
	return it;
}


// CdGDSVirtualFolder

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

CdGDSObj *CdGDSVirtualFolder::NewObject()
{
	return new CdGDSVirtualFolder;
}

void CdGDSVirtualFolder::Assign(CdGDSObj &Source, bool Full)
{
	if (dynamic_cast<CdGDSLabel*>(&Source))
	{
		if (Full)
			AssignAttribute(Source);

		CdGDSVirtualFolder *S = static_cast<CdGDSVirtualFolder*>(&Source);
		fLinkFileName = S->fLinkFileName;
		fErrMsg = S->fErrMsg;
		fHasTried = false;
		if (fLinkFile)
		{
			delete fLinkFile;
			fLinkFile = NULL;
		}
	} else
		RaiseInvalidAssign("CdGDSVirtualFolder", &Source);
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
	{
		static const char *ERR_FAIL_LINK = "Fail to link the GDS file '%s'.";
		throw ErrGDSObj(ERR_FAIL_LINK, fLinkFileName.c_str());
	}
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

CdGDSObj *CdGDSVirtualFolder::AddFolder(const UTF8String &Name)
{
	_CheckLinked();
	return fLinkFile->Root().AddFolder(Name);
}

CdGDSObj *CdGDSVirtualFolder::AddObj(const UTF8String &Name, CdGDSObj *val)
{
	_CheckLinked();
	return fLinkFile->Root().AddObj(Name, val);
}

CdGDSObj *CdGDSVirtualFolder::InsertObj(int index, const UTF8String &Name,
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

void CdGDSVirtualFolder::UnloadObj(int Index)
{
	_CheckLinked();
	fLinkFile->Root().UnloadObj(Index);
}

void CdGDSVirtualFolder::UnloadObj(CdGDSObj *val)
{
	_CheckLinked();
	fLinkFile->Root().UnloadObj(val);
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

CdGDSObj *CdGDSVirtualFolder::ObjItem(const UTF8String &Name)
{
	_CheckLinked();
	return fLinkFile->Root().ObjItem(Name);
}

CdGDSObj *CdGDSVirtualFolder::ObjItemEx(int Index)
{
	_CheckLinked();
	return fLinkFile->Root().ObjItemEx(Index);
}

CdGDSObj *CdGDSVirtualFolder::ObjItemEx(const UTF8String &Name)
{
	_CheckLinked();
	return fLinkFile->Root().ObjItemEx(Name);
}

CdGDSObj *CdGDSVirtualFolder::Path(const UTF8String &FullName)
{
	_CheckLinked();
	return fLinkFile->Root().Path(FullName);
}

CdGDSObj *CdGDSVirtualFolder::PathEx(const UTF8String &FullName)
{
	_CheckLinked();
	return fLinkFile->Root().PathEx(FullName);
}

int CdGDSVirtualFolder::IndexObj(CdGDSObj *Obj)
{
	_CheckLinked();
	return fLinkFile->Root().IndexObj(Obj);
}

bool CdGDSVirtualFolder::HasChild(CdGDSObj *Obj, bool Recursive)
{
	_CheckLinked();
	return fLinkFile->Root().HasChild(Obj, Recursive);
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

CdGDSObj *CdGDSStreamContainer::NewObject()
{
	return (new CdGDSStreamContainer)->AssignPipe(*this);
}

void CdGDSStreamContainer::Assign(CdGDSObj &Source, bool Full)
{
	if (dynamic_cast<CdGDSStreamContainer*>(&Source))
	{
		if (Full)
		{
			AssignAttribute(Source);
		}
		CdGDSStreamContainer *S = static_cast<CdGDSStreamContainer*>(&Source);
		S->CloseWriter();
		S->CopyTo(*BufStream());
		CloseWriter();
	} else
		RaiseInvalidAssign("CdGDSStreamContainer", &Source);
}

void CdGDSStreamContainer::Loading(CdReader &Reader, TdVersion Version)
{
	CdGDSObjPipe::Loading(Reader, Version);

	if (fGDSStream)
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
		throw ErrGDSStreamContainer(ERR_GDSStream, "CdGDSStreamContainer");
	}
}

void CdGDSStreamContainer::Saving(CdWriter &Writer)
{
	CdGDSObjPipe::Saving(Writer);

	if (fGDSStream)
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
		throw ErrGDSStreamContainer(ERR_GDSStream, "CdGDSStreamContainer");
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
	static const char *ERR_PACKED_MODE =
		"Invalid packed/compression method '%s'.";

	_CheckWritable();

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
				CdStream *TmpStream = new CdTempStream;
				TdAutoRef<CdBufStream> Output(new CdBufStream(TmpStream));
				if (fPipeInfo)
					fPipeInfo->PushWritePipe(*Output);

				CopyTo(*Output, TotalSize);
				Output.get()->FlushWrite();
				if (fPipeInfo)
				{
					fPipeInfo->ClosePipe(*Output);
					fPipeInfo->GetStreamInfo(Output.get());
				}

				// copy
				vAllocStream->SetPosition(0);
				vAllocStream->SetSizeOnly(0);
				vAllocStream->CopyFrom(*TmpStream, 0, -1);
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

void CdGDSStreamContainer::CopyFromBuf(CdBufStream &Source, SIZE64 Count)
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

void CdGDSStreamContainer::GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
{
	Out.clear();
	if (vAllocStream) Out.push_back(vAllocStream);
}

void CdGDSStreamContainer::GetOwnBlockStream(vector<CdStream*> &Out)
{
	Out.clear();
	if (vAllocStream) Out.push_back(vAllocStream);
}


// CdGDSRoot

CdGDSRoot::CdGDSRoot(): CdGDSFolder()
{
	fVFolder = NULL;
}

UTF8String CdGDSRoot::Name() const
{
	if (fVFolder)
	{
		return fVFolder->Name();
	} else {
		return UTF8String();
	}
}

void CdGDSRoot::SetName(const UTF8String &NewName)
{
	static const char *ERR_ROOT_NAME =
		"The root of a GDS file is not allowed to have a name.";
	if (fVFolder)
		fVFolder->SetName(NewName);
	else
		throw ErrGDSFile(ERR_ROOT_NAME);
}



// =====================================================================
// CdGDSUnknown
// =====================================================================

CdGDSObj *CdGDSUnknown::NewObject()
{
	return new CdGDSUnknown;
}

void CdGDSUnknown::Assign(CdGDSObj &Source, bool Full)
{
	if (dynamic_cast<CdGDSUnknown*>(&Source))
	{
		if (Full)
			AssignAttribute(Source);
	} else
		RaiseInvalidAssign("CdGDSUnknown", &Source);
}

void CdGDSUnknown::SaveStruct(CdWriter &Writer, bool IncludeName)
{
	// do nothing!
}



// =====================================================================
// CdGDSFile
// =====================================================================

static const char *ERR_GDS_OPEN_MODE = "Invalid open mode in CdGDSFile.";
static const char *ERR_GDS_MAGIC     = "Invalid magic number!";
static const char *ERR_GDS_ENTRY     = "Invalid entry point(0x%04X).";
static const char *ERR_GDS_SAVE      = "Should save it to a GDS file first!";

#ifdef COREARRAY_CODE_DEBUG
static const char *ERR_GDS_STREAM    = "The GDS file has been saved.";
#endif

const char *CdGDSFile::GDSFilePrefix()
{
	return COREARRAY_FILE_PREFIX;
}

void CdGDSFile::_Init()
{
	fVersion = COREARRAY_FILE_VERSION;
	fRoot.AddRef();
	fCodeStart = strlen(GDSFilePrefix()) + sizeof(TdVersion) + GDS_BLOCK_ID_SIZE;
	fReadOnly = false;
	fLog = new CdLogRecord; fLog->AddRef();
	fprocess_id = GetCurrentProcessID();
}

CdGDSFile::CdGDSFile(): CdBlockCollection()
{
	_Init();
}

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
			throw ErrGDSFile(ERR_GDS_OPEN_MODE);
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
			throw ErrGDSFile(ERR_GDS_OPEN_MODE);
	}
}

CdGDSFile::~CdGDSFile()
{
	CloseFile();
	if (fLog) fLog->Release();
}

void CdGDSFile::LoadStream(CdStream *Stream, bool ReadOnly, bool AllowError)
{
	// Initialize
	CloseFile();
	fLog->List().clear();
	fReadOnly = ReadOnly;

	// Check the prefix
	const char *prefix = GDSFilePrefix();
	const size_t L = strlen(prefix);  // should be > 0 always
	vector<char> buf(L);
	Stream->ReadData((void*)&buf[0], L);
	if (memcmp((void*)prefix, (void*)&buf[0], L) !=0)
		throw ErrGDSFile(ERR_GDS_MAGIC);

	// Load Version
	fVersion = Stream->R8b();
	fVersion |= Stream->R8b() << 8;

#ifdef COREARRAY_CODE_USING_LOG
	Log().Add(CdLogRecord::LOG_INFO, "Open a GDS file (File Version: v%d.%d).",
		int(fVersion >> 8), int(fVersion & 0xFF));
#endif

	// The entry of stream ID
	TdGDSBlockID Entry;
	BYTE_LE<CdStream>(Stream) >> Entry;

	// Block construction
	CdBlockCollection::LoadStream(Stream, ReadOnly, AllowError, &Log());

#ifdef COREARRAY_CODE_USING_LOG
	Log().Add(CdLogRecord::LOG_INFO,
		"Load all data stream (%d in total) with an entry id (0x%04X).",
		(int)BlockList().size(), Entry.Get());
#endif

	if (HaveID(Entry))
	{
		fRoot.fGDSStream = (*this)[Entry];
		fRoot.fGDSStream->AddRef();

	#ifdef COREARRAY_CODE_USING_LOG
		Log().Add(CdLogRecord::LOG_INFO,
			"Load the root folder from the entry (size: %g).",
			(double)fRoot.fGDSStream->Size());
	#endif

		CdReader Reader(fRoot.fGDSStream, &Log());
		Reader.BeginNameSpace();
		_INTERNAL::CdObject_LoadStruct(fRoot, Reader, fVersion);
		Reader.EndStruct();
	} else
		throw ErrGDSFile(ERR_GDS_ENTRY, Entry.Get());
}

void CdGDSFile::SaveStream(CdStream *Stream)
{
	#ifdef COREARRAY_CODE_DEBUG
	if (fStream != NULL)
		throw ErrGDSFile(ERR_GDS_STREAM);
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

void CdGDSFile::LoadFile(const UTF8String &fn, bool ReadOnly, bool AllowError)
{
	TdAutoRef<CdStream> F(new CdFileStream(RawText(fn).c_str(),
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly, AllowError);
	fFileName = fn;
}

void CdGDSFile::LoadFile(const char *fn, bool ReadOnly, bool AllowError)
{
	TdAutoRef<CdStream> F(new CdFileStream(fn,
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly, AllowError);
	fFileName = UTF8Text(fn);
}

void CdGDSFile::LoadFileFork(const char *fn, bool ReadOnly, bool AllowError)
{
	TdAutoRef<CdStream> F(new CdForkFileStream(fn,
		ReadOnly ? CdFileStream::fmOpenRead : CdFileStream::fmOpenReadWrite));
	LoadStream(F.get(), ReadOnly, AllowError);
	fFileName = UTF8Text(fn);
}

void CdGDSFile::SyncFile()
{
	if (fStream == NULL)
		throw ErrGDSFile(ERR_GDS_SAVE);
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
		file.Root().AssignFolder(Root());
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
				CdBlockStream::TBlockInfo::HEAD_SIZE + bSize) |
				GDS_STREAM_POS_MASK_HEAD_BIT;
			TdGDSPos sNext = 0;
			BYTE_LE<CdStream>(*F) <<
				sSize << sNext << fBlockList[i]->ID() << bSize;
			F->CopyFrom(*fBlockList[i], 0, -1);
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
