// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStrGDS.h: GDS format with character types and functions
//
// Copyright (C) 2007-2020    Xiuwen Zheng
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
 *	\file     dStrGDS.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007 - 2020
 *	\brief    GDS format with character types and functions
 *	\details
**/

#ifndef _HEADER_COREARRAY_STRING_GDS_
#define _HEADER_COREARRAY_STRING_GDS_

#include "dStruct.h"

#include <typeinfo>


namespace CoreArray
{
	// =======================================================================
	// Persistent offset index for variable-length strings
	// =======================================================================

	/// On-disk checkpoint table mapping element index to byte offset
	/** Variable-length string containers store nothing but the concatenated
	 *  encoding, so locating element N means walking N elements from the start.
	 *  This object keeps a checkpoint every STRIDE elements in a GDS block
	 *  stream, turning that walk into an O(1) jump plus a bounded scan.
	 *
	 *  Layout: a fixed header followed by one TdGDSPos per checkpoint, where
	 *  checkpoint i (i >= 1) holds the byte offset of element i*STRIDE; element
	 *  0 is implicitly at offset 0, so it takes no space.
	 *
	 *  The header records the element count and stream size the table was built
	 *  for, and the table is trusted only on an exact match. Counting the
	 *  checkpoints is not enough on its own: an in-place overwrite shifts every
	 *  later offset while leaving their number untouched, so a table left
	 *  behind by a writer that does not maintain it looks the right size.
	**/
	class COREARRAY_DLL_DEFAULT CdStrIndexing
	{
	public:
		/// the number of elements between two adjacent checkpoints
		static const C_Int64 STRIDE = 65536;
		/// identifies the block and the layout revision
		/** Little-endian this is the bytes 'S' 'D' 'X' '1' -- string index,
		 *  revision 1 -- so the block is recognisable in a hex dump. Bump the
		 *  trailing digit if the layout below ever changes: Load() rejects any
		 *  value it does not know, which degrades to having no table rather
		 *  than misreading one. **/
		static const C_UInt32 MAGIC = 0x31584453;
		/// magic + element count + stream size
		static const SIZE64 HEAD_SIZE = sizeof(C_UInt32) + 2*GDS_POS_SIZE;
		/// the node property holding the block ID of the table
		static const char *VarName() { return "INDEX"; }

		CdStrIndexing() { fStream = NULL; fValid = fStale = false; }

		COREARRAY_INLINE CdBlockStream *Stream() const { return fStream; }
		/// whether the table may be used for lookups
		COREARRAY_INLINE bool Valid() const { return fValid; }

		void Clear() { fStream = NULL; fValid = fStale = false; }

		/// attach a freshly created block stream, ready to be filled
		void Create(CdBlockStream *Stream)
		{
			fStream = Stream;
			fValid = (Stream != NULL);
			fStale = false;
			// reserve the header up front: CdBlockStream refuses a seek past
			// its end, so the first checkpoint write would otherwise land at
			// HEAD_SIZE in a still-empty block
			if (fStream && (fStream->GetSize() < HEAD_SIZE))
				fStream->SetSize(HEAD_SIZE);
		}

		/// attach a loaded block stream, trusting it only if the header agrees
		void Load(CdBlockStream *Stream, C_Int64 Count, SIZE64 Size)
		{
			fStream = Stream;
			fValid = false;
			// Refusing the table for lookups is not enough: it must also be
			// refused for writing. Appending to a node whose table went stale
			// would file correct new checkpoints among the stale ones, and the
			// next Flush() would stamp that mixture as trustworthy.
			fStale = true;
			if (!Stream || (Stream->GetSize() < HEAD_SIZE)) return;
			Stream->SetPosition(0);
			BYTE_LE<CdStream> R(Stream);
			C_UInt32 magic = 0;
			R >> magic;
			if (magic != MAGIC) return;
			TdGDSPos cnt, size;
			R >> cnt; R >> size;
			// an exact match, or the table describes data that has since been
			// changed by something that does not maintain it
			if ((SIZE64(cnt) != Count) || (SIZE64(size) != Size)) return;
			// enough checkpoints for the elements claimed?
			if (Stream->GetSize() < HEAD_SIZE + (Count / STRIDE) * GDS_POS_SIZE)
				return;
			fValid = true;
			fStale = false;
		}

		/// the last checkpoint at or before Index, if there is a usable one
		/** \return true if OutIndex/OutPos were set to a checkpoint **/
		bool Lookup(C_Int64 Index, C_Int64 &OutIndex, SIZE64 &OutPos) const
		{
			if (!fValid || (Index < STRIDE)) return false;
			C_Int64 i = Index / STRIDE;
			// checkpoints are appended in order, so the stream size says how
			// many exist -- during writing the table trails the data, and a
			// lookup past its end has to settle for the last one written
			C_Int64 avail = (fStream->GetSize() - HEAD_SIZE) / GDS_POS_SIZE;
			if (i > avail) i = avail;
			if (i < 1) return false;
			fStream->SetPosition(HEAD_SIZE + (i-1)*GDS_POS_SIZE);
			TdGDSPos pos;
			BYTE_LE<CdStream>(fStream) >> pos;
			OutIndex = i * STRIDE;
			OutPos = pos;
			return true;
		}

		/// record the byte offset of element Index, a multiple of STRIDE
		void Store(C_Int64 Index, SIZE64 Pos)
		{
			if (!fStream || fStale || (Index < STRIDE)) return;
			C_Int64 i = Index / STRIDE;
			SIZE64 at = HEAD_SIZE + (i-1)*GDS_POS_SIZE;
			// grow first, for the same reason Create() reserves the header
			if (fStream->GetSize() < at + GDS_POS_SIZE)
				fStream->SetSize(at + GDS_POS_SIZE);
			fStream->SetPosition(at);
			BYTE_LE<CdStream>(fStream) << TdGDSPos(Pos);
		}

