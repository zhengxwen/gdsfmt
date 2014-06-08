// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBase.cpp: Basic classes for CoreArray library
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

#include <dBase.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>


namespace CoreArray
{
	// CdTrimStream

	static const char *esTrimInvalidPos = "Invalid Stream Position.";

	class CdTrimStream: public CdStream
	{
	public:
		CdTrimStream(CdStream* vStream, const TdPtr64 TrimStart,
			const TdPtr64 TrimLen, bool vOffset)
		{
			(fStream = vStream)->AddRef();
			fStart = TrimStart;
			fLength = TrimLen;
			fStream->SetPosition(TrimStart);
			fOffset = vOffset;
			vPos = 0;
		}
		virtual ~CdTrimStream()
		{
			fStream->Release();
		}

		virtual ssize_t Read(void *Buffer, ssize_t Count)
		{
			if ((vPos+Count) > fLength)
				Count = fLength - vPos;
			Count = fStream->Read(Buffer, Count);
			vPos += Count;
			return Count;
		}

		virtual ssize_t Write(void *const Buffer, ssize_t Count)
		{
			TdPtr64 L;
			L = vPos + Count - fLength;
			if ((L > 0) && !OutOfLimit(L))
				Count -= L;
			Count = fStream->Write(Buffer, Count);
			vPos += Count;
			if (vPos > fLength) fLength = vPos;
			return Count;
		}

		virtual TdPtr64 Seek(const TdPtr64 Offset, TdSysSeekOrg Origin)
		{
			TdPtr64 rv;
			switch (Origin)
			{
				case soBeginning:
					if ((Offset<0) || (Offset>fLength))
						throw Err_dStream(esTrimInvalidPos);
					if (fOffset)
						fStream->SetPosition(fStart+Offset);
					else fStream->SetPosition(Offset);
					vPos = Offset;
					return vPos;

				case soCurrent:
					rv = vPos + Offset;
					if ((rv<0) || (rv>fLength))
						throw Err_dStream(esTrimInvalidPos);
					if (fOffset)
						fStream->SetPosition(rv + fStart);
					else fStream->SetPosition(rv);
					vPos = rv;
					return rv;

				case soEnd:
					if ((Offset>0) || (Offset<-fLength))
						throw Err_dStream(esTrimInvalidPos);
					if (fOffset) {
						rv = fLength + Offset;
						fStream->SetPosition(rv + fStart);
					} else {
						rv = fLength + fStart + Offset;
						fStream->SetPosition(rv);
					}
					vPos = rv;
					return rv;

				default:
					return -1;
			}
		}

		virtual TdPtr64 GetSize()
		{
			return fOffset? fLength : fLength+fStart;
		}

		virtual void SetSize(const TdPtr64 NewSize)
		{
			throw Err_dStream("Abstract function!");
		}

		virtual bool OutOfLimit(const TdPtr64 nOut)
		{
			return true;
		}

		COREARRAY_INLINE const CdStream* Stream() const { return fStream; }
		COREARRAY_INLINE bool Offset() const { return fOffset; }
		COREARRAY_INLINE TdPtr64 Start() const { return fStart; }
		COREARRAY_INLINE TdPtr64 Length() const { return fLength; }

	protected:
		TdPtr64 vPos, fStart, fLength;
		CdStream* fStream;
		bool fOffset;
	};

	namespace _Internal_
	{
		// for TdTypeID::osShortRec
		typedef struct { int Len; char Data[256]; } TShortRec;

		// null type
		typedef struct { } EmptyT;

		template<typename DestT, int trT>
			struct TValCvt<DestT, TShortRec, trT, COREARRAY_TR_UNKNOWN>
		{
			static DestT Cvt(const TShortRec &val)
				{ throw Err_dFilter("Error!"); }
		};

		template<typename DestT, int trT>
			struct TValCvt<DestT, EmptyT, trT, COREARRAY_TR_UNKNOWN>
		{
			static DestT Cvt(const EmptyT &val)
				{ throw Err_dFilter("Error!"); }
		};
	}
}

using namespace std;
using namespace CoreArray;


#ifdef COREARRAY_MSC
#define snprintf sprintf_s
#endif


static CdObjClassMgr dObjMgr;

CdObjClassMgr &CoreArray::dObjManager()
{
	return dObjMgr;
}


// resourcestring

static const char *SReadError = "Stream read error";
static const char *SWriteError = "Stream write error";

// CdObject
#ifdef COREARRAY_DEBUG_CODE
static const char *esInvalidOp = "Invalid Operation in function '%s'!";
#endif

// CdObjClassMgr, CdSerial, CdSerial, CdSerial
static const char *esDupClass = "Duplicate Class Stream Name '%s'!";
static const char *esStackEmpty = "Stream Stack is empty!";
static const char *esRelPosOutRange = "Relative position is out of the Currentrent struct!";
static const char *esInvalidStructLength = "Invalid Structure Length!";
static const char *esNoClassName = "Class '%s' does not exist!";

static const char *esDupName = "Duplicate Name '%s' in the Name Space!";
static const char *esInvalidType = "Invalid TYPE (id: %d) of the data!";
static const char *esInvalidPos = "Invalid position of stream.";
//static const char *esMissingName = "Missing property name '%s' in class '%s'.";
static const char *esNoNameSpace = "No name space in the current struct!";
static const char *esTooLongRecord = "The length of record (%d) should be [0..255] in Short Record Struct!";
static const char *esInvalidStreamName = "The class named of stream '%s' is invalid!";
static const char *esDuplicateName = "Duplicate name '%s'.";
//static const char *esInvalidInherited = "The class '%s' is not the same as, or inherited from '%s'.";
//static const char *esNoRTTIInfo = "The class '%s' has no RTTI information.";
static const char *esInvalidRecord = "No matching length of the record (%d) for Property Name %s.";
//static const char *esVersionHint = "The current data version (%x) is greater than the program version";
static const char *esBitOffset = "Invalid bit offset.";
static const char *esShortStrTooLong = "The length of string > 255.";
static const char *esPropName = "Invalid character in property name '%s'.";

