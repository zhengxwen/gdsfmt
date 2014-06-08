// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dStream.h: Stream classes and functions
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
 *	\file     dStream.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Stream classes and functions
 *	\details
**/

#ifndef _HEADER_COREARRAY_STREAM_
#define _HEADER_COREARRAY_STREAM_

#include <dBase.h>
#include <string.h>
#include <zlib.h>
#include <vector>

#ifdef COREARRAY_PLATFORM_UNIX
#  include <sys/types.h>
#  include <unistd.h>
#endif


namespace CoreArray
{
	using namespace std;

	/// the sizes of different block
	enum {
		MinBlock    = 65536,		// 64K
		SmallBlock  = 1024*1024*8,	// 8MB
		MediumBlock = 1024*1024*32,	// 32MB
		LargeBlock  = 1024*1024*96,	// 64MB
		HugeBlock   = 1024*1024*512	// 512MB
	};

	/// the levels of allocator
	enum TAllocLevel {
		blChunkMemory,	//< memory
		blTempFile,		//< temporary file
		blBufStream,	//< buffer stream
		blUnknown		//< undefined
	};

	/// the definition of allocator
	struct COREARRAY_DLL_DEFAULT TdAllocator
	{
	public:
		typedef COREARRAY_FASTCALL void (*TacDone)(TdAllocator &obj);
		typedef COREARRAY_FASTCALL void (*TacCapacity)(TdAllocator &obj, const SIZE64 Size);
		typedef COREARRAY_FASTCALL void (*TacRead)(TdAllocator &obj, const SIZE64 I, void *Buf, ssize_t Len);
		typedef COREARRAY_FASTCALL void (*TacWrite)(TdAllocator &obj, const SIZE64 I, void const* Buf, ssize_t Len);
		typedef COREARRAY_FASTCALL void (*TacFill8)(TdAllocator &obj, const SIZE64 I, const SIZE64 Len, C_UInt8 Value);
		typedef COREARRAY_FASTCALL void (*TacMove)(TdAllocator &obj, const SIZE64 Source, const SIZE64 Dest, const SIZE64 Len);
		typedef COREARRAY_FASTCALL void (*TacSwap)(TdAllocator &obj, const SIZE64 I1, const SIZE64 I2, const SIZE64 Len);
		typedef COREARRAY_FASTCALL int (*TacCompare)(TdAllocator &obj, const SIZE64 I, const void *Buf, ssize_t Len);
		typedef COREARRAY_FASTCALL C_UInt8 (*TacRead8)(TdAllocator &obj, const SIZE64 I);
		typedef COREARRAY_FASTCALL C_UInt16 (*TacRead16)(TdAllocator &obj, const SIZE64 I);
		typedef COREARRAY_FASTCALL C_UInt32 (*TacRead32)(TdAllocator &obj, const SIZE64 I);
		typedef COREARRAY_FASTCALL C_UInt64 (*TacRead64)(TdAllocator &obj, const SIZE64 I);
		typedef COREARRAY_FASTCALL float (*TacRead32f)(TdAllocator &obj, const SIZE64 I);
		typedef COREARRAY_FASTCALL double (*TacRead64f)(TdAllocator &obj, const SIZE64 I);
		typedef COREARRAY_FASTCALL void (*TacWrite8)(TdAllocator &obj, const SIZE64 I, C_UInt8 Value);
		typedef COREARRAY_FASTCALL void (*TacWrite16)(TdAllocator &obj, const SIZE64 I, C_UInt16 Value);
		typedef COREARRAY_FASTCALL void (*TacWrite32)(TdAllocator &obj, const SIZE64 I, C_UInt32 Value);
		typedef COREARRAY_FASTCALL void (*TacWrite64)(TdAllocator &obj, const SIZE64 I, const C_UInt64 Value);
		typedef COREARRAY_FASTCALL void (*TacWrite32f)(TdAllocator &obj, const SIZE64 I, const float Value);
		typedef COREARRAY_FASTCALL void (*TacWrite64f)(TdAllocator &obj, const SIZE64 I, const double Value);

		TAllocLevel Level;
		SIZE64 Capacity;

		TacDone _Done;
		TacCapacity _SetCapacity;
		TacRead _Read;
		TacWrite _Write;
		TacFill8 _Fill;
		TacMove _Move;
		TacSwap _Swap;
		TacCompare _Compare;
		TacRead8 _r8;
		TacRead16 _r16;
		TacRead32 _r32;
		TacRead64 _r64;
		TacRead32f _r32f;
		TacRead64f _r64f;
		TacWrite8 _w8;
		TacWrite16 _w16;
		TacWrite32 _w32;
		TacWrite64 _w64;
		TacWrite32f _w32f;
		TacWrite64f _w64f;

