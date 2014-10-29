// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStruct.h: Data container - array, matrix, etc
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
 *	\file     dStruct.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Data container - array, matrix, etc
 *	\details
**/

#ifndef _HEADER_COREARRAY_STRUCT_
#define _HEADER_COREARRAY_STRUCT_

#include "dFile.h"
#include "dAllocator.h"


namespace CoreArray
{
	// =====================================================================
	// CdIterator: the iterator for container
	// =====================================================================

	class CdContainer;

	/// Iterator for CoreArray container
	class COREARRAY_DLL_DEFAULT CdIterator: public CdBaseIterator
	{
	public:
		/// the handler of this iterator
		CdContainer *Handler;

		/// next position
		CdIterator& operator++();
		/// previous position
		CdIterator& operator--();
		/// move to next offset position
		CdIterator& operator+= (SIZE64 offset);
		/// move back to previous offset position
		CdIterator& operator-= (SIZE64 offset);

		/// get an integer
		C_Int64 GetInteger();
		/// get a float number
		double GetFloat();
		/// get a string
		UTF16String GetString();

		/// set an integer
		void SetInteger(C_Int64 val);
		/// set a float number
		void SetFloat(double val);
		/// set a string
		void SetString(const UTF16String &val);

		/// read an array of data from this iterator
		void *ReadData(void *OutBuf, ssize_t n, C_SVType OutSV);
		/// write an array of data to the position of this iterator
		const void *WriteData(const void *InBuf, ssize_t n, C_SVType InSV);
	};

	/// The pointer to an iterator
	typedef CdIterator *PdIterator;



	// =====================================================================
	// CdContainer: the CoreArray container
	// =====================================================================

	/// The root class for CoreArray container
	/** This class provide basic functions for a container. **/
	class COREARRAY_DLL_DEFAULT CdContainer: public CdGDSObjPipe
	{
	public:
		friend class CdIterator;

		CdContainer();
		virtual ~CdContainer();

		/// Throw a ErrArray error
        virtual CdGDSObj *NewOne(void *Param=NULL) = 0;
		/// Throw a ErrArray error
		virtual void AssignOne(CdGDSObj &Source, void *Param=NULL)
		{
			AssignOneEx(Source, false, Param);
		}

		virtual void AssignOneEx(CdGDSObj &Source, bool Append=false,
			void *Param=NULL);

    	/// Return C_SVType of data type
		virtual C_SVType SVType() = 0;
		/// Return number of bits for the element type
		virtual unsigned BitOf() = 0;

		/// Clear the container
		virtual void Clear() = 0;
		/// Return true, if the container is empty
		virtual bool Empty() = 0;
		/// Return total number of elements in the container
		virtual C_Int64 TotalCount() = 0;

		/// Close the writing mode if it is in compression, and sync the file
		virtual void CloseWriter() = 0;

		/// Cache the data in memory depending on the operating system
		virtual void Caching();

		/// Get the size of data in the GDS file/stream
		virtual SIZE64 GDSStreamSize();


		// Iterator functions

		/// the starting iterator
		virtual CdIterator IterBegin() = 0;
		/// the end iterator
		virtual CdIterator IterEnd() = 0;

	protected:

		// Iterator functions

		/// offset the iterator
		virtual void IterOffset(CdIterator &I, SIZE64 val) = 0;
		/// get an integer
		virtual C_Int64 IterGetInteger(CdIterator &I) = 0;
		/// get a float number
		virtual double IterGetFloat(CdIterator &I) = 0;
		/// get a string
		virtual UTF16String IterGetString(CdIterator &I) = 0;
		/// set an integer
		virtual void IterSetInteger(CdIterator &I, C_Int64 val) = 0;
		/// set a float number
		virtual void IterSetFloat(CdIterator &I, double val) = 0;
		/// set a string
		virtual void IterSetString(CdIterator &I, const UTF16String &val) = 0;
		/// read an array of data from this iterator
		virtual void *IterRData(CdIterator &I, void *OutBuf, ssize_t n,
			C_SVType OutSV);
		/// write an array of data to the position of this iterator
		virtual const void *IterWData(CdIterator &I, const void *InBuf,
			ssize_t n, C_SVType InSV);
	};

	/// The pointer to a GDS container
	typedef CdContainer *PdContainer;



	// =====================================================================
	// CdAbstractArray
	// =====================================================================

	/// Container of array-oriented data
	class COREARRAY_DLL_DEFAULT CdAbstractArray: public CdContainer
	{
	public:
		/// the maximum number of dimensions
		static const size_t MAX_ARRAY_DIM = 256u;
		/// dimension type
		typedef C_Int32 TArrayDim[MAX_ARRAY_DIM];

		/// Constructor
		CdAbstractArray();
		/// Destructor
		virtual ~CdAbstractArray();

