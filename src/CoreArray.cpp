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

#include "CoreArray.h"


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

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(SBIT2, CdSBit2, ctArray, "signed integer of  2 bits");
		REG_CLASS(SBIT3, CdSBit3, ctArray, "signed integer of  3 bits");
		REG_CLASS(SBIT4, CdSBit4, ctArray, "signed integer of  4 bits");
		REG_CLASS(SBIT5, CdSBit5, ctArray, "signed integer of  5 bits");
		REG_CLASS(SBIT6, CdSBit6, ctArray, "signed integer of  6 bits");
		REG_CLASS(SBIT7, CdSBit7, ctArray, "signed integer of  7 bits");
	#endif
		REG_CLASS(C_Int8, CdInt8, ctArray, "signed integer of  8 bits");
		REG_CLASS_EX("dSBit8", CdSBit8, ctArray, "signed integer of  8 bits");

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(SBIT9,  CdSBit9,  ctArray, "signed integer of  9 bits");
		REG_CLASS(SBIT10, CdSBit10, ctArray, "signed integer of 10 bits");
		REG_CLASS(SBIT11, CdSBit11, ctArray, "signed integer of 11 bits");
		REG_CLASS(SBIT12, CdSBit12, ctArray, "signed integer of 12 bits");
		REG_CLASS(SBIT13, CdSBit13, ctArray, "signed integer of 13 bits");
		REG_CLASS(SBIT14, CdSBit14, ctArray, "signed integer of 14 bits");
		REG_CLASS(SBIT15, CdSBit15, ctArray, "signed integer of 15 bits");
	#endif
		REG_CLASS(C_Int16, CdInt16, ctArray, "signed integer of 16 bits");
		REG_CLASS_EX("dSBit16", CdSBit16, ctArray, "signed integer of 16 bits");

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(SBIT17, CdSBit17, ctArray, "signed integer of 17 bits");
		REG_CLASS(SBIT18, CdSBit18, ctArray, "signed integer of 18 bits");
		REG_CLASS(SBIT19, CdSBit19, ctArray, "signed integer of 19 bits");
		REG_CLASS(SBIT20, CdSBit20, ctArray, "signed integer of 20 bits");
		REG_CLASS(SBIT21, CdSBit21, ctArray, "signed integer of 21 bits");
		REG_CLASS(SBIT22, CdSBit22, ctArray, "signed integer of 22 bits");
		REG_CLASS(SBIT23, CdSBit23, ctArray, "signed integer of 23 bits");
	#endif
		REG_CLASS(SBIT24, CdSBit24, ctArray, "signed integer of 24 bits");
		REG_CLASS_EX("dInt24", CdSBit24, ctArray, "signed integer of 24 bits");

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(SBIT25, CdSBit17, ctArray, "signed integer of 25 bits");
		REG_CLASS(SBIT26, CdSBit18, ctArray, "signed integer of 26 bits");
		REG_CLASS(SBIT27, CdSBit19, ctArray, "signed integer of 27 bits");
		REG_CLASS(SBIT28, CdSBit20, ctArray, "signed integer of 28 bits");
		REG_CLASS(SBIT29, CdSBit21, ctArray, "signed integer of 29 bits");
		REG_CLASS(SBIT30, CdSBit22, ctArray, "signed integer of 30 bits");
		REG_CLASS(SBIT31, CdSBit23, ctArray, "signed integer of 31 bits");
	#endif
		REG_CLASS(C_Int32, CdInt32, ctArray, "signed integer of 32 bits");
		REG_CLASS_EX("dSBit32", CdSBit32, ctArray, "signed integer of 32 bits");

		REG_CLASS(C_Int64, CdInt64, ctArray, "signed integer of 64 bits");
		REG_CLASS_EX("dSBit64", CdInt64, ctArray, "signed integer of 64 bits");

		// unsigned integer

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(BIT1, CdBit1, ctArray, "unsigned integer of  1 bit");
		REG_CLASS(BIT2, CdBit2, ctArray, "unsigned integer of  2 bits");
		REG_CLASS(BIT3, CdBit3, ctArray, "unsigned integer of  3 bits");
		REG_CLASS(BIT4, CdBit4, ctArray, "unsigned integer of  4 bits");
		REG_CLASS(BIT5, CdBit5, ctArray, "unsigned integer of  5 bits");
		REG_CLASS(BIT6, CdBit6, ctArray, "unsigned integer of  6 bits");
		REG_CLASS(BIT7, CdBit7, ctArray, "unsigned integer of  7 bits");
	#endif
		REG_CLASS(C_UInt8, CdUInt8, ctArray, "unsigned integer of  8 bits");
		REG_CLASS_EX("dBit8", CdBit8, ctArray, "unsigned integer of  8 bits");

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(BIT9,  CdBit9,  ctArray, "unsigned integer of  9 bits");
		REG_CLASS(BIT10, CdBit10, ctArray, "unsigned integer of 10 bits");
		REG_CLASS(BIT11, CdBit11, ctArray, "unsigned integer of 11 bits");
		REG_CLASS(BIT12, CdBit12, ctArray, "unsigned integer of 12 bits");
		REG_CLASS(BIT13, CdBit13, ctArray, "unsigned integer of 13 bits");
		REG_CLASS(BIT14, CdBit14, ctArray, "unsigned integer of 14 bits");
		REG_CLASS(BIT15, CdBit15, ctArray, "unsigned integer of 15 bits");
	#endif
		REG_CLASS(C_UInt16, CdUInt16, ctArray, "unsigned integer of 16 bits");
		REG_CLASS_EX("dBit16", CdBit16, ctArray, "unsigned integer of 16 bits");

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(BIT17, CdBit17, ctArray, "unsigned integer of 17 bits");
		REG_CLASS(BIT18, CdBit18, ctArray, "unsigned integer of 18 bits");
		REG_CLASS(BIT19, CdBit19, ctArray, "unsigned integer of 19 bits");
		REG_CLASS(BIT20, CdBit20, ctArray, "unsigned integer of 20 bits");
		REG_CLASS(BIT21, CdBit21, ctArray, "unsigned integer of 21 bits");
		REG_CLASS(BIT22, CdBit22, ctArray, "unsigned integer of 22 bits");
		REG_CLASS(BIT23, CdBit23, ctArray, "unsigned integer of 23 bits");
	#endif
		REG_CLASS(BIT24, CdBit24, ctArray, "unsigned integer of 24 bits");
		REG_CLASS_EX("dUInt24", CdBit24, ctArray, "unsigned integer of 24 bits");

	#ifndef COREARRAY_NO_BIT_GDSCLASS
		REG_CLASS(BIT25, CdBit25, ctArray, "unsigned integer of 25 bits");
		REG_CLASS(BIT26, CdBit26, ctArray, "unsigned integer of 26 bits");
		REG_CLASS(BIT27, CdBit27, ctArray, "unsigned integer of 27 bits");
		REG_CLASS(BIT28, CdBit28, ctArray, "unsigned integer of 28 bits");
		REG_CLASS(BIT29, CdBit29, ctArray, "unsigned integer of 29 bits");
		REG_CLASS(BIT30, CdBit30, ctArray, "unsigned integer of 30 bits");
		REG_CLASS(BIT31, CdBit31, ctArray, "unsigned integer of 31 bits");
	#endif
		REG_CLASS(C_UInt32, CdUInt32, ctArray, "unsigned integer of 32 bits");
		REG_CLASS_EX("dBit32", CdBit32, ctArray, "unsigned integer of 32 bits");

		REG_CLASS(C_UInt64, CdUInt64, ctArray, "unsigned integer of 64 bits");
		REG_CLASS_EX("dBit64", CdUInt64, ctArray, "unsigned integer of 64 bits");

		// float
		REG_CLASS(C_Float32, CdFloat32, ctArray, "floating-point number (32 bits)");
		REG_CLASS(C_Float64, CdFloat64, ctArray, "floating-point number (64 bits)");

		// fixed-length string
		REG_CLASS(FIXED_LENGTH<C_UTF8>,  CdFStr8,  ctArray, "fixed-length UTF-8 string");
		REG_CLASS(FIXED_LENGTH<C_UTF16>, CdFStr16, ctArray, "fixed-length UTF-16 string");
		REG_CLASS(FIXED_LENGTH<C_UTF32>, CdFStr32, ctArray, "fixed-length UTF-32 string");

		// variable-length string
		REG_CLASS(VARIABLE_LENGTH<C_UTF8>,  CdVStr8,  ctArray, "variable-length UTF-8 string");
		REG_CLASS(VARIABLE_LENGTH<C_UTF16>, CdVStr16, ctArray, "variable-length UTF-16 string");
		REG_CLASS(VARIABLE_LENGTH<C_UTF32>, CdVStr32, ctArray, "variable-length UTF-32 string");

		// stream container
		dObjManager().AddClass("dStream", OnObjCreate<CdGDSStreamContainer>,
			CdObjClassMgr::ctStream, "Stream Container");

		CoreArray_ifRegClass = true;

		#undef REG_CLASS
		#undef REG_CLASS_EX
	}
}
