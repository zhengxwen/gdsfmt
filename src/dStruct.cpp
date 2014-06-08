// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStruct.cpp: Data container - array, matrix, etc
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

#include <dStruct.h>
#include <memory>
#include <algorithm>


namespace CoreArray
{
	namespace _Internal_
	{
		template<typename T> struct _Seq
		{
			static void rArray(TIterDataExt &Rec)
			{
				T *p = (T*)Rec.pBuf;
				TdIterator it = Rec.Seq->Iterator(Rec.Index);
				for (ssize_t i=Rec.LastDim; i > 0; i--)
				{
					*p++ = TdIterToVal< T, TdTraits<T>::trVal >::Get(it);
					++it;
				}
				Rec.pBuf = (void*)p;
			}

			static void rArrayEx(TIterDataExt &Rec, const CBOOL *Sel)
			{
				T *p = (T*)Rec.pBuf;
				TdIterator it = Rec.Seq->Iterator(Rec.Index);
				for (ssize_t i=Rec.LastDim; i > 0; i--)
				{
					if (*Sel++)
					{
						*p++ = TdIterToVal< T, TdTraits<T>::trVal >::Get(it);
						++it;
                    }
				}
				Rec.pBuf = (void*)p;
			}

			static void wArray(TIterDataExt &Rec)
			{
				T *p = (T*)Rec.pBuf;
				TdIterator it = Rec.Seq->Iterator(Rec.Index);
				for (ssize_t i=Rec.LastDim; i > 0; i--)
				{
					TdIterToVal< T, TdTraits<T>::trVal >::Set(it, *p++);
					++it;
				}
				Rec.pBuf = (void*)p;
			}
		};
	}
}

using namespace std;
using namespace CoreArray;
using namespace CoreArray::_Internal_;


static const char *rsErrReadCell = "No more string can be read.";
static const char *rsErrFormat = "wFormat: Invalid format.";
static const char *rsErrRData = "rData: Invalid SVType.";
static const char *rsErrWData = "wData: Invalid SVType.";
static const char *rsErrLoadText = "LoadStreamText does not support 3 or higher dimensions.";

static const char *errDimIndex = "Invalid Dimension Index!";
static const char *errSetCount = "Invalid Count Value [l%d].";
static const char *errSetCount2 = "Not allow to change Count in %d dims.";
static const char *errVectorCapacity = "Vector Capacity out of bounds (%ld)";

static const char *errElmSize = "Invalid ElmSize (%d)";
static const char *errCheckRect = "Invalid selection!";
static const char *errDimIndexValue = "Invalid Dimension Len (%d)!";
static const char *errLoadModeError = "Only allow to change load mode in a read-only file.";
static const char *errPackedMode = "Invalid compression method '%s'.";

#define pnCnt		0
#define pnDATA		1
#define pnDimCnt	2
#define pnDim		3

static const char *PropNames[4] =
	{ "CNT", "DATA", "DCNT", "DIM" };


// TdIterator

TdIterator & TdIterator::operator++()
{
	Handler->_Advance(*this);
	return *this;
}

TdIterator & TdIterator::operator--()
{
	Handler->_Previous(*this);
	return *this;
}

TdIterator & TdIterator::operator+= (const ssize_t offset)
{
	Handler->_Offset(*this, offset);
	return *this;
}

TdIterator & TdIterator::operator-= (const ssize_t offset)
{
	Handler->_Offset(*this, -offset);
	return *this;
}

Int64 TdIterator::toInt()
{
	return Handler->_toInt(*this);
}

long double TdIterator::toFloat()
{
	return Handler->_toFloat(*this);
}

UTF16String TdIterator::toStr()
{
	return Handler->_toStr(*this);
}

void TdIterator::IntTo(const Int64 v)
{
	Handler->_IntTo(*this, v);
}

void TdIterator::FloatTo(const long double v)
{
	Handler->_FloatTo(*this, v);
}

void TdIterator::StrTo(const UTF16String &v)
{
	Handler->_StrTo(*this, v);
}

size_t TdIterator::rData(void *OutBuf, size_t Cnt, TSVType OutSV)
{
    return Handler->_IterRData(*this, OutBuf, Cnt, OutSV);
}

size_t TdIterator::wData(const void *InBuf, size_t Cnt, TSVType InSV)
{
    return Handler->_IterWData(*this, InBuf, Cnt, InSV);
}


// CdBaseOp

CdBaseOp::CdBaseOp()
	{ fFilter = NULL; }

CdBaseOp::CdBaseOp(CBufdStream* vFilter)
	{ (fFilter = vFilter)->AddRef(); }

CdBaseOp::~CdBaseOp()
	{ fFilter->Release(); }

void CdBaseOp::Init()
	{ fRow = fCol = fRowCnt = fColCnt = 0; }

void CdBaseOp::SetFilter(CBufdStream *Value)
{
	fFilter->Release();
	fFilter = Value; Value->AddRef();
}

// CdBaseOpRead

CdBaseOpRead::CdBaseOpRead()
{
	fStatus = dsValid;
}

CdBaseOpRead::CdBaseOpRead(CBufdStream* vFilter): CdBaseOp(vFilter)
{
	fStatus = dsValid;
}

CdBaseOpRead::~CdBaseOpRead() { }

void CdBaseOpRead::Init()
{
	fStatus = dsValid;
	CdBaseOp::Init();
}

// TdOpReadText

CdOpReadText::CdOpReadText()
{
	Init(); InitParam();
}

CdOpReadText::CdOpReadText(CBufdStream* vFilter) : CdBaseOpRead(vFilter)
{
	Init(); InitParam();
}

CdOpReadText::CdOpReadText(char const *const FileName): CdBaseOpRead(
	new CBufdStream(new CdFileStream(FileName, CdFileStream::fmOpenRead)))
{
	Init(); InitParam();
}

CdOpReadText::~CdOpReadText() { }

void CdOpReadText::InitParam()
{
	SetDelimit("\t");
	RepDelimit = false;
}

void CdOpReadText::BeginOp(CdContainer* dC)
{
	CdBaseOpRead::BeginOp(dC);
	int ch = fFilter->Peek();
	fStatus = (ch >= 0) ? dsValid : dsEOF;
}

CdBaseOpRead &CdOpReadText::Read(TdIterator &it)
{
	TdPtr64 Pos, Last;
	int Cnt;

	if (ReadItem(Pos, Cnt) && it.Handler)
	{
		Last = fFilter->Position();
		fFilter->SetPosition(Pos);
		it.Handler->_LoadUTF8(it, *fFilter, Cnt);
		fFilter->SetPosition(Last);
	}
	return *this;
}

UTF8String CdOpReadText::rCell()
{
	TdPtr64 Pos, Last;
	int Cnt;

	if (ReadItem(Pos, Cnt))
	{
		UTF8String rv;
		rv.resize(Cnt);
		Last = fFilter->Position();
		fFilter->SetPosition(Pos);
		fFilter->Read((void*)rv.data(), Cnt);
		fFilter->SetPosition(Last);
		return rv;
	} else
    	throw ErrBaseOp(rsErrReadCell);
}

long double CdOpReadText::rFloat()
{
	return StrToFloat(rCell().c_str());
}

Int64 CdOpReadText::rInt()
{
	return StrToInt(rCell().c_str());
}

CdOpReadText &CdOpReadText::SkipCell(int Num)
{
	TdPtr64 Pos;
    int Cnt;
	while (fStatus!=dsEOF && Num>0)
	{
		ReadItem(Pos, Cnt);
		--Num;
	}
	return *this;
}

CdOpReadText &CdOpReadText::NextLine()
{
	TdPtr64 Pos;
	int Cnt;
	do {
		ReadItem(Pos, Cnt);
	} while (fStatus!=dsEOF && fStatus!=dsEndL);
	return *this;
}

UTF8String CdOpReadText::Delimit()
{
	fDelimit[0] = fDelimit[10] = fDelimit[13] = 0;
	UTF8String rv;
	rv.resize(fDelimit.count());
	int j = 0;
	for (int i=0; i < 256; i++)
		if (fDelimit[i])
			rv[j++] = i;
	fDelimit[10] = fDelimit[13] = 1;
	return rv;
}

void CdOpReadText::SetDelimit(const UTF8String &str)
{
	fDelimit[10] = fDelimit[13] = 1;
	for (size_t i=0; i < str.size(); i++)
		fDelimit[str[i]] = 1;
	if (Delimit() == "")
		fDelimit[9] = 1;
}

bool CdOpReadText::ReadItem(TdPtr64 &vPos, int &vCnt)
{
	if (fStatus != dsEOF)
	{
		// read cell
		int ch, Cnt = 0;
		bool flag;
		vPos = fFilter->Position();
		do {
			ch = fFilter->rByteL();
			flag = (ch>=0 && fDelimit[(UInt8)ch]==0);
			if (flag) Cnt++;
		} while (flag);
		vCnt = Cnt;

		// update fCol and fRow
		if (Cnt>0 || ch>=0)
		{
			if (fCol==0 && fRow+1>fRowCnt)
				fRowCnt = fRow+1;
			++fCol;
			if (fCol > fColCnt) fColCnt = fCol;
		}

		// Check Delimit
		if (ch >= 0)
		{
			switch (ch) {
				case 10: // for UNIX format
					fCol = 0; ++fRow;
					fStatus = (fFilter->Peek() >= 0) ? dsEndL : dsEOF;
					break;
				case 13: // for Windows format and Mac format
					fCol = 0; ++fRow;
					ch = fFilter->Peek();
					if (ch >= 0)
					{
						if (ch == 10) { // for Windows format
							fFilter->Position()++;
							fStatus = (fFilter->Peek() >= 0) ? dsEndL : dsEOF;
						} else // otherwise, for Mac format
							fStatus = dsEndL;
					} else
						fStatus = dsEOF;
                    break;
				default: // otherwise
					if (RepDelimit)
					{
						int cp = ch;
						while (cp == ch)
							cp = fFilter->rByteL();
						if (cp >= 0) {
							fFilter->Position()--;
							fStatus = dsTab;
						} else
							fStatus = dsEOF;
					} else
                    	fStatus = dsTab;
			}
		} else
			fStatus = dsEOF;

		return true;
	} else
		return false;
}

// TdOpWriteText

