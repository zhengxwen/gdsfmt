// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStrGDS.cpp: GDS format with character types and functions
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

#include "dStrGDS.h"


namespace CoreArray
{
	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	COREARRAY_DLL_LOCAL void RegisterClass_String()
	{
		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)

		// fixed-length strings
		REG_CLASS(FIXED_LEN<C_UTF8>,  CdFStr8,  ctArray, "fixed-length UTF-8 string");
		REG_CLASS(FIXED_LEN<C_UTF16>, CdFStr16, ctArray, "fixed-length UTF-16 string");
		REG_CLASS(FIXED_LEN<C_UTF32>, CdFStr32, ctArray, "fixed-length UTF-32 string");

		// variable-length null-terminated strings
		REG_CLASS(C_STRING<C_UTF8>,  CdVStr8,  ctArray, "UTF-8 null-terminated string");
		REG_CLASS(C_STRING<C_UTF16>, CdVStr16, ctArray, "UTF-16 null-terminated string");
		REG_CLASS(C_STRING<C_UTF32>, CdVStr32, ctArray, "UTF-32 null-terminated string");

		// variable-length strings allowing null character
		REG_CLASS(VARIABLE_LEN<C_UTF8>,  CdStr8,  ctArray, "variable-length UTF-8 string");
		REG_CLASS(VARIABLE_LEN<C_UTF16>, CdStr16, ctArray, "variable-length UTF-16 string");
		REG_CLASS(VARIABLE_LEN<C_UTF32>, CdStr32, ctArray, "variable-length UTF-32 string");

		#undef REG_CLASS
	}
}