		union
		{
			unsigned char *Base;
			CdBufStream* Filter;
		};

	public:
		TdAllocator() { memset((void*)this, 0, sizeof(TdAllocator)); }
		~TdAllocator() { if (_Done) { _Done(*this); _Done = NULL; } }

		COREARRAY_INLINE bool MemLevel() const { return (Level <= blChunkMemory); }

		COREARRAY_INLINE void SetCapacity(const SIZE64 Size) { _SetCapacity(*this, Size); }

		COREARRAY_INLINE void Fill(const SIZE64 I, const SIZE64 Len, C_UInt8 val)
			{ _Fill(*this, I, Len, val); }
		COREARRAY_INLINE void Move(const SIZE64 Source, const SIZE64 Dest, const SIZE64 Len)
			{ _Move(*this, Source, Dest, Len); }
		COREARRAY_INLINE void Swap(const SIZE64 I1, const SIZE64 I2, const SIZE64 Len)
        	{ _Swap(*this, I1, I2, Len); }

		COREARRAY_INLINE void Read(const SIZE64 I, void *Buf, ssize_t Len)
			{ _Read(*this, I, Buf, Len); }
		COREARRAY_INLINE C_UInt8 r8(const SIZE64 I) { return _r8(*this, I); }
		COREARRAY_INLINE C_UInt16 r16(const SIZE64 I) { return _r16(*this, I); }
		COREARRAY_INLINE C_UInt32 r32(const SIZE64 I) { return _r32(*this, I); }
		COREARRAY_INLINE C_UInt64 r64(const SIZE64 I) { return _r64(*this, I); }

		COREARRAY_INLINE void rChar(const SIZE64 I, UTF8 &out) { out = _r8(*this, I); }
		COREARRAY_INLINE void rChar(const SIZE64 I, UTF16 &out) { out = _r16(*this, I); }
		COREARRAY_INLINE void rChar(const SIZE64 I, UTF32 &out) { out = _r32(*this, I); }

		COREARRAY_INLINE void Write(const SIZE64 I, void const* Buf, ssize_t Len)
			{ _Write(*this, I, Buf, Len); }
		COREARRAY_INLINE void w8(const SIZE64 I, C_UInt8 val) { _w8(*this, I, val); }
		COREARRAY_INLINE void w16(const SIZE64 I, C_UInt16 val) { _w16(*this, I, val); }
		COREARRAY_INLINE void w32(const SIZE64 I, C_UInt32 val) { _w32(*this, I, val); }
		COREARRAY_INLINE void w64(const SIZE64 I, C_UInt64 val) { _w64(*this, I, val); }
	};

	/// Exception for TdAllocator
	class COREARRAY_DLL_EXPORT ErrAllocator: public ErrObject
	{
	public:
		enum EdAllocType { eaRead, eaWrite };

		ErrAllocator(EdAllocType Ed);
		ErrAllocator(const char *fmt, ...): ErrObject()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ErrAllocator(TAllocLevel OldLevel, TAllocLevel NewLevel);
	};

	class COREARRAY_DLL_EXPORT ErrAllocRead: public ErrAllocator
	{
	public:
		ErrAllocRead(): ErrAllocator(eaRead) { }
	};

	class COREARRAY_DLL_EXPORT ErrAllocWrite: public ErrAllocator
	{
	public:
		ErrAllocWrite(): ErrAllocator(eaWrite) { }
	};


	COREARRAY_DLL_DEFAULT void InitAllocator(TdAllocator &Allocator,
		bool CanRead, bool CanWrite, TAllocLevel vLevel=blChunkMemory,
		CdBufStream* BufFilter=NULL);
	COREARRAY_DLL_DEFAULT void InitAllocatorEx(TdAllocator &Allocator,
		bool CanRead, bool CanWrite, CdStream* Stream);
	COREARRAY_DLL_DEFAULT void DoneAllocator(TdAllocator &Allocator);
	COREARRAY_DLL_DEFAULT void InitMemAllocator(TdAllocator &Allocator,
		const SIZE64 Size = 0);
	COREARRAY_DLL_DEFAULT void SwitchAllocator(TdAllocator &Allocator,
		bool CanRead, bool CanWrite, const TAllocLevel NewLevel,
		CdBufStream* BufFilter=NULL);
	COREARRAY_DLL_DEFAULT void LoadAllocator(TdAllocator &Allocator,
		CdStream* Source, SIZE64 Start, SIZE64 Len);
	COREARRAY_DLL_DEFAULT void SaveAllocator(TdAllocator &Allocator,
		CdStream* Dest, SIZE64 Start, SIZE64 Len);
	COREARRAY_DLL_DEFAULT void LoadAllocator(TdAllocator &Allocator,
		CdBufStream* Source, SIZE64 Start, SIZE64 Len);
	COREARRAY_DLL_DEFAULT void SaveAllocator(TdAllocator &Allocator,
		CdBufStream* Dest, SIZE64 Start, SIZE64 Len);










