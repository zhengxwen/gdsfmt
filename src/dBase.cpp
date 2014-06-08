// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBase.cpp: Fundamental classes for CoreArray library
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

#include <dBase.h>
#include <string.h>
#include <stdarg.h>
#include <algorithm>


namespace CoreArray
{
	// CdTrimStream

	static const char *esTrimInvalidPos = "Invalid Stream Position.";

	class COREARRAY_DLL_LOCAL CdTrimStream: public CdStream
	{
	public:
		CdTrimStream(CdStream* vStream, const SIZE64 TrimStart,
			const SIZE64 TrimLen, bool vOffset): CdStream()
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
			SIZE64 L;
			L = vPos + Count - fLength;
			if ((L > 0) && !OutOfLimit(L))
				Count -= L;
			Count = fStream->Write(Buffer, Count);
			vPos += Count;
			if (vPos > fLength) fLength = vPos;
			return Count;
		}

		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
		{
			SIZE64 rv;
			switch (Origin)
			{
				case soBeginning:
					if ((Offset<0) || (Offset>fLength))
						throw ErrStream(esTrimInvalidPos);
					if (fOffset)
						fStream->SetPosition(fStart+Offset);
					else fStream->SetPosition(Offset);
					vPos = Offset;
					return vPos;

				case soCurrent:
					rv = vPos + Offset;
					if ((rv<0) || (rv>fLength))
						throw ErrStream(esTrimInvalidPos);
					if (fOffset)
						fStream->SetPosition(rv + fStart);
					else fStream->SetPosition(rv);
					vPos = rv;
					return rv;

				case soEnd:
					if ((Offset>0) || (Offset<-fLength))
						throw ErrStream(esTrimInvalidPos);
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

		virtual SIZE64 GetSize()
		{
			return fOffset? fLength : fLength+fStart;
		}

		virtual void SetSize(const SIZE64 NewSize)
		{
			throw ErrStream("Abstract function!");
		}

		virtual bool OutOfLimit(const SIZE64 nOut)
		{
			return true;
		}

		COREARRAY_INLINE const CdStream* Stream() const { return fStream; }
		COREARRAY_INLINE bool Offset() const { return fOffset; }
		COREARRAY_INLINE SIZE64 Start() const { return fStart; }
		COREARRAY_INLINE SIZE64 Length() const { return fLength; }

	protected:
		SIZE64 vPos, fStart, fLength;
		CdStream* fStream;
		bool fOffset;
	};

	namespace _INTERNAL
	{
		// for TdSerialTypeID::osShortRec
		typedef struct { int Len; char Data[256]; } TShortRec;

		// null type
		typedef struct { } EmptyT;

		template<typename DestT, int trT>
			struct COREARRAY_DLL_LOCAL TValCvt
			<DestT, TShortRec, trT, COREARRAY_TR_UNKNOWN>
		{
			static DestT Cvt(const TShortRec &val)
				{ throw ErrSerial("Error!"); }
		};

		template<typename DestT, int trT>
			struct COREARRAY_DLL_LOCAL TValCvt
			<DestT, EmptyT, trT, COREARRAY_TR_UNKNOWN>
		{
			static DestT Cvt(const EmptyT &val)
				{ throw ErrSerial("Error!"); }
		};
	}
}

using namespace std;
using namespace CoreArray;


#ifdef COREARRAY_CC_MSC
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
#ifdef COREARRAY_CODE_DEBUG
static const char *esInvalidOp = "Invalid Operation in function '%s'!";
#endif

// CdObjClassMgr, CdSerial
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
static const char *esInvalidStreamName = "No class name '%s' in the GDS system according to the stream.";
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

void CoreArray::_INTERNAL::CdObject_LoadStruct(CdObject &Obj, CdSerial &Reader,
	TdVersion Version)
{
	Obj.LoadStruct(Reader, Version);
}

void CoreArray::_INTERNAL::CdObject_SaveStruct(CdObject &Obj, CdSerial &Writer,
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

void CoreArray::_INTERNAL::DirectAddRef(CdRef &Obj)
{
	Obj.fReference ++;
}

void CoreArray::_INTERNAL::DirectDecRef(CdRef &Obj)
{
	Obj.fReference --;
}

CdRef::CdRef()
{
	fReference = 0;
}

CdRef::~CdRef() {}

void CdRef::AddRef()
{
	fReference ++;
}

ssize_t CdRef::Release()
{
	fReference --;
	ssize_t rv = fReference;
	if (fReference <= 0) delete this;
	return rv;
}


// A broadcast object

void TdOnBroadcast::Notify(CdObjMsg *Sender, C_Int32 MsgCode, void *Param)
{
	if (Event)
		(Obj->*Event)(Sender, MsgCode, Param);
}


// CdObjMsg

CdObjMsg::CdObjMsg(): CdObjRef()
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

void CdObjMsg::Notify(C_Int32 MsgCode, void *Param)
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

bool CdObjMsg::MsgFilter(C_Int32 MsgCode, void *Param)
{
	return true;
}

// CdLogRecord

CdLogRecord::CdLogRecord(): CdObjRef() {}

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

void CdLogRecord::Add(C_Int32 vType, const char *fmt, ...)
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
	C_UInt32 Cnt = 0;
	Reader["LOGSIZE"] >> Cnt;

	if (Reader["LOGDATA"].rBuffer())
	{
		for (C_UInt32 i=0; i < Cnt; i++)
		{
			string s = Reader.rStrUTF8();
			C_Int32 type = Reader.rUInt32();
			Add(s, type);
        }
		Reader.rEndStruct();
	} else if (Cnt > 0)
    	throw ErrSerial("Invalid LOGSIZE and LOGDATA.");
}

void CdLogRecord::SaveBefore(CdSerial &Writer)
{
	vector<TdItem>::const_iterator it;
	C_UInt32 Cnt = fList.size();

	Writer["LOGSIZE"] << Cnt;
	if (Cnt > 0)
	{
		Writer["LOGDATA"].wBuffer();
		for (it=fList.begin(); it != fList.end(); it++)
		{
			Writer << it->Msg.c_str();
			Writer << C_Int32(it->Type);
		}
		Writer.wEndStruct();
	}
}


// CdStream

CdStream::CdStream(): CdRef() {}

CdStream::~CdStream() {}

SIZE64 CdStream::GetSize()
{
	SIZE64 pos, rv;
	pos = Seek(0, soCurrent);
	rv = Seek(0, soEnd);
	Seek(pos, soBeginning);
	return rv;
}

void CdStream::ReadBuffer(void *Buffer, ssize_t Count)
{
	if ((Count > 0) && (Read(Buffer, Count) != Count))
		throw ErrStream(SReadError);
}

void CdStream::WriteBuffer(void *const Buffer, ssize_t Count)
{
	if ((Count > 0) && (Write(Buffer, Count) != Count))
		throw ErrStream(SWriteError);
}

SIZE64 CdStream::CopyFrom(CdStream &Source, SIZE64 Count)
{
	char Buffer[0x10000];
	ssize_t N;
	SIZE64 rv;

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	rv = Count;
	while (Count > 0)
	{
		N = (Count >= (SIZE64)sizeof(Buffer))? (ssize_t)sizeof(Buffer) : Count;
		Source.ReadBuffer((void*)Buffer, N);
		WriteBuffer((void*)Buffer, N);
		Count -= N;
	}
	return rv;
}

C_UInt8 CdStream::rUInt8()
{
	C_UInt8 rv;
	ReadBuffer((void*)&rv, 1);
	return rv;
}

C_UInt16 CdStream::rUInt16()
{
	C_UInt16 rv;
	ReadBuffer((void*)&rv, 2);
	return COREARRAY_ENDIAN_CVT_I16(rv);
}

C_UInt32 CdStream::rUInt32()
{
	C_UInt32 rv;
	ReadBuffer((void*)&rv, 4);
	return COREARRAY_ENDIAN_CVT_I32(rv);
}

C_UInt64 CdStream::rUInt64()
{
	C_UInt64 rv;
	ReadBuffer((void*)&rv, 8);
	return COREARRAY_ENDIAN_CVT_I64(rv);
}

C_Float32 CdStream::rFloat32()
{
	C_Float32 rv;
	ReadBuffer((void*)&rv, 4);
	return rv;
}

C_Float64 CdStream::rFloat64()
{
	C_Float64 rv;
	ReadBuffer((void*)&rv, 8);
	return rv;
}

void CdStream::wUInt8(C_UInt8 val)
{
	WriteBuffer((void*)&val, 1);
}

void CdStream::wUInt16(C_UInt16 val)
{
	val = COREARRAY_ENDIAN_CVT_I16(val);
	WriteBuffer((void*)&val, 2);
}

void CdStream::wUInt32(C_UInt32 val)
{
	val = COREARRAY_ENDIAN_CVT_I32(val);
	WriteBuffer((void*)&val, 4);
}

void CdStream::wUInt64(C_UInt64 val)
{
	val = COREARRAY_ENDIAN_CVT_I64(val);
	WriteBuffer((void*)&val, 8);
}

void CdStream::wFloat32(const C_Float32 &val)
{
	WriteBuffer((void*)&val, 4);
}

void CdStream::wFloat64(const C_Float64 &val)
{
	WriteBuffer((void*)&val, 8);
}

CdStream& CdStream::operator= (const CdStream& m)
{
	return *this;
}

CdStream& CdStream::operator= (CdStream& m)
{
	return *this;
}

CdStream& CoreArray::operator>> (CdStream &s, TdPosType& out)
{
	C_UInt64 v = s.rUInt8();
	v |= (C_UInt64)s.rUInt8() << 8;
	v |= (C_UInt64)s.rUInt8() << 16;
	v |= (C_UInt64)s.rUInt8() << 24;
	v |= (C_UInt64)s.rUInt8() << 32;
	v |= (C_UInt64)s.rUInt8() << 40;
	out = (C_Int64)v;
	return s;
}

CdStream& CoreArray::operator<< (CdStream &s, const TdPosType &in)
{
	C_UInt64 v = (C_UInt64)in;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v);
	return s;
}


