// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dRleGDS.h: Run-length encoded arrays in GDS format
//
// Copyright (C) 2026    Xiuwen Zheng
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
 *	\file     dRleGDS.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2026
 *	\brief    Run-length encoded arrays in GDS format
 *	\details
 *
 *  The data stream is a sequence of records, each standing for the next L
 *  elements (L >= 1). A record starts with a header, and the lowest bits of
 *  its first byte b tell the kind of the record:
 *
 *    b & 0x01 == 0     LITERAL: L = (b >> 1) + 1, from 1 to 128; the header
 *                      is this byte only, and L values follow
 *    b & 0x03 == 0x01  REPEAT: one value v follows, standing for L copies
 *    b & 0x03 == 0x03  STEP: v0 and a step d follow, standing for
 *                      x_i = v0 + i*d, i = 0, ..., L-1
 *
 *  For REPEAT and STEP, L - 1 is a variable-length integer of at most 63
 *  bits: the bits 2..6 of b hold its lowest 5 bits, bit 7 (0x80) says that
 *  another byte follows, and every following byte holds 7 more bits with
 *  0x80 again flagging a continuation. The header is at most 10 bytes long.
 *
 *  A string array has no STEP record, so b & 0x01 == 1 alone means REPEAT
 *  and the bits 1..6 of b hold the lowest 6 bits of L - 1.
 *
 *  A number is stored as it is, in little endian, and so is a step. An
 *  integer step is added with wrap-around in the width of the type; a
 *  floating-point step is computed as v0 + i*d with two roundings (see
 *  dRleGDS.cpp). A string is its length as a variable-length integer
 *  followed by its UTF-8 bytes.
 *
 *  A checkpoint table in a separate block stream lists, in increasing order,
 *  the first element and the byte offset of some records, so that random
 *  access starts at the closest one before the element it wants. The writer
 *  keeps one for the record holding each multiple of a stride that is not in
 *  the first record: 65536 elements for numbers, whose literal records are
 *  passed over in one step, and 4096 for strings, whose literal records are
 *  passed over one string at a time. The stride is not part of the format; a
 *  reader uses whatever checkpoints there are.
**/

#ifndef _HEADER_COREARRAY_RLE_GDS_
#define _HEADER_COREARRAY_RLE_GDS_

#include "dStruct.h"
#include <vector>
#include <cstring>
#include <typeinfo>


namespace CoreArray
{
	using namespace std;

	/// Element type of a run-length encoded array holding values of TYPE
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT TRLE
	{
		typedef TYPE TType;
	};

	typedef TRLE<C_UInt8>     TRleUInt8;   ///< run-length encoded unsigned 8-bit integer
	typedef TRLE<C_Int32>     TRleInt32;   ///< run-length encoded signed 32-bit integer
	typedef TRLE<C_Int64>     TRleInt64;   ///< run-length encoded signed 64-bit integer
	typedef TRLE<C_Float32>   TRleReal32;  ///< run-length encoded 32-bit real number
	typedef TRLE<C_Float64>   TRleReal64;  ///< run-length encoded 64-bit real number
	typedef TRLE<UTF8String>  TRleStr8;    ///< run-length encoded UTF-8 string