CdOpWriteText::CdOpWriteText()
{
	InitParam();
}

CdOpWriteText::CdOpWriteText(char const *const FileName): CdBaseOpWrite(
	new CBufdStream(new CdFileStream(FileName, CdFileStream::fmCreate)))
{
	InitParam();
}

CdOpWriteText::CdOpWriteText(CBufdStream* vFilter): CdBaseOpWrite(vFilter)
{
    InitParam();
}

void CdOpWriteText::InitParam()
{
	Delimit = "\t";
	Ln = sLineBreak;
}

CdBaseOpWrite &CdOpWriteText::Write(TdIterator &it)
{
	if (it.Handler)
	{
		WriteItem();
		UTF8String s = UTF16toUTF8(it.toStr());
		fFilter->Write((void*)s.c_str(), s.size());
	}
	return *this;
}

CdOpWriteText &CdOpWriteText::wLn()
{
	fFilter->Write((void*)Ln.c_str(), Ln.size());
	fCol = 0;
	if ((++fRow) > fRowCnt) fRowCnt = fRow;
	return *this;
}

CdOpWriteText &CdOpWriteText::wTab()
{
	WriteItem();
	return *this;
}

CdOpWriteText &CdOpWriteText::wCell(char const* const Text)
{
	WriteItem();
	fFilter->Write((void*)Text, strlen(Text));
	return *this;
}

CdOpWriteText &CdOpWriteText::wFormat(char const* const fmt, ...)
{
	int L;
	char buf[1024];
	va_list args; va_start(args, fmt);
	L = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	if (L >= 0)
	{
		WriteItem();
		fFilter->Write((void*)buf, strlen(buf));
		return *this;
	} else
		throw ErrBaseOp(rsErrFormat);
}

CdOpWriteText &CdOpWriteText::wInt(int Value)
{
	char buf[32];
	sprintf(buf, "%d", Value);
	wCell(buf);
	return *this;
}

CdOpWriteText &CdOpWriteText::wInt(int const* Values, ssize_t num)
{
	char buf[32];
	for (; num > 0; num--)
	{
		sprintf(buf, "%d", *Values++);
		wCell(buf);
	}
	return *this;
}

CdOpWriteText &CdOpWriteText::wFloat(const float Value)
{
	char buf[64];
	sprintf(buf, "%.7g", Value);
	wCell(buf);
	return *this;
}

CdOpWriteText &CdOpWriteText::wFloat(const double Value)
{
	char buf[64];
	sprintf(buf, "%.15g", Value);
	wCell(buf);
	return *this;
}

CdOpWriteText &CdOpWriteText::wFloat(float const* Values, ssize_t num)
{
	char buf[64];
	for (; num > 0; num--)
	{
		sprintf(buf, "%.7g", *Values++);
		wCell(buf);
	}
	return *this;
}

CdOpWriteText &CdOpWriteText::wFloat(double const* Values, ssize_t num)
{
	char buf[64];
	for (; num > 0; num--)
	{
		sprintf(buf, "%.15g", *Values++);
		wCell(buf);
	}
	return *this;
}

void CdOpWriteText::WriteItem()
{
	if (fCol > 0)
		fFilter->Write((void*)Delimit.c_str(), Delimit.size());
	if ((++fCol) > fColCnt)
		fColCnt = fCol;
	if (fRow+1 > fRowCnt)
		fRowCnt = fRow + 1;
}

// CdContainer

CdContainer::CdContainer()
{
	fStoreMode = lmKeepInStream;
}

CdContainer::~CdContainer() { }

CdGDSObj *CdContainer::NewOne(void *Param)
{
	throw ErrSequence("Invalid New operation.");
}

void CdContainer::AssignOne(CdGDSObj &Source, bool Append, void *Param)
{
	_RaiseInvalidAssign(string(dName()) + " := " + Source.dName());
}

bool CdContainer::Empty()
{
	return _isEnd(atStart());
}

void CdContainer::LoadFromText(const char *FileName,
	TdDefParamText *Param)
{
	TdAutoRef<CBufdStream> Reader( new CBufdStream(new CdFileStream(
		FileName, CdFileStream::fmOpenRead)) );
	LoadStreamText(*Reader, Param);
}

void CdContainer::SaveToText(const char *FileName,
	TdDefParamText *Param)
{
	TdAutoRef<CBufdStream> Writer( new CBufdStream(new CdFileStream(
		FileName, CdFileStream::fmCreate)) );
	SaveStreamText(*Writer, Param);
}

Int64 CdContainer::Count()
{
	TdIterator i = atStart();
	Int64 rv = 0;
	while (!_isEnd(i))
		{ rv++; _Advance(i); }
	return rv;
}

void CdContainer::SetLoadMode(TdStoreMode Mode)
{
	fStoreMode = Mode;
}

void CdContainer::SetPackedMode(const char *Mode)
{
	throw ErrContainer("Invalid SetPackedMode.");
}

void CdContainer::CloseWriter()
{
    // do nothing ...
}

void CdContainer::_Offset(TdIterator &it, ssize_t I)
{
	if (I > 0)
	{
		for (; I > 0; I--)
		{
			_Advance(it);
			if (_isEnd(it)) break;
		}
	} else {
		for (; I < 0; I++)
		{
			_Previous(it);
			if (_isEnd(it)) break;
		}
	}
}

size_t CdContainer::_IterRData(TdIterator &it, void *OutBuf, size_t Cnt,
	TSVType OutSV)
{
	#define ITER_READ_INT(TYPE) { \
				TYPE *p = (TYPE*)OutBuf; \
				for (; (Cnt > 0) && !_isEnd(it); Cnt--, ++it) \
					*p++ = it.toInt(); \
			}
	#define ITER_READ_FLOAT(TYPE) { \
				TYPE *p = (TYPE*)OutBuf; \
				for (; (Cnt > 0) && !_isEnd(it); Cnt--, ++it) \
					*p++ = it.toFloat(); \
			}
	#define ITER_READ_STR(TYPE) { \
			TYPE *p = (TYPE*)OutBuf; \
			for (; (Cnt > 0) && !_isEnd(it); Cnt--, ++it) \
				*p++ = ValCvt<TYPE, UTF16String>(it.toStr()); \
		}

	size_t OldCnt = Cnt;
	switch (OutSV)
	{
		case svInt8: case svUInt8:
			ITER_READ_INT(Int8); break;
		case svInt16: case svUInt16:
			ITER_READ_INT(Int16); break;
		case svInt32: case svUInt32:
			ITER_READ_INT(Int32); break;
		case svInt64: case svUInt64:
			ITER_READ_INT(Int64); break;
		case svFloat32:
			ITER_READ_FLOAT(Float32); break;
		case svFloat64:
			ITER_READ_FLOAT(Float64); break;
		case svStrUTF8:
			ITER_READ_STR(UTF8String); break;
		case svStrUTF16:
			ITER_READ_STR(UTF16String); break;
		default:
			throw ErrContainer("Invalid SVType.");
	}
	return OldCnt - Cnt;

	#undef ITER_READ_INT
	#undef ITER_READ_FLOAT
	#undef ITER_READ_STR
}

size_t CdContainer::_IterWData(TdIterator &it, const void *InBuf, size_t Cnt,
	TSVType InSV)
{
	#define ITER_WRITE_INT(TYPE) { \
				const TYPE *p = (const TYPE*)InBuf; \
				for (; (Cnt > 0) && !_isEnd(it); Cnt--, ++it) \
					it.IntTo(*p++); \
			}
	#define ITER_WRITE_FLOAT(TYPE) { \
				const TYPE *p = (const TYPE*)InBuf; \
				for (; (Cnt > 0) && !_isEnd(it); Cnt--, ++it) \
					it.FloatTo(*p++); \
			}
	#define ITER_WRITE_STR(TYPE) { \
				const TYPE *p = (const TYPE*)InBuf; \
				for (; (Cnt > 0) && !_isEnd(it); Cnt--, ++it) \
					it.StrTo(ValCvt<UTF16String, TYPE>(*p++)); \
			}

	size_t OldCnt = Cnt;
	switch (InSV)
	{
		case svInt8: case svUInt8:
			ITER_WRITE_INT(Int8); break;
		case svInt16: case svUInt16:
			ITER_WRITE_INT(Int16); break;
		case svInt32: case svUInt32:
			ITER_WRITE_INT(Int32); break;
		case svInt64: case svUInt64:
			ITER_WRITE_INT(Int64); break;
		case svFloat32:
			ITER_WRITE_FLOAT(Float32); break;
		case svFloat64:
			ITER_WRITE_FLOAT(Float64); break;
		case svStrUTF8:
			ITER_WRITE_STR(UTF8String); break;
		case svStrUTF16:
			ITER_WRITE_STR(UTF16String); break;
		default:
			throw ErrContainer("Invalid SVType.");
	}
	return OldCnt - Cnt;

	#undef ITER_WRITE_INT
	#undef ITER_WRITE_FLOAT
	#undef ITER_WRITE_STR
}

CdBaseOpRead* CdContainer::DefOpRead(CBufdStream *Stream)
{
	return new CdOpReadText(Stream);
}

CdBaseOpWrite* CdContainer::DefOpWrite(CBufdStream *Stream)
{
	return new CdOpWriteText(Stream);
}

void CdContainer::_LoadUTF8(TdIterator &it, CBufdStream &Buf, size_t Len)
{
	char xbuffer[256], *p;
	auto_ptr<char> str(NULL);
	if (Len < sizeof(xbuffer))
		p = xbuffer;
	else {
		str.reset(new char[Len+1]);
        p = str.get();
	}
	Buf.Read(p, Len);
	p[Len] = '\x0';
	_StrTo(it, PChartoUTF16(p));
}

// CdSequenceX

CdSequenceX::CdSequenceX() { }

CdSequenceX::~CdSequenceX() { }