		virtual void AssignOneEx(CdGDSObj &Source, bool Append=false,
			void *Param=NULL);

		/// get how many dimensions
		virtual int DimCnt() const = 0;
		/// get the dimensions
		virtual void GetDim(C_Int32 DimLen[]) const = 0;
		/// reset the dimensions
		virtual void ResetDim(const C_Int32 DimLen[], int DCnt) = 0;
		/// get the length of specified dimension
		virtual C_Int32 GetDLen(int I) const = 0;
		/// set the length of specified dimension
		virtual void SetDLen(int I, C_Int32 Value) = 0;

		/// get how many elements in total according to dimensions
		virtual C_Int64 TotalArrayCount() = 0;


		// Iterator functions
		/// get the iterator corresponding to 'DimIndex'
		virtual CdIterator Iterator(const C_Int32 *DimIndex) = 0;

		/// read array-oriented data
		/** \param Start       the starting positions (from ZERO), it should not be NULL
		 *  \param Length      the lengths of each dimension, it should not be NULL
		 *  \param OutBuffer   the pointer to the output buffer
		 *  \param OutSV       data type of output buffer
		**/
		virtual void ReadData(const C_Int32 *Start, const C_Int32 *Length,
			void *OutBuffer, C_SVType OutSV);

		/// read array-oriented data from the selection
		/** \param Start       the starting positions (from ZERO), it should not be NULL
		 *  \param Length      the lengths of each dimension, it should not be NULL
		 *  \param Selection   the array of selection
		 *  \param OutBuffer   the pointer to the output buffer
		 *  \param OutSV       data type of output buffer
		**/
		virtual void ReadDataEx(const C_Int32 *Start, const C_Int32 *Length,
			const C_BOOL *const Selection[], void *OutBuffer, C_SVType OutSV);

		/// write array-oriented data
		/** \param Start       the starting positions (from ZERO), it should not be NULL
		 *  \param Length      the lengths of each dimension, it should not be NULL
		 *  \param InBuffer    the pointer to the input buffer
		 *  \param InSV        data type of input buffer
		**/
		virtual void WriteData(const C_Int32 *Start, const C_Int32 *Length,
			const void *InBuffer, C_SVType InSV);

		/// append new data
		virtual void Append(void const* Buffer, ssize_t Cnt, C_SVType InSV) = 0;

		/// determine the starting position, block length and valid count from a selection
		/** \param Selection    NULL (the whole dataset), or a selection
		 *  \param OutStart     NULL (ignore), or output the starting position
		 *  \param OutBlockLen  NULL (ignore), or output the block length
		 *  \param OutValidCnt  NULL (ignore), or output the valid count
		**/
		void GetInfoSelection(const C_BOOL *const Selection[],
			C_Int32 OutStart[], C_Int32 OutBlockLen[], C_Int32 OutValidCnt[]);

		/// determine the starting position, block length and valid count from a selection
		/** \param Selection    NULL (the whole dataset), or a selection
		 *  \param OutStart     NULL (ignore), or output the starting position
		 *  \param OutBlockLen  NULL (ignore), or output the block length
		 *  \param OutValidCnt  NULL (ignore), or output the valid count
		**/
		void GetInfoSelection(const C_Int32 Start[], const C_Int32 Length[],
			const C_BOOL *const Selection[],
			C_Int32 OutStart[], C_Int32 OutBlockLen[], C_Int32 OutValidCnt[]);

	protected:

		void _CheckRect(const C_Int32 Start[], const C_Int32 Length[]) const;
		void _AssignToDim(CdAbstractArray &Dest) const;

	private:
		static void IIndex(CdAbstractArray &Obj, CdIterator &I,
			const C_Int32 DimI[]);
	};

	/// The pointer to a sequence object
	typedef CdAbstractArray *PdAbstractArray;


	/// The size of memory buffer
	const ssize_t MEMORY_BUFFER_SIZE = 0x10000;

	/// The size of memory buffer with additional size
	const ssize_t MEMORY_BUFFER_SIZE_PLUS = MEMORY_BUFFER_SIZE + 0x10;



	template<typename TYPE, typename TARRAY, typename F_ITER, typename F_PROC>
	COREARRAY_DLL_DEFAULT
	void ArrayRIterRect(const C_Int32 *Start, const C_Int32 *Length,
		int DimCnt, TARRAY &Obj, TYPE *Buffer, F_ITER SetI, F_PROC Proc)
	{
		if ((Start != NULL) && (Length != NULL))
		{
			CdAbstractArray::TArrayDim DFor, DForLen;
			C_Int32 *ForP = &DFor[0], *ForLenP = &DForLen[0];
			C_Int32 ForI = 0, ForEnd = DimCnt-1;
			SIZE64 Cnt = Length[ForEnd];
			CdIterator I = Obj.IterBegin();

			DFor[0] = Start[0]; DForLen[0] = Length[0];
			while (ForI >= 0)
			{
				if (*ForLenP > 0)
				{
					if (ForI < ForEnd)
					{
						++ForI; *(++ForP) = *(++Start);
						*(++ForLenP) = *(++Length);
						continue;
					} else {
						SetI(Obj, I, DFor);
						Buffer = Proc(I, Buffer, Cnt);
					}
				}
				--ForP; --ForLenP; --Start; --Length; --ForI;
				if (ForI >= 0)
					{ ++(*ForP); --(*ForLenP); }
			}
		} else {
			CdIterator I = Obj.IterBegin();
			Proc(I, Buffer, Obj.TotalCount());
		}
	}

