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
 *	\file     dStrGDS.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    GDS format with character types and functions
 *	\details
**/

#ifndef _HEADER_COREARRAY_STRING_GDS_
#define _HEADER_COREARRAY_STRING_GDS_

#include <dString.h>
#include <dStruct.h>


namespace CoreArray
{
	// ***********************************************************
	//
	// String classes and functions
	//
	// ***********************************************************

	/// fixed-length array
	/** \tparam TYPE  any data type, e.g integer or float number
	 *  \tparam SIZE  the array length
	**/
	template<typename TYPE, int SIZE=0> struct FIXED_LENGTH
	{
		typedef typename TdTraits<TYPE>::TType TType;
		typedef typename TdTraits<TYPE>::ElmType ElmType;
		TYPE Value[SIZE];
	};

	/// variable-length array
	/** \tparam TYPE  any data type, e.g integer or float number
	**/
	template<typename TYPE> struct VARIABLE_LENGTH
	{
		typedef typename TdTraits<TYPE>::TType TType;
		typedef typename TdTraits<TYPE>::ElmType ElmType;
	};


	template<> struct TdTraits< FIXED_LENGTH<UTF8*> >
	{
		typedef UTF8String TType;
		typedef UTF8 ElmType;
		static const int trVal = COREARRAY_TR_FIXED_LENGTH_STRING;
		static const unsigned BitOf = 8u;
		static const bool isClass = false;
		static const C_SVType SVType = svStrUTF8;

		static const char * TraitName() { return StreamName()+1; }
		static const char * StreamName() { return "dFStr8"; }
	};
	template<> struct TdTraits< FIXED_LENGTH<UTF16*> >
	{
		typedef UTF16String TType;
		typedef UTF16 ElmType;
		static const int trVal = COREARRAY_TR_FIXED_LENGTH_STRING;
		static const unsigned BitOf = 16u;
		static const bool isClass = false;
		static const C_SVType SVType = svStrUTF16;

		static const char * TraitName() { return StreamName()+1; }
		static const char * StreamName() { return "dFStr16"; }
	};
	template<> struct TdTraits< FIXED_LENGTH<UTF32*> >
	{
		typedef UTF32String TType;
		typedef UTF32 ElmType;
		static const int trVal = COREARRAY_TR_FIXED_LENGTH_STRING;
		static const unsigned BitOf = 32u;
		static const bool isClass = false;
		static const C_SVType SVType = svCustomStr;

		static const char * TraitName() { return StreamName()+1; }
		static const char * StreamName() { return "dFStr32"; }
	};


	/// Fixed-length string container
	/** \tparam T  should FIXED_LENGTH<UTF8*>, FIXED_LENGTH<UTF16*>, or FIXED_LENGTH<UTF32*>
	 *  \sa  CdFStr8, CdFStr16, CdFStr32
	**/
	template<typename T> class COREARRAY_DLL_DEFAULT CdFixLenStr:
		public CdVector<T>
	{
	public:
		typedef T ElmType;
		typedef typename TdTraits<T>::TType ElmTypeEx;

		CdFixLenStr(): CdVector<T>()
		{
			vElmSize_Ptr = 0;
		}
		virtual ~CdFixLenStr()
		{
			if (this->fGDSStream) this->Synchronize();
		}

        virtual CdGDSObj *NewOne(void *Param = NULL)
		{
			CdFixLenStr<T> *rv = new CdFixLenStr<T>;
			rv->SetMaxLength(this->MaxLength());
			this->xAssignToDim(*rv);
			if (this->fPipeInfo)
				rv->fPipeInfo = this->fPipeInfo->NewOne();
			return rv;
		}

		void AppendString(const T data)
		{
			typename TdTraits<T>::TType val(data);
			this->Append(&val, 1, TdTraits<T>::SVType);
		}

		COREARRAY_INLINE ssize_t MaxLength() const
		{
			return this->fElmSize / (TdTraits<T>::BitOf/8);
		}
		void SetMaxLength(ssize_t NewLen)
		{
			if (NewLen > 0)
				this->SetElmSize(NewLen * (TdTraits<T>::BitOf/8));
		}

	protected:
		virtual void UpdateInfoProc(CBufdStream *Sender)
		{
			if (this->vElmSize_Ptr != 0)
			{
				this->fGDSStream->SetPosition(this->vElmSize_Ptr);
				this->fGDSStream->wUInt32(this->fElmSize);
			}
		}
		virtual void LoadBefore(CdSerial &Reader, TdVersion Version)
		{
			this->Clear();
			C_UInt32 esize = 0;
			if (Reader["ESIZE"] >> esize)
				this->vElmSize_Ptr = Reader.Position();
			else
				this->vElmSize_Ptr = 0;
			this->SetMaxLength(esize);
			CdVectorX::LoadBefore(Reader, Version);
		}
		virtual void SaveBefore(CdSerial &Writer)
		{
			Writer["ESIZE"] << C_UInt32(this->fElmSize);
			this->vElmSize_Ptr = Writer.Position() - sizeof(C_Int32);
			CdVectorX::SaveBefore(Writer);
		}
		virtual void _Assign(TdIterator &it, TdIterator &source)
			{ this->_StrTo(it, source.toStr()); };
		virtual int _Compare(TdIterator &it1, TdIterator &it2)
			{ return it1.toStr().compare(it2.toStr()); };
	private:
		SIZE64 vElmSize_Ptr;
	};


