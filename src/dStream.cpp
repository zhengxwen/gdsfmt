// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStream.cpp: Stream classes and functions
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

#include <dStream.h>
#include <cctype>
#include <limits>


using namespace std;
using namespace CoreArray;

static const char *SFCreateErrorEx = "Can not create file '%s'. %s";
static const char *SFOpenErrorEx = "Can not open file '%s'. %s";

static const char *rsBlockInvalidPos = "Invalid Position: %lld in CdBlockStream.";
static const char *rsInvalidBlockLen = "Invalid length of Block!";


// CoreArray GDS Stream position mask
const C_Int64 CoreArray::GDS_STREAM_POS_MASK =
	(C_Int64(0x7FFF) << 32) | C_Int64(0xFFFFFFFF);  // 0x7FFF,FFFFFFFF
const C_Int64 CoreArray::GDS_STREAM_POS_MASK_HEAD_BIT =
	(C_Int64(0x8000) << 32) | C_Int64(0x00000000);  // 0x8000,00000000


// CdHandleStream

CdHandleStream::CdHandleStream(): CdStream()
{
	fHandle = NullSysHandle;
}

CdHandleStream::CdHandleStream(TSysHandle AHandle): CdStream()
{
	fHandle = AHandle;
}

ssize_t CdHandleStream::Read(void *Buffer, ssize_t Count)
{
	if (Count > 0)
		return SysHandleRead(fHandle, Buffer, Count);
	else
		return 0;
}

ssize_t CdHandleStream::Write(const void *Buffer, ssize_t Count)
{
	if (Count > 0)
		return SysHandleWrite(fHandle, Buffer, Count);
	else
		return 0;
}

SIZE64 CdHandleStream::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
	SIZE64 rv = SysHandleSeek(fHandle, Offset, Origin);

	if (rv < 0)
	{
		RaiseLastOSError<ErrOSError>();
		return 0;
	} else
		return rv;
}

void CdHandleStream::SetSize(const SIZE64 NewSize)
{
	if (!SysHandleSetSize(fHandle, NewSize))
    	RaiseLastOSError<ErrOSError>();
}

// CdFileStream

static TSysShareMode ShMode[4] = { saNone, saRead, saNone, saNone };

CdFileStream::CdFileStream(const char * const AFileName, TdOpenMode Mode):
	CdHandleStream()
{
	Init(AFileName, Mode);
}

CdFileStream::~CdFileStream()
{
	if (fHandle != NullSysHandle)
	{
		if (!SysCloseHandle(fHandle))
			RaiseLastOSError<ErrOSError>();
	}
}

void CdFileStream::Init(const char * const AFileName, TdOpenMode mode)
{
	if (mode == fmCreate)
	{
		fHandle = SysCreateFile(AFileName, 0);
		if (fHandle == NullSysHandle)
			throw ErrStream(SFCreateErrorEx, AFileName, LastSysErrMsg().c_str());
	} else{
		fHandle = SysOpenFile(AFileName, (TSysOpenMode)(mode-fmOpenRead),
			ShMode[mode]);
		if (fHandle == NullSysHandle)
			throw ErrStream(SFOpenErrorEx, AFileName, LastSysErrMsg().c_str());
	}

	fFileName = AFileName;
	fMode = mode;
}


// File stream for forked processes

CdForkFileStream::CdForkFileStream(const char * const AFileName,
	TdOpenMode Mode): CdFileStream()
{
#ifdef COREARRAY_PLATFORM_UNIX
	Current_PID = getpid();
#endif

	if (Mode == fmCreate)
		throw ErrStream("Not support create a file in a forked process.");
	Init(AFileName, Mode);
}

ssize_t CdForkFileStream::Read(void *Buffer, ssize_t Count)
{
	RedirectFile();
	return CdFileStream::Read(Buffer, Count);
}

ssize_t CdForkFileStream::Write(const void *Buffer, ssize_t Count)
{
	RedirectFile();
	return CdFileStream::Write(Buffer, Count);
}

SIZE64 CdForkFileStream::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
	RedirectFile();
	return CdFileStream::Seek(Offset, Origin);
}

SIZE64 CdForkFileStream::GetSize()
{
	RedirectFile();
	return CdFileStream::GetSize();
}

void CdForkFileStream::SetSize(const SIZE64 NewSize)
{
	RedirectFile();
	CdFileStream::SetSize(NewSize);
}

COREARRAY_INLINE void CdForkFileStream::RedirectFile()
{
#ifdef COREARRAY_PLATFORM_UNIX
	if (Current_PID != getpid())
	{
		Current_PID = getpid();
		SIZE64 p = 0;
		if (fHandle != NullSysHandle)
		{
			p = Position();
			SysCloseHandle(fHandle);
		}
		Init(fFileName.c_str(), fMode);
		SetPosition(p);
	}
#endif
}


// CdTempStream

CdTempStream::CdTempStream(): CdFileStream(
	TempFileName("tmp", ".").c_str(), CdFileStream::fmCreate)
{ }

CdTempStream::CdTempStream(const char *Path): CdFileStream(
	TempFileName("tmp", Path).c_str(), CdFileStream::fmCreate)
{ }

