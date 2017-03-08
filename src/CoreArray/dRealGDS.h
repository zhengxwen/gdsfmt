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
// Copyright (C) 2015-2017    Xiuwen Zheng
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
 *	\date     2017
 *	\brief    Bit operators and classes of GDS format
 *	\details
**/

#ifndef _HEADER_COREARRAY_REAL_GDS_
#define _HEADER_COREARRAY_REAL_GDS_

#include "dStruct.h"
#include <math.h>
#include <typeinfo>


namespace CoreArray
{
	using namespace std;

	/// define 8-bit packed real number
	typedef struct { C_Int8 Val; } TREAL8;

	/// define 16-bit packed real number
	typedef struct { C_Int16 Val; } TREAL16;

	/// define 24-bit packed real number
	typedef struct { C_UInt8 Val[3]; } TREAL24;

	/// define 32-bit packed real number
	typedef struct { C_Int32 Val; } TREAL32;


	/// Traits of 8-bit packed real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TREAL8>
	{
		typedef C_Float64 TType;
		typedef C_Int8 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal8"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.01; }
		static const C_Int8 MissingValue = 0x80;
	};

	/// Traits of 16-bit packed real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TREAL16>
	{
		typedef C_Float64 TType;
		typedef C_Int16 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal16"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.0001; }
		static const C_Int16 MissingValue = 0x8000;
	};

	/// Traits of 24-bit packed real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TREAL24>
	{
		typedef C_Float64 TType;
		typedef C_Int32 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 24u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal24"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.00001; }
		static const C_Int32 MissingValue = 0x800000;
	};

