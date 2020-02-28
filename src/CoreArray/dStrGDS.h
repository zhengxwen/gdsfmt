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


namespace CoreArray
{
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
		static const int trVal = COREARRAY_TR_FIXED_LENGTH_STRING;
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
		static const int trVal = COREARRAY_TR_FIXED_LENGTH_STRING;
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
		static const int trVal = COREARRAY_TR_FIXED_LENGTH_STRING;
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
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
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
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
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
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
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

			if (this->fGDSStream)
			{
				if (this->fPipeInfo)
				{
					this->_TotalSize = this->fPipeInfo->StreamTotalIn();
				} else {
					if (this->fAllocator.BufStream())
						this->_TotalSize = this->fAllocator.BufStream()->GetSize();
				}
			}
		}

		SIZE64 _ActualPosition;
		C_Int64 _CurrentIndex;
		SIZE64 _TotalSize;

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
			fIndexing.Reset(this->_CurrentIndex);
		}

		COREARRAY_INLINE void _Find_Position(SIZE64 Index)
		{
			if (Index != this->_CurrentIndex)
			{
				fIndexing.Set(Index, this->_CurrentIndex, this->_ActualPosition);
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
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
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
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
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
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
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

			if (this->fGDSStream)
			{
				if (this->fPipeInfo)
				{
					this->_TotalSize = this->fPipeInfo->StreamTotalIn();
				} else {
					if (this->fAllocator.BufStream())
						this->_TotalSize = this->fAllocator.BufStream()->GetSize();
				}
			}
		}

		SIZE64 _ActualPosition;
		C_Int64 _CurrentIndex;
		SIZE64 _TotalSize;

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
			fIndexing.Reset(this->_CurrentIndex);
		}

		COREARRAY_INLINE void _Find_Position(SIZE64 Index)
		{
			if (Index != this->_CurrentIndex)
			{
				fIndexing.Set(Index, this->_CurrentIndex, this->_ActualPosition);
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
