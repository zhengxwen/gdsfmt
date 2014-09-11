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
#include <dSerial.h>

#include <string.h>
#include <stdarg.h>
#include <algorithm>


using namespace std;
using namespace CoreArray;


#ifdef COREARRAY_CC_MSC
#define snprintf sprintf_s
#endif


// =====================================================================
// CdRef
// =====================================================================

ssize_t CoreArray::_INTERNAL::DirectAddRef(CdRef &Obj)
{
	Obj.fReference ++;
	return Obj.fReference;
}

ssize_t CoreArray::_INTERNAL::DirectDecRef(CdRef &Obj)
{
	Obj.fReference --;
	return Obj.fReference;
}

CdRef::CdRef()
{
	fReference = 0;
}

CdRef::~CdRef() {}

ssize_t CdRef::AddRef()
{
	fReference ++;
	return fReference;
}

ssize_t CdRef::Release()
{
	fReference --;
	ssize_t rv = fReference;
	if (fReference <= 0) delete this;
	return rv;
}


void CoreArray::_INTERNAL::CdObject_LoadStruct(CdObject &Obj,
	CdReader &Reader, TdVersion Version)
{
	Obj.LoadStruct(Reader, Version);
}

void CoreArray::_INTERNAL::CdObject_SaveStruct(CdObject &Obj,
	CdWriter &Writer, bool IncludeName)
{
    Obj.SaveStruct(Writer, IncludeName);
}



// =====================================================================
// CdObject
// =====================================================================

char const* CdObject::dName() { return ""; }

char const* CdObject::dTraitName() { return ""; }

TdVersion CdObject::dVersion() { return COREARRAY_CLASS_VERSION; }

TdVersion CdObject::SaveVersion() { return dVersion(); }

void CdObject::LoadStruct(CdReader &Reader, TdVersion Version)
{
	// call load function
	try {
		Loading(Reader, Version);
	} catch (exception &E) {
		Reader.Log().Add(E.what());
		throw;
	} catch (const char *E) {
		Reader.Log().Add(E);
		throw;
	}
}

void CdObject::SaveStruct(CdWriter &Writer, bool IncludeName)
{
	Writer.BeginNameSpace();
	if (IncludeName)
	{
		TdVersion Version = SaveVersion();
		Writer.Storage() << C_UInt16(Version);
		Writer.WriteClassName(dName());
	}
	Saving(Writer);
	Writer.EndStruct();
}

void CdObject::Loading(CdReader &Reader, TdVersion Version)
{ }

void CdObject::Saving(CdWriter &Writer)
{ }


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



// =====================================================================
// CdLogRecord
// =====================================================================

static const char *VAR_LOGSIZE = "LOGSIZE";
static const char *VAR_LOGDATA = "LOGDATA";

CdLogRecord::CdLogRecord(): CdObjRef() {}

void CdLogRecord::Add(const char *const str, int vType)
{
	TdItem I;
	I.Msg = UTF8Text(str); I.Type = vType;
	fList.push_back(I);
}