	// ******************************************************************** //
	// ******************************************************************** //


	/// Define the size of buffer, when saving, loading, or copying
	const size_t COREARRAY_STREAM_BUFFER	= 0x10000;


	/// Stream with a handle
	class COREARRAY_DLL_DEFAULT CdHandleStream: public CdStream
	{
	public:
		CdHandleStream();
		CdHandleStream(TSysHandle AHandle);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(void *const Buffer, ssize_t Count);
		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(const SIZE64 NewSize);

		COREARRAY_INLINE TSysHandle Handle() const { return fHandle; }

	protected:
		TSysHandle fHandle;
	};


	/// File stream
	class COREARRAY_DLL_DEFAULT CdFileStream: public CdHandleStream
	{
	public:
		enum TdOpenMode {
			fmCreate = 0,
			fmOpenRead,
			fmOpenWrite,
			fmOpenReadWrite
		};

		CdFileStream(const char * const AFileName, TdOpenMode mode);
		virtual ~CdFileStream();

		COREARRAY_INLINE const string& FileName() const { return fFileName; }
		COREARRAY_INLINE TdOpenMode Model() const { return fMode; }

	protected:
		string fFileName;
		TdOpenMode fMode;

		CdFileStream(): CdHandleStream() {}
		void Init(const char * const AFileName, TdOpenMode Mode);
	};


	/// File stream for forked processes
	class COREARRAY_DLL_DEFAULT CdForkFileStream: public CdFileStream
	{
	public:
		CdForkFileStream(const char * const AFileName, TdOpenMode Mode);

		/// Read block of data, and return number of read in bytes
		virtual ssize_t Read(void *Buffer, ssize_t Count);
		/// Write block of data, and return number of write in bytes
		virtual ssize_t Write(void *const Buffer, ssize_t Count);

		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(const SIZE64 NewSize);

	protected:
	#ifdef COREARRAY_PLATFORM_UNIX
		pid_t Current_PID;
	#endif

	private:
		COREARRAY_INLINE void RedirectFile();
	};


	/// Temporary stream, in which a temporary file is created
	class COREARRAY_DLL_DEFAULT CdTempStream: public CdFileStream
	{
	public:
		CdTempStream(const char * const Path);
		virtual ~CdTempStream();
	};


	/// Memory stream
	class COREARRAY_DLL_DEFAULT CdMemoryStream: public CdStream
	{
	public:
		CdMemoryStream(size_t Size = 0);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(void *const Buffer, ssize_t Count);
		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);

		virtual SIZE64 GetSize();
		virtual void SetSize(const SIZE64 NewSize);