// CdBufStream

CdBufStream::CdBufStream(CdStream* vStream, ssize_t vBufSize): CdRef()
{
	vBuffer = NULL;
	vActualPos = vBufStart = vBufEnd = 0;
	vBufWriteFlag = false;

	fStream = fBaseStream = vStream;
	if (vStream)
		vStream->AddRef();
	fBufSize = 0; SetBufSize(vBufSize);
	if (vStream)
		RefreshStream();
	fPosition = 0;
}

CdBufStream::~CdBufStream()
{
	ClearPipe();
	FlushWrite();
	if (fStream)
		fStream->Release();
	if (vBuffer) free((void*)vBuffer);
}

CdBufStream* CdBufStream::NewFilter()
{
	return new CdBufStream(fStream);
}

void CdBufStream::RefreshStream()
{
	vActualPos = fStream->Position();
	fPosition = vActualPos;
	vBufEnd = vBufStart = 0;
}

void CdBufStream::FlushBuffer()
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

void CdBufStream::FlushWrite()
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

void CdBufStream::Read(void *Buf, ssize_t Count)
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
			if (L <= 0) throw ErrStream(SReadError);
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

void CdBufStream::Write(void const* Buf, ssize_t Count)
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

void CdBufStream::CopyFrom(CdBufStream &Source, SIZE64 Count)
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
		N = (Count >= (SIZE64)sizeof(Buffer))? (ssize_t)sizeof(Buffer) : Count;
		Source.Read((void*)Buffer, N);
		Write((void*)Buffer, N);
		Count -= N;
	}
}

void CdBufStream::Truncate()
{
    FlushWrite();
	fStream->SetSize(fPosition);
}

int CdBufStream::rByteL()
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
		C_UInt8 * p = (C_UInt8*)vBuffer;
		p += (ssize_t)fPosition - (ssize_t)vBufStart;
		fPosition++;
		return *p;
	} else
		return -1;
}

int CdBufStream::Peek()
{
	int rv = rByteL();
	if (rv >= 0) fPosition--;
	return rv;
}

C_UInt8 CdBufStream::rUInt8()
{
	C_UInt8 rv;
	Read((void*)&rv, 1);
	return rv;
}

C_UInt16 CdBufStream::rUInt16()
{
	C_UInt16 rv;
	Read((void*)&rv, 2);
	return COREARRAY_ENDIAN_CVT_I16(rv);
}

C_UInt32 CdBufStream::rUInt32()
{
	C_UInt32 rv;
	Read((void*)&rv, 4);
	return COREARRAY_ENDIAN_CVT_I32(rv);
}

C_UInt64 CdBufStream::rUInt64()
{
	C_UInt64 rv;
	Read((void*)&rv, 8);
	return COREARRAY_ENDIAN_CVT_I64(rv);
}

C_UInt16 CdBufStream::rPack16u()
{
	unsigned char B, i, offset = 0;
	C_UInt16 rv = 0;
	for (i = 1; i <= 3; i++)
	{
		Read((void*)&B, 1);
		rv |= unsigned(B & 0x7F) << offset;
		if ((B & 0x80) == 0) break;
		offset += 7;
	}
	return rv;
}

C_UInt32 CdBufStream::rPack32u()
{
	unsigned char B, i, offset = 0;
	C_UInt32 rv = 0;
	for (i = 1; i <= 5; i++)
	{
		Read((void*)&B, 1);
		rv |= unsigned(B & 0x7F) << offset;
		if ((B & 0x80) == 0) break;
		offset += 7;
	}
	return rv;
}

C_UInt64 CdBufStream::rPack64u()
{
	unsigned char B, i, offset = 0;
	C_UInt64 rv = 0;
	for (i = 1; i <= 8; i++)
	{
		Read((void*)&B, 1);
		rv |= (C_UInt64(B & 0x7F) << offset);
		if ((B & 0x80) == 0) return rv;
		offset += 7;
	}
	Read((void*)((char*)&rv + 15), 1);
	return rv;
}

C_Float32 CdBufStream::rFloat32()
{
	C_Float32 rv;
	Read((void*)&rv, 4);
	return rv;
}

C_Float64 CdBufStream::rFloat64()
{
	C_Float64 rv;
	Read((void*)&rv, 8);
	return rv;
}

UTF8String CdBufStream::rStrUTF8()
{
	unsigned L = rPack32u();
	string rv;
	rv.resize(L);
	Read((void*)rv.c_str(), L);
	return rv;
}

UTF16String CdBufStream::rStrUTF16()
{
	TRWBit rec;
	unsigned w, i, L = rPack32u();
	UTF16String rv(L, '\x0');

	InitRBit(rec);
	for (i = 0; i < L; i++)
	{
		w = rBits(8, rec);
		if (w > 0x7F)
			w = (w & 0x7F) | (rBits(9, rec) << 7);
		rv[i] = w;
	}
	return rv;
}

UTF32String CdBufStream::rStrUTF32()
{
	size_t L = rPack32u();
	UTF32String rv(L, '\x0');
	for (size_t i=0; i < L; i++)
		rv[i] = rPack32u();
	return rv;
}

void CdBufStream::wUInt8(C_UInt8 val)
{
	Write((void*)&val, 1);
}

void CdBufStream::wUInt16(C_UInt16 val)
{
	val = COREARRAY_ENDIAN_CVT_I16(val);
	Write((void*)&val, 2);
}

void CdBufStream::wUInt32(C_UInt32 val)
{
	val = COREARRAY_ENDIAN_CVT_I32(val);
	Write((void*)&val, 4);
}

void CdBufStream::wUInt64(C_UInt64 val)
{
	val = COREARRAY_ENDIAN_CVT_I64(val);
	Write((void*)&val, 8);
}

void CdBufStream::wPack16u(C_UInt16 Value)
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

void CdBufStream::wPack32u(C_UInt32 Value)
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

void CdBufStream::wPack64u(C_UInt64 Value)
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

void CdBufStream::wFloat32(const C_Float32 &val)
{
	Write((void*)&val, 4);
}

void CdBufStream::wFloat64(const C_Float64 &val)
{
	Write((void*)&val, 8);
}

