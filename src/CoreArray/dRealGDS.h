// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dRealGDS.h: Packed real number in GDS format
//
// Copyright (C) 2015-2016    Xiuwen Zheng
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
 *	\file     dRealGDS.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.1
 *	\date     2016
 *	\brief    Bit operators and classes of GDS format
 *	\details
**/

#ifndef _HEADER_COREARRAY_REAL_GDS_
#define _HEADER_COREARRAY_REAL_GDS_

#include "dStruct.h"
#include <math.h>


namespace CoreArray
{
	using namespace std;

	/// define 8-bit packed real number
	typedef struct { C_Int8 Val; } TREAL8;

	/// define 16-bit packed real number
	typedef struct { C_Int16 Val; } TREAL16;

	/// define 32-bit packed real number
	typedef struct { C_Int32 Val; } TREAL32;


	/// Traits of 8-bit packed real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TREAL8>
	{
		typedef C_Float64 TType;
		typedef C_Int8 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 8u;
		static const bool isPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal8"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static const C_Float64 InitialOffset() { return 0; }
		static const C_Float64 InitialScale() { return 0.01; }
		static const C_Int8 MissingValue = 0x80;
	};

	/// Traits of 16-bit packed real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TREAL16>
	{
		typedef C_Float64 TType;
		typedef C_Int16 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 16u;
		static const bool isPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal16"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static const C_Float64 InitialOffset() { return 0; }
		static const C_Float64 InitialScale() { return 0.0001; }
		static const C_Int16 MissingValue = 0x8000;
	};

	/// Traits of 32-bit packed real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TREAL32>
	{
		typedef C_Float64 TType;
		typedef C_Int32 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 32u;
		static const bool isPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal32"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static const C_Float64 InitialOffset() { return 0; }
		static const C_Float64 InitialScale() { return 0.000001; }
		static const C_Int32 MissingValue = 0x80000000;
	};



	// =====================================================================
	// Packed real number classes of GDS format
	// =====================================================================

	/// Container of packed real number
	/** \tparam REAL_TYPE    should be TREAL8, TREAL16 or TREAL32
	**/
	template<typename REAL_TYPE>
		class COREARRAY_DLL_DEFAULT CdPackedReal: public CdArray<REAL_TYPE>
	{
	public:
		typedef REAL_TYPE ElmType;

		CdPackedReal(): CdArray<REAL_TYPE>()
		{
			fOffset = TdTraits<REAL_TYPE>::InitialOffset();
			fScale  = TdTraits<REAL_TYPE>::InitialScale();
			fInvScale = 1.0 / fScale;
		}

		virtual CdGDSObj *NewObject()
		{
			return (new CdPackedReal<REAL_TYPE>)->AssignPipe(*this);
		}

		COREARRAY_INLINE C_Float64 Offset() const
		{
			return fOffset;
		}
		void SetOffset(C_Float64 val)
		{
			if (val != fOffset)
			{
				fOffset = val;
				this->fChanged = true;
			}
		}

		COREARRAY_INLINE C_Float64 Scale() const
		{
			return fScale;
		}
		void SetScale(C_Float64 val)
		{
			if (val != fScale)
			{
				fScale = val;
				fInvScale = 1.0 / fScale;
				this->fChanged = true;
			}
		}

		COREARRAY_INLINE C_Float64 InvScale() const
		{
			return fInvScale;
		}

	protected:

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			CdAllocArray::Loading(Reader, Version);
			Reader["OFFSET"] >> fOffset;
			Reader["SCALE"]  >> fScale;
		}

		/// saving function for serialization
		virtual void Saving(CdWriter &Writer)
		{
			CdAllocArray::Saving(Writer);
			Writer["OFFSET"] << fOffset;
			Writer["SCALE"]  << fScale;
		}