void CdLogRecord::Add(std::string &str, int vType)
{
	TdItem I;
	I.Msg = UTF8Text(str); I.Type = vType;
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

void CdLogRecord::Loading(CdReader &Reader, TdVersion Version)
{
	fList.clear();
	C_UInt32 Cnt = 0;
	Reader[VAR_LOGSIZE] >> Cnt;
	if (Cnt > 0)
	{
		Reader[VAR_LOGDATA].BeginStruct();
		for (C_UInt32 i=0; i < Cnt; i++)
		{
			UTF8String s = Reader.Storage().RpUTF8();
			C_Int32 type;
			Reader.Storage() >> type;
			TdItem I;
			I.Msg = s; I.Type = type;
			fList.push_back(I);
        }
		Reader.EndStruct();
	}
}

void CdLogRecord::Saving(CdWriter &Writer)
{
	vector<TdItem>::const_iterator it;
	C_UInt32 Cnt = fList.size();

	Writer[VAR_LOGSIZE] << Cnt;
	if (Cnt > 0)
	{
		Writer[VAR_LOGDATA].NewStruct();
		for (it=fList.begin(); it != fList.end(); it++)
		{
			Writer.Storage().WpUTF8(it->Msg.c_str());
			Writer.Storage() << C_Int32(it->Type);
		}
		Writer.EndStruct();
	}
}



// =====================================================================
// Memory Object
// =====================================================================

CdMemory::CdMemory(void *Ptr)
{
	_Base = _CurPtr = Ptr;
}

void CdMemory::ReadData(void *Buffer, ssize_t Count)
{
	if (Count > 0)
	{
		memcpy(Buffer, _CurPtr, Count);
		_CurPtr = ((C_UInt8*)_CurPtr) + Count;
	}
}

C_UInt8 CdMemory::R8b()
{
	C_UInt8 rv = *((C_UInt8*)_CurPtr);
	_CurPtr = ((C_UInt8*)_CurPtr) + 1;
	return rv;
}

void CdMemory::WriteData(const void *Buffer, ssize_t Count)
{
	if (Count > 0)
	{
		memcpy(_CurPtr, Buffer, Count);
		_CurPtr = ((C_UInt8*)_CurPtr) + Count;
	}
}

void CdMemory::W8b(C_UInt8 val)
{
	*((C_UInt8*)_CurPtr) = val;
	_CurPtr = ((C_UInt8*)_CurPtr) + 1;
}

SIZE64 CdMemory::Position()
{
	return ((C_UInt8*)_CurPtr) - ((C_UInt8*)_Base);
}

void CdMemory::SetPosition(const SIZE64 pos)
{
	_CurPtr = ((C_UInt8*)_Base) + pos;
}



// =====================================================================
// CdStream
// =====================================================================

// Error messages in CdStream
static const char *SReadError = "Stream read error";
static const char *SWriteError = "Stream write error";

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

SIZE64 CdStream::Position()
{
	return Seek(0, soCurrent);
}

void CdStream::SetPosition(const SIZE64 pos)
{
	Seek(pos, soBeginning);
}

void CdStream::ReadData(void *Buffer, ssize_t Count)
{
	if ((Count > 0) && (Read(Buffer, Count) != Count))
		throw ErrStream(SReadError);
}

C_UInt8 CdStream::R8b()
{
	C_UInt8 rv;
	if (Read(&rv, sizeof(rv)) != (ssize_t)sizeof(rv))
		throw ErrStream(SReadError);
	return rv;
}

C_UInt16 CdStream::R16b()
{
	C_UInt16 rv;
	if (Read(&rv, sizeof(rv)) != (ssize_t)sizeof(rv))
		throw ErrStream(SReadError);
	return rv;
}

C_UInt32 CdStream::R32b()
{
	C_UInt32 rv;
	if (Read(&rv, sizeof(rv)) != (ssize_t)sizeof(rv))
		throw ErrStream(SReadError);
	return rv;
}

C_UInt64 CdStream::R64b()
{
	C_UInt64 rv;
	if (Read(&rv, sizeof(rv)) != (ssize_t)sizeof(rv))
		throw ErrStream(SReadError);
	return rv;
}

void CdStream::WriteData(const void *Buffer, ssize_t Count)
{
	if ((Count > 0) && (Write(Buffer, Count) != Count))
		throw ErrStream(SWriteError);
}

void CdStream::W8b(C_UInt8 val)
{
	if (Write(&val, sizeof(val)) != (ssize_t)sizeof(val))
		throw ErrStream(SWriteError);
}

void CdStream::W16b(C_UInt16 val)
{
	if (Write(&val, sizeof(val)) != (ssize_t)sizeof(val))
		throw ErrStream(SWriteError);
}

void CdStream::W32b(C_UInt32 val)
{
	if (Write(&val, sizeof(val)) != (ssize_t)sizeof(val))
		throw ErrStream(SWriteError);
}

void CdStream::W64b(C_UInt64 val)
{
	if (Write(&val, sizeof(val)) != (ssize_t)sizeof(val))
		throw ErrStream(SWriteError);
}

CdStream& CdStream::operator= (const CdStream& m)
{
	return *this;
}

CdStream& CdStream::operator= (CdStream& m)
{
	return *this;
}

SIZE64 CdStream::CopyFrom(CdStream &Source, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
	SIZE64 rv;

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	rv = Count;
	while (Count > 0)
	{
		ssize_t N = (Count >= (SIZE64)sizeof(Buffer)) ?
			(ssize_t)sizeof(Buffer) : Count;
		Source.ReadData(Buffer, N);
		WriteData((void*)Buffer, N);
		Count -= N;
	}
	return rv;
}

SIZE64 CdStream::CopyFrom(CdBufStream &Source, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
	SIZE64 rv;

	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	rv = Count;
	while (Count > 0)
	{
		ssize_t N = (Count >= (SIZE64)sizeof(Buffer)) ?
			(ssize_t)sizeof(Buffer) : Count;
		Source.ReadData(Buffer, N);
		WriteData((void*)Buffer, N);
		Count -= N;
	}
	return rv;
}



// =====================================================================
// Stream Buffer Object
// =====================================================================

CdBufStream::CdBufStream(CdStream *vStream, ssize_t vBufSize): CdRef()
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
			fStream->SetPosition(vActualPos);
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
			fStream->SetPosition(vActualPos);
		}
		WriteStream();
		vActualPos = vBufStart = vBufEnd;
		OnFlush.Notify(this);
	}
}

