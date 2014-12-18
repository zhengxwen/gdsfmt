// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dAny.cpp: Methods for CdAny
//
// Copyright (C) 2007 - 2015	Xiuwen Zheng
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

#include "dAny.h"


using namespace std;
using namespace CoreArray;

// Define variant type of data structure.

static const char * const erDSConvert = "Can't convert '%s' to '%s'!";

Err_dsAny::Err_dsAny(CdAny::TDataType fromType, CdAny::TDataType toType):
	ErrConvert()
{
	const char *s1 = CdAny::dvtNames(fromType);
	const char *s2 = CdAny::dvtNames(toType);
	fMessage = Format(erDSConvert, s1, s2);
}

// CdAny

CdAny::CdAny()
{
	dsType = dvtNULL;
}

CdAny::~CdAny()
{
	_Done();
}

void CdAny::_Done()
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

const char *CdAny::dvtNames(int index)
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

// CdAny : float number
#define DSDATA_FLOAT(TYPE) \
	case dvtFloat32:  return ValCvt<TYPE, C_Float32>(VAL<C_Float32>()); \
	case dvtFloat64:  return ValCvt<TYPE, C_Float64>(VAL<C_Float64>()); \

// CdAny : string
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

// CdAny : others
#define DSDATA_RETURN_OTHER(TYPE, dvt) \
	case dvtBoolean: \
		return ValCvt<TYPE, int>(VAL<C_UInt8>() ? 1 : 0); \
	default: \
		throw Err_dsAny(dsType, dvt);

#define DSDATA_RETURN_STR_OTHER(TYPE, dvt) \
	case dvtBoolean: \
		return ValCvt<TYPE, UTF8String>(VAL<C_UInt8>() ? ASC("TRUE") : ASC("FLASE")); \
	case dvtObjRef: \
		if (mix.aR.obj != NULL) \
			return ValCvt<TYPE, UTF8String>(UTF8Text(mix.aR.obj->dTraitName())); \
		else \
			return ValCvt<TYPE, UTF8String>(ASC("[NULL]")); \
	default: \
		throw Err_dsAny(dsType, dvt);


C_Int8 CdAny::GetInt8() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int8)
		DSDATA_FLOAT(C_Int8)
		DSDATA_STR(C_Int8)
		DSDATA_RETURN_OTHER(C_Int8, dvtInt8)
	}
}

C_UInt8 CdAny::GetUInt8() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt8)
		DSDATA_FLOAT(C_UInt8)
		DSDATA_STR(C_UInt8)
		DSDATA_RETURN_OTHER(C_UInt8, dvtUInt8)
	}
}

C_Int16 CdAny::GetInt16() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int16)
		DSDATA_FLOAT(C_Int16)
		DSDATA_STR(C_Int16)
		DSDATA_RETURN_OTHER(C_Int16, dvtInt16)
	}
}

C_UInt16 CdAny::GetUInt16() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt16)
		DSDATA_FLOAT(C_UInt16)
		DSDATA_STR(C_UInt16)
		DSDATA_RETURN_OTHER(C_UInt16, dvtUInt16)
	}
}

C_Int32 CdAny::GetInt32() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int32)
		DSDATA_FLOAT(C_Int32)
		DSDATA_STR(C_Int32)
		DSDATA_RETURN_OTHER(C_Int32, dvtInt32)
	}
}

C_UInt32 CdAny::GetUInt32() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt32)
		DSDATA_FLOAT(C_UInt32)
		DSDATA_STR(C_UInt32)
		DSDATA_RETURN_OTHER(C_UInt32, dvtUInt32)
	}
}

C_Int64 CdAny::GetInt64() const
{
	switch (dsType)
	{
		DSDATA_INT(C_Int64)
		DSDATA_FLOAT(C_Int64)
		DSDATA_STR(C_Int64)
		DSDATA_RETURN_OTHER(C_Int64, dvtInt64)
	}
}

C_UInt64 CdAny::GetUInt64() const
{
	switch (dsType)
	{
		DSDATA_INT(C_UInt64)
		DSDATA_FLOAT(C_UInt64)
		DSDATA_STR(C_UInt64)
		DSDATA_RETURN_OTHER(C_UInt64, dvtUInt64)
	}
}

C_Float32 CdAny::GetFloat32() const
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

C_Float64 CdAny::GetFloat64() const
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

UTF8String CdAny::GetStr8() const
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

UTF16String CdAny::GetStr16() const
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

UTF32String CdAny::GetStr32() const
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

bool CdAny::GetBool() const
{
	switch (dsType)
	{
		DSDATA_INT(int)
		DSDATA_FLOAT(int)
		DSDATA_STR(int)
		DSDATA_RETURN_OTHER(int, dvtStr32)
	}
}