	/// Traits of run-length encoded unsigned 8-bit integer
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TRleUInt8>
	{
		typedef C_UInt8 TType;
		typedef C_UInt8 ElmType;
		static const int trVal = COREARRAY_TR_RLE_INTEGER;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svUInt8;

		static const char *StreamName() { return "dRLEUInt8"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static unsigned short Min() { return 0; }
		COREARRAY_INLINE static unsigned short Max() { return UINT8_MAX; }
	};

	/// Traits of run-length encoded signed 32-bit integer
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TRleInt32>
	{
		typedef C_Int32 TType;
		typedef C_Int32 ElmType;
		static const int trVal = COREARRAY_TR_RLE_INTEGER;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svInt32;

		static const char *StreamName() { return "dRLEInt32"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Int32 Min() { return INT32_MIN; }
		COREARRAY_INLINE static C_Int32 Max() { return INT32_MAX; }
	};

	/// Traits of run-length encoded signed 64-bit integer
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TRleInt64>
	{
		typedef C_Int64 TType;
		typedef C_Int64 ElmType;
		static const int trVal = COREARRAY_TR_RLE_INTEGER;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svInt64;

		static const char *StreamName() { return "dRLEInt64"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Int64 Min() { return numeric_limits<C_Int64>::min(); }
		COREARRAY_INLINE static C_Int64 Max() { return numeric_limits<C_Int64>::max(); }
	};

	/// Traits of run-length encoded 32-bit real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TRleReal32>
	{
		typedef C_Float32 TType;
		typedef C_Float32 ElmType;
		static const int trVal = COREARRAY_TR_RLE_REAL;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svFloat32;

		static const char *StreamName() { return "dRLEReal32"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float32 Min() { return FLT_MIN; }
		COREARRAY_INLINE static C_Float32 Max() { return FLT_MAX; }
		COREARRAY_INLINE static C_Float32 Epsilon() { return FLT_EPSILON; }
		COREARRAY_INLINE static int Digits() { return FLT_MANT_DIG; }
	};

	/// Traits of run-length encoded 64-bit real number
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TRleReal64>
	{
		typedef C_Float64 TType;
		typedef C_Float64 ElmType;
		static const int trVal = COREARRAY_TR_RLE_REAL;
		static const unsigned BitOf = 64u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svFloat64;

		static const char *StreamName() { return "dRLEReal64"; }
		static const char *TraitName() { return StreamName()+1; }

		COREARRAY_INLINE static C_Float64 Min() { return DBL_MIN; }
		COREARRAY_INLINE static C_Float64 Max() { return DBL_MAX; }
		COREARRAY_INLINE static C_Float64 Epsilon() { return DBL_EPSILON; }
		COREARRAY_INLINE static int Digits() { return DBL_MANT_DIG; }
	};

	/// Traits of run-length encoded UTF-8 string
	template<> struct COREARRAY_DLL_DEFAULT TdTraits<TRleStr8>
	{
		typedef UTF8String TType;
		typedef C_UTF8 ElmType;
		static const int trVal = COREARRAY_TR_RLE_STRING;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF8;

		static const char *StreamName() { return "dRLEStr8"; }
		static const char *TraitName() { return StreamName()+1; }
	};



	// =====================================================================
	// Coding of the values
	// =====================================================================

	namespace _INTERNAL
	{
		/// the i-th value of a floating-point step run, v0 + i*d
		/** Written out of line, in dRleGDS.cpp, where the product and the sum
		 *  are kept two separately rounded operations on every compiler. **/
		COREARRAY_DLL_DEFAULT C_Float64 RLE_StepValue(C_Float64 v0, C_Float64 d,
			C_Int64 i);
		/// the i-th value of a floating-point step run, v0 + i*d in double
		/// precision, rounded to single precision
		COREARRAY_DLL_DEFAULT C_Float32 RLE_StepValue(C_Float32 v0, C_Float32 d,
			C_Int64 i);
		/// the values i, ..., i+n-1 of a floating-point step run
		COREARRAY_DLL_DEFAULT void RLE_StepFill(C_Float64 *p, C_Float64 v0,
			C_Float64 d, C_Int64 i, ssize_t n);
		/// the values i, ..., i+n-1 of a floating-point step run
		COREARRAY_DLL_DEFAULT void RLE_StepFill(C_Float32 *p, C_Float32 v0,
			C_Float32 d, C_Int64 i, ssize_t n);
		/// the steps that make x0, x1 the first two values of a run
		/** \return the number of steps stored in out, at most RLE_MAX_CAND;
		 *          each one reproduces x0 and x1 exactly **/
		COREARRAY_DLL_DEFAULT int RLE_StepCand(C_Float64 x0, C_Float64 x1,
			C_Float64 out[]);
		/// the steps that make x0, x1 the first two values of a run
		COREARRAY_DLL_DEFAULT int RLE_StepCand(C_Float32 x0, C_Float32 x1,
			C_Float32 out[]);

		/// the largest number of candidate steps a floating-point run keeps
		const int RLE_MAX_CAND = 6;


		/// Value coding of an integer type
		/** A step is added with wrap-around in the width of TYPE, UTYPE being
		 *  its unsigned counterpart. **/
		template<typename TYPE, typename UTYPE>
			struct COREARRAY_DLL_DEFAULT RLE_INT_CODEC
		{
			typedef TYPE TValue;
			static const bool FIXED = true;     ///< a value has a fixed width
			static const bool HAS_STEP = true;  ///< STEP records are allowed
			static const bool INT_STEP = true;  ///< a step run is exact and cheap to check
			static const int MAX_CAND = 1;      ///< candidate steps kept by a run

			static COREARRAY_INLINE ssize_t Size(const TYPE &)
				{ return sizeof(TYPE); }
			static COREARRAY_INLINE bool Same(const TYPE &a, const TYPE &b)
				{ return a == b; }
			static COREARRAY_INLINE int Cand(const TYPE &x0, const TYPE &x1,
				TYPE out[])
			{
				out[0] = (TYPE)(UTYPE)((UTYPE)x1 - (UTYPE)x0);
				return 1;
			}
			static COREARRAY_INLINE TYPE Pred(const TYPE &v0, const TYPE &d,
				C_Int64 i)
			{
				return (TYPE)(UTYPE)((UTYPE)v0 +
					(UTYPE)((C_UInt64)i * (C_UInt64)(UTYPE)d));
			}
			static void Fill(TYPE *p, const TYPE &v0, const TYPE &d, C_Int64 i,
				ssize_t n)
			{
				UTYPE v = (UTYPE)Pred(v0, d, i), dd = (UTYPE)d;
				for (; n > 0; n--)
				{
					*p++ = (TYPE)v;
					v = (UTYPE)(v + dd);
				}
			}

			static COREARRAY_INLINE ssize_t Write(CdAllocator &A, const TYPE &v)
			{
				BYTE_LE<CdAllocator>(A) << v;
				return sizeof(TYPE);
			}
			static SIZE64 WriteArray(CdAllocator &A, const TYPE *p, ssize_t n)
			{
				BYTE_LE<CdAllocator>(A).W(p, n);
				return (SIZE64)n * sizeof(TYPE);
			}
			static COREARRAY_INLINE ssize_t Read(CdAllocator &A, TYPE &v)
			{
				BYTE_LE<CdAllocator>(A) >> v;
				return sizeof(TYPE);
			}
			static COREARRAY_INLINE void ReadArray(CdAllocator &A, TYPE *p,
				ssize_t n)
			{
				BYTE_LE<CdAllocator>(A).R(p, n);
			}
			static COREARRAY_INLINE ssize_t Skip(CdAllocator &A)
			{
				A.SetPosition(A.Position() + sizeof(TYPE));
				return sizeof(TYPE);
			}
		};


		/// Value coding of a floating-point type
		/** Two values are the same only if they have the same bits, so that
		 *  NA, NaN, -0 and 0 all keep their identities. **/
		template<typename TYPE> struct COREARRAY_DLL_DEFAULT RLE_FLOAT_CODEC
		{
			typedef TYPE TValue;
			static const bool FIXED = true;
			static const bool HAS_STEP = true;
			static const bool INT_STEP = false;
			static const int MAX_CAND = RLE_MAX_CAND;

			static COREARRAY_INLINE ssize_t Size(const TYPE &)
				{ return sizeof(TYPE); }
			static COREARRAY_INLINE bool Same(const TYPE &a, const TYPE &b)
				{ return memcmp(&a, &b, sizeof(TYPE)) == 0; }
			static COREARRAY_INLINE int Cand(const TYPE &x0, const TYPE &x1,
				TYPE out[])
				{ return RLE_StepCand(x0, x1, out); }
			static COREARRAY_INLINE TYPE Pred(const TYPE &v0, const TYPE &d,
				C_Int64 i)
				{ return RLE_StepValue(v0, d, i); }
			static COREARRAY_INLINE void Fill(TYPE *p, const TYPE &v0,
				const TYPE &d, C_Int64 i, ssize_t n)
				{ RLE_StepFill(p, v0, d, i, n); }

			static COREARRAY_INLINE ssize_t Write(CdAllocator &A, const TYPE &v)
			{
				BYTE_LE<CdAllocator>(A) << v;
				return sizeof(TYPE);
			}
			static SIZE64 WriteArray(CdAllocator &A, const TYPE *p, ssize_t n)
			{
				BYTE_LE<CdAllocator>(A).W(p, n);
				return (SIZE64)n * sizeof(TYPE);
			}
			static COREARRAY_INLINE ssize_t Read(CdAllocator &A, TYPE &v)
			{
				BYTE_LE<CdAllocator>(A) >> v;
				return sizeof(TYPE);
			}
			static COREARRAY_INLINE void ReadArray(CdAllocator &A, TYPE *p,
				ssize_t n)
			{
				BYTE_LE<CdAllocator>(A).R(p, n);
			}
			static COREARRAY_INLINE ssize_t Skip(CdAllocator &A)
			{
				A.SetPosition(A.Position() + sizeof(TYPE));
				return sizeof(TYPE);
			}
		};


		/// Value coding of a UTF-8 string
		/** A string is its length in bytes as a variable-length integer (7 bits
		 *  a byte, lowest first, 0x80 flagging another byte) followed by the
		 *  bytes. There is no step. **/
		struct COREARRAY_DLL_DEFAULT RLE_STR_CODEC
		{
			typedef UTF8String TValue;
			static const bool FIXED = false;
			static const bool HAS_STEP = false;
			static const bool INT_STEP = false;
			static const int MAX_CAND = 1;

			static COREARRAY_INLINE ssize_t Size(const UTF8String &s)
			{
				size_t n = s.size();
				ssize_t b = 1;
				for (; n >= 0x80; n >>= 7) b++;
				return b + (ssize_t)s.size();
			}
			static COREARRAY_INLINE bool Same(const UTF8String &a,
				const UTF8String &b)
				{ return a == b; }
			static COREARRAY_INLINE int Cand(const UTF8String &,
				const UTF8String &, UTF8String [])
				{ return 0; }
			static COREARRAY_INLINE UTF8String Pred(const UTF8String &v0,
				const UTF8String &, C_Int64)
				{ return v0; }
			static void Fill(UTF8String *p, const UTF8String &v0,
				const UTF8String &, C_Int64, ssize_t n)
				{ for (; n > 0; n--) *p++ = v0; }

			static ssize_t Write(CdAllocator &A, const UTF8String &v)
			{
				size_t n = v.size();
				ssize_t b = 0;
				do {
					C_UInt8 ch = n & 0x7F;
					n >>= 7;
					if (n > 0) ch |= 0x80;
					A.W8b(ch); b++;
				} while (n > 0);
				if (!v.empty()) A.WriteData(&v[0], v.size());
				return b + (ssize_t)v.size();
			}
			static SIZE64 WriteArray(CdAllocator &A, const UTF8String *p,
				ssize_t n)
			{
				SIZE64 sz = 0;
				for (; n > 0; n--) sz += Write(A, *p++);
				return sz;
			}
			/// read the length, and return the bytes it takes in b
			static size_t ReadLen(CdAllocator &A, ssize_t &b)
			{
				size_t n = 0;
				C_UInt8 ch, shl = 0;
				b = 0;
				do {
					ch = A.R8b(); b++;
					if (shl >= 63)
						throw ErrArray("Invalid string length in a run-length encoded array.");
					n |= size_t(ch & 0x7F) << shl;
					shl += 7;
				} while (ch & 0x80);
				return n;
			}
			static ssize_t Read(CdAllocator &A, UTF8String &v)
			{
				ssize_t b;
				size_t n = ReadLen(A, b);
				v.resize(n);
				if (n > 0) A.ReadData(&v[0], n);
				return b + (ssize_t)n;
			}
			static void ReadArray(CdAllocator &A, UTF8String *p, ssize_t n)
				{ for (; n > 0; n--) Read(A, *p++); }
			static ssize_t Skip(CdAllocator &A)
			{
				ssize_t b;
				size_t n = ReadLen(A, b);
				if (n > 0) A.SetPosition(A.Position() + n);
				return b + (ssize_t)n;
			}
		};


		/// Value coding of a run-length encoded array of TYPE
		template<typename TYPE> struct COREARRAY_DLL_DEFAULT RLE_CODEC { };

		template<> struct COREARRAY_DLL_DEFAULT RLE_CODEC<C_UInt8>:
			public RLE_INT_CODEC<C_UInt8, C_UInt8> { };
		template<> struct COREARRAY_DLL_DEFAULT RLE_CODEC<C_Int32>:
			public RLE_INT_CODEC<C_Int32, C_UInt32> { };
		template<> struct COREARRAY_DLL_DEFAULT RLE_CODEC<C_Int64>:
			public RLE_INT_CODEC<C_Int64, C_UInt64> { };
		template<> struct COREARRAY_DLL_DEFAULT RLE_CODEC<C_Float32>:
			public RLE_FLOAT_CODEC<C_Float32> { };
		template<> struct COREARRAY_DLL_DEFAULT RLE_CODEC<C_Float64>:
			public RLE_FLOAT_CODEC<C_Float64> { };
		template<> struct COREARRAY_DLL_DEFAULT RLE_CODEC<UTF8String>:
			public RLE_STR_CODEC { };
	}



	// =====================================================================
	// Run-length encoded arrays
	// =====================================================================

	/// The part of a run-length encoded array that does not depend on the
	/// value type: record headers and the checkpoint table
	class COREARRAY_DLL_DEFAULT CdRLEBase
	{
	public:
		/// the kind of a record
		enum TKind
		{
			rkLiteral = 0,  ///< values stored as they are
			rkRepeat  = 1,  ///< one value repeated
			rkStep    = 2   ///< an arithmetic progression
		};

		/// the largest number of values in a literal record
		static const int MAX_LITERAL = 128;

		/// constructor
		/** \param HasStep  whether the header can name a STEP record
		 *  \param Stride   a checkpoint is kept for the record holding each
		 *                  multiple of Stride
		**/
		CdRLEBase(bool HasStep, C_Int64 Stride);

	protected:
		const bool fHasStep;   ///< whether the header has a bit for STEP
		const C_Int64 fStride; ///< the elements between two checkpoints

		TdGDSBlockID fIndexingID;        ///< block ID of the checkpoint table
		CdBlockStream *fIndexingStream;  ///< the checkpoint table
		C_Int64 fNumIndex;               ///< the number of checkpoints
		vector<C_Int64> fArrayIndex;     ///< the elements of the checkpoints, loaded on demand
		vector<SIZE64> fArrayPos;        ///< the byte offsets of the checkpoints, loaded with fArrayIndex

		SIZE64 fTotalStreamSize;  ///< the bytes of the records in the data stream
		C_Int64 fStreamCount;     ///< the number of elements the records hold

		/// the bytes of the header of a record of L elements
		int HeaderSize(C_Int64 L, TKind Kind) const;
		/// encode the header of a record of L elements into s
		/** \return the number of bytes, at most 10 **/
		int PutHeader(C_UInt8 *s, C_Int64 L, TKind Kind) const;
		/// read the header of a record
		/** \return the number of bytes of the header **/
		int GetHeader(CdAllocator &A, TKind &Kind, C_Int64 &L) const;

		/// load the checkpoint table and the stream size
		void RleLoad(CdReader &Reader, CdBlockStream *GDSStream,
			CdPipeMgrItem *PipeInfo, CdAllocator &Allocator);
		/// save the block ID of the checkpoint table
		void RleSave(CdWriter &Writer, CdBlockStream *GDSStream);

		/// whether a record of L elements starting at element Start needs a
		/// checkpoint
		COREARRAY_INLINE bool CkWanted(C_Int64 Start, C_Int64 L) const
		{
			// the first multiple of the stride not before Start
			C_Int64 B = ((Start + fStride - 1) / fStride) * fStride;
			return (Start > 0) && (Start + L > B);
		}
		/// append a checkpoint: a record starts at element Index and byte Pos
		void CkStore(C_Int64 Index, SIZE64 Pos);
		/// the last checkpoint at or before element Index
		/** \return false if there is none **/
		bool CkLookup(C_Int64 Index, C_Int64 &OutIndex, SIZE64 &OutPos);
		/// drop the checkpoints of the records starting at or after element Index
		void CkTruncate(C_Int64 Index);

		/// the error for a data stream that does not decode
		static void ThrowCorrupt();

	private:
		/// load all checkpoints into fArrayIndex and fArrayPos
		void CkLoad();
	};


	/// Container of run-length encoded values
	/** \tparam TYPE  C_UInt8, C_Int32, C_Int64, C_Float32, C_Float64 or
	 *                UTF8String
	 *
	 *  Values are appended through an encoder which keeps in memory the values
	 *  whose record is not settled yet: at most 127 values waiting for a
	 *  literal record and the current run. They are written to the stream when
	 *  a record closes, and all of them by Synchronize() and CloseWriter().
	 *  Reading serves them from memory, so reading does not cut a run short.
	**/
	template<typename TYPE> class COREARRAY_DLL_DEFAULT CdRLEArray:
		public CdArray< TRLE<TYPE> >, public CdRLEBase
	{
	public:
		template<typename ALLOC_TYPE, typename MEM_TYPE> friend struct ALLOC_FUNC;

		typedef TYPE TValue;
		typedef _INTERNAL::RLE_CODEC<TYPE> Codec;
		typedef CdArray< TRLE<TYPE> > TBase;

		/// constructor
		CdRLEArray(): TBase(1),
			CdRLEBase(Codec::HAS_STEP, Codec::FIXED ? 65536 : 4096)
		{
			fRunV0 = TValue();
			for (int i=0; i < Codec::MAX_CAND; i++) fRunCand[i] = TValue();
			fRunNCand = 0;
			fRunLen = 0;
			fRunKind = rkLiteral;
			fRecIdx = fRecLen = fSubIdx = 0;
			fRecKind = rkLiteral;
			fRecPos = fRecData = fRecEnd = fSubPos = 0;
			fRecV0 = fRecD = TValue();
		}
		/// destructor
		virtual ~CdRLEArray()
		{
			// the base destructor can no longer reach the values held in memory
			try {
				EncFlush();
			} catch (...) { }
		}

		/// create a new CdRLEArray<TYPE> object
		virtual CdGDSObj *NewObject()
		{
			return (new CdRLEArray<TYPE>)->AssignPipe(*this);
		}

		/// synchronize data
		virtual void Synchronize()
		{
			if (PendingCount() > 0)
			{
				EncFlush();
				// the records just written have to reach the file as well
				this->fNeedUpdate = true;
			}
			TBase::Synchronize();
		}

		/// close the writing mode and sync the file
		virtual void CloseWriter()
		{
			EncFlush();
			TBase::CloseWriter();
		}

		/// get a list of CdBlockStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
		{
			TBase::GetOwnBlockStream(Out);
			if (fIndexingStream) Out.push_back(fIndexingStream);
		}

		/// get a list of CdStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<CdStream*> &Out)
		{
			TBase::GetOwnBlockStream(Out);
			if (fIndexingStream) Out.push_back(fIndexingStream);
		}

		/// the stream holding the checkpoint table
		virtual void GetIndexStream(vector<const CdBlockStream*> &Out) const
		{
			Out.clear();
			if (fIndexingStream) Out.push_back(fIndexingStream);
		}

		/// set the length of the specified dimension
		/** Values are stored in the order of the elements, so only the first
		 *  dimension (the last one in R) can change once there is data: a new
		 *  element is zero, or an empty string. **/
		virtual void SetDLen(int I, C_Int32 Value)
		{
			this->_CheckSetDLen(I, Value);
			if (this->fDimension[I].DimLen == Value) return;
			if ((I > 0) && (this->fTotalCount > 0))
			{
				throw ErrArray(
					"Only the last dimension of a run-length encoded array can be resized.");
			}
			CdAbstractArray::TArrayDim D;
			this->GetDim(D);
			D[I] = Value;
			this->ResetDim(D, this->DimCnt());
		}

		/// append new data from an iterator
		/** From an array of the same type, the records wholly in the range
		 *  are copied as they stand, and only a record the range starts or
		 *  ends inside of is decoded. **/
		virtual void AppendIter(CdIterator &I, C_Int64 Count)
		{
			if ((Count > 0) && (typeid(*this) == typeid(*I.Handler)) &&
				(I.Handler != this) && this->fAllocator.BufStream())
			{
				CdRLEArray<TYPE> *Src = static_cast<CdRLEArray<TYPE>*>(I.Handler);
				if (Src->fAllocator.BufStream() && (I.Ptr >= 0) &&
					(I.Ptr + Count <= Src->fStreamCount + Src->PendingCount()))
				{
					CheckWritable();
					CheckCount();
					this->_SetLargeBuffer();
					// the count follows what was appended, also when the
					// source fails to be read part-way
					try {
						AppendFrom(*Src, I.Ptr, Count);
					} catch (...) {
						UpdateCount();
						throw;
					}
					UpdateCount();
					I.Ptr += Count;
					return;
				}
			}
			TBase::AppendIter(I, Count);
		}

	protected:

		// ---- the encoder: values not written to the stream yet ----

		vector<TValue> fLit;     ///< values waiting for a literal record
		TValue fRunV0;           ///< the first value of the current run
		TValue fRunCand[Codec::MAX_CAND];  ///< the steps all run values agree with
		int fRunNCand;           ///< the number of steps in fRunCand
		C_Int64 fRunLen;         ///< the length of the current run, 0 if none
		TKind fRunKind;          ///< rkRepeat or rkStep, once fRunLen >= 2

		// ---- the cursor: the record last visited ----

		C_Int64 fRecIdx;   ///< the first element of the record
		C_Int64 fRecLen;   ///< the number of elements, 0 if there is no record
		TKind fRecKind;    ///< the kind of the record
		SIZE64 fRecPos;    ///< the byte offset of the header
		SIZE64 fRecData;   ///< the byte offset of the values
		SIZE64 fRecEnd;    ///< the byte offset of the next record, -1 if unknown yet
		TValue fRecV0;     ///< the value of REPEAT, or v0 of STEP
		TValue fRecD;      ///< the step of STEP
		C_Int64 fSubIdx;   ///< a string literal: element fRecIdx+fSubIdx ...
		SIZE64 fSubPos;    ///< ... starts at this byte offset


		/// get the size in byte corresponding to the count 'Num'
		/** Only the size of all records is meaningful: shrinking the array
		 *  goes through IterDone(), which has already cut the stream. **/
		virtual SIZE64 AllocSize(C_Int64)
		{
			return fTotalStreamSize;
		}

		/// append n zeros, or empty strings
		virtual void IterInit(CdIterator &, SIZE64 n)
		{
			if (n > 0)
			{
				EncPrepare();
				EncPushRepeat(TValue(), n);
			}
		}

		/// remove the last n elements
		virtual void IterDone(CdIterator &I, SIZE64 n)
		{
			if (n <= 0) return;
			if (I.Ptr + n != this->fTotalCount)
			{
				throw ErrArray(
					"A run-length encoded array can only remove its last elements.");
			}
			TruncateTo(I.Ptr);
		}

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			TBase::Loading(Reader, Version);
			RleLoad(Reader, this->fGDSStream, this->fPipeInfo, this->fAllocator);
			fStreamCount = this->fTotalCount;
			fLit.clear();
			fRunLen = 0;
			fRecLen = 0;
		}

		/// saving function for serialization
		virtual void Saving(CdWriter &Writer)
		{
			TBase::Saving(Writer);
			RleSave(Writer, this->fGDSStream);
		}


		/// fail now, not when the values in memory are written, if the stream
		/// cannot take more data
		void CheckWritable()
		{
			if (this->fPipeInfo && this->fAllocator.BufStream() &&
				!this->fPipeInfo->WriteMode(*this->fAllocator.BufStream()))
			{
				throw ErrAllocWrite();
			}
		}

		/// the number of values not written to the stream yet
		COREARRAY_INLINE C_Int64 PendingCount() const
		{
			return (C_Int64)fLit.size() + fRunLen;
		}

		/// the values in the stream and in memory must be all the elements
		/** They are not after a write error part-way; appending more would
		 *  then put the values at the wrong elements. **/
		void CheckCount()
		{
			if (this->fTotalCount != fStreamCount + PendingCount())
			{
				throw ErrArray(
					"The run-length encoded array is incomplete after a failed write.");
			}
		}

		/// set the total count and the first dimension to the values held
		void UpdateCount()
		{
			CdAllocArray::TDimItem &R = this->fDimension.front();
			this->fTotalCount = fStreamCount + PendingCount();
			if (this->fTotalCount >= R.DimElmCnt*(R.DimLen+1))
			{
				R.DimLen = this->fTotalCount / R.DimElmCnt;
				this->_SetFlushEvent();
				this->fNeedUpdate = true;
			}
		}

		/// the j-th value of the current run
		COREARRAY_INLINE TValue RunValue(C_Int64 j) const
		{
			if ((fRunLen >= 2) && (fRunKind == rkStep))
				return Codec::Pred(fRunV0, fRunCand[0], j);
			else
				return fRunV0;
		}

		/// the k-th value not written to the stream yet
		COREARRAY_INLINE TValue PendingValue(C_Int64 k) const
		{
			if (k < (C_Int64)fLit.size())
				return fLit[k];
			else
				return RunValue(k - (C_Int64)fLit.size());
		}


		// ---- encoding ----

		/// whether the current run takes fewer bytes as a record of its own
		/** A run that is not a record joins the literal values; a record in
		 *  their middle costs one more literal header besides its own. **/
		bool RunWorth() const
		{
			if (fRunLen < 2) return false;
			if (fRunLen >= 64) return true;
			C_Int64 w = Codec::Size(fRunV0);
			C_Int64 run = HeaderSize(fRunLen, fRunKind) +
				((fRunKind == rkStep) ? 2*w : w) + 1;
			return run <= fRunLen * w;
		}

		/// write the values waiting for a literal record
		void EmitLit()
		{
			const ssize_t n = fLit.size();
			if (n <= 0) return;
			const SIZE64 pos = fTotalStreamSize;
			this->fAllocator.SetPosition(pos);
			this->fAllocator.W8b((C_UInt8)((n - 1) << 1));
			SIZE64 sz = 1 + Codec::WriteArray(this->fAllocator, &fLit[0], n);
			if (CkWanted(fStreamCount, n)) CkStore(fStreamCount, pos);
			fTotalStreamSize += sz;
			fStreamCount += n;
			fLit.clear();
		}

		/// write the current run as a REPEAT or STEP record
		void EmitRun()
		{
			C_UInt8 h[16];
			int hn = PutHeader(h, fRunLen, fRunKind);
			const SIZE64 pos = fTotalStreamSize;
			this->fAllocator.SetPosition(pos);
			this->fAllocator.WriteData(h, hn);
			SIZE64 sz = hn + Codec::Write(this->fAllocator, fRunV0);
			if (fRunKind == rkStep)
				sz += Codec::Write(this->fAllocator, fRunCand[0]);
			if (CkWanted(fStreamCount, fRunLen)) CkStore(fStreamCount, pos);
			fTotalStreamSize += sz;
			fStreamCount += fRunLen;
			fRunLen = 0;
		}

		/// add a value to the literal values
		COREARRAY_INLINE void LitPush(const TValue &v)
		{
			fLit.push_back(v);
			if ((ssize_t)fLit.size() >= MAX_LITERAL) EmitLit();
		}

		/// append a value
		void EncPush(const TValue &x)
		{
			if (fRunLen == 0)
			{
				fRunV0 = x; fRunLen = 1;
				return;
			}
			if (fRunLen == 1)
			{
				if (Codec::Same(x, fRunV0))
				{
					fRunKind = rkRepeat; fRunLen = 2;
					return;
				}
				if (Codec::HAS_STEP)
				{
					fRunNCand = Codec::Cand(fRunV0, x, fRunCand);
					if (fRunNCand > 0)
					{
						fRunKind = rkStep; fRunLen = 2;
						return;
					}
				}
				LitPush(fRunV0);
				fRunV0 = x;
				return;
			}

			// a run of two or more values
			if (fRunKind == rkRepeat)
			{
				if (Codec::Same(x, fRunV0))
					{ fRunLen ++; return; }
			} else {
				int k = 0;
				for (int i=0; i < fRunNCand; i++)
				{
					if (Codec::Same(x, Codec::Pred(fRunV0, fRunCand[i], fRunLen)))
						fRunCand[k++] = fRunCand[i];
				}
				if (k > 0)
					{ fRunNCand = k; fRunLen ++; return; }
			}

			// x breaks the run
			if (RunWorth())
			{
				EmitLit(); EmitRun();
				fRunV0 = x; fRunLen = 1;
			} else {
				// too short to pay for a record: its first value becomes a
				// literal and the others are appended again, as they may
				// start a run with x
				const C_Int64 len = fRunLen;
				const TKind kind = fRunKind;
				const TValue v0 = fRunV0, d = fRunCand[0];
				LitPush(v0);
				fRunLen = 0;
				for (C_Int64 j=1; j < len; j++)
					EncPush((kind == rkRepeat) ? v0 : Codec::Pred(v0, d, j));
				EncPush(x);
			}
		}

		/// append n copies of v
		void EncPushRepeat(const TValue &v, C_Int64 n)
		{
			for (; n > 0; n--)
			{
				if ((fRunLen >= 2) && (fRunKind == rkRepeat) &&
					Codec::Same(fRunV0, v))
				{
					fRunLen += n;
					return;
				}
				EncPush(v);
			}
		}

		/// append a value, the usual case of continuing a run done inline
		COREARRAY_INLINE void EncPushOne(const TValue &v)
		{
			if (fRunLen >= 2)
			{
				if (fRunKind == rkRepeat)
				{
					if (Codec::Same(v, fRunV0))
						{ fRunLen ++; return; }
				} else if (Codec::INT_STEP)
				{
					if (Codec::Same(v, Codec::Pred(fRunV0, fRunCand[0], fRunLen)))
						{ fRunLen ++; return; }
				}
			}
			EncPush(v);
		}

		/// write all values held in memory
		void EncFlush()
		{
			if (fRunLen > 0)
			{
				if (RunWorth())
				{
					EmitLit(); EmitRun();
				} else {
					// a run too short to be a record has fewer than 64 values
					for (C_Int64 j=0; j < fRunLen; j++)
						LitPush(RunValue(j));
					fRunLen = 0;
				}
			}
			EmitLit();
		}

		/// before appending: the last record goes back into memory, so that
		/// the values appended can continue it
		/** Only a data stream without compression can be cut. The encoder is
		 *  empty just after the values were written out or the array was
		 *  loaded, which is when the stream ends with a closed record. **/
		void EncPrepare()
		{
			if ((fRunLen == 0) && fLit.empty() && (fStreamCount > 0) &&
				!this->fPipeInfo && this->fAllocator.BufStream() &&
				this->fGDSStream && !this->fGDSStream->ReadOnly())
			{
				Seek(fStreamCount - 1);
				CutAt(fRecLen);
			}
		}

		/// cut the stream at the record of the cursor, and append again its
		/// first Keep values
		/** The encoder must be empty. **/
		void CutAt(C_Int64 Keep)
		{
			const C_Int64 s = fRecIdx;
			const SIZE64 pos = fRecPos;
			const TKind kind = fRecKind;
			const TValue v0 = fRecV0, d = fRecD;
			vector<TValue> lit;
			if ((kind == rkLiteral) && (Keep > 0))
			{
				lit.resize(Keep);
				RecRead(&lit[0], 0, Keep);
			}
			// the checkpoints go first: they must never point beyond the data
			CkTruncate(s);
			this->fAllocator.SetSize(pos);
			fTotalStreamSize = pos;
			fStreamCount = s;
			fRecLen = 0;
			switch (kind)
			{
			case rkLiteral:
				for (C_Int64 i=0; i < Keep; i++) EncPushOne(lit[i]);
				break;
			case rkRepeat:
				EncPushRepeat(v0, Keep);
				break;
			default:
				if (Keep >= 2)
				{
					fRunV0 = v0; fRunCand[0] = d; fRunNCand = 1;
					fRunKind = rkStep; fRunLen = Keep;
				} else if (Keep == 1)
					EncPush(v0);
			}
		}

		/// keep the first Num elements
		void TruncateTo(C_Int64 Num)
		{
			if (Num >= fStreamCount)
			{
				// only values in memory go
				C_Int64 k = Num - fStreamCount;
				if (k <= (C_Int64)fLit.size())
				{
					fLit.resize(k);
					fRunLen = 0;
				} else
					fRunLen = k - (C_Int64)fLit.size();
				return;
			}
			if (this->fPipeInfo)
			{
				throw ErrArray(
					"A compressed run-length encoded array cannot remove the elements already written.");
			}
			fLit.clear();
			fRunLen = 0;
			Seek(Num);
			CutAt(Num - fRecIdx);
		}


		// ---- decoding ----

		/// load the record starting at element Idx and byte Pos to the cursor
		void LoadRecordAt(C_Int64 Idx, SIZE64 Pos)
		{
			fRecLen = 0;
			if ((Idx >= fStreamCount) || (Pos >= fTotalStreamSize))
				ThrowCorrupt();
			this->fAllocator.SetPosition(Pos);
			TKind kind;
			C_Int64 L;
			const SIZE64 data = Pos + GetHeader(this->fAllocator, kind, L);
			if (L > fStreamCount - Idx) ThrowCorrupt();
			switch (kind)
			{
			case rkLiteral:
				fRecEnd = Codec::FIXED ?
					(data + L * (SIZE64)sizeof(TValue)) : (SIZE64)-1;
				fSubIdx = 0; fSubPos = data;
				break;
			case rkRepeat:
				fRecEnd = data + Codec::Read(this->fAllocator, fRecV0);
				break;
			default:
				fRecEnd = data + Codec::Read(this->fAllocator, fRecV0);
				fRecEnd += Codec::Read(this->fAllocator, fRecD);
			}
			if (fRecEnd > fTotalStreamSize) ThrowCorrupt();
			fRecIdx = Idx; fRecPos = Pos; fRecKind = kind;
			fRecData = data; fRecLen = L;
		}

		/// a string literal: move the sub-cursor to element fRecIdx + Off
		void LitSeek(C_Int64 Off)
		{
			if (Off < fSubIdx)
				{ fSubIdx = 0; fSubPos = fRecData; }
			this->fAllocator.SetPosition(fSubPos);
			for (; fSubIdx < Off; fSubIdx++)
				fSubPos += Codec::Skip(this->fAllocator);
		}

		/// move the cursor to the next record
		void NextRecord()
		{
			if (fRecEnd < 0)
			{
				LitSeek(fRecLen);
				fRecEnd = fSubPos;
				if (fRecEnd > fTotalStreamSize) ThrowCorrupt();
			}
			LoadRecordAt(fRecIdx + fRecLen, fRecEnd);
		}

		/// move the cursor to the record holding element Idx < fStreamCount
		void Seek(C_Int64 Idx)
		{
			C_Int64 ci; SIZE64 cp;
			if ((fRecLen > 0) && (fRecIdx <= Idx))
			{
				if (Idx < fRecIdx + fRecLen) return;
				// walking on from the cursor, unless the target is far enough
				// ahead for a checkpoint to be closer: reading on sequentially
				// never needs the table
				if ((Idx - (fRecIdx + fRecLen) >= fStride) &&
						CkLookup(Idx, ci, cp) && (ci > fRecIdx))
					LoadRecordAt(ci, cp);
			} else {
				if (CkLookup(Idx, ci, cp))
					LoadRecordAt(ci, cp);
				else
					LoadRecordAt(0, 0);
			}
			while (Idx >= fRecIdx + fRecLen)
				NextRecord();
		}

		/// read m values of the cursor record, from its element Off on
		template<typename MEM_TYPE>
			MEM_TYPE *RecRead(MEM_TYPE *p, C_Int64 Off, ssize_t m)
		{
			switch (fRecKind)
			{
			case rkRepeat:
				{
					MEM_TYPE v = VAL_CONVERT(MEM_TYPE, TValue, fRecV0);
					for (; m > 0; m--) *p++ = v;
					return p;
				}
			case rkStep:
				return StepRead(p, fRecV0, fRecD, Off, m);
			default:
				if (Codec::FIXED)
				{
					TValue buf[MAX_LITERAL];
					this->fAllocator.SetPosition(fRecData + Off * sizeof(TValue));
					Codec::ReadArray(this->fAllocator, buf, m);
					return VAL_CONV<MEM_TYPE, TValue>::Cvt(p, buf, m);
				} else {
					LitSeek(Off);
					TValue v;
					for (; m > 0; m--)
					{
						fSubPos += Codec::Read(this->fAllocator, v);
						fSubIdx ++;
						*p++ = VAL_CONVERT(MEM_TYPE, TValue, v);
					}
					return p;
				}
			}
		}

		/// read m values of the cursor record with a selection
		template<typename MEM_TYPE>
			MEM_TYPE *RecReadSel(MEM_TYPE *p, C_Int64 Off, ssize_t m,
				const C_BOOL sel[])
		{
			switch (fRecKind)
			{
			case rkRepeat:
				{
					ssize_t c = 0;
					for (ssize_t i=0; i < m; i++) if (sel[i]) c++;
					if (c > 0)
					{
						MEM_TYPE v = VAL_CONVERT(MEM_TYPE, TValue, fRecV0);
						for (; c > 0; c--) *p++ = v;
					}
					return p;
				}
			case rkStep:
				{
					TValue buf[RLE_BUF];
					while (m > 0)
					{
						ssize_t k = (m <= RLE_BUF) ? m : RLE_BUF;
						Codec::Fill(buf, fRecV0, fRecD, Off, k);
						p = VAL_CONV<MEM_TYPE, TValue>::CvtSub(p, buf, k, sel);
						Off += k; m -= k; sel += k;
					}
					return p;
				}
			default:
				if (Codec::FIXED)
				{
					TValue buf[MAX_LITERAL];
					this->fAllocator.SetPosition(fRecData + Off * sizeof(TValue));
					Codec::ReadArray(this->fAllocator, buf, m);
					return VAL_CONV<MEM_TYPE, TValue>::CvtSub(p, buf, m, sel);
				} else {
					LitSeek(Off);
					TValue v;
					for (; m > 0; m--)
					{
						if (*sel++)
						{
							fSubPos += Codec::Read(this->fAllocator, v);
							*p++ = VAL_CONVERT(MEM_TYPE, TValue, v);
						} else
							fSubPos += Codec::Skip(this->fAllocator);
						fSubIdx ++;
					}
					return p;
				}
			}
		}

		/// read m values of a step run, from its element Off on
		template<typename MEM_TYPE>
			MEM_TYPE *StepRead(MEM_TYPE *p, const TValue &v0, const TValue &d,
				C_Int64 Off, ssize_t m)
		{
			TValue buf[RLE_BUF];
			while (m > 0)
			{
				ssize_t k = (m <= RLE_BUF) ? m : RLE_BUF;
				Codec::Fill(buf, v0, d, Off, k);
				p = VAL_CONV<MEM_TYPE, TValue>::Cvt(p, buf, k);
				Off += k; m -= k;
			}
			return p;
		}

		/// read m values held in memory, from the k-th one on
		template<typename MEM_TYPE>
			MEM_TYPE *PendRead(MEM_TYPE *p, C_Int64 k, ssize_t m)
		{
			for (; (m > 0) && (k < (C_Int64)fLit.size()); m--, k++)
				*p++ = VAL_CONVERT(MEM_TYPE, TValue, fLit[k]);
			if (m > 0)
			{
				if ((fRunLen >= 2) && (fRunKind == rkStep))
				{
					p = StepRead(p, fRunV0, fRunCand[0],
						k - (C_Int64)fLit.size(), m);
				} else {
					MEM_TYPE v = VAL_CONVERT(MEM_TYPE, TValue, fRunV0);
					for (; m > 0; m--) *p++ = v;
				}
			}
			return p;
		}

		/// read m values held in memory with a selection
		template<typename MEM_TYPE>
			MEM_TYPE *PendReadSel(MEM_TYPE *p, C_Int64 k, ssize_t m,
				const C_BOOL sel[])
		{
			for (; m > 0; m--, k++)
			{
				if (*sel++)
					*p++ = VAL_CONVERT(MEM_TYPE, TValue, PendingValue(k));
			}
			return p;
		}


		// ---- appending from an array of the same type ----

		/// append the values of Src[Start, Start+Count)
		void AppendFrom(CdRLEArray<TYPE> &Src, C_Int64 Start, C_Int64 Count)
		{
			const C_Int64 End = Start + Count;
			const C_Int64 SrcN = Src.fStreamCount;
			const C_Int64 RecEnd = (End <= SrcN) ? End : SrcN;
			C_Int64 idx = Start;
			EncPrepare();
			if (idx < RecEnd)
			{
				// a record the range starts inside of is decoded
				Src.Seek(idx);
				if (Src.fRecIdx < idx)
				{
					C_Int64 e = Src.fRecIdx + Src.fRecLen;
					if (e > RecEnd) e = RecEnd;
					FeedFrom(Src, idx, e - idx);
					idx = e;
				}
				// the whole records in [idx, b2) are copied
				if (idx < RecEnd)
				{
					C_Int64 b2 = SrcN;
					SIZE64 P2 = Src.fTotalStreamSize;
					if (End < SrcN)
					{
						Src.Seek(End);
						b2 = Src.fRecIdx; P2 = Src.fRecPos;
					}
					if (b2 > idx)
					{
						Src.Seek(idx);
						CopyRecords(Src, idx, Src.fRecPos, b2, P2);
						idx = b2;
					}
				}
				// and a record the range ends inside of is decoded
				if (idx < RecEnd)
				{
					FeedFrom(Src, idx, RecEnd - idx);
					idx = RecEnd;
				}
			}
			// the values Src holds in memory
			for (; idx < End; idx++)
				EncPush(Src.PendingValue(idx - SrcN));
		}

		/// append the values of the records of Src[Start, Start+Count)
		void FeedFrom(CdRLEArray<TYPE> &Src, C_Int64 Start, C_Int64 Count)
		{
			while (Count > 0)
			{
				Src.Seek(Start);
				C_Int64 off = Start - Src.fRecIdx;
				C_Int64 m = Src.fRecLen - off;
				if (m > Count) m = Count;
				if (Src.fRecKind == rkRepeat)
				{
					EncPushRepeat(Src.fRecV0, m);
				} else {
					if (m > RLE_BUF) m = RLE_BUF;
					TValue buf[RLE_BUF];
					Src.RecRead(buf, off, m);
					for (C_Int64 i=0; i < m; i++) EncPushOne(buf[i]);
				}
				Start += m; Count -= m;
			}
		}

		/// copy the records of Src holding the elements [B1, B2), which
		/// take the bytes [P1, P2) of its stream
		void CopyRecords(CdRLEArray<TYPE> &Src, C_Int64 B1, SIZE64 P1,
			C_Int64 B2, SIZE64 P2)
		{
			// the stream has to end with a closed record
			EncFlush();
			const C_Int64 i0 = fStreamCount;
			const SIZE64 p0 = fTotalStreamSize;
			// the checkpoints the records need in this array
			vector<C_Int64> ck_idx;
			vector<SIZE64> ck_pos;
			Src.LoadRecordAt(B1, P1);
			for (;;)
			{
				C_Int64 di = i0 + (Src.fRecIdx - B1);
				if (CkWanted(di, Src.fRecLen))
				{
					ck_idx.push_back(di);
					ck_pos.push_back(p0 + (Src.fRecPos - P1));
				}
				if (Src.fRecIdx + Src.fRecLen >= B2) break;
				Src.NextRecord();
			}
			// going through the allocators keeps the copy in the coordinates
			// that P1 and P2 are given in, with or without compression
			Src.fAllocator.SetPosition(P1);
			this->fAllocator.SetPosition(p0);
			C_UInt8 Buf[COREARRAY_STREAM_BUFFER];
			for (SIZE64 n = P2 - P1; n > 0; )
			{
				ssize_t m = (n <= (SIZE64)sizeof(Buf)) ? n : sizeof(Buf);
				Src.fAllocator.ReadData(Buf, m);
				this->fAllocator.WriteData(Buf, m);
				n -= m;
			}
			// the checkpoints go in once the bytes they point at are there
			for (size_t i=0; i < ck_idx.size(); i++)
				CkStore(ck_idx[i], ck_pos[i]);
			fTotalStreamSize += P2 - P1;
			fStreamCount += B2 - B1;
		}

		/// the number of values decoded at a time
		static const ssize_t RLE_BUF = 256;
	};

	template<typename TYPE> const ssize_t CdRLEArray<TYPE>::RLE_BUF;


	// =====================================================================
	// Template for Allocator for run-length encoded array
	// =====================================================================

	/// Template functions for allocator of run-length encoded array
	template<typename TYPE, typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<TRLE<TYPE>, MEM_TYPE>
	{
		typedef CdRLEArray<TYPE> TArray;
		typedef typename TArray::TValue TValue;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			TArray *IT = static_cast<TArray*>(I.Handler);
			C_Int64 idx = I.Ptr;
			// the values in records
			while ((n > 0) && (idx < IT->fStreamCount))
			{
				IT->Seek(idx);
				C_Int64 off = idx - IT->fRecIdx;
				C_Int64 m = IT->fRecLen - off;
				if (m > n) m = n;
				p = IT->RecRead(p, off, m);
				idx += m; n -= m;
			}
			// the values in memory
			if (n > 0)
			{
				p = IT->PendRead(p, idx - IT->fStreamCount, n);
				idx += n;
			}
			I.Ptr = idx;
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			TArray *IT = static_cast<TArray*>(I.Handler);
			C_Int64 idx = I.Ptr;
			while (n > 0)
			{
				// elements not selected are passed over without decoding
				for (; (n > 0) && !*sel; n--, sel++) idx++;
				if (n <= 0) break;
				if (idx < IT->fStreamCount)
				{
					IT->Seek(idx);
					C_Int64 off = idx - IT->fRecIdx;
					C_Int64 m = IT->fRecLen - off;
					if (m > n) m = n;
					p = IT->RecReadSel(p, off, m, sel);
					idx += m; n -= m; sel += m;
				} else {
					p = IT->PendReadSel(p, idx - IT->fStreamCount, n, sel);
					idx += n; n = 0;
				}
			}
			I.Ptr = idx;
			return p;
		}

		/// write an array to CdAllocator, appending only
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			TArray *IT = static_cast<TArray*>(I.Handler);
			if (I.Ptr != IT->fTotalCount)
			{
				throw ErrArray(
					"A run-length encoded array can only be appended to.");
			}
			IT->CheckWritable();
			IT->CheckCount();
			IT->EncPrepare();
			I.Ptr += n;
			for (; n > 0; n--)
				IT->EncPushOne(VAL_CONVERT(TValue, MEM_TYPE, *p++));
			return p;
		}
	};


	// =====================================================================
	// Run-length encoded arrays in GDS files
	// =====================================================================

	typedef CdRLEArray<C_UInt8>     CdRleUInt8;
	typedef CdRLEArray<C_Int32>     CdRleInt32;
	typedef CdRLEArray<C_Int64>     CdRleInt64;
	typedef CdRLEArray<C_Float32>   CdRleReal32;
	typedef CdRLEArray<C_Float64>   CdRleReal64;
	typedef CdRLEArray<UTF8String>  CdRleStr8;
}

#endif /* _HEADER_COREARRAY_RLE_GDS_ */