		/// add Delta to every checkpoint that sits after element Index
		/** An in-place overwrite whose encoded length changed moves everything
		 *  after that element by a constant, so the checkpoints beyond it stay
		 *  correct once shifted -- no rescan is needed. **/
		void Shift(C_Int64 Index, SIZE64 Delta)
		{
			if (!fStream || fStale || (Delta == 0)) return;
			C_Int64 avail = (fStream->GetSize() - HEAD_SIZE) / GDS_POS_SIZE;
			// checkpoint i marks element i*STRIDE; it moved only if that
			// element lies strictly after the one just rewritten
			for (C_Int64 i = Index / STRIDE + 1; i <= avail; i++)
			{
				SIZE64 at = HEAD_SIZE + (i-1)*GDS_POS_SIZE;
				fStream->SetPosition(at);
				TdGDSPos pos;
				BYTE_LE<CdStream>(fStream) >> pos;
				fStream->SetPosition(at);
				BYTE_LE<CdStream>(fStream) << TdGDSPos(SIZE64(pos) + Delta);
			}
		}

		/// drop the checkpoints that no longer describe any data
		void Truncate(C_Int64 Count)
		{
			if (!fStream || fStale) return;
			SIZE64 want = HEAD_SIZE + (Count / STRIDE) * GDS_POS_SIZE;
			if (fStream->GetSize() > want) fStream->SetSize(want);
		}

		/// stamp the header with what the table now describes
		void Flush(C_Int64 Count, SIZE64 Size)
		{
			if (!fStream || fStale) return;
			fStream->SetPosition(0);
			BYTE_LE<CdStream> W(fStream);
			W << C_UInt32(MAGIC);
			W << TdGDSPos(Count);
			W << TdGDSPos(Size);
			fValid = true;
		}

	protected:
		CdBlockStream *fStream;  ///< the GDS stream holding the table
		bool fValid;             ///< header agrees with the container
		bool fStale;             ///< offsets no longer describe the data
	};


	// =======================================================================
	// Fixed-length string
	// =======================================================================

	/// Fixed-length array
	/** \tparam TYPE  data type, e.g C_UTF8, C_UTF16 and C_UTF32
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT FIXED_LEN
	{
		typedef TYPE TType;
	};


	template<> struct COREARRAY_DLL_DEFAULT TdTraits< FIXED_LEN<C_UTF8> >
	{
		typedef UTF8String TType;
		typedef C_UTF8 ElmType;
		typedef char RawType;
		static const int trVal = COREARRAY_TR_FIXED_LEN_STRING;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF8;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dFStr8"; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< FIXED_LEN<C_UTF16> >
	{
		typedef UTF16String TType;
		typedef C_UTF16 ElmType;
		typedef C_UTF16 RawType;
		static const int trVal = COREARRAY_TR_FIXED_LEN_STRING;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF16;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dFStr16"; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< FIXED_LEN<C_UTF32> >
	{
		typedef UTF32String TType;
		typedef C_UTF32 ElmType;
		typedef C_UTF32 RawType;
		static const int trVal = COREARRAY_TR_FIXED_LEN_STRING;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svCustomStr;

		static const char *TraitName() { return StreamName()+1; }
		static const char *StreamName() { return "dFStr32"; }
	};



	/// Fixed-length string container
	/** \tparam TYPE  should be C_UTF8, C_UTF16 or C_UTF32
	 *  \sa  CdFStr8, CdFStr16, CdFStr32
	**/
	template<typename TYPE> class COREARRAY_DLL_DEFAULT CdFixedStr:
		public CdArray< FIXED_LEN<TYPE> >
	{
	public:
		typedef typename TdTraits< FIXED_LEN<TYPE> >::TType TType;
		typedef TYPE ElmType;

		CdFixedStr(): CdArray< FIXED_LEN<TYPE> >()
		{
			this->vElmSize_Ptr = 0;
		}

		virtual CdGDSObj *NewObject()
		{
			return (new CdFixedStr<TYPE>)->AssignPipe(*this);
		}

		COREARRAY_INLINE ssize_t MaxLength() const
		{
			return this->fElmSize / (TdTraits<TYPE>::BitOf/8);
		}

		/// set the maximum length of fixed-length string
		void SetMaxLength(ssize_t NewLen)
		{
			if (NewLen > 0)
				this->SetElmSize(NewLen * (TdTraits<TYPE>::BitOf/8));
			else
				throw ErrArray("CdFixedStr::SetMaxLength, invalid parameter.");
		}

	protected:

		/// loading function for serialization
		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			static const char *VAR_ESIZE = "ESIZE";
			C_UInt32 esize = 0;
			Reader[VAR_ESIZE] >> esize;
			this->vElmSize_Ptr = Reader.PropPosition(VAR_ESIZE);
			this->fElmSize = esize;
			CdAllocArray::Loading(Reader, Version);
		}

		/// saving function for serialization
		virtual void Saving(CdWriter &Writer)
		{
			static const char *VAR_ESIZE = "ESIZE";
			Writer[VAR_ESIZE] << C_UInt32(this->fElmSize);
			this->vElmSize_Ptr = Writer.PropPosition(VAR_ESIZE);
			CdAllocArray::Saving(Writer);
		}

		virtual void UpdateInfoExt(CdBufStream *Sender)
		{
			if (this->vElmSize_Ptr != 0)
			{
				this->fGDSStream->SetPosition(this->vElmSize_Ptr);
				BYTE_LE<CdStream>(this->fGDSStream) << C_UInt32(this->fElmSize);
			}
		}