void CdBufStream::wStrUTF8(const UTF8 *Value)
{
	size_t L = strlen(Value);
	wPack32u(L);
	Write((void*)Value, L);
}

void CdBufStream::wStrUTF16(const UTF16 *Value)
{
	TRWBit Rec;
	size_t L = char_traits<UTF16>::length(Value);

	wPack32u(L); InitWBit(Rec);
	for (; L > 0; L--)
	{
		C_UInt16 w = *Value++;
		if (w <= 0x7F)
			wBits(w, 8, Rec);
		else {
			wBits(w | 0x80, 8, Rec);
			w = w >> 7; wBits(w, 9, Rec);
		}
	}
	DoneWBit(Rec);
}

void CdBufStream::wStrUTF32(const UTF32 *Value)
{
	size_t L = char_traits<UTF32>::length(Value);
	wPack32u(L);
	for (; L > 0; L--) wPack32u(*Value++);
}

void CdBufStream::InitRBit(TRWBit &Rec)
{
  Rec.Offset = Rec.Value = 0;
}

unsigned CdBufStream::rBits(unsigned char Bits, TRWBit &Rec)
{
	unsigned char I, Mask, RI=0;
	unsigned rv = 0;

	#ifdef COREARRAY_CODE_DEBUG
	if (Rec.Offset > 7)
		throw ErrBufStream(esBitOffset);
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

void CdBufStream::InitWBit(TRWBit &Rec)
{
   Rec.Value = Rec.Offset = 0;
}

void CdBufStream::DoneWBit(TRWBit &Rec)
{
	if (Rec.Offset > 0)
	{
		wUInt8(Rec.Value);
		Rec.Value = Rec.Offset = 0;
	}
}

void CdBufStream::wBits(unsigned Value, unsigned char Bits, TRWBit &Rec)
{
	unsigned char I;
    unsigned Mask;

	#ifdef COREARRAY_CODE_DEBUG
	if (Rec.Offset > 7)
		throw ErrBufStream(esBitOffset);
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

void CdBufStream::wCopy(CdBufStream &Source, SIZE64 Size)
{
	char Buf[0x10000];
	ssize_t L;

	while (Size > 0)
	{
		L = (Size >= (SIZE64)sizeof(Buf)) ? (ssize_t)sizeof(Buf) : Size;
		Source.Read((void*)Buf, L); Write((void*)Buf, L);
		Size -= L;
	}
}

void CdBufStream::SetStream(CdStream* Value)
{
	if (fStream != Value)
	{
		if (fStream)
		{
			FlushWrite();
			fStream->Release();
		}
		fStream = Value;
		if (!fBaseStream)
			fBaseStream = Value;
		if (fStream)
		{
			fStream->AddRef();
			RefreshStream();
		}
	}
}

void CdBufStream::SetBufSize(const ssize_t NewBufSize)
{
	if ((fBufSize!=NewBufSize) && (NewBufSize>=(1 << BufStreamAlign)))
	{
		FlushWrite();
		fBufSize = (NewBufSize >> BufStreamAlign) << BufStreamAlign;
		vBuffer = (char*)realloc((void*)vBuffer, fBufSize);
		COREARRAY_ALLOCCHECK(vBuffer);
    }
}

SIZE64 CdBufStream::GetSize()
{
	FlushBuffer();
	return fStream->GetSize();
}

void CdBufStream::SetSize(const SIZE64 Value)
{
	FlushWrite();
	fStream->SetSize(Value);
	RefreshStream();
}

void CdBufStream::ClearPipe()
{
	for (int i=vPipeItems.size(); i > 0; i--)
		PopPipe();
}

void CdBufStream::PushPipe(CdStreamPipe* APipe)
{
	vPipeItems.push_back(APipe);
	FlushWrite();
	fStream = APipe->InitPipe(this);
	fStream->AddRef();
	RefreshStream();
}

void CdBufStream::PopPipe()
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

ssize_t CdBufStream::ReadStream()
{
	return fStream->Read((void*)vBuffer, fBufSize);
}

void CdBufStream::WriteStream()
{
	if (vBufEnd > vBufStart)
	{
		fStream->WriteBuffer((void*)vBuffer, vBufEnd-vBufStart);
	}
}

string CdBufStream::rPropName()
{
	TRWBit Rec;
	string rv;

	InitRBit(Rec);
	rv.resize(rBits(6, Rec));
	for (size_t i = 0; i < rv.size(); i++)
		rv[i] = PropNameMapStr[rBits(6, Rec)];
	return rv;
}

void CdBufStream::wPropName(const char *Name)
{
	TRWBit Rec;
	char ch;
	char const* p = Name;
	int L = strlen(Name);

	#ifdef COREARRAY_CODE_DEBUG
	if (L > 63)
		throw ErrBufStream(esShortStrTooLong);
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
			throw ErrBufStream(esPropName, Name);
		wBits(ch, 6, Rec);
		p++;
	}
	DoneWBit(Rec);
}

string CdBufStream::rObjName()
{
	TRWBit Rec;
	string rv;

	rv.resize(rUInt8());
	InitRBit(Rec);
	for (size_t i = 0; i < rv.size(); i++)
		rv[i] = rBits(7, Rec);
	return rv;
}

void CdBufStream::wObjName(const char *Name)
{
	TRWBit Rec;
	int L=strlen(Name);
	char const* p = Name;

	#ifdef COREARRAY_CODE_DEBUG
	if (L > 255)
		throw ErrBufStream(esShortStrTooLong);
	#endif
	wUInt8(L); InitWBit(Rec);
	for (; *p; p++) wBits(*p & 0x7F, 7, Rec);
	DoneWBit(Rec);
}

CdBufStream& CoreArray::operator>> (CdBufStream &s, TdPosType& out)
{
	C_UInt64 v = s.rUInt8();
	v |= (C_UInt64)s.rUInt8() << 8;
	v |= (C_UInt64)s.rUInt8() << 16;
	v |= (C_UInt64)s.rUInt8() << 24;
	v |= (C_UInt64)s.rUInt8() << 32;
	v |= (C_UInt64)s.rUInt8() << 40;
	out = (C_Int64)v;
	return s;
}

CdBufStream& CoreArray::operator<< (CdBufStream &s, const TdPosType &in)
{
	C_UInt64 v = (C_UInt64)in;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v); v >>= 8;
	s.wUInt8(v);
	return s;
}


// CdSerial

CdSerial::CdSerial(CdStream* vStream, ssize_t vBufSize,
	CdObjClassMgr* vClassMgr): CdBufStream(vStream, vBufSize)
{
	fClassMgr = (vClassMgr) ? vClassMgr : &dObjMgr;
	fLog = new CdLogRecord;
	fLog->AddRef();
	fVar.fFilter = this;
}

CdSerial::~CdSerial()
{
	if (fLog)
		fLog->Release();
}

SIZE64 CdSerial::rBeginStruct()
{
	fFilterRec.push_front(CVarList());
	CVarList &p = fFilterRec.front();
	p.Start = fPosition;
	p.VarCount = -1; // Indicate no name space
	TdPosType tmp;
	(*this) >> tmp;
	p.Length = tmp.get();
	if (p.Length < TdPosType::Size)
		throw ErrSerial(esInvalidStructLength);
	return p.Length - TdPosType::Size;
}

SIZE64 CdSerial::rBeginNameSpace()
{
	fFilterRec.push_front(CVarList());
	CVarList &p = fFilterRec.front();
	p.Start = fPosition;
	TdPosType tmp;
	(*this) >> tmp;
	p.Length = tmp.get();
	if (p.Length < TdPosType::Size)
		throw ErrSerial(esInvalidStructLength);
	p.VarCount = rUInt16();
	return p.Length - TdPosType::Size;
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
	(*this) << TdPosType(0);
}

void CdSerial::wBeginNameSpace()
{
	fFilterRec.push_front(CVarList());
	CVarList &p = fFilterRec.front();
	p.Start = fPosition;
	(*this) << TdPosType(0) << C_UInt16(0);
}

void CdSerial::wEndStruct()
{
	CVarList &p = Current();
	p.Length = fPosition - p.Start;
	fPosition = p.Start;
	(*this) << TdPosType(p.Length);
	if (p.VarCount >= 0) // having name space
		wUInt16(p.VarCount);
	fPosition = p.Start + p.Length;
	fFilterRec.pop_front();
}