CdTempStream::~CdTempStream()
{
	if (fHandle != NullSysHandle)
	{
		if (!SysCloseHandle(fHandle))
		{
			fHandle = NullSysHandle;
			RaiseLastOSError<ErrOSError>();
        }
		fHandle = NullSysHandle;
		remove(fFileName.c_str());
	}
}

// CdMemoryStream

// TODO
CdMemoryStream::CdMemoryStream(size_t Size): CdStream()
{
//	InitMemAllocator(fAllocator, Size);
	fPosition = 0;
}

ssize_t CdMemoryStream::Read(void *Buffer, ssize_t Count)
{
//	fAllocator.Read(fPosition, Buffer, Count);
	fPosition += Count;
	return Count;
}

ssize_t CdMemoryStream::Write(const void *Buffer, ssize_t Count)
{
//	fAllocator.Write(fPosition, Buffer, Count);
	fPosition += Count;
	return Count;
}

SIZE64 CdMemoryStream::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
/*	switch (Origin)
	{
		case soBeginning:
			fPosition = Offset;
			break;
		case soCurrent:
        	fPosition += Offset;
			break;
		case soEnd:
        	fPosition += fAllocator.Capacity;
			break;
		default:
			return -1;
	}
	if ((fPosition < 0) || (fPosition > fAllocator.Capacity))
		throw ErrStream("Invalid position (%d).", fPosition);
*/	return fPosition;
}

SIZE64 CdMemoryStream::GetSize()
{
	return 0;
//    return fAllocator.Capacity;
}

void CdMemoryStream::SetSize(const SIZE64 NewSize)
{
//	fAllocator.SetCapacity(NewSize);
}

void *CdMemoryStream::BufPointer()
{
	return NULL;
// return fAllocator.Base;
}


#ifndef COREARRAY_NO_STD_IN_OUT

// CdStdInStream

CdStdInStream::CdStdInStream(): CdStream()
{ }

CdStdInStream::~CdStdInStream()
{ }

ssize_t CdStdInStream::Read(void *Buffer, ssize_t Count)
{
	cin.read((char*)Buffer, Count);
	return Count;
}

ssize_t CdStdInStream::Write(const void *Buffer, ssize_t Count)
{
	throw ErrStream("Invalid CdStdInStream::Write.");
}

SIZE64 CdStdInStream::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
	return 0;
}

SIZE64 CdStdInStream::GetSize()
{
	return 0;
}

void CdStdInStream::SetSize(const SIZE64 NewSize)
{
	throw ErrStream("Invalid CdStdInStream::SetSize.");
}

// CdStdOutStream

CdStdOutStream::CdStdOutStream(): CdStream()
{ }

CdStdOutStream::~CdStdOutStream()
{ }

ssize_t CdStdOutStream::Read(void *Buffer, ssize_t Count)
{
	throw ErrStream("Invalid CdStdOutStream::Read.");
}

ssize_t CdStdOutStream::Write(const void *Buffer, ssize_t Count)
{
	cout.write((const char*)Buffer, Count);
	return Count;
}

SIZE64 CdStdOutStream::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
	return 0;
}

SIZE64 CdStdOutStream::GetSize()
{
	return 0;
}

void CdStdOutStream::SetSize(const SIZE64 NewSize)
{
	throw ErrStream("Invalid CdStdOutStream::SetSize.");
}

#endif


// CdBaseZStream

CdBaseZStream::CdBaseZStream(CdStream *vStream): CdStream()
{
	(fStream = vStream)->AddRef();
	fStreamBase = fStreamPos = vStream->Position();
	fTotalIn = fTotalOut = 0;

	memset((void*)&fZStream, 0, sizeof(z_stream));
	memset((void*)fBuffer, 0, sizeof(fBuffer));
}

CdBaseZStream::~CdBaseZStream()
{
	if (fStream)
		fStream->Release();
}


static const char *SZDeflateInvalid =
	"Invalid Zip Deflate Stream operation '%s'!";
static const char *SZInflateInvalid =
	"Invalid Zip Inflate Stream operation '%s'!";

static short ZLevels[13] =
	{
		Z_NO_COMPRESSION,       // zcNone
		Z_BEST_SPEED,           // zcFastest
		Z_DEFAULT_COMPRESSION,  // zcDefault
		Z_BEST_COMPRESSION,     // zcMax
		1, 2, 3, 4, 5, 6, 7, 8, 9
	};

static short ZStrategies[5] =
	{
		Z_DEFAULT_STRATEGY,     // zsDefault
		Z_FILTERED,             // zsFiltered
		Z_HUFFMAN_ONLY,         // zsHuffman
		Z_RLE,                  // zsRLE
		Z_FIXED                 // zsFixed
	};

COREARRAY_INLINE static int ZCheck(int Code)
{
	if (Code < 0) throw EZLibError(Code);
	return Code;
}

// CdZIPDeflate

CdZIPDeflate::CdZIPDeflate(CdStream* Dest, TZLevel DeflateLevel):
	CdBaseZStream(Dest)
{
	PtrExtRec = NULL;
	fZStream.next_out = fBuffer;
	fZStream.avail_out = sizeof(fBuffer);
	fHaveClosed = false;

	ZCheck( deflateInit_(&fZStream, ZLevels[DeflateLevel],
		ZLIB_VERSION, sizeof(fZStream)) );
}

