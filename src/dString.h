// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dString.h: Character type and operation
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

/**
 *	\file     dString.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Character type and operation
 *	\details
**/


#ifndef _HEADER_COREARRAY_STRING_
#define _HEADER_COREARRAY_STRING_

#include <CoreDEF.h>
#include <dType.h>
#include <dTrait.h>


namespace CoreArray
{
	// String

	/// UTF-8 character
	typedef char UTF8;
	//  UTF-16 character
	typedef C_Int16 UTF16;
	/// UTF-32 character
	typedef C_Int32 UTF32;

	/// UTF-8 string
	typedef std::string                 UTF8String;
	/// UTF-16 string
	typedef std::basic_string<UTF16>    UTF16String;
	/// UTF-32 string
	typedef std::basic_string<UTF32>    UTF32String;


	// String Traits

	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF8String>
	{
		typedef UTF8String TType;
		typedef UTF8 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 8u;
		static const bool isClass = true;
		static const C_SVType SVType = svStrUTF8;

		static const char * TraitName() { return "Str8"; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF8*>
	{
		typedef UTF8String TType;
		typedef UTF8 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 8u;
		static const bool isClass = false;
		static const C_SVType SVType = svStrUTF8;

		static const char * TraitName() { return "Str8"; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF16String>
	{
		typedef UTF16String TType;
		typedef UTF16 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 16u;
		static const bool isClass = true;
		static const C_SVType SVType = svStrUTF16;

		static const char * TraitName() { return "Str16"; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF16*>
	{
		typedef UTF16String TType;
		typedef UTF16 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 16u;
		static const bool isClass = false;
		static const C_SVType SVType = svStrUTF16;

		static const char * TraitName() { return "Str16"; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF32String>
	{
		typedef UTF32String TType;
		typedef UTF32 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 32u;
		static const bool isClass = true;
		static const C_SVType SVType = svCustomStr;

		static const char * TraitName() { return "Str32"; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits<UTF32*>
	{
		typedef UTF32String TType;
		typedef UTF32 ElmType;
		static const int trVal = COREARRAY_TR_STRING;
		static const unsigned BitOf = 32u;
		static const bool isClass = false;
		static const C_SVType SVType = svCustomStr;

		static const char * TraitName() { return "Str32"; }
	};
}

#endif /* _HEADER_COREARRAY_STRING_ */