void CdSequenceX::AssignOne(CdGDSObj &Source, bool Append, void *Param)
{
	if (dynamic_cast<CdSequenceX*>(&Source))
	{
		CdSequenceX &Seq = *static_cast<CdSequenceX*>(&Source);
		Seq.CloseWriter();
		if (!Append)
			Seq.xAssignToDim(*this);

		Int64 Cnt = Seq.Count();
		size_t BufSize = min(Int64(65536), Cnt);
		TdIterator it = Seq.atStart();

		TSVType sv = SVType();
		if (COREARRAY_SV_SINT(sv))
		{
			auto_ptr<Int64> buf(new Int64[BufSize]);
			while (Cnt > 0)
			{
				size_t L = (Cnt > 65536) ? 65536 : Cnt;
				L = it.rData(buf.get(), L, svInt64);
				if (L == 0) break;
				this->Append(buf.get(), L, svInt64);
				Cnt -= L;
			}
		} else if (COREARRAY_SV_UINT(sv))
		{
			auto_ptr<UInt64> buf(new UInt64[BufSize]);
			while (Cnt > 0)
			{
				size_t L = (Cnt > 65536) ? 65536 : Cnt;
				L = it.rData(buf.get(), L, svUInt64);
				if (L == 0) break;
				this->Append(buf.get(), L, svUInt64);
				Cnt -= L;
			}
		} else if (COREARRAY_SV_FLOAT(sv))
		{
			auto_ptr<Float64> buf(new Float64[BufSize]);
			while (Cnt > 0)
			{
				size_t L = (Cnt > 65536) ? 65536 : Cnt;
				L = it.rData(buf.get(), L, svFloat64);
				if (L == 0) break;
				this->Append(buf.get(), L, svFloat64);
				Cnt -= L;
			}
		} else if (COREARRAY_SV_STRING(sv))
		{
			vector<UTF16String> buf(BufSize);
			while (Cnt > 0)
			{
				size_t L = (Cnt > 65536) ? 65536 : Cnt;
				L = it.rData(&(buf[0]), L, svStrUTF16);
				if (L == 0) break;
				this->Append(&(buf[0]), L, svStrUTF16);
				Cnt -= L;
			}
		}

		if (!Append)
			CloseWriter();
	} else
		CdContainer::AssignOne(Source, Append, Param);
}

void CdSequenceX::rData(Int32 const* Start, Int32 const* Length,
	void *OutBuffer, TSVType OutSV)
{
	#ifdef COREARRAY_DEBUG_CODE
	xCheckRect(Start, Length);
	#endif

	TIterDataExt Rec;
	Rec.pBuf = OutBuffer;
	Rec.Seq = this;
	int vDim = DimCnt();

	if (vDim > 0)
	{
		Rec.LastDim = *(Length + vDim - 1);
	} else {
		Rec.LastDim = 1; Rec.Index = NULL; Start = Length = NULL;
	}
	if (Rec.LastDim > 0)
	{
		switch (OutSV)
		{
			case svInt8:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int8>::rArray);
				break;
			case svUInt8:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt8>::rArray);
				break;
			case svInt16:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int16>::rArray);
				break;
			case svUInt16:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt16>::rArray);
				break;
			case svInt32:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int32>::rArray);
				break;
			case svUInt32:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt32>::rArray);
				break;
			case svInt64:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int64>::rArray);
				break;
			case svUInt64:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt64>::rArray);
				break;
			case svFloat32:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Float32>::rArray);
				break;
			case svFloat64:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Float64>::rArray);
				break;
			case svStrUTF8:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UTF8String>::rArray);
				break;
			case svStrUTF16:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UTF16String>::rArray);
				break;
			default:
				throw ErrSequence(rsErrRData);
		}
	}
}

void CdSequenceX::rDataEx(Int32 const* Start, Int32 const* Length,
	const CBOOL *const Selection[], void *OutBuffer, TSVType OutSV)
{
	#ifdef COREARRAY_DEBUG_CODE
	xCheckRect(Start, Length);
	#endif

	TIterDataExt Rec;
	Rec.pBuf = OutBuffer;
	Rec.Seq = this;
	int vDim = DimCnt();

	if (vDim > 0)
	{
		Rec.LastDim = *(Length + vDim - 1);
	} else {
		Rec.LastDim = 1; Rec.Index = NULL; Start = Length = NULL;
	}
	if (Rec.LastDim > 0)
	{
		switch (OutSV)
		{
			case svInt8:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<Int8>::rArrayEx);
				break;
			case svUInt8:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<UInt8>::rArrayEx);
				break;
			case svInt16:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<Int16>::rArrayEx);
				break;
			case svUInt16:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<UInt16>::rArrayEx);
				break;
			case svInt32:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<Int32>::rArrayEx);
				break;
			case svUInt32:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<UInt32>::rArrayEx);
				break;
			case svInt64:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<Int64>::rArrayEx);
				break;
			case svUInt64:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<UInt64>::rArrayEx);
				break;
			case svFloat32:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<Float32>::rArrayEx);
				break;
			case svFloat64:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<Float64>::rArrayEx);
				break;
			case svStrUTF8:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<UTF8String>::rArrayEx);
				break;
			case svStrUTF16:
				SeqIterRectEx(Start, Length, Selection, vDim, Rec, _Seq<UTF16String>::rArrayEx);
				break;
			default:
				throw ErrSequence(rsErrRData);
		}
	}
}

void CdSequenceX::wData(Int32 const* Start, Int32 const* Length,
	void const* InBuffer, TSVType InSV)
{
	#ifdef COREARRAY_DEBUG_CODE
	xCheckRect(Start, Length);
	#endif

	TIterDataExt Rec;
	Rec.pBuf = (void*)InBuffer;
	Rec.Seq = this;
	int vDim = DimCnt();

	if (vDim > 0)
		Rec.LastDim = *(Length + vDim - 1);
	else {
		Rec.LastDim = 1; Rec.Index = NULL; Start = Length = NULL;
	}

	if (Rec.LastDim > 0)
	{
		switch (InSV) {
			case svInt8:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int8>::wArray);
				break;
			case svUInt8:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt8>::wArray);
				break;
			case svInt16:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int16>::wArray);
				break;
			case svUInt16:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt16>::wArray);
				break;
			case svInt32:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int32>::wArray);
				break;
			case svUInt32:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt32>::wArray);
				break;
			case svInt64:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Int64>::wArray);
				break;
			case svUInt64:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UInt64>::wArray);
				break;
			case svFloat32:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Float32>::wArray);
				break;
			case svFloat64:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<Float64>::wArray);
				break;
			case svStrUTF8:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UTF8String>::wArray);
				break;
			case svStrUTF16:
				SeqIterRect(Start, Length, vDim, Rec, _Seq<UTF16String>::wArray);
				break;
			default:
				throw ErrSequence(rsErrWData);
		}
	}
}

void CdSequenceX::LoadStreamText(CBufdStream &Reader, TdDefParamText *Param)
{
	Clear();

	TdDefParamText Op;
	if (Param == NULL) Param = &Op;

	auto_ptr<CdBaseOpRead> dImport(DefOpRead(&Reader));
	TdAutoObjMsg _msg(this);

	{
		TdAutoBaseOp _op(dImport.get(), this); // call BeginMsg and EndMsg
		TdPtr64 Base = dImport->Filter()->Position();
    	TdIterator it;
    	clock_t Tick;
		Int32 I[2];
		Int64 StreamSize = dImport->Filter()->GetSize() - Base;

		Notify64(mcBeginLoad, StreamSize);
		it = atStart();

		switch (DimCnt())
		{
			case 0:
				dImport->Read(it);
				break;
			case 1:
				Tick = clock(); I[0] = 0;
				while (dImport->Status() != CdBaseOpRead::dsEOF)
				{
					SetDLen(0, ++I[0]);
					dImport->Read(it); _Advance(it);
					if ((clock()-Tick) >= Param->TimeInterval)
					{
						Tick = clock();
						Notify64(mcLoading, dImport->Filter()->Position() - Base);
					}
				}
				break;
			case 2:
				Tick = clock(); I[0] = I[1] = 0;
				while (dImport->Status() != CdBaseOpRead::dsEOF)
				{
					if (I[1] >= GetDLen(1))
					{
						SetDLen(1, I[1] + 1);
						if (!Empty()) it = Iterator(I);
					}
					if (I[0] >= GetDLen(0))
					{
						SetDLen(0, I[0] + 1);
						if (!Empty()) it = Iterator(I);
					}

					// Read Cell
					dImport->Read(it);
					_Advance(it); ++I[1];

					// Notify Event
					if ((clock()-Tick) >= Param->TimeInterval)
					{
						Tick = clock();
						Notify64(mcLoading, dImport->Filter()->Position() - Base);
					}
					if (dImport->Status() == CdBaseOpRead::dsEndL)
					{
						I[1] = 0; ++I[0];
					}
				}
				break;
			default:
				throw ErrSequence(rsErrLoadText);
		}
		Notify64(mcLoaded, StreamSize);
	}
}

void CdSequenceX::SaveStreamText(CBufdStream &Writer, TdDefParamText *Param)
{
	TdDefParamText Op;
	if (Param == NULL) Param = &Op;

	auto_ptr<CdBaseOpWrite> dExport(DefOpWrite(&Writer));
	TdAutoObjMsg _msg(this);

	{
		TdAutoBaseOp _op(dExport.get(), this); // call BeginMsg and EndMsg
    	TdIterator it;
    	clock_t Tick;
		Int64 L, Cur = 0;
        Int64 CntSize = Count();

		Notify64(mcBeginSave, CntSize);
		it = atStart();
		switch (DimCnt()) {
			case 0:
				dExport->Write(it); break;
			case 1:
				Tick = clock();
				while (!_isEnd(it))
				{
					dExport->Write(it).WriteLn();
					_Advance(it); ++Cur;
					if ((clock()-Tick) > Param->TimeInterval)
					{
						Tick = clock(); Notify64(mcSaving, Cur);
                    }
				}
				break;
			default:
				Tick = clock(); L = 1;
				for (int i=1; i < DimCnt(); i++)
					L = L * GetDLen(i);
				for (int i=0; i < GetDLen(0); i++)
				{
					for (Int64 j=0; j < L; j++)
					{
						dExport->Write(it);
						_Advance(it); ++Cur;
						if ((clock()-Tick) > Param->TimeInterval)
						{
							Tick = clock();
							Notify64(mcSaving, Cur);
						}
					}
					dExport->WriteLn();
				}
		}
		Notify64(mcSaved, CntSize);
	}
}

