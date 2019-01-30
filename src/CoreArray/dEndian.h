// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dEndian.h: Cross-platform functions with independent endianness
//
// Copyright (C) 2007-2017    Xiuwen Zheng
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
 *	\file     dEndian.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007-2017
 *	\brief    Endianness-independent functions
 *	\details
**/

#ifndef _HEADER_COREARRAY_ENDIAN_
#define _HEADER_COREARRAY_ENDIAN_

#include "dPlatform.h"
#include "dTrait.h"


namespace CoreArray
{
	using namespace std;


	// =====================================================================
	// Bit stream object (little endianness)
	// =====================================================================

	/// Bit stream with reading with little endianness
	/** \tparam TYPE    it could be CdStream, CdBufStream or CdAllocator
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT BIT_LE_R
	{
	public:
		TYPE *Stream;
		C_UInt8 Reminder, Offset;

		BIT_LE_R() { Stream = NULL; }
		BIT_LE_R(TYPE *s) { Stream = s; Reminder = Offset = 0; }
		BIT_LE_R(TYPE &s) { Stream = &s; Reminder = Offset = 0; }

		/// read an integer with 'nbit' bits
		C_UInt32 ReadBit(C_UInt8 nbit)
		{
			C_UInt8 shl = 0;
			C_UInt32 rv = 0;
			while (nbit > 0)
			{
				if (Offset == 0) Reminder = Stream->R8b();
				C_UInt8 I = 8 - Offset;
				if (I > nbit) I = nbit;
				C_UInt32 Mask = ~(0xFFFFFFFF << I);
				rv |= (C_UInt32(Reminder >> Offset) & Mask) << shl;
				shl += I; Offset += I;
				if (Offset > 7) Offset = 0;
				nbit -= I;
			}
			return rv;
		}

		void SkipBit(C_UInt8 nbit)
		{
			ReadBit(nbit);
		}
	};


	/// Bit stream with writing with little endianness
	/** \tparam TYPE    it could be CdStream, CdBufStream or CdAllocator
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT BIT_LE_W
	{
	public:
		TYPE *Stream;
		C_UInt8 Reminder, Offset;

		BIT_LE_W() { Stream = NULL; }
		BIT_LE_W(TYPE *s) { Stream = s; Reminder = Offset = 0; }
		BIT_LE_W(TYPE &s) { Stream = &s; Reminder = Offset = 0; }

		/// finalize writing
		~BIT_LE_W()
		{
			if (Offset > 0)
			{
				Stream->W8b(Reminder);
				Reminder = Offset = 0;
			}
		}

		/// write an integer with 'nbits' bits to a stream
		void WriteBit(C_UInt32 Value, C_UInt8 nbit)
		{
			while (nbit > 0)
			{
				C_UInt8 I = 8 - Offset;
				if (I > nbit) I = nbit;
				C_UInt32 Mask = ~(0xFFFFFFFF << I);
				Reminder |= (Value & Mask) << Offset;
				Value >>= I; Offset += I;
				if (Offset > 7)
				{
					Offset = 0;
					Stream->W8b(Reminder);
					Reminder = 0;
				}
				nbit -= I;
			}
		}
	};



	// =====================================================================
	// Data stream object (native endianness)
	// =====================================================================

	/// Data stream with native endianness
	/** \tparam TYPE    it could be CdStream, CdBufStream or CdAllocator
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT BYTE_NT
	{
		TYPE *Stream;

		BYTE_NT() { Stream = NULL; }
		BYTE_NT(TYPE *s) { Stream = s; }
		BYTE_NT(TYPE &s) { Stream = &s; }

		/// Return the current position
		COREARRAY_FORCEINLINE SIZE64 Position()
		{
			return Stream->Position();
		}
		/// Reset the current position
		COREARRAY_FORCEINLINE void SetPosition(const SIZE64 Pos)
		{
			Stream->SetPosition(Pos);
		}

		/// Read block of data, or throw an exception if fail
		COREARRAY_FORCEINLINE void ReadData(void *Buffer, ssize_t Count)
		{
			Stream->ReadData(Buffer, Count);
		}
		/// Read a 8-bit integer
		COREARRAY_FORCEINLINE C_UInt8 R8b()
		{
			return Stream->R8b();
		}

		/// Write block of data, or throw an exception if fail
		COREARRAY_FORCEINLINE void WriteData(const void *Buffer, ssize_t Count)
		{
			Stream->WriteData(Buffer, Count);
		}
		/// Write a 8-bit integer with native endianness
		COREARRAY_FORCEINLINE void W8b(C_UInt8 val)
		{
			Stream->W8b(val);
		}


		// =================================================================
		// Operator - Read

		/// get a signed 8-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_Int8 &val)
		{
			val = Stream->R8b(); return *this;
		}
		/// get an unsigned 8-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_UInt8 &val)
		{
			val = Stream->R8b(); return *this;
		}
		/// get a vector of signed 8-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_Int8 *pval, size_t n)
		{
			Stream->ReadData(pval, n); return *this;
		}
		/// get a vector of unsigned 8-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_UInt8 *pval, size_t n)
		{
			Stream->ReadData(pval, n); return *this;
		}

		/// get a signed 16-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_Int16 &val)
		{
			val = Stream->R16b(); return *this;
		}
		/// get an unsigned 16-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_UInt16 &val)
		{
			val = Stream->R16b(); return *this;
		}
		/// get a vector of signed 16-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_Int16 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}
		/// get a vector of unsigned 16-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_UInt16 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}

		/// get a signed 32-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_Int32 &val)
		{
			val = Stream->R32b(); return *this;
		}
		/// get an unsigned 32-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_UInt32 &val)
		{
			val = Stream->R32b(); return *this;
		}
		/// get a vector of signed 32-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_Int32 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}
		/// get a vector of unsigned 32-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_UInt32 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}

		/// get a signed 64-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_Int64 &val)
		{
			val = Stream->R64b(); return *this;
		}
		/// get an unsigned 64-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_UInt64 &val)
		{
			val = Stream->R64b(); return *this;
		}
		/// get a vector of signed 64-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_Int64 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}
		/// get a vector of unsigned 64-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_UInt64 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}

		/// get a 32-bit floating number
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_Float32 &val)
		{
			Stream->ReadData(&val, sizeof(val)); return *this;
		}
		/// get a 64-bit floating number
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator >>(C_Float64 &val)
		{
			Stream->ReadData(&val, sizeof(val)); return *this;
		}
		/// get a vector of 32-bit floating numbers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_Float32 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}
		/// get a vector of 64-bit floating numbers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& R(C_Float64 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n); return *this;
		}


		// =================================================================
		// Operator - Write

		// write a signed 8-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_Int8 val)
		{
			Stream->W8b(val); return *this;
		}
		// write an unsigned 8-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_UInt8 val)
		{
			Stream->W8b(val); return *this;
		}
		/// write a vector of signed 8-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_Int8 *pval, size_t n)
		{
			Stream->WriteData(pval, n); return *this;
		}
		/// write a vector of unsigned 8-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_UInt8 *pval, size_t n)
		{
			Stream->WriteData(pval, n); return *this;
		}

		// write a signed 16-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_Int16 val)
		{
			Stream->W16b(val); return *this;
		}
		// set an unsigned 16-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_UInt16 val)
		{
			Stream->W16b(val); return *this;
		}
		/// write a vector of signed 16-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_Int16 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}
		/// write a vector of unsigned 16-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_UInt16 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}

		/// write a signed 32-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_Int32 val)
		{
			Stream->W32b(val); return *this;
		}
		/// write an unsigned 32-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_UInt32 val)
		{
			Stream->W32b(val); return *this;
		}
		/// write a vector of signed 32-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_Int32 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}
		/// write a vector of unsigned 32-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_UInt32 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}

		/// write a signed 64-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_Int64 val)
		{
			Stream->W64b(val); return *this;
		}
		/// write an unsigned 64-bit integer
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(C_UInt64 val)
		{
			Stream->W64b(val); return *this;
		}
		/// write a vector of signed 64-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_Int64 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}
		/// write a vector of unsigned 64-bit integers
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_UInt64 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}

		/// write a 32-bit floating number
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(const C_Float32 val)
		{
			Stream->WriteData(&val, sizeof(val)); return *this;
		}
		/// write a 64-bit floating number
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& operator <<(const C_Float64 val)
		{
			Stream->WriteData(&val, sizeof(val)); return *this;
		}
		/// write a vector of 32-bit floating number
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_Float32 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}
		/// write a vector of 64-bit floating number
		COREARRAY_FORCEINLINE BYTE_NT<TYPE>& W(const C_Float64 *pval, size_t n)
		{
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		}
	};



	// =====================================================================
	// Endian Macro and Function
	// =====================================================================

	#ifdef COREARRAY_ENDIAN_LITTLE

		/// native format to little endian, single value
		#define COREARRAY_ENDIAN_NT_TO_LE(x)               (x)
		/// native format to little endian, array
		#define COREARRAY_ENDIAN_NT_TO_LE_ARRAY(p, n)
		/// native format to little endian, array
		#define COREARRAY_ENDIAN_NT_TO_LE_ARRAY2(d, s, n)

		/// little endian to native format, single value
		#define COREARRAY_ENDIAN_LE_TO_NT(x)               (x)
		/// little endian to native format, array
		#define COREARRAY_ENDIAN_LE_TO_NT_ARRAY(p, n)
		/// little endian to native format, array
		#define COREARRAY_ENDIAN_LE_TO_NT_ARRAY2(d, s, n)

	#else

		/// native format to little endian, single value
		#define COREARRAY_ENDIAN_NT_TO_LE(x)               CoreArray::NT_TO_LE(x)
		/// native format to little endian, array
		#define COREARRAY_ENDIAN_NT_TO_LE_ARRAY(p, n)      CoreArray::NT_TO_LE_ARRAY(p, n)
		/// native format to little endian, array
		#define COREARRAY_ENDIAN_NT_TO_LE_ARRAY2(d, s, n)  CoreArray::NT_TO_LE_ARRAY2(d, s, n)

		/// little endian to native format, single value
		#define COREARRAY_ENDIAN_LE_TO_NT(x)               CoreArray::LE_TO_NT(x)
		/// little endian to native format, array
		#define COREARRAY_ENDIAN_LE_TO_NT_ARRAY(p, n)      CoreArray::LE_TO_NT_ARRAY(p, n)
		/// little endian to native format, array
		#define COREARRAY_ENDIAN_LE_TO_NT_ARRAY2(d, s, n)  CoreArray::LE_TO_NT_ARRAY2(d, s, n)


		/// define a buffer for endianness conversion, 8K
		#define COREARRAY_ENDIAN_BUF_SIZE                  8192u

		///
		#define COREARRAY_ENDIAN_BYTE_LE_W(TYPE)	\
			const size_t N = COREARRAY_ENDIAN_BUF_SIZE / sizeof(*pval); \
			TYPE Buffer[N]; \
			while (n > 0) \
			{ \
				size_t L = (n <= N) ? n : N; \
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY2(Buffer, pval, L); \
				pval += L; \
				Stream->WriteData(Buffer, sizeof(*pval)*L); \
				n -= L; \
			}


		// Worry about neither big or little endian? NOT support.

		// Function -- native format to little endian, single value

		static COREARRAY_FORCEINLINE C_Int8 NT_TO_LE(C_Int8 val)
			{ return val; }
		static COREARRAY_FORCEINLINE C_UInt8 NT_TO_LE(C_UInt8 val)
			{ return val; }
		static COREARRAY_FORCEINLINE C_Int16 NT_TO_LE(C_Int16 val)
			{ return ((C_UInt16)val << 8) | ((C_UInt16)val >> 8); }
		static COREARRAY_FORCEINLINE C_UInt16 NT_TO_LE(C_UInt16 val)
			{ return (val << 8) | (val >> 8); }
		static COREARRAY_FORCEINLINE C_Int32 NT_TO_LE(C_Int32 val)
			{
				return ((C_UInt32)val << 24) | (((C_UInt32)val & 0xFF00) << 8)
					| (((C_UInt32)val >> 8) & 0xFF00) | ((C_UInt32)val >> 24);
			}
		static COREARRAY_FORCEINLINE C_UInt32 NT_TO_LE(C_UInt32 val)
			{
				return (val << 24) | ((val & 0xFF00) << 8) |
					((val >> 8) & 0xFF00) | (val >> 24);
			}
		static COREARRAY_FORCEINLINE C_Int64 NT_TO_LE(C_Int64 val)
			{
				C_UInt32 L = NT_TO_LE((C_UInt32)val);
				C_UInt32 H = NT_TO_LE((C_UInt32)(val >> 32));
				return ((C_UInt64)L << 32) | (C_UInt64)H;
			}
		static COREARRAY_FORCEINLINE C_UInt64 NT_TO_LE(C_UInt64 val)
			{
				C_UInt32 L = NT_TO_LE((C_UInt32)val);
				C_UInt32 H = NT_TO_LE((C_UInt32)(val >> 32));
				return ((C_UInt64)L << 32) | (C_UInt64)H;
			}


		// Function -- native format to little endian, array

		static COREARRAY_FORCEINLINE
			void NT_TO_LE_ARRAY(C_Int8 *p, size_t n) { }
		static COREARRAY_FORCEINLINE
			void NT_TO_LE_ARRAY(C_UInt8 *p, size_t n) { }
		void NT_TO_LE_ARRAY(C_Int16 *p, size_t n);
		void NT_TO_LE_ARRAY(C_UInt16 *p, size_t n);
		void NT_TO_LE_ARRAY(C_Int32 *p, size_t n);
		void NT_TO_LE_ARRAY(C_UInt32 *p, size_t n);
		void NT_TO_LE_ARRAY(C_Int64 *p, size_t n);
		void NT_TO_LE_ARRAY(C_UInt64 *p, size_t n);
		void NT_TO_LE_ARRAY(C_Float32 *p, size_t n);
		void NT_TO_LE_ARRAY(C_Float64 *p, size_t n);

		static COREARRAY_FORCEINLINE
			void NT_TO_LE_ARRAY2(C_Int8 *d, const C_Int8 *s, size_t n) { }
		static COREARRAY_FORCEINLINE
			void NT_TO_LE_ARRAY2(C_UInt8 *d, const C_UInt8 *s, size_t n) { }
		void NT_TO_LE_ARRAY2(C_Int16 *d, const C_Int16 *s, size_t n);
		void NT_TO_LE_ARRAY2(C_UInt16 *d, const C_UInt16 *s, size_t n);
		void NT_TO_LE_ARRAY2(C_Int32 *d, const C_Int32 *s, size_t n);
		void NT_TO_LE_ARRAY2(C_UInt32 *d, const C_UInt32 *s, size_t n);
		void NT_TO_LE_ARRAY2(C_Int64 *d, const C_Int64 *s, size_t n);
		void NT_TO_LE_ARRAY2(C_UInt64 *d, const C_UInt64 *s, size_t n);
		void NT_TO_LE_ARRAY2(C_Float32 *d, const C_Float32 *s, size_t n);
		void NT_TO_LE_ARRAY2(C_Float64 *d, const C_Float64 *s, size_t n);


		// Function -- little endian to native format, single value

		static COREARRAY_FORCEINLINE C_Int8 LE_TO_NT(C_Int8 val)
			{ return val; }
		static COREARRAY_FORCEINLINE C_UInt8 LE_TO_NT(C_UInt8 val)
			{ return val; }
		static COREARRAY_FORCEINLINE C_Int16 LE_TO_NT(C_Int16 val)
			{ return ((C_UInt16)val << 8) | ((C_UInt16)val >> 8); }
		static COREARRAY_FORCEINLINE C_UInt16 LE_TO_NT(C_UInt16 val)
			{ return (val << 8) | (val >> 8); }
		static COREARRAY_FORCEINLINE C_Int32 LE_TO_NT(C_Int32 val)
			{
				return ((C_UInt32)val << 24) | (((C_UInt32)val & 0xFF00) << 8)
					| (((C_UInt32)val >> 8) & 0xFF00) | ((C_UInt32)val >> 24);
			}
		static COREARRAY_FORCEINLINE C_UInt32 LE_TO_NT(C_UInt32 val)
			{
				return (val << 24) | ((val & 0xFF00) << 8) |
					((val >> 8) & 0xFF00) | (val >> 24);
			}
		static COREARRAY_FORCEINLINE C_Int64 LE_TO_NT(C_Int64 val)
			{
				C_UInt32 L = NT_TO_LE((C_UInt32)val);
				C_UInt32 H = NT_TO_LE((C_UInt32)(val >> 32));
				return ((C_UInt64)L << 32) | (C_UInt64)H;
			}
		static COREARRAY_FORCEINLINE C_UInt64 LE_TO_NT(C_UInt64 val)
			{
				C_UInt32 L = NT_TO_LE((C_UInt32)val);
				C_UInt32 H = NT_TO_LE((C_UInt32)(val >> 32));
				return ((C_UInt64)L << 32) | (C_UInt64)H;
			}

		// Function -- little endian to native format, array

		static COREARRAY_FORCEINLINE
			void LE_TO_NT_ARRAY(C_Int8 *p, size_t n) { }
		static COREARRAY_FORCEINLINE
			void LE_TO_NT_ARRAY(C_UInt8 *p, size_t n) { }
		void LE_TO_NT_ARRAY(C_Int16 *p, size_t n);
		void LE_TO_NT_ARRAY(C_UInt16 *p, size_t n);
		void LE_TO_NT_ARRAY(C_Int32 *p, size_t n);
		void LE_TO_NT_ARRAY(C_UInt32 *p, size_t n);
		void LE_TO_NT_ARRAY(C_Int64 *p, size_t n);
		void LE_TO_NT_ARRAY(C_UInt64 *p, size_t n);
		void LE_TO_NT_ARRAY(C_Float32 *p, size_t n);
		void LE_TO_NT_ARRAY(C_Float64 *p, size_t n);

		static COREARRAY_FORCEINLINE
			void LE_TO_NT_ARRAY2(C_Int8 *d, const C_Int8 *s, size_t n) { }
		static COREARRAY_FORCEINLINE
			void LE_TO_NT_ARRAY2(C_UInt8 *d, const C_UInt8 *s, size_t n) { }
		void LE_TO_NT_ARRAY2(C_Int16 *d, const C_Int16 *s, size_t n);
		void LE_TO_NT_ARRAY2(C_UInt16 *d, const C_UInt16 *s, size_t n);
		void LE_TO_NT_ARRAY2(C_Int32 *d, const C_Int32 *s, size_t n);
		void LE_TO_NT_ARRAY2(C_UInt32 *d, const C_UInt32 *s, size_t n);
		void LE_TO_NT_ARRAY2(C_Int64 *d, const C_Int64 *s, size_t n);
		void LE_TO_NT_ARRAY2(C_UInt64 *d, const C_UInt64 *s, size_t n);
		void LE_TO_NT_ARRAY2(C_Float32 *d, const C_Float32 *s, size_t n);
		void LE_TO_NT_ARRAY2(C_Float64 *d, const C_Float64 *s, size_t n);


	#endif



	// =====================================================================
	// Data stream object (little endianness)
	// =====================================================================

	/// Data stream with little endianness
	/** \tparam TYPE    it could be CdStream, CdBufStream or CdAllocator
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT BYTE_LE
	{
		TYPE *Stream;

		BYTE_LE() { Stream = NULL; }
		BYTE_LE(TYPE *s) { Stream = s; }
		BYTE_LE(TYPE &s) { Stream = &s; }

		/// Return the current position
		COREARRAY_FORCEINLINE SIZE64 Position()
		{
			return Stream->Position();
		}
		/// Reset the current position
		COREARRAY_FORCEINLINE void SetPosition(const SIZE64 Pos)
		{
			Stream->SetPosition(Pos);
		}

		/// Read block of data, or throw an exception if fail
		COREARRAY_FORCEINLINE void ReadData(void *Buffer, ssize_t Count)
		{
			Stream->ReadData(Buffer, Count);
		}
		/// Read a 8-bit integer
		COREARRAY_FORCEINLINE C_UInt8 R8b()
		{
			return Stream->R8b();
		}

		/// Write block of data, or throw an exception if fail
		COREARRAY_FORCEINLINE void WriteData(const void *Buffer, ssize_t Count)
		{
			Stream->WriteData(Buffer, Count);
		}
		/// Write a 8-bit integer with native endianness
		COREARRAY_FORCEINLINE void W8b(C_UInt8 val)
		{
			Stream->W8b(val);
		}


		// =================================================================
		// Operator - Read

		/// get a signed 8-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_Int8 &val)
		{
			val = Stream->R8b(); return *this;
		}
		/// get an unsigned 8-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_UInt8 &val)
		{
			val = Stream->R8b(); return *this;
		}
		/// get a vector of signed 8-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_Int8 *pval, size_t n)
		{
			Stream->ReadData(pval, n); return *this;
		}
		/// get a vector of unsigned 8-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_UInt8 *pval, size_t n)
		{
			Stream->ReadData(pval, n); return *this;
		}

		/// get a signed 16-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_Int16 &val)
		{
			val = COREARRAY_ENDIAN_LE_TO_NT(Stream->R16b()); return *this;
		}
		/// get an unsigned 16-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_UInt16 &val)
		{
			val = COREARRAY_ENDIAN_LE_TO_NT(Stream->R16b()); return *this;
		}
		/// get a vector of signed 16-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_Int16 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}
		/// get a vector of unsigned 16-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_UInt16 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}

		/// get a signed 32-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_Int32 &val)
		{
			val = COREARRAY_ENDIAN_LE_TO_NT(Stream->R32b()); return *this;
		}
		/// get an unsigned 32-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_UInt32 &val)
		{
			val = COREARRAY_ENDIAN_LE_TO_NT(Stream->R32b()); return *this;
		}
		/// get a vector of signed 32-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_Int32 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}
		/// get a vector of unsigned 32-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_UInt32 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}

		/// get a signed 64-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_Int64 &val)
		{
			val = COREARRAY_ENDIAN_LE_TO_NT(Stream->R64b()); return *this;
		}
		/// get an unsigned 64-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_UInt64 &val)
		{
			val = COREARRAY_ENDIAN_LE_TO_NT(Stream->R64b()); return *this;
		}
		/// get a vector of signed 64-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_Int64 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}
		/// get a vector of unsigned 64-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_UInt64 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}

		/// get a 32-bit floating number
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_Float32 &val)
		{
			Stream->ReadData(&val, sizeof(val));
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(&val, 1);
			return *this;
		}
		/// get a 64-bit floating number
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator >>(C_Float64 &val)
		{
			Stream->ReadData(&val, sizeof(val));
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(&val, 1);
			return *this;
		}
		/// get a vector of 32-bit floating numbers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_Float32 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}
		/// get a vector of 64-bit floating numbers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& R(C_Float64 *pval, size_t n)
		{
			Stream->ReadData(pval, sizeof(*pval)*n);
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY(pval, n);
			return *this;
		}


		// =================================================================
		// Operator - Write

		// write a signed 8-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_Int8 val)
		{
			Stream->W8b(val); return *this;
		}
		// write an unsigned 8-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_UInt8 val)
		{
			Stream->W8b(val); return *this;
		}
		/// write a vector of signed 8-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_Int8 *pval, size_t n)
		{
			Stream->WriteData(pval, n); return *this;
		}
		/// write a vector of unsigned 8-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_UInt8 *pval, size_t n)
		{
			Stream->WriteData(pval, n); return *this;
		}

		// write a signed 16-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_Int16 val)
		{
			Stream->W16b(COREARRAY_ENDIAN_NT_TO_LE(val)); return *this;
		}
		// set an unsigned 16-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_UInt16 val)
		{
			Stream->W16b(COREARRAY_ENDIAN_NT_TO_LE(val)); return *this;
		}
		/// write a vector of signed 16-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_Int16 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_Int16); return *this;
		#endif
		}
		/// write a vector of unsigned 16-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_UInt16 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_UInt16); return *this;
		#endif
		}

		/// write a signed 32-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_Int32 val)
		{
			Stream->W32b(COREARRAY_ENDIAN_NT_TO_LE(val)); return *this;
		}
		/// write an unsigned 32-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_UInt32 val)
		{
			Stream->W32b(COREARRAY_ENDIAN_NT_TO_LE(val)); return *this;
		}
		/// write a vector of signed 32-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_Int32 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_Int32); return *this;
		#endif
		}
		/// write a vector of unsigned 32-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_UInt32 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_UInt32); return *this;
		#endif
		}

		/// write a signed 64-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_Int64 val)
		{
			Stream->W64b(COREARRAY_ENDIAN_NT_TO_LE(val)); return *this;
		}
		/// write an unsigned 64-bit integer
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_UInt64 val)
		{
			Stream->W64b(COREARRAY_ENDIAN_NT_TO_LE(val)); return *this;
		}
		/// write a vector of signed 64-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_Int64 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_Int64); return *this;
		#endif
		}
		/// write a vector of unsigned 64-bit integers
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_UInt64 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_UInt64); return *this;
		#endif
		}

		/// write a 32-bit floating number
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_Float32 val)
		{
			COREARRAY_ENDIAN_NT_TO_LE_ARRAY(&val, 1);
			Stream->WriteData(&val, sizeof(val));
			return *this;
		}
		/// write a 64-bit floating number
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& operator <<(C_Float64 val)
		{
			COREARRAY_ENDIAN_NT_TO_LE_ARRAY(&val, 1);
			Stream->WriteData(&val, sizeof(val));
			return *this;
		}
		/// write a vector of 32-bit floating number
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_Float32 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_Float32); return *this;
		#endif
		}
		/// write a vector of 64-bit floating number
		COREARRAY_FORCEINLINE BYTE_LE<TYPE>& W(const C_Float64 *pval, size_t n)
		{
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(pval, sizeof(*pval)*n); return *this;
		#else
			COREARRAY_ENDIAN_BYTE_LE_W(C_Float64); return *this;
		#endif
		}


		// =================================================================
		// Read/Write packed integer

		/// get an unsigned 16-bit integer from packed stream
		C_UInt16 Rp16b()
		{
			C_UInt16 B, rv;
			B = Stream->R8b(); rv = B & 0x7F;			// 1
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= (B & 0x7F) << 7;	// 2
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= (B & 0x7F) << 14;	// 3
			return rv;
		}
		/// get an unsigned 32-bit integer from packed stream
		C_UInt32 Rp32b()
		{
			C_UInt32 B, rv;
			B = Stream->R8b(); rv = B & 0x7F;			// 1
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= (B & 0x7F) << 7;	// 2
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= (B & 0x7F) << 14;	// 3
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= (B & 0x7F) << 21;	// 4
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= (B & 0x7F) << 28;	// 5
			return rv;
		}
		/// get an unsigned 64-bit integer from packed stream
		C_UInt64 Rp64b()
		{
			C_UInt64 rv; C_UInt8 B;
			B = Stream->R8b(); rv = B & 0x7F;					// 1
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B & 0x7F) << 7;	// 2
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B & 0x7F) << 14;	// 3
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B & 0x7F) << 21;	// 4
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B & 0x7F) << 28;	// 5
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B & 0x7F) << 35;	// 6
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B & 0x7F) << 42;	// 7
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B & 0x7F) << 49;	// 8
			if ((B & 0x80) == 0) return rv;
			B = Stream->R8b(); rv |= C_UInt64(B) << 56;			// 9
			return rv;
		}

		// write an unsigned 16-bit integer to packed stream
		void Wp16b(C_UInt16 val)
		{
			C_UInt8 B;
			B = val & 0x7F; val >>= 7;		// 1
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 2
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			Stream->W8b(val);				// 3
		}
		// write an unsigned 32-bit integer to packed stream
		void Wp32b(C_UInt32 val)
		{
			C_UInt8 B;
			B = val & 0x7F; val >>= 7;		// 1
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 2
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 3
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 4
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			Stream->W8b(val);				// 5
		}
		// write an unsigned 64-bit integer to packed stream
		void Wp64b(C_UInt64 val)
		{
			C_UInt8 B;
			B = val & 0x7F; val >>= 7;		// 1
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 2
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 3
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 4
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 5
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 6
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 7
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			B = val & 0x7F; val >>= 7;		// 8
			if (val > 0) { Stream->W8b(B | 0x80); } else { Stream->W8b(B); return; }
			Stream->W8b(val);				// 9
		}


		// =================================================================
		// Read/Write string from packed stream

		/// Read a UTF-8 string
		UTF8String RpUTF8()
		{
			size_t L = Rp32b();
			UTF8String rv(L, 0);
			Stream->ReadData(&rv[0], L);
			return rv;
		}
		/// Read a UTF-16 string
		UTF16String RpUTF16()
		{
			size_t L = Rp32b();
			BIT_LE_R<TYPE> RBit(Stream);
			UTF16String rv(L, 0);
			for (size_t i=0; i < L; i++)
			{
				C_UTF16 w = RBit.ReadBit(8);
				if (w > 0x7F)
					w = (w & 0x7F) | (RBit.ReadBit(9) << 7);
				rv[i] = w;
			}
			return rv;
		}
		/// Read a UTF-32 string
		UTF32String RpUTF32()
		{
			size_t L = Rp32b();
			UTF32String rv(L, 0);
			for (size_t i=0; i < L; i++) rv[i] = Rp32b();
			return rv;
		}

		/// Write a UTF-8 string
		void WpUTF8(const UTF8String &val)
		{
			Wp32b(val.size());
			Stream->WriteData(&(val[0]), val.size());
		}
		/// Write a UTF-16 string
		void WpUTF16(const UTF16String &val)
		{
			size_t L = val.size();
			Wp32b(L);
			BIT_LE_W<TYPE> WBit(Stream);
			for (size_t i=0; i < L; i++)
			{
				C_UTF16 w = val[i];
				if (w <= 0x7F)
				{
					WBit.WriteBit(w, 8);
				} else {
					WBit.WriteBit(w | 0x80, 8);
					w >>= 7;
					WBit.WriteBit(w, 9);
				}
			}
		}
		/// Write a UTF-32 string
		void WpUTF32(const UTF32String &val)
		{
			size_t L = val.size();
			Wp32b(L);
			for (size_t i=0; i < L; i++) Wp32b(val[i]);
		}


		// =================================================================
		// Read/Write TdGDSPos

		/// an operator, to read TdGDSPos
		BYTE_LE<TYPE>& operator >>(TdGDSPos& out)
		{
			C_UInt64 v = 0;
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->ReadData(&v, GDS_POS_SIZE);
		#else
			v |= Stream->R8b();
			v |= (C_UInt64)Stream->R8b() << 8;
			v |= (C_UInt64)Stream->R8b() << 16;
			v |= (C_UInt64)Stream->R8b() << 24;
			v |= (C_UInt64)Stream->R8b() << 32;
			v |= (C_UInt64)Stream->R8b() << 40;
		#endif
			out = (C_Int64)v;
			return *this;
		}
		/// an operator, to write TdGDSPos
		BYTE_LE<TYPE>& operator <<(const TdGDSPos &in)
		{
			C_UInt64 v = (C_UInt64)in;
		#ifdef COREARRAY_ENDIAN_LITTLE
			Stream->WriteData(&v, GDS_POS_SIZE);
		#else
			Stream->W8b(v); v >>= 8;
			Stream->W8b(v); v >>= 8;
			Stream->W8b(v); v >>= 8;
			Stream->W8b(v); v >>= 8;
			Stream->W8b(v); v >>= 8;
			Stream->W8b(v);
		#endif
			return *this;
		}


		// =================================================================
		// Read/Write TdGDSBlockID

		/// an operator, to read TdGDSBlockID
		BYTE_LE<TYPE>& operator >>(TdGDSBlockID& out)
		{
			(*this) >> out.Get();
			return *this;
		}
		/// an operator, to write TdGDSBlockID
		BYTE_LE<TYPE>& operator <<(const TdGDSBlockID &in)
		{
			(*this) << in.Get();
			return *this;
		}
	};



	// =====================================================================
	// Get value
	// =====================================================================

	/// Unaligned C_Int16 -- get
	static COREARRAY_FORCEINLINE C_Int16 GET_VAL_UNALIGNED_LE_PTR(const C_Int16 *p)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_Int16(s[0]) | (C_Int16(s[1]) << 8);
		} else
			return COREARRAY_ENDIAN_LE_TO_NT(*p);
	}

	/// Unaligned C_UInt16 -- get
	static COREARRAY_FORCEINLINE C_UInt16 GET_VAL_UNALIGNED_LE_PTR(const C_UInt16 *p)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_UInt16(s[0]) | (C_UInt16(s[1]) << 8);
		} else
			return COREARRAY_ENDIAN_LE_TO_NT(*p);
	}

	/// Unaligned C_Int32 -- get
	static COREARRAY_FORCEINLINE C_Int32 GET_VAL_UNALIGNED_LE_PTR(const C_Int32 *p)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_Int32(s[0]) | (C_Int32(s[1]) << 8) |
				(C_Int32(s[2]) << 16) | (C_Int32(s[3]) << 24);
		} else
			return COREARRAY_ENDIAN_LE_TO_NT(*p);
	}

	/// Unaligned C_UInt32 -- get
	static COREARRAY_FORCEINLINE C_UInt32 GET_VAL_UNALIGNED_LE_PTR(const C_UInt32 *p)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_UInt32(s[0]) | (C_UInt32(s[1]) << 8) |
				(C_UInt32(s[2]) << 16) | (C_UInt32(s[3]) << 24);
		} else
			return COREARRAY_ENDIAN_LE_TO_NT(*p);
	}

	/// Unaligned C_Int16 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_LE_PTR(C_Int16 *p, C_Int16 val)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1];
		} else
			*p = COREARRAY_ENDIAN_NT_TO_LE(val);
	}

	/// Unaligned C_UInt16 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_LE_PTR(C_UInt16 *p, C_UInt16 val)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1];
		} else
			*p = COREARRAY_ENDIAN_NT_TO_LE(val);
	}

	/// Unaligned C_Int32 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_LE_PTR(C_Int32 *p, C_Int32 val)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1]; s2[2] = s1[2]; s2[3] = s1[3];
		} else
			*p = COREARRAY_ENDIAN_NT_TO_LE(val);
	}

	/// Unaligned C_UInt32 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_LE_PTR(C_UInt32 *p, C_UInt32 val)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1]; s2[2] = s1[2]; s2[3] = s1[3];
		} else
			*p = COREARRAY_ENDIAN_NT_TO_LE(val);
	}
}

#endif /* _HEADER_COREARRAY_ENDIAN_ */