static char PropNameMapStr[64] = {
	'.', '_', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
	'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
	's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

void CoreArray::_Internal_::CdObject_LoadStruct(CdObject &Obj, CdSerial &Reader,
	TdVersion Version)
{
	Obj.LoadStruct(Reader, Version);
}

void CoreArray::_Internal_::CdObject_SaveStruct(CdObject &Obj, CdSerial &Writer,
	bool IncludeName)
{
    Obj.SaveStruct(Writer, IncludeName);
}

// CdObject

char const* CdObject::dName() { return ""; }

char const* CdObject::dTraitName() { return ""; }

TdVersion CdObject::dVersion() { return COREARRAY_CLASS_VERSION; }

TdVersion CdObject::SaveVersion() { return dVersion(); }

void CdObject::LoadStruct(CdSerial &Reader, TdVersion Version)
{
	Reader.rInitNameSpace();
	LoadBefore(Reader, Version);
	try {
		Loading(Reader, Version);
	} catch (exception &E) {
		Reader.Log().Add(E.what());
	} catch (const char *E) {
        Reader.Log().Add(E);
    }
	try {
		LoadAfter(Reader, Version);
	} catch (exception &E) {
		Reader.Log().Add(E.what());
	} catch (const char *E) {
        Reader.Log().Add(E);
    }
}

void CdObject::SaveStruct(CdSerial &Writer, bool IncludeName)
{
	Writer.wBeginNameSpace();
	{
		if (IncludeName)
		{
			TdVersion Version = SaveVersion();
			Writer.wUInt8(Version & 0xFF);
			Writer.wUInt8(Version >> 8);
			Writer.wObjName(dName());
		}
		SaveBefore(Writer);
		Saving(Writer);
		SaveAfter(Writer);
	}
	Writer.wEndStruct();
}

void CdObject::LoadBefore(CdSerial &Reader, TdVersion Version) {}

void CdObject::Loading(CdSerial &Reader, TdVersion Version) {}

void CdObject::LoadAfter(CdSerial &Reader, TdVersion Version) {}

void CdObject::SaveBefore(CdSerial &Writer) {}

void CdObject::Saving(CdSerial &Writer) {}

void CdObject::SaveAfter(CdSerial &Writer) {}

CdObject& CdObject::operator= (const CdObject& m) { return *this; }

CdObject& CdObject::operator= (CdObject& m) { return *this; }


// CdRef

void CoreArray::_Internal_::DirectAddRef(CdRef &Obj)
{
	Obj.fReference++;
}

void CoreArray::_Internal_::DirectDecRef(CdRef &Obj)
{
	Obj.fReference--;
}

CdRef::CdRef() { fReference = 0; }

CdRef::~CdRef() {}

void CdRef::AddRef() { if (this) fReference++; }

ssize_t CdRef::Release()
{
	if (this)
	{
		fReference--;
		ssize_t rv = fReference;
		if (fReference <= 0) delete this;
		return rv;
	} else
		return 0;
}


// A broadcast object

void TdOnBroadcast::Notify(CdObjMsg *Sender, Int32 MsgCode, void *Param)
{
	if (Event)
		(Obj->*Event)(Sender, MsgCode, Param);
}


// CdObjMsg

CdObjMsg::CdObjMsg()
{
	vMsgRef = 0;
	vMsgFlag = false;
}

CdObjMsg::~CdObjMsg()
{
	fMsgList.clear();
}

void CdObjMsg::AddMsg(const TdOnBroadcast &MsgObj)
{
	vector<TdOnBroadcast>::iterator rv;
	rv = find(fMsgList.begin(), fMsgList.end(), MsgObj);
	if (MsgObj && (rv == fMsgList.end()))
		fMsgList.push_back(MsgObj);
}

void CdObjMsg::RemoveMsg(const TdOnBroadcast &MsgObj)
{
	vector<TdOnBroadcast>::iterator rv;
	rv = find(fMsgList.begin(), fMsgList.end(), MsgObj);
	if (rv != fMsgList.end())
    	fMsgList.erase(rv);
}

void CdObjMsg::Notify(Int32 MsgCode, void *Param)
{
	if (((MsgCode<0) || (vMsgRef<=0)) && MsgFilter(MsgCode, Param))
	{
		vector<TdOnBroadcast>::iterator p;
		for (p = fMsgList.begin(); p!=fMsgList.end(); p++)
			p->Notify(this, MsgCode, Param);
	} else
		vMsgFlag = true;
}

void CdObjMsg::BeginMsg()
{
	vMsgRef++;
}

void CdObjMsg::EndMsg()
{
	if (((--vMsgRef) <= 0) && vMsgFlag)
	{
		Notify(0, NULL);
		vMsgFlag = false;
	}
}

bool CdObjMsg::MsgFilter(Int32 MsgCode, void *Param)
{
	return true;
}

// CdLogRecord

void CdLogRecord::Add(const char *const str, int vType)
{
	TdItem I;
	I.Msg = str; I.Type = vType;
	fList.push_back(I);
}

void CdLogRecord::Add(std::string &str, int vType)
{
	TdItem I;
	I.Msg = str; I.Type = vType;
	fList.push_back(I);
}

void CdLogRecord::Add(Int32 vType, const char *fmt, ...)
{
	va_list args;
	char buf[4096];
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	Add(buf, vType);
}

void CdLogRecord::LoadBefore(CdSerial &Reader, TdVersion Version)
{
	fList.clear();
	UInt32 Cnt = 0;
	Reader["LOGSIZE"] >> Cnt;

	if (Reader["LOGDATA"].rBuffer())
	{
		for (UInt32 i=0; i < Cnt; i++)
		{
			string s = Reader.rStrUTF8();
			Int32 type = Reader.rUInt32();
			Add(s, type);
        }
		Reader.rEndStruct();
	} else if (Cnt > 0)
    	throw Err_dFilter("Invalid LOGSIZE and LOGDATA.");
}

void CdLogRecord::SaveBefore(CdSerial &Writer)
{
	vector<TdItem>::const_iterator it;
	UInt32 Cnt = fList.size();

	Writer["LOGSIZE"] << Cnt;
	if (Cnt > 0)
	{
		Writer["LOGDATA"].wBuffer();
		for (it=fList.begin(); it != fList.end(); it++)
		{
			Writer << it->Msg.c_str();
			Writer << Int32(it->Type);
		}
		Writer.wEndStruct();
	}
}


// CdStream

CdStream::CdStream() {}

CdStream::~CdStream() {}

TdPtr64 CdStream::GetSize()
{
	TdPtr64 pos, rv;
	pos = Seek(0, soCurrent);
	rv = Seek(0, soEnd);
	Seek(pos, soBeginning);
	return rv;
}

void CdStream::ReadBuffer(void *Buffer, ssize_t Count)
{
	if ((Count > 0) && (Read(Buffer, Count) != Count))
		throw Err_dStream(SReadError);
}

void CdStream::WriteBuffer(void *const Buffer, ssize_t Count)
{
	if ((Count > 0) && (Write(Buffer, Count) != Count))
		throw Err_dStream(SWriteError);
}

TdPtr64 CdStream::CopyFrom(CdStream &Source, TdPtr64 Count)
{
	char Buffer[0x10000];
	ssize_t N;
	TdPtr64 rv;

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	rv = Count;
	while (Count > 0)
	{
		N = (Count >= (TdPtr64)sizeof(Buffer))? (ssize_t)sizeof(Buffer) : Count;
		Source.ReadBuffer((void*)Buffer, N);
		WriteBuffer((void*)Buffer, N);
		Count -= N;
	}
	return rv;
}

UInt8 CdStream::rUInt8()
{
	UInt8 rv;
	ReadBuffer((void*)&rv, 1);
	return rv;
}

UInt16 CdStream::rUInt16()
{
	UInt16 rv;
	ReadBuffer((void*)&rv, 2);
	return COREARRAY_ENDIAN_CVT16(rv);
}

UInt32 CdStream::rUInt32()
{
	UInt32 rv;
	ReadBuffer((void*)&rv, 4);
	return COREARRAY_ENDIAN_CVT32(rv);
}

UInt64 CdStream::rUInt64()
{
	UInt64 rv;
	ReadBuffer((void*)&rv, 8);
	return COREARRAY_ENDIAN_CVT64(rv);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
UInt128 CdStream::rUInt128()
{
	UInt128 rv;
	ReadBuffer((void*)&rv, 128);
	return COREARRAY_ENDIAN_CVT128(rv);
}
#endif

Float32 CdStream::rFloat32()
{
	Float32 rv;
	ReadBuffer((void*)&rv, 4);
	return rv;
}

Float64 CdStream::rFloat64()
{
	Float64 rv;
	ReadBuffer((void*)&rv, 8);
	return rv;
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
Float128 CdStream::rFloat128()
{
	Float128 rv;
	ReadBuffer((void*)&rv, 16);
	return rv;
}
#endif

void CdStream::wUInt8(UInt8 val)
{
	WriteBuffer((void*)&val, 1);
}

void CdStream::wUInt16(UInt16 val)
{
	val = COREARRAY_ENDIAN_CVT16(val);
	WriteBuffer((void*)&val, 2);
}

void CdStream::wUInt32(UInt32 val)
{
	val = COREARRAY_ENDIAN_CVT32(val);
	WriteBuffer((void*)&val, 4);
}

void CdStream::wUInt64(UInt64 val)
{
	val = COREARRAY_ENDIAN_CVT64(val);
	WriteBuffer((void*)&val, 8);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
void CdStream::wUInt128(UInt128 val)
{
	val = COREARRAY_ENDIAN_CVT128(val);
	WriteBuffer((void*)&val, 16);
}
#endif

void CdStream::wFloat32(const Float32 &val)
{
	WriteBuffer((void*)&val, 4);
}

void CdStream::wFloat64(const Float64 &val)
{
	WriteBuffer((void*)&val, 8);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
void CdStream::wFloat128(const Float128 &val)
{
	WriteBuffer((void*)&val, 16);
}
#endif

CdStream& CdStream::operator= (const CdStream& m)
{
	return *this;
}

CdStream& CdStream::operator= (CdStream& m)
{
	return *this;
}


// CBufdStream

CBufdStream::CBufdStream(CdStream* vStream, ssize_t vBufSize)
{
	vBuffer = NULL;
	vActualPos = vBufStart = vBufEnd = 0;
	vBufWriteFlag = false;

	fBaseStream = vStream;
	(fStream = vStream)->AddRef();
	fBufSize = 0; SetBufSize(vBufSize);
	if (vStream)
		RefreshStream();
	fPosition = 0;
}

CBufdStream::~CBufdStream()
{
	ClearPipe();
	FlushWrite();
	fStream->Release();
	if (vBuffer) free((void*)vBuffer);
}

CBufdStream* CBufdStream::NewFilter()
{
	return new CBufdStream(fStream);
}

void CBufdStream::RefreshStream()
{
	vActualPos = fStream->Position();
	fPosition = vActualPos;
	vBufEnd = vBufStart = 0;
}

void CBufdStream::FlushBuffer()
{
	if (vBufWriteFlag)
	{
		vBufWriteFlag = false;
		if (vActualPos != vBufStart)
		{
			vActualPos = vBufStart;
			fStream->SetPosition(vBufStart);
		}
		WriteStream();
		vActualPos = vBufEnd;
		OnFlush.Notify(this);
	}
}

void CBufdStream::FlushWrite()
{
	if (vBufWriteFlag)
	{
		vBufWriteFlag = false;
		if (vActualPos != vBufStart)
		{
			vActualPos = vBufStart;
			fStream->SetPosition(vBufStart);
        }
	    WriteStream();
		vActualPos = vBufStart = vBufEnd;
		OnFlush.Notify(this);
	}
}

void CBufdStream::Read(void *Buf, ssize_t Count)
{
	ssize_t L;
    char *p;

	if (Count > 0)
	{
		// Check in Range
		if ((fPosition<vBufStart) || (fPosition>=vBufEnd))
		{
			// Save to Buffer
			FlushBuffer();
			// Make it in range
			vBufStart = (fPosition >> BufStreamAlign) << BufStreamAlign;
			if (vActualPos != vBufStart)
			{
				vActualPos = vBufStart;
				fStream->SetPosition(vBufStart);
			}
			vActualPos = vBufEnd = vBufStart + ReadStream();
		}
		// Loop Copy
		p = (char*)Buf;
		do {
			L = vBufEnd - fPosition;
			if (L <= 0) throw Err_dStream(SReadError);
			if (L > Count) L = Count;
			memcpy((void*)p, (void*)(vBuffer+(ssize_t)fPosition-(ssize_t)vBufStart), L);
			fPosition += L; p += L; Count -= L;
			if (Count > 0)
			{
				FlushBuffer();
				vBufStart = vBufEnd;
				vActualPos = vBufEnd = vBufStart + ReadStream();
			}
		} while (Count > 0);
	}
}

void CBufdStream::Write(void const* Buf, ssize_t Count)
{
	ssize_t L;
	char const* p;

	if (Count > 0)
	{
		// Check in Range
		if ((fPosition<vBufStart) || (fPosition>vBufEnd))
		{
			// Save to Buffer
			FlushBuffer();
			// Make it in range
			if (vActualPos != fPosition)
			{
				vActualPos = fPosition;
				fStream->SetPosition(fPosition);
            }
			vBufEnd = vBufStart = fPosition;
		}

		// Loop Copy
		p = (char const*)Buf;
		do {
			vBufWriteFlag = true;
			L = vBufStart + fBufSize - fPosition;
			if (L > Count) L = Count;
			memcpy((void*)(vBuffer+(ssize_t)fPosition-(ssize_t)vBufStart), (void*)p, L);
			fPosition += L;
			if (fPosition > vBufEnd)
				vBufEnd = fPosition;
			p += L; Count -= L;
			if (Count > 0)
			{
				FlushBuffer(); vBufStart = vBufEnd;
            }
		} while (Count > 0);
	}
}

void CBufdStream::CopyFrom(CBufdStream &Source, TdPtr64 Count)
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
		N = (Count >= (TdPtr64)sizeof(Buffer))? (ssize_t)sizeof(Buffer) : Count;
		Source.Read((void*)Buffer, N);
		Write((void*)Buffer, N);
		Count -= N;
	}
}

void CBufdStream::Truncate()
{
    FlushWrite();
	fStream->SetSize(fPosition);
}

int CBufdStream::rByteL()
{
	ssize_t L;

	// Check in Range
	if ((fPosition<vBufStart) || (fPosition>=vBufEnd))
	{
		// Save to Buffer
		FlushBuffer();
		// Make it in range
		vBufStart = (fPosition >> BufStreamAlign) << BufStreamAlign;
		if (vActualPos != vBufStart)
		{
			vActualPos = vBufStart;
			fStream->SetPosition(vBufStart);
		}
		vBufEnd = vBufStart + ReadStream();
		vActualPos = vBufEnd;
	}

	// Loop Copy
	L = vBufEnd - fPosition;
	if (L > 0)
	{
		UInt8 * p = (UInt8*)vBuffer;
		p += (ssize_t)fPosition - (ssize_t)vBufStart;
		fPosition++;
		return *p;
	} else
		return -1;
}

int CBufdStream::Peek()
{
	int rv = rByteL();
	if (rv >= 0) fPosition--;
	return rv;
}

UInt8 CBufdStream::rUInt8()
{
	UInt8 rv;
	Read((void*)&rv, 1);
	return rv;
}

UInt16 CBufdStream::rUInt16()
{
	UInt16 rv;
	Read((void*)&rv, 2);
	return COREARRAY_ENDIAN_CVT16(rv);
}

UInt32 CBufdStream::rUInt32()
{
	UInt32 rv;
	Read((void*)&rv, 4);
	return COREARRAY_ENDIAN_CVT32(rv);
}

UInt64 CBufdStream::rUInt64()
{
	UInt64 rv;
	Read((void*)&rv, 8);
	return COREARRAY_ENDIAN_CVT64(rv);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
UInt128 CBufdStream::rUInt128()
{
	UInt128 rv;
	Read((void*)&rv, 16);
	return COREARRAY_ENDIAN_CVT128(rv);
}
#endif

UInt16 CBufdStream::rPack16u()
{
	unsigned char B, i, offset = 0;
	UInt16 rv = 0;
	for (i = 1; i <= 3; i++)
	{
		Read((void*)&B, 1);
		rv |= unsigned(B & 0x7F) << offset;
		if ((B & 0x80) == 0) break;
		offset += 7;
	}
	return rv;
}

UInt32 CBufdStream::rPack32u()
{
	unsigned char B, i, offset = 0;
	UInt32 rv = 0;
	for (i = 1; i <= 5; i++)
	{
		Read((void*)&B, 1);
		rv |= unsigned(B & 0x7F) << offset;
		if ((B & 0x80) == 0) break;
		offset += 7;
	}
	return rv;
}

UInt64 CBufdStream::rPack64u()
{
	unsigned char B, i, offset = 0;
	UInt64 rv = 0;
	for (i = 1; i <= 8; i++)
	{
		Read((void*)&B, 1);
		rv |= (UInt64(B & 0x7F) << offset);
		if ((B & 0x80) == 0) return rv;
		offset += 7;
	}
	Read((void*)((char*)&rv + 15), 1);
	return rv;
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
UInt128 CBufdStream::rPack128u()
{
	/// \todo "rPack128u" is not supported currently
	throw Err_dFilter("Not support rPack128u.");
}
#endif

Float32 CBufdStream::rFloat32()
{
	Float32 rv;
	Read((void*)&rv, 4);
	return rv;
}

Float64 CBufdStream::rFloat64()
{
	Float64 rv;
	Read((void*)&rv, 8);
	return rv;
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
Float128 CBufdStream::rFloat128()
{
	Float128 rv;
	Read((void*)&rv, 16);
	return rv;
}
#endif

UTF8String CBufdStream::rStrUTF8()
{
	unsigned L = rPack32u();
	string rv;
	rv.resize(L);
	Read((void*)rv.c_str(), L);
	return rv;
}

UTF16String CBufdStream::rStrUTF16()
{
	TRWBit rec;
	unsigned w, i, L = rPack32u();
	UTF16String rv;

	rv.resize(L); InitRBit(rec);
	for (i = 0; i < L; i++)
	{
		w = rBits(8, rec);
		if (w > 0x7F)
			w = (w & 0x7F) | (rBits(9, rec) << 7);
		rv.at(i) = w;
	}
	return rv;
}

UTF32String CBufdStream::rStrUTF32()
{
	size_t L = rPack32u();
	UTF32String rv;
	rv.resize(L);
	for (UTF32String::iterator p = rv.begin(); p != rv.end(); p++)
		*p = rPack32u();
	return rv;
}

void CBufdStream::wUInt8(UInt8 val)
{
	Write((void*)&val, 1);
}

void CBufdStream::wUInt16(UInt16 val)
{
	val = COREARRAY_ENDIAN_CVT16(val);
	Write((void*)&val, 2);
}

void CBufdStream::wUInt32(UInt32 val)
{
	val = COREARRAY_ENDIAN_CVT32(val);
	Write((void*)&val, 4);
}

void CBufdStream::wUInt64(UInt64 val)
{
	val = COREARRAY_ENDIAN_CVT64(val);
	Write((void*)&val, 8);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
void CBufdStream::wUInt128(UInt128 val)
{
	val = COREARRAY_ENDIAN_CVT128(val);
	Write((void*)&val, 16);
}
#endif

void CBufdStream::wPack16u(UInt16 Value)
{
	unsigned char B, i;
	for (i = 1; i <= 3; i++)
	{
		B = Value & 0x7F; Value >>= 7;
		if (Value > 0) {
			B = B | 0x80; wUInt8(B);
    	} else {
			wUInt8(B); break;
		}
	}
}

void CBufdStream::wPack32u(UInt32 Value)
{
	unsigned char B, i;
	for (i = 1; i <= 5; i++)
	{
		B = Value & 0x7F; Value >>= 7;
		if (Value > 0) {
			B = B | 0x80; wUInt8(B);
    	} else {
			wUInt8(B); break;
		}
	}
}

void CBufdStream::wPack64u(UInt64 Value)
{
	unsigned char B, i;
	for (i = 1; i <= 8; i++)
	{
		B = Value & 0x7F; Value >>= 7;
		if (Value > 0) {
			B = B | 0x80; wUInt8(B);
		} else {
			wUInt8(B); return;
		}
	}
	if (Value > 0) wUInt8(Value);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
void CBufdStream::wPack128u(UInt128 Value)
{
	/// \todo "wPack128u" is not supported currently
	throw Err_dFilter("Not support wPack128u.");
}
#endif

void CBufdStream::wFloat32(const Float32 &val)
{
	Write((void*)&val, 4);
}

void CBufdStream::wFloat64(const Float64 &val)
{
	Write((void*)&val, 8);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
void CBufdStream::wFloat128(const Float128 &val)
{
	Write((void*)&val, 16);
}
#endif

void CBufdStream::wStrUTF8(const UTF8 *Value)
{
	size_t L = strlen(Value);
	wPack32u(L);
	Write((void*)Value, L);
}

void CBufdStream::wStrUTF16(const UTF16 *Value)
{
	TRWBit Rec;
	size_t L = char_traits<UTF16>::length(Value);

	wPack32u(L); InitWBit(Rec);
	for (; L > 0; L--)
	{
		UInt16 w = *Value++;
		if (w <= 0x7F)
			wBits(w, 8, Rec);
		else {
			wBits(w | 0x80, 8, Rec);
			w = w >> 7; wBits(w, 9, Rec);
		}
	}
	DoneWBit(Rec);
}

void CBufdStream::wStrUTF32(const UTF32 *Value)
{
	size_t L = char_traits<UTF32>::length(Value);
	wPack32u(L);
	for (; L > 0; L--) wPack32u(*Value++);
}

void CBufdStream::InitRBit(TRWBit &Rec)
{
  Rec.Offset = Rec.Value = 0;
}

unsigned CBufdStream::rBits(unsigned char Bits, TRWBit &Rec)
{
	unsigned char I, Mask, RI=0;
	unsigned rv = 0;

	#ifdef COREARRAY_DEBUG_CODE
	if (Rec.Offset > 7)
		throw Err_BufStream(esBitOffset);
	#endif

	while (Bits > 0)
	{
		if (Rec.Offset == 0) Rec.Value = rUInt8();
		I = 8 - Rec.Offset;
		if (I > Bits) I = Bits;
		Mask = ~((unsigned char)(-1) << I);
		rv |= (unsigned)((unsigned char)(Rec.Value >> Rec.Offset) & Mask) << RI;
		RI += I; Rec.Offset += I;
		if (Rec.Offset > 7) Rec.Offset = 0;
		Bits -= I;
	}
	return rv;
}

void CBufdStream::InitWBit(TRWBit &Rec)
{
   Rec.Value = Rec.Offset = 0;
}

void CBufdStream::DoneWBit(TRWBit &Rec)
{
	if (Rec.Offset > 0)
	{
		wUInt8(Rec.Value);
		Rec.Value = Rec.Offset = 0;
	}
}

void CBufdStream::wBits(unsigned Value, unsigned char Bits, TRWBit &Rec)
{
	unsigned char I;
    unsigned Mask;

	#ifdef COREARRAY_DEBUG_CODE
	if (Rec.Offset > 7)
		throw Err_BufStream(esBitOffset);
	#endif
	while (Bits > 0)
	{
		I = 8 - Rec.Offset;
		if (I > Bits) I = Bits;
	    Mask = ~((unsigned)(-1) << I);
		Rec.Value |= (Value & Mask) << Rec.Offset;
		Value = Value >> I;
		Rec.Offset += I;
		if (Rec.Offset > 7)
		{
			Rec.Offset = 0;
			wUInt8(Rec.Value);
			Rec.Value = 0;
		}
		Bits -= I;
	}
}

void CBufdStream::wCopy(CBufdStream &Source, TdPtr64 Size)
{
	char Buf[0x10000];
	ssize_t L;

	while (Size > 0)
	{
		L = (Size >= (TdPtr64)sizeof(Buf)) ? (ssize_t)sizeof(Buf) : Size;
		Source.Read((void*)Buf, L); Write((void*)Buf, L);
		Size -= L;
	}
}

void CBufdStream::SetStream(CdStream* Value)
{
	if (fStream != Value)
	{
		if (fStream) FlushWrite();
		fStream->Release();
		(fStream = Value)->AddRef();
		if (!fBaseStream)
			fBaseStream = Value;
		if (fStream)
			RefreshStream();
	};
}

void CBufdStream::SetBufSize(const ssize_t NewBufSize)
{
	if ((fBufSize!=NewBufSize) && (NewBufSize>=(1 << BufStreamAlign)))
	{
		FlushWrite();
		fBufSize = (NewBufSize >> BufStreamAlign) << BufStreamAlign;
		vBuffer = (char*)realloc((void*)vBuffer, fBufSize);
		COREARRAY_ALLOCCHECK(vBuffer);
    }
}

TdPtr64 CBufdStream::GetSize()
{
	FlushBuffer();
	return fStream->GetSize();
}

void CBufdStream::SetSize(const TdPtr64 Value)
{
	FlushWrite();
	fStream->SetSize(Value);
	RefreshStream();
}

void CBufdStream::ClearPipe()
{
	for (int i=vPipeItems.size(); i > 0; i--)
		PopPipe();
}

void CBufdStream::PushPipe(CdStreamPipe* APipe)
{
	vPipeItems.push_back(APipe);
	FlushWrite();
	fStream = APipe->InitPipe(this);
	fStream->AddRef();
	RefreshStream();
}

void CBufdStream::PopPipe()
{
	int L = vPipeItems.size();
	if (L > 0)
	{
		{
			auto_ptr<CdStreamPipe> FC(vPipeItems[L-1]);
			vPipeItems.pop_back();
			FlushBuffer();
			fStream = FC->FreePipe();
		}
		RefreshStream();
	}
}

ssize_t CBufdStream::ReadStream()
{
	return fStream->Read((void*)vBuffer, fBufSize);
}

void CBufdStream::WriteStream()
{
	if (vBufEnd > vBufStart)
	{
		fStream->WriteBuffer((void*)vBuffer, vBufEnd-vBufStart);
	}
}

string CBufdStream::rPropName()
{
	TRWBit Rec;
	string rv;

	InitRBit(Rec);
	rv.resize(rBits(6, Rec));
	for (size_t i = 0; i < rv.size(); i++)
		rv[i] = PropNameMapStr[rBits(6, Rec)];
	return rv;
}

void CBufdStream::wPropName(const char *Name)
{
	TRWBit Rec;
	char ch;
	char const* p = Name;
	int L = strlen(Name);

	#ifdef COREARRAY_DEBUG_CODE
	if (L > 63)
		throw Err_BufStream(esShortStrTooLong);
	#endif
	InitWBit(Rec);
	wBits(L, 6, Rec);

	while (*p)
	{
		ch = *p;
		if (ch == '.')
			ch = 0x00;
		else if (ch == '_')
			ch = 0x01;
		else if (('0'<=ch) && (ch<='9'))
			ch = 0x02 + ch - '0';
		else if (('A'<=ch) && (ch<='Z'))
			ch = 0x02 + 10 + ch - 'A';
		else if (('a'<=ch) && (ch<='z'))
			ch = 0x02 + 10 + 26 + ch - 'a';
		else
			throw Err_BufStream(esPropName, Name);
		wBits(ch, 6, Rec);
		p++;
	}
	DoneWBit(Rec);
}

string CBufdStream::rObjName()
{
	TRWBit Rec;
	string rv;

	rv.resize(rUInt8());
	InitRBit(Rec);
	for (size_t i = 0; i < rv.size(); i++)
		rv[i] = rBits(7, Rec);
	return rv;
}

void CBufdStream::wObjName(const char *Name)
{
	TRWBit Rec;
	int L=strlen(Name);
	char const* p = Name;

	#ifdef COREARRAY_DEBUG_CODE
	if (L > 255)
		throw Err_BufStream(esShortStrTooLong);
	#endif
	wUInt8(L); InitWBit(Rec);
	for (; *p; p++) wBits(*p & 0x7F, 7, Rec);
	DoneWBit(Rec);
}

// CdSerial

CdSerial::CdSerial(CdStream* vStream, ssize_t vBufSize, CdObjClassMgr* vClassMgr):
	CBufdStream(vStream, vBufSize)
{
	fClassMgr = (vClassMgr) ? vClassMgr : &dObjMgr;
	fLog = new CdLogRecord;
	fLog->AddRef();
	fVar.fFilter = this;
}

CdSerial::~CdSerial()
{
	fLog->Release();
}

TdPtr64 CdSerial::rBeginStruct()
{
	fFilterRec.push_front(CVarList());
	CVarList &p = fFilterRec.front();
	p.Start = fPosition;
	p.VarCount = -1; // Indicate no name space
	TdPosType tmp; (*this) >> tmp;
	p.Length = tmp.get();
	if (p.Length < TdPosType::size)
		throw Err_dFilter(esInvalidStructLength);
	return p.Length - TdPosType::size;
}

TdPtr64 CdSerial::rBeginNameSpace()
{
	fFilterRec.push_front(CVarList());
	CVarList &p = fFilterRec.front();
	p.Start = fPosition;
	TdPosType tmp; (*this) >> tmp;
	p.Length = tmp.get();
	if (p.Length < TdPosType::size)
		throw Err_dFilter(esInvalidStructLength);
	p.VarCount = rUInt16();
	return p.Length - TdPosType::size;
}

void CdSerial::rEndStruct()
{
	CVarList &p = Current();
	fPosition = p.Start + p.Length;
	fFilterRec.pop_front();
}

void CdSerial::wBeginStruct()
{
	fFilterRec.push_front(CVarList());
	CVarList &p = fFilterRec.front();
	p.Start = fPosition;
	p.VarCount = -1; // Indicate no name space
	*this << TdPosType(0);
}

void CdSerial::wBeginNameSpace()
{
	fFilterRec.push_front(CVarList());
	CVarList &p = fFilterRec.front();
	p.Start = fPosition;
	*this << TdPosType(0) << UInt16(0);
}

void CdSerial::wEndStruct()
{
	CVarList &p = Current();
	p.Length = fPosition - p.Start;
	fPosition = p.Start;
	*this << TdPosType(p.Length);
	if (p.VarCount >= 0) // Indicate name space
		wUInt16(p.VarCount);
	fPosition = p.Start + p.Length;
	fFilterRec.pop_front();
}

bool CdSerial::HasName(const char *Name)
{
	CVarList &p = Current();
	return p.Name2Iter(Name)!=p.VarList.end();
}

TdTypeID CdSerial::NameType(const char *Name)
{
	CVarList &p = Current();
	list<CBaseVar*>::iterator it = p.Name2Iter(Name);
	return (it==p.VarList.end()) ? osUnknown : (*it)->Kind;
}

TdPtr64 CdSerial::NamePosition(const char *Name)
{
	CVarList &p = Current();
	list<CBaseVar*>::iterator it = p.Name2Iter(Name);
	return (it==p.VarList.end()) ? -1 : (*it)->Start;
}

bool CdSerial::rValue(TdTypeID Kind, const char *Name, void *OutBuf)
{
	/// \todo "osFloat128" is not supported currently

	CVarList &cur = Current();
	list<CBaseVar*>::iterator it = cur.Name2Iter(Name);

	if (it != cur.VarList.end())
	{
		bool nFlag = true;
		TdTypeID CurKind = (*it)->Kind;

		switch (CurKind)
		{
			case osRecord:
				if (Kind == osRecord)
				{
					fPosition = (*it)->Start;
					if (ssize_t(OutBuf) != -1)
					{
						TdPtr64 L = rBeginStruct();
						if (ssize_t(OutBuf) != 0)
						{
							Read(OutBuf, L);
							if (L > 0) fPosition -= L;
						}
					}
					nFlag = false;
				} else
					return false;
				break;
			case osShortRec:
				if (Kind == osShortRec)
				{
					if (OutBuf)
					{
						CVar<_Internal_::TShortRec> *pi =
							(CVar<_Internal_::TShortRec>*)(*it);
						memcpy(OutBuf, (void*)&pi->X.Data, pi->X.Len);
					}
					fPosition = (*it)->Start + 1;
				} else
					return false;
				break;
			case osNameSpace:
				if (Kind == osNameSpace)
				{
					fPosition = (*it)->Start;
					if (ssize_t(OutBuf) != -1)
						rBeginNameSpace();
					nFlag = false;
				} else
					return false;
				break;

        	// integer
			case osInt8:     case osUInt8:
			case osInt16:    case osUInt16:
			case osInt32:    case osUInt32:
			case osInt64:    case osUInt64:
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case osInt128:   case osUInt128:
			#endif
			case os16Packed: case os32Packed:
			case os64Packed:
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case os128Packed:
			#endif
				if (CurKind == Kind)
				{
					memcpy(OutBuf, (*it)->PtrData(), (*it)->SizeOf());
				} else {
					switch (Kind)
					{
						case osInt8: case osUInt8:
							*static_cast<UInt8*>(OutBuf) = (*it)->Int(); break;
						case osInt16: case osUInt16: case os16Packed:
							*static_cast<UInt16*>(OutBuf) = (*it)->Int(); break;
						case osInt32: case osUInt32: case os32Packed:
							*static_cast<UInt32*>(OutBuf) = (*it)->Int(); break;
						case osInt64: case osUInt64: case os64Packed:
							*static_cast<UInt64*>(OutBuf) = (*it)->Int(); break;
						#ifndef COREARRAY_NO_EXTENDED_TYPES
						case osInt128:
							*static_cast<Int128*>(OutBuf) = (*it)->Int(); break;
						case osUInt128: case os128Packed:
							*static_cast<UInt128*>(OutBuf) = (*it)->Int(); break;
						#endif
					default:
						return false;
					}
				}
				break;

			// float number
			case osFloat32: case osFloat64:
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case osFloat128:
			#endif
				if (CurKind == Kind)
				{
					memcpy(OutBuf, (*it)->PtrData(), (*it)->SizeOf());
				} else {
					switch (Kind)
					{
						case osFloat32:
							*static_cast<Float32*>(OutBuf) = (*it)->Float(); break;
						case osFloat64:
							*static_cast<Float64*>(OutBuf) = (*it)->Float(); break;
						#ifndef COREARRAY_NO_EXTENDED_TYPES
						case osFloat128:
							*static_cast<Float128*>(OutBuf) = (*it)->Float(); break;
						#endif
					default:
						return false;
					}
				}
				break;

			// string
			case osStrUTF8:
				switch (Kind)
				{
					case osStrUTF8:
						*static_cast<UTF8String*>(OutBuf) =
							(*it)->get<UTF8String>();
						break;
					case osStrUTF16:
						*static_cast<UTF16String*>(OutBuf) =
							UTF8toUTF16((*it)->get<UTF8String>());
						break;
					case osStrUTF32:
						*static_cast<UTF32String*>(OutBuf) =
							UTF8toUTF32((*it)->get<UTF8String>());
						break;
					default:
						return false;
				}
				break;
			case osStrUTF16:
				switch (Kind)
				{
					case osStrUTF8:
						*static_cast<UTF8String*>(OutBuf) =
							UTF16toUTF8((*it)->get<UTF16String>());
						break;
					case osStrUTF16:
						*static_cast<UTF16String*>(OutBuf) =
							(*it)->get<UTF16String>();
						break;
					case osStrUTF32:
						*static_cast<UTF32String*>(OutBuf) =
							UTF16toUTF32((*it)->get<UTF16String>());
						break;
					default:
						return false;
				}
				break;
			case osStrUTF32:
				switch (Kind)
				{
					case osStrUTF8:
						*static_cast<UTF8String*>(OutBuf) =
							UTF32toUTF8((*it)->get<UTF32String>());
						break;
					case osStrUTF16:
						*static_cast<UTF16String*>(OutBuf) =
							UTF32toUTF16((*it)->get<UTF32String>());
						break;
					case osStrUTF32:
						*static_cast<UTF32String*>(OutBuf) =
							(*it)->get<UTF32String>();
						break;
					default:
						return false;
				}
				break;

			// others
			case osStreamPos:
				if (Kind == osStreamPos)
				{
					*static_cast<TdPosType*>(OutBuf) =
						(*it)->get<TdPosType>();
				} else
					return false;
				break;

			default:
				return false;
		}

		// Move the current property to the last position
		// Make the secord ordered property the first one
		CBaseVar *pit = (*it);
		cur.VarList.erase(it);
		cur.VarList.push_back(pit);
		if (nFlag) fPosition = pit->Start;
		return true;

	} else
		return false;
}

bool CdSerial::rValue(TdTypeID Kind, const char *const Name, void *OutBuffer,
	ssize_t BufLen)
{
	if (Kind==osRecord || Kind==osShortRec)
	{
		CVarList &p = Current();
		list<CBaseVar*>::iterator it = p.Name2Iter(Name);
		ssize_t L;
		TdPtr64 pt = 0;

		if (it != p.VarList.end())
		{
			switch ((*it)->Kind)
			{
				case osRecord:
					if (Kind == osRecord)
					{
						fPosition = (*it)->Start;
						L = rBeginStruct();
						pt = fPosition;
						try {
							if (L != BufLen)
							{
								#ifdef COREARRAY_WARN_CODE
								Log().Add(CdLogRecord::logWarn,
									esInvalidRecord, BufLen, Name);
								#endif
								fPosition = pt;
								return false;
							} else
								Read(OutBuffer, L);
						} catch (exception &E) {
							Log().Add(E.what());
						}
						rEndStruct();
						fPosition = pt;
					} else
						return false;
					break;

				case osShortRec:
					if (Kind == osShortRec)
					{
						CVar<_Internal_::TShortRec> *pi =
							(CVar<_Internal_::TShortRec>*)(*it);
						L = pi->X.Len;
						if (L != BufLen)
						{
							#ifdef COREARRAY_WARN_CODE
							Log().Add(CdLogRecord::logWarn,
								esInvalidRecord, BufLen, Name);
							#endif
							return false;
						} else
							memcpy(OutBuffer, (void*)pi->X.Data, L);
					} else
						return false;
					fPosition = (*it)->Start + 1;
					break;
				default:
					return false;
				}
		} else
        	return false;

		// Move the current property to the last position
		// Make the secord ordered property the first one
		CBaseVar* pit = (*it);
		p.VarList.erase(it);
		p.VarList.push_back(pit);
		return true;
	} else
		return rValue(Kind, Name, OutBuffer);
}

void CdSerial::wValue(TdTypeID Kind, const char * Name, const void * InBuf,
	ssize_t BufLen)
{
	/// \todo "osFloat128" is not supported currently

	if (COREARRAY_VALID_TYPEID(Kind))
	{
		CVarList &Cur = Current();

		#ifdef COREARRAY_DEBUG_CODE
		if (Cur.VarCount < 0) // if namespace
			throw Err_dFilter(esNoNameSpace);
		if (Cur.Name2Iter(Name) != Cur.VarList.end()) // if name exists
			throw Err_dFilter(esDuplicateName, Name);
		#endif

		Write((void*)&Kind, 1);	// Save Kind to stream
		wPropName(Name);		// Save property name to stream
		Cur.VarCount++;
		Cur.AddVar<_Internal_::EmptyT>(*this, Kind, fPosition, Name);

		switch (Kind)
		{
			case osRecord:
				if (BufLen >= 0)
				{
					wBeginStruct();
					try {
						Write(InBuf, BufLen);
					} catch (exception &E) {
						Log().Add(E.what());
					}
					wEndStruct();
				} else
					wBeginStruct();
				break;
			case osShortRec:
				if ((BufLen>=0) && (BufLen<256))
				{
					wUInt8(BufLen);
					Write(InBuf, BufLen);
				} else if (BufLen < 0) {
				}
				#ifdef COREARRAY_DEBUG_CODE
					else throw Err_dFilter(esTooLongRecord, BufLen);
				#endif
				break;
			case osNameSpace:
				wBeginNameSpace(); break;

            // integer
			case osInt8: case osUInt8:
				wUInt8(*static_cast<const UInt8*>(InBuf)); break;
			case osInt16: case osUInt16:
				wUInt16(*static_cast<const UInt16*>(InBuf)); break;
			case osInt32: case osUInt32:
				wUInt32(*static_cast<const UInt32*>(InBuf)); break;
			case osInt64: case osUInt64:
				wUInt64(*static_cast<const UInt64*>(InBuf)); break;
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case osInt128: case osUInt128:
				wUInt128(*static_cast<const UInt128*>(InBuf)); break;
			#endif
			case os16Packed:
            	wPack16u(*static_cast<const UInt16*>(InBuf)); break;
			case os32Packed:
            	wPack32u(*static_cast<const UInt32*>(InBuf)); break;
			case os64Packed:
            	wPack64u(*static_cast<const UInt64*>(InBuf)); break;
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case os128Packed:
            	wPack128u(*static_cast<const UInt128*>(InBuf)); break;
            #endif

			// float number
			case osFloat32:
				wFloat32(*static_cast<const Float32*>(InBuf)); break;
			case osFloat64:
				wFloat64(*static_cast<const Float64*>(InBuf)); break;
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case osFloat128:
				throw Err_dFilter("Not support Float128.");
			#endif

			// string
			case osStrUTF8:
				wStrUTF8(static_cast<const UTF8*>(InBuf)); break;
			case osStrUTF16:
				wStrUTF16(static_cast<const UTF16*>(InBuf)); break;
			case osStrUTF32:
				wStrUTF32(static_cast<const UTF32*>(InBuf)); break;

			// others
			case osStreamPos:
				*this << *static_cast<const TdPosType*>(InBuf); break;
			default:
				return;
		}
	} else
		throw Err_dFilter(esInvalidType, (int)Kind);
}

CdStream* CdSerial::BlockStream()
{
	if (!fFilterRec.empty())
	{
    	vActualPos = fStream->Position();
		CVarList &p = fFilterRec.front();
		return new CdTrimStream(fStream,
			p.Start + TdPosType::size, p.Length - TdPosType::size, true);
	} else
		return fStream;
}

void CdSerial::CheckInStruct()
{
	CVarList &p = Current();
	if ((fPosition<p.Start) || (fPosition>p.Start+p.Length))
		throw Err_dFilter(esInvalidPos);
}

bool CdSerial::EndOfStruct()
{
	CVarList &p = Current();
	if (fPosition == p.Start+p.Length)
		return true;
	else if ((p.Start<=fPosition) && (fPosition<p.Start+p.Length))
		return false;
	else
		throw Err_dFilter(esInvalidPos);
}

void CdSerial::StructInfo(TdPtr64 &Start, TdPtr64 &Length)
{
	CVarList &p = Current();
	Start = p.Start + TdPosType::size;
	Length = p.Length - TdPosType::size;
}

TdPtr64 CdSerial::GetRelPos()
{
	CVarList &p = Current();
	TdPtr64 r = fStream->Position() - p.Start;
	if ((r<0) || (r>p.Length))
		throw Err_dFilter(esRelPosOutRange);
	return r;
}

void CdSerial::SetRelPos(const TdPtr64 Value)
{
	CVarList &p = Current();
	if ((Value<0) || (Value>p.Length))
		throw Err_dFilter(esRelPosOutRange);
	fPosition = p.Start + Value;
}

void CdSerial::SetLog(CdLogRecord &vLog)
{
	if (fLog != &vLog)
	{
		fLog->Release();
		fLog = &vLog; vLog.AddRef();
	}
}

CdSerial::TdVar &CdSerial::operator[] (const char *vName)
{
	fVar.fName = vName;
	return fVar;
}

void CdSerial::rInitNameSpace()
{
	CVarList &Cur = Current();
	Cur.ClearVarList();
	string Name;

	for (int i=1; i <= Cur.VarCount; i++)
	{
		TdTypeID Kind = (TdTypeID)0;
		Read((void*)&Kind, 1); // Read Type ID
		Name = rPropName();    // Read property name

		#ifdef COREARRAY_WARN_CODE
		if (Cur.Name2Iter(Name.c_str()) != Cur.VarList.end()) // if name exists
		{
			Log().Add(CdLogRecord::logWarn,
				esDuplicateName, Name.c_str());
		}
		#endif

		switch (Kind)
		{
			case osRecord: case osNameSpace:
				{
					Cur.AddVar<_Internal_::EmptyT>(*this, Kind, fPosition, Name);
					TdPosType PI; *this >> PI;
					if (PI < TdPosType::size)
						throw Err_dFilter(esInvalidStructLength);
					fPosition += PI - TdPosType::size;
				}
    	        break;
			case osShortRec:
				{
					_Internal_::TShortRec &X =
						Cur.AddVar<_Internal_::TShortRec>(
						*this, Kind, fPosition, Name);
                	X.Len = rUInt8();
                	Read((void*)X.Data, X.Len);
				}
                break;

        	// integer
			case osInt8:
				Cur.AddVar<Int8>(*this, Kind, fPosition, Name) = rUInt8();
				break;
			case osUInt8:
				Cur.AddVar<UInt8>(*this, Kind, fPosition, Name) = rUInt8();
				break;
			case osInt16:
				Cur.AddVar<Int16>(*this, Kind, fPosition, Name) = rUInt16();
				break;
			case osUInt16:
				Cur.AddVar<UInt16>(*this, Kind, fPosition, Name) = rUInt16();
				break;
			case osInt32:
				Cur.AddVar<Int32>(*this, Kind, fPosition, Name) = rUInt32();
				break;
			case osUInt32:
				Cur.AddVar<UInt32>(*this, Kind, fPosition, Name) = rUInt32();
				break;
			case osInt64:
				Cur.AddVar<Int64>(*this, Kind, fPosition, Name) = rUInt64();
				break;
			case osUInt64:
				Cur.AddVar<UInt64>(*this, Kind, fPosition, Name) = rUInt64();
				break;
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case osInt128:
				Cur.AddVar<Int128>(*this, Kind, fPosition, Name) = rUInt128();
				break;
			case osUInt128:
				Cur.AddVar<UInt128>(*this, Kind, fPosition, Name) = rUInt128();
				break;
			#endif
			case os16Packed:
				Cur.AddVar<UInt16>(*this, Kind, fPosition, Name) = rPack16u();
            	break;
			case os32Packed:
				Cur.AddVar<UInt32>(*this, Kind, fPosition, Name) = rPack32u();
				break;
			case os64Packed:
				Cur.AddVar<UInt64>(*this, Kind, fPosition, Name) = rPack64u();
				break;
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			case os128Packed:
				Cur.AddVar<UInt128>(*this, Kind, fPosition, Name) = rPack128u();
				break;
			#endif

			// float
			case osFloat32:
				Cur.AddVar<Float32>(*this, Kind, fPosition, Name) = rFloat32();
				break;
			case osFloat64:
				Cur.AddVar<Float64>(*this, Kind, fPosition, Name) = rFloat64();
				break;
			#ifndef COREARRAY_NO_EXTENDED_TYPES
            case osFloat128:
            	throw Err_dFilter("Not support Float128.");
            #endif

			// string
			case osStrUTF8:
				Cur.AddVar<UTF8String>(*this, Kind, fPosition, Name) = rStrUTF8();
				break;
			case osStrUTF16:
				Cur.AddVar<UTF16String>(*this, Kind, fPosition, Name) = rStrUTF16();
				break;
			case osStrUTF32:
				Cur.AddVar<UTF32String>(*this, Kind, fPosition, Name) = rStrUTF32();
				break;

            // others
			case osStreamPos:
				*this >> Cur.AddVar<TdPosType>(*this, Kind, fPosition, Name);
				break;
			default:
				throw Err_dFilter(esInvalidType, (int)Kind);
		}
	}
}

CdSerial::CVarList & CdSerial::Current()
{
	if (fFilterRec.size() <= 0)
		throw Err_dFilter(esStackEmpty);
	return fFilterRec.front();
}

void CdSerial::CVarList::_AddVarItem(CdSerial &Filter, CBaseVar *rec)
{
	#ifdef COREARRAY_DEBUG_CODE
	if (VarCount < 0)
		throw Err_dObj(esInvalidOp, rec->Name.c_str());
	#endif
	#ifdef COREARRAY_WARN_CODE
	list<CBaseVar*>::iterator it;
	for (it = VarList.begin(); it != VarList.end(); it++)
	{
		if ((*it)->Name == rec->Name)
		{
			Filter.Log().Add(CdLogRecord::logWarn, esDupName,
				rec->Name.c_str());
			delete rec;
			break;
		}
	}
	#endif
	VarList.push_back(rec);
}

void CdSerial::CVarList::ClearVarList()
{
	list<CBaseVar*>::iterator it = VarList.begin();
	for (; it != VarList.end(); it++) delete *it;
	VarList.clear();
}

list<CdSerial::CBaseVar*>::iterator CdSerial::CVarList::Name2Iter(
	const char * Name)
{
	list<CBaseVar*>::iterator it;
	for (it = VarList.begin(); it != VarList.end(); it++)
	{
		if ((*it)->Name.compare(Name) == 0)
        	return it;
	}
	return VarList.end();
}

bool CdSerial::TdVar::rShortBuf(Int32 *pval, size_t ValCnt)
{
	bool rv = fFilter->rValue(osShortRec, fName,
		pval, ValCnt*sizeof(Int32));
	#ifndef COREARRAY_LITTLE_ENDIAN
	if (rv)
	{
		for (; ValCnt > 0; ValCnt--, pval++)
			*pval = COREARRAY_ENDIAN_CVT32(*pval);
	}
	#endif
	return rv;
}

bool CdSerial::TdVar::rShortBuf(Int64 *pval, size_t ValCnt)
{
	bool rv = fFilter->rValue(osShortRec, fName,
		pval, ValCnt*sizeof(Int64));
	#ifndef COREARRAY_LITTLE_ENDIAN
	if (rv)
	{
		for (; ValCnt > 0; ValCnt--, pval++)
			*pval = COREARRAY_ENDIAN_CVT64(*pval);
	}
	#endif
	return rv;
}

bool CdSerial::TdVar::rBuf(Int32 *pval, size_t ValCnt)
{
	bool rv = fFilter->rValue(osRecord, fName,
		pval, ValCnt*sizeof(Int32));
	#ifndef COREARRAY_LITTLE_ENDIAN
	if (rv)
	{
		for (; ValCnt > 0; ValCnt--, pval++)
			*pval = COREARRAY_ENDIAN_CVT32(*pval);
	}
	#endif
	return rv;
}

bool CdSerial::TdVar::rBuf(Int64 *pval, size_t ValCnt)
{
	bool rv = fFilter->rValue(osRecord, fName,
		pval, ValCnt*sizeof(Int64));
	#ifndef COREARRAY_LITTLE_ENDIAN
	if (rv)
	{
		for (; ValCnt > 0; ValCnt--, pval++)
			*pval = COREARRAY_ENDIAN_CVT64(*pval);
	}
	#endif
	return rv;
}

void CdSerial::TdVar::wShortBuf(const Int32 *pval, size_t ValCnt)
{
	#ifndef COREARRAY_LITTLE_ENDIAN
	auto_ptr<Int32> buf(new Int32[ValCnt]);
	Int32 *dest = buf.get();
	for (size_t L = ValCnt; L > 0; L--)
		*dest++ = COREARRAY_ENDIAN_CVT32(*pval++);
	pval = buf.get();
	#endif
	fFilter->wValue(osShortRec, fName, pval, ValCnt*sizeof(Int32));
}

void CdSerial::TdVar::wShortBuf(const Int64 *pval, size_t ValCnt)
{
	#ifndef COREARRAY_LITTLE_ENDIAN
	auto_ptr<Int64> buf(new Int64[ValCnt]);
	Int64 *dest = buf.get();
	for (size_t L = ValCnt; L > 0; L--)
		*dest++ = COREARRAY_ENDIAN_CVT64(*pval++);
	pval = buf.get();
	#endif
	fFilter->wValue(osShortRec, fName, pval, ValCnt*sizeof(Int64));
}

void CdSerial::TdVar::wBuf(const Int32 *pval, size_t ValCnt)
{
	#ifndef COREARRAY_LITTLE_ENDIAN
	auto_ptr<Int32> buf(new Int32[ValCnt]);
	Int32 *dest = buf.get();
	for (size_t L = ValCnt; L > 0; L--)
		*dest++ = COREARRAY_ENDIAN_CVT32(*pval++);
	pval = buf.get();
	#endif
	fFilter->wValue(osRecord, fName, pval, ValCnt*sizeof(Int32));
}

void CdSerial::TdVar::wBuf(const Int64 *pval, size_t ValCnt)
{
	#ifndef COREARRAY_LITTLE_ENDIAN
	auto_ptr<Int64> buf(new Int64[ValCnt]);
	Int64 *dest = buf.get();
	for (size_t L = ValCnt; L > 0; L--)
		*dest++ = COREARRAY_ENDIAN_CVT64(*pval++);
	pval = buf.get();
	#endif
	fFilter->wValue(osRecord, fName, pval, ValCnt*sizeof(Int64));
}


// CdObjClassMgr

CdObjClassMgr::CdObjClassMgr() {}

CdObjClassMgr::~CdObjClassMgr() {}


bool CdObjClassMgr::_strCmp::operator()(const char* s1, const char* s2) const
{
	if ((s1 == NULL) && (s2 != NULL))
		return true;
	else if ((s1 != NULL) && (s2 != NULL))
		return strcmp(s1, s2) < 0;
	else
		return false;
}

void CdObjClassMgr::AddClass(const char *ClassName,
	TdOnObjCreate OnCreate, ClassType vCType, const char *Desp)
{
	map<const char *, _ClassStruct, _strCmp>::const_iterator it;

	it = fClassMap.find(ClassName);
	if (it == fClassMap.end())
	{
		_ClassStruct p;
		p.OnCreate = OnCreate; p.Desp = Desp; p.CType = vCType;
		fClassMap.insert(pair<const char *, _ClassStruct>(ClassName, p));
	} else
		throw Err_dObj(esDupClass, ClassName);
}

void CdObjClassMgr::RemoveClass(const char * const ClassName)
{
	fClassMap.erase(ClassName);
}

void CdObjClassMgr::Clear()
{
	fClassMap.clear();
}

CdObjClassMgr::TdOnObjCreate CdObjClassMgr::NameToClass(
	const char * ClassName)
{
	map<const char *, _ClassStruct, _strCmp>::const_iterator it;
	it = fClassMap.find(ClassName);
	if (it != fClassMap.end())
		return it->second.OnCreate;
	else
		return NULL;
}

CdObjRef* CdObjClassMgr::toObj(CdSerial &Reader, TdInit OnInit, void *Data)
{
	TdOnObjCreate OnCreate;
	TdVersion Version;
	string Name;
	CdObjRef *Obj = NULL;

	Reader.rBeginNameSpace();
	try {
		Version = Reader.rUInt8();
		Version |= Reader.rUInt8() << 8;
		Name = Reader.rObjName();
		OnCreate = NameToClass(Name.c_str());

		if (OnCreate)
		{
			Obj = OnCreate();
			if (OnInit) OnInit(*this, Obj, Data);
			_Internal_::CdObject_LoadStruct(*Obj, Reader, Version);
		} else
			throw Err_dFilter(esInvalidStreamName, Name.c_str());
	} catch (exception &E) {
		Reader.Log().Add(E.what());
		delete Obj;
		Obj = NULL;
	}
	Reader.rEndStruct();
	return Obj;
}

const CdObjClassMgr::_ClassStruct &CdObjClassMgr::ClassStruct(
	const char *ClassName) const
{
	map<const char *, _ClassStruct, _strCmp>::const_iterator it;
	it = fClassMap.find(ClassName);
	if (it != fClassMap.end())
		return it->second;
	else
		throw Err_dFilter(esNoClassName, ClassName);
}


// Define variant type of data structure.

static const char * const erDSConvert = "Can't convert '%s' to '%s'!";

Err_dsAny::Err_dsAny(TdsAny::TdsType fromType, TdsAny::TdsType toType)
{
	const char *s1 = TdsAny::dvtNames(fromType);
	const char *s2 = TdsAny::dvtNames(toType);
	fMessage = Format(erDSConvert, s1, s2);
}

// TdsAny

TdsAny::TdsAny()
{
	memset((void*)this, 0, sizeof(TdsAny));
}

TdsAny::~TdsAny()
{
	_Done();
}

void TdsAny::_Done()
{
	switch (dsType)
	{
		case dvtStr8:
			if (aR.ptrStr8) delete aR.ptrStr8;
			aR.ptrStr8 = NULL;
			break;
		case dvtStr16:
			if (aR.ptrStr16) delete aR.ptrStr16;
			aR.ptrStr16 = NULL;
			break;
		case dvtStr32:
			if (aR.ptrStr32) delete aR.ptrStr32;
			aR.ptrStr32 = NULL;
			break;
		case dvtArray:
			if (aArray.ArrayPtr) delete[] aArray.ArrayPtr;
			aArray.ArrayLength = 0; aArray.ArrayPtr = NULL;
			break;
		case dvtObjRef:
			if (aR.obj) aR.obj->Release();
			aR.obj = NULL;
			break;
	}
	dsType = dvtNULL;
}

const char *TdsAny::dvtNames(int index)
{
	switch (index)
	{
		case dvtNULL:   return "Empty";
		// integer
		case dvtInt8:    return "Int8";
		case dvtUInt8:   return "UInt8";
		case dvtInt16:   return "Int16";
		case dvtUInt16:  return "UInt16";
		case dvtInt32:   return "Int32";
		case dvtUInt32:  return "UInt32";
		case dvtInt64:   return "Int64";
		case dvtUInt64:  return "UInt64";
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		case dvtInt128:  return "Int128";
		case dvtUInt128: return "UInt128";
		#endif

		// float number
		case dvtFloat32:  return "Float32";
		case dvtFloat64:  return "Float64";
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		case dvtFloat128: return "Float128";
		#endif

		// string
		case dvtSString8:  return "UTF-8 short string";
		case dvtSString16: return "UTF-16 short string";
		case dvtSString32: return "UTF-32 short string";
		case dvtStr8:      return "UTF-8 string";
		case dvtStr16:     return "UTF-16 string";
		case dvtStr32:     return "UTF-32 string";

		// others
		case dvtBoolean: return "boolean";
		case dvtObjRef:  return "CdObjRef";
		default:
			return "Unknown";
	}
}


#ifndef COREARRAY_NO_EXTENDED_TYPES

#define DSDATA_INT(TYPE) \
	case dvtInt8:    return ValCvt<TYPE, Int8>(VAL<Int8>()); \
	case dvtUInt8:   return ValCvt<TYPE, UInt8>(VAL<UInt8>()); \
	case dvtInt16:   return ValCvt<TYPE, Int16>(VAL<Int16>()); \
	case dvtUInt16:  return ValCvt<TYPE, UInt16>(VAL<UInt16>()); \
	case dvtInt32:   return ValCvt<TYPE, Int32>(VAL<Int32>()); \
	case dvtUInt32:  return ValCvt<TYPE, UInt32>(VAL<UInt32>()); \
	case dvtInt64:   return ValCvt<TYPE, Int64>(VAL<Int64>()); \
	case dvtUInt64:  return ValCvt<TYPE, UInt64>(VAL<UInt64>()); \
	case dvtInt128:  return ValCvt<TYPE, Int128>(VAL<Int128>()); \
	case dvtUInt128: return ValCvt<TYPE, UInt128>(VAL<UInt128>());

// TdsAny : float number
#define DSDATA_FLOAT(TYPE) \
	case dvtFloat32:  return ValCvt<TYPE, Float32>(VAL<Float32>()); \
	case dvtFloat64:  return ValCvt<TYPE, Float64>(VAL<Float64>()); \
	case dvtFloat128: return ValCvt<TYPE, Float128>(VAL<Float128>());

#else

#define DSDATA_INT(TYPE) \
	case dvtInt8:    return ValCvt<TYPE, Int8>(VAL<Int8>()); \
	case dvtUInt8:   return ValCvt<TYPE, UInt8>(VAL<UInt8>()); \
	case dvtInt16:   return ValCvt<TYPE, Int16>(VAL<Int16>()); \
	case dvtUInt16:  return ValCvt<TYPE, UInt16>(VAL<UInt16>()); \
	case dvtInt32:   return ValCvt<TYPE, Int32>(VAL<Int32>()); \
	case dvtUInt32:  return ValCvt<TYPE, UInt32>(VAL<UInt32>()); \
	case dvtInt64:   return ValCvt<TYPE, Int64>(VAL<Int64>()); \
	case dvtUInt64:  return ValCvt<TYPE, UInt64>(VAL<UInt64>()); \

// TdsAny : float number
#define DSDATA_FLOAT(TYPE) \
	case dvtFloat32:  return ValCvt<TYPE, Float32>(VAL<Float32>()); \
	case dvtFloat64:  return ValCvt<TYPE, Float64>(VAL<Float64>()); \

#endif


// TdsAny : string

#define DSDATA_STR(TYPE) \
	case dvtSString8: \
		return ValCvt<TYPE, UTF8String>(UTF8String(&aS8.SStr8[0], &aS8.SStr8[aS8.SStrLen8])); \
	case dvtSString16: \
		return ValCvt<TYPE, UTF16String>(UTF16String(&aS16.SStr16[0], &aS16.SStr16[aS16.SStrLen16])); \
	case dvtSString32: \
		return ValCvt<TYPE, UTF32String>(UTF32String(&aS32.SStr32[0], &aS32.SStr32[aS32.SStrLen32])); \
	case dvtStr8: \
		return ValCvt<TYPE, UTF8String>(*aR.ptrStr8); \
	case dvtStr16: \
		return ValCvt<TYPE, UTF16String>(*aR.ptrStr16); \
	case dvtStr32: \
		return ValCvt<TYPE, UTF32String>(*aR.ptrStr32);

// TdsAny : others

#define DSDATA_RETURN_OTHER(TYPE, dvt) \
	case dvtBoolean: \
		return ValCvt<TYPE, int>(VAL<int>() ? 1 : 0); \
	default: \
		throw Err_dsAny(dsType, dvt);
#define DSDATA_RETURN_STR_OTHER(TYPE, dvt) \
	case dvtBoolean: \
		return ValCvt<TYPE, UTF8String>(VAL<int>() ? "TRUE" : "FLASE"); \
	case dvtObjRef: \
		if (aR.obj != NULL) \
			return ValCvt<TYPE, UTF8String>(aR.obj->dTraitName()); \
		else \
			return ValCvt<TYPE, UTF8String>("[NULL]"); \
	default: \
		throw Err_dsAny(dsType, dvt);

Int8 TdsAny::GetInt8() const
{
	switch (dsType)
	{
		DSDATA_INT(Int8)
		DSDATA_FLOAT(Int8)
		DSDATA_STR(Int8)
		DSDATA_RETURN_OTHER(Int8, dvtInt8)
	}
}

UInt8 TdsAny::GetUInt8() const
{
	switch (dsType)
	{
		DSDATA_INT(UInt8)
		DSDATA_FLOAT(UInt8)
		DSDATA_STR(UInt8)
		DSDATA_RETURN_OTHER(UInt8, dvtUInt8)
	}
}

Int16 TdsAny::GetInt16() const
{
	switch (dsType)
	{
		DSDATA_INT(Int16)
		DSDATA_FLOAT(Int16)
		DSDATA_STR(Int16)
		DSDATA_RETURN_OTHER(Int16, dvtInt16)
	}
}

UInt16 TdsAny::GetUInt16() const
{
	switch (dsType)
	{
		DSDATA_INT(UInt16)
		DSDATA_FLOAT(UInt16)
		DSDATA_STR(UInt16)
		DSDATA_RETURN_OTHER(UInt16, dvtUInt16)
	}
}

Int32 TdsAny::GetInt32() const
{
	switch (dsType)
	{
		DSDATA_INT(Int32)
		DSDATA_FLOAT(Int32)
		DSDATA_STR(Int32)
		DSDATA_RETURN_OTHER(Int32, dvtInt32)
	}
}

UInt32 TdsAny::GetUInt32() const
{
	switch (dsType)
	{
		DSDATA_INT(UInt32)
		DSDATA_FLOAT(UInt32)
		DSDATA_STR(UInt32)
		DSDATA_RETURN_OTHER(UInt32, dvtUInt32)
	}
}

Int64 TdsAny::GetInt64() const
{
	switch (dsType)
	{
		DSDATA_INT(Int64)
		DSDATA_FLOAT(Int64)
		DSDATA_STR(Int64)
		DSDATA_RETURN_OTHER(Int64, dvtInt64)
	}
}

UInt64 TdsAny::GetUInt64() const
{
	switch (dsType)
	{
		DSDATA_INT(UInt64)
		DSDATA_FLOAT(UInt64)
		DSDATA_STR(UInt64)
		DSDATA_RETURN_OTHER(UInt64, dvtUInt64)
	}
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
Int128 TdsAny::GetInt128() const
{
	switch (dsType)
	{
		DSDATA_INT(Int128)
		DSDATA_FLOAT(Int128)
		DSDATA_STR(Int128)
		DSDATA_RETURN_OTHER(Int128, dvtInt128)
	}
}

UInt128 TdsAny::GetUInt128() const
{
	switch (dsType)
	{
		DSDATA_INT(UInt128)
		DSDATA_FLOAT(UInt128)
		DSDATA_STR(UInt128)
		DSDATA_RETURN_OTHER(UInt128, dvtUInt128)
	}
}
#endif

Float32 TdsAny::GetFloat32() const
{
	switch (dsType)
	{
		case dvtNULL: return NaN;
		DSDATA_INT(Float32)
		DSDATA_FLOAT(Float32)
		DSDATA_STR(Float32)
		DSDATA_RETURN_OTHER(Float32, dvtFloat32)
	}
}

Float64 TdsAny::GetFloat64() const
{
	switch (dsType)
	{
		case dvtNULL: return NaN;
		DSDATA_INT(Float64)
		DSDATA_FLOAT(Float64)
		DSDATA_STR(Float64)
		DSDATA_RETURN_OTHER(Float64, dvtFloat64)
	}
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
Float128 TdsAny::GetFloat128() const
{
	switch (dsType)
	{
		case dvtNULL: return Float128(NaN);
		DSDATA_INT(Float128)
		DSDATA_FLOAT(Float128)
		DSDATA_STR(Float128)
		DSDATA_RETURN_OTHER(Float128, dvtFloat128)
	}
}
#endif

UTF8String TdsAny::GetStr8() const
{
	switch (dsType)
	{
		case dvtNULL: return UTF8String();
		DSDATA_INT(UTF8String)
		DSDATA_FLOAT(UTF8String)
		DSDATA_STR(UTF8String)
		DSDATA_RETURN_STR_OTHER(UTF8String, dvtStr8)
	}
}

UTF16String TdsAny::GetStr16() const
{
	switch (dsType)
	{
		case dvtNULL: return UTF16String();
		DSDATA_INT(UTF16String)
		DSDATA_FLOAT(UTF16String)
		DSDATA_STR(UTF16String)
		DSDATA_RETURN_STR_OTHER(UTF16String, dvtStr16)
	}
}

UTF32String TdsAny::GetStr32() const
{
	switch (dsType)
	{
		case dvtNULL: return UTF32String();
		DSDATA_INT(UTF32String)
		DSDATA_FLOAT(UTF32String)
		DSDATA_STR(UTF32String)
		DSDATA_RETURN_STR_OTHER(UTF32String, dvtStr32)
	}
}

bool TdsAny::GetBool() const
{
	switch (dsType)
	{
		DSDATA_INT(int)
		DSDATA_FLOAT(int)
		DSDATA_STR(int)
		DSDATA_RETURN_OTHER(int, dvtStr32)
	}
}

const void *TdsAny::GetPtr() const
{
	if (dsType == dvtPointer)
	{
		return aR.const_ptr;
	} else
		throw Err_dsAny(dsType, dvtPointer);
}

const TdsAny *TdsAny::GetArray() const
{
	if (dsType == dvtArray)
	{
		return aArray.ArrayPtr;
	} else
		throw Err_dsAny(dsType, dvtArray);
}

UInt32 TdsAny::GetArrayLength() const
{
	if (dsType == dvtArray)
	{
		return aArray.ArrayLength;
	} else
		throw Err_dsAny(dsType, dvtArray);
}

CdObjRef *TdsAny::GetObj() const
{
	if (dsType == dvtObjRef)
		return aR.obj;
	else
		throw Err_dsAny(dsType, dvtObjRef);
}

void TdsAny::SetEmpty()
{
	_Done();
	dsType = dvtNULL;
}

void TdsAny::SetInt8(Int8 val)
{
	_Done();
	dsType = dvtInt8;
	VAL<Int8>() = val;
}

void TdsAny::SetUInt8(UInt8 val)
{
	_Done();
	dsType = dvtUInt8;
	VAL<UInt8>() = val;
}

void TdsAny::SetInt16(Int16 val)
{
	_Done();
	dsType = dvtInt16;
	VAL<Int16>() = val;
}

void TdsAny::SetUInt16(UInt16 val)
{
	_Done();
	dsType = dvtUInt16;
	VAL<UInt16>() = val;
}

void TdsAny::SetInt32(Int32 val)
{
	_Done();
	dsType = dvtInt32;
	VAL<Int32>() = val;
}

void TdsAny::SetUInt32(UInt32 val)
{
	_Done();
	dsType = dvtUInt32;
	VAL<UInt32>() = val;
}

void TdsAny::SetInt64(Int64 val)
{
	_Done();
	dsType = dvtInt64;
	VAL<Int64>() = val;
}

void TdsAny::SetUInt64(UInt64 val)
{
	_Done();
	dsType = dvtUInt64;
	VAL<UInt64>() = val;
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
void TdsAny::SetInt128(const Int128 &val)
{
	_Done();
	dsType = dvtInt128;
	VAL<Int128>() = val;
}

void TdsAny::SetUInt128(const UInt128 &val)
{
	_Done();
	dsType = dvtUInt128;
	VAL<UInt128>() = val;
}
#endif

void TdsAny::SetFloat32(Float32 val)
{
	_Done();
	dsType = dvtFloat32;
	VAL<Float32>() = val;
}

void TdsAny::SetFloat64(Float64 val)
{
	_Done();
	dsType = dvtFloat64;
	VAL<Float64>() = val;
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
void TdsAny::SetFloat128(const Float128 &val)
{
	_Done();
	dsType = dvtFloat128;
	VAL<Float128>() = val;
}
#endif

void TdsAny::SetStr8(const UTF8String &val)
{
	_Done();
	if (val.size() <= 22)
	{
		dsType = dvtSString8;
		aS8.SStrLen8 = val.size();
		memcpy(aS8.SStr8, val.c_str(), val.size());
	} else {
		dsType = dvtStr8;
        aR.ptrStr8 = new UTF8String(val);
    }
}

void TdsAny::SetStr16(const UTF16String &val)
{
	_Done();
	if (val.size() <= 11)
	{
		dsType = dvtSString16;
		aS16.SStrLen16 = val.size();
		memcpy(aS16.SStr16, val.c_str(), val.size()*2);
	} else {
		dsType = dvtStr16;
        aR.ptrStr16 = new UTF16String(val);
    }
}

void TdsAny::SetStr32(const UTF32String &val)
{
	_Done();
	if (val.size() <= 5)
	{
		dsType = dvtSString32;
		aS32.SStrLen32 = val.size();
		memcpy(aS32.SStr32, val.c_str(), val.size()*4);
	} else {
		dsType = dvtStr32;
        aR.ptrStr32 = new UTF32String(val);
    }
}

void TdsAny::SetBool(bool val)
{
	_Done();
	dsType = dvtBoolean;
	VAL<int>() = val;
}

void TdsAny::SetPtr(const void *ptr)
{
	_Done();
	dsType = dvtPointer;
	aR.const_ptr = ptr;
}

void TdsAny::SetArray(const Int32 ptr[], UInt32 size)
{
	_Done();
	dsType = dvtArray;
	aArray.ArrayLength = size;
	aArray.ArrayPtr = new TdsAny[size];
	for (UInt32 i=0; i < size; i++)
		aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const Int64 ptr[], UInt32 size)
{
	_Done();
	dsType = dvtArray;
	aArray.ArrayLength = size;
	aArray.ArrayPtr = new TdsAny[size];
	for (UInt32 i=0; i < size; i++)
		aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const Float64 ptr[], UInt32 size)
{
	_Done();
	dsType = dvtArray;
	aArray.ArrayLength = size;
	aArray.ArrayPtr = new TdsAny[size];
	for (UInt32 i=0; i < size; i++)
		aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const char* const ptr[], UInt32 size)
{
	_Done();
	dsType = dvtArray;
	aArray.ArrayLength = size;
	aArray.ArrayPtr = new TdsAny[size];
	for (UInt32 i=0; i < size; i++)
		aArray.ArrayPtr[i] = UTF8String(ptr[i]);
}

void TdsAny::SetArray(const std::string ptr[], UInt32 size)
{
	_Done();
	dsType = dvtArray;
	aArray.ArrayLength = size;
	aArray.ArrayPtr = new TdsAny[size];
	for (UInt32 i=0; i < size; i++)
		aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const bool ptr[], UInt32 size)
{
	_Done();
	dsType = dvtArray;
	aArray.ArrayLength = size;
	aArray.ArrayPtr = new TdsAny[size];
	for (UInt32 i=0; i < size; i++)
		aArray.ArrayPtr[i].SetBool(ptr[i]);
}

void TdsAny::SetObj(CdObjRef *obj)
{
	_Done();
	dsType = dvtObjRef;
	VAL<CdObjRef*>() = obj;
	obj->AddRef();
}

void TdsAny::Assign(const UTF8String &val)
{
	_Done();

	if (val.empty())
	{
		dsType = dvtNULL;
		return;
	}
	if (val == "TRUE")
	{
		SetBool(true);
		return;
	}
	if (val == "FALSE")
	{
		SetBool(false);
		return;
	}

	char * p;
	long rv = strtol(val.c_str(), &p, 10);
	if (!*p)
	{
		SetInt64(rv); Packed();
		return;
	}

	double rd = strtod(val.c_str(), &p);
	if (!*p)
	{
		SetFloat64(rd);
		return;
	}

	SetStr8(val);
}

bool TdsAny::IsNULL() const
{
	return (dsType == dvtNULL);
}

bool TdsAny::IsInt() const
{
	#ifndef COREARRAY_NO_EXTENDED_TYPES
		return (dvtInt8 <= dsType) && (dsType <= dvtUInt128);
	#else
		return (dvtInt8 <= dsType) && (dsType <= dvtUInt64);
	#endif
}

bool TdsAny::IsFloat() const
{
	#ifndef COREARRAY_NO_EXTENDED_TYPES
		return (dvtFloat32 <= dsType) && (dsType <= dvtFloat128);
	#else
		return (dvtFloat32 <= dsType) && (dsType <= dvtFloat64);
	#endif
}

bool TdsAny::IsNumeric() const
{
	return IsInt() || IsFloat();
}

bool TdsAny::IsBool() const
{
	return (dsType == dvtBoolean);
}

bool TdsAny::IsNaN() const
{
	switch (dsType)
	{
		case dvtNULL   : return true;
		case dvtInt8: case dvtUInt8:
		case dvtInt16: case dvtUInt16:
		case dvtInt32: case dvtUInt32:
		case dvtInt64: case dvtUInt64:
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		case dvtInt128: case dvtUInt128:
		#endif
			return false;
		case dvtFloat32:  return !IsFinite(VAL<Float32>());
		case dvtFloat64:  return !IsFinite(VAL<Float64>());
		default:          return true;
	}
}

bool TdsAny::IsNA() const
{
	switch (dsType)
	{
		case dvtNULL   : return true;
		case dvtFloat32:  return !IsFinite(VAL<Float32>());
		case dvtFloat64:  return !IsFinite(VAL<Float64>());
		default:          return false;
	}
}

bool TdsAny::IsString() const
{
	return (dvtSString8 <= dsType) && (dsType <= dvtStr32);
}

bool TdsAny::IsArray() const
{
	return (dsType == dvtArray);
}

bool TdsAny::Packed()
{
	#define xRANGE(L, I, H) ((L<=I) && (I<=H))
	#define yRANGE(L, H) (L<=H)
	Int64 I = 0;
	UInt64 U = 0;
	TdsType t = dsType;

	switch (dsType)
	{
		case dvtInt32:
			I = VAL<Int32>(); break;
		case dvtInt64:
        	I = VAL<Int64>(); break;
		case dvtUInt64:
			U = VAL<UInt64>(); break;
		default:
			return false;
	}

	if (dsType != dvtUInt64)
	{
		if (xRANGE(INT8_MIN, I, INT8_MAX)) {
    		dsType = dvtInt8; VAL<Int8>() = I;
		} else if (xRANGE(0, I, UINT8_MAX)) {
			dsType = dvtUInt8; VAL<UInt8>() = I;
		} else if (xRANGE(INT16_MIN, I, INT16_MAX)) {
			dsType = dvtInt16; VAL<Int16>() = I;
		} else if (xRANGE(0, I, UINT16_MAX)) {
			dsType = dvtUInt16; VAL<UInt16>() = I;
		} else if (xRANGE(INT32_MIN, I, INT32_MAX)) {
			dsType = dvtInt32; VAL<Int32>() = I;
		} else if (xRANGE(0, I, UINT32_MAX)) {
			dsType = dvtUInt32; VAL<UInt32>() = I;
		}
	} else {
		if (yRANGE(U, INT8_MAX)) {
    		dsType = dvtInt8; VAL<Int8>() = U;
		} else if (yRANGE(U, UINT8_MAX)) {
			dsType = dvtUInt8; VAL<UInt8>() = U;
		} else if (yRANGE(U, INT16_MAX)) {
			dsType = dvtInt16; VAL<Int16>() = U;
		} else if (yRANGE(U, UINT16_MAX)) {
			dsType = dvtUInt16; VAL<UInt16>() = U;
		} else if (yRANGE(U, INT32_MAX)) {
			dsType = dvtInt32; VAL<Int32>() = U;
		} else if (yRANGE(U, UINT32_MAX)) {
			dsType = dvtUInt32; VAL<UInt32>() = U;
		}
	}

	return t != dsType;
	#undef yRANGE
	#undef xRANGE
}

void TdsAny::Swap(TdsAny &D)
{
	char buf[sizeof(TdsAny)];

	memcpy((void*)buf, (void*)&D, sizeof(TdsAny));
	memcpy((void*)&D, (void*)this, sizeof(TdsAny));
	memcpy((void*)this, (void*)buf, sizeof(TdsAny));
}

int TdsAny::Compare(const TdsAny &D, bool NALast)
{
	if (IsNA())
	{
		if (D.IsNA())
			return 0;
		else if (NALast)
			return 1;
		else
			return -1;
	} else if (D.IsNA())
	{
		if (NALast)
			return -1;
		else
			return 1;
	} else if (IsNumeric() && D.IsNumeric())
	{
		if (IsInt() && D.IsInt())
		{
			Int64 I1 = GetInt64();
			Int64 I2 = D.GetInt64();
			if (I1 < I2)
				return -1;
			else if (I1 > I2)
				return 1;
			else
				return 0;
		} else {
			long double F1 = GetFloat64();
			long double F2 = D.GetFloat64();
			if (F1 < F2)
				return -1;
			else if (F1 > F2)
				return 1;
			else
				return 0;
		}
	} else
		return GetStr32().compare(D.GetStr32());
}

TdsAny & TdsAny::operator= (const TdsAny &_Right)
{
	if (this != &_Right)
	{
		_Done();
		switch (_Right.dsType)
		{
			case dvtObjRef:
				memcpy((void*)this, (void*)&_Right, sizeof(TdsAny));
				aR.obj->AddRef();
				break;
			case dvtStr8:
				dsType = dvtStr8;
				aR.ptrStr8 = new UTF8String(*_Right.aR.ptrStr8);
				break;
			case dvtStr16:
				dsType = dvtStr16;
				aR.ptrStr16 = new UTF16String(*_Right.aR.ptrStr16);
				break;
			case dvtStr32:
				dsType = dvtStr32;
				aR.ptrStr32 = new UTF32String(*_Right.aR.ptrStr32);
				break;
			default:
				memcpy(this, &_Right, sizeof(TdsAny));
		}
	}
	return *this;
}

CdSerial& CoreArray::operator>> (CdSerial &s, TdsAny& out)
{
	out._Done();
	out.dsType = s.rUInt8();

	switch (out.dsType)
	{
		case TdsAny::dvtNULL:
			break;
    	// integer
		case TdsAny::dvtInt8: case TdsAny::dvtUInt8:
			out.VAL<UInt8>() = s.rUInt8();
			break;
		case TdsAny::dvtInt16: case TdsAny::dvtUInt16:
			out.VAL<UInt16>() = s.rUInt16();
			break;
		case TdsAny::dvtInt32: case TdsAny::dvtUInt32:
			out.VAL<UInt32>() = s.rUInt32();
			break;
		case TdsAny::dvtInt64: case TdsAny::dvtUInt64:
			out.VAL<UInt64>() = s.rUInt64();
			break;
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		case TdsAny::dvtInt128: case TdsAny::dvtUInt128:
			out.VAL<UInt128>() = s.rUInt128();
			break;
		#endif

		// float number
		case TdsAny::dvtFloat32:
			out.VAL<Float32>() = s.rFloat32();
			break;
		case TdsAny::dvtFloat64:
			out.VAL<Float64>() = s.rFloat64();
			break;
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		case TdsAny::dvtFloat128:
			out.VAL<Float128>() = s.rFloat128();
			break;
		#endif

		// string
		case TdsAny::dvtSString8:
			out.aS8.SStrLen8 = s.rUInt8();
			if (out.aS8.SStrLen8 > 22)
            	throw Err_dsAny("Invalid length (%d) for dvtSString8", out.aS8.SStrLen8);
			s.Read((void*)out.aS8.SStr8, out.aS8.SStrLen8);
			break;
		case TdsAny::dvtSString16:
			out.aS16.SStrLen16 = s.rUInt8();
			if (out.aS16.SStrLen16 > 11)
            	throw Err_dsAny("Invalid length (%d) for dvtSString16", out.aS16.SStrLen16);
			s.Read((void*)out.aS16.SStr16, out.aS16.SStrLen16*2);
			break;
		case TdsAny::dvtSString32:
			out.aS32.SStrLen32 = s.rUInt8();
			if (out.aS32.SStrLen32 > 5)
            	throw Err_dsAny("Invalid length (%d) for dvtSString32", out.aS32.SStrLen32);
			s.Read((void*)out.aS32.SStr32, out.aS32.SStrLen32*4);
        	break;
		case TdsAny::dvtStr8:
            out.aR.ptrStr8 = new UTF8String(s.rStrUTF8());
			break;
		case TdsAny::dvtStr16:
			out.aR.ptrStr16 = new UTF16String(s.rStrUTF16());
			break;
		case TdsAny::dvtStr32:
			out.aR.ptrStr32 = new UTF32String(s.rStrUTF32());
			break;

		// other extended type
		case TdsAny::dvtBoolean:
			out.VAL<int>() = s.rUInt8();
			break;

		// pointer
		case TdsAny::dvtPointer:
			out.aR.const_ptr = NULL;
			break;
		// array
		case TdsAny::dvtArray:
			out.aArray.ArrayLength = s.rUInt32();
			out.aArray.ArrayPtr = new TdsAny[out.aArray.ArrayLength];
			for (UInt32 i=0; i < out.aArray.ArrayLength; i++)
				s >> out.aArray.ArrayPtr[i];
			break;

		// CdObjRef
		case TdsAny::dvtObjRef:
			if (s.rUInt8())
			{
				out.aR.obj = dObjMgr.toObj(s);
				out.aR.obj->AddRef();
			} else
				out.aR.obj = NULL;
			break;
	}
	return s;
}

CdSerial& CoreArray::operator<< (CdSerial &s, TdsAny &in)
{
	s.wUInt8(in.dsType);

	switch (in.dsType)
	{
		// integer
		case TdsAny::dvtInt8: case TdsAny::dvtUInt8:
			s.wUInt8(in.VAL<UInt8>());
			break;
		case TdsAny::dvtInt16: case TdsAny::dvtUInt16:
        	s.wUInt16(in.VAL<UInt16>());
			break;
		case TdsAny::dvtInt32: case TdsAny::dvtUInt32:
        	s.wUInt32(in.VAL<UInt32>());
			break;
		case TdsAny::dvtInt64: case TdsAny::dvtUInt64:
        	s.wUInt64(in.VAL<UInt64>());
			break;
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		case TdsAny::dvtInt128: case TdsAny::dvtUInt128:
			s.wUInt128(in.VAL<UInt128>());
			break;
		#endif

		// float number
		case TdsAny::dvtFloat32:
			s.wFloat32(in.VAL<Float32>());
			break;
		case TdsAny::dvtFloat64:
			s.wFloat64(in.VAL<Float64>());
			break;
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		case TdsAny::dvtFloat128:
			s.wFloat128(in.VAL<Float128>());
			break;
		#endif

		// string
		case TdsAny::dvtSString8:
			s.wUInt8(in.aS8.SStrLen8);
			s.Write((void*)in.aS8.SStr8, in.aS8.SStrLen8);
			break;
		case TdsAny::dvtSString16:
			s.wUInt8(in.aS16.SStrLen16);
			s.Write((void*)in.aS16.SStr16, in.aS16.SStrLen16*2);
			break;
		case TdsAny::dvtSString32:
			s.wUInt8(in.aS32.SStrLen32);
			s.Write((void*)in.aS32.SStr32, in.aS32.SStrLen32*4);
        	break;
		case TdsAny::dvtStr8:
			s.wStrUTF8(in.aR.ptrStr8->c_str());
			break;
		case TdsAny::dvtStr16:
			s.wStrUTF16(in.aR.ptrStr16->c_str());
			break;
		case TdsAny::dvtStr32:
        	s.wStrUTF32(in.aR.ptrStr32->c_str());
			break;

		// other extended type
		case TdsAny::dvtBoolean:
        	s.wUInt8(in.VAL<int>() ? 1 : 0);
			break;

		// pointer
		case TdsAny::dvtPointer:
			break;
		// array
		case TdsAny::dvtArray:
			s.wUInt32(in.aArray.ArrayLength);
			for (UInt32 i=0; i < in.aArray.ArrayLength; i++)
				s << in.aArray.ArrayPtr[i];
			break;

		// CdObjRef
		case TdsAny::dvtObjRef:
			if (in.aR.obj)
			{
				s.wUInt8(1);
				_Internal_::CdObject_SaveStruct(*in.aR.obj, s, true);
			} else
				s.wUInt8(0);
			break;
	}
	return s;
}