void CdSequenceX::LoadDirect(CdSerial &Reader)
{
	TdIterator I;
	clock_t tm;
	Int64 Cur, Cnt = Count();

	Notify64(mcBeginLoad, Cnt);

	if (fPipeInfo)
		fPipeInfo->PushReadPipe(Reader);

	tm = clock(); Cur = 0; I = atStart();
	while (!_isEnd(I))
	{
		_LoadIter(I, Reader); _Advance(I);
		if ((clock()-tm) > COREARRAY_NOTIFY_TICK)
		{
			tm = clock();
			Int64 D[2] = { Cur, (ssize_t)Reader.Stream() };
			Notify64(mcLoading, D);
		}
		++Cur;
	}
	Notify64(mcLoaded, Cnt);
}

void CdSequenceX::SaveDirect(CdSerial &Writer)
{
	TdIterator I;
	clock_t tm;
	Int64 Cur, Cnt = Count();

	Notify64(mcBeginSave, Cnt);

	if (fPipeInfo)
		fPipeInfo->PushWritePipe(Writer);

	tm = clock(); Cur = 0; I = atStart();
	while (!_isEnd(I))
	{
		_SaveIter(I, Writer); _Advance(I);
		if ((clock()-tm) > COREARRAY_NOTIFY_TICK)
		{
			tm = clock();
			Int64 D[2] = { Cur, (ssize_t)Writer.Stream() };
			Notify64(mcSaving, D);
		}
		++Cur;
	}
	Writer.FlushWrite();
	Notify64(mcSaved, Cnt);
}

void CdSequenceX::xCheckRect(const Int32 Start[], const Int32 Length[]) const
{
	if ((Start==NULL) || (Length==NULL))
		throw ErrSequence(errCheckRect);
	for (int i=0; i < DimCnt(); i++)
	{
		if ((*Start < 0) || (*Length < 0) || ((*Start + *Length)>GetDLen(i)))
			throw ErrSequence(errCheckRect);
		++Start; ++Length;
	}
}

void CdSequenceX::xAssignToDim(CdSequenceX &Dest) const
{
	TSeqDimBuf dims;
	GetDimLen(dims);
	if (DimCnt() > 0) dims[0] = 0;
	Dest.SetDimLen(dims, DimCnt());
}


// fill selection, return true if it is a block
static bool fill_selection(Int32 DimSize, const CBOOL Selection[],
	Int32 &OutStart, Int32 &OutCnt, Int32 &OutCntValid)
{
	if (Selection)
	{
		// find the first
		const CBOOL *s = Selection;
		OutStart = 0;
		for (Int32 D = DimSize; D >= 0; D--)
		{
			if (*s) break;
			s ++; OutStart ++;
		}
		if (OutStart >= DimSize)
		{
			OutStart = 0; OutCnt = 0; OutCntValid = 0;
			return true;
		} 

		// find the last
		s = Selection + DimSize - 1;
		Int32 _st = DimSize - 1;
		for (Int32 D = DimSize; D >= 0; D--)
		{
			if (*s) break;
			s --; _st --;
		}

		// the block size
		OutCnt = _st - OutStart + 1;
		OutCntValid = 0;
		bool rv = true;

		s = &Selection[OutStart];
		for (Int32 i=0; i < OutCnt; i++)
		{
			if (*s)
				OutCntValid ++;
			else
				rv = false;
			s ++;
		}

		return rv;

	} else {
		OutStart = 0;
		OutCnt = DimSize;
		OutCntValid = DimSize;
		return true;
	}
}


void CdSequenceX::GetInfoSelection(const CBOOL *const Selection[],
	Int32 OutStart[], Int32 OutBlockLen[], Int32 OutValidCnt[])
{
	// no need a memory buffer
	if (Selection)
	{
		// with selection
		for (int i=0; i < DimCnt(); i++)
		{
			Int32 S, L, C;
			fill_selection(GetDLen(i), Selection[i], S, L, C);
			if (OutStart) OutStart[i] = S;
			if (OutBlockLen) OutBlockLen[i] = L;
			if (OutValidCnt) OutValidCnt[i] = C;
		}
	} else {
		// no selection, using all data
		for (int i=0; i < DimCnt(); i++)
		{
			if (OutStart) OutStart[i] = 0;
			Int32 L = GetDLen(i);
			if (OutValidCnt) OutValidCnt[i] = L;
			if (OutBlockLen) OutBlockLen[i] = L;
		}
	}
}

void CdSequenceX::GetInfoSelection(const Int32 Start[], const Int32 Length[],
	const CBOOL *const Selection[],
	Int32 OutStart[], Int32 OutBlockLen[], Int32 OutValidCnt[])
{
	// no need a memory buffer
	if (Selection)
	{
		// with selection
		for (int i=0; i < DimCnt(); i++)
		{
			Int32 S, L, C;
			fill_selection(Length[i], Selection[i], S, L, C);
			if (OutStart) OutStart[i] = Start[i]+S;
			if (OutBlockLen) OutBlockLen[i] = L;
			if (OutValidCnt) OutValidCnt[i] = C;
		}
	} else {
		// no selection, using all data
		for (int i=0; i < DimCnt(); i++)
		{
			if (OutStart) OutStart[i] = Start[i];
			if (OutBlockLen) OutBlockLen[i] = Length[i];
			if (OutValidCnt) OutValidCnt[i] = Length[i];
		}
	}
}



// CdVectorX

COREARRAY_INLINE static TdIterator IteratorPtr(CdContainer *Handler, const TdPtr64 ptr)
{
	TdIterator rv;
	rv.Handler = Handler; rv.Ptr = ptr;
	return rv;
}

CdVectorX::CdVectorX(ssize_t vElmSize, size_t vDimCnt, bool vDirectMem)
{
	memset(&fAllocator, 0, sizeof(TdIterator));
    vAllocID = 0;
	vAlloc_Stream = NULL;
	vAlloc_Ptr = vCnt_Ptr = 0;
	fNeedUpdate = false;

	fElmSize = vElmSize;
	if (vElmSize <= 0)
    	throw ErrSequence(errElmSize, vElmSize);
	if (!vDirectMem)
		InitAllocator(fAllocator, true, true);
	else
		InitMemAllocator(fAllocator);

	if (vDimCnt == 0)
	{
    	TdIterator it;
		fAllocator.SetCapacity(fElmSize);
		fCapacityMem = fElmSize; fEndPtr = fElmSize;
		fCount = fCurrentCnt = 1;
		it = IteratorPtr(this, 0);
		_InitIter(it, 1);
	} else {
		fDims.resize(vDimCnt);
		fDims[vDimCnt-1].DimElmSize = fElmSize;
		fDims[vDimCnt-1].DimElmCnt = 1;
		fCount = fCurrentCnt = fCapacityMem = fEndPtr = 0;
	}
}

CdVectorX::~CdVectorX()
{
	CloseWriter();
	if (fGDSStream) Synchronize();
	Clear();
}

bool CdVectorX::Empty()
{
	return fEndPtr <= 0;
}

void CdVectorX::Clear()
{
	if (fPipeInfo == NULL)
	{
		if (!fDims.empty())
			SetDLen(0, 0);
		else {
			TdIterator it;
			it.Handler = this; it.Ptr = 0;
			_DoneIter(it, 1);
			fAllocator.Fill(0, fElmSize, 0);
		}
	}
}

TdIterator CdVectorX::atStart()
{
	TdIterator rv;
	rv.Handler = this; rv.Ptr = 0;
	return rv;
}

TdIterator CdVectorX::atEnd()
{
	TdIterator rv;
	rv.Handler = this; rv.Ptr = fEndPtr;
	return rv;
}

void CdVectorX::SaveStruct(CdSerial &Writer, bool IncludeName)
{
	CdSequenceX::SaveStruct(Writer, IncludeName);

	if ((vAlloc_Stream==NULL) && (fGDSStream!=NULL))
	{
		#ifdef COREARRAY_DEBUG_CODE
		_CheckGDSStream();
		if (vAlloc_Ptr == 0)
        	throw ErrSequence("vAlloc_Ptr should not be ZERO.");
		#endif

		vAlloc_Stream = fGDSStream->Collection().NewBlockStream();
		TdBlockID Entry = vAlloc_Stream->ID();
		Writer.SetPosition(vAlloc_Ptr);
		Writer << Entry;
		Writer.FlushBuffer();

		{
			TdAutoRef<CdSerial> M(new CdSerial(vAlloc_Stream));
			SaveDirect(*M);
			DoneAllocator(fAllocator);
			InitAllocator(fAllocator, fPipeInfo==NULL, true, blBufStream, M.get());
		}
	}
}

void CdVectorX::Synchronize()
{
	CdSequenceX::Synchronize();

	if (fGDSStream && (!fGDSStream->ReadOnly()) && fNeedUpdate)
	{
		fAllocator.Filter->FlushWrite();
		UpdateInfo(NULL);
	}
}

void CdVectorX::CloseWriter()
{
	if (fAllocator.Level == blBufStream)
	{
		fAllocator.Filter->OnFlush.Clear();
		fAllocator.Filter->FlushWrite();
		if (fPipeInfo)
		{
			if (fPipeInfo->WriteMode(*fAllocator.Filter))
			{
            	fPipeInfo->ClosePipe(*fAllocator.Filter);
				fNeedUpdate = true;
				UpdateInfo(NULL);
				vAlloc_Stream->AddRef();
				DoneAllocator(fAllocator);
				vAlloc_Stream->SetPosition(0);
				CdSerial * Reader = new CdSerial(vAlloc_Stream);
				vAlloc_Stream->Release();
				if (fPipeInfo)
					fPipeInfo->PushReadPipe(*Reader);
				InitAllocator(fAllocator, true, false, blBufStream, Reader);
            }
		} else {
			fNeedUpdate = true;
			Synchronize();
		}
	}
}

void CdVectorX::AddDim(Int32 NewDimLen)
{
	TdDimItem D;
	if (!fDims.empty())
	{
		TdDimItem & r = fDims.front();
		D.DimElmSize = r.DimLen * r.DimElmSize;
		D.DimElmCnt = r.DimLen * r.DimElmCnt;
		D.DimLen = (D.DimElmSize>0) ? 1 : 0;
	} else {
		D.DimLen = 1; D.DimElmSize = fElmSize; D.DimElmCnt = 1;
		fCurrentCnt = 1;
	}
	fDims.insert(fDims.begin(), D);

	if (NewDimLen >= 0) SetDLen(0, NewDimLen);
	Notify(mcDimChanged);
	fChanged = true;

	if (fGDSStream) SaveToBlockStream();
}