CdZIPDeflate::CdZIPDeflate(CdStream* Dest, TZLevel DeflateLevel,
	int windowBits, int memLevel, TZStrategy Strategy): CdBaseZStream(Dest)
{
	fZStream.next_out = fBuffer;
	fZStream.avail_out = sizeof(fBuffer);
	fHaveClosed = false;

	#define Z_DEFLATED 8
	ZCheck( deflateInit2_(&fZStream, ZLevels[DeflateLevel],
		Z_DEFLATED, windowBits, memLevel, ZStrategies[Strategy],
		ZLIB_VERSION, sizeof(fZStream)) );
	#undef Z_DEFLATED
}

CdZIPDeflate::~CdZIPDeflate()
{
	try {
		Close();
	} catch (...) {
		deflateEnd(&fZStream);
		throw;
	}
	deflateEnd(&fZStream);
}

ssize_t CdZIPDeflate::Read(void *Buffer, ssize_t Count)
{
	throw EZLibError(SZDeflateInvalid, "Read");
}

ssize_t CdZIPDeflate::Write(const void *Buffer, ssize_t Count)
{
	fZStream.next_in = (Bytef*)Buffer;
	fZStream.avail_in = Count;
	if (fStream->Position() != fStreamPos)
		fStream->SetPosition(fStreamPos);

	int L = fZStream.avail_in;
	while (fZStream.avail_in > 0)
	{
		ZCheck(deflate(&fZStream, Z_NO_FLUSH));
		fTotalIn += L - fZStream.avail_in;
		L = fZStream.avail_in;
		if (fZStream.avail_out == 0)
		{
			fStream->WriteData((void*)fBuffer, sizeof(fBuffer));
			fStreamPos += sizeof(fBuffer);
			fTotalOut += sizeof(fBuffer);
			fZStream.next_out = fBuffer;
			fZStream.avail_out = sizeof(fBuffer);
		}
	}

	if (Count > 0) fHaveClosed = false;
	return Count;
}

SIZE64 CdZIPDeflate::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
	switch (Origin)
	{
		case soBeginning:
			if (Offset == fTotalIn) return fTotalIn;
			break;
		case soCurrent:
			if (Offset == 0) return fTotalIn;
			break;
		case soEnd:
			if (Offset == 0) return fTotalIn;
			break;
	}
	throw EZLibError(SZDeflateInvalid, "Seek");
}

void CdZIPDeflate::SetSize(const SIZE64 NewSize)
{
	if (NewSize != fTotalIn)
		throw EZLibError(SZDeflateInvalid, "SetSize");
}

void CdZIPDeflate::Close()
{
	SyncFlush(Z_FINISH);
}

ssize_t CdZIPDeflate::Pending()
{
	return sizeof(fBuffer) - fZStream.avail_out;
}

void CdZIPDeflate::SyncFlush(int Code)
{
	if (!fHaveClosed)
	{
		if (PtrExtRec)
		{
			WriteData((void*)PtrExtRec->Buf, PtrExtRec->Size);
			PtrExtRec = NULL;
		}

		fZStream.next_in = NULL;
		fZStream.avail_in = 0;
		if (fStream->Position() != fStreamPos)
			fStream->SetPosition(fStreamPos);

		while (ZCheck(deflate(&fZStream, Code)) != Z_STREAM_END)
		{
			int L = sizeof(fBuffer) - fZStream.avail_out;
			fStream->WriteData((void*)fBuffer, L);
			fTotalOut += L;
			fZStream.next_out = fBuffer;
			fZStream.avail_out = sizeof(fBuffer);
		}

		if (fZStream.avail_out < sizeof(fBuffer))
		{
			int L = sizeof(fBuffer)-fZStream.avail_out;
			fStream->WriteData((void*)fBuffer, L);
			fTotalOut += L;
		}

		fHaveClosed = true;
	}
}

// CdZIPInflate

CdZIPInflate::CdZIPInflate(CdStream* Source):
	CdBaseZStream(Source)
{
	fZStream.next_in = fBuffer;
	fZStream.avail_in = 0;
	fBlockSize = 1024*1024; // 1024K
	fRandomAccess = false;
	fBlockStart = fCurPos = 0;
	ZCheck(inflateInit_(&fZStream, ZLIB_VERSION, sizeof(fZStream)));
}

CdZIPInflate::CdZIPInflate(CdStream* Source, int windowBits):
	CdBaseZStream(Source)
{
	fZStream.next_in = fBuffer;
	fZStream.avail_in = 0;
	fBlockSize = 1024*1024; // 1024K
	fRandomAccess = false;
	fBlockStart = fCurPos = 0;
	ZCheck(inflateInit2_(&fZStream, windowBits, ZLIB_VERSION, sizeof(fZStream)));
}

CdZIPInflate::~CdZIPInflate()
{
	inflateEnd(&fZStream);
	ClearPoints();
}