const void *CdAny::GetPtr() const
{
	if (dsType == dvtPointer)
	{
		return mix.aR.const_ptr;
	} else
		throw Err_dsAny(dsType, dvtPointer);
}

CdAny *CdAny::GetArray() const
{
	if (dsType == dvtArray)
	{
		return mix.aArray.ArrayPtr;
	} else
		throw Err_dsAny(dsType, dvtArray);
}

C_UInt32 CdAny::GetArrayLength() const
{
	if (dsType == dvtArray)
	{
		return mix.aArray.ArrayLength;
	} else
		throw Err_dsAny(dsType, dvtArray);
}

CdObjRef *CdAny::GetObj() const
{
	if (dsType == dvtObjRef)
		return mix.aR.obj;
	else
		throw Err_dsAny(dsType, dvtObjRef);
}

void CdAny::SetEmpty()
{
	_Done();
	dsType = dvtNULL;
}

void CdAny::SetInt8(const C_Int8 val)
{
	_Done();
	dsType = dvtInt8;
	VAL<C_Int8>() = val;
}

void CdAny::SetUInt8(const C_UInt8 val)
{
	_Done();
	dsType = dvtUInt8;
	VAL<C_UInt8>() = val;
}

void CdAny::SetInt16(const C_Int16 val)
{
	_Done();
	dsType = dvtInt16;
	VAL<C_Int16>() = val;
}

void CdAny::SetUInt16(const C_UInt16 val)
{
	_Done();
	dsType = dvtUInt16;
	VAL<C_UInt16>() = val;
}

void CdAny::SetInt32(const C_Int32 val)
{
	_Done();
	dsType = dvtInt32;
	VAL<C_Int32>() = val;
}

void CdAny::SetUInt32(const C_UInt32 val)
{
	_Done();
	dsType = dvtUInt32;
	VAL<C_UInt32>() = val;
}

void CdAny::SetInt64(const C_Int64 val)
{
	_Done();
	dsType = dvtInt64;
	VAL<C_Int64>() = val;
}

void CdAny::SetUInt64(const C_UInt64 val)
{
	_Done();
	dsType = dvtUInt64;
	VAL<C_UInt64>() = val;
}

void CdAny::SetFloat32(const C_Float32 val)
{
	_Done();
	dsType = dvtFloat32;
	VAL<C_Float32>() = val;
}

void CdAny::SetFloat64(const C_Float64 val)
{
	_Done();
	dsType = dvtFloat64;
	VAL<C_Float64>() = val;
}

void CdAny::SetStr8(const UTF8String &val)
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

void CdAny::SetStr16(const UTF16String &val)
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

void CdAny::SetStr32(const UTF32String &val)
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

void CdAny::SetBool(const bool val)
{
	_Done();
	dsType = dvtBoolean;
	VAL<C_UInt8>() = (C_UInt8)val;
}

void CdAny::SetPtr(const void *ptr)
{
	_Done();
	dsType = dvtPointer;
	mix.aR.const_ptr = ptr;
}

void CdAny::SetArray(C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new CdAny[size];
}

void CdAny::SetArray(const C_Int32 ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new CdAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = ptr[i];
}

void CdAny::SetArray(const C_Int64 ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new CdAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = ptr[i];
}

void CdAny::SetArray(const C_Float64 ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new CdAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = ptr[i];
}

void CdAny::SetArray(const char* const ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new CdAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = UTF8Text(ptr[i]);
}

void CdAny::SetArray(const std::string ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new CdAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i] = UTF8Text(ptr[i]);
}

void CdAny::SetArray(const bool ptr[], C_UInt32 size)
{
	_Done();
	dsType = dvtArray;
	mix.aArray.ArrayLength = size;
	mix.aArray.ArrayPtr = new CdAny[size];
	for (C_UInt32 i=0; i < size; i++)
		mix.aArray.ArrayPtr[i].SetBool(ptr[i]);
}

void CdAny::SetObj(CdObjRef *obj)
{
	_Done();
	dsType = dvtObjRef;
	VAL<CdObjRef*>() = obj;
	if (obj)
		obj->AddRef();
}

void CdAny::Assign(const UTF8String &val)
{
	_Done();

	string s = RawText(val);

	if (s.empty())
	{
		dsType = dvtNULL;
		return;
	}
	if (s == "TRUE")
	{
		SetBool(true);
		return;
	}
	if (s == "FALSE")
	{
		SetBool(false);
		return;
	}

	char * p;
	long rv = strtol(s.c_str(), &p, 10);
	if (!*p)
	{
		SetInt64(rv); Packed();
		return;
	}

	double rd = strtod(s.c_str(), &p);
	if (!*p)
	{
		SetFloat64(rd);
		return;
	}

	SetStr8(val);
}