void CdVectorX::DeleteDim()
{
	size_t n = fDims.size();
	if (n > 0)
	{
		TdIterator it;
		it.Handler = this;

		if (n > 1)
		{
			vector<TdDimItem>::iterator p = fDims.begin() + 1;
			it.Ptr = p->DimElmCnt * fElmSize;
			xDoneIter(it, fCurrentCnt - p->DimElmCnt);
			fCurrentCnt = fCount = p->DimElmCnt;
			fEndPtr = fCount * fElmSize;
			fDims.erase(fDims.begin());
		} else {
			it.Ptr = 0;
			xDoneIter(it, fCurrentCnt);
            fDims.clear();
			NeedMemory(fElmSize);
			xInitIter(it, 1);
			fCurrentCnt = fCount = 1;
			fEndPtr = fElmSize;
		}

		Notify(mcDimChanged);
		fChanged = true;
		if (fGDSStream) SaveToBlockStream();
	}
}

void CdVectorX::ClearDim()
{
	if (!fDims.empty())
	{
		Clear();
		fDims.clear();
	}
}

void CdVectorX::GetDimLen(Int32 *Dims) const
{
	vector<TdDimItem>::const_iterator it;
	for (it=fDims.begin(); it != fDims.end(); it++)
		*Dims++ = it->DimLen;
}

void CdVectorX::SetDimLen(const Int32 *Lens, size_t LenCnt)
{
	if (LenCnt > MAX_SEQ_DIM)
		throw ErrSequence("Invalid number of dimentions (%d).", LenCnt);

	bool UpdateFlag = (LenCnt != fDims.size());

	fDims.clear();
	fDims.resize(LenCnt);
	for (size_t i=0; i < LenCnt; i++)
		fDims[i].DimLen = Lens[i];

	TdIterator it;
	it.Handler = this;

	if (LenCnt > 0)
	{
    	Int64 OldCnt = fCurrentCnt;
		TdDimItem &r = fDims.back();
		r.DimElmSize = fElmSize;
		r.DimElmCnt = 1;
		this->xDimAuto(LenCnt-1);
		if (OldCnt > fCurrentCnt)
		{
			it.Ptr = fCurrentCnt * fElmSize;
			xDoneIter(it, OldCnt - fCurrentCnt);
		} else if (OldCnt < fCurrentCnt)
		{
			NeedMemory(fCurrentCnt * fElmSize);
			it.Ptr = OldCnt * fElmSize;
			xInitIter(it, fCurrentCnt - OldCnt);
		}
	} else {
		it.Ptr = 0;
		xDoneIter(it, fCurrentCnt);
		NeedMemory(fElmSize);
		xInitIter(it, 1);
		fCurrentCnt = fCount = 1;
		fEndPtr = fElmSize;
	}

	Notify(mcDimChanged);
	if (UpdateFlag)
	{
		fChanged = true;
		if (fGDSStream) SaveToBlockStream();
	} else
		fNeedUpdate = true;
}

Int32 CdVectorX::GetDLen(int DimIndex) const
{
	return fDims.at(DimIndex).DimLen;
}

void CdVectorX::SetDLen(int DimIndex, Int32 Value)
{
	#ifdef COREARRAY_DEBUG_CODE
	fDims.at(DimIndex);
	if (Value < 0)
		throw ErrSequence(errDimIndexValue, Value);
	#endif

	TdIterator it;
	Int64 MDimOld, MDimNew, LStep, DCnt, DResid;
	TdPtr64 pS, pD;
	TdDimItem &pDim = fDims[DimIndex];

	if (pDim.DimLen != Value)
	{
		if (fCurrentCnt > fCount)
		{
			it.Handler = this; it.Ptr = fCount*fElmSize;
			xDoneIter(it, fCurrentCnt-fCount);
		}

		if (pDim.DimElmSize > 0)
		{
			DCnt = 1;
			for (int i=DimIndex-1; i >= 0; i--)
				DCnt *= fDims[i].DimLen;
			if (DCnt > 0)
			{
				MDimOld = pDim.DimLen * pDim.DimElmSize;
				MDimNew = Value * pDim.DimElmSize;
				if (pDim.DimLen < Value)
				{
					NeedMemory(DCnt * MDimNew);
					DResid = (Value - pDim.DimLen) * pDim.DimElmCnt;
					pS = (DCnt-1)*MDimOld; pD = (DCnt-1)*MDimNew;
					it.Handler = this;
					while (DCnt > 0)
					{
						fAllocator.Move(pS, pD, MDimOld);
						it.Ptr = pD + MDimOld;
						xInitIter(it, DResid);
						pS -= MDimOld; pD -= MDimNew;
						--DCnt;
					}
				} else {
					LStep = MDimOld - MDimNew;
					DResid = (pDim.DimLen - Value) * pDim.DimElmCnt;
					it.Handler = this; it.Ptr = 0;
					pD = pS = 0;
					while (DCnt > 0)
					{
						it.Ptr += MDimNew;
						xDoneIter(it, DResid);
						it.Ptr += LStep;
						fAllocator.Move(pS, pD, MDimNew);
						pS += MDimOld; pD += MDimNew;
						--DCnt;
                    }
				}
			}
		}
		pDim.DimLen = Value;
		xDimAuto(DimIndex);
		Notify32(mcDimLength, DimIndex);
	}
}

TdPtr64 CdVectorX::IndexPtr(Int32 const* DimIndex)
{
	if (!fDims.empty()) {
		vector<TdDimItem>::iterator it;
		TdPtr64 rv = 0;
		for (it=fDims.begin(); it != fDims.end(); it++, DimIndex++)
			rv += (*DimIndex) * it->DimElmSize;
		return rv;
	} else
		return 0;
}

TdIterator CdVectorX::Iterator(Int32 const* DimIndex)
{
	TdIterator it;
	it.Handler = this;
	it.Ptr = IndexPtr(DimIndex);
	return it;
}

void CdVectorX::SetLoadMode(TdStoreMode Mode)
{
	if (fGDSStream != NULL)
	{
		if (fGDSStream->ReadOnly())
		{
			if (fStoreMode != Mode)
			{
				if (vAlloc_Stream)
					vAlloc_Stream = fGDSStream->Collection()[vAllocID];
				vAlloc_Stream->SetPosition(0);

				CdSerial *Reader = NULL;
				Reader = new CdSerial(vAlloc_Stream);
				if (fStoreMode == lmKeepInMem)
				{
					if (fPipeInfo)
						fPipeInfo->PushReadPipe(*Reader);
					DoneAllocator(fAllocator);
					InitAllocator(fAllocator, true, false, blBufStream, Reader);
					fAllocator.Capacity = Reader->GetSize();
				} else {
					DoneAllocator(fAllocator);
					fCapacityMem = AllocNeed(true);
					InitMemAllocator(fAllocator, fCapacityMem);
					try {
						Reader->AddRef();
						vAlloc_Stream->AddRef();
						try {
							LoadDirect(*Reader);
                        } catch (...) {
							Reader->Release();
							DirectDecRef(*vAlloc_Stream);
                        	throw;
						}
						Reader->Release();
						DirectDecRef(*vAlloc_Stream);
					} catch (...) {
						if (fPipeInfo)
							fPipeInfo->PushReadPipe(*Reader);
						DoneAllocator(fAllocator);
						InitAllocator(fAllocator, true, false, blBufStream, Reader);
						fAllocator.Capacity = Reader->GetSize();
						throw;
					}
				}
				fStoreMode = Mode;
			}
		} else
			throw ErrSequence(errLoadModeError);
	} else
		fStoreMode = Mode;
}

void CdVectorX::SetPackedMode(const char *Mode)
{
	if (fPipeInfo ? (!fPipeInfo->Equal(Mode)) : true)
	{
		if (fCount>0 && (vAlloc_Stream!=NULL) && (fGDSStream!=NULL))
		{
			Synchronize();

			if (fPipeInfo) delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (*Mode!=0))
				throw ErrSequence(errPackedMode, Mode);

			{
				TdAutoRef<CdTempStream> Temp(new CdTempStream(""));
				{
					TdAutoRef<CdSerial> Filter(new CdSerial(Temp.get()));
					SaveDirect(*Filter);
					if (fPipeInfo)
						_GetStreamPipeInfo(Filter.get(), true);
				}
				vAlloc_Stream->SetPosition(0);
				vAlloc_Stream->SetSizeOnly(0);
				vAlloc_Stream->CopyFrom(*Temp);
				vAlloc_Stream->SetPosition(0);
			}
			// New Filter
			{
				DoneAllocator(fAllocator);
				CdSerial *Filter = new CdSerial(vAlloc_Stream);
				if (fPipeInfo)
					fPipeInfo->PushReadPipe(*Filter);
				InitAllocator(fAllocator, true, fPipeInfo==NULL, blBufStream, Filter);
			}
			// Save self
			SaveToBlockStream();
		} else {
			if (fPipeInfo)
				delete fPipeInfo;
			fPipeInfo = dStreamPipeMgr.Match(*this, Mode);
			if ((fPipeInfo==NULL) && (*Mode!=0))
				throw ErrSequence(errPackedMode, Mode);
		}
	}
}

void CdVectorX::rData(Int32 const* Start, Int32 const* Length,
	void *OutBuffer, TSVType OutSV)
{
	xSetSmallBuf();
	CdSequenceX::rData(Start, Length, OutBuffer, OutSV);
}

