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

#include <CoreArray.h>


namespace CoreArray
{
	static bool CoreArray_ifRegClass = false;


	template<typename TClass> static CdObjRef * OnObjCreate()
	{
		return new TClass();
	}

	void RegisterClass()
	{
		if (CoreArray_ifRegClass) return;

		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)
		#define REG_CLASS_EX(Name, CLASS, CType, Desp)	\
			dObjManager().AddClass(Name, OnObjCreate< CLASS >, \
				CdObjClassMgr::CType, Desp)

		// signed integer

	#ifndef COREARRAY_LIMIT
		REG_CLASS(SBit2, CdSBit2, ctArray, "signed integer of  2 bits");
		REG_CLASS(SBit3, CdSBit3, ctArray, "signed integer of  3 bits");
		REG_CLASS(SBit4, CdSBit4, ctArray, "signed integer of  4 bits");
		REG_CLASS(SBit5, CdSBit5, ctArray, "signed integer of  5 bits");
		REG_CLASS(SBit6, CdSBit6, ctArray, "signed integer of  6 bits");
		REG_CLASS(SBit7, CdSBit7, ctArray, "signed integer of  7 bits");
	#endif
		REG_CLASS(C_Int8, CdInt8, ctArray, "signed integer of  8 bits");
		REG_CLASS_EX("dSBit8", CdSBit8, ctArray, "signed integer of  8 bits");

	#ifndef COREARRAY_LIMIT
		REG_CLASS(SBit9,  CdSBit9,  ctArray, "signed integer of  9 bits");
		REG_CLASS(SBit10, CdSBit10, ctArray, "signed integer of 10 bits");
		REG_CLASS(SBit11, CdSBit11, ctArray, "signed integer of 11 bits");
		REG_CLASS(SBit12, CdSBit12, ctArray, "signed integer of 12 bits");
		REG_CLASS(SBit13, CdSBit13, ctArray, "signed integer of 13 bits");
		REG_CLASS(SBit14, CdSBit14, ctArray, "signed integer of 14 bits");
		REG_CLASS(SBit15, CdSBit15, ctArray, "signed integer of 15 bits");
	#endif
		REG_CLASS(C_Int16, CdInt16, ctArray, "signed integer of 16 bits");
		REG_CLASS_EX("dSBit16", CdSBit16, ctArray, "signed integer of 16 bits");

		REG_CLASS(SBit24, CdSBit24, ctArray, "signed integer of 24 bits");
		REG_CLASS_EX("dInt24", CdSBit24, ctArray, "signed integer of 24 bits");

		REG_CLASS(C_Int32, CdInt32, ctArray, "signed integer of 32 bits");
		REG_CLASS_EX("dSBit32", CdSBit32, ctArray, "signed integer of 32 bits");

		REG_CLASS(C_Int64, CdInt64, ctArray, "signed integer of 64 bits");

		// unsigned integer

		REG_CLASS(Bit1, CdBit1, ctArray, "unsigned integer of  1 bit");
		REG_CLASS(Bit2, CdBit2, ctArray, "unsigned integer of  2 bits");
		REG_CLASS(Bit3, CdBit3, ctArray, "unsigned integer of  3 bits");
		REG_CLASS(Bit4, CdBit4, ctArray, "unsigned integer of  4 bits");
		REG_CLASS(Bit5, CdBit5, ctArray, "unsigned integer of  5 bits");
		REG_CLASS(Bit6, CdBit6, ctArray, "unsigned integer of  6 bits");
		REG_CLASS(Bit7, CdBit7, ctArray, "unsigned integer of  7 bits");
		REG_CLASS(C_UInt8, CdUInt8, ctArray, "unsigned integer of  8 bits");
		REG_CLASS_EX("dBit8", CdBit8, ctArray, "unsigned integer of  8 bits");

	#ifndef COREARRAY_LIMIT
		REG_CLASS(Bit9,  CdBit9,  ctArray, "unsigned integer of  9 bits");
		REG_CLASS(Bit10, CdBit10, ctArray, "unsigned integer of 10 bits");
		REG_CLASS(Bit11, CdBit11, ctArray, "unsigned integer of 11 bits");
		REG_CLASS(Bit12, CdBit12, ctArray, "unsigned integer of 12 bits");
		REG_CLASS(Bit13, CdBit13, ctArray, "unsigned integer of 13 bits");
		REG_CLASS(Bit14, CdBit14, ctArray, "unsigned integer of 14 bits");
		REG_CLASS(Bit15, CdBit15, ctArray, "unsigned integer of 15 bits");
	#endif
		REG_CLASS(C_UInt16, CdUInt16, ctArray, "unsigned integer of 16 bits");
		REG_CLASS_EX("dBit16", CdBit16, ctArray, "unsigned integer of 16 bits");

		REG_CLASS(Bit24, CdBit24, ctArray, "unsigned integer of 24 bits");
		REG_CLASS_EX("dUInt24", CdBit24, ctArray, "unsigned integer of 24 bits");

		REG_CLASS(C_UInt32, CdUInt32, ctArray, "unsigned integer of 32 bits");
		REG_CLASS_EX("dBit32", CdBit32, ctArray, "unsigned integer of 32 bits");
		REG_CLASS(C_UInt64, CdUInt64, ctArray, "unsigned integer of 64 bits");

		// float
		REG_CLASS(C_Float32, CdFloat32, ctArray, "float  32");
		REG_CLASS(C_Float64, CdFloat64, ctArray, "float  64");
	#ifndef COREARRAY_NO_EXTENDED_TYPES
		REG_CLASS(Float128, CdFloat128, ctArray, "float 128");
	#endif

		// string
		REG_CLASS(FIXED_LENGTH<UTF8*>,  CdFStr8, ctArray,  "Fixed-length UTF-8 string");
		REG_CLASS(FIXED_LENGTH<UTF16*>, CdFStr16, ctArray, "Fixed-length UTF-16 string");
		REG_CLASS(FIXED_LENGTH<UTF32*>, CdFStr32, ctArray, "Fixed-length UTF-32 string");

		// string
		REG_CLASS(VARIABLE_LENGTH<UTF8*>,  CdVStr8, ctArray,  "Variable-length UTF-8 string");
		REG_CLASS(VARIABLE_LENGTH<UTF16*>, CdVStr16, ctArray, "Variable-length UTF-16 string");
		REG_CLASS(VARIABLE_LENGTH<UTF32*>, CdVStr32, ctArray, "Variable-length UTF-32 string");

		// stream container
		dObjManager().AddClass("dStream", OnObjCreate<CdGDSStreamContainer>,
			CdObjClassMgr::ctStream, "Stream Container");

		CoreArray_ifRegClass = true;

		#undef REG_CLASS
		#undef REG_CLASS_EX
	}
}
