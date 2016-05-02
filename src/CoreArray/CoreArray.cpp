// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// CoreArray.cpp: CoreArray interface
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

#include "CoreArray.h"


namespace CoreArray
{
	static bool CoreArray_ifRegClass = false;


	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	COREARRAY_DLL_DEFAULT void RegisterClass()
	{
		if (CoreArray_ifRegClass) return;

		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)
		#define REG_CLASS_EX(Name, CLASS, CType, Desp)	\
			dObjManager().AddClass(Name, OnObjCreate< CLASS >, \
				CdObjClassMgr::CType, Desp)

		// primitive integer
		REG_CLASS(C_Int8, CdInt8, ctArray, "signed integer of 8 bits");
		REG_CLASS(C_Int16, CdInt16, ctArray, "signed integer of 16 bits");
		REG_CLASS(SBIT24, CdSBit24, ctArray, "signed integer of 24 bits");
		REG_CLASS(C_Int32, CdInt32, ctArray, "signed integer of 32 bits");
		REG_CLASS(C_Int64, CdInt64, ctArray, "signed integer of 64 bits");

		REG_CLASS(C_UInt8, CdUInt8, ctArray, "unsigned integer of 8 bits");
		REG_CLASS(C_UInt16, CdUInt16, ctArray, "unsigned integer of 16 bits");
		REG_CLASS(BIT24, CdBit24, ctArray, "unsigned integer of 24 bits");
		REG_CLASS(C_UInt32, CdUInt32, ctArray, "unsigned integer of 32 bits");
		REG_CLASS(C_UInt64, CdUInt64, ctArray, "unsigned integer of 64 bits");

		// signed bit integer

		REG_CLASS(SBIT2, CdSBit2, ctArray, "signed integer of 2 bits");
		REG_CLASS(SBIT3, CdSBit3, ctArray, "signed integer of 3 bits");
		REG_CLASS(SBIT4, CdSBit4, ctArray, "signed integer of 4 bits");
		REG_CLASS(SBIT5, CdSBit5, ctArray, "signed integer of 5 bits");
		REG_CLASS(SBIT6, CdSBit6, ctArray, "signed integer of 6 bits");
		REG_CLASS(SBIT7, CdSBit7, ctArray, "signed integer of 7 bits");
		REG_CLASS_EX("dSBit8", CdSBit8, ctArray, "signed integer of 8 bits");

		REG_CLASS(SBIT9,  CdSBit9,  ctArray, "signed integer of 9 bits");
		REG_CLASS(SBIT10, CdSBit10, ctArray, "signed integer of 10 bits");
		REG_CLASS(SBIT11, CdSBit11, ctArray, "signed integer of 11 bits");
		REG_CLASS(SBIT12, CdSBit12, ctArray, "signed integer of 12 bits");
		REG_CLASS(SBIT13, CdSBit13, ctArray, "signed integer of 13 bits");
		REG_CLASS(SBIT14, CdSBit14, ctArray, "signed integer of 14 bits");
		REG_CLASS(SBIT15, CdSBit15, ctArray, "signed integer of 15 bits");

		REG_CLASS_EX("dSBit16", CdSBit16, ctArray, "signed integer of 16 bits");
		REG_CLASS_EX("dSBit24", CdSBit24, ctArray, "signed integer of 24 bits");
		REG_CLASS_EX("dSBit32", CdSBit32, ctArray, "signed integer of 32 bits");
		REG_CLASS_EX("dSBit64", CdInt64, ctArray, "signed integer of 64 bits");

		// unsigned bit integer

		REG_CLASS(BIT1, CdBit1, ctArray, "unsigned integer of 1 bit");
		REG_CLASS(BIT2, CdBit2, ctArray, "unsigned integer of 2 bits");
		REG_CLASS(BIT3, CdBit3, ctArray, "unsigned integer of 3 bits");
		REG_CLASS(BIT4, CdBit4, ctArray, "unsigned integer of 4 bits");
		REG_CLASS(BIT5, CdBit5, ctArray, "unsigned integer of 5 bits");
		REG_CLASS(BIT6, CdBit6, ctArray, "unsigned integer of 6 bits");
		REG_CLASS(BIT7, CdBit7, ctArray, "unsigned integer of 7 bits");
		REG_CLASS_EX("dBit8", CdBit8, ctArray, "unsigned integer of 8 bits");

		REG_CLASS(BIT9,  CdBit9,  ctArray, "unsigned integer of 9 bits");
		REG_CLASS(BIT10, CdBit10, ctArray, "unsigned integer of 10 bits");
		REG_CLASS(BIT11, CdBit11, ctArray, "unsigned integer of 11 bits");
		REG_CLASS(BIT12, CdBit12, ctArray, "unsigned integer of 12 bits");
		REG_CLASS(BIT13, CdBit13, ctArray, "unsigned integer of 13 bits");
		REG_CLASS(BIT14, CdBit14, ctArray, "unsigned integer of 14 bits");
		REG_CLASS(BIT15, CdBit15, ctArray, "unsigned integer of 15 bits");

		REG_CLASS_EX("dBit16", CdBit16, ctArray, "unsigned integer of 16 bits");
		REG_CLASS_EX("dBit24", CdBit24, ctArray, "unsigned integer of 24 bits");
		REG_CLASS_EX("dBit32", CdBit32, ctArray, "unsigned integer of 32 bits");
		REG_CLASS_EX("dBit64", CdUInt64, ctArray, "unsigned integer of 64 bits");

		// real number
		REG_CLASS(C_Float32, CdFloat32, ctArray, "floating-point number (32 bits)");
		REG_CLASS(C_Float64, CdFloat64, ctArray, "floating-point number (64 bits)");
		REG_CLASS(TREAL8,  CdPackedReal8,  ctArray, "packed real number (8 bits)");
		REG_CLASS(TREAL16, CdPackedReal16, ctArray, "packed real number (16 bits)");
		REG_CLASS(TREAL24, CdPackedReal24, ctArray, "packed real number (24 bits)");
		REG_CLASS(TREAL32, CdPackedReal32, ctArray, "packed real number (32 bits)");

		// fixed-length string
		REG_CLASS(FIXED_LEN<C_UTF8>,  CdFStr8,  ctArray, "fixed-length UTF-8 string");
		REG_CLASS(FIXED_LEN<C_UTF16>, CdFStr16, ctArray, "fixed-length UTF-16 string");
		REG_CLASS(FIXED_LEN<C_UTF32>, CdFStr32, ctArray, "fixed-length UTF-32 string");

		// variable-length null-terminated string
		REG_CLASS(C_STRING<C_UTF8>,  CdVStr8,  ctArray, "UTF-8 null-terminated string");
		REG_CLASS(C_STRING<C_UTF16>, CdVStr16, ctArray, "UTF-16 null-terminated string");
		REG_CLASS(C_STRING<C_UTF32>, CdVStr32, ctArray, "UTF-32 null-terminated string");

		// variable-length string allowing null character
		REG_CLASS(VARIABLE_LEN<C_UTF8>,  CdStr8,  ctArray, "variable-length UTF-8 string");
		REG_CLASS(VARIABLE_LEN<C_UTF16>, CdStr16, ctArray, "variable-length UTF-16 string");
		REG_CLASS(VARIABLE_LEN<C_UTF32>, CdStr32, ctArray, "variable-length UTF-32 string");

		// stream container
		dObjManager().AddClass("dStream", OnObjCreate<CdGDSStreamContainer>,
			CdObjClassMgr::ctStream, "stream container");

		CoreArray_ifRegClass = true;

		#undef REG_CLASS
		#undef REG_CLASS_EX
	}
}