bool CdSerial::HasName(const char *Name)
{
	CVarList &p = Current();
	return p.Name2Iter(Name)!=p.VarList.end();
}

TdSerialTypeID CdSerial::NameType(const char *Name)
{
	CVarList &p = Current();
	list<CBaseVar*>::iterator it = p.Name2Iter(Name);
	return (it==p.VarList.end()) ? osUnknown : (*it)->Kind;
}

SIZE64 CdSerial::NamePosition(const char *Name)
{
	CVarList &p = Current();
	list<CBaseVar*>::iterator it = p.Name2Iter(Name);
	return (it==p.VarList.end()) ? -1 : (*it)->Start;
}

bool CdSerial::rValue(TdSerialTypeID Kind, const char *Name, void *OutBuf)
{
	CVarList &cur = Current();
	list<CBaseVar*>::iterator it = cur.Name2Iter(Name);

	if (it != cur.VarList.end())
	{
		bool nFlag = true;
		TdSerialTypeID CurKind = (*it)->Kind;

		switch (CurKind)
		{
			case osRecord:
				if (Kind == osRecord)
				{
					fPosition = (*it)->Start;
					if (ssize_t(OutBuf) != -1)
					{
						SIZE64 L = rBeginStruct();
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
						CVar<_INTERNAL::TShortRec> *pi =
							(CVar<_INTERNAL::TShortRec>*)(*it);
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
			case os16Packed: case os32Packed:
			case os64Packed:
				if (CurKind == Kind)
				{
					memcpy(OutBuf, (*it)->PtrData(), (*it)->SizeOf());
				} else {
					switch (Kind)
					{
						case osInt8: case osUInt8:
							*static_cast<C_UInt8*>(OutBuf) = (*it)->Int();
							break;
						case osInt16: case osUInt16: case os16Packed:
							*static_cast<C_UInt16*>(OutBuf) = (*it)->Int();
							break;
						case osInt32: case osUInt32: case os32Packed:
							*static_cast<C_UInt32*>(OutBuf) = (*it)->Int();
							break;
						case osInt64: case osUInt64: case os64Packed:
							*static_cast<C_UInt64*>(OutBuf) = (*it)->Int();
							break;
					default:
						return false;
					}
				}
				break;

			// float number
			case osFloat32: case osFloat64:
				if (CurKind == Kind)
				{
					memcpy(OutBuf, (*it)->PtrData(), (*it)->SizeOf());
				} else {
					switch (Kind)
					{
						case osFloat32:
							*static_cast<C_Float32*>(OutBuf) = (*it)->Float();
							break;
						case osFloat64:
							*static_cast<C_Float64*>(OutBuf) = (*it)->Float();
							break;
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
							UTF8ToUTF16((*it)->get<UTF8String>());
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
							UTF16ToUTF8((*it)->get<UTF16String>());
						break;
					case osStrUTF16:
						*static_cast<UTF16String*>(OutBuf) =
							(*it)->get<UTF16String>();
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

bool CdSerial::rValue(TdSerialTypeID Kind, const char *const Name, void *OutBuffer,
	ssize_t BufLen)
{
	if (Kind==osRecord || Kind==osShortRec)
	{
		CVarList &p = Current();
		list<CBaseVar*>::iterator it = p.Name2Iter(Name);
		ssize_t L;
		SIZE64 pt = 0;

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
								#ifdef COREARRAY_CODE_WARNING
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
						CVar<_INTERNAL::TShortRec> *pi =
							(CVar<_INTERNAL::TShortRec>*)(*it);
						L = pi->X.Len;
						if (L != BufLen)
						{
							#ifdef COREARRAY_CODE_WARNING
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

void CdSerial::wValue(TdSerialTypeID Kind, const char * Name,
	const void * InBuf, ssize_t BufLen)
{
	if (COREARRAY_VALID_TYPEID(Kind))
	{
		CVarList &Cur = Current();

		#ifdef COREARRAY_CODE_DEBUG
		if (Cur.VarCount < 0) // if namespace
			throw ErrSerial(esNoNameSpace);
		if (Cur.Name2Iter(Name) != Cur.VarList.end()) // if name exists
			throw ErrSerial(esDuplicateName, Name);
		#endif

		// Save Kind to stream
		wUInt8((C_UInt8)Kind);
		// Save property name to stream
		wPropName(Name);		
		// Update variable
		Cur.VarCount++;
		Cur.AddVar<_INTERNAL::EmptyT>(*this, Kind, fPosition, Name);

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
				#ifdef COREARRAY_CODE_DEBUG
					else throw ErrSerial(esTooLongRecord, BufLen);
				#endif
				break;
			case osNameSpace:
				wBeginNameSpace(); break;

            // integer
			case osInt8: case osUInt8:
				wUInt8(*static_cast<const C_UInt8*>(InBuf)); break;
			case osInt16: case osUInt16:
				wUInt16(*static_cast<const C_UInt16*>(InBuf)); break;
			case osInt32: case osUInt32:
				wUInt32(*static_cast<const C_UInt32*>(InBuf)); break;
			case osInt64: case osUInt64:
				wUInt64(*static_cast<const C_UInt64*>(InBuf)); break;
			case os16Packed:
            	wPack16u(*static_cast<const C_UInt16*>(InBuf)); break;
			case os32Packed:
            	wPack32u(*static_cast<const C_UInt32*>(InBuf)); break;
			case os64Packed:
            	wPack64u(*static_cast<const C_UInt64*>(InBuf)); break;

			// float number
			case osFloat32:
				wFloat32(*static_cast<const C_Float32*>(InBuf)); break;
			case osFloat64:
				wFloat64(*static_cast<const C_Float64*>(InBuf)); break;

			// string
			case osStrUTF8:
				wStrUTF8(static_cast<const UTF8*>(InBuf)); break;
			case osStrUTF16:
				wStrUTF16(static_cast<const UTF16*>(InBuf)); break;

			// others
			case osStreamPos:
				*this << *static_cast<const TdPosType*>(InBuf); break;
			default:
				return;
		}
	} else
		throw ErrSerial(esInvalidType, (int)Kind);
}

CdStream* CdSerial::BlockStream()
{
	if (!fFilterRec.empty())
	{
    	vActualPos = fStream->Position();
		CVarList &p = fFilterRec.front();
		return new CdTrimStream(fStream,
			p.Start + TdPosType::Size, p.Length - TdPosType::Size, true);
	} else
		return fStream;
}

void CdSerial::CheckInStruct()
{
	CVarList &p = Current();
	if ((fPosition<p.Start) || (fPosition>p.Start+p.Length))
		throw ErrSerial(esInvalidPos);
}

bool CdSerial::EndOfStruct()
{
	CVarList &p = Current();
	if (fPosition == p.Start+p.Length)
		return true;
	else if ((p.Start<=fPosition) && (fPosition<p.Start+p.Length))
		return false;
	else
		throw ErrSerial(esInvalidPos);
}

void CdSerial::StructInfo(SIZE64 &Start, SIZE64 &Length)
{
	CVarList &p = Current();
	Start = p.Start + TdPosType::Size;
	Length = p.Length - TdPosType::Size;
}

SIZE64 CdSerial::GetRelPos()
{
	CVarList &p = Current();
	SIZE64 r = fStream->Position() - p.Start;
	if ((r<0) || (r>p.Length))
		throw ErrSerial(esRelPosOutRange);
	return r;
}

void CdSerial::SetRelPos(const SIZE64 Value)
{
	CVarList &p = Current();
	if ((Value<0) || (Value>p.Length))
		throw ErrSerial(esRelPosOutRange);
	fPosition = p.Start + Value;
}

void CdSerial::SetLog(CdLogRecord &vLog)
{
	if (fLog != &vLog)
	{
		if (fLog) fLog->Release();
		fLog = &vLog;
		vLog.AddRef();
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
		// Read Type ID
		TdSerialTypeID Kind = (TdSerialTypeID)rUInt8();
		// Read property name
		Name = rPropName();

		#ifdef COREARRAY_CODE_WARNING
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
					Cur.AddVar<_INTERNAL::EmptyT>(*this, Kind, fPosition, Name);
					TdPosType PI; *this >> PI;
					if (PI < TdPosType::Size)
						throw ErrSerial(esInvalidStructLength);
					fPosition += PI - TdPosType::Size;
				}
    	        break;
			case osShortRec:
				{
					_INTERNAL::TShortRec &X =
						Cur.AddVar<_INTERNAL::TShortRec>(
						*this, Kind, fPosition, Name);
                	X.Len = rUInt8();
                	Read((void*)X.Data, X.Len);
				}
                break;

        	// integer
			case osInt8:
				Cur.AddVar<C_Int8>(*this, Kind, fPosition, Name) =
					(C_Int8)rUInt8();
				break;
			case osUInt8:
				Cur.AddVar<C_UInt8>(*this, Kind, fPosition, Name) =
					rUInt8();
				break;
			case osInt16:
				Cur.AddVar<C_Int16>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I16((C_Int16)rUInt16());
				break;
			case osUInt16:
				Cur.AddVar<C_UInt16>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I16(rUInt16());
				break;
			case osInt32:
				Cur.AddVar<C_Int32>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I32((C_Int32)rUInt32());
				break;
			case osUInt32:
				Cur.AddVar<C_UInt32>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I32(rUInt32());
				break;
			case osInt64:
				Cur.AddVar<C_Int64>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I64((C_Int64)rUInt64());
				break;
			case osUInt64:
				Cur.AddVar<C_UInt64>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I64(rUInt64());
				break;
			case os16Packed:
				Cur.AddVar<C_UInt16>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I16(rPack16u());
            	break;
			case os32Packed:
				Cur.AddVar<C_UInt32>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I32(rPack32u());
				break;
			case os64Packed:
				Cur.AddVar<C_UInt64>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_I64(rPack64u());
				break;

			// float
			case osFloat32:
				Cur.AddVar<C_Float32>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_F32(rFloat32());
				break;
			case osFloat64:
				Cur.AddVar<C_Float64>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_F64(rFloat64());
				break;

			// string
			case osStrUTF8:
				Cur.AddVar<UTF8String>(*this, Kind, fPosition, Name) =
					rStrUTF8();
				break;
			case osStrUTF16:
				Cur.AddVar<UTF16String>(*this, Kind, fPosition, Name) =
					COREARRAY_ENDIAN_CVT_S16(rStrUTF16());
				break;

            // others
			case osStreamPos:
				*this >> Cur.AddVar<TdPosType>(*this, Kind, fPosition, Name);
				break;

			default:
				throw ErrSerial(esInvalidType, (int)Kind);
		}
	}
}

CdSerial::CVarList & CdSerial::Current()
{
	if (fFilterRec.size() <= 0)
		throw ErrSerial(esStackEmpty);
	return fFilterRec.front();
}

void CdSerial::CVarList::_AddVarItem(CdSerial &Filter, CBaseVar *rec)
{
	#ifdef COREARRAY_CODE_DEBUG
	if (VarCount < 0)
		throw ErrObject(esInvalidOp, rec->Name.c_str());
	#endif
	#ifdef COREARRAY_CODE_WARNING
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

bool CdSerial::TdVar::rShortBuf(C_Int32 *pval, size_t ValCnt)
{
	if (fFilter->rValue(osShortRec, fName, pval, ValCnt*sizeof(C_Int32)))
	{
		COREARRAY_ENDIAN_ARRAY(pval, ValCnt);
		return true;
	} else
		return false;
}

bool CdSerial::TdVar::rShortBuf(C_Int64 *pval, size_t ValCnt)
{
	if (fFilter->rValue(osShortRec, fName, pval, ValCnt*sizeof(C_Int64)))
	{
		COREARRAY_ENDIAN_ARRAY(pval, ValCnt);
		return true;
	} else
		return false;
}

bool CdSerial::TdVar::rBuf(C_Int32 *pval, size_t ValCnt)
{
	if (fFilter->rValue(osRecord, fName, pval, ValCnt*sizeof(C_Int32)))
	{
		COREARRAY_ENDIAN_ARRAY(pval, ValCnt);
		return true;
	} else
		return false;
}

bool CdSerial::TdVar::rBuf(C_Int64 *pval, size_t ValCnt)
{
	if (fFilter->rValue(osRecord, fName, pval, ValCnt*sizeof(C_Int64)))
	{
		COREARRAY_ENDIAN_ARRAY(pval, ValCnt);
		return true;
	} else
		return false;
}

void CdSerial::TdVar::wShortBuf(const C_Int32 *pval, size_t ValCnt)
{
#ifndef COREARRAY_ENDIAN_LITTLE
	if (ValCnt <= 16384)
	{
		C_Int32 Buffer[16384], *p=Buffer;
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I64(*pval ++);
		fFilter->wValue(osShortRec, fName, Buffer, ValCnt*sizeof(C_Int32));
	} else {
		vector<C_Int32> Buffer(ValCnt);
		C_Int32 *p = &(Buffer[0]);
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I64(*pval ++);
		fFilter->wValue(osShortRec, fName, &(Buffer[0]), ValCnt*sizeof(C_Int32));
	}
#else
	fFilter->wValue(osShortRec, fName, pval, ValCnt*sizeof(C_Int32));
#endif
}

void CdSerial::TdVar::wShortBuf(const C_Int64 *pval, size_t ValCnt)
{
#ifndef COREARRAY_ENDIAN_LITTLE
	if (ValCnt <= 8192)
	{
		C_Int64 Buffer[8192], *p=Buffer;
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I64(*pval ++);
		fFilter->wValue(osShortRec, fName, Buffer, ValCnt*sizeof(C_Int64));
	} else {
		vector<C_Int64> Buffer(ValCnt);
		C_Int64 *p = &(Buffer[0]);
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I64(*pval ++);
		fFilter->wValue(osShortRec, fName, &(Buffer[0]), ValCnt*sizeof(C_Int64));
	}
#else
	fFilter->wValue(osShortRec, fName, pval, ValCnt*sizeof(C_Int64));
#endif
}

void CdSerial::TdVar::wBuf(const C_Int32 *pval, size_t ValCnt)
{
#ifndef COREARRAY_ENDIAN_LITTLE
	if (ValCnt <= 16384)
	{
		C_Int32 Buffer[16384], *p=Buffer;
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I32(*pval ++);
		fFilter->wValue(osRecord, fName, Buffer, ValCnt*sizeof(C_Int32));
	} else {
		vector<C_Int32> Buffer(ValCnt);
		C_Int32 *p = &(Buffer[0]);
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I32(*pval ++);
		fFilter->wValue(osRecord, fName, &(Buffer[0]), ValCnt*sizeof(C_Int32));
	}
#else
	fFilter->wValue(osRecord, fName, pval, ValCnt*sizeof(C_Int32));
#endif
}

void CdSerial::TdVar::wBuf(const C_Int64 *pval, size_t ValCnt)
{
#ifndef COREARRAY_ENDIAN_LITTLE
	if (ValCnt <= 8192)
	{
		C_Int64 Buffer[8192], *p=Buffer;
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I64(*pval ++);
		fFilter->wValue(osRecord, fName, Buffer, ValCnt*sizeof(C_Int64));
	} else {
		vector<C_Int64> Buffer(ValCnt);
		C_Int64 *p = &(Buffer[0]);
		for (size_t L=ValCnt; L > 0; L--)
			*p ++ = COREARRAY_ENDIAN_CVT_I64(*pval ++);
		fFilter->wValue(osRecord, fName, &(Buffer[0]), ValCnt*sizeof(C_Int64));
	}
#else
	fFilter->wValue(osRecord, fName, pval, ValCnt*sizeof(C_Int64));
#endif
}


// CdObjClassMgr

CdObjClassMgr::CdObjClassMgr(): CdAbstractManager() {}

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
		throw ErrObject(esDupClass, ClassName);
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

CdObjRef* CdObjClassMgr::ToObj(CdSerial &Reader, TdInit OnInit,
	void *Data, bool Silent)
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
			_INTERNAL::CdObject_LoadStruct(*Obj, Reader, Version);
		} else
			throw ErrSerial(esInvalidStreamName, Name.c_str());
	} catch (exception &E) {
		Reader.Log().Add(E.what());
		delete Obj;
		Obj = NULL;
		if (!Silent)
		{
			Reader.rEndStruct();
			throw;
		}
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
		throw ErrSerial(esNoClassName, ClassName);
}


// Define variant type of data structure.

static const char * const erDSConvert = "Can't convert '%s' to '%s'!";

Err_dsAny::Err_dsAny(TdsAny::TdsType fromType, TdsAny::TdsType toType):
	ErrConvert()
{
	const char *s1 = TdsAny::dvtNames(fromType);
	const char *s2 = TdsAny::dvtNames(toType);
	fMessage = Format(erDSConvert, s1, s2);
}

// TdsAny

TdsAny::TdsAny()
{
	dsType = dvtNULL;
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
			if (mix.aR.ptrStr8) delete mix.aR.ptrStr8;
			mix.aR.ptrStr8 = NULL;
			break;
		case dvtStr16:
			if (mix.aR.ptrStr16) delete mix.aR.ptrStr16;
			mix.aR.ptrStr16 = NULL;
			break;
		case dvtStr32:
			if (mix.aR.ptrStr32) delete mix.aR.ptrStr32;
			mix.aR.ptrStr32 = NULL;
			break;
		case dvtArray:
			if (mix.aArray.ArrayPtr)
				delete[] mix.aArray.ArrayPtr;
			mix.aArray.ArrayLength = 0;
			mix.aArray.ArrayPtr = NULL;
			break;
		case dvtObjRef:
			if (mix.aR.obj) mix.aR.obj->Release();
			mix.aR.obj = NULL;
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
		case dvtInt8:    return "C_Int8";
		case dvtUInt8:   return "C_UInt8";
		case dvtInt16:   return "C_Int16";
		case dvtUInt16:  return "C_UInt16";
		case dvtInt32:   return "C_Int32";
		case dvtUInt32:  return "C_UInt32";
		case dvtInt64:   return "C_Int64";
		case dvtUInt64:  return "C_UInt64";

		// float number
		case dvtFloat32:  return "C_Float32";
		case dvtFloat64:  return "C_Float64";

		// string
		case dvtSString8:  return "UTF-8 short string";
		case dvtSString16: return "UTF-16 short string";
		case dvtSString32: return "UTF-32 short string";
		case dvtStr8:      return "UTF-8 string";
		case dvtStr16:     return "UTF-16 string";
		case dvtStr32:     return "UTF-32 string";

		// others
		case dvtBoolean: return "Boolean";
		case dvtObjRef:  return "CdObjRef";
		default:
			return "Unknown";
	}
}


#define DSDATA_INT(TYPE) \
	case dvtInt8:    return ValCvt<TYPE, C_Int8>(VAL<C_Int8>()); \
	case dvtUInt8:   return ValCvt<TYPE, C_UInt8>(VAL<C_UInt8>()); \
	case dvtInt16:   return ValCvt<TYPE, C_Int16>(VAL<C_Int16>()); \
	case dvtUInt16:  return ValCvt<TYPE, C_UInt16>(VAL<C_UInt16>()); \
	case dvtInt32:   return ValCvt<TYPE, C_Int32>(VAL<C_Int32>()); \
	case dvtUInt32:  return ValCvt<TYPE, C_UInt32>(VAL<C_UInt32>()); \
	case dvtInt64:   return ValCvt<TYPE, C_Int64>(VAL<C_Int64>()); \
	case dvtUInt64:  return ValCvt<TYPE, C_UInt64>(VAL<C_UInt64>()); \

// TdsAny : float number
#define DSDATA_FLOAT(TYPE) \
	case dvtFloat32:  return ValCvt<TYPE, C_Float32>(VAL<C_Float32>()); \
	case dvtFloat64:  return ValCvt<TYPE, C_Float64>(VAL<C_Float64>()); \


// TdsAny : string

#define DSDATA_STR(TYPE) \
	case dvtSString8: \
		return ValCvt<TYPE, UTF8String>(UTF8String(&mix.aS8.SStr8[0], &mix.aS8.SStr8[mix.aS8.SStrLen8])); \
	case dvtSString16: \
		return ValCvt<TYPE, UTF16String>(UTF16String(&mix.aS16.SStr16[0], &mix.aS16.SStr16[mix.aS16.SStrLen16])); \
	case dvtSString32: \
		return ValCvt<TYPE, UTF32String>(UTF32String(&mix.aS32.SStr32[0], &mix.aS32.SStr32[mix.aS32.SStrLen32])); \
	case dvtStr8: \
		return ValCvt<TYPE, UTF8String>(*mix.aR.ptrStr8); \
	case dvtStr16: \
		return ValCvt<TYPE, UTF16String>(*mix.aR.ptrStr16); \
	case dvtStr32: \
		return ValCvt<TYPE, UTF32String>(*mix.aR.ptrStr32);

// TdsAny : others

#define DSDATA_RETURN_OTHER(TYPE, dvt) \
	case dvtBoolean: \
		return ValCvt<TYPE, int>(VAL<C_UInt8>() ? 1 : 0); \
	default: \
		throw Err_dsAny(dsType, dvt);

#define DSDATA_RETURN_STR_OTHER(TYPE, dvt) \
	case dvtBoolean: \
		return ValCvt<TYPE, UTF8String>(VAL<C_UInt8>() ? "TRUE" : "FLASE"); \
	case dvtObjRef: \
		if (mix.aR.obj != NULL) \
			return ValCvt<TYPE, UTF8String>(mix.aR.obj->dTraitName()); \
		else \
			return ValCvt<TYPE, UTF8String>("[NULL]"); \
	default: \
		throw Err_dsAny(dsType, dvt);

C_Int8 TdsAny::GetInt8() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int8)
		DSDATA_FLOAT(C_Int8)
		DSDATA_STR(C_Int8)
		DSDATA_RETURN_OTHER(C_Int8, dvtInt8)
	}
}

C_UInt8 TdsAny::GetUInt8() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt8)
		DSDATA_FLOAT(C_UInt8)
		DSDATA_STR(C_UInt8)
		DSDATA_RETURN_OTHER(C_UInt8, dvtUInt8)
	}
}