ssize_t CdZIPInflate::Read(void *Buffer, ssize_t Count)
{
	fZStream.next_out = (Bytef*)Buffer;
	if (fStream->Position() != fStreamPos)
		fStream->SetPosition(fStreamPos);

	int OldOut, ZResult = Z_OK;
	ssize_t rv=0, L;

	while (Count>0 && ZResult!=Z_STREAM_END)
	{
		if (fZStream.avail_in == 0)
		{
			fZStream.avail_in = fStream->Read((void*)fBuffer, sizeof(fBuffer));
			if (fZStream.avail_in == 0)
				return rv;
			fStreamPos += fZStream.avail_in;
			fZStream.next_in = fBuffer;
		}

		L = fBlockStart + fBlockSize - fCurPos;
		fZStream.avail_out = (Count>=L) ? L : Count;
		OldOut = fZStream.avail_out;
		ZResult = ZCheck(inflate(&fZStream, Z_NO_FLUSH));

		L = OldOut-fZStream.avail_out;
		fCurPos += L; Count -= L; rv += L;

		if (fCurPos >= fBlockStart + fBlockSize)
		{
			fBlockStart += fBlockSize;
			// Add an access point
			if (fRandomAccess)
			{
				int DivI = fCurPos / fBlockSize;
				if (DivI > 0)
				{
					TZIPPointRec * pRec = PointIndex(DivI-1);
					if (!pRec)
					{
						// Should be current position, i.e BlockSize time an integer
						pRec = AddPoint();
						pRec->SourcePos = fStreamPos - fZStream.avail_in;
						inflateCopy(&pRec->Rec, &fZStream);
						pRec->Rec.avail_in = 0;
					}
				}
			}
		}
	}

	if ((ZResult==Z_STREAM_END) && (fZStream.avail_in>0))
	{
		fStream->SetPosition(fStream->Position() - fZStream.avail_in);
		fStreamPos = fStream->Position();
		fZStream.avail_in = 0;
	}

	if (fStreamPos-fStreamBase > fTotalIn)
		fTotalIn = fStreamPos - fStreamBase;
	if (fCurPos > fTotalOut)
		fTotalOut = fCurPos;

	return rv;
}

ssize_t CdZIPInflate::Write(const void *Buffer, ssize_t Count)
{
	throw EZLibError(SZInflateInvalid, "Write");
}

SIZE64 CdZIPInflate::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
	if ((Offset==0) && (Origin==soBeginning))
	{
		ZCheck(inflateReset(&fZStream));
		fZStream.next_in = fBuffer;
		fZStream.avail_in = 0;
		fStream->SetPosition(0);
		fStreamPos = fCurPos = fBlockStart = 0;
		return 0;
	} else if (Origin != soEnd)
	{
		if (Offset==0 && Origin==soCurrent)
			return fCurPos;

		SIZE64 vOff = (Origin==soCurrent) ? (fCurPos + Offset) : Offset;

		if (vOff < fCurPos)
		{
			if (fRandomAccess && vOff>=fBlockSize)
			{
				inflateEnd(&fZStream);
				int i = vOff / fBlockSize;
				TZIPPointRec *p = PointIndexEx(i-1);
				fStream->SetPosition(fStreamPos = p->SourcePos);
				fBlockStart = fCurPos = (SIZE64)i * fBlockSize;
				inflateCopy(&fZStream, &p->Rec);
				vOff -= fCurPos;
			} else
				Seek(0, soBeginning);
		} else {
			if (fRandomAccess && vOff>=fBlockStart+fBlockSize)
			{
				int i = vOff / fBlockSize;
				TZIPPointRec *p = PointIndex(i-1);
				if (p)
				{
					inflateEnd(&fZStream);
					fStream->SetPosition(fStreamPos = p->SourcePos);
					fBlockStart = fCurPos = (SIZE64)i * fBlockSize;
					inflateCopy(&fZStream, &p->Rec);
                }
			}
			vOff -= fCurPos;
		}

    	char buf[4096*2];
		int DivI = vOff / sizeof(buf);
		while (DivI > 0)
		{
			ReadData((void*)buf, sizeof(buf));
			DivI--;
        }
		ReadData((void*)buf, vOff % sizeof(buf));
	} else
		throw EZLibError(SZInflateInvalid, "Seek");

	return fCurPos;
}

SIZE64 CdZIPInflate::GetSize()
{
	return -1;
}

void CdZIPInflate::SetSize(const SIZE64 NewSize)
{
	throw EZLibError(SZInflateInvalid, "SetSize");
}

void CdZIPInflate::ClearPoints()
{
	vector<TZIPPointRec>::iterator it;
	for (it=vPoints.begin(); it != vPoints.end(); it++)
		inflateEnd(&it->Rec);
	vPoints.clear();
}

void CdZIPInflate::SetRandomAccess(bool Value)
{
	if (Value != fRandomAccess)
	{
		fRandomAccess = Value;
		ClearPoints();
	}
}

void CdZIPInflate::SetBlockSize(ssize_t Value)
{
	if (fBlockSize!=Value && Value>=(ssize_t)sizeof(fBuffer))
	{
		fBlockSize = Value;
		ClearPoints();
	}
}

CdZIPInflate::TZIPPointRec *CdZIPInflate::AddPoint()
{
	vPoints.resize(vPoints.size()+1);
	return &vPoints.back();
}