	template<typename TYPE, typename TARRAY, typename F_ITER, typename F_PROC>
	COREARRAY_DLL_DEFAULT
	void ArrayRIterRectEx(const C_Int32 Start[], const C_Int32 Length[],
		const C_BOOL *const Sel[], int DimCnt, TARRAY &Obj, TYPE *Buffer,
		F_ITER SetI, F_PROC Proc)
	{
		CdAbstractArray::TArrayDim DFor, DForLen;
		C_Int32 *ForP = &DFor[0], *ForLenP = &DForLen[0];
		C_Int32 ForI = 0, ForEnd = DimCnt-1;
		const C_BOOL *Selection = Sel[ForEnd];
		SIZE64 Cnt = Length[ForEnd];
		CdIterator I = Obj.IterBegin();

		DFor[0] = Start[0]; DForLen[0] = Length[0];
		while (ForI >= 0)
		{
			if (*ForLenP > 0)
			{
				if (ForI >= ForEnd)
				{
					SetI(Obj, I, DFor);
					Buffer = Proc(I, Buffer, Cnt, Selection);
				} else if (Sel[ForI][*ForP - *Start])
				{
					++ForI; *(++ForP) = *(++Start);
					*(++ForLenP) = *(++Length);
					continue;
				} else {
					++(*ForP); --(*ForLenP);
					continue;
				}
			}

			--ForP; --ForLenP; --Start; --Length; --ForI;
			if (ForI >= 0)
				{ ++(*ForP); --(*ForLenP); }
		}
	}

	template<typename TYPE, typename TARRAY, typename F_ITER, typename F_PROC>
	COREARRAY_DLL_DEFAULT
	void ArrayWIterRect(const C_Int32 *Start, const C_Int32 *Length,
		int DimCnt, TARRAY &Obj, const TYPE *Buffer, F_ITER SetI, F_PROC Proc)
	{
		if ((Start != NULL) && (Length != NULL))
		{
			CdAbstractArray::TArrayDim DFor, DForLen;
			C_Int32 *ForP = &DFor[0], *ForLenP = &DForLen[0];
			C_Int32 ForI = 0, ForEnd = DimCnt-1;
			SIZE64 Cnt = Length[ForEnd];
			CdIterator I = Obj.IterBegin();

			DFor[0] = Start[0]; DForLen[0] = Length[0];
			while (ForI >= 0)
			{
				if (*ForLenP > 0)
				{
					if (ForI < ForEnd)
					{
						++ForI; *(++ForP) = *(++Start);
						*(++ForLenP) = *(++Length);
						continue;
					} else {
						SetI(Obj, I, DFor);
						Buffer = Proc(I, Buffer, Cnt);
					}
				}
				--ForP; --ForLenP; --Start; --Length; --ForI;
				if (ForI >= 0)
					{ ++(*ForP); --(*ForLenP); }
			}
		} else {
			CdIterator I = Obj.IterBegin();
			Proc(I, Buffer, Obj.TotalCount());
		}
	}



	// =====================================================================
	// CdAllocArray: array-oriented container with an allocator
	// =====================================================================

	/// Array-oriented container with CdAllocator
	class COREARRAY_DLL_DEFAULT CdAllocArray: public CdAbstractArray
	{
	public:
		CdAllocArray(ssize_t vElmSize);
		virtual ~CdAllocArray();

		virtual bool Empty();
		virtual void Clear();

		/// Return total number of elements in the container
		virtual C_Int64 TotalCount();

		/// get how many dimensions
		virtual int DimCnt() const;
		/// get the dimensions
		virtual void GetDim(C_Int32 DimLen[]) const;
		/// reset the dimensions
		virtual void ResetDim(const C_Int32 DimLen[], int DCnt);
		/// get the length of specified dimension
		virtual C_Int32 GetDLen(int I) const;
		/// set the length of specified dimension
		virtual void SetDLen(int I, C_Int32 Value);

		/// get how many elements in total according to dimensions
		virtual C_Int64 TotalArrayCount();