C_Int16 TdsAny::GetInt16() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int16)
		DSDATA_FLOAT(C_Int16)
		DSDATA_STR(C_Int16)
		DSDATA_RETURN_OTHER(C_Int16, dvtInt16)
	}
}

C_UInt16 TdsAny::GetUInt16() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt16)
		DSDATA_FLOAT(C_UInt16)
		DSDATA_STR(C_UInt16)
		DSDATA_RETURN_OTHER(C_UInt16, dvtUInt16)
	}
}

C_Int32 TdsAny::GetInt32() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int32)
		DSDATA_FLOAT(C_Int32)
		DSDATA_STR(C_Int32)
		DSDATA_RETURN_OTHER(C_Int32, dvtInt32)
	}
}

C_UInt32 TdsAny::GetUInt32() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt32)
		DSDATA_FLOAT(C_UInt32)
		DSDATA_STR(C_UInt32)
		DSDATA_RETURN_OTHER(C_UInt32, dvtUInt32)
	}
}

C_Int64 TdsAny::GetInt64() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int64)
		DSDATA_FLOAT(C_Int64)
		DSDATA_STR(C_Int64)
		DSDATA_RETURN_OTHER(C_Int64, dvtInt64)
	}
}

C_UInt64 TdsAny::GetUInt64() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt64)
		DSDATA_FLOAT(C_UInt64)
		DSDATA_STR(C_UInt64)
		DSDATA_RETURN_OTHER(C_UInt64, dvtUInt64)
	}
}