void CdVectorX::Append(void const* Buffer, ssize_t Cnt, TSVType InSV)
{
	if (Cnt <= 0) return;
	#ifdef COREARRAY_DEBUG_CODE
	if (InSV != svCustom)
    	throw ErrSequence("InSV should be svCustom for 'CdVectorX::Append'.");
	#endif

	if (!fDims.empty())
	{
		fNeedUpdate = true;
		if (fAllocator.MemLevel())
			NeedMemory((fCurrentCnt+Cnt) * fElmSize);
		else if (fGDSStream)
		{
			xSetLargeBuf();
			fAllocator.Filter->OnFlush.Set(this, &CdVectorX::UpdateInfo);
		}
		fAllocator.Write(fCurrentCnt*fElmSize, Buffer, Cnt*fElmSize);

    	TdDimItem &R = fDims.front();
		fCurrentCnt += Cnt;
		if (fCurrentCnt >= fCount + R.DimElmCnt)
		{
			Cnt = (fCurrentCnt - fCount) / R.DimElmCnt;
			R.DimLen += Cnt;
			fCount += Cnt * R.DimElmCnt;
			fEndPtr = fCount * fElmSize;
			Notify32(mcDimLength, (Int32)0);
		}
		fNeedUpdate = true;
	} else {
        fAllocator.Write(0, Buffer, fElmSize);
		Notify(mcRefresh);
	}
}

void CdVectorX::CheckRange(Int32 const* DimI)
{
	vector<TdDimItem>::iterator it;
	for (it=fDims.begin(); it != fDims.end(); it++, DimI++) {
		if (*DimI < 0 || *DimI > it->DimLen)
			throw ErrSequence(errDimIndex);
	}
}

Int64 CdVectorX::Count()
{
	return fCount;
}

void CdVectorX::SetCount(const Int64 Value)
{
	if (fDims.size() == 1)
	{
		if ((Value < 0) || (Value > INT32_MAX))
			throw ErrSequence(errSetCount, Value);
		SetDLen(0, Value);
	} else
		throw ErrSequence(errSetCount2, fDims.size());
}

void CdVectorX::SetCapacityMem(const TdPtr64 NewMem)
{
	#ifdef COREARRAY_DEBUG_CODE
	if (NewMem < fEndPtr)
		throw ErrSequence(errVectorCapacity, NewMem);
	#endif
	xSetCapacity(NewMem);
}

bool CdVectorX::_isStart(const TdIterator &it)
{
	return it.Ptr==0;
}

bool CdVectorX::_isEnd(const TdIterator &it)
{
	return it.Ptr >= fEndPtr;
}

bool CdVectorX::_isEqual(TdIterator &it1, TdIterator &it2)
{
	return it1.Ptr == it2.Ptr;
}

void CdVectorX::_Advance(TdIterator &it)
{
	it.Ptr += fElmSize;
}

void CdVectorX::_Previous(TdIterator &it)
{
	it.Ptr -= fElmSize;
}

void CdVectorX::_Offset(TdIterator &it, ssize_t I)
{
	it.Ptr += fElmSize * (Int64)I;
}

void CdVectorX::_LoadIter(TdIterator &it, CdSerial &Reader)
{
	// do nothing ...
}

void CdVectorX::_SaveIter(TdIterator &it, CdSerial &Writer) {
	// do nothing ...
}

void CdVectorX::_InitIter(TdIterator &it, ssize_t Len)
{
	fAllocator.Fill(it.Ptr, fElmSize * (Int64)Len, 0);
}

void CdVectorX::_DoneIter(TdIterator &it, ssize_t Len) {
	// do nothing ...
}

void CdVectorX::_Swap(TdIterator &it1, TdIterator &it2)
{
	fAllocator.Swap(it1.Ptr, it2.Ptr, fElmSize);
}

void CdVectorX::KeepInStream(CdSerial &Reader, void * Data)
{
	if (fGDSStream)
	{
		#ifdef COREARRAY_DEBUG_CODE
		if (!(Reader[PropNames[pnDATA]] >> vAllocID))
			throw ErrSequence("No 'DATA' field!");
		#else
		Reader[PropNames[pnDATA]] >> vAllocID;
		#endif
		vAlloc_Ptr = Reader.NamePosition(PropNames[pnDATA]);
		vAlloc_Stream = fGDSStream->Collection()[vAllocID];

		CdSerial *Reader = new CdSerial(vAlloc_Stream);
		if (fPipeInfo)
			fPipeInfo->PushReadPipe(*Reader);
		DoneAllocator(fAllocator);
		InitAllocator(fAllocator, true, fPipeInfo==NULL, blBufStream, Reader);
		fAllocator.Capacity = Reader->GetSize();
		fCapacityMem = (fAllocator.Capacity>0) ? fAllocator.Capacity : 0;
	} else {
		Reader[PropNames[pnDATA]].rBuffer();
		try {
			if (fPipeInfo)
				fPipeInfo->PushReadPipe(Reader);
			bool flag = (fPipeInfo == NULL);
			DoneAllocator(fAllocator);
			InitAllocatorEx(fAllocator, true, flag, Reader.Stream());
			fAllocator.Capacity = Reader.GetSize();
			fCapacityMem =  (fAllocator.Capacity>0) ? fAllocator.Capacity : 0;
			if (fPipeInfo)
				fPipeInfo->PopPipe(Reader);
		} catch (...) {
			Reader.rEndStruct();
			throw;
		}
		Reader.rEndStruct();
	}

	Int32 * p = (Int32*)Data;
	vector<TdDimItem>::iterator it;
	for (it=fDims.begin(); it != fDims.end(); it++, p++)
		it->DimLen = *p;
	int i = fDims.size();
	if (i > 0) {
		--i;
		fDims[i].DimElmSize = fElmSize;
		fDims[i].DimElmCnt = 1;
		xDimAuto(i);
		Notify(mcReset);
	}
}

TdPtr64 CdVectorX::AllocNeed(bool Full)
{
	return fEndPtr;
}

void CdVectorX::LoadBefore(CdSerial &Reader, TdVersion Version)
{
	CdSequenceX::LoadBefore(Reader, Version);

	Clear();

	TdIterator ip;
	TSeqDimBuf DimBuf;
	memset((void*)DimBuf, 0, sizeof(DimBuf));

	UInt16 DCnt = 0;

	if (Reader[PropNames[pnDimCnt]] >> DCnt)
	{
		fDims.resize(DCnt);
		if (DCnt <= 60)
			Reader[PropNames[pnDim]].rShortBuf(DimBuf, DCnt);
		else
			Reader[PropNames[pnDim]].rBuf(DimBuf, DCnt);

		if (fGDSStream ? fGDSStream->ReadOnly() : true)
			vCnt_Ptr = 0;
		else if (DCnt > 0)
			vCnt_Ptr = Reader.Position();
		else
			vCnt_Ptr = 0;
	} else {
		fDims.clear();
		vCnt_Ptr = 0;
	}

	switch (fStoreMode)
	{
		case lmKeepInMem:
			if (!fDims.empty())
			{
				Int32 * p = DimBuf;
				vector<TdDimItem>::iterator it;
				for (it=fDims.begin(); it != fDims.end(); it++, p++)
					it->DimLen = *p;
				DCnt = fDims.size()-1;
				fDims[DCnt].DimElmSize = fElmSize;
				fDims[DCnt].DimElmCnt = 1;
				xDimAuto(DCnt);
				NeedMemory(AllocNeed(true));
				ip.Handler = this; ip.Ptr = 0;
				xInitIter(ip, fCount);
			} else {
				NeedMemory(fElmSize);
				ip.Handler = this; ip.Ptr = 0;
				_InitIter(ip, 1);
				fEndPtr = fElmSize; fCount = fCurrentCnt = 1;
			}
			break;
		case lmKeepInStream:
			KeepInStream(Reader, (void*)DimBuf);
	}

	fChanged = fNeedUpdate = false;
}

void CdVectorX::LoadAfter(CdSerial &Reader, TdVersion Version)
{
	if (fStoreMode==lmKeepInMem && !Empty())
	{
		if (fGDSStream != NULL)
		{
			vAllocID = 0;
			Reader[PropNames[pnDATA]] >> vAllocID;
			vAlloc_Ptr = Reader.NamePosition(PropNames[pnDATA]);

			TdAutoRef<CdSerial> rd(new CdSerial(fGDSStream->Collection()[vAllocID]));
			rd->Stream()->AddRef();
			try {
				LoadDirect(*rd);
			} catch (exception &E) {
				Reader.Log().Add(E.what());
			}
		} else {
			Reader[PropNames[pnDATA]].rBuffer();
			try {
				try {
					LoadDirect(Reader);
				} catch (exception &E) {
					Reader.Log().Add(E.what());
				}
				if (fPipeInfo)
					fPipeInfo->PopPipe(Reader);
			} catch (...) {
				Reader.rEndStruct();
				throw;
			}
			Reader.rEndStruct();
		}
	}

	CdContainer::LoadAfter(Reader, Version);
}

void CdVectorX::SaveBefore(CdSerial &Writer)
{
	CdSequenceX::SaveBefore(Writer);

	if (!fDims.empty())
	{
		TSeqDimBuf DBuf;
		UInt16 D = fDims.size();
		Writer[PropNames[pnDimCnt]] << D;
		vector<TdDimItem>::iterator it;
		Int32 *p = &DBuf[0];
		for (it=fDims.begin(); it != fDims.end(); it++, p++)
			*p = it->DimLen;
		if (D <= 60)
		{
			Writer[PropNames[pnDim]].wShortBuf(DBuf, D);
			vCnt_Ptr = Writer.Position() - D*sizeof(Int32);
		} else {
			Writer[PropNames[pnDim]].wBuf(DBuf, D);
			vCnt_Ptr = Writer.NamePosition(PropNames[pnDim]) + TdPosType::size;
		}
	}
}

void CdVectorX::SaveAfter(CdSerial &Writer)
{
	if ((fGDSStream!=NULL) || !Empty())
	{
		TdBlockID Entry = 0;
		if (vAlloc_Stream)
			Entry = vAlloc_Stream->ID();
		Writer[PropNames[pnDATA]] << Entry;
		vAlloc_Ptr = Writer.Position() - TdBlockID::size;
	} else {
		if (!Empty())
		{
			Writer[PropNames[pnDATA]].wBuffer();
			SaveDirect(Writer);
			if (fPipeInfo)
				fPipeInfo->PopPipe(Writer);
			Writer.wEndStruct();
		}
	}

	if (fGDSStream)
		fStoreMode = lmKeepInStream;

	CdContainer::SaveAfter(Writer);
}

void CdVectorX::GetPipeInfo()
{
	CBufdStream *buf = (fAllocator.Level==blBufStream) ? fAllocator.Filter : NULL;
	if (_GetStreamPipeInfo(buf, false))
		fNeedUpdate = true;
}