CdZIPInflate::TZIPPointRec *CdZIPInflate::PointIndex(unsigned int i)
{
	if (i < vPoints.size())
		return &vPoints[i];
	else return NULL;
}

CdZIPInflate::TZIPPointRec *CdZIPInflate::PointIndexEx(unsigned int i)
{
	return &vPoints.at(i);
}

// EZLibError

EZLibError::EZLibError(int Code): ErrStream()
{
	fErrCode = Code;
	fMessage = zError(Code);
}


// CdBlockStream

static void xClearList(CdBlockStream::TBlockInfo *Head)
{
	CdBlockStream::TBlockInfo *p = Head;
	while (p != NULL)
	{
		CdBlockStream::TBlockInfo *q = p;
		p = p->Next;
		delete q;
	}
}

CdBlockStream::TBlockInfo::TBlockInfo()
{
	Next = NULL;
	BlockStart = BlockSize = StreamStart = StreamNext = 0;
	Head = false;
}

SIZE64 CdBlockStream::TBlockInfo::AbsStart()
{
	return StreamStart - (Head ? (HeadSize+2*GDS_POS_SIZE) : (2*GDS_POS_SIZE));
}

void CdBlockStream::TBlockInfo::SetSize(CdStream &Stream, const SIZE64 _Size)
{
	BlockSize = _Size;
	SIZE64 L = Head ? (HeadSize + 2*GDS_POS_SIZE) : (2*GDS_POS_SIZE);
	Stream.SetPosition(StreamStart - L);
	BYTE_LE<CdStream>(Stream) <<
		TdGDSPos((_Size+L) | (Head ? GDS_STREAM_POS_MASK_HEAD_BIT : 0));
}

void CdBlockStream::TBlockInfo::SetNext(CdStream &Stream, const SIZE64 _Next)
{
	StreamNext = _Next;
	Stream.SetPosition(StreamStart -
		(Head ? (HeadSize+GDS_POS_SIZE) : GDS_POS_SIZE));
	BYTE_LE<CdStream>(Stream) << TdGDSPos(_Next);
}

void CdBlockStream::TBlockInfo::SetSize2(CdStream &Stream,
	const SIZE64 _Size, const SIZE64 _Next)
{
	BlockSize = _Size;
	StreamNext = _Next;
	SIZE64 L = Head ? (HeadSize + 2*GDS_POS_SIZE) : (2*GDS_POS_SIZE);
	Stream.SetPosition(StreamStart - L);
	BYTE_LE<CdStream>(Stream)
		<< TdGDSPos((_Size+L) | (Head ? GDS_STREAM_POS_MASK_HEAD_BIT : 0))
		<< TdGDSPos(_Next);
}


CdBlockStream::CdBlockStream(CdBlockCollection &vCollection):
	CdStream(), fCollection(vCollection)
{
	fID = 0;
	fList = fCurrent = NULL;
	fPosition = fBlockCapacity = 0;
	fBlockSize = 0;
	fNeedSyncSize = false;
	if (vCollection.fStream)
	{
		vCollection.fStream->AddRef();
	}
}

CdBlockStream::~CdBlockStream()
{
	SyncSizeInfo();
	xClearList(fList);
	if (fCollection.fStream)
	{
		fCollection.fStream->Release();
	}
}

ssize_t CdBlockStream::Read(void *Buffer, ssize_t Count)
{
	SIZE64 LastPos = fPosition;
	if ((LastPos+Count) > fBlockSize)
		Count = fBlockSize - LastPos;

	if (fCurrent && (Count>0))
	{
		CdStream *vStream = fCollection.Stream();
		if (!vStream) return 0;

		char *p = (char*)Buffer;
		SIZE64 I, L;
		ssize_t RL;
		do {
			I = fPosition - fCurrent->BlockStart;
			L = fCurrent->BlockSize - I;
			if (Count < L)
			{
				vStream->SetPosition(fCurrent->StreamStart + I);
				RL = vStream->Read((void*)p, Count);
				fPosition += RL;
				break;
			} else {
				if (L > 0)
				{
					vStream->SetPosition(fCurrent->StreamStart + I);
					RL = vStream->Read((void*)p, L);
					Count -= RL; fPosition += RL; p += RL;
					if (RL != L) break;
                }
				fCurrent = fCurrent->Next;
				if ((fCurrent==NULL) || (Count<=0))
					break;
			}
		} while (true);
	}

	return fPosition - LastPos;
}