C_Float32 TdsAny::GetFloat32() const
{
	switch (dsType)
	{
		case dvtNULL: return NaN;
		DSDATA_INT(C_Float32)
		DSDATA_FLOAT(C_Float32)
		DSDATA_STR(C_Float32)
		DSDATA_RETURN_OTHER(C_Float32, dvtFloat32)
	}
}

C_Float64 TdsAny::GetFloat64() const
{
	switch (dsType)
	{
		case dvtNULL: return NaN;
		DSDATA_INT(C_Float64)
		DSDATA_FLOAT(C_Float64)
		DSDATA_STR(C_Float64)
		DSDATA_RETURN_OTHER(C_Float64, dvtFloat64)
	}
}

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
		return mix.aR.const_ptr;
	} else
		throw Err_dsAny(dsType, dvtPointer);
}

TdsAny *TdsAny::GetArray() const
{
	if (dsType == dvtArray)
	{
		return mix.aArray.ArrayPtr;
	} else
		throw Err_dsAny(dsType, dvtArray);
}

C_UInt32 TdsAny::GetArrayLength() const
{
	if (dsType == dvtArray)
	{
		return mix.aArray.ArrayLength;
	} else
		throw Err_dsAny(dsType, dvtArray);
}

CdObjRef *TdsAny::GetObj() const
{
	if (dsType == dvtObjRef)
		return mix.aR.obj;
	else
		throw Err_dsAny(dsType, dvtObjRef);
}