bool CdAny::IsNULL() const
{
	return (dsType == dvtNULL);
}

bool CdAny::IsInt() const
{
	return (dvtInt8 <= dsType) && (dsType <= dvtUInt64);
}

bool CdAny::IsFloat() const
{
	return (dvtFloat32 <= dsType) && (dsType <= dvtFloat64);
}

bool CdAny::IsNumeric() const
{
	return IsInt() || IsFloat();
}

bool CdAny::IsBool() const
{
	return (dsType == dvtBoolean);
}

bool CdAny::IsNaN() const
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

bool CdAny::IsNA() const
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

bool CdAny::IsString() const
{
	return (dvtSString8 <= dsType) && (dsType <= dvtStr32);
}

bool CdAny::IsArray() const
{
	return (dsType == dvtArray);
}

bool CdAny::Packed()
{
	#define xRANGE(L, I, H) ((L<=I) && (I<=H))
	#define yRANGE(L, H) (L<=H)
	C_Int64 I = 0;
	C_UInt64 U = 0;
	TDataType t = dsType;

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

void CdAny::Swap(CdAny &D)
{
	char buf[sizeof(CdAny)];

	memcpy((void*)buf, (void*)&D, sizeof(CdAny));
	memcpy((void*)&D, (void*)this, sizeof(CdAny));
	memcpy((void*)this, (void*)buf, sizeof(CdAny));
}

int CdAny::Compare(const CdAny &D, bool NALast)
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

CdAny & CdAny::operator= (const CdAny &_Right)
{
	if (this != &_Right)
	{
		_Done();
		switch (_Right.dsType)
		{
			case dvtObjRef:
				memcpy((void*)this, (void*)&_Right, sizeof(CdAny));
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
				memcpy(this, &_Right, sizeof(CdAny));
		}
	}
	return *this;
}

// TODO
CdReader& CoreArray::operator>> (CdReader &s, CdAny& out)
{
	out._Done();
	out.dsType = s.Storage().R8b();

	switch (out.dsType)
	{
		case CdAny::dvtNULL:
			break;

    	// integer
		case CdAny::dvtInt8:
			s.Storage() >> out.VAL<C_Int8>(); break;
		case CdAny::dvtUInt8:
			s.Storage() >> out.VAL<C_UInt8>(); break;
		case CdAny::dvtInt16:
			s.Storage() >> out.VAL<C_Int16>(); break;
		case CdAny::dvtUInt16:
			s.Storage() >> out.VAL<C_UInt16>(); break;
		case CdAny::dvtInt32:
			s.Storage() >> out.VAL<C_Int32>(); break;
		case CdAny::dvtUInt32:
			s.Storage() >> out.VAL<C_UInt32>(); break;
		case CdAny::dvtInt64:
			s.Storage() >> out.VAL<C_Int64>(); break;
		case CdAny::dvtUInt64:
			s.Storage() >> out.VAL<C_UInt64>(); break;

		// float number
		case CdAny::dvtFloat32:
			s.Storage() >> out.VAL<C_Float32>(); break;
		case CdAny::dvtFloat64:
			s.Storage() >> out.VAL<C_Float64>(); break;

		// string
		case CdAny::dvtSString8:
			out.mix.aS8.SStrLen8 = s.Storage().R8b();
			if (out.mix.aS8.SStrLen8 > 22)
			{
            	throw Err_dsAny("Invalid length (%d) for dvtSString8",
            		out.mix.aS8.SStrLen8);
            }
			s.Storage().R(out.mix.aS8.SStr8, out.mix.aS8.SStrLen8);
			break;
		case CdAny::dvtSString16:
			out.mix.aS16.SStrLen16 = s.Storage().R8b();
			if (out.mix.aS16.SStrLen16 > 11)
			{
            	throw Err_dsAny("Invalid length (%d) for dvtSString16",
            		out.mix.aS16.SStrLen16);
            }
			s.Storage().R(out.mix.aS16.SStr16, out.mix.aS16.SStrLen16);
			break;
		case CdAny::dvtSString32:
			out.mix.aS32.SStrLen32 = s.Storage().R8b();
			if (out.mix.aS32.SStrLen32 > 5)
			{
            	throw Err_dsAny("Invalid length (%d) for dvtSString32",
            		out.mix.aS32.SStrLen32);
            }
			s.Storage().R(out.mix.aS32.SStr32, out.mix.aS32.SStrLen32);
        	break;
		case CdAny::dvtStr8:
            out.mix.aR.ptrStr8 = new UTF8String(s.Storage().RpUTF8());
			break;

		case CdAny::dvtStr16:
			out.mix.aR.ptrStr16 = new UTF16String(s.Storage().RpUTF16());
			break;
		case CdAny::dvtStr32:
			out.mix.aR.ptrStr32 = new UTF32String(s.Storage().RpUTF32());
			break;

		// other extended type
		case CdAny::dvtBoolean:
			s.Storage() >> out.VAL<C_UInt8>();
			break;

		// pointer
		case CdAny::dvtPointer:
			out.mix.aR.const_ptr = NULL;
			break;

		// array
		case CdAny::dvtArray:
			s.Storage() >> out.mix.aArray.ArrayLength;
			out.mix.aArray.ArrayPtr = new CdAny[out.mix.aArray.ArrayLength];
			for (C_UInt32 i=0; i < out.mix.aArray.ArrayLength; i++)
				s >> out.mix.aArray.ArrayPtr[i];
			break;

		// CdObjRef
		case CdAny::dvtObjRef:
			if (s.Storage().R8b())
			{
				out.mix.aR.obj = dObjManager().ToObj(s, NULL, NULL, true);
				if (out.mix.aR.obj)
					out.mix.aR.obj->AddRef();
			} else
				out.mix.aR.obj = NULL;
			break;
	}
	return s;
}

CdWriter& CoreArray::operator<< (CdWriter &s, CdAny &in)
{
	s.Storage() << C_UInt8(in.dsType);
	switch (in.dsType)
	{
		// integer
		case CdAny::dvtInt8:
			s.Storage() << in.VAL<C_Int8>(); break;
		case CdAny::dvtUInt8:
			s.Storage() << in.VAL<C_UInt8>(); break;
		case CdAny::dvtInt16:
			s.Storage() << in.VAL<C_Int16>(); break;
		case CdAny::dvtUInt16:
			s.Storage() << in.VAL<C_UInt16>(); break;
		case CdAny::dvtInt32:
			s.Storage() << in.VAL<C_Int32>(); break;
		case CdAny::dvtUInt32:
			s.Storage() << in.VAL<C_UInt32>(); break;
		case CdAny::dvtInt64:
			s.Storage() << in.VAL<C_Int64>(); break;
		case CdAny::dvtUInt64:
			s.Storage() << in.VAL<C_UInt64>(); break;

		// float number
		case CdAny::dvtFloat32:
			s.Storage() << in.VAL<C_Float32>(); break;
		case CdAny::dvtFloat64:
			s.Storage() << in.VAL<C_Float64>(); break;

		// string
		case CdAny::dvtSString8:
			s.Storage() << C_UInt8(in.mix.aS8.SStrLen8);
			s.Storage().W(in.mix.aS8.SStr8, in.mix.aS8.SStrLen8);
			break;
		case CdAny::dvtSString16:
			s.Storage() << C_UInt8(in.mix.aS16.SStrLen16);
			s.Storage().W(in.mix.aS16.SStr16, in.mix.aS16.SStrLen16);
			break;
		case CdAny::dvtSString32:
			s.Storage() << C_UInt8(in.mix.aS32.SStrLen32);
			s.Storage().W(in.mix.aS32.SStr32, in.mix.aS32.SStrLen32);
        	break;
		case CdAny::dvtStr8:
			s.Storage().WpUTF8(*in.mix.aR.ptrStr8); break;
		case CdAny::dvtStr16:
			s.Storage().WpUTF16(*in.mix.aR.ptrStr16); break;
		case CdAny::dvtStr32:
			s.Storage().WpUTF32(*in.mix.aR.ptrStr32); break;

		// other extended type
		case CdAny::dvtBoolean:
			s.Storage() << C_UInt8(in.VAL<C_UInt8>() ? 1 : 0); break;

		// pointer
		case CdAny::dvtPointer:
			break;

		// array
		case CdAny::dvtArray:
			s.Storage() << C_UInt32(in.mix.aArray.ArrayLength);
			for (C_UInt32 i=0; i < in.mix.aArray.ArrayLength; i++)
				s << in.mix.aArray.ArrayPtr[i];
			break;

		// CdObjRef
		// TODO:
		case CdAny::dvtObjRef:
			if (in.mix.aR.obj)
			{
				s.Storage() << C_UInt8(1);
				_INTERNAL::CdObject_SaveStruct(*in.mix.aR.obj, s, true);
			} else
				s.Storage() << C_UInt8(0);
			break;
	}
	return s;
}