	template<typename TOutside, typename TInside, int O>
		struct VEC_DATA<TOutside, TInside, false, O, COREARRAY_TR_FIXED_LENGTH_STRING>
	{
		// Read
		COREARRAY_INLINE static void rItem(void *OutBuffer, const SIZE64 p64, CdVectorX &Seq)
		{
			typename TdTraits<TInside>::TType buf(Seq.fElmSize, '\x0');
			Seq.fAllocator.Read(p64, (void*)buf.c_str(), Seq.fElmSize);
			const typename TdTraits<TInside>::ElmType Ch = 0;
			size_t pos = buf.find(Ch);
			if (pos != string::npos) buf.resize(pos);
			*static_cast<TOutside*>(OutBuffer) =
				ValCvt<TOutside, typename TdTraits<TInside>::TType>(buf);
		}

		static void rArray(TIterVDataExt &Rec)
		{
			TOutside *pt = (TOutside*)Rec.pBuf;
			ssize_t Lx = Rec.Seq->ElmSize();
			typename TdTraits<TInside>::TType buf(Lx, '\x0');
			const typename TdTraits<TInside>::ElmType Ch = 0;

			for (ssize_t L = Rec.LastDim; L > 0; L--)
			{
				buf.resize(Lx);
				Rec.Seq->Allocator().Read(Rec.p64, (void*)buf.c_str(), Lx);
				size_t pos = buf.find(Ch);
				if (pos != string::npos) buf.resize(pos);
				*pt++ = ValCvt<TOutside, typename TdTraits<TInside>::TType>(
					typename TdTraits<TInside>::TType(buf.begin(), buf.end()));
				Rec.p64 += Lx;
			}
			Rec.pBuf = (char*)pt;
		}

		static void rArrayEx(TIterVDataExt &Rec, const C_BOOL *Sel)
		{
			TOutside *pt = (TOutside*)Rec.pBuf;
			ssize_t Lx = Rec.Seq->ElmSize();
			typename TdTraits<TInside>::TType buf(Lx, '\x0');
			const typename TdTraits<TInside>::ElmType Ch = 0;
			for (ssize_t L = Rec.LastDim; L > 0; L--)
			{
				if (*Sel++)
				{
					buf.resize(Lx);
					Rec.Seq->Allocator().Read(Rec.p64, (void*)buf.c_str(), Lx);
					size_t pos = buf.find(Ch);
					if (pos != string::npos) buf.resize(pos);
					*pt++ = ValCvt<TOutside, typename TdTraits<TInside>::TType>(
						typename TdTraits<TInside>::TType(buf.begin(), buf.end()));
				}
				Rec.p64 += Lx;
			}
			Rec.pBuf = (char*)pt;
		}