		/// the starting iterator
		virtual CdIterator IterBegin();
		/// the end iterator
		virtual CdIterator IterEnd();
		/// get the iterator corresponding to 'DimIndex'
		virtual CdIterator Iterator(const C_Int32 DimIndex[]);


		virtual void Synchronize();
        virtual void CloseWriter();

		virtual void SetPackedMode(const char *Mode);

		/// append new data
		virtual void Append(void const* Buffer, ssize_t Cnt, C_SVType InSV);

		/// Cache the data in memory depending on the operating system
		virtual void Caching();

		/// Get the size of data in the GDS file/stream
		virtual SIZE64 GDSStreamSize();

		/// Get a list of CdBlockStream owned by this object, except fGDSStream
		virtual void GetOwnBlockStream(vector<const CdBlockStream*> &Out);

		/// the size of element
		COREARRAY_FORCEINLINE ssize_t ElmSize() const { return fElmSize; }
		/// the allocator
		COREARRAY_FORCEINLINE CdAllocator &Allocator() { return fAllocator; }


	protected:

		/// the size of element
		ssize_t fElmSize;
		/// the allocator
		CdAllocator fAllocator;

		/// dimensions
		struct TDimItem
		{
			C_Int32 DimLen;
			SIZE64 DimElmSize, DimElmCnt;
			TDimItem() { DimLen = DimElmSize = DimElmCnt = 0; }
		};
		std::vector<TDimItem> fDimension;

		/// the total number
		C_Int64 fTotalCount;

		/// update a part of data, not all; fChanged -- update all
		bool fNeedUpdate;

		/// get the size in byte corresponding to the count 'Num'
		virtual SIZE64 AllocSize(C_Int64 Num);


		// Iterator functions

		/// offset the iterator
		virtual void IterOffset(CdIterator &I, SIZE64 val);
		/// initialize n array
		virtual void IterInit(CdIterator &I, SIZE64 n);
		/// finalize n array
		virtual void IterDone(CdIterator &I, SIZE64 n);


		virtual void Loading(CdReader &Reader, TdVersion Version);
		virtual void Saving(CdWriter &Writer);
        virtual void GetPipeInfo();

		/// call from UpdateInfo
		virtual void UpdateInfoProc(CdBufStream *Sender);

		void UpdateInfo(CdBufStream *Sender);

		/// set new size of element
		void SetElmSize(ssize_t NewSize);


		/// check the validity of dimension index
		void _CheckRange(const C_Int32 DimI[]);
		/// check the validity of dimension
		void _CheckRect(const C_Int32 *Start, const C_Int32 *Length);
		/// get the pointer corresponding to 'DimI'
		SIZE64 _IndexPtr(const C_Int32 DimI[]);
		/// assign values to fDimension
		void _ResetDim(const C_Int32 DimLen[], int DCnt);

		void xDimAuto(int DimIndex);
		void _SetSmallBuffer();
		void _SetLargeBuffer();
		void _SetFlushEvent();

	private:
		TdGDSBlockID vAllocID;
		CdBlockStream *vAllocStream;
		SIZE64 vAlloc_Ptr, vCnt_Ptr;
	};




	// =====================================================================
	// CdAllocArray: array-oriented container with an allocator
	// =====================================================================

	/// Array-oriented container, template class
	/** \tparam T  atomic data type, e.g. C_Int8, C_Int32 **/
	template<typename TYPE>
		class COREARRAY_DLL_DEFAULT CdArray: public CdAllocArray
	{
	public:
		/// define ElmType = TYPE
		typedef TYPE ElmType;

		/// Constructor
		CdArray(): CdAllocArray(
			(TdTraits<TYPE>::BitOf/8) + ((TdTraits<TYPE>::BitOf%8)?1:0))
		{ }

    	/// new a CdArray<T> object
		virtual CdGDSObj *NewOne(void *Param=NULL)
		{
			CdArray<TYPE> *rv = new CdArray<TYPE>;
			_AssignToDim(*rv);
			if (fPipeInfo)
				rv->fPipeInfo = fPipeInfo->NewOne();
			return rv;
		}

		/// assignment
		virtual void AssignOneEx(CdGDSObj &Source, bool Append=false,
			void *Param=NULL)
		{
// TODO
			if (dynamic_cast< CdArray<TYPE>* >(&Source))
			{
				CdArray<TYPE> &Array = *static_cast< CdArray<TYPE>* >(&Source);
				Array._AssignToDim(*this);
			} else
				CdAllocArray::AssignOneEx(Source, Append, Param);
		}

		/// Return a string specifying the class name in stream
		virtual const char *dName()
		{
			return TdTraits<TYPE>::StreamName();
		}

		/// Return a string specifying the class name
		virtual const char *dTraitName()
		{
			return TdTraits<TYPE>::TraitName();
		}

		virtual C_SVType SVType()
		{
			return TdTraits<TYPE>::SVType;
		}

