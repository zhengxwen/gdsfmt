// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dSerial.cpp: Serialization between class objects and stream data
//
// Copyright (C) 2007-2016    Xiuwen Zheng
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

#include "dSerial.h"


namespace CoreArray
{
using namespace std;


#ifdef COREARRAY_CC_MSC
#define snprintf sprintf_s
#endif


static const char *ERR_NO_STRUCTURE =
	"There is no structure in data stream!";
static const char *ERR_NO_NAMESPACE =
	"No name space in the current structure!";
static const char *ERR_INV_STRUCT_LENGTH =
	"Invalid structure length!";

static const char *ERR_NO_PROPERTY =
	"No property named '%s' in the current structure.";
static const char *ERR_DUP_VAR_NAME =
	"Duplicate property name '%s' in the current structure.";

static const char *ERR_PROP_NAME =
	"Invalid property name.";
static const char *ERR_PROP_NAME_CHAR_TOO_LONG =
	"The length of property name is too long (should be < 64).";
static const char *ERR_PROP_NAME_CHAR =
	"Invalid character in property name '%s'.";

static const char *ERR_CLASS_NAME_TOO_LONG =
	"The length of class name is too long (should be < 256).";

static const char *ERR_SHORT_BUFFER =
	"The length of buffer should be < 256 in a short structure!";

static const char *ERR_INV_TYPE_ID =
	"Invalid TYPE (id: %d) of the data!";

static const char *ERR_READER_NOT_STRING =
	"CdReader: the property '%s' is not a string.";
static const char *ERR_READER_NOT_RECORD =
	"CdReader: the property '%s' is not a structure.";
static const char *ERR_READER_NOT_SREC =
	"CdReader: the property '%s' is not a short structure.";
static const char *ERR_READER_NOT_NAMESPACE =
	"CdReader: the property '%s' is not a name space.";
static const char *ERR_READER_NOT_GDSPOS =
	"CdReader: the property '%s' is not a GDS position.";
static const char *ERR_READER_NOT_BLOCKID =
	"CdReader: the property '%s' is not a block id.";

static const char *ERR_READER_REC_INV_SIZE =
	"CdReader: the property '%s' has invalid size.";
static const char *ERR_READER_SREC_INV_SIZE =
	"CdReader: the property '%s' (a short structure) has invalid size.";


static char PropNameMapChar[64] = {
	'.', '_', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
	'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
	's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };


static const char *ERR_DUP_CLASS =
	"Duplicate Class Stream Name '%s'!";
static const char *ERR_INV_CLASS_NAME =
	"No class name '%s' in the GDS system.";
static const char *ERR_INV_VERSION =
	"Data version (v%d.%d) of '%s' is higher than what the object supports.";


// =====================================================================
// Serialization -- Root class
// =====================================================================

CdSerialization::CdSerialization(CdBufStream *vBufStream, CdLogRecord *vLog,
	CdObjClassMgr *vClassMgr)
{
	// buffer object
#ifdef COREARRAY_CODE_DEBUG
	if (vBufStream == NULL)
		throw ErrDEBUG("CdSerial::CdSerial(), vBufStream should not be NULL.");
#endif
	fStorage.Stream = vBufStream;
	if (vBufStream != NULL) vBufStream->AddRef();

	// class manager
	fClassMgr = (vClassMgr) ? vClassMgr : &dObjManager();

	// log recorder
	if (vLog == NULL) vLog = new CdLogRecord;
	(fLog = vLog)->AddRef();

	fStructListHead = NULL;
}

CdSerialization::CdSerialization(CdStream *vStream, CdLogRecord *vLog,
	CdObjClassMgr *vClassMgr)
{
	// buffer object
#ifdef COREARRAY_CODE_DEBUG
	if (vStream == NULL)
		throw ErrDEBUG("CdSerial::CdSerial(), vStream should not be NULL.");
#endif
	fStorage.Stream = new CdBufStream(vStream);
	fStorage.Stream->AddRef();

	// class manager
	fClassMgr = (vClassMgr) ? vClassMgr : &dObjManager();

	// log recorder
	if (vLog == NULL) vLog = new CdLogRecord;
	(fLog = vLog)->AddRef();

	fStructListHead = NULL;
}

CdSerialization::~CdSerialization()
{
	if (fStorage.Stream != NULL)
		fStorage.Stream->Release();
	if (fLog)
		fLog->Release();

	// clear structure list
	CVarList *p = fStructListHead;
	while (p != NULL)
	{
		CVarList *tmp = p;
		p = p->Next;
		delete tmp;
	}
	fStructListHead = NULL;
}

void CdSerialization::SetLog(CdLogRecord &vLog)
{
	if (fLog != &vLog)
	{
		if (fLog) fLog->Release();
		(fLog = &vLog)->AddRef();
	}
}

bool CdSerialization::HaveProperty(const char *Name)
{
	CVarList &p = CurrentStruct();
	return (p.Name2Variable(Name) != NULL);
}

TdSerialTypeID CdSerialization::PropTypeID(const char *Name)
{
	CVarList &p = CurrentStruct();
	TVariable *v = p.Name2Variable(Name);
	if (v)
		return v->TypeID;
	else
		throw ErrSerial(ERR_NO_PROPERTY, Name);
}

SIZE64 CdSerialization::PropPosition(const char *Name)
{
	CVarList &p = CurrentStruct();
	TVariable *v = p.Name2Variable(Name);
	if (v)
		return v->Start;
	else
		throw ErrSerial(ERR_NO_PROPERTY, Name);
}

void CdSerialization::PropInfo(const char *Name, TdSerialTypeID &TypeID,
	SIZE64 &Start, SIZE64 &Length)
{
	CVarList &p = CurrentStruct();
	TVariable *v = p.Name2Variable(Name);
	if (v)
	{
		TypeID = v->TypeID;
		Start = v->Start;
		Length = v->Length;
	} else
		throw ErrSerial(ERR_NO_PROPERTY, Name);
}

CdSerialization::CVarList &CdSerialization::PushStruct()
{
	CVarList *p = new CVarList;
	p->Next = fStructListHead;
	fStructListHead = p;
	return *p;
}

void CdSerialization::PopStruct()
{
	CVarList *p = fStructListHead;
	if (p != NULL)
	{
		fStructListHead = fStructListHead->Next;
		delete p;
	} else
		throw ErrSerial(ERR_NO_STRUCTURE);
}

CdSerialization::CVarList & CdSerialization::CurrentStruct()
{
	if (fStructListHead == NULL)
		throw ErrSerial(ERR_NO_STRUCTURE);
	return *fStructListHead;
}

CdSerialization::TVariable::TVariable()
{
	TypeID = osUnknown;
	Start = Length = 0;
	Next = NULL;
}

CdSerialization::TVariable::~TVariable()
{ }

CdSerialization::CVarList::CVarList()
{
	VarHead = VarTail = NULL;
	Start = Length = 0;
	VarCount = 0;
	Next = NULL;
}

CdSerialization::CVarList::~CVarList()
{
	ClearVarList();
}

CdSerialization::TVariable* CdSerialization::CVarList::Name2Variable(
	const char *Name)
{
	TVariable *p = VarHead;
	while (p != NULL)
	{
		if (p->Name.compare(Name) == 0)
			return p;
		p = p->Next;
	}
	return NULL;
}

void CdSerialization::CVarList::ClearVarList()
{
	TVariable *p = VarHead;
	while (p != NULL)
	{
		TVariable *tmp = p;
		p = p->Next;
		delete tmp;
	}
	VarHead = VarTail = NULL;
}



// =====================================================================
// Serialization -- Writer
// =====================================================================

CdWriter::CdWriter(CdBufStream *vBufStream, CdLogRecord *vLog,
	CdObjClassMgr *vClassMgr):
	CdSerialization(vBufStream, vLog, vClassMgr)
{
	fVar.Obj = this;
	fVar.Name = "";
}

CdWriter::CdWriter(CdStream *vStream, CdLogRecord *vLog,
	CdObjClassMgr *vClassMgr):
	CdSerialization(vStream, vLog, vClassMgr)
{
	fVar.Obj = this;
	fVar.Name = "";
}

CdWriter::~CdWriter()
{ }

void CdWriter::BeginStruct()
{
	CVarList &p = PushStruct();
	p.Start = fStorage.Position();
	p.VarCount = -1;  // indicating no name space
	fStorage << TdGDSPos(0);
}

void CdWriter::BeginNameSpace()
{
	CVarList &p = PushStruct();
	p.Start = fStorage.Position();
	p.VarCount = 0;  // indicating the number of variables in the name space
	fStorage << TdGDSPos(0) << C_UInt16(0);
}

void CdWriter::EndStruct()
{
	CVarList &p = CurrentStruct();
	p.Length = fStorage.Position() - p.Start;
	fStorage.SetPosition(p.Start);
	fStorage << TdGDSPos(p.Length);
	if (p.VarCount >= 0)  // having name space
		fStorage << C_UInt16(p.VarCount);
	fStorage.SetPosition(p.Start + p.Length);
	PopStruct();
}

CdWriter::TdVar &CdWriter::operator[] (const char *Name)
{
	if (Name == NULL)
		throw ErrSerial(ERR_PROP_NAME);
	if (strcmp(Name, "") == 0)
		throw ErrSerial(ERR_PROP_NAME);
	fVar.Name = Name;
	return fVar;
}

CdWriter::TVariable* CdWriter::NewVar(const char *Name,
	TdSerialTypeID TypeID, SIZE64 Size)
{
	CVarList &Cur = CurrentStruct();
	if (Cur.VarCount < 0)
		throw ErrSerial(ERR_NO_NAMESPACE);
	if (Cur.Name2Variable(Name) != NULL) // if name exists
		throw ErrSerial(ERR_DUP_VAR_NAME, Name);

	fStorage << C_UInt8(TypeID);
	WritePropName(Name);

	TVariable *p = new TVariable;
	p->Name = Name;
	p->TypeID = TypeID;
	p->Start = fStorage.Position();
	p->Length = Size;

	if (Cur.VarTail == NULL)
	{
		Cur.VarHead = Cur.VarTail = p;
	} else {
		Cur.VarTail->Next = p;
		Cur.VarTail = p;
	}
	Cur.VarCount ++;

	return p;
}

void CdWriter::WritePropName(const char *Name)
{
	size_t n = strlen(Name);
#ifdef COREARRAY_CODE_DEBUG
	// a 6-bit integer
	if (n > 63)
		throw ErrSerial(ERR_PROP_NAME_CHAR_TOO_LONG);
#endif

	BIT_LE_W<CdBufStream> BIT(fStorage.Stream);
	BIT.WriteBit(n, 6);

	char const* p = Name;
	while (*p)
	{
		char ch = *p++;
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
			throw ErrBufStream(ERR_PROP_NAME_CHAR, Name);
		BIT.WriteBit(ch, 6);
	}
}

void CdWriter::WriteClassName(const char *Name)
{
	size_t n = strlen(Name);
#ifdef COREARRAY_CODE_DEBUG
	if (n > 255)
		throw ErrSerial(ERR_CLASS_NAME_TOO_LONG);
#endif
	fStorage << C_UInt8(n);

	BIT_LE_W<CdBufStream> BIT(fStorage.Stream);
	for (; *Name; Name++)
		BIT.WriteBit(*Name & 0x7F, 7);
}


CdWriter::TdVar::TdVar()
{
	Obj = NULL;
	Name = NULL;
}

void CdWriter::TdVar::operator << (C_Int8 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osInt8, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (C_UInt8 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osUInt8, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (C_Int16 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osInt16, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (C_UInt16 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osUInt16, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (C_Int32 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osInt32, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (C_UInt32 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osUInt32, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (C_Float32 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osFloat32, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (C_Float64 val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osFloat64, sizeof(val));
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (const UTF8String &val)
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, osStrUTF8, 0);
	Obj->fStorage.WpUTF8(val);
	p->Length = Obj->fStorage.Position() - p->Start;
}

void CdWriter::TdVar::operator << (const UTF16String &val)
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, osStrUTF16, 0);
	Obj->fStorage.WpUTF16(val);
	p->Length = Obj->fStorage.Position() - p->Start;
}

void CdWriter::TdVar::operator << (const UTF32String &val)
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, osStrUTF32, 0);
	Obj->fStorage.WpUTF32(val);
	p->Length = Obj->fStorage.Position() - p->Start;
}

void CdWriter::TdVar::Wp16b(C_UInt16 val)
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, os16Packed, 0);
	Obj->fStorage.Wp16b(val);
	p->Length = Obj->fStorage.Position() - p->Start;
}

void CdWriter::TdVar::Wp32b(C_UInt32 val)
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, os32Packed, 0);
	Obj->fStorage.Wp32b(val);
	p->Length = Obj->fStorage.Position() - p->Start;
}