		// Write
		COREARRAY_INLINE static void wItem(void *InBuffer, SIZE64 p64, CdVectorX &Seq)
		{
			typename TdTraits<TInside>::TType buf(
				ValCvt<typename TdTraits<TInside>::TType, TOutside>(
				*static_cast<TOutside*>(InBuffer)) );
			ssize_t len = buf.length() * TdTraits<TInside>::BitOf / 8;
			if (len > Seq.fElmSize)
			{
				p64 /= Seq.fElmSize;
				Seq.SetElmSize(len);
				p64 *= len;
			}
			Seq.fAllocator.Write(p64, buf.c_str(), len);
			if (len < Seq.fElmSize)
				Seq.fAllocator.Fill(p64+len, Seq.fElmSize-len, 0);
		}
		static void wArray(TIterVDataExt &Rec)
		{
			TOutside *pt;
			TdAllocator &alloc = Rec.Seq->fAllocator;
			typename TdTraits<TInside>::TType buf;

			if (Rec.AppendMode || !Rec.Seq->fAllocator.MemLevel())
			{
				ssize_t MaxLen = 0;
				pt = (TOutside*)Rec.pBuf;
				for (ssize_t L = Rec.LastDim; L > 0; L--)
				{
					buf = ValCvt<typename TdTraits<TInside>::TType, TOutside>(*pt++);
					if (MaxLen < (ssize_t)buf.length())
					{
						MaxLen = (ssize_t)buf.length();
					}
				}
				MaxLen *= TdTraits<TInside>::BitOf / 8;
				if (MaxLen > Rec.Seq->fElmSize)
					Rec.Seq->SetElmSize(MaxLen);
			}

			ssize_t Lx = Rec.Seq->fElmSize;
			pt = (TOutside*)Rec.pBuf;
			for (ssize_t L = Rec.LastDim; L > 0; L--)
			{
				buf = ValCvt<typename TdTraits<TInside>::TType, TOutside>(*pt++);
				ssize_t len = buf.length() * TdTraits<TInside>::BitOf / 8;
				if (len > Lx)
				{
					Rec.p64 /= Lx;
					Rec.Seq->SetElmSize(len);
					Rec.p64 *= (Lx=len);
				}
				alloc.Write(Rec.p64, buf.c_str(), len);
				Rec.p64 += len; len = Lx - len;
				if (len > 0)
				{
					alloc.Fill(Rec.p64, len, 0);
					Rec.p64 += len;
				}
			}
			Rec.pBuf = (char*)pt;
		}
	};


	/// Fixed-length of UTF-8 string
	typedef CdFixLenStr< FIXED_LENGTH<UTF8*> >		CdFStr8;
	/// Fixed-length of UTF-16 string
	typedef CdFixLenStr< FIXED_LENGTH<UTF16*> >		CdFStr16;
	/// Fixed-length of UTF-32 string
	typedef CdFixLenStr< FIXED_LENGTH<UTF32*> >		CdFStr32;



	// ***********************************************************
	// ***********************************************************

	template<> struct TdTraits< VARIABLE_LENGTH<UTF8*> >
	{
		typedef UTF8String TType;
		typedef UTF8 ElmType;
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
		static const unsigned BitOf = 8u;
		static const bool isClass = true;
		static const C_SVType SVType = svCustomStr;

		static const char * StreamName() { return "dVStr8"; }
		static const char * TraitName() { return StreamName()+1; }
	};
	template<> struct TdTraits< VARIABLE_LENGTH<UTF16*> >
	{
		typedef UTF16String TType;
		typedef UTF16 ElmType;
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
		static const unsigned BitOf = 16u;
		static const bool isClass = true;
		static const C_SVType SVType = svCustomStr;

		static const char * StreamName() { return "dVStr16"; }
		static const char * TraitName() { return StreamName()+1; }
	};
	template<> struct TdTraits< VARIABLE_LENGTH<UTF32*> >
	{
		typedef UTF32String TType;
		typedef UTF32 ElmType;
		static const int trVal = COREARRAY_TR_VARIABLE_LENGTH_STRING;
		static const unsigned BitOf = 32u;
		static const bool isClass = true;
		static const C_SVType SVType = svCustomStr;

		static const char * StreamName() { return "dVStr32"; }
		static const char * TraitName() { return StreamName()+1; }
	};