void TdsAny::SetEmpty()
{
	_Done();
	dsType = dvtNULL;
}

void TdsAny::SetInt8(const C_Int8 val)
{
	_Done();
	dsType = dvtInt8;
	VAL<C_Int8>() = val;
}

void TdsAny::SetUInt8(const C_UInt8 val)
{
	_Done();
	dsType = dvtUInt8;
	VAL<C_UInt8>() = val;
}

void TdsAny::SetInt16(const C_Int16 val)
{
	_Done();
	dsType = dvtInt16;
	VAL<C_Int16>() = val;
}

void TdsAny::SetUInt16(const C_UInt16 val)
{
	_Done();
	dsType = dvtUInt16;
	VAL<C_UInt16>() = val;
}

void TdsAny::SetInt32(const C_Int32 val)
{
	_Done();
	dsType = dvtInt32;
	VAL<C_Int32>() = val;
}

void TdsAny::SetUInt32(const C_UInt32 val)
{
	_Done();
	dsType = dvtUInt32;
	VAL<C_UInt32>() = val;
}

void TdsAny::SetInt64(const C_Int64 val)
{
	_Done();
	dsType = dvtInt64;
	VAL<C_Int64>() = val;
}

void TdsAny::SetUInt64(const C_UInt64 val)
{
	_Done();
	dsType = dvtUInt64;
	VAL<C_UInt64>() = val;
}

void TdsAny::SetFloat32(const C_Float32 val)
{
	_Done();
	dsType = dvtFloat32;
	VAL<C_Float32>() = val;
}

void TdsAny::SetFloat64(const C_Float64 val)
{
	_Done();
	dsType = dvtFloat64;
	VAL<C_Float64>() = val;
}

void TdsAny::SetStr8(const UTF8String &val)
{
	_Done();
	if (val.size() <= 22)
	{
		dsType = dvtSString8;
		mix.aS8.SStrLen8 = val.size();
		memcpy(mix.aS8.SStr8, val.c_str(), val.size());
	} else {
		dsType = dvtStr8;
		mix.aR.ptrStr8 = new UTF8String(val);
    }
}

void TdsAny::SetStr16(const UTF16String &val)
{
	_Done();
	if (val.size() <= 11)
	{
		dsType = dvtSString16;
		mix.aS16.SStrLen16 = val.size();
		memcpy(mix.aS16.SStr16, val.c_str(), val.size()*2);
	} else {
		dsType = dvtStr16;
		mix.aR.ptrStr16 = new UTF16String(val);
    }
}

void TdsAny::SetStr32(const UTF32String &val)
{
	_Done();
	if (val.size() <= 5)
	{
		dsType = dvtSString32;
		mix.aS32.SStrLen32 = val.size();
		memcpy(mix.aS32.SStr32, val.c_str(), val.size()*4);
	} else {
		dsType = dvtStr32;
		mix.aR.ptrStr32 = new UTF32String(val);
    }
}

void TdsAny::SetBool(const bool val)
{
	_Done();
	dsType = dvtBoolean;
	VAL<C_UInt8>() = (C_UInt8)val;
}

void TdsAny::SetPtr(const void *ptr)
{
	_Done();
	dsType = dvtPointer;
	mix.aR.const_ptr = ptr;
}

void TdsAny::SetArray(C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new TdsAny[size];
}

void TdsAny::SetArray(const C_Int32 ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new TdsAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const C_Int64 ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new TdsAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const C_Float64 ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new TdsAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const char* const ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new TdsAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = UTF8String(ptr[i]);
}

void TdsAny::SetArray(const std::string ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new TdsAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = ptr[i];
}

void TdsAny::SetArray(const bool ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new TdsAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i].SetBool(ptr[i]);
}

void TdsAny::SetObj(CdObjRef *obj)
{
	_Done();
	dsType = dvtObjRef;
	VAL<CdObjRef*>() = obj;
	if (obj)
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
	return (dvtInt8 <= dsType) && (dsType <= dvtUInt64);
}

bool TdsAny::IsFloat() const
{
	return (dvtFloat32 <= dsType) && (dsType <= dvtFloat64);
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
		case dvtNULL:
			return true;
		case dvtInt8: case dvtUInt8:
		case dvtInt16: case dvtUInt16:
		case dvtInt32: case dvtUInt32:
		case dvtInt64: case dvtUInt64:
			return false;
		case dvtFloat32:
			return !IsFinite(VAL<C_Float32>());
		case dvtFloat64:
			return !IsFinite(VAL<C_Float64>());
		default:
			return true;
	}
}