void CdWriter::TdVar::Wp64b(C_UInt64 val)
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, os64Packed, 0);
	Obj->fStorage.Wp64b(val);
	p->Length = Obj->fStorage.Position() - p->Start;
}

void CdWriter::TdVar::NewStruct()
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, osRecord, 0);
	Obj->BeginStruct();
	p->Start = Obj->fStorage.Position();
}

void CdWriter::TdVar::NewShortRec(void *Ptr, size_t Size)
{
	if (Obj == NULL) return;
	if (Size < 256)
	{
		CdWriter::TVariable *p =
			Obj->NewVar(Name, osShortRec, Size+1);
		Obj->fStorage << C_UInt8(Size);
		Obj->fStorage.WriteData(Ptr, Size);
		p->Start ++; // due to the first is the number of bytes
		             // the second is the context
		p->Length = Obj->fStorage.Position() - p->Start;
	} else
		throw ErrSerial(ERR_SHORT_BUFFER, (int)Size);
}

void CdWriter::TdVar::NewShortRec(const C_Int32 *pval, size_t n)
{
	if (Obj == NULL) return;
	size_t Size = sizeof(*pval) * n;
	if (Size < 256)
	{
		CdWriter::TVariable *p =
			Obj->NewVar(Name, osShortRec, Size+1);
		Obj->fStorage << C_UInt8(Size);
		Obj->fStorage.W(pval, n);
		p->Start ++; // due to the first is the number of bytes
		             // the second is the context
		p->Length = Obj->fStorage.Position() - p->Start;
	} else
		throw ErrSerial(ERR_SHORT_BUFFER, (int)Size);
}