ssize_t CdBlockStream::Write(const void *Buffer, ssize_t Count)
{
	SIZE64 LastPos = fPosition;

	if (Count > 0)
	{
		SIZE64 L = fPosition + Count;
		if (L > fBlockCapacity)
			fCollection._IncStreamSize(*this, L);

		CdStream *vStream = fCollection.Stream();
		if (!vStream) return 0;

		C_UInt8 *p = (C_UInt8*)Buffer;
		ssize_t RL;
		do {
			SIZE64 I = fPosition - fCurrent->BlockStart;
			L = fCurrent->BlockSize - I;
			if (Count < L)
			{
				vStream->SetPosition(fCurrent->StreamStart + I);
				fPosition += vStream->Write(p, Count);
				break;
			} else {
				if (L > 0)
				{
					vStream->SetPosition(fCurrent->StreamStart + I);
					RL = vStream->Write(p, L);
					Count -= RL; fPosition += RL; p += RL;
					if (RL != L) break;
				}
				fCurrent = fCurrent->Next;
				if ((fCurrent==NULL) || (Count<=0))
					break;
			}
		} while (true);

		if (fPosition > fBlockSize)
		{
			fBlockSize = fPosition;
			fNeedSyncSize = true;
			SyncSizeInfo();
		}
	}
	return fPosition - LastPos;
}

SIZE64 CdBlockStream::Seek(const SIZE64 Offset, TdSysSeekOrg Origin)
{
	SIZE64 rv = 0;
	switch (Origin)
	{
		case soBeginning:
			if (fPosition != Offset)
				rv = Offset;
			else
				return Offset;
			break;
		case soCurrent:
			if (Offset != 0)
				rv = fPosition + Offset;
			else
				return fPosition;
			break;
		case soEnd:
			if (Offset != 0)
				rv = fBlockSize + Offset;
			else
				return fBlockSize;
			break;
		default:
			return -1;
	}
	if ((rv < 0) || (rv > fBlockSize))
		throw ErrStream(rsBlockInvalidPos, rv);
	fCurrent = _FindCur(rv);
	return (fPosition = rv);
}

SIZE64 CdBlockStream::GetSize()
{
	return fBlockSize;
}

void CdBlockStream::SetSize(const SIZE64 NewSize)
{
	if ((0<=NewSize) && (NewSize!=fBlockSize))
	{
		if (NewSize > fBlockCapacity)
			fCollection._IncStreamSize(*this, NewSize);
		else if (NewSize < fBlockCapacity)
			fCollection._DecStreamSize(*this, NewSize);
		fBlockSize = NewSize;
		if (fPosition > NewSize)
		{
			fPosition = NewSize;
			fCurrent = _FindCur(fPosition);
		}
		fNeedSyncSize = true;
		SyncSizeInfo();
	}
}

void CdBlockStream::SetSizeOnly(const SIZE64 NewSize)
{
	if ((0<=NewSize) && (NewSize!=fBlockSize))
	{
		if (NewSize > fBlockCapacity)
		{
			SetSize(NewSize);
		} else {
			fBlockSize = NewSize;
			if (fPosition > NewSize)
			{
				fPosition = NewSize;
				fCurrent = _FindCur(fPosition);
			}
			fNeedSyncSize = true;
			SyncSizeInfo();
		}
	}
}

bool CdBlockStream::ReadOnly() const
{
    return fCollection.fReadOnly;
}

int CdBlockStream::ListCount() const
{
	int rv = 0;
	TBlockInfo *p = fList;
	while (p != NULL) { p = p->Next; rv++; }
	return rv;
}

void CdBlockStream::SyncSizeInfo()
{
	if (fNeedSyncSize)
	{
		if (fList)
		{
			CdStream *s = fCollection.Stream();
			s->SetPosition(fList->StreamStart - GDS_POS_SIZE);
			BYTE_LE<CdStream>(*s) << TdGDSPos(fBlockSize);
        }
    	fNeedSyncSize = false;
	}
}

CdBlockStream::TBlockInfo *CdBlockStream::_FindCur(const SIZE64 Pos)
{
	if (Pos < fBlockCapacity)
	{
		TBlockInfo *p = fCurrent;
		if (p == NULL) p = fList;
		if (p == NULL) return NULL;
		if (Pos < p->BlockStart) p = fList;

		TBlockInfo *n = p;
		while ((p != NULL) && (Pos >= p->BlockStart))
		{
			n = p; p = p->Next;
		}
		return n;
	} else
		return NULL;
}

// CdBlockCollection

CdBlockCollection::CdBlockCollection(const SIZE64 vCodeStart)
{
	fStream = NULL;
	fStreamSize = 0;
	fUnuse = NULL;
	vNextID = 1; // start from 1
	fCodeStart = vCodeStart;
	fClassMgr = &dObjManager();
	fReadOnly = false;
}

CdBlockCollection::~CdBlockCollection()
{
	Clear();
}