void CdVectorX::LoadDirect(CdSerial &Reader)
{
	if (DirectStream())
	{
		char Buf[COREARRAY_STREAM_BUFFER];
		TdPtr64 Total, Cur;
		clock_t tm;

		Total = AllocNeed(true); // Total size
		Notify64(mcBeginLoad, fCount);
		if (fPipeInfo)
			fPipeInfo->PushReadPipe(Reader);
		Cur = 0; tm = clock();
		while (Total > 0)
		{
			ssize_t L = (Total >= (TdPtr64)sizeof(Buf)) ? (ssize_t)sizeof(Buf) : Total;
			Reader.Read((void*)Buf, L);
			fAllocator.Write(Cur, (void*)Buf, L);
			Cur += L;
			if ((clock()-tm) > COREARRAY_NOTIFY_TICK)
			{
				tm = clock();
				Int64 D[2] = { Cur, (ssize_t)Reader.Stream() };
				Notify64(mcLoading, D);
            }
			Total -= L;
		}
		Notify64(mcLoaded, fCount);
	} else
		CdSequenceX::LoadDirect(Reader);
}

void CdVectorX::SaveDirect(CdSerial &Writer)
{
	if (DirectStream())
	{
		char Buf[COREARRAY_STREAM_BUFFER];
		TdPtr64 Total, Cur;
		clock_t tm;

		Total = AllocNeed(false); // Total size
		Notify64(mcBeginSave, Total);
		if (fPipeInfo)
			fPipeInfo->PushWritePipe(Writer);
		Cur = 0; tm = clock();
		while (Total > 0)
		{
			ssize_t L = (Total >= (TdPtr64)sizeof(Buf)) ? (ssize_t)sizeof(Buf) : Total;
			fAllocator.Read(Cur, Buf, L);
			Writer.Write((void*)Buf, L);
			Cur += L;
			if ((clock()-tm) > COREARRAY_NOTIFY_TICK)
			{
				tm = clock();
				Int64 D[2] = { Cur, (ssize_t)Writer.Stream() };
				Notify64(mcSaving, D);
            }
			Total -= L;
		}
		Writer.FlushWrite();
		Notify64(mcSaved, Total);
	} else
		CdSequenceX::SaveDirect(Writer);
}

void CdVectorX::NeedMemory(const TdPtr64 NewMem)
{
	if (NewMem > fCapacityMem)
	{
		Int64 DeltaMem = NewMem-fCapacityMem, Delta;
		if (fCapacityMem > 64)
		{
			if (fAllocator.MemLevel())
			{
				Delta = fAllocator.Capacity >> 2;
				if (Delta > MediumBlock) Delta = MediumBlock;
			} else
				Delta = DeltaMem;
		} else Delta = 16;

		if (Delta < DeltaMem) Delta= DeltaMem;

		xSetCapacity(fCapacityMem + Delta);
	}
}

void CdVectorX::UpdateInfo(CBufdStream *Sender)
{
	if (fNeedUpdate)
	{
		if (vCnt_Ptr != 0)
		{
			TSeqDimBuf DBuf;
			Int32 *p = &DBuf[0];
			vector<TdDimItem>::iterator it;
			for (it=fDims.begin(); it != fDims.end(); it++, p++)
				*p = it->DimLen;
			fGDSStream->SetPosition(vCnt_Ptr);
			fGDSStream->WriteBuffer((void*)DBuf, sizeof(Int32)*fDims.size());
		}
		if (fPipeInfo)
		{
			if (_GetStreamPipeInfo(fAllocator.Filter, false))
				_UpdateStreamPipeInfo(*fGDSStream);
        }

		UpdateInfoProc(Sender);
		fNeedUpdate = false;
	}

	fAllocator.Filter->OnFlush.Clear();
}

void CdVectorX::SetElmSize(ssize_t NewSize)
{
	if ((NewSize>0) && (fElmSize!=NewSize))
	{
		if (fCurrentCnt > 0)
		{
			TdPtr64 pS, pD;
			if (NewSize > fElmSize)
			{
				pS = fElmSize * fCurrentCnt;
				NeedMemory(pD = NewSize * fCurrentCnt);
				ssize_t Lx = NewSize - fElmSize;
				for (Int64 i=fCurrentCnt; i > 0; i--)
				{
					pS -= fElmSize; pD -= NewSize;
					fAllocator.Move(pS, pD, fElmSize);
					fAllocator.Fill(pD+fElmSize, Lx, 0);
				}
			} else {
				pS = pD = 0;
				for (Int64 i=fCurrentCnt; i > 0; i--)
				{
					pS += fElmSize; pD += NewSize;
					fAllocator.Move(pS, pD, NewSize);
				}
			}
		}
		fElmSize = NewSize;

		if (!fDims.empty())
		{
			int DimIndex = fDims.size();
			vector<TdDimItem>::iterator it = fDims.begin() + (DimIndex-1);
			Int64 LSize = it->DimElmSize = fElmSize;
			Int64 LCnt = it->DimElmCnt = 1;

			for (; DimIndex > 1; DimIndex--)
			{
				LSize = LSize * it->DimLen;
				LCnt = LCnt * it->DimLen;
				it--;
				it->DimElmSize = LSize;
				it->DimElmCnt = LCnt;
			}
			fEndPtr = it->DimLen * LSize;
		} else
        	fEndPtr = fElmSize;

		fNeedUpdate = true;
		Notify(mcElmSize);
	}
}

void CdVectorX::xDimAuto(int DimIndex)
{
	Int64 LSize, LCnt;
	vector<TdDimItem>::iterator it = fDims.begin() + DimIndex;

	LSize = it->DimElmSize;
	LCnt = it->DimElmCnt;
	for (; DimIndex >= 1; DimIndex--)
	{
		LSize = LSize * it->DimLen;
		LCnt = LCnt * it->DimLen;
		it--;
		it->DimElmSize = LSize;
		it->DimElmCnt = LCnt;
	}
	fEndPtr = it->DimLen * LSize;
	fCount = fCurrentCnt = it->DimLen * LCnt;
	fNeedUpdate = true;
}

void CdVectorX::xInitIter(TdIterator &it, Int64 Len)
{
	#ifdef SIZE_MAX
	#  if (SIZE_MAX > UINT32_MAX)
		_InitIter(it, Len);
	#  else
		while (Len > 0)
		{
			ssize_t L = (Len > INT32_MAX) ? INT32_MAX : Len;
			_InitIter(it, L);
			Len -= L;
			if (Len > 0) it.Ptr += fElmSize*Int64(L);
		}
	#  endif
	#else
		while (Len > 0)
		{
			ssize_t L = (Len > INT32_MAX) ? INT32_MAX : Len;
			_InitIter(it, L);
			Len -= L;
			if (Len > 0) it.Ptr += fElmSize*Int64(L);
		}
	#endif
}

void CdVectorX::xDoneIter(TdIterator &it, Int64 Len)
{
	#ifdef SIZE_MAX
	#  if (SIZE_MAX > UINT32_MAX)
		_DoneIter(it, Len);
	#  else
		while (Len > 0)
		{
			ssize_t L = (Len > INT32_MAX) ? INT32_MAX : Len;
			_DoneIter(it, L);
			Len -= L;
			if (Len > 0) it.Ptr += fElmSize*Int64(L);
		}
	#  endif
	#else
		while (Len > 0)
		{
			ssize_t L = (Len > INT32_MAX) ? INT32_MAX : Len;
			_DoneIter(it, L);
			Len -= L;
			if (Len > 0) it.Ptr += fElmSize*Int64(L);
		}
	#endif
}

void CdVectorX::xCheckRect(Int32 const* Start, Int32 const* Length)
{
	vector<TdDimItem>::iterator it;
	for (it=fDims.begin(); it != fDims.end(); it++)
	{
		if (*Start < 0 || *Length < 0 || *Start + *Length > it->DimLen)
			throw ErrSequence(errCheckRect);
		++Start; ++Length;
	}
}

void CdVectorX::xSetSmallBuf()
{
	if (!fAllocator.MemLevel())
	{
		if (fAllocator.Filter->BufSize() != DefaultBufSmallSize)
			fAllocator.Filter->SetBufSize(DefaultBufSmallSize);
	}
}

void CdVectorX::xSetLargeBuf()
{
	if (!fAllocator.MemLevel())
	{
		if (fAllocator.Filter->BufSize() != DefaultBufLargeSize)
			fAllocator.Filter->SetBufSize(DefaultBufLargeSize);
    }
}

void CdVectorX::xSetCapacity(const TdPtr64 NewMem)
{
	if (NewMem != fCapacityMem)
	{
		fAllocator.SetCapacity(NewMem);
		fCapacityMem = NewMem;
	}
}



// ***********************************************************
// Apply functions
// ***********************************************************

// the size of memory buffer for reading dataset marginally
Int64 CoreArray::ARRAY_READ_MEM_BUFFER_SIZE = 1024*1024*1024;


// read an array-oriented object margin by margin

CdArrayRead::CdArrayRead()
{
	fObject = NULL;
	fMargin = 0;
	fSVType = svCustom;
	fIndex = fCount = 0;
	fMarginCount = 0;
	fMarginIndex = _MarginStart = _MarginEnd = 0;
	_Margin_Buf_IncCnt = 0;
	_Margin_Buf_Cnt = 0;
}

CdArrayRead::~CdArrayRead()
{
}
		