void CdWriter::TdVar::NewShortRec(const C_Int64 *pval, size_t n)
{
	if (Obj == NULL) return;
	size_t Size = sizeof(*pval) * n;
	if (Size < 256)
	{
		CdWriter::TVariable *p =
			Obj->NewVar(Name, osShortRec, Size+1);
		Obj->fStorage << C_UInt8(Size);
		Obj->fStorage.W(pval, n);
		p->Start ++; // due to the first is the number of bytes
		             // the second is the context
		p->Length = Obj->fStorage.Position() - p->Start;
	} else
		throw ErrSerial(ERR_SHORT_BUFFER, (int)Size);
}

void CdWriter::TdVar::NewNameSpace()
{
	if (Obj == NULL) return;
	CdWriter::TVariable *p =
		Obj->NewVar(Name, osNameSpace, 0);
	Obj->BeginNameSpace();
	p->Start = Obj->fStorage.Position();
}

void CdWriter::TdVar::operator << (const TdGDSPos &val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osGDSPos, GDS_POS_SIZE);
	Obj->fStorage << val;
}

void CdWriter::TdVar::operator << (const TdGDSBlockID &val)
{
	if (Obj == NULL) return;
	Obj->NewVar(Name, osUInt32, 4);
	Obj->fStorage << (C_UInt32)val;
}