void CdBlockCollection::_IncStreamSize(CdBlockStream &Block,
	const SIZE64 NewCapacity)
{
	// NewCapacity > fBlockCapacity
	if (Block.fList != NULL)
	{
		CdBlockStream::TBlockInfo *p = Block.fList;
		while (p->Next) p = p->Next;

		SIZE64 L = p->BlockSize + p->StreamStart;

		// to check if it is the last block
		if (L == fStreamSize)
		{
			// immediately increase size of the original stream
			L += NewCapacity - Block.fBlockCapacity;
			fStream->SetSize(L); fStreamSize = L;
			// set size
			p->SetSize(*fStream, NewCapacity - p->BlockStart);
			Block.fBlockCapacity = NewCapacity;
			// check Block.fCurrent
			if (Block.fCurrent == NULL)
			{
				p = Block.fList;
				while (p->Next) p = p->Next;
				Block.fCurrent = p;
			}
		} else if (L < fStreamSize)
		{
			// Need a new block
			CdBlockStream::TBlockInfo *n =
				_NeedBlock(NewCapacity - Block.fBlockCapacity, false);

			n->BlockStart = p->BlockStart + p->BlockSize;
			p->Next = n; n->Next = NULL;
			p->SetNext(*fStream, n->AbsStart());

			Block.fBlockCapacity = n->BlockStart + n->BlockSize;
			if (Block.fCurrent == NULL)
				Block.fCurrent = n;
		} else
			throw ErrStream(rsInvalidBlockLen);

	} else {
		// Need a new block
		CdBlockStream::TBlockInfo *n =
			_NeedBlock(NewCapacity - Block.fBlockCapacity, true);

		n->BlockStart = 0; n->Next = NULL;
		Block.fBlockCapacity = n->BlockSize;
		Block.fList = Block.fCurrent = n;

		fStream->SetPosition(n->StreamStart -
			CdBlockStream::TBlockInfo::HeadSize);
		BYTE_LE<CdStream>(fStream) << Block.fID << TdGDSPos(0);
	}
}

void CdBlockCollection::_DecStreamSize(CdBlockStream &Block,
	const SIZE64 NewSize)
{
	// NewSize < fBlockCapacity
	CdBlockStream::TBlockInfo *p, *q;

	p = Block.fList; q = NULL;
	while ((p!=NULL) && (p->BlockStart < NewSize))
	{
		q = p;
		p = p->Next;
	}

	if (p != NULL)
	{
		if (p == Block.fList)
		{
			// skip the header
			q = p;
			p = p->Next;
		}

		// delete the link
		q->Next = NULL;
		q->SetNext(*fStream, 0);

		// delete the unused parts
		while (p != NULL)
		{
			Block.fBlockCapacity -= p->BlockSize;
			p->SetSize2(*fStream, p->BlockSize, 0);
			q = p;
			p = p->Next;
			// insert into fUnuse
			q->Next = fUnuse;
			fUnuse = q;
		}
	}
}

CdBlockStream::TBlockInfo *CdBlockCollection::_NeedBlock(
	SIZE64 Size, bool Head)
{
	if (Head)
		Size += CdBlockStream::TBlockInfo::HeadSize;

	// First, find a suitable block for Unuse
	CdBlockStream::TBlockInfo *p = fUnuse;
	CdBlockStream::TBlockInfo *rv, *q, *qrv;
	rv = q = qrv = NULL;
	while (p != NULL)
	{
		if (p->BlockSize >= Size)
		{
			if (rv != NULL)
			{
				if (p->BlockSize < rv->BlockSize)
				{
					qrv = q; rv = p;
				}
			} else {
				qrv = q; rv = p;
			}
		}
		q = p; p = p->Next;
	}

	// Secend, no such block
	if (rv == NULL)
	{
		SIZE64 Pos = fStreamSize;
		fStreamSize += 2*GDS_POS_SIZE + Size;
		fStream->SetSize(fStreamSize);

		// Result
		rv = new CdBlockStream::TBlockInfo;
		rv->StreamStart = Pos + 2*GDS_POS_SIZE +
			(Head ? CdBlockStream::TBlockInfo::HeadSize : 0);
		rv->Head = Head;
		rv->SetSize2(*fStream,
			Size - (Head ? CdBlockStream::TBlockInfo::HeadSize : 0), 0);

	} else {
		// Have such block
		rv->Head = Head;
		if (Head)
		{
			rv->BlockSize -= CdBlockStream::TBlockInfo::HeadSize;
			rv->StreamStart += CdBlockStream::TBlockInfo::HeadSize;
		}
		rv->SetSize2(*fStream, rv->BlockSize, 0);

		// Delete it
		if (qrv)
			qrv->Next = rv->Next;
		else
        	fUnuse = rv->Next;

		rv->Next = NULL;
	}

	return rv;
}

CdBlockStream *CdBlockCollection::NewBlockStream()
{
	#ifdef COREARRAY_CODE_DEBUG
	if (!fStream)
		throw ErrStream("CdBlockCollection::fStream = NULL.");
	#endif

	// Need a new ID
	while (HaveID(vNextID)) ++vNextID;

	// New
	CdBlockStream *rv = new CdBlockStream(*this);
	rv->AddRef();
	rv->fID = vNextID; ++vNextID;
	fBlockList.push_back(rv);

	return rv;
}

bool CdBlockCollection::HaveID(TdGDSBlockID id)
{
	vector<CdBlockStream*>::const_iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
		if ((*it)->fID == id)
			return true;
	return false;
}

int CdBlockCollection::NumOfFragment()
{
	int Cnt = 0;

	vector<CdBlockStream*>::const_iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
		Cnt += (*it)->ListCount();

	CdBlockStream::TBlockInfo *p = fUnuse;
	while (p != NULL)
	{
		p = p->Next;
		Cnt ++;
	}

	return Cnt;
}