	/// Variable-length string container
	/** \tparam T  should be VARIABLE_LENGTH<UTF8*>, VARIABLE_LENGTH<UTF16*>,
	 *             or VARIABLE_LENGTH<UTF32*>
	 *  \sa  CdVStr8, CdVStr16, CdVStr32
	**/
	template<typename T> class COREARRAY_DLL_DEFAULT CdVarLenStr:
		public CdVector<T>
	{
	public:
		typedef T ElmType;
		typedef typename TdTraits<T>::TType ElmTypeEx;

		CdVarLenStr(): CdVector<T>()
		{
			this->_ActualPosition = 0;
			this->_CurrentIndex = 0;
			this->_TotalSize = 0;
		}

        virtual CdGDSObj *NewOne(void *Param = NULL)
		{
			CdVarLenStr<T> *rv = new CdVarLenStr<T>;
			this->xAssignToDim(*rv);
			if (this->fPipeInfo)
				rv->fPipeInfo = this->fPipeInfo->NewOne();
			return rv;
		}

		virtual void Clear()
		{
			CdVector<T>::Clear();
			this->_RewindIndex();
		}

		void AppendString(const ElmTypeEx val)
		{
			this->Append(&val, 1, TdTraits<T>::SVType);
		}


		SIZE64 _ActualPosition;
		C_Int64 _CurrentIndex;
		SIZE64 _TotalSize;

		void _RewindIndex()
		{
			this->_ActualPosition = 0;
			this->_CurrentIndex = 0;
		}

		COREARRAY_INLINE ElmTypeEx _ReadString()
		{
			typename TdTraits<T>::ElmType ch;
			ElmTypeEx val;
			do {
				this->fAllocator.rChar(_ActualPosition, ch);
				this->_ActualPosition += sizeof(ch);
				if (ch != 0) val.push_back(ch);
			} while (ch != 0);
			this->_CurrentIndex ++;
			return val;
		}

		COREARRAY_INLINE void _SkipString()
		{
			typename TdTraits<T>::ElmType ch;
			do {
				this->fAllocator.rChar(_ActualPosition, ch);
				this->_ActualPosition += sizeof(ch);
			} while (ch != 0);
			this->_CurrentIndex ++;
		}

		COREARRAY_INLINE void _WriteString(ElmTypeEx val)
		{
			typename TdTraits<T>::ElmType Ch = 0;
			size_t pos = val.find(Ch);
			if (pos != string::npos) val.resize(pos);
			ssize_t str_size = val.length() * (TdTraits<T>::BitOf / 8);

			int old_len = 0;
			SIZE64 I = this->_ActualPosition;
			do {
				this->fAllocator.rChar(I, Ch);
				I += sizeof(Ch);
				if (Ch != 0) old_len ++;
			} while (Ch != 0);

			if (old_len > (int)val.length())
			{
				this->fAllocator.Move(this->_ActualPosition + old_len,
					this->_ActualPosition + (int)val.length(),
					this->_TotalSize - this->_ActualPosition - old_len);
				this->_TotalSize -= (old_len - (int)val.length());
			} else if (old_len < (int)val.length())
			{
				this->fAllocator.Move(this->_ActualPosition + old_len,
					this->_ActualPosition + (int)val.length(),
					this->_TotalSize - this->_ActualPosition - old_len);
				this->_TotalSize += ((int)val.length() - old_len);
			}

			this->fAllocator.Write(this->_ActualPosition, val.c_str(), str_size);
			Ch = 0;
			this->fAllocator.Write(this->_ActualPosition + str_size, &Ch, sizeof(Ch));

			this->_ActualPosition += str_size + sizeof(Ch);
			this->_CurrentIndex ++;
		}

		COREARRAY_INLINE void _AppendString(ElmTypeEx val)
		{
			typename TdTraits<T>::ElmType Ch = 0;
			size_t pos = val.find(Ch);
			if (pos != string::npos) val.resize(pos);
			ssize_t str_size = (ssize_t)(val.length() * sizeof(Ch));

			SIZE64 OldSize = _TotalSize;
			this->fAllocator.Write(OldSize, val.c_str(), str_size);
			Ch = 0;
			this->fAllocator.Write(OldSize + str_size, &Ch, sizeof(Ch));

			this->_TotalSize += str_size + (ssize_t)sizeof(Ch);
		}

		COREARRAY_INLINE void _Find_Position(C_Int64 Index)
		{
			Index /= sizeof(typename TdTraits<T>::ElmType);

			if (Index < this->_CurrentIndex)
				this->_RewindIndex();
			while (this->_CurrentIndex < Index)
			{
				typename TdTraits<T>::ElmType ch;
				do {
					this->fAllocator.rChar(this->_ActualPosition, ch);
					this->_ActualPosition += sizeof(ch);
				} while (ch != 0);
				this->_CurrentIndex ++;
			}
		}

	protected:
		virtual void _Assign(TdIterator &it, TdIterator &source)
			{ this->_StrTo(it, source.toStr()); };
		virtual int _Compare(TdIterator &it1, TdIterator &it2)
			{ return it1.toStr().compare(it2.toStr()); };
	};