void CdWriter::TdVar::NewAutoArray(const C_Int32 *pval, size_t n)
{
	if (Obj == NULL) return;
	if (sizeof(*pval) * n < 256)
	{
		NewShortRec(pval, n);
	} else {
		NewStruct();
		Obj->fStorage.W(pval, n);
		Obj->EndStruct();
	}
}

void CdWriter::TdVar::NewAutoArray(const C_Int64 *pval, size_t n)
{
	if (Obj == NULL) return;
	if (sizeof(*pval) * n < 256)
	{
		NewShortRec(pval, n);
	} else {
		NewStruct();
		Obj->fStorage.W(pval, n);
		Obj->EndStruct();
	}
}



// =====================================================================
// Serialization -- Reader
// =====================================================================

CdReader::CdReader(CdBufStream *vBufStream, CdLogRecord *vLog,
	CdObjClassMgr *vClassMgr):
	CdSerialization(vBufStream, vLog, vClassMgr)
{
	fVar.Obj = this;
	fVar.Name = "";
}

CdReader::CdReader(CdStream *vStream, CdLogRecord *vLog,
	CdObjClassMgr *vClassMgr):
	CdSerialization(vStream, vLog, vClassMgr)
{
	fVar.Obj = this;
	fVar.Name = "";
}

CdReader::~CdReader()
{ }

