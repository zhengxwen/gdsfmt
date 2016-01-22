// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dAny.h: Methods for CdAny
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

/**
 *	\file     dAny.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2016
 *	\brief    Basic classes for CoreArray library
 *	\details
**/


#ifndef _HEADER_COREARRAY_ANY_
#define _HEADER_COREARRAY_ANY_

#include "dBase.h"
#include "dSerial.h"
#include "dAllocator.h"


namespace CoreArray
{
	/// Generic data type
	struct COREARRAY_DLL_DEFAULT CdAny
	{
	public:
		friend CdReader& operator>> (CdReader &s, CdAny &out);
		friend CdWriter& operator<< (CdWriter &s, CdAny &in);

		/// Data type for CdAny
		typedef C_UInt8 TDataType;

		/// Type ID
		enum {
			dvtAtomicBegin = 0,    ///< the first atomic type

			dvtNULL        = 0,    ///< NULL type

			// integer
			dvtInt8        = 1,    ///< Signed integer of 8 bits
			dvtUInt8       = 2,    ///< Unsigned integer of 8 bits
			dvtInt16       = 3,    ///< Signed integer of 16 bits
			dvtUInt16      = 4,    ///< Unsigned integer of 16 bits
			dvtInt32       = 5,    ///< Signed integer of 32 bits
			dvtUInt32      = 6,    ///< Unsigned integer of 32 bits
			dvtInt64       = 7,    ///< Signed integer of 64 bits
			dvtUInt64      = 8,    ///< Unsigned integer of 64 bits

			// be prepared for future use
			// dvtInt128      = 9,    ///< Signed integer of 128 bits
			// dvtUInt128     = 10,   ///< Unsigned integer of 128 bits

			// float number
			dvtFloat32     = 11,   ///< Float number of single precision (32 bits)
			dvtFloat64     = 12,   ///< Float number of double precision (64 bits)
			// be prepared for future use
			// dvtFloat128    = 13,   ///< Float number of quadruple precision (128 bits)

			// string
			dvtSString8    = 14,   ///< Short UTF-8 string, the length <= 22
			dvtSString16   = 15,   ///< Short UTF-16 string, the length <= 11
			dvtSString32   = 16,   ///< Short UTF-32 string, the length <= 5
			dvtStr8        = 17,   ///< UTF-8 string
			dvtStr16       = 18,   ///< UTF-16 string
			dvtStr32       = 19,   ///< UTF-32 string

			// other extended type
			dvtBoolean     = 20,   ///< Boolean type
			dvtAtomicLast  = 20,   ///< the last atomic type

			// pointer
			dvtExtFirst    = 32,   ///< the first extended type
			dvtPointer     = 32,   ///< pointer
			dvtArray       = 33,   ///< array of CdAny, include a length
			dvtObjRef      = 34,   ///< TdObjRef object
			dvtExtLast     = 34    ///< the last extended type
		};

		/// Initialize CdAny, clear CdAny with ZERO
		CdAny();
		/// Free CdAny
		~CdAny();

		/// Return type ID
		COREARRAY_INLINE TDataType Type() const { return dsType; }

		/// Type ID of CdAny to a name
		static const char *dvtNames(int index);


		// integer
		C_Int8 GetInt8() const;       ///< get C_Int8, throw an exception if fail
		C_UInt8 GetUInt8() const;     ///< get C_UInt8, throw an exception if fail
		C_Int16 GetInt16() const;     ///< get C_Int16, throw an exception if fail
		C_UInt16 GetUInt16() const;   ///< get C_UInt16, throw an exception if fail
		C_Int32 GetInt32() const;     ///< get C_Int32, throw an exception if fail
		C_UInt32 GetUInt32() const;   ///< get C_UInt32, throw an exception if fail
		C_Int64 GetInt64() const;     ///< get C_Int64, throw an exception if fail
		C_UInt64 GetUInt64() const;   ///< get C_UInt64, throw an exception if fail

		// float number
		C_Float32 GetFloat32() const;   ///< get C_Float32, throw an exception if fail
		C_Float64 GetFloat64() const;   ///< get C_Float64, throw an exception if fail