		virtual unsigned BitOf()
		{
			return TdTraits<TYPE>::BitOf;
		}


		virtual void ReadData(const C_Int32 *Start, const C_Int32 *Length,
			void *OutBuffer, C_SVType OutSV)
		{
			_CheckRect(Start, Length);
			switch (OutSV)
			{
				case svInt8:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_Int8*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int8>::Read);
					break;
				case svUInt8:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_UInt8*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt8>::Read);
					break;
				case svInt16:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_Int16*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int16>::Read);
					break;
				case svUInt16:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_UInt16*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt16>::Read);
					break;
				case svInt32:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_Int32*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int32>::Read);
					break;
				case svUInt32:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_UInt32*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt32>::Read);
					break;
				case svInt64:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_Int64*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int64>::Read);
					break;
				case svUInt64:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_UInt64*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt64>::Read);
					break;
				case svFloat32:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_Float32*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Float32>::Read);
					break;
				case svFloat64:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(C_Float64*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Float64>::Read);
					break;
				case svStrUTF8:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(UTF8String*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, UTF8String>::Read);
					break;
				case svStrUTF16:
					ArrayRIterRect(Start, Length, fDimension.size(), *this,
						(UTF16String*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, UTF16String>::Read);
					break;
				default:
					CdAllocArray::ReadData(Start, Length, OutBuffer, OutSV);
			}
		}

		virtual void ReadDataEx(const C_Int32 *Start, const C_Int32 *Length,
			const C_BOOL *const Selection[], void *OutBuffer, C_SVType OutSV)
		{
			if (Selection == NULL)
			{
				ReadData(Start, Length, OutBuffer, OutSV);
				return;
			}
			_CheckRect(Start, Length);
			switch (OutSV)
			{
				case svInt8:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_Int8*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int8>::ReadEx);
					break;
				case svUInt8:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_UInt8*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt8>::ReadEx);
					break;
				case svInt16:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_Int16*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int16>::ReadEx);
					break;
				case svUInt16:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_UInt16*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt16>::ReadEx);
					break;
				case svInt32:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_Int32*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int32>::ReadEx);
					break;
				case svUInt32:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_UInt32*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt32>::ReadEx);
					break;
				case svInt64:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_Int64*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int64>::ReadEx);
					break;
				case svUInt64:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_UInt64*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt64>::ReadEx);
					break;
				case svFloat32:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_Float32*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Float32>::ReadEx);
					break;
				case svFloat64:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(C_Float64*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, C_Float64>::ReadEx);
					break;
				case svStrUTF8:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(UTF8String*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, UTF8String>::ReadEx);
					break;
				case svStrUTF16:
					ArrayRIterRectEx(Start, Length, Selection, fDimension.size(), *this,
						(UTF16String*)OutBuffer, IIndex, ALLOC_FUNC<TYPE, UTF16String>::ReadEx);
					break;
				default:
					CdAllocArray::ReadDataEx(Start, Length, Selection, OutBuffer, OutSV);
			}
		}

		virtual void WriteData(const C_Int32 *Start, const C_Int32 *Length,
			const void *InBuffer, C_SVType InSV)
		{
			_CheckRect(Start, Length);
			switch (InSV)
			{
				case svInt8:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_Int8*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int8>::Write);
					break;
				case svUInt8:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_UInt8*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt8>::Write);
					break;
				case svInt16:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_Int16*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int16>::Write);
					break;
				case svUInt16:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_UInt16*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt16>::Write);
					break;
				case svInt32:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_Int32*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int32>::Write);
					break;
				case svUInt32:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_UInt32*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt32>::Write);
					break;
				case svInt64:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_Int64*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_Int64>::Write);
					break;
				case svUInt64:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_UInt64*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_UInt64>::Write);
					break;
				case svFloat32:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_Float32*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_Float32>::Write);
					break;
				case svFloat64:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const C_Float64*)InBuffer, IIndex, ALLOC_FUNC<TYPE, C_Float64>::Write);
					break;
				case svStrUTF8:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const UTF8String*)InBuffer, IIndex, ALLOC_FUNC<TYPE, UTF8String>::Write);
					break;
				case svStrUTF16:
					ArrayWIterRect(Start, Length, DimCnt(), *this,
						(const UTF16String*)InBuffer, IIndex, ALLOC_FUNC<TYPE, UTF16String>::Write);
					break;
				default:
					CdAllocArray::WriteData(Start, Length, InBuffer, InSV);
			}
		}


		/// append new data
		virtual void Append(const void *Buffer, ssize_t Cnt, C_SVType InSV)
		{
			if (Cnt <= 0) return;

			// writing
			// TODO+++
			_SetLargeBuffer();
			CdIterator I = IterEnd();
			switch (InSV)
			{
				case svInt8:
					ALLOC_FUNC<TYPE, C_Int8>::Write(I, (const C_Int8*)Buffer, Cnt);
					break;
				case svUInt8:
					ALLOC_FUNC<TYPE, C_UInt8>::Write(I, (const C_UInt8*)Buffer, Cnt);
					break;
				case svInt16:
					ALLOC_FUNC<TYPE, C_Int16>::Write(I, (const C_Int16*)Buffer, Cnt);
					break;
				case svUInt16:
					ALLOC_FUNC<TYPE, C_UInt16>::Write(I, (const C_UInt16*)Buffer, Cnt);
					break;
				case svInt32:
					ALLOC_FUNC<TYPE, C_Int32>::Write(I, (const C_Int32*)Buffer, Cnt);
					break;
				case svUInt32:
					ALLOC_FUNC<TYPE, C_UInt32>::Write(I, (const C_UInt32*)Buffer, Cnt);
					break;
				case svInt64:
					ALLOC_FUNC<TYPE, C_Int64>::Write(I, (const C_Int64*)Buffer, Cnt);
					break;
				case svUInt64:
					ALLOC_FUNC<TYPE, C_UInt64>::Write(I, (const C_UInt64*)Buffer, Cnt);
					break;
				case svFloat32:
					ALLOC_FUNC<TYPE, C_Float32>::Write(I, (const C_Float32*)Buffer, Cnt);
					break;
				case svFloat64:
					ALLOC_FUNC<TYPE, C_Float64>::Write(I, (const C_Float64*)Buffer, Cnt);
					break;
				case svStrUTF8:
					ALLOC_FUNC<TYPE, UTF8String>::Write(I, (const UTF8String*)Buffer, Cnt);
					break;
				case svStrUTF16:
					ALLOC_FUNC<TYPE, UTF16String>::Write(I, (const UTF16String*)Buffer, Cnt);
					break;
				default:
					CdAllocArray::Append(Buffer, Cnt, InSV);
			}

			// check
			TDimItem &R = fDimension.front();
			fTotalCount += Cnt;
			if (fTotalCount >= R.DimElmCnt*(R.DimLen+1))
			{
				R.DimLen = fTotalCount / R.DimElmCnt;
				_SetFlushEvent();
				fNeedUpdate = true;
			}
		}