SIZE64 CdReader::BeginStruct()
{
	CVarList &p = PushStruct();
	p.Start = fStorage.Position();
	p.VarCount = -1; // indicating no name space
	TdGDSPos tmp; fStorage >> tmp; p.Length = tmp;
	if (p.Length < GDS_POS_SIZE)
		throw ErrSerial(ERR_INV_STRUCT_LENGTH);
	return p.Length - GDS_POS_SIZE;
}

SIZE64 CdReader::BeginNameSpace()
{
	SIZE64 rv = _BeginNameSpace();
	// initialize the name space
	_InitNameSpace();
	// output
	return rv;
}

void CdReader::EndStruct()
{
	CVarList &p = CurrentStruct();
	fStorage.SetPosition(p.Start + p.Length);
	PopStruct();
}

SIZE64 CdReader::_BeginNameSpace()
{
	// push a block structure
	CVarList &p = PushStruct();

	// get the length of block
	p.Start = fStorage.Position();
	TdGDSPos tmp;
	fStorage >> tmp;
	p.Length = tmp;
	if (p.Length < (GDS_POS_SIZE + 2))
		throw ErrSerial(ERR_INV_STRUCT_LENGTH);

	// get the number of variables in the block
	C_UInt16 nVar;
	fStorage >> nVar;
	p.VarCount = nVar;

	// output
	return p.Length - GDS_POS_SIZE;
}

void CdReader::_InitNameSpace()
{
	// get the current block structure
	CVarList &Cur = CurrentStruct();
	// clear the current variable list
	Cur.ClearVarList();

	// for-loop: variables
	for (int i=1; i <= Cur.VarCount; i++)
	{
		// Read Type ID
		TdSerialTypeID TypeID = (TdSerialTypeID)fStorage.R8b();
		// Read property name
		string Name = ReadPropName();

		// if name exists, add a warning
		if (Cur.Name2Variable(Name.c_str()) != NULL)
		{
			Log().Add(CdLogRecord::logWarn,
				ERR_DUP_VAR_NAME, Name.c_str());
		}

		switch (TypeID)
		{
			case osRecord:
			{
				NewVar<void*>(Name, TypeID);
				TdGDSPos PI; fStorage >> PI;
				Cur.VarTail->Start = fStorage.Position();
				if (PI < GDS_POS_SIZE)
					throw ErrSerial(ERR_INV_STRUCT_LENGTH);
				fStorage.SetPosition(Cur.VarTail->Start + PI - GDS_POS_SIZE);
				break;
			}
			case osShortRec:
			{
				TShortRec &Rec = NewVar<TShortRec>(Name, TypeID);
				memset(&Rec, 0, sizeof(Rec));
				Rec.Size = fStorage.R8b();
				// due to the first is the number of bytes, the second is the context
				Cur.VarTail->Start = fStorage.Position();
				fStorage.ReadData(Rec.Buffer, Rec.Size);
				break;
			}
			case osNameSpace:
			{
				NewVar<void*>(Name, TypeID);
				Cur.VarTail->Start = fStorage.Position();
				TdGDSPos PI; fStorage >> PI;
				if (PI < (GDS_POS_SIZE + (ssize_t)sizeof(C_UInt16)))
					throw ErrSerial(ERR_INV_STRUCT_LENGTH);
				fStorage.SetPosition(Cur.VarTail->Start + PI);
				break;
			}

			// integer
			case osInt8:
				fStorage >> NewVar<C_Int8>(Name, TypeID); break;
			case osUInt8:
				fStorage >> NewVar<C_UInt8>(Name, TypeID); break;
			case osInt16:
				fStorage >> NewVar<C_Int16>(Name, TypeID); break;
			case osUInt16:
				fStorage >> NewVar<C_UInt16>(Name, TypeID); break;
			case osInt32:
				fStorage >> NewVar<C_Int32>(Name, TypeID); break;
			case osUInt32:
				fStorage >> NewVar<C_UInt32>(Name, TypeID); break;
			case osInt64:
				fStorage >> NewVar<C_Int64>(Name, TypeID); break;
			case osUInt64:
				fStorage >> NewVar<C_UInt64>(Name, TypeID); break;
			case os16Packed:
				NewVar<C_UInt16>(Name, TypeID) = fStorage.Rp16b(); break;
			case os32Packed:
				NewVar<C_UInt32>(Name, TypeID) = fStorage.Rp32b(); break;
			case os64Packed:
				NewVar<C_UInt64>(Name, TypeID) = fStorage.Rp64b(); break;

			// floating point number
			case osFloat32:
				fStorage >> NewVar<C_Float32>(Name, TypeID); break;
			case osFloat64:
				fStorage >> NewVar<C_Float64>(Name, TypeID); break;

			// string
			case osStrUTF8:
				NewVar<UTF8String>(Name, TypeID) = fStorage.RpUTF8(); break;
			case osStrUTF16:
				NewVar<UTF16String>(Name, TypeID) = fStorage.RpUTF16(); break;
			case osStrUTF32:
				NewVar<UTF32String>(Name, TypeID) = fStorage.RpUTF32(); break;

			// others
			case osGDSPos:
				fStorage >> NewVar<TdGDSPos>(Name, TypeID); break;

			default:
				throw ErrSerial(ERR_INV_TYPE_ID, (int)TypeID);
		}

		// the length of variable
		Cur.VarTail->Length = fStorage.Position() - Cur.VarTail->Start;
	}
}

