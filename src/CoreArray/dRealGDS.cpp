// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dRealGDS.cpp: Packed real number in GDS format
//
// Copyright (C) 2018-2019    Xiuwen Zheng
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

#if defined(__clang__)
#pragma clang optimize on
#elif defined(__GNUC__) && ((__GNUC__>4) || (__GNUC__==4 && __GNUC_MINOR__>=4))
#pragma GCC optimize("O3")
#endif

#include "dRealGDS.h"


namespace CoreArray
{
	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	COREARRAY_DLL_LOCAL void RegisterClass_PackedReal()
	{
		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)

		// real numbers
		REG_CLASS(TReal8,   CdPackedReal8,   ctArray, "packed real number (signed 8 bits)");
		REG_CLASS(TReal8u,  CdPackedReal8U,  ctArray, "packed real number (unsigned 8 bits)");
		REG_CLASS(TReal16,  CdPackedReal16,  ctArray, "packed real number (signed 16 bits)");
		REG_CLASS(TReal16u, CdPackedReal16U, ctArray, "packed real number (unsigned 16 bits)");
		REG_CLASS(TReal24,  CdPackedReal24,  ctArray, "packed real number (signed 24 bits)");
		REG_CLASS(TReal24u, CdPackedReal24U, ctArray, "packed real number (unsigned 24 bits)");
		REG_CLASS(TReal32,  CdPackedReal32,  ctArray, "packed real number (signed 32 bits)");
		REG_CLASS(TReal32u, CdPackedReal32U, ctArray, "packed real number (unsigned 32 bits)");

		#undef REG_CLASS
	}
}