		// string
		UTF8String GetStr8() const;   ///< get UTF8String, throw an exception if fail
		UTF16String GetStr16() const; ///< get UTF16String, throw an exception if fail
		UTF32String GetStr32() const; ///< get UTF32String, throw an exception if fail

		// other extended type
		bool GetBool() const;  ///< get boolean, throw an exception if fail

		// pointer
		const void *GetPtr() const;  ///< get a pointer, throw an exception if fail

		// array
		CdAny *GetArray() const;  ///< get a pointer
		C_UInt32 GetArrayLength() const;   ///< get the length of array

		// others
		CdObjRef* GetObj() const; ///< get CdObjRef, throw an exception if fail


		/// define a generic 'Get' method
		template<typename FUNC, typename TYPE, TYPE DEFAULT>
			TYPE Get()
		{
			try {
				return FUNC();
			} catch (ErrCoreArray &E) {
				return DEFAULT;
			}
		}


		void SetEmpty();    ///< set Empty
		// integer
		void SetInt8(const C_Int8 val);       ///< set C_Int8
		void SetUInt8(const C_UInt8 val);     ///< set C_UInt8
		void SetInt16(const C_Int16 val);     ///< set C_Int16
		void SetUInt16(const C_UInt16 val);   ///< set C_UInt16
		void SetInt32(const C_Int32 val);     ///< set C_Int32
		void SetUInt32(const C_UInt32 val);   ///< set C_UInt32
		void SetInt64(const C_Int64 val);     ///< set C_Int64
		void SetUInt64(const C_UInt64 val);   ///< set C_UInt64

		// float number
		void SetFloat32(const C_Float32 val);   ///< set C_Float32
		void SetFloat64(const C_Float64 val);   ///< set C_Float64

		// string
		void SetStr8(const UTF8String &val);   ///< set UTF8String
		void SetStr16(const UTF16String &val); ///< set UTF16String
		void SetStr32(const UTF32String &val); ///< set UTF32String

		// other extended type
		void SetBool(const bool val);     ///< set boolean

		// pointer
		void SetPtr(const void *ptr);  ///< set a pointer

		// array
		void SetArray(C_UInt32 size);  ///< set an array
		void SetArray(const C_Int32 ptr[], C_UInt32 size);  ///< set an int32 array
		void SetArray(const C_Int64 ptr[], C_UInt32 size);  ///< set an int64 array
		void SetArray(const C_Float64 ptr[], C_UInt32 size);  ///< set a double array
		void SetArray(const char* const ptr[], C_UInt32 size);  ///< set a string array
		void SetArray(const std::string ptr[], C_UInt32 size);  ///< set a string array
		void SetArray(const bool ptr[], C_UInt32 size);  ///< set a boolean array

		// CdObjRef
		void SetObj(CdObjRef *obj); ///< set a CdObjRef object

		/// auto determine data type
		void Assign(const UTF8String &val);

		// check data type
		bool IsNULL() const;  ///< return true, if it is dvtNULL
		bool IsInt() const;    ///< return true, if it is an integer
		bool IsFloat() const;  ///< return true, if it is a float number
		bool IsNumeric() const;    ///< return true, if it is an integer or float number
		bool IsBool() const;   ///< return true, if it is dvtBoolean
		bool IsNaN() const;    ///< return true, if it is a NaN, or not a number
		bool IsNA() const;     ///< return true, if it is a NaN, or dvtNULL
		bool IsString() const;    ///< return true, if it is a string
		bool IsArray() const;  ///< return true, if it is array-type

		bool Packed();  ///< pack the data, if packed return true
		void Swap(CdAny &D); ///< swap *this and D

		/// compare *this and D (CdAny)
		/** \param D       a CdAny data
		 *  \param NALast  NA value will be the last
		 *
		 *  return  1, if *this > D;
		 *  return  0, if *this == D;
         *  return -1, if *this < D
		**/
		int Compare(const CdAny &D, bool NALast = true);