CdReader::TdVar &CdReader::operator[] (const char *Name)
{
	if (Name == NULL)
		throw ErrSerial(ERR_PROP_NAME);
	if (strcmp(Name, "") == 0)
		throw ErrSerial(ERR_PROP_NAME);
	fVar.Name = Name;
	return fVar;
}

string CdReader::ReadPropName()
{
	BIT_LE_R<CdBufStream> BIT(fStorage.Stream);
	size_t n = BIT.ReadBit(6);
	string rv(n, '\x0');
	for (size_t i=0; i < n; i++)
		rv[i] = PropNameMapChar[BIT.ReadBit(6)];
	return rv;
}

string CdReader::ReadClassName()
{
	size_t n = fStorage.Stream->R8b();
	string rv(n, '\x0');
	BIT_LE_R<CdBufStream> BIT(fStorage.Stream);
	for (size_t i = 0; i < n; i++)
		rv[i] = BIT.ReadBit(7);
	return rv;
}

CdSerialization::TVariable* CdReader::FindVar(const char *Name)
{
	CVarList &Cur = CurrentStruct();
	if (Cur.VarCount < 0)
		throw ErrSerial(ERR_NO_NAMESPACE);

	// find it
	TVariable *p=Cur.VarHead, *pH=NULL;
	while (p != NULL)
	{
		if (p->Name.compare(Name) == 0)
			break;
		pH = p;
		p = p->Next;
	}
	if (p == NULL)
		throw ErrSerial(ERR_NO_PROPERTY, Name);

	if (p->Next != NULL)
	{
		// not the last one, move to the last one
		if (pH != NULL)
		{
			pH->Next = p->Next;
			Cur.VarTail->Next = p;
			p->Next = NULL;
			Cur.VarTail = p;
		} else {
			// VarHead == VarTail, it is not possible
			Cur.VarHead = p->Next;
			Cur.VarTail->Next = p;
			p->Next = NULL;
			Cur.VarTail = p;
		}
	}

	return p;
}

CdReader::TdVar::TdVar()
{
	Obj = NULL;
	Name = NULL;
}