		C_Float64 fOffset;
		C_Float64 fScale;
		C_Float64 fInvScale;
	};



	// =====================================================================
	// Template for Allocator
	// =====================================================================

	/// Template functions for allocator of TREAL8 (MEM_TYPE is numeric)
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL8, MEM_TYPE, true>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER;
			C_Int8 Buf[N];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_Int8 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					*Buffer++ = MEM_TYPE((*p != C_Int8(0x80)) ?
						((*p) * scale + offset) : NaN);
				}
			}
			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL Sel[])
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER;
			C_Int8 Buf[N];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_Int8 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					if (*Sel++)
					{
						*Buffer++ = MEM_TYPE((*p != C_Int8(0x80)) ?
							((*p) * scale + offset) : NaN);
					}
				}
			}
			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER;
			C_Int8 Buf[N];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				C_Int8 *p = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round(((*Buffer++) - offset) * scale);
					C_Int8 I = 0x80;
					if (IsFinite(v))
					{
						if ((-127.5 < v) && (v <= 127.5))
							I = (C_Int8)v;
					}
					*p++ = I;
				}
				I.Allocator->WriteData(Buf, Cnt);
				n -= Cnt;
			}
			return Buffer;
		}
	};

	/// Template functions for allocator of TREAL8 (MEM_TYPE is not numeric)
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL8, MEM_TYPE, false>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER;
			C_Int8 Buf[N];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_Int8 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					*Buffer++ = ValCvt<MEM_TYPE, C_Float64>(
						(*p != C_Int8(0x80)) ? ((*p)*scale + offset) : NaN);
				}
			}
			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL Sel[])
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER;
			C_Int8 Buf[N];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_Int8 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					if (*Sel++)
					{
						*Buffer++ = ValCvt<MEM_TYPE, C_Float64>(
							(*p != C_Int8(0x80)) ? ((*p)*scale + offset) : NaN);
					}
				}
			}
			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER;
			C_Int8 Buf[N];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				C_Int8 *p = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((ValCvt<C_Float64, MEM_TYPE>(*Buffer++) -
						offset) * scale);
					C_Int8 I = 0x80;
					if (IsFinite(v))
					{
						if ((-127.5 < v) && (v <= 127.5))
							I = (C_Int8)v;
					}
					*p++ = I;
				}
				I.Allocator->WriteData(Buf, Cnt);
				n -= Cnt;
			}
			return Buffer;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TREAL16 (MEM_TYPE is numeric)
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL16, MEM_TYPE, true>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 1;
			C_Int16 Buf[N];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					*Buffer++ = MEM_TYPE((*p != C_Int16(0x8000)) ?
						((*p) * scale + offset) : NaN);
				}
			}
			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL Sel[])
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 1;
			C_Int16 Buf[N];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					if (*Sel++)
					{
						*Buffer++ = MEM_TYPE((*p != C_Int16(0x8000)) ?
							((*p) * scale + offset) : NaN);
					}
				}
			}
			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 1;
			C_Int16 Buf[N];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				C_Int16 *p = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round(((*Buffer++) - offset) * scale);
					C_Int16 I = 0x8000;
					if (IsFinite(v))
					{
						if ((-32767.5 < v) && (v <= 32767.5))
							I = (C_Int16)v;
					}
					*p++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 1);
				n -= Cnt;
			}
			return Buffer;
		}
	};

	/// Template functions for allocator of TREAL16 (MEM_TYPE is not numeric)
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL16, MEM_TYPE, false>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 1;
			C_Int16 Buf[N];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					*Buffer++ = ValCvt<MEM_TYPE, C_Float64>(
						(*p != C_Int16(0x8000)) ? ((*p) * scale + offset) : NaN);
				}
			}
			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL Sel[])
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 1;
			C_Int16 Buf[N];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					if (*Sel++)
					{
						*Buffer++ = ValCvt<MEM_TYPE, C_Float64>(
							(*p != C_Int16(0x8000)) ? ((*p) * scale + offset) : NaN);
					}
				}
			}
			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 1;
			C_Int16 Buf[N];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				C_Int16 *p = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((ValCvt<C_Float64, MEM_TYPE>(*Buffer++) -
						offset) * scale);
					C_Int16 I = 0x8000;
					if (IsFinite(v))
					{
						if ((-32767.5 < v) && (v <= 32767.5))
							I = (C_Int16)v;
					}
					*p++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 1);
				n -= Cnt;
			}
			return Buffer;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TREAL32 (MEM_TYPE is numeric)
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL32, MEM_TYPE, true>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 2;
			C_Int32 Buf[N];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					*Buffer++ = MEM_TYPE((*p != C_Int32(0x80000000)) ?
						((*p) * scale + offset) : NaN);
				}
			}
			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL Sel[])
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 2;
			C_Int32 Buf[N];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					if (*Sel++)
					{
						*Buffer++ = MEM_TYPE((*p != C_Int32(0x80000000)) ?
							((*p) * scale + offset) : NaN);
					}
				}
			}
			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 2;
			C_Int32 Buf[N];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				C_Int32 *p = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round(((*Buffer++) - offset) * scale);
					C_Int32 I = 0x80000000;
					if (IsFinite(v))
					{
						if ((-2147483647.5 < v) && (v <= 2147483647.5))
							I = (C_Int32)v;
					}
					*p++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 2);
				n -= Cnt;
			}
			return Buffer;
		}
	};

	/// Template functions for allocator of TREAL32 (MEM_TYPE is not numeric)
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL32, MEM_TYPE, false>
	{
		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *Buffer, ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 2;
			C_Int32 Buf[N];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					*Buffer++ = ValCvt<MEM_TYPE, C_Float64>(
						(*p != C_Int32(0x80000000)) ? ((*p) * scale + offset) : NaN);
				}
			}
			return Buffer;
		}

		/// read an array from CdAllocator
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *Buffer, ssize_t n,
			const C_BOOL Sel[])
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 2;
			C_Int32 Buf[N];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *p=Buf; Cnt > 0; Cnt--, p++)
				{
					if (*Sel++)
					{
						*Buffer++ = ValCvt<MEM_TYPE, C_Float64>(
							(*p != C_Int32(0x80000000)) ? ((*p) * scale + offset) : NaN);
					}
				}
			}
			return Buffer;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *Buffer,
			ssize_t n)
		{
			const ssize_t N = COREARRAY_ALLOC_FUNC_BUFFER >> 2;
			C_Int32 Buf[N];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= N) ? N : n;
				C_Int32 *p = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((ValCvt<C_Float64, MEM_TYPE>(*Buffer++) -
						offset) * scale);
					C_Int32 I = 0x80000000;
					if (IsFinite(v))
					{
						if ((-2147483647.5 < v) && (v <= 2147483647.5))
							I = (C_Int32)v;
					}
					*p++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 2);
				n -= Cnt;
			}
			return Buffer;
		}
	};



	// =====================================================================
	// Packed real numbers in GDS files
	// =====================================================================

	typedef CdPackedReal<TREAL8>     CdPackedReal8;
	typedef CdPackedReal<TREAL16>    CdPackedReal16;
	typedef CdPackedReal<TREAL32>    CdPackedReal32;
}

#endif /* _HEADER_COREARRAY_REAL_GDS_ */