	private:
		SIZE64 vElmSize_Ptr;
	};



	/// Template functions for allocator
	
	template<typename TYPE, typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<FIXED_LEN<TYPE>, MEM_TYPE>
	{
		/// string type
		typedef typename TdTraits< FIXED_LEN<TYPE> >::TType StrType;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			const typename TdTraits< FIXED_LEN<TYPE> >::RawType ZERO_CHAR = 0;
			const ssize_t ElmSize =
				static_cast<CdAllocArray*>(I.Handler)->ElmSize();
			const ssize_t N = ElmSize / sizeof(TYPE);
			StrType s(N, ZERO_CHAR), ss;

			// TODO: check string object
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n * ElmSize;
			for (; n > 0; n--)
			{
				s.resize(N);
				I.Allocator->ReadData((void*)s.c_str(), ElmSize);
				size_t pos = s.find(ZERO_CHAR);
				if (pos != string::npos) s.resize(pos);
				COREARRAY_ENDIAN_LE_TO_NT_ARRAY((TYPE*)s.c_str(), s.size());
				ss.assign(s.begin(), s.end()); // compatible with platforms
				ValCvtArray(p, &ss, 1);
				p ++;
			}
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			const ssize_t ElmSize =
				static_cast<CdAllocArray*>(I.Handler)->ElmSize();
			for (; n>0 && !*sel; n--, sel++) I.Ptr += ElmSize;
			const typename TdTraits< FIXED_LEN<TYPE> >::RawType ZERO_CHAR = 0;
			const ssize_t N = ElmSize / sizeof(TYPE);
			StrType s(N, ZERO_CHAR), ss;

			// TODO: check string object
			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n * ElmSize;
			for (; n > 0; n--)
			{
				if (*sel++)
				{
					s.resize(N);
					I.Allocator->ReadData((void*)s.c_str(), ElmSize);
					size_t pos = s.find(ZERO_CHAR);
					if (pos != string::npos) s.resize(pos);
					COREARRAY_ENDIAN_LE_TO_NT_ARRAY((TYPE*)s.c_str(), s.size());
					ss.assign(s.begin(), s.end()); // compatible with platforms
					ValCvtArray(p, &ss, 1);
					p ++;
				} else {
					I.Allocator->SetPosition(
						I.Allocator->Position() + ElmSize);
				}
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			CdFixedStr<TYPE> *Ary = static_cast< CdFixedStr<TYPE>* >(I.Handler);
			ssize_t ElmSize = Ary->ElmSize();
			StrType s;

			// determine whether need to extend ElmSize
			ssize_t MaxSize = 0;
			const MEM_TYPE *pp = p;
			for (ssize_t m=n; m > 0; m--, pp++)
			{
				VAL_CONV<StrType, MEM_TYPE>::Cvt(&s, pp, 1);
				ssize_t L = s.size()*sizeof(TYPE);
				if (L > MaxSize) MaxSize = L;
			}

			if (MaxSize > ElmSize)
			{
				Ary->SetMaxLength(MaxSize / sizeof(TYPE));
				I.Ptr = (I.Ptr / ElmSize) * MaxSize;
				ElmSize = MaxSize;
			}

			I.Allocator->SetPosition(I.Ptr);
			I.Ptr += n * ElmSize;
			const size_t N = ElmSize / sizeof(TYPE);

			for (; n > 0; n--)
			{
				VAL_CONV<StrType, MEM_TYPE>::Cvt(&s, p, 1);
				COREARRAY_ENDIAN_NT_TO_LE_ARRAY((TYPE*)s.c_str(), s.size());
				s.resize(N, 0);
				I.Allocator->WriteData(s.c_str(), ElmSize);
				p ++;
			}

			return p;
		}
	};


	/// Fixed-length UTF-8 string
	typedef CdFixedStr<C_UTF8>     CdFStr8;
	/// Fixed-length UTF-16 string
	typedef CdFixedStr<C_UTF16>    CdFStr16;
	/// Fixed-length UTF-32 string
	typedef CdFixedStr<C_UTF32>    CdFStr32;



	// =======================================================================
	// Variable-length null-terminated string (C string)
	// =======================================================================

	/// Variable-length array
	/** \tparam TYPE  data type, e.g C_UTF8, C_UTF16 and C_UTF32
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT C_STRING
	{
		typedef TYPE TType;
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< C_STRING<C_UTF8> >
	{
		typedef UTF8String TType;
		typedef C_UTF8 ElmType;
		typedef char RawType;
		static const int trVal = COREARRAY_TR_VARIABLE_LEN_STRING;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF8;

		static const char *StreamName() { return "dVStr8"; }
		static const char *TraitName() { return StreamName()+1; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< C_STRING<C_UTF16> >
	{
		typedef UTF16String TType;
		typedef C_UTF16 ElmType;
		typedef C_UTF16 RawType;
		static const int trVal = COREARRAY_TR_VARIABLE_LEN_STRING;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF16;

		static const char *StreamName() { return "dVStr16"; }
		static const char *TraitName() { return StreamName()+1; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< C_STRING<C_UTF32> >
	{
		typedef UTF32String TType;
		typedef C_UTF32 ElmType;
		typedef C_UTF32 RawType;
		static const int trVal = COREARRAY_TR_VARIABLE_LEN_STRING;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svCustomStr;

		static const char *StreamName() { return "dVStr32"; }
		static const char *TraitName() { return StreamName()+1; }
	};


	/// Variable-length string container
	/** \tparam T  should be C_STRING<C_UTF8>,
	 *             C_STRING<C_UTF16> or C_STRING<C_UTF32>
	 *  \sa  CdVStr8, CdVStr16, CdVStr32
	**/
	template<typename TYPE> class COREARRAY_DLL_DEFAULT CdCString:
		public CdArray< C_STRING<TYPE> >
	{
	public:
		template<typename ALLOC_TYPE, typename MEM_TYPE> friend struct ALLOC_FUNC;

		typedef typename TdTraits< C_STRING<TYPE> >::TType TType;
		typedef TYPE ElmType;
		typedef typename TdTraits< C_STRING<TYPE> >::RawType RawType;

		CdCString(): CdArray< C_STRING<TYPE> >()
		{
			this->_ActualPosition = 0;
			this->_CurrentIndex = 0;
			this->_TotalSize = 0;
			this->fIndexingID = 0;
		}

		/// get a list of CdBlockStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
		{
			CdArray< C_STRING<TYPE> >::GetOwnBlockStream(Out);
			if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
		}

		/// get a list of CdStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<CdStream*> &Out)
		{
			CdArray< C_STRING<TYPE> >::GetOwnBlockStream(Out);
			if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
		}

        virtual CdGDSObj *NewObject()
		{
			return (new CdCString<TYPE>)->AssignPipe(*this);
		}

		virtual void SetDLen(int I, C_Int32 Value)
		{
			this->_CheckSetDLen(I, Value);
			CdAllocArray::TDimItem &pDim = this->fDimension[I];
			if (pDim.DimLen != Value)
				throw ErrArray("The current version does not support this function.");
		}

		/// append Count elements from an iterator
		/** If the source is a container of exactly the same type, the on-disk
		 *  encoding of the two is identical, so the elements can be appended as
		 *  a raw byte block instead of being decoded and re-encoded one string
		 *  at a time. Otherwise fall back to the element-wise implementation.
		**/
		virtual void AppendIter(CdIterator &I, C_Int64 Count)
		{
			if ((Count > 0) && (typeid(*this) == typeid(*I.Handler)))
			{
				CdCString<TYPE> *Src = static_cast< CdCString<TYPE>* >(I.Handler);
				CdBufStream *DstBuf = this->fAllocator.BufStream();
				C_Int64 Idx = I.Ptr / sizeof(TYPE), IdxEnd = Idx + Count;
				if ((Src != this) && DstBuf && Src->fAllocator.BufStream() &&
					(Idx >= 0) && (IdxEnd <= Src->fTotalCount))
				{
					// the byte range [pS, pE) holding the source elements
					Src->_Find_Position(Idx);
					SIZE64 pS = Src->_ActualPosition, pE;
					if (IdxEnd < Src->fTotalCount)
					{
						Src->_Find_Position(IdxEnd);
						pE = Src->_ActualPosition;
					} else
						pE = Src->_TotalSize;

					// copy the block to the end of this container
					this->_SetLargeBuffer();
					DstBuf->SetPosition(this->_TotalSize);
					Src->fAllocator.CopyTo(*DstBuf, pS, pE - pS);

					// Carry the checkpoints over. Appended element
					// fTotalCount+k is source element Idx+k, and the two lie a
					// constant Delta apart in their streams, so each checkpoint
					// is just a source offset plus Delta. Asking the source for
					// those offsets reuses its own index: when the two
					// containers share the stride alignment every answer is an
					// O(1) lookup, and otherwise it degrades to one forward
					// walk of the copied range rather than a second parse.
					if (fPersistIndex.Stream())
					{
						const C_Int64 SD = CdStrIndexing::STRIDE;
						SIZE64 Delta = this->_TotalSize - pS;
						C_Int64 base = this->fTotalCount;
						for (C_Int64 t = (base/SD + 1) * SD; t <= base + Count;
							t += SD)
						{
							C_Int64 si = Idx + (t - base);
							if (si < Src->fTotalCount)
							{
								Src->_Find_Position(si);
								fPersistIndex.Store(t,
									Src->_ActualPosition + Delta);
							} else {
								// one past the copied range: its offset is the
								// end of the block, not a locatable element
								fPersistIndex.Store(t, pE + Delta);
							}
						}
						this->fNeedUpdate = true;
					}

					// CopyTo leaves the source stream at the end of the block;
					// _Find_Position only re-seeks when the index moves, so the
					// source cursor has to be left consistent with it here
					Src->_ActualPosition = pE;
					Src->_CurrentIndex = IdxEnd;
					Src->fAllocator.SetPosition(pE);

					this->_TotalSize += (pE - pS);
					this->_ActualPosition = this->_TotalSize;
					I.Ptr += Count * sizeof(TYPE);

					// update the total count and the first dimension
					CdAllocArray::TDimItem &R = this->fDimension.front();
					this->fTotalCount += Count;
					if (this->fTotalCount >= R.DimElmCnt*(R.DimLen+1))
					{
						R.DimLen = this->fTotalCount / R.DimElmCnt;
						this->_SetFlushEvent();
						this->fNeedUpdate = true;
					}
					this->_CurrentIndex = this->fTotalCount;
					fIndexing.Reset(this->fTotalCount);
					return;
				}
			}
			CdAllocArray::AppendIter(I, Count);
		}

	protected:
		/// indexing object
		CdStreamIndex fIndexing;

		/// initialize n array
		virtual void IterInit(CdIterator &I, SIZE64 n)
		{
			if ((I.Ptr == this->fTotalCount) && (n > 0))
			{
				this->fAllocator.ZeroFill(this->_TotalSize, n);
				this->_TotalSize += n;
			}
		}

		/// finalize n array
		virtual void IterDone(CdIterator &I, SIZE64 n)
		{
			if ((I.Ptr + n) == this->fTotalCount)
			{
				_Find_Position(I.Ptr);
				this->_TotalSize = this->_ActualPosition;
				// the data past I.Ptr is going away; the checkpoints beyond it
				// would otherwise still be counted by the stream size
				fPersistIndex.Truncate(I.Ptr);
				this->fNeedUpdate = true;
			}
		}

		/// offset the iterator
		virtual void IterOffset(CdIterator &I, SIZE64 val)
		{
			I.Ptr += val;
		}

		virtual SIZE64 AllocSize(C_Int64 Num)
		{
			if (Num >= this->fTotalCount)
			{
				return this->_TotalSize + (Num - this->fTotalCount);
			} else if (Num > 0)
			{
				_Find_Position(Num);
				return _ActualPosition;
			} else
				return 0;
		}

		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			CdAllocArray::Loading(Reader, Version);

			this->_ActualPosition = 0;
			this->_CurrentIndex = 0;
			this->_TotalSize = 0;
			fIndexing.Reset(this->fTotalCount);
			fIndexing.Initialize();

			fPersistIndex.Clear();

			if (this->fGDSStream)
			{
				if (this->fPipeInfo)
				{
					this->_TotalSize = this->fPipeInfo->StreamTotalIn();
				} else {
					if (this->fAllocator.BufStream())
						this->_TotalSize = this->fAllocator.BufStream()->GetSize();
				}
				// files written before the checkpoint table existed have no
				// such property, so it has to be optional here
				if (Reader.HaveProperty(CdStrIndexing::VarName()))
				{
					Reader[CdStrIndexing::VarName()] >> fIndexingID;
					fPersistIndex.Load(
						this->fGDSStream->Collection()[fIndexingID],
						this->fTotalCount, this->_TotalSize);
				}
			}
		}

		virtual void Saving(CdWriter &Writer)
		{
			CdAllocArray::Saving(Writer);
			if (this->fGDSStream)
			{
				if (!fPersistIndex.Stream())
				{
					fPersistIndex.Create(
						this->fGDSStream->Collection().NewBlockStream());
				}
				fIndexingID = fPersistIndex.Stream()->ID();
				Writer[CdStrIndexing::VarName()] << fIndexingID;
			}
		}

		/// stamp the index header once the count and stream size have settled
		virtual void UpdateInfoExt(CdBufStream *Sender)
		{
			CdAllocArray::UpdateInfoExt(Sender);
			if (this->fGDSStream && !this->fGDSStream->ReadOnly())
				fPersistIndex.Flush(this->fTotalCount, this->_TotalSize);
		}

		SIZE64 _ActualPosition;
		C_Int64 _CurrentIndex;
		SIZE64 _TotalSize;

		/// on-disk checkpoint table, complete as soon as the data is written
		CdStrIndexing fPersistIndex;
		/// block ID of the on-disk checkpoint table
		TdGDSBlockID fIndexingID;

		COREARRAY_INLINE TType _ReadString()
		{
			TType s;
			while (true)
			{
				TYPE ch;
				this->fAllocator.ReadVar(ch);
				if (ch != 0) s.push_back(ch); else break;
			}
			this->_ActualPosition += sizeof(TYPE) * (s.size() + 1);
			fIndexing.Forward(this->_ActualPosition);
			this->_CurrentIndex ++;
			COREARRAY_ENDIAN_LE_TO_NT_ARRAY((TYPE*)s.c_str(), s.size());
			return s;
		}

		COREARRAY_INLINE void _SkipString()
		{
			TYPE ch;
			do {
				this->fAllocator.ReadVar(ch);
				this->_ActualPosition += sizeof(ch);
			} while (ch != 0);
			this->_CurrentIndex ++;
			fIndexing.Forward(this->_ActualPosition);
		}

		COREARRAY_INLINE void _WriteString(const TType &val)
		{
			TYPE ch = 0;
			size_t pos = val.find(ch);
			if (pos == string::npos) pos = val.length();
			ssize_t str_size = pos * sizeof(TYPE);

			ssize_t old_len = 0;
			this->fAllocator.SetPosition(this->_ActualPosition);
			do {
				this->fAllocator.ReadData(&ch, sizeof(ch));
				if (ch != 0) old_len += sizeof(TYPE);
			} while (ch != 0);

			if (old_len != str_size)
			{
				this->fAllocator.Move(this->_ActualPosition + old_len,
					this->_ActualPosition + str_size,
					this->_TotalSize - this->_ActualPosition - old_len);
				this->_TotalSize += (str_size - old_len);
				// everything after this element just moved by a constant
				fPersistIndex.Shift(this->_CurrentIndex, str_size - old_len);
				this->fNeedUpdate = true;
			}

			BYTE_LE<CdAllocator> ss(this->fAllocator);
			ss.SetPosition(this->_ActualPosition);
			ss.W((TYPE*)val.c_str(), pos+1);

			this->_ActualPosition += str_size + sizeof(ch);
			this->_CurrentIndex ++;
			fIndexing.Reset(this->fTotalCount);
		}

		COREARRAY_INLINE void _AppendString(const TType &val)
		{
			const RawType ch = 0;
			size_t pos = val.find(ch);
			if (pos == string::npos) pos = val.length();

			BYTE_LE<CdAllocator> ss(this->fAllocator);
			ss.SetPosition(this->_TotalSize);
			ss.W((TYPE*)val.c_str(), pos+1);

			this->_ActualPosition = this->_TotalSize = ss.Position();
			this->_CurrentIndex ++;
			// the new element ends where the next one starts, so once the
			// count reaches a multiple of STRIDE this is that element's offset
			if (!(this->_CurrentIndex % CdStrIndexing::STRIDE))
			{
				fPersistIndex.Store(this->_CurrentIndex, this->_TotalSize);
				this->fNeedUpdate = true;
			}
			fIndexing.Reset(this->_CurrentIndex);
		}

		COREARRAY_INLINE void _Find_Position(SIZE64 Index)
		{
			if (Index != this->_CurrentIndex)
			{
				fIndexing.Set(Index, this->_CurrentIndex, this->_ActualPosition);
				// the on-disk table can start closer than either the cursor or
				// the in-memory index -- notably just after the file is opened,
				// when the latter holds nothing but element zero
				C_Int64 pi; SIZE64 pp;
				if (fPersistIndex.Lookup(Index, pi, pp))
				{
					if (pi > this->_CurrentIndex)
					{
						this->_CurrentIndex = pi;
						this->_ActualPosition = pp;
						fIndexing.Reposition(pi);
					}
				}
				BYTE_LE<CdAllocator> ss(this->fAllocator);
				ss.SetPosition(this->_ActualPosition);
				while (this->_CurrentIndex < Index) _SkipString();
			}
		}
	};


	/// Template functions for allocator
	
	template<typename TYPE, typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<C_STRING<TYPE>, MEM_TYPE>
	{
		/// string type
		typedef typename TdTraits< C_STRING<TYPE> >::TType StrType;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			CdCString<TYPE> *IT = static_cast< CdCString<TYPE>* >(I.Handler);
			IT->_Find_Position(I.Ptr / sizeof(TYPE));
			I.Ptr += n * sizeof(TYPE);
			for (; n > 0; n--)
				*p++ = VAL_CONVERT(MEM_TYPE, StrType, IT->_ReadString());
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += sizeof(TYPE);
			CdCString<TYPE> *IT = static_cast< CdCString<TYPE>* >(I.Handler);
			IT->_Find_Position(I.Ptr / sizeof(TYPE));
			I.Ptr += n * sizeof(TYPE);
			for (; n > 0; n--)
			{
				if (*sel++)
					*p++ = VAL_CONVERT(MEM_TYPE, StrType, IT->_ReadString());
				else
					IT->_SkipString();
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			CdCString<TYPE> *IT = static_cast< CdCString<TYPE>* >(I.Handler);
			SIZE64 Idx = I.Ptr / sizeof(TYPE);
			if (Idx < IT->fTotalCount)
				IT->_Find_Position(Idx);
			else {
				// appending: _AppendString assigns _ActualPosition absolutely but
				// only increments _CurrentIndex, so the cursor has to start at the
				// end of the data or both it and fIndexing's count end up short
				IT->_ActualPosition = IT->_TotalSize;
				IT->_CurrentIndex   = IT->fTotalCount;
			}
			for (; n > 0; n--)
			{
				if (Idx < IT->fTotalCount)
					IT->_WriteString(VAL_CONVERT(StrType, MEM_TYPE, *p++));
				else
					IT->_AppendString(VAL_CONVERT(StrType, MEM_TYPE, *p++));
			}
			return p;
		}
	};


	/// Variable-length of UTF-8 string
	typedef CdCString<C_UTF8>     CdVStr8;
	/// Variable-length of UTF-16 string
	typedef CdCString<C_UTF16>    CdVStr16;
	/// Variable-length of UTF-32 string
	typedef CdCString<C_UTF32>    CdVStr32;



	// =======================================================================
	// Variable-length string allowing null character
	// =======================================================================

	/// Variable-length array
	/** \tparam TYPE  data type, e.g C_UTF8, C_UTF16 and C_UTF32
	**/
	template<typename TYPE> struct COREARRAY_DLL_DEFAULT VARIABLE_LEN
	{
		typedef TYPE TType;
		C_UInt8 LengthCoding;
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< VARIABLE_LEN<C_UTF8> >
	{
		typedef UTF8String TType;
		typedef C_UTF8 ElmType;
		typedef char RawType;
		static const int trVal = COREARRAY_TR_VARIABLE_LEN_STRING;
		static const unsigned BitOf = 8u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF8;

		static const char *StreamName() { return "dStr8"; }
		static const char *TraitName() { return StreamName()+1; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< VARIABLE_LEN<C_UTF16> >
	{
		typedef UTF16String TType;
		typedef C_UTF16 ElmType;
		typedef C_UTF16 RawType;
		static const int trVal = COREARRAY_TR_VARIABLE_LEN_STRING;
		static const unsigned BitOf = 16u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svStrUTF16;

		static const char *StreamName() { return "dStr16"; }
		static const char *TraitName() { return StreamName()+1; }
	};

	template<> struct COREARRAY_DLL_DEFAULT TdTraits< VARIABLE_LEN<C_UTF32> >
	{
		typedef UTF32String TType;
		typedef C_UTF32 ElmType;
		typedef C_UTF32 RawType;
		static const int trVal = COREARRAY_TR_VARIABLE_LEN_STRING;
		static const unsigned BitOf = 32u;
		static const bool IsPrimitive = false;
		static const C_SVType SVType = svCustomStr;

		static const char *StreamName() { return "dStr32"; }
		static const char *TraitName() { return StreamName()+1; }
	};


	/// Variable-length string container
	/** \tparam T  should be VARIABLE_LEN<C_UTF8>,
	 *             VARIABLE_LEN<C_UTF16> or VARIABLE_LEN<C_UTF32>
	 *  \sa  CdStr8, CdStr16, CdStr32
	**/
	template<typename TYPE> class COREARRAY_DLL_DEFAULT CdString:
		public CdArray< VARIABLE_LEN<TYPE> >
	{
	public:
		template<typename ALLOC_TYPE, typename MEM_TYPE> friend struct ALLOC_FUNC;

		typedef typename TdTraits< VARIABLE_LEN<TYPE> >::TType TType;
		typedef TYPE ElmType;
		typedef typename TdTraits< VARIABLE_LEN<TYPE> >::RawType RawType;

		CdString(): CdArray< VARIABLE_LEN<TYPE> >()
		{
			this->_ActualPosition = 0;
			this->_CurrentIndex = 0;
			this->_TotalSize = 0;
			this->fIndexingID = 0;
		}

		/// get a list of CdBlockStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<const CdBlockStream*> &Out) const
		{
			CdArray< VARIABLE_LEN<TYPE> >::GetOwnBlockStream(Out);
			if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
		}

		/// get a list of CdStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<CdStream*> &Out)
		{
			CdArray< VARIABLE_LEN<TYPE> >::GetOwnBlockStream(Out);
			if (fPersistIndex.Stream()) Out.push_back(fPersistIndex.Stream());
		}

        virtual CdGDSObj *NewObject()
		{
			return (new CdString<TYPE>)->AssignPipe(*this);
		}

		virtual void SetDLen(int I, C_Int32 Value)
		{
			this->_CheckSetDLen(I, Value);
			CdAllocArray::TDimItem &pDim = this->fDimension[I];
			if (pDim.DimLen != Value)
				throw ErrArray("The current version does not support this function.");
		}

		/// append Count elements from an iterator
		/** If the source is a container of exactly the same type, the on-disk
		 *  encoding of the two is identical, so the elements can be appended as
		 *  a raw byte block instead of being decoded and re-encoded one string
		 *  at a time. Otherwise fall back to the element-wise implementation.
		**/
		virtual void AppendIter(CdIterator &I, C_Int64 Count)
		{
			if ((Count > 0) && (typeid(*this) == typeid(*I.Handler)))
			{
				CdString<TYPE> *Src = static_cast< CdString<TYPE>* >(I.Handler);
				CdBufStream *DstBuf = this->fAllocator.BufStream();
				C_Int64 Idx = I.Ptr / sizeof(TYPE), IdxEnd = Idx + Count;
				if ((Src != this) && DstBuf && Src->fAllocator.BufStream() &&
					(Idx >= 0) && (IdxEnd <= Src->fTotalCount))
				{
					// the byte range [pS, pE) holding the source elements
					Src->_Find_Position(Idx);
					SIZE64 pS = Src->_ActualPosition, pE;
					if (IdxEnd < Src->fTotalCount)
					{
						Src->_Find_Position(IdxEnd);
						pE = Src->_ActualPosition;
					} else
						pE = Src->_TotalSize;

					// copy the block to the end of this container
					this->_SetLargeBuffer();
					DstBuf->SetPosition(this->_TotalSize);
					Src->fAllocator.CopyTo(*DstBuf, pS, pE - pS);

					// Carry the checkpoints over. Appended element
					// fTotalCount+k is source element Idx+k, and the two lie a
					// constant Delta apart in their streams, so each checkpoint
					// is just a source offset plus Delta. Asking the source for
					// those offsets reuses its own index: when the two
					// containers share the stride alignment every answer is an
					// O(1) lookup, and otherwise it degrades to one forward
					// walk of the copied range rather than a second parse.
					if (fPersistIndex.Stream())
					{
						const C_Int64 SD = CdStrIndexing::STRIDE;
						SIZE64 Delta = this->_TotalSize - pS;
						C_Int64 base = this->fTotalCount;
						for (C_Int64 t = (base/SD + 1) * SD; t <= base + Count;
							t += SD)
						{
							C_Int64 si = Idx + (t - base);
							if (si < Src->fTotalCount)
							{
								Src->_Find_Position(si);
								fPersistIndex.Store(t,
									Src->_ActualPosition + Delta);
							} else {
								// one past the copied range: its offset is the
								// end of the block, not a locatable element
								fPersistIndex.Store(t, pE + Delta);
							}
						}
						this->fNeedUpdate = true;
					}

					// CopyTo leaves the source stream at the end of the block;
					// _Find_Position only re-seeks when the index moves, so the
					// source cursor has to be left consistent with it here
					Src->_ActualPosition = pE;
					Src->_CurrentIndex = IdxEnd;
					Src->fAllocator.SetPosition(pE);

					this->_TotalSize += (pE - pS);
					this->_ActualPosition = this->_TotalSize;
					I.Ptr += Count * sizeof(TYPE);

					// update the total count and the first dimension
					CdAllocArray::TDimItem &R = this->fDimension.front();
					this->fTotalCount += Count;
					if (this->fTotalCount >= R.DimElmCnt*(R.DimLen+1))
					{
						R.DimLen = this->fTotalCount / R.DimElmCnt;
						this->_SetFlushEvent();
						this->fNeedUpdate = true;
					}
					this->_CurrentIndex = this->fTotalCount;
					fIndexing.Reset(this->fTotalCount);
					return;
				}
			}
			CdAllocArray::AppendIter(I, Count);
		}

	protected:
		/// indexing object
		CdStreamIndex fIndexing;

		/// initialize n array
		virtual void IterInit(CdIterator &I, SIZE64 n)
		{
			if ((I.Ptr == this->fTotalCount) && (n > 0))
			{
				this->fAllocator.ZeroFill(this->_TotalSize, n);
				this->_TotalSize += n;
			}
		}

		/// finalize n array
		virtual void IterDone(CdIterator &I, SIZE64 n)
		{
			if ((I.Ptr + n) == this->fTotalCount)
			{
				_Find_Position(I.Ptr);
				this->_TotalSize = this->_ActualPosition;
				// the data past I.Ptr is going away; the checkpoints beyond it
				// would otherwise still be counted by the stream size
				fPersistIndex.Truncate(I.Ptr);
				this->fNeedUpdate = true;
			}
		}

		/// offset the iterator
		virtual void IterOffset(CdIterator &I, SIZE64 val)
		{
			I.Ptr += val;
		}

		virtual SIZE64 AllocSize(C_Int64 Num)
		{
			if (Num >= this->fTotalCount)
			{
				return this->_TotalSize + (Num - this->fTotalCount);
			} else if (Num > 0)
			{
				_Find_Position(Num);
				return _ActualPosition;
			} else
				return 0;
		}

		virtual void Loading(CdReader &Reader, TdVersion Version)
		{
			CdAllocArray::Loading(Reader, Version);

			this->_ActualPosition = 0;
			this->_CurrentIndex = 0;
			this->_TotalSize = 0;
			fIndexing.Reset(this->fTotalCount);
			fIndexing.Initialize();

			fPersistIndex.Clear();

			if (this->fGDSStream)
			{
				if (this->fPipeInfo)
				{
					this->_TotalSize = this->fPipeInfo->StreamTotalIn();
				} else {
					if (this->fAllocator.BufStream())
						this->_TotalSize = this->fAllocator.BufStream()->GetSize();
				}
				// files written before the checkpoint table existed have no
				// such property, so it has to be optional here
				if (Reader.HaveProperty(CdStrIndexing::VarName()))
				{
					Reader[CdStrIndexing::VarName()] >> fIndexingID;
					fPersistIndex.Load(
						this->fGDSStream->Collection()[fIndexingID],
						this->fTotalCount, this->_TotalSize);
				}
			}
		}

		virtual void Saving(CdWriter &Writer)
		{
			CdAllocArray::Saving(Writer);
			if (this->fGDSStream)
			{
				if (!fPersistIndex.Stream())
				{
					fPersistIndex.Create(
						this->fGDSStream->Collection().NewBlockStream());
				}
				fIndexingID = fPersistIndex.Stream()->ID();
				Writer[CdStrIndexing::VarName()] << fIndexingID;
			}
		}

		/// stamp the index header once the count and stream size have settled
		virtual void UpdateInfoExt(CdBufStream *Sender)
		{
			CdAllocArray::UpdateInfoExt(Sender);
			if (this->fGDSStream && !this->fGDSStream->ReadOnly())
				fPersistIndex.Flush(this->fTotalCount, this->_TotalSize);
		}

		SIZE64 _ActualPosition;
		C_Int64 _CurrentIndex;
		SIZE64 _TotalSize;

		/// on-disk checkpoint table, complete as soon as the data is written
		CdStrIndexing fPersistIndex;
		/// block ID of the on-disk checkpoint table
		TdGDSBlockID fIndexingID;

		COREARRAY_INLINE TType _ReadString()
		{
			// get the length of string
			ssize_t n=0, len_byte=0;
			C_UInt8 ch, shl=0;
			do {
				ch = this->fAllocator.R8b();
				n |= ssize_t(ch & 0x7F) << shl;
				shl += 7;
				len_byte ++;
			} while (ch & 0x80);

			TType s;
			if (n > 0)
			{
				s.resize(n);
				TYPE *p = (TYPE*)&s[0];
				this->fAllocator.ReadData(p, n * sizeof(TYPE));
				COREARRAY_ENDIAN_LE_TO_NT_ARRAY(p, n);
				len_byte += n * sizeof(TYPE);
			}

			this->_ActualPosition += len_byte;
			fIndexing.Forward(this->_ActualPosition);
			this->_CurrentIndex ++;
			return s;
		}

		COREARRAY_INLINE void _SkipString()
		{
			ssize_t n=0, len_byte=0;
			C_UInt8 ch, shl=0;
			do {
				ch = this->fAllocator.R8b();
				n |= ssize_t(ch & 0x7F) << shl;
				shl += 7;
				len_byte ++;
			} while (ch & 0x80);
			len_byte += n * sizeof(TYPE);

			this->_ActualPosition += len_byte;
			if (n > 0)
				this->fAllocator.SetPosition(this->_ActualPosition);
			fIndexing.Forward(this->_ActualPosition);
			this->_CurrentIndex ++;
		}

		COREARRAY_INLINE void _WriteString(const TType &val)
		{
			// calculate the total length in bytes for the new string
			ssize_t n = val.size(), len_byte = 0;
			size_t m = n;
			do {
				m >>= 7; len_byte ++;
			} while (m > 0);
			len_byte += n * sizeof(TYPE);

			// get the total length in bytes for the old string
			this->fAllocator.SetPosition(this->_ActualPosition);
			ssize_t old_len=0, old_n=0;
			C_UInt8 ch, shl=0;
			do {
				ch = this->fAllocator.R8b();
				old_n |= ssize_t(ch & 0x7F) << shl;
				shl += 7;
				old_len ++;
			} while (ch & 0x80);
			old_len += old_n * sizeof(TYPE);

			// move data if needed
			if (old_len != len_byte)
			{
				this->fAllocator.Move(this->_ActualPosition + old_len,
					this->_ActualPosition + len_byte,
					this->_TotalSize - this->_ActualPosition - old_len);
				this->_TotalSize += (len_byte - old_len);
				// everything after this element just moved by a constant
				fPersistIndex.Shift(this->_CurrentIndex, len_byte - old_len);
				this->fNeedUpdate = true;
			}

			// write the length
			this->fAllocator.SetPosition(this->_ActualPosition);
			m = n;
			do {
				C_UInt8 ch = (m & 0x7F); m >>= 7;
				if (m > 0) ch |= 0x80;
				this->fAllocator.W8b(ch);
			} while (m > 0);
			// write the string
			if (n > 0)
			{
				BYTE_LE<CdAllocator> ss(this->fAllocator);
				ss.W((const TYPE*)&val[0], n);
			}

			this->_ActualPosition += len_byte;
			this->_CurrentIndex ++;
			fIndexing.Reset(this->fTotalCount);
		}

		COREARRAY_INLINE void _AppendString(const TType &val)
		{
			// write the length
			size_t n = val.size(), len_byte = 0;
			this->fAllocator.SetPosition(this->_TotalSize);
			size_t m = n;
			do {
				C_UInt8 ch = (m & 0x7F); m >>= 7;
				if (m > 0) ch |= 0x80;
				this->fAllocator.W8b(ch);
				len_byte ++;
			} while (m > 0);
			// write the string
			if (n > 0)
			{
				BYTE_LE<CdAllocator> ss(this->fAllocator);
				ss.W((const TYPE*)&val[0], n);
				len_byte += n * sizeof(TYPE);
			}

			this->_TotalSize += len_byte;
			this->_ActualPosition = this->_TotalSize;
			this->_CurrentIndex ++;
			// the new element ends where the next one starts, so once the
			// count reaches a multiple of STRIDE this is that element's offset
			if (!(this->_CurrentIndex % CdStrIndexing::STRIDE))
			{
				fPersistIndex.Store(this->_CurrentIndex, this->_TotalSize);
				this->fNeedUpdate = true;
			}
			fIndexing.Reset(this->_CurrentIndex);
		}

		COREARRAY_INLINE void _Find_Position(SIZE64 Index)
		{
			if (Index != this->_CurrentIndex)
			{
				fIndexing.Set(Index, this->_CurrentIndex, this->_ActualPosition);
				// the on-disk table can start closer than either the cursor or
				// the in-memory index -- notably just after the file is opened,
				// when the latter holds nothing but element zero
				C_Int64 pi; SIZE64 pp;
				if (fPersistIndex.Lookup(Index, pi, pp))
				{
					if (pi > this->_CurrentIndex)
					{
						this->_CurrentIndex = pi;
						this->_ActualPosition = pp;
						fIndexing.Reposition(pi);
					}
				}
				this->fAllocator.SetPosition(this->_ActualPosition);
				while (this->_CurrentIndex < Index) _SkipString();
			}
		}
	};


	/// Template functions for allocator
	
	template<typename TYPE, typename MEM_TYPE>
		struct COREARRAY_DLL_DEFAULT ALLOC_FUNC<VARIABLE_LEN<TYPE>, MEM_TYPE>
	{
		/// string type
		typedef typename TdTraits< VARIABLE_LEN<TYPE> >::TType StrType;

		/// read an array from CdAllocator
		static MEM_TYPE *Read(CdIterator &I, MEM_TYPE *p, ssize_t n)
		{
			if (n <= 0) return p;
			CdString<TYPE> *IT = static_cast< CdString<TYPE>* >(I.Handler);
			IT->_Find_Position(I.Ptr / sizeof(TYPE));
			I.Ptr += n * sizeof(TYPE);
			for (; n > 0; n--)
				*p++ = VAL_CONVERT(MEM_TYPE, StrType, IT->_ReadString());
			return p;
		}

		/// read an array from CdAllocator with selection
		static MEM_TYPE *ReadEx(CdIterator &I, MEM_TYPE *p, ssize_t n,
			const C_BOOL sel[])
		{
			if (n <= 0) return p;
			for (; n>0 && !*sel; n--, sel++) I.Ptr += sizeof(TYPE);
			CdString<TYPE> *IT = static_cast< CdString<TYPE>* >(I.Handler);
			IT->_Find_Position(I.Ptr / sizeof(TYPE));
			I.Ptr += n * sizeof(TYPE);
			for (; n > 0; n--)
			{
				if (*sel++)
					*p++ = VAL_CONVERT(MEM_TYPE, StrType, IT->_ReadString());
				else
					IT->_SkipString();
			}
			return p;
		}

		/// write an array to CdAllocator
		static const MEM_TYPE *Write(CdIterator &I, const MEM_TYPE *p,
			ssize_t n)
		{
			if (n <= 0) return p;
			CdString<TYPE> *IT = static_cast< CdString<TYPE>* >(I.Handler);
			SIZE64 Idx = I.Ptr / sizeof(TYPE);
			if (Idx < IT->fTotalCount)
				IT->_Find_Position(Idx);
			else {
				// appending: _AppendString assigns _ActualPosition absolutely but
				// only increments _CurrentIndex, so the cursor has to start at the
				// end of the data or both it and fIndexing's count end up short
				IT->_ActualPosition = IT->_TotalSize;
				IT->_CurrentIndex   = IT->fTotalCount;
			}

			for (; n > 0; n--)
			{
				if (Idx < IT->fTotalCount)
					IT->_WriteString(VAL_CONVERT(StrType, MEM_TYPE, *p++));
				else
					IT->_AppendString(VAL_CONVERT(StrType, MEM_TYPE, *p++));
			}
			return p;
		}
	};


	/// Variable-length of UTF-8 string
	typedef CdString<C_UTF8>     CdStr8;
	/// Variable-length of UTF-16 string
	typedef CdString<C_UTF16>    CdStr16;
	/// Variable-length of UTF-32 string
	typedef CdString<C_UTF32>    CdStr32;
}

#endif /* _HEADER_COREARRAY_STRING_GDS_ */