        void *BufPointer();
	protected:
		TdAllocator fAllocator;
		ssize_t fPosition;
	};


	#ifndef COREARRAY_NO_STD_IN_OUT

	/// Stream for standard input
	class COREARRAY_DLL_DEFAULT CdStdInStream: public CdStream
	{
	public:
		CdStdInStream();
		virtual ~CdStdInStream();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(void *const Buffer, ssize_t Count);
		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);

		virtual SIZE64 GetSize();
		virtual void SetSize(const SIZE64 NewSize);
	};

	/// Stream for standard output
	class COREARRAY_DLL_DEFAULT CdStdOutStream: public CdStream
	{
	public:
		CdStdOutStream();
		virtual ~CdStdOutStream();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(void *const Buffer, ssize_t Count);
		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);

		virtual SIZE64 GetSize();
		virtual void SetSize(const SIZE64 NewSize);
	};

	#endif


	// TdCompressRemainder

	struct COREARRAY_DLL_DEFAULT TdCompressRemainder
	{
		size_t Size;
		union {
			unsigned char Buf[8];
			C_UInt64 Buf64;
		};

		TdCompressRemainder() { Size = 0; Buf64 = 0; }
	};


	/// The abstract class of ZLIB stream
	/** The wrapper of zlib algorithm (http://www.zlib.net). **/
	class COREARRAY_DLL_DEFAULT CdBaseZStream: public CdStream
	{
	public:
		CdBaseZStream(CdStream* vStream);
		virtual ~CdBaseZStream();

		COREARRAY_INLINE CdStream *Stream() const { return fStream; }
		COREARRAY_INLINE SIZE64 TotalIn() const { return fTotalIn; }
		COREARRAY_INLINE SIZE64 TotalOut() const { return fTotalOut; }

	protected:
		CdStream* fStream;
		SIZE64 fStreamPos, fStreamBase;
		C_Int64 fTotalIn, fTotalOut;
		z_stream fZStream;
		unsigned char fBuffer[65536];
	};


	/// Input stream for zlib
	class COREARRAY_DLL_DEFAULT CdZIPDeflate: public CdBaseZStream
	{
	public:
		enum TZLevel {
			zcNone = 0, zcFastest = 1, zcDefault = 2, zcMax = 3,
			zcLevel1, zcLevel2, zcLevel3, zcLevel4, zcLevel5,
			zcLevel6, zcLevel7, zcLevel8, zcLevel9
		};
		enum TZStrategy { zsDefault, zsFiltered, zsHuffman, zsRLE, zsFixed };

		CdZIPDeflate(CdStream* Dest, TZLevel DeflateLevel);
		CdZIPDeflate(CdStream* Dest, TZLevel DeflateLevel,
			int windowBits, int memLevel, TZStrategy Strategy);
		virtual ~CdZIPDeflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(void *const Buffer, ssize_t Count);
		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(const SIZE64 NewSize);
		void Close();

		ssize_t Pending();
    	COREARRAY_INLINE bool HaveClosed() const { return fHaveClosed; }
		TdCompressRemainder *PtrExtRec;

	protected:
		bool fHaveClosed;

		void SyncFlush(int Code);
	};


	/// Output stream for zlib
	class COREARRAY_DLL_DEFAULT CdZIPInflate: public CdBaseZStream
	{
	public:
		CdZIPInflate(CdStream* Source);
		CdZIPInflate(CdStream* Source, int windowBits);
		virtual ~CdZIPInflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(void *const Buffer, ssize_t Count);
		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(const SIZE64 NewSize);

		void ClearPoints();

		COREARRAY_INLINE bool RandomAccess() const { return fRandomAccess; }
		void SetRandomAccess(bool Value);

		COREARRAY_INLINE ssize_t BlockSize() const { return fBlockSize; }
		void SetBlockSize(ssize_t Value);

	protected:
		ssize_t fBlockSize;
		bool fRandomAccess;
		SIZE64 fBlockStart, fCurPos;

		struct TZIPPointRec { SIZE64 SourcePos; z_stream Rec; };
		vector<TZIPPointRec> vPoints;

		TZIPPointRec *AddPoint();
		TZIPPointRec *PointIndex(unsigned int i);
		TZIPPointRec *PointIndexEx(unsigned int i);
	};


	/// Exception for ZIP stream
	class COREARRAY_DLL_EXPORT EZLibError: public ErrStream
	{
	public:
		EZLibError(int Code);
		EZLibError(const char *fmt, ...): ErrStream()
			{ fErrCode = -1; _COREARRAY_ERRMACRO_(fmt); }
		int ErrCode() const { return fErrCode; };

	protected:
		int fErrCode;
	};


	// TdBlockCollection, TdBlockStream

	/// Type of block ID, used in TdBlockCollection and TdBlockStream
	typedef TdNumber<C_UInt32, sizeof(C_UInt32)> TdBlockID;

	/// an operator, to read TdBlockID from a stream
	COREARRAY_INLINE COREARRAY_DLL_DEFAULT
	CdStream& operator>> (CdStream &s, TdBlockID& out)
		{ out = s.rUInt32(); return s; }
	/// an operator, to write TdBlockID to a stream
	COREARRAY_INLINE COREARRAY_DLL_DEFAULT
	CdStream& operator<< (CdStream &s, const TdBlockID &in)
		{ s.wUInt32(in); return s; }

	/// an operator, to read a TdBlockID from a buffer
	COREARRAY_INLINE COREARRAY_DLL_DEFAULT
	bool operator>> (CdSerial::TdVar &s, TdBlockID& out)
		{ return (s >> out.get()); }
	/// an operator, to write a TdBlockID to a buffer
	COREARRAY_INLINE COREARRAY_DLL_DEFAULT
	void operator<< (CdSerial::TdVar &s, const TdBlockID& in)
		{ s << in.get(); }



	class COREARRAY_DLL_DEFAULT CdBlockCollection;


	extern const C_Int64 GDS_STREAM_POS_MASK;           //< 0x7FFF,FFFFFFFF
	extern const C_Int64 GDS_STREAM_POS_MASK_HEAD_BIT;  //< 0x8000,00000000


	/// The chunk stream in a GDS file
	class COREARRAY_DLL_DEFAULT CdBlockStream: public CdStream
	{
	public:
		friend class CdBlockCollection;

		struct TBlockInfo
		{
		public:
			static const SIZE64 HeadSize = TdBlockID::Size + TdPosType::Size;

			TBlockInfo *Next;
			SIZE64 BlockStart, BlockSize;	// Position in Block
			SIZE64 StreamStart, StreamNext;	// Stream Position
			bool Head;

			TBlockInfo();
			SIZE64 AbsStart();
			void SetSize(CdStream &Stream, const SIZE64 _Size);
			void SetNext(CdStream &Stream, const SIZE64 _Next);
			void SetSize2(CdStream &Stream, const SIZE64 _Size, const SIZE64 _Next);
		};

		CdBlockStream(CdBlockCollection &vCollection);
		virtual ~CdBlockStream();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(void *const Buffer, ssize_t Count);
		virtual SIZE64 Seek(const SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(const SIZE64 NewSize);
        void SetSizeOnly(const SIZE64 NewSize);

		void SyncSizeInfo();

		bool ReadOnly() const;
		int ListCount() const;

		COREARRAY_INLINE TdBlockID ID() const { return fID; }
		COREARRAY_INLINE SIZE64 Capacity() const { return fBlockCapacity; }
		COREARRAY_INLINE SIZE64 Size() const { return fBlockSize; }
		COREARRAY_INLINE CdBlockCollection &Collection() const { return fCollection; }
		COREARRAY_INLINE const TBlockInfo *List() const { return fList; }

	protected:
		CdBlockCollection &fCollection;
		TdBlockID fID;
		TBlockInfo *fList, *fCurrent;
		SIZE64 fPosition, fBlockCapacity;
		TdPosType fBlockSize;

	private:
    	bool fNeedSyncSize;
		TBlockInfo *_FindCur(const SIZE64 Pos);
	};

	/// The pointer to the chunk stream
	typedef CdBlockStream* PdBlockStream;
	/// The pointer to the block info in the chunk stream
	typedef CdBlockStream::TBlockInfo* PdBlockStream_BlockInfo;


	/// a collection of stream block
	class COREARRAY_DLL_DEFAULT CdBlockCollection
	{
	public:
		friend class CdBlockStream;

		CdBlockCollection(const SIZE64 vCodeStart=0);
		virtual ~CdBlockCollection();

		void LoadStream(CdStream *vStream, bool vReadOnly);
		void WriteStream(CdStream *vStream);
		void Clear();

    	CdBlockStream *NewBlockStream();
    	void DeleteBlockStream(TdBlockID id);
		CdBlockStream *operator[] (const TdBlockID &id); // always return an object
		bool HaveID(TdBlockID id);

		int NumOfFragment();

		COREARRAY_INLINE CdStream *Stream() const
			{ return fStream; }
		COREARRAY_INLINE CdObjClassMgr *ClassMgr() const
			{ return fClassMgr; }
		COREARRAY_INLINE bool ReadOnly() const
			{ return fReadOnly; }
		COREARRAY_INLINE const vector<CdBlockStream*> &BlockList() const
			{ return fBlockList; }
		COREARRAY_INLINE PdBlockStream_BlockInfo const UnusedBlock() const
        	{ return fUnuse; }

	protected:
		CdStream *fStream;
		SIZE64 fStreamSize;
		PdBlockStream_BlockInfo fUnuse;
		vector<CdBlockStream*> fBlockList;
		SIZE64 fCodeStart;
		CdObjClassMgr *fClassMgr;
		bool fReadOnly;

		void _IncStreamSize(CdBlockStream &Block, const SIZE64 NewSize);
		void _DecStreamSize(CdBlockStream &Block, const SIZE64 NewSize);
		PdBlockStream_BlockInfo _NeedBlock(SIZE64 Size, bool Head);
	private:
		TdBlockID vNextID;
	};
}

#endif /* _HEADER_COREARRAY_STREAM_ */