bool TdsAny::IsNA() const
{
	switch (dsType)
	{
		case dvtNULL:
			return true;
		case dvtFloat32:
			return !IsFinite(VAL<C_Float32>());
		case dvtFloat64:
			return !IsFinite(VAL<C_Float64>());
		default:
			return false;
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
	C_Int64 I = 0;
	C_UInt64 U = 0;
	TdsType t = dsType;

	switch (dsType)
	{
		case dvtInt32:
			I = VAL<C_Int32>(); break;
		case dvtInt64:
        	I = VAL<C_Int64>(); break;
		case dvtUInt64:
			U = VAL<C_UInt64>(); break;
		default:
			return false;
	}

	if (dsType != dvtUInt64)
	{
		if (xRANGE(INT8_MIN, I, INT8_MAX)) {
    		dsType = dvtInt8; VAL<C_Int8>() = I;
		} else if (xRANGE(0, I, UINT8_MAX)) {
			dsType = dvtUInt8; VAL<C_UInt8>() = I;
		} else if (xRANGE(INT16_MIN, I, INT16_MAX)) {
			dsType = dvtInt16; VAL<C_Int16>() = I;
		} else if (xRANGE(0, I, UINT16_MAX)) {
			dsType = dvtUInt16; VAL<C_UInt16>() = I;
		} else if (xRANGE(INT32_MIN, I, INT32_MAX)) {
			dsType = dvtInt32; VAL<C_Int32>() = I;
		} else if (xRANGE(0, I, UINT32_MAX)) {
			dsType = dvtUInt32; VAL<C_UInt32>() = I;
		}
	} else {
		if (yRANGE(U, INT8_MAX)) {
    		dsType = dvtInt8; VAL<C_Int8>() = U;
		} else if (yRANGE(U, UINT8_MAX)) {
			dsType = dvtUInt8; VAL<C_UInt8>() = U;
		} else if (yRANGE(U, INT16_MAX)) {
			dsType = dvtInt16; VAL<C_Int16>() = U;
		} else if (yRANGE(U, UINT16_MAX)) {
			dsType = dvtUInt16; VAL<C_UInt16>() = U;
		} else if (yRANGE(U, INT32_MAX)) {
			dsType = dvtInt32; VAL<C_Int32>() = U;
		} else if (yRANGE(U, UINT32_MAX)) {
			dsType = dvtUInt32; VAL<C_UInt32>() = U;
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
			C_Int64 I1 = GetInt64();
			C_Int64 I2 = D.GetInt64();
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
				if (mix.aR.obj)
					mix.aR.obj->AddRef();
				break;
			case dvtStr8:
				dsType = dvtStr8;
				mix.aR.ptrStr8 = new UTF8String(*_Right.mix.aR.ptrStr8);
				break;
			case dvtStr16:
				dsType = dvtStr16;
				mix.aR.ptrStr16 = new UTF16String(*_Right.mix.aR.ptrStr16);
				break;
			case dvtStr32:
				dsType = dvtStr32;
				mix.aR.ptrStr32 = new UTF32String(*_Right.mix.aR.ptrStr32);
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
			out.VAL<C_UInt8>() = s.rUInt8();
			break;
		case TdsAny::dvtInt16: case TdsAny::dvtUInt16:
			out.VAL<C_UInt16>() = COREARRAY_ENDIAN_CVT_I16(s.rUInt16());
			break;
		case TdsAny::dvtInt32: case TdsAny::dvtUInt32:
			out.VAL<C_UInt32>() = COREARRAY_ENDIAN_CVT_I32(s.rUInt32());
			break;
		case TdsAny::dvtInt64: case TdsAny::dvtUInt64:
			out.VAL<C_UInt64>() = COREARRAY_ENDIAN_CVT_I64(s.rUInt64());
			break;

		// float number
		case TdsAny::dvtFloat32:
			out.VAL<C_Float32>() = COREARRAY_ENDIAN_CVT_F32(s.rFloat32());
			break;
		case TdsAny::dvtFloat64:
			out.VAL<C_Float64>() = COREARRAY_ENDIAN_CVT_F64(s.rFloat64());
			break;

		// string
		case TdsAny::dvtSString8:
			out.mix.aS8.SStrLen8 = s.rUInt8();
			if (out.mix.aS8.SStrLen8 > 22)
			{
            	throw Err_dsAny("Invalid length (%d) for dvtSString8",
            		out.mix.aS8.SStrLen8);
            }
			s.Read((void*)out.mix.aS8.SStr8, out.mix.aS8.SStrLen8);
			break;

		case TdsAny::dvtSString16:
			out.mix.aS16.SStrLen16 = s.rUInt8();
			if (out.mix.aS16.SStrLen16 > 11)
			{
            	throw Err_dsAny("Invalid length (%d) for dvtSString16",
            		out.mix.aS16.SStrLen16);
            }
			s.Read((void*)out.mix.aS16.SStr16, out.mix.aS16.SStrLen16*2);
			COREARRAY_ENDIAN_ARRAY(out.mix.aS16.SStr16, out.mix.aS16.SStrLen16);
			break;

		case TdsAny::dvtSString32:
			out.mix.aS32.SStrLen32 = s.rUInt8();
			if (out.mix.aS32.SStrLen32 > 5)
			{
            	throw Err_dsAny("Invalid length (%d) for dvtSString32",
            		out.mix.aS32.SStrLen32);
            }
			s.Read((void*)out.mix.aS32.SStr32, out.mix.aS32.SStrLen32*4);
			COREARRAY_ENDIAN_ARRAY(out.mix.aS32.SStr32, out.mix.aS32.SStrLen32);
        	break;

		case TdsAny::dvtStr8:
            out.mix.aR.ptrStr8 = new UTF8String(s.rStrUTF8());
			break;

		case TdsAny::dvtStr16:
			out.mix.aR.ptrStr16 = new UTF16String(COREARRAY_ENDIAN_CVT_S16(
				s.rStrUTF16()));
			break;

		case TdsAny::dvtStr32:
			out.mix.aR.ptrStr32 = new UTF32String(COREARRAY_ENDIAN_CVT_S32(
				s.rStrUTF32()));
			break;

		// other extended type
		case TdsAny::dvtBoolean:
			out.VAL<C_UInt8>() = s.rUInt8();
			break;

		// pointer
		case TdsAny::dvtPointer:
			out.mix.aR.const_ptr = NULL;
			break;

		// array
		case TdsAny::dvtArray:
			out.mix.aArray.ArrayLength = COREARRAY_ENDIAN_CVT_I32(s.rUInt32());
			out.mix.aArray.ArrayPtr = new TdsAny[out.mix.aArray.ArrayLength];
			for (C_UInt32 i=0; i < out.mix.aArray.ArrayLength; i++)
				s >> out.mix.aArray.ArrayPtr[i];
			break;

		// CdObjRef
		case TdsAny::dvtObjRef:
			if (s.rUInt8())
			{
				out.mix.aR.obj = dObjMgr.ToObj(s, NULL, NULL, true);
				if (out.mix.aR.obj)
					out.mix.aR.obj->AddRef();
			} else
				out.mix.aR.obj = NULL;
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
			s.wUInt8(in.VAL<C_UInt8>());
			break;
		case TdsAny::dvtInt16: case TdsAny::dvtUInt16:
        	s.wUInt16(COREARRAY_ENDIAN_CVT_I16(in.VAL<C_UInt16>()));
			break;
		case TdsAny::dvtInt32: case TdsAny::dvtUInt32:
        	s.wUInt32(COREARRAY_ENDIAN_CVT_I32(in.VAL<C_UInt32>()));
			break;
		case TdsAny::dvtInt64: case TdsAny::dvtUInt64:
        	s.wUInt64(COREARRAY_ENDIAN_CVT_I64(in.VAL<C_UInt64>()));
			break;

		// float number
		case TdsAny::dvtFloat32:
			s.wFloat32(COREARRAY_ENDIAN_CVT_F32(in.VAL<C_Float32>()));
			break;
		case TdsAny::dvtFloat64:
			s.wFloat64(COREARRAY_ENDIAN_CVT_F64(in.VAL<C_Float64>()));
			break;

		// string
		case TdsAny::dvtSString8:
			s.wUInt8(in.mix.aS8.SStrLen8);
			s.Write((void*)in.mix.aS8.SStr8, in.mix.aS8.SStrLen8);
			break;
		case TdsAny::dvtSString16:
			s.wUInt8(in.mix.aS16.SStrLen16);
			#ifndef COREARRAY_ENDIAN_LITTLE
				for (size_t i=0; i < in.mix.aS16.SStrLen16; i++)
					s.wUInt16(COREARRAY_ENDIAN_CVT_I16(in.mix.aS16.SStr16[i]));
			#else
				s.Write((void*)in.mix.aS16.SStr16, in.mix.aS16.SStrLen16*2);
			#endif
			break;
		case TdsAny::dvtSString32:
			s.wUInt8(in.mix.aS32.SStrLen32);
			#ifndef COREARRAY_ENDIAN_LITTLE
				for (size_t i=0; i < in.mix.aS32.SStrLen32; i++)
					s.wUInt32(COREARRAY_ENDIAN_CVT_I32(in.mix.aS32.SStr32[i]));
			#else
				s.Write((void*)in.mix.aS32.SStr32, in.mix.aS32.SStrLen32*4);
			#endif
        	break;
		case TdsAny::dvtStr8:
			s.wStrUTF8(in.mix.aR.ptrStr8->c_str());
			break;
		case TdsAny::dvtStr16:
			s.wStrUTF16(COREARRAY_ENDIAN_CVT_S16(*in.mix.aR.ptrStr16).c_str());
			break;
		case TdsAny::dvtStr32:
        	s.wStrUTF32(COREARRAY_ENDIAN_CVT_S32(*in.mix.aR.ptrStr32).c_str());
			break;

		// other extended type
		case TdsAny::dvtBoolean:
        	s.wUInt8(in.VAL<C_UInt8>() ? 1 : 0);
			break;

		// pointer
		case TdsAny::dvtPointer:
			break;
		// array
		case TdsAny::dvtArray:
			s.wUInt32(COREARRAY_ENDIAN_CVT_I32(in.mix.aArray.ArrayLength));
			for (C_UInt32 i=0; i < in.mix.aArray.ArrayLength; i++)
				s << in.mix.aArray.ArrayPtr[i];
			break;

		// CdObjRef
		case TdsAny::dvtObjRef:
			if (in.mix.aR.obj)
			{
				s.wUInt8(1);
				_INTERNAL::CdObject_SaveStruct(*in.mix.aR.obj, s, true);
			} else
				s.wUInt8(0);
			break;
	}
	return s;
}