		// operator
        COREARRAY_INLINE operator C_Int8() { return GetInt8(); }
        COREARRAY_INLINE operator C_UInt8() { return GetUInt8(); }
		COREARRAY_INLINE operator C_Int16() { return GetInt16(); }
		COREARRAY_INLINE operator C_UInt16() { return GetUInt16(); }
		COREARRAY_INLINE operator C_Int32() { return GetInt32(); }
		COREARRAY_INLINE operator C_UInt32() { return GetUInt32(); }
		COREARRAY_INLINE operator C_Int64() { return GetInt64(); }
		COREARRAY_INLINE operator C_UInt64() { return GetUInt64(); }

		COREARRAY_INLINE operator C_Float32() { return GetFloat32(); }
		COREARRAY_INLINE operator C_Float64() { return GetFloat64(); }

		COREARRAY_INLINE operator UTF8String() { return GetStr8(); }
		COREARRAY_INLINE operator UTF16String() { return GetStr16(); }
		COREARRAY_INLINE operator UTF32String() { return GetStr32(); }

		/// an operator of assignment
		CdAny & operator= (const CdAny &_Right);
		CdAny & operator= (const C_Int8 val) { SetInt8(val); return *this; }
		CdAny & operator= (const C_UInt8 val) { SetUInt8(val); return *this; }
		CdAny & operator= (const C_Int16 val) { SetInt16(val); return *this; }
		CdAny & operator= (const C_UInt16 val) { SetUInt16(val); return *this; }
		CdAny & operator= (const C_Int32 val) { SetInt32(val); return *this; }
		CdAny & operator= (const C_UInt32 val) { SetUInt32(val); return *this; }
		CdAny & operator= (const C_Int64 val) { SetInt64(val); return *this; }
		CdAny & operator= (const C_UInt64 val) { SetUInt64(val); return *this; }

		CdAny & operator= (const C_Float32 val) { SetFloat32(val); return *this; }
		CdAny & operator= (const C_Float64 val) { SetFloat64(val); return *this; }

		CdAny & operator= (const UTF8String &val) { SetStr8(val); return *this; }
		CdAny & operator= (const UTF16String &val) { SetStr16(val); return *this; }
		CdAny & operator= (const UTF32String &val) { SetStr32(val); return *this; }

		CdAny & operator= (const void *val) { SetPtr(val); return *this; }

	private:

		TDataType dsType;
		union TUnion {
			struct TaR {
				C_UInt8 Reserved[7];
				union {  // 8-byte aligned
					void *Align;
					UTF8String *ptrStr8;
					UTF16String *ptrStr16;
					UTF32String *ptrStr32;
					CdObjRef *obj;
					const void *const_ptr;
				};
			} aR;
			struct TaS8 {
				C_UInt8 SStrLen8;
				C_UTF8 SStr8[22];
			} aS8;
			struct TaS16 {
				C_UInt8 SStrLen16;
				C_UTF16 SStr16[11];
			} aS16;
			struct TaS32 {
				C_UInt8 Reserved1, SStrLen32, Reserved2;
				C_UTF32 SStr32[5];
			} aS32;
			struct TaArray {
				TDataType dsArray;
				C_UInt8 ReservedArray1, ReservedArray2;
				C_UInt32 ArrayLength;
				CdAny *ArrayPtr;
			} aArray;
		} mix;

		template<typename TYPE> TYPE & VAL()
			{
				void *tmp = &mix.aR.Align;
				return *static_cast<TYPE*>(tmp);
			}
		template<typename TYPE> const TYPE & VAL() const
			{
				const void *tmp = &mix.aR.Align;
				return *static_cast<const TYPE*>(tmp);
			}

		void _Done();
	};

	/// an operator, to read CdAny from a filter, little endian
	COREARRAY_DLL_DEFAULT CdReader& operator>> (CdReader &s, CdAny& out);
	/// an operator, to write CdAny to a filter, little endian
	COREARRAY_DLL_DEFAULT CdWriter& operator<< (CdWriter &s, CdAny &in);


	/// Exception for CdAny
	class COREARRAY_DLL_EXPORT Err_dsAny: public ErrConvert
	{
	public:
		Err_dsAny(): ErrConvert()
			{ }
		Err_dsAny(const char *fmt, ...): ErrConvert()
			{ _COREARRAY_ERRMACRO_(fmt); }
		Err_dsAny(CdAny::TDataType fromType, CdAny::TDataType toType);
	};
}

#endif /* _HEADER_COREARRAY_ANY_ */