	template<typename TOutside, typename TInside, int O>
		struct VEC_DATA<TOutside, TInside, false, O, COREARRAY_TR_VARIABLE_LENGTH_STRING>
	{
		// Read
		COREARRAY_INLINE static void rItem(void *OutBuffer, const SIZE64 p64, CdVectorX &Seq)
		{
			void *tmp = (void*)&Seq;
			CdVarLenStr<TInside> *IT = (CdVarLenStr<TInside> *)tmp;
			IT->_Find_Position(p64);
			*static_cast<TOutside*>(OutBuffer) =
				ValCvt<TOutside>(IT->_ReadString());
		}

		static void rArray(TIterVDataExt &Rec)
		{
			void *tmp = (void*)(Rec.Seq);
			CdVarLenStr<TInside> *IT = (CdVarLenStr<TInside> *)tmp;
			IT->_Find_Position(Rec.p64);
			TOutside *pt = (TOutside*)Rec.pBuf;
			for (ssize_t L = Rec.LastDim; L > 0; L--)
			{
				*pt++ = ValCvt<TOutside>(IT->_ReadString());
				Rec.p64 += sizeof(typename TdTraits<TInside>::ElmType);
			}
			Rec.pBuf = (char*)pt;
		}

		static void rArrayEx(TIterVDataExt &Rec, const C_BOOL *Sel)
		{
			void *tmp = (void*)(Rec.Seq);
			CdVarLenStr<TInside> *IT = (CdVarLenStr<TInside> *)tmp;
			IT->_Find_Position(Rec.p64);
			TOutside *pt = (TOutside*)Rec.pBuf;
			for (ssize_t L = Rec.LastDim; L > 0; L--)
			{
				if (*Sel++)
					*pt++ = ValCvt<TOutside>(IT->_ReadString());
				else
					IT->_SkipString();
				Rec.p64 += sizeof(typename TdTraits<TInside>::ElmType);
			}
			Rec.pBuf = (char*)pt;
		}

		// Write
		COREARRAY_INLINE static void wItem(void *InBuffer, SIZE64 p64, CdVectorX &Seq)
		{
			void *tmp = (void*)&Seq;
			CdVarLenStr<TInside> *IT = (CdVarLenStr<TInside> *)tmp;
			IT->_Find_Position(p64);

			typename TInside::TType val(
				ValCvt<typename TInside::TType, TOutside>(
				*static_cast<TOutside*>(InBuffer)) );

			IT->_Find_Position(p64);
			IT->_WriteString(val);
		}

		static void wArray(TIterVDataExt &Rec)
		{
			void *tmp = (void*)(Rec.Seq);
			CdVarLenStr<TInside> *IT = (CdVarLenStr<TInside> *)tmp;
			TOutside *pt;
			typename TInside::TType val;
			pt = (TOutside*)Rec.pBuf;

			if (!Rec.AppendMode)
				IT->_Find_Position(Rec.p64);
			for (ssize_t L = Rec.LastDim; L > 0; L--)
			{
				val = ValCvt<typename TInside::TType, TOutside>(*pt++);
				if (!Rec.AppendMode)
					IT->_WriteString(val);
				else
					IT->_AppendString(val);
				Rec.p64 += sizeof(typename TdTraits<TInside>::ElmType);
			}
			Rec.pBuf = (char*)pt;
		}
	};


	/// Variable-length of UTF-8 string
	typedef CdVarLenStr< VARIABLE_LENGTH<UTF8*> >        CdVStr8;
	/// Variable-length of UTF-16 string
	typedef CdVarLenStr< VARIABLE_LENGTH<UTF16*> >       CdVStr16;
	/// Variable-length of UTF-32 string
	typedef CdVarLenStr< VARIABLE_LENGTH<UTF32*> >       CdVStr32;
}

#endif /* _HEADER_COREARRAY_STRING_GDS_ */