void CdReader::TdVar::operator >> (C_Int8 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_Int8>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_UInt8 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_UInt8>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_Int16 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_Int16>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_UInt16 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_UInt16>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_Int32 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_Int32>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_UInt32 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_UInt32>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_Int64 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_Int64>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_UInt64 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_UInt64>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_Float32 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_Float32>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (C_Float64 &val)
{
	if (Obj == NULL) return;
	val = CdReader::Cvt_Num<C_Float64>(Obj->FindVar(Name));
}

void CdReader::TdVar::operator >> (UTF8String &val)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	switch (Var->TypeID)
	{
	case osStrUTF8:
		val = static_cast<TVar<UTF8String>*>(Var)->Data;
		break;
	case osStrUTF16:
		val = UTF16ToUTF8(static_cast<TVar<UTF16String>*>(Var)->Data);
		break;
	case osStrUTF32:
		val = UTF32ToUTF8(static_cast<TVar<UTF32String>*>(Var)->Data);
		break;
	default:
		throw ErrSerial(ERR_READER_NOT_STRING, Var->Name.c_str());
	}
}

void CdReader::TdVar::operator >> (UTF16String &val)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	switch (Var->TypeID)
	{
	case osStrUTF8:
		val = UTF8ToUTF16(static_cast<TVar<UTF8String>*>(Var)->Data);
		break;
	case osStrUTF16:
		val = static_cast<TVar<UTF16String>*>(Var)->Data;
		break;
	case osStrUTF32:
		val = UTF32ToUTF16(static_cast<TVar<UTF32String>*>(Var)->Data);
		break;
	default:
		throw ErrSerial(ERR_READER_NOT_STRING, Var->Name.c_str());
	}
}

void CdReader::TdVar::operator >> (UTF32String &val)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	switch (Var->TypeID)
	{
	case osStrUTF8:
		val = UTF8ToUTF32(static_cast<TVar<UTF8String>*>(Var)->Data);
		break;
	case osStrUTF16:
		val = UTF16ToUTF32(static_cast<TVar<UTF16String>*>(Var)->Data);
		break;
	case osStrUTF32:
		val = static_cast<TVar<UTF32String>*>(Var)->Data;
		break;
	default:
		throw ErrSerial(ERR_READER_NOT_STRING, Var->Name.c_str());
	}
}

void CdReader::TdVar::BeginStruct()
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	if (Var->TypeID == osRecord)
	{
		Obj->fStorage.SetPosition(Var->Start - GDS_POS_SIZE);
		Obj->BeginStruct();
	} else
		throw ErrSerial(ERR_READER_NOT_RECORD, Var->Name.c_str());
}

void CdReader::TdVar::BeginNameSpace()
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	if (Var->TypeID == osNameSpace)
	{
		Obj->fStorage.SetPosition(Var->Start);
		Obj->BeginNameSpace();
	} else
		throw ErrSerial(ERR_READER_NOT_NAMESPACE, Var->Name.c_str());
}

void CdReader::TdVar::GetShortRec(void* &OutPtr, size_t &Size)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	if (Var->TypeID == osShortRec)
	{
		TShortRec &Rec = static_cast<TVar<TShortRec>*>(Var)->Data;
		OutPtr = Rec.Buffer;
		Size = Rec.Size;
	} else
		throw ErrSerial(ERR_READER_NOT_SREC, Var->Name.c_str());
}

void CdReader::TdVar::GetShortRec(C_Int32 *pval, size_t n)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	if (Var->TypeID == osShortRec)
	{
		TShortRec &Rec = static_cast<TVar<TShortRec>*>(Var)->Data;
		if (Rec.Size == sizeof(*pval)*n)
		{
			memcpy(pval, Rec.Buffer, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
		} else
			throw ErrSerial(ERR_READER_SREC_INV_SIZE, Var->Name.c_str());
	} else
		throw ErrSerial(ERR_READER_NOT_SREC, Var->Name.c_str());
}

void CdReader::TdVar::GetShortRec(C_Int64 *pval, size_t n)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	if (Var->TypeID == osShortRec)
	{
		TShortRec &Rec = static_cast<TVar<TShortRec>*>(Var)->Data;
		if (Rec.Size == sizeof(*pval)*n)
		{
			memcpy(pval, Rec.Buffer, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
		} else
			throw ErrSerial(ERR_READER_SREC_INV_SIZE, Var->Name.c_str());
	} else
		throw ErrSerial(ERR_READER_NOT_SREC, Var->Name.c_str());
}

void CdReader::TdVar::operator >> (TdGDSPos &val)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	if (Var->TypeID == osGDSPos)
	{
		val = static_cast<TVar<TdGDSPos>*>(Var)->Data;
	} else
		throw ErrSerial(ERR_READER_NOT_GDSPOS, Var->Name.c_str());
}