void CdBlockCollection::LoadStream(CdStream *vStream, bool vReadOnly)
{
	if (fStream)
		throw ErrStream("Call CdBlockCollection::Clear() first.");

	// Assign
	(fStream=vStream)->AddRef();
    fReadOnly = vReadOnly;

	// Start to screen
	CdBlockStream::TBlockInfo *p, *q, *n;

	fStream->SetPosition(fCodeStart);
	fStreamSize = fStream->GetSize();

	p = fUnuse;
	while (fStream->Position() < fStreamSize)
	{
		TdGDSPos sSize, sNext;
		BYTE_LE<CdStream>(fStream) >> sSize >> sNext;
		SIZE64 sPos = fStream->Position() +
			(sSize & GDS_STREAM_POS_MASK) - 2*GDS_POS_SIZE;

		CdBlockStream::TBlockInfo *n = new CdBlockStream::TBlockInfo;
		n->Head = (sSize & GDS_STREAM_POS_MASK_HEAD_BIT) != 0;
		int L = n->Head ? CdBlockStream::TBlockInfo::HeadSize : 0;
		n->BlockSize = (sSize & GDS_STREAM_POS_MASK) - L - 2*GDS_POS_SIZE;
		n->StreamStart = fStream->Position() + L;
		n->StreamNext = sNext;

		if (p) p->Next = n; else fUnuse = n;
		p = n;

		fStream->SetPosition(sPos);
	}

	// Reorganize Block
	while (fUnuse != NULL)
	{
		// Find the Head
		p = fUnuse; q = NULL;
		while (p != NULL) {
			if (p->Head) break;
			q = p; p = p->Next;
        }

		if (p != NULL)
		{
        	// Delete p
			if (q) q->Next = p->Next; else fUnuse = p->Next;
			//
			CdBlockStream *bs = new CdBlockStream(*this);
			bs->AddRef();
			fBlockList.push_back(bs);
			//
			fStream->SetPosition(p->StreamStart -
				CdBlockStream::TBlockInfo::HeadSize);
			BYTE_LE<CdStream>(fStream) >> bs->fID >> bs->fBlockSize;
			bs->fBlockCapacity = p->BlockSize;
			bs->fList = bs->fCurrent = p;
			p->Next = NULL;

			// Find a list
			n = fUnuse; q = NULL;
			while ((n != NULL) && (p->StreamNext != 0))
			{
				if ((p->StreamNext == n->AbsStart()) && (!n->Head))
				{
					if (q) q->Next = n->Next; else fUnuse = n->Next;
					p->Next = n;
					n->BlockStart = p->BlockStart + p->BlockSize;
					bs->fBlockCapacity += n->BlockSize;
					p = n; n = n->Next;
					p->Next = NULL;
				} else {
					q = n; n = n->Next;
                }
			}

			// need checking codes
		} else
        	break;
	}
}

void CdBlockCollection::WriteStream(CdStream *vStream)
{
	if (fStream)
		throw ErrStream("Call CdBlockCollection::Clear() first.");
	// Assign
	(fStream=vStream)->AddRef();
    fReadOnly = false;

	fStream->SetSize(fStreamSize=fCodeStart);
}

void CdBlockCollection::Clear()
{
	vector<CdBlockStream*>::iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
	{
		CdBlockStream *p = *it;
		if (p)
		{
		#ifdef COREARRAY_CODE_DEBUG
			if (p->Release() != 0)
				throw ErrStream("CdBlockStream::Release() should return 0 here.");
		#else
			p->Release();
		#endif
		}
	}
	fBlockList.clear();

	if (fStream)
	{
	#ifdef COREARRAY_CODE_DEBUG
		if (fStream->Release() != 0)
			throw ErrStream("CdBlockStream::Release() should return 0 here.");
	#else
		fStream->Release();
	#endif
		fStream = NULL;
	}

	xClearList(fUnuse);
	fUnuse = NULL;
}

void CdBlockCollection::DeleteBlockStream(TdGDSBlockID id)
{
	vector<CdBlockStream*>::iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
	{
		if ((*it)->fID == id)
		{
			CdBlockStream::TBlockInfo *p, *q;
			p = (*it)->fList; q = NULL;
			while (p != NULL)
			{
				if (p->Head)
				{
					p->BlockSize += CdBlockStream::TBlockInfo::HeadSize;
					p->StreamStart -= CdBlockStream::TBlockInfo::HeadSize;
					p->Head = false;
				}
				p->SetSize2(*fStream, p->BlockSize, 0);
            	q = p; p = p->Next;
            }
			if (q) {
				q->Next = fUnuse;
				fUnuse = (*it)->fList;
				(*it)->fList = NULL;
            }

			(*it)->Release();
			fBlockList.erase(it);
			return;
		}
	}
	throw ErrStream("Invalid block with id: %x", id.Get());
}

CdBlockStream *CdBlockCollection::operator[] (const TdGDSBlockID &id)
{
	vector<CdBlockStream*>::iterator it;
	for (it=fBlockList.begin(); it != fBlockList.end(); it++)
	{
		if ((*it)->fID == id)
			return *it;
	}

	CdBlockStream *rv = new CdBlockStream(*this);
	rv->AddRef();
	rv->fID = id;
	fBlockList.push_back(rv);
	if (vNextID.Get() < id.Get())
		vNextID = id.Get() + 1;

	return rv;
}