void CdBufStream::ReadData(void *Buf, ssize_t Count)
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

C_UInt8 CdBufStream::R8b()
{
	C_UInt8 rv;
	ReadData(&rv, sizeof(rv));
	return rv;
}

C_UInt16 CdBufStream::R16b()
{
	C_UInt16 rv;
	ReadData(&rv, sizeof(rv));
	return rv;
}

C_UInt32 CdBufStream::R32b()
{
	C_UInt32 rv;
	ReadData(&rv, sizeof(rv));
	return rv;
}

C_UInt64 CdBufStream::R64b()
{
	C_UInt64 rv;
	ReadData(&rv, sizeof(rv));
	return rv;
}

void CdBufStream::WriteData(void const* Buf, ssize_t Count)
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
			memcpy((void*)(vBuffer+(ssize_t)fPosition-(ssize_t)vBufStart),
				(void*)p, L);
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

void CdBufStream::W8b(C_UInt8 val)
{
	WriteData(&val, sizeof(val));
}

void CdBufStream::W16b(C_UInt16 val)
{
	WriteData(&val, sizeof(val));
}

void CdBufStream::W32b(C_UInt32 val)
{
	WriteData(&val, sizeof(val));
}

void CdBufStream::W64b(C_UInt64 val)
{
	WriteData(&val, sizeof(val));
}

void CdBufStream::CopyFrom(CdStream &Source, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	while (Count > 0)
	{
		ssize_t N = (Count >= (SIZE64)sizeof(Buffer)) ?
			(ssize_t)sizeof(Buffer) : Count;
		Source.ReadData((void*)Buffer, N);
		WriteData((void*)Buffer, N);
		Count -= N;
	}
}

void CdBufStream::CopyFrom(CdBufStream &Source, SIZE64 Count)
{
	C_UInt8 Buffer[COREARRAY_STREAM_BUFFER];
	if (Count < 0)
	{
		Source.SetPosition(0);
		Count = Source.GetSize();
	}
	while (Count > 0)
	{
		ssize_t N = (Count >= (SIZE64)sizeof(Buffer)) ?
			(ssize_t)sizeof(Buffer) : Count;
		Source.ReadData((void*)Buffer, N);
		WriteData((void*)Buffer, N);
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

void CdBufStream::SetStream(CdStream *Value)
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
		fStream->WriteData((void*)vBuffer, vBufEnd-vBufStart);
	}
}