void CdReader::TdVar::operator >> (TdGDSBlockID &val)
{
	if (Obj == NULL) return;
	CdReader::TVariable *Var = Obj->FindVar(Name);
	if (Var->TypeID == osUInt32)
	{
		val = static_cast<TVar<C_UInt32>*>(Var)->Data;
	} else
		throw ErrSerial(ERR_READER_NOT_BLOCKID, Var->Name.c_str());
}

void CdReader::TdVar::GetAutoArray(C_Int32 *pval, ssize_t n)
{
	if ((Obj == NULL) || (n < 0))
		return;
	if (sizeof(*pval) * n >= 256)
	{
		CdReader::TVariable *Var = Obj->FindVar(Name);
		if (Var->TypeID == osRecord)
		{
			if (Var->Length != SIZE64(n * sizeof(*pval)))
				throw ErrSerial(ERR_READER_REC_INV_SIZE, Var->Name.c_str());
			Obj->fStorage.SetPosition(Var->Start - GDS_POS_SIZE);
			Obj->BeginStruct();
			Obj->fStorage.R(pval, n);
			Obj->EndStruct();
		} else
			throw ErrSerial(ERR_READER_NOT_RECORD, Var->Name.c_str());
	} else
		GetShortRec(pval, n);
}

void CdReader::TdVar::GetAutoArray(C_Int64 *pval, ssize_t n)
{
	if ((Obj == NULL) || (n < 0))
		return;
	if (sizeof(*pval) * n >= 256)
	{
		CdReader::TVariable *Var = Obj->FindVar(Name);
		if (Var->TypeID == osRecord)
		{
			if (Var->Length != SIZE64(n * sizeof(*pval)))
				throw ErrSerial(ERR_READER_REC_INV_SIZE, Var->Name.c_str());
			Obj->fStorage.SetPosition(Var->Start - GDS_POS_SIZE);
			Obj->BeginStruct();
			Obj->fStorage.R(pval, n);
			Obj->EndStruct();
		} else
			throw ErrSerial(ERR_READER_NOT_RECORD, Var->Name.c_str());
	} else
		GetShortRec(pval, n);
}



// =====================================================================
// Serialization -- Class manager
// =====================================================================

static CdObjClassMgr dObjMgr;

CdObjClassMgr &dObjManager()
{
	return dObjMgr;
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
		throw ErrObject(ERR_DUP_CLASS, ClassName);
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

CdObjRef* CdObjClassMgr::ToObj(CdReader &Reader, TdInit OnInit,
	void *Data, bool Silent)
{
	TdOnObjCreate OnCreate;
	TdVersion Version;
	string Name;
	CdObjRef *Obj = NULL;

	Reader._BeginNameSpace();
	try {
		Version = Reader.Storage().R8b();
		Version |= ((TdVersion)Reader.Storage().R8b()) << 8;
		Name = Reader.ReadClassName();
		OnCreate = NameToClass(Name.c_str());

		if (OnCreate)
		{
			Obj = OnCreate();
			if (OnInit) OnInit(*this, Obj, Data);
			// check version number
			if (Version > Obj->dVersion())
			{
				throw ErrSerial(ERR_INV_VERSION, Version >> 8, Version & 0xFF,
					Name.c_str());
			}
			// load
			Reader._InitNameSpace();
			_INTERNAL::CdObject_LoadStruct(*Obj, Reader, Version);
		} else
			throw ErrSerial(ERR_INV_CLASS_NAME, Name.c_str());
	} catch (exception &E) {
		Reader.Log().Add(E.what());
		delete Obj;
		Obj = NULL;
		if (!Silent)
		{
			Reader.EndStruct();
			throw;
		}
	}
	Reader.EndStruct();

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
		throw ErrSerial(ERR_INV_CLASS_NAME, ClassName);
}

void CdObjClassMgr::ClassList(vector<string> &Key, vector<string> &Desp)
{
	Key.clear();
	Desp.clear();

	map<const char *, _ClassStruct, _strCmp>::iterator it;
	for (it=fClassMap.begin(); it != fClassMap.end(); it++)
	{
		Key.push_back(it->first);
		Desp.push_back(it->second.Desp);
	}
}

}