void CdArrayRead::Init(CdSequenceX &vObj, int vMargin, TSVType vSVType,
	const CBOOL *const vSelection[], bool buf_if_need)
{
	// set object
	fObject = &vObj;

	// check
	int DCnt = vObj.DimCnt();
	fMargin = vMargin;
	if ((vMargin < 0) || (vMargin >= DCnt))
		throw ErrSequence("Error margin %d: reading dataset marginally.", vMargin);

	// initialize ...
	CdSequenceX::TSeqDimBuf DimLen;
	vObj.GetDimLen(DimLen);

	// determine the size of element
	fSVType = vSVType;
	switch (vSVType)
	{
		case svInt8:         // Signed integer of 8 bits
		case svUInt8:        // Unsigned integer of 8 bits
			fElmSize = 1; break;
		case svInt16:        // Signed integer of 16 bits
		case svUInt16:       // Unsigned integer of 16 bits
			fElmSize = 2; break;
		case svInt32:        // Signed integer of 32 bits
		case svUInt32:       // Unsigned integer of 32 bits
			fElmSize = 4; break;
		case svInt64:        // Signed integer of 64 bits
		case svUInt64:       // Unsigned integer of 64 bits
			fElmSize = 8; break;
		case svFloat32:      // Float number of single precision (32 bits)
			fElmSize = 4; break;
		case svFloat64:      // Float number of double precision (64 bits)
			fElmSize = 8; break;
		case svStrUTF8:      // UTF-8 string
			fElmSize = 1;
			throw "Not support character in apply.gdsn";
			break;
		case svStrUTF16:     // UTF-16 string
			fElmSize = 2;
			throw "Not support character in apply.gdsn";
			break;
		default:
			if (dynamic_cast<CdVectorX*>(&vObj))
			{
				fElmSize = dynamic_cast<CdVectorX*>(&vObj)->ElmSize();
			} else {
				fElmSize = ((vObj.BitOf() & 0x7) > 0) ? (vObj.BitOf()/8 + 1) : (vObj.BitOf()/8);
			}
			if (fElmSize <= 0) fElmSize = 1;
			break;
	};

	// true for calling rData, false for calling rDataEx
	_Call_rData = true;

	// selection determination
	_sel_array.clear();
	_Have_Selection = (vSelection != NULL);
	if (_Have_Selection)
	{
		// with selection
		_sel_array.resize(DCnt);
		// for - loop
		for (int i=0; i < DCnt; i++)
		{
			bool v = fill_selection(DimLen[i], vSelection[i],
				_DStart[i], _DCount[i], _DCntValid[i]);
			if ((i > 0) && (!v))
				_Call_rData = false;
			_sel_array[i].assign(vSelection[i] + _DStart[i],
				vSelection[i] + _DStart[i] + _DCount[i]);
			_Selection[i] = &(_sel_array[i][0]);
		}
	} else {
		// no selection, using all data
		for (int i=0; i < DCnt; i++)
		{
			_DStart[i] = 0;
			_DCount[i] = DimLen[i];
			_DCntValid[i] = DimLen[i];
		}
	}

	// total number
	Int64 TotalCount = 1;
	for (int i=0; i < DCnt; i++)
		TotalCount *= _DCntValid[i];

	fIndex = 0; fCount = _DCntValid[vMargin];
	fMarginCount = (fCount > 0) ? (TotalCount / fCount) : 0;

	_MarginStart = _DStart[vMargin];
	fMarginIndex = _MarginStart;
	_MarginEnd = _DStart[vMargin] + _DCount[vMargin];


	// make a margin buffer
	if (vMargin > 0)
	{
		_Margin_Buf_Cnt = 0;

		_Margin_Buf_MajorCnt = 1;
		for (int i=0; i < vMargin; i++)
			_Margin_Buf_MajorCnt *= _DCntValid[i];

		_Margin_Buf_MinorSize = fElmSize;
		for (int i=vMargin+1; i < DCnt; i++)
			_Margin_Buf_MinorSize *= _DCntValid[i];

		// determine buffer
		if (buf_if_need)
		{
			// need a memory buffer to speed up
			_Margin_Buf_IncCnt = ARRAY_READ_MEM_BUFFER_SIZE / (fElmSize * fMarginCount);
	
			if (_Margin_Buf_IncCnt > 1)
			{
				if (_Margin_Buf_IncCnt > fCount)
					_Margin_Buf_IncCnt = fCount;
				_Margin_Buffer.resize(fElmSize * _Margin_Buf_IncCnt * fMarginCount);
			} else {
				_Margin_Buf_IncCnt = 1;
				_Margin_Buffer.clear();
			}
		} else {
			_Margin_Buf_IncCnt = 1;
			_Margin_Buffer.clear();
		}
	} else {
		_Margin_Buffer.clear();
	}
}

void CdArrayRead::AllocBuffer(Int64 buffer_size)
{
	if (fIndex >= fCount)
	{
		throw ErrSequence("call CdArrayRead::Init first.");
	}

	if (fMargin > 0)
	{
		if (buffer_size < 0)
			buffer_size = ARRAY_READ_MEM_BUFFER_SIZE;

		// need a memory buffer to speed up
		_Margin_Buf_IncCnt = buffer_size / (fElmSize * fMarginCount);
	
		if (_Margin_Buf_IncCnt > 1)
		{
			if (_Margin_Buf_IncCnt > fCount)
				_Margin_Buf_IncCnt = fCount;
			_Margin_Buffer.resize(fElmSize * _Margin_Buf_IncCnt * fMarginCount);
		} else {
			_Margin_Buf_IncCnt = 1;
			_Margin_Buffer.clear();
		}
	} else {
		_Margin_Buffer.clear();
	}
}

void CdArrayRead::Read(void *Buffer)
{
	if (fIndex < fCount)
	{
		// whether it is the major dimension
		if (fMargin == 0)
		{
			// init
			_DStart[0] = fMarginIndex;
			_DCount[0] = 1;

			// read sub data
			if (_Call_rData)
			{
				fObject->rData(_DStart, _DCount, Buffer, fSVType);
			} else {
				_Selection[0] = &(_sel_array[0][fMarginIndex - _MarginStart]);
				fObject->rDataEx(_DStart, _DCount, _Selection, Buffer, fSVType);
			}

			// next ``Index'', ``MarginIndex''
			fIndex ++;
			fMarginIndex ++;
			if (_Have_Selection)
			{
				// skip unselected layout
				while ((fMarginIndex < _MarginEnd) &&
					!_sel_array[0][fMarginIndex - _MarginStart])
				{
					fMarginIndex ++;
				}
			}
		} else {

			// determine buffer size
			if (_Margin_Buf_Cnt <= 0)
			{
				// determine '_Margin_Buf_Cnt' first
				if (_Margin_Buf_IncCnt > 1)
				{
					if (_Have_Selection)
					{
						_DCount[fMargin] = 0;
						_Margin_Buf_Cnt = 0;

						Int32 Cnt = _Margin_Buf_IncCnt;
						for (Int32 k=fMarginIndex; (k < _MarginEnd) && (Cnt > 0); k ++)
						{
							_DCount[fMargin] ++;
							if (_Selection[fMargin][k - _MarginStart])
							{
								_Margin_Buf_Cnt ++;
								Cnt --;
							}
						}
					} else {
						Int32 I = fMarginIndex + _Margin_Buf_IncCnt;
						if (I > _MarginEnd) I = _MarginEnd;
						_Margin_Buf_Cnt = I - fMarginIndex;
						_DCount[fMargin] = _Margin_Buf_Cnt;
					}
				} else {
					_Margin_Buf_Cnt = 1;
				}

				// read sub data to margin buffer
				_Margin_Buf_Need = (_Margin_Buf_Cnt > 1);
				_DStart[fMargin] = fMarginIndex;
				if (_Call_rData)
				{
					// call reading with a block
					if (_Margin_Buf_Need)
					{
						fObject->rData(_DStart, _DCount, &_Margin_Buffer[0], fSVType);
					} else {
						fObject->rData(_DStart, _DCount, Buffer, fSVType);
					}
				} else {
					// call reading with a selection
					_Selection[fMargin] = &(_sel_array[fMargin][fMarginIndex - _MarginStart]);
					if (_Margin_Buf_Need)
					{
						fObject->rDataEx(_DStart, _DCount, _Selection, &_Margin_Buffer[0], fSVType);
					} else {
						fObject->rDataEx(_DStart, _DCount, _Selection, Buffer, fSVType);
					}
				}

				if (_Margin_Buf_Need)
				{
					_Margin_Buf_MinorSize2 = _Margin_Buf_MinorSize * _Margin_Buf_Cnt;
				}

				_Margin_Buf_Old_Index = fIndex;
			}

			if (_Margin_Buf_Need)
			{
				UInt8 *s = &(_Margin_Buffer[0]) + _Margin_Buf_MinorSize *
					(fIndex - _Margin_Buf_Old_Index);
				UInt8 *p = (UInt8*)Buffer;
				for (Int64 n=_Margin_Buf_MajorCnt; n > 0; n--)
				{
					memcpy(p, s, _Margin_Buf_MinorSize);
					p += _Margin_Buf_MinorSize;
					s += _Margin_Buf_MinorSize2;
				}
			}

			_Margin_Buf_Cnt	--;

			// next ``Index'', ``MarginIndex''
			fIndex ++;
			fMarginIndex ++;
			if (_Have_Selection)
			{
				// skip unselected layout
				while ((fMarginIndex < _MarginEnd) &&
					!_sel_array[fMargin][fMarginIndex - _MarginStart])
				{
					fMarginIndex ++;
				}
			}
		}
	} else {
		throw ErrSequence("Invalid CdArrayRead::Read.");	
	}
}

bool CdArrayRead::Eof()
{
	return (fIndex >= fCount);
}



void CoreArray::Balance_ArrayRead_Buffer(CdArrayRead *array[], int n,
	Int64 buffer_size)
{
	if (n <= 0)
		throw ErrSequence("CoreArray::Balance_ArrayRead_Buffer !");

	if (buffer_size < 0)
		buffer_size = ARRAY_READ_MEM_BUFFER_SIZE;

	// calculate memory sizes
	vector<double> Mem(n);
	for (int i=0; i < n; i++)
	{
		Mem[i] = (array[i]->Margin() > 0) ? (double)array[i]->MarginSize() : 0.0;
	}

	// compute ratio
	double sum = 0;
	for (int i=0; i < n; i++) sum += Mem[i];
	for (int i=0; i < n; i++) Mem[i] /= sum;

	// reallocate buffer
	for (int i=0; i < n; i++)
	{
		if (Mem[i] > 0)
		{
			Int64 size = (Int64)(buffer_size * Mem[i]);
			array[i]->AllocBuffer(size);
		}
	}
}

void CoreArray::Balance_ArrayRead_Buffer(CdArrayRead array[], int n,
	Int64 buffer_size)
{
	vector<CdArrayRead*> list(n);
	for (int i=0; i < n; i++)
		list[i] = &array[i];
	Balance_ArrayRead_Buffer(&list[0], n, buffer_size);
}
