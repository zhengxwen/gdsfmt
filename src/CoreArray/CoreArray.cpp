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
// Copyright (C) 2007-2018    Xiuwen Zheng
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

#include "CoreDEF.h"
#include "dType.h"
#include "dTrait.h"
#include "dPlatform.h"
#include "dParallel.h"
#include "dStream.h"
#include "dEndian.h"
#include "dSerial.h"
#include "dBase.h"
#include "dFile.h"
#include "dStruct.h"


namespace CoreArray
{
	static bool CoreArray_ifRegClass = false;


	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	extern COREARRAY_DLL_LOCAL void RegisterClass_Bit_s24();
	extern COREARRAY_DLL_LOCAL void RegisterClass_Bit_u24();
	extern COREARRAY_DLL_LOCAL void RegisterClass_Bit();
	extern COREARRAY_DLL_LOCAL void RegisterClass_VLInt();
	extern COREARRAY_DLL_LOCAL void RegisterClass_PackedReal();
	extern COREARRAY_DLL_LOCAL void RegisterClass_String();


	COREARRAY_DLL_DEFAULT void RegisterClass()
	{
		if (CoreArray_ifRegClass) return;

		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)
		#define REG_CLASS_EX(Name, CLASS, CType, Desp)	\
			dObjManager().AddClass(Name, OnObjCreate< CLASS >, \
				CdObjClassMgr::CType, Desp)

		// primitive integers
		REG_CLASS(C_Int8, CdInt8, ctArray, "signed integer of 8 bits");
		REG_CLASS(C_Int16, CdInt16, ctArray, "signed integer of 16 bits");
		RegisterClass_Bit_s24();
		REG_CLASS(C_Int32, CdInt32, ctArray, "signed integer of 32 bits");
		REG_CLASS(C_Int64, CdInt64, ctArray, "signed integer of 64 bits");

		REG_CLASS(C_UInt8, CdUInt8, ctArray, "unsigned integer of 8 bits");
		REG_CLASS(C_UInt16, CdUInt16, ctArray, "unsigned integer of 16 bits");
		RegisterClass_Bit_u24();
		REG_CLASS(C_UInt32, CdUInt32, ctArray, "unsigned integer of 32 bits");
		REG_CLASS(C_UInt64, CdUInt64, ctArray, "unsigned integer of 64 bits");

		// signed and unsigned bit integers
		RegisterClass_Bit();

		// variable-length integers
		RegisterClass_VLInt();

		// real numbers
		REG_CLASS(C_Float32, CdFloat32, ctArray, "floating-point number (32 bits)");
		REG_CLASS(C_Float64, CdFloat64, ctArray, "floating-point number (64 bits)");
		RegisterClass_PackedReal();

		// fixed-length strings
		// variable-length null-terminated strings
		// variable-length strings allowing null character
		RegisterClass_String();

		// stream container
		dObjManager().AddClass("dStream", OnObjCreate<CdGDSStreamContainer>,
			CdObjClassMgr::ctStream, "stream container");

		CoreArray_ifRegClass = true;

		#undef REG_CLASS
		#undef REG_CLASS_EX
	}
}