/*
// TODO
		virtual void AppendIter(CdIterator &iter, ssize_t Cnt)
		{
			if (Cnt > 0)
			{
				vector<ElmTypeEx> buf(Cnt);
				ElmTypeEx *p = &(buf[0]);
				for (ssize_t L=Cnt; L > 0; L--)
				{
					*p++ = _INTERNAL::TdIterToVal<ElmTypeEx>::Get(iter);
					++iter;
				}
				Append(&(buf[0]), Cnt, TdTraits<ElmTypeEx>::SVType);
			}
		}
*/

	protected:

		/// Constructor
		CdArray(ssize_t vElmSize): CdAllocArray(vElmSize) {}

		/// get an integer
		virtual C_Int64 IterGetInteger(CdIterator &I)
		{
			C_Int64 ans;
			ALLOC_FUNC<TYPE, C_Int64>::Read(I, &ans, 1);
			return ans;
		}

		/// get a float number
		virtual double IterGetFloat(CdIterator &I)
		{
			double ans;
			ALLOC_FUNC<TYPE, double>::Read(I, &ans, 1);
			return ans;
		}

		/// get a string
		virtual UTF16String IterGetString(CdIterator &I)
		{
			UTF16String ans;
			ALLOC_FUNC<TYPE, UTF16String>::Read(I, &ans, 1);
			return ans;
		}

		/// set an integer
		virtual void IterSetInteger(CdIterator &I, C_Int64 val)
		{
			ALLOC_FUNC<TYPE, C_Int64>::Write(I, &val, 1);
		}

		/// set a float number
		virtual void IterSetFloat(CdIterator &I, double val)
		{
			ALLOC_FUNC<TYPE, double>::Write(I, &val, 1);
		}

		/// set a string
		virtual void IterSetString(CdIterator &I, const UTF16String &val)
		{
			ALLOC_FUNC<TYPE, UTF16String>::Write(I, &val, 1);
		}

		/// read an array of data from this iterator
		virtual void *IterRData(CdIterator &I, void *OutBuf, ssize_t n,
			C_SVType OutSV)
		{
			switch (OutSV)
			{
				case svInt8:
					return ALLOC_FUNC<TYPE, C_Int8>::Read(I, (C_Int8*)OutBuf, n);
				case svUInt8:
					return ALLOC_FUNC<TYPE, C_UInt8>::Read(I, (C_UInt8*)OutBuf, n);
				case svInt16:
					return ALLOC_FUNC<TYPE, C_Int16>::Read(I, (C_Int16*)OutBuf, n);
				case svUInt16:
					return ALLOC_FUNC<TYPE, C_UInt16>::Read(I, (C_UInt16*)OutBuf, n);
				case svInt32:
					return ALLOC_FUNC<TYPE, C_Int32>::Read(I, (C_Int32*)OutBuf, n);
				case svUInt32:
					return ALLOC_FUNC<TYPE, C_UInt32>::Read(I, (C_UInt32*)OutBuf, n);
				case svInt64:
					return ALLOC_FUNC<TYPE, C_Int64>::Read(I, (C_Int64*)OutBuf, n);
				case svUInt64:
					return ALLOC_FUNC<TYPE, C_UInt64>::Read(I, (C_UInt64*)OutBuf, n);
				case svFloat32:
					return ALLOC_FUNC<TYPE, C_Float32>::Read(I, (C_Float32*)OutBuf, n);
				case svFloat64:
					return ALLOC_FUNC<TYPE, C_Float64>::Read(I, (C_Float64*)OutBuf, n);
				case svStrUTF8:
					return ALLOC_FUNC<TYPE, UTF8String>::Read(I, (UTF8String*)OutBuf, n);
				case svStrUTF16:
					return ALLOC_FUNC<TYPE, UTF16String>::Read(I, (UTF16String*)OutBuf, n);
				default:
					return CdAllocArray::IterRData(I, OutBuf, n, OutSV);
			}
		}

		/// write an array of data to the position of this iterator
		virtual const void *IterWData(CdIterator &I, const void *InBuf,
			ssize_t n, C_SVType InSV)
		{
			switch (InSV)
			{
				case svInt8:
					return ALLOC_FUNC<TYPE, C_Int8>::Write(I, (const C_Int8*)InBuf, n);
				case svUInt8:
					return ALLOC_FUNC<TYPE, C_UInt8>::Write(I, (const C_UInt8*)InBuf, n);
				case svInt16:
					return ALLOC_FUNC<TYPE, C_Int16>::Write(I, (const C_Int16*)InBuf, n);
				case svUInt16:
					return ALLOC_FUNC<TYPE, C_UInt16>::Write(I, (const C_UInt16*)InBuf, n);
				case svInt32:
					return ALLOC_FUNC<TYPE, C_Int32>::Write(I, (const C_Int32*)InBuf, n);
				case svUInt32:
					return ALLOC_FUNC<TYPE, C_UInt32>::Write(I, (const C_UInt32*)InBuf, n);
				case svInt64:
					return ALLOC_FUNC<TYPE, C_Int64>::Write(I, (const C_Int64*)InBuf, n);
				case svUInt64:
					return ALLOC_FUNC<TYPE, C_UInt64>::Write(I, (const C_UInt64*)InBuf, n);
				case svFloat32:
					return ALLOC_FUNC<TYPE, C_Float32>::Write(I, (const C_Float32*)InBuf, n);
				case svFloat64:
					return ALLOC_FUNC<TYPE, C_Float64>::Write(I, (const C_Float64*)InBuf, n);
				case svStrUTF8:
					return ALLOC_FUNC<TYPE, UTF8String>::Write(I, (const UTF8String*)InBuf, n);
				case svStrUTF16:
					return ALLOC_FUNC<TYPE, UTF16String>::Write(I, (const UTF16String*)InBuf, n);
				default:
					return CdAllocArray::IterWData(I, InBuf, n, InSV);
			}
		}

	private:
		COREARRAY_FORCEINLINE static void IIndex(CdArray<TYPE> &Obj,
			CdIterator &I, const C_Int32 DimI[])
		{
			I.Ptr = Obj._IndexPtr(DimI);
		}
	};



	// =====================================================================
	// Exception
	// =====================================================================

	/// Exception for container
	class COREARRAY_DLL_EXPORT ErrContainer: public ErrObject
	{
	public:
		ErrContainer(): ErrObject()
			{ }
		ErrContainer(const string &msg): ErrObject()
			{ fMessage = msg; }
		ErrContainer(const char *fmt, ...): ErrObject()
			{ _COREARRAY_ERRMACRO_(fmt); }
	};

	/// Exception for array-oriented container
	class COREARRAY_DLL_EXPORT ErrArray: public ErrContainer
	{
	public:
		ErrArray(): ErrContainer()
			{ }
		ErrArray(const string &msg): ErrContainer()
			{ fMessage = msg; }
		ErrArray(const char *fmt, ...): ErrContainer()
			{ _COREARRAY_ERRMACRO_(fmt); }
	};



	// =====================================================================
	// Classes
	// =====================================================================

	/// Array of signed integer with 8 bits
	typedef CdArray<C_Int8>        CdInt8;
	/// Array of unsigned integer with 8 bits
	typedef CdArray<C_UInt8>       CdUInt8;
	/// Array of signed integer with 16 bits
	typedef CdArray<C_Int16>       CdInt16;
	/// Array of unsigned integer with 16 bits
	typedef CdArray<C_UInt16>      CdUInt16;
	/// Array of signed integer with 32 bits
	typedef CdArray<C_Int32>       CdInt32;
	/// Array of unsigned integer with 32 bits
	typedef CdArray<C_UInt32>      CdUInt32;
	/// Array of signed integer with 64 bits
	typedef CdArray<C_Int64>       CdInt64;
	/// Array of unsigned integer with 64 bits
	typedef CdArray<C_UInt64>      CdUInt64;

	/// Array of float number with single precision
	typedef CdArray<C_Float32>     CdFloat32;
	/// Array of float number with double precision
	typedef CdArray<C_Float64>     CdFloat64;



	// =====================================================================
	// Apply functions by margin
	// =====================================================================

	/// the size of memory buffer for reading dataset marginally, by default 1G
	extern C_Int64 ARRAY_READ_MEM_BUFFER_SIZE;

	/// read an array-oriented object margin by margin
	class COREARRAY_DLL_DEFAULT CdArrayRead
	{
	public:

		CdArrayRead();
		~CdArrayRead();

		void Init(CdAbstractArray &vObj, int vMargin, C_SVType vSVType,
			const C_BOOL *const vSelection[], bool buf_if_need=true);

		/// allocate memory buffer if needed
		/** \param  buffer_size  the size of memory buffer; if -1,
		 *                       'buffer_size = ARRAY_READ_MEM_BUFFER_SIZE'
		 */
		void AllocBuffer(C_Int64 buffer_size);

		/// read data
		void Read(void *Buffer);

		/// return true, if it is of the end
		bool Eof();

		/// return an object
		COREARRAY_INLINE CdAbstractArray &Object() { return *fObject; }
		/// return margin index
		COREARRAY_INLINE int Margin() { return fMargin; }
		/// 
		COREARRAY_INLINE C_SVType SVType() { return fSVType; }
		///
		COREARRAY_INLINE ssize_t ElmSize() { return fElmSize; }

		COREARRAY_INLINE C_Int32 Index() { return fIndex; }
		COREARRAY_INLINE C_Int32 Count() { return fCount; }

		COREARRAY_INLINE C_Int32 MarginIndex() { return fMarginIndex; }
		COREARRAY_INLINE C_Int64 MarginCount() { return fMarginCount; }
		COREARRAY_INLINE C_Int64 MarginSize() { return fMarginCount * fElmSize; }

		COREARRAY_INLINE const C_Int32 *DimCntValid() { return _DCntValid; }

	protected:
		CdAbstractArray *fObject;
		int fMargin;
		C_SVType fSVType;
		ssize_t fElmSize;

		C_Int32 fIndex, fCount, fMarginIndex;
		C_Int64 fMarginCount;

		CdAbstractArray::TArrayDim _DStart;
		CdAbstractArray::TArrayDim _DCount;
		CdAbstractArray::TArrayDim _DCntValid;

		/// a variable of selection
		const C_BOOL* _Selection[CdAbstractArray::MAX_ARRAY_DIM];
		/// 
		bool _Have_Selection;
		/// true for calling ReadData, false for calling ReadDataEx
		bool _Call_rData;
		bool _Margin_Call_rData;

		C_Int32 _MarginStart;
		C_Int32 _MarginEnd;

		vector< vector<C_BOOL> > _sel_array;

		/// the buffer for numeric data
		vector<C_UInt8> _Margin_Buffer;
		/// the buffer for UTF8String
		vector<UTF8String> _Margin_Buffer_UTF8;
		/// the buffer for UTF16String
		vector<UTF16String> _Margin_Buffer_UTF16;

		/// the pointer to the buffer
		void *_Margin_Buffer_Ptr;

		C_Int32 _Margin_Buf_IncCnt;
		C_Int32 _Margin_Buf_Cnt;
		C_Int32 _Margin_Buf_Old_Index;
		bool _Margin_Buf_Need;
		C_Int64 _Margin_Buf_MajorCnt;
		C_Int64 _Margin_Buf_MinorSize;
		C_Int64 _Margin_Buf_MinorSize2;
	};

	/// read an array-oriented object margin by margin
	typedef CdArrayRead* PdArrayRead;


	/// reallocate the buffer with specified size with respect to array
	COREARRAY_DLL_DEFAULT void Balance_ArrayRead_Buffer(
		CdArrayRead *array[], int n, C_Int64 buffer_size=-1);

	/// reallocate the buffer with specified size with respect to array
	COREARRAY_DLL_DEFAULT void Balance_ArrayRead_Buffer(
		CdArrayRead array[], int n, C_Int64 buffer_size=-1);
}

#endif /* _HEADER_COREARRAY_STRUCT_ */