	/// Traits of 32-bit packed real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TREAL32>
	{
		typedef C_Float64 TType;
		typedef C_Int32 ElmType;

		static const int trVal = COREARRAY_TR_PACKED_REAL;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = true;
		static const C_SVType SVType = svCustomFloat;

		static const char *StreamName() { return "dPackedReal32"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }

		static C_Float64 InitialOffset() { return 0; }
		static C_Float64 InitialScale() { return 0.000001; }
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

		/// append new data from an iterator
		virtual void AppendIter(CdIterator &I, C_Int64 Count)
		{
			if ((Count >= 65536) && (typeid(*this) == typeid(*I.Handler)))
			{
				CdPackedReal<REAL_TYPE> *Src = (CdPackedReal<REAL_TYPE> *)I.Handler;
				if ((this->fOffset == Src->fOffset) &&
					(this->fScale == Src->fScale) &&
					this->fAllocator.BufStream())
				{
					Src->Allocator().BufStream()->FlushWrite();
					this->fAllocator.BufStream()->CopyFrom(
						*(Src->Allocator().BufStream()->Stream()),
						I.Ptr, Count * this->fElmSize);

					// check
					CdAllocArray::TDimItem &R = this->fDimension.front();
					this->fTotalCount += Count;
					if (this->fTotalCount >= R.DimElmCnt*(R.DimLen+1))
					{
						R.DimLen = this->fTotalCount / R.DimElmCnt;
						this->fNeedUpdate = true;
					}

					return;
				}
			}
			CdAbstractArray::AppendIter(I, Count);
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

	/// Template functions for allocator of TREAL8
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL8, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			C_Int8 Buf[NBUF];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_Int8 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
						(*s != C_Int8(0x80)) ? ((*s) * scale + offset) : NaN);
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL Sel[])
		{
			C_Int8 Buf[NBUF];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt);
				n -= Cnt;
				for (C_Int8 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					if (*Sel++)
					{
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
							(*s != C_Int8(0x80)) ? ((*s) * scale + offset) : NaN);
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			C_Int8 Buf[NBUF];
			CdPackedReal<TREAL8> *IT =
				static_cast< CdPackedReal<TREAL8>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n;
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_Int8 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int8 I = 0x80;
					if (IsFinite(v))
					{
						if ((-127.5 < v) && (v <= 127.5))
							I = (C_Int8)v;
					}
					*s++ = I;
				}
				I.Allocator->WriteData(Buf, Cnt);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TREAL16
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL16, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER >> 1;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			C_Int16 Buf[NBUF];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
						(*s != C_Int16(0x8000)) ? ((*s) * scale + offset) : NaN);
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL Sel[])
		{
			C_Int16 Buf[NBUF];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int16 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					if (*Sel++)
					{
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
							(*s != C_Int16(0x8000)) ? ((*s) * scale + offset) : NaN);
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			C_Int16 Buf[NBUF];
			CdPackedReal<TREAL16> *IT =
				static_cast< CdPackedReal<TREAL16>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 1);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_Int16 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int16 I = 0x8000;
					if (IsFinite(v))
					{
						if ((-32767.5 < v) && (v <= 32767.5))
							I = (C_Int16)v;
					}
					*s++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 1);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TREAL24
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL24, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER / 3;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			C_UInt8 Buf[NBUF][3];
			CdPackedReal<TREAL24> *IT =
				static_cast< CdPackedReal<TREAL24>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt*3);
				n -= Cnt;
				for (C_UInt8 *s=Buf[0]; Cnt > 0; Cnt--, s+=3)
				{
					C_Int32 val = s[0] | (C_Int32(s[1]) << 8) | (C_Int32(s[2]) << 16);
					if (val != 0x800000)
					{
						if (val & 0x800000) val |= 0xFF000000;
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE, val * scale + offset);
					} else
						*p++ = NaN;
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL Sel[])
		{
			C_UInt8 Buf[NBUF][3];
			CdPackedReal<TREAL24> *IT =
				static_cast< CdPackedReal<TREAL24>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				I.Allocator->ReadData(Buf, Cnt*3);
				n -= Cnt;
				for (C_UInt8 *s=Buf[0]; Cnt > 0; Cnt--, s+=3)
				{
					if (*Sel++)
					{
						C_Int32 val = s[0] | (C_Int32(s[1]) << 8) | (C_Int32(s[2]) << 16);
						if (val != 0x800000)
						{
							if (val & 0x800000) val |= 0xFF000000;
							*p++ = VAL_CONV_FROM_F64(MEM_TYPE, val * scale + offset);
						} else
							*p++ = NaN;
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			C_UInt8 Buf[NBUF][3];
			CdPackedReal<TREAL24> *IT =
				static_cast< CdPackedReal<TREAL24>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n * 3);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_UInt8 *s = Buf[0];
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int32 I = 0x800000;
					if (IsFinite(v))
					{
						if ((-8388607.5 < v) && (v <= 8388607.5))
							I = (C_Int32)v;
					}
					s[0] = C_UInt8(I);
					s[1] = C_UInt8(I >> 8);
					s[2] = C_UInt8(I >> 16);
					s += 3;
				}
				I.Allocator->WriteData(Buf, Cnt*3);
				n -= Cnt;
			}
			return p;
		}
	};


	// ---------------------------------------------------------------------

	/// Template functions for allocator of TREAL32
	template<typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TREAL32, MEM_TYPE>
	{
		static const ssize_t NBUF = COREARRAY_ALLOC_FUNC_BUFFER >> 2;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			C_Int32 Buf[NBUF];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
						(*s != C_Int32(0x80000000)) ? ((*s) * scale + offset) : NaN);
				}
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL Sel[])
		{
			C_Int32 Buf[NBUF];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->Scale();

			BYTE_LE<CdAllocator> ss(I.Allocator);
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				ss.R(Buf, Cnt);
				n -= Cnt;
				for (C_Int32 *s=Buf; Cnt > 0; Cnt--, s++)
				{
					if (*Sel++)
					{
						*p++ = VAL_CONV_FROM_F64(MEM_TYPE,
							(*s != C_Int32(0x80000000)) ? ((*s) * scale + offset) : NaN);
					}
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			C_Int32 Buf[NBUF];
			CdPackedReal<TREAL32> *IT =
				static_cast< CdPackedReal<TREAL32>* >(I.Handler);
			const C_Float64 offset = IT->Offset();
			const C_Float64 scale = IT->InvScale();

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += (n << 2);
			while (n > 0)
			{
				ssize_t Cnt = (n >= NBUF) ? NBUF : n;
				C_Int32 *s = Buf;
				for (ssize_t m=Cnt; m > 0; m--)
				{
					double v = round((VAL_CONV_TO_F64(MEM_TYPE, *p++) - offset) * scale);
					C_Int32 I = 0x80000000;
					if (IsFinite(v))
					{
						if ((-2147483647.5 < v) && (v <= 2147483647.5))
							I = (C_Int32)v;
					}
					*s++ = I;
				}
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY(Buf, Cnt);
				I.Allocator->WriteData(Buf, Cnt << 2);
				n -= Cnt;
			}
			return p;
		}
	};



	// =====================================================================
	// Packed real numbers in GDS files
	// =====================================================================

	typedef CdPackedReal<TREAL8>     CdPackedReal8;
	typedef CdPackedReal<TREAL16>    CdPackedReal16;
	typedef CdPackedReal<TREAL24>    CdPackedReal24;
	typedef CdPackedReal<TREAL32>    CdPackedReal32;
}

#endif /* _HEADER_COREARRAY_REAL_GDS_ */
