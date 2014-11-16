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

#include "dBase.h"
#include "dSerial.h"

// zlib library, need 'deflatePending()' in zlib, requiring >= v1.2.5.1"
#include "ZLIB/zlib.h"

// LZ4 library
#include "LZ4/lz4.h"
#include "LZ4/lz4hc.h"
#include "LZ4/lz4frame.h"

#include <cstring>
#include <vector>

#ifdef COREARRAY_PLATFORM_UNIX
#  include <sys/types.h>
#  include <unistd.h>
#endif


namespace CoreArray
{
	using namespace std;


	/// Stream with a handle
	class COREARRAY_DLL_DEFAULT CdHandleStream: public CdStream
	{
	public:
		CdHandleStream();
		CdHandleStream(TSysHandle AHandle);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(SIZE64 NewSize);

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
		virtual ssize_t Write(const void *Buffer, ssize_t Count);

		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);

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
		CdTempStream();
		CdTempStream(const char *Path);
		virtual ~CdTempStream();
	};


	/// Memory stream
	class COREARRAY_DLL_DEFAULT CdMemoryStream: public CdStream
	{
	public:
		CdMemoryStream(size_t Size = 0);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);

        void *BufPointer();

	protected:
		ssize_t fPosition;
	};



	// =====================================================================
	// Standard input and output
	// =====================================================================

	#ifndef COREARRAY_NO_STD_IN_OUT

	/// Stream for standard input
	class COREARRAY_DLL_DEFAULT CdStdInStream: public CdStream
	{
	public:
		CdStdInStream();
		virtual ~CdStdInStream();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);
	};

	/// Stream for standard output
	class COREARRAY_DLL_DEFAULT CdStdOutStream: public CdStream
	{
	public:
		CdStdOutStream();
		virtual ~CdStdOutStream();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);
	};

	#endif



	// =====================================================================
	// The classes of transformed stream
	// =====================================================================

	/// TdCompressRemainder
	struct COREARRAY_DLL_DEFAULT TdCompressRemainder
	{
		size_t Size;
		union {
			C_UInt8 Buf[8];
			C_UInt64 Buf64;
		};

		TdCompressRemainder() { Size = 0; Buf64 = 0; }
	};

	class COREARRAY_DLL_DEFAULT CdRecodeStream: public CdStream
	{
	public:
		friend class CdRAAlgorithm;
		friend class CdRA_Read;
		friend class CdRA_Write;

		/// compression level
		enum TLevel
		{
			clUnknown = -1,   //< unknown or unspecified compression level
			clNone    =  0,   //< no data compression, but possibly allow checksum procedure
			clFast    =  1,   //< fast mode when compressing
			clDefault =  2,   //< default mode with high compression ratio
			clMax     =  3,   //< maximize the compression ratio
			clFirst   =  0,   //< the first valid value
			clLast    =  3    //< the last valid value
		};

		CdRecodeStream(CdStream &vStream);
		virtual ~CdRecodeStream();

		COREARRAY_INLINE CdStream &Stream() const { return *fStream; }
		COREARRAY_INLINE SIZE64 TotalIn() const { return fTotalIn; }
		COREARRAY_INLINE SIZE64 TotalOut() const { return fTotalOut; }

	protected:
		CdStream *fStream;
		SIZE64 fStreamPos, fStreamBase;
		SIZE64 fTotalIn, fTotalOut;
	};



	// =====================================================================
	// Algorithm of random access
	// =====================================================================

	/// The algorithm of random access on independent compressed blocks
	class COREARRAY_DLL_DEFAULT CdRAAlgorithm
	{
	public:
		/// the size of independent compressed data block
		enum TBlockSize
		{
			raUnknown = -1,   //< unknown or unspecified size
			ra16KB    =  0,   //< 16 KiB
			ra32KB    =  1,   //< 32 KiB
			ra64KB    =  2,   //< 64 KiB
			ra128KB   =  3,   //< 128 KiB
			ra256KB   =  4,   //< 256 KiB
			ra512KB   =  5,   //< 512 KiB
			ra1MB     =  6,   //< 1 MiB
			ra2MB     =  7,   //< 2 MiB
			ra4MB     =  8,   //< 4 MiB
			ra8MB     =  9,   //< 8 MiB
			raFirst   =  0,   //< the first valid value
			raLast    =  9,   //< the last valid value
			raDefault =  4    //< the default value
		};

		CdRAAlgorithm(CdRecodeStream &owner);
		COREARRAY_INLINE TBlockSize SizeType() const { return fSizeType; }

	protected:
		/// the owner of this object
		CdRecodeStream &fOwner;
		/// the size of independent compressed block
		TBlockSize fSizeType;
	};

	/// The reading algorithm with random access on data stream
	class COREARRAY_DLL_DEFAULT CdRA_Read: public CdRAAlgorithm
	{
	public:
		CdRA_Read(CdRecodeStream *owner);

		/// initialize the stream with magic number and others
		void InitReadStream();
		/// seek in the stream, if return true indicates reset deflate algorithm
		bool SeekStream(SIZE64 Position);

	protected:
		/// the total number of independent compressed block
		C_Int32 fBlockNum;
		/// the current index of independent compressed block
		C_Int32 fBlockIdx;
		/// the starting position of compressed block
		SIZE64 fCB_ZStart;
		/// the size of compressed block
		SIZE64 fCB_ZSize;
		/// the starting position of uncompressed block
		SIZE64 fCB_UZStart;
		/// the size of uncompressed block
		SIZE64 fCB_UZSize;
		/// the start position of block list
		SIZE64 fBlockListStart;

		/// read the magic number on Stream
		virtual void ReadMagicNumber(CdStream &Stream) = 0;
		/// go to the next block
		bool NextBlock();

	private:
		/// get the header of block
		void GetBlockHeader();
	};

	/// The writing algorithm with random access on data stream
	class COREARRAY_DLL_DEFAULT CdRA_Write: public CdRAAlgorithm
	{
	public:
		CdRA_Write(CdRecodeStream *owner, TBlockSize bs);

		/// initialize the stream with magic number and others
		void InitWriteStream();
		/// finalize the stream
		void DoneWriteStream();

		/// initialize a compressed block
		void InitWriteBlock();
		/// finalize a compressed block
		void DoneWriteBlock();

	protected:
		/// the total number of independent compressed block
		C_Int32 fBlockNum;
		/// the starting position of compressed block
		SIZE64 fCB_ZStart;
		/// the starting position of uncompressed block
		SIZE64 fCB_UZStart;
		/// the start position of block list
		SIZE64 fBlockListStart;
		/// whether a block is initialized
		bool fHasInitWriteBlock;

		/// write the magic number on Stream
		virtual void WriteMagicNumber(CdStream &Stream) = 0;
	};



	// =====================================================================
	// The classes of ZLIB stream
	// =====================================================================

	/** The wrapper of zlib algorithm (http://www.zlib.net). **/
	class COREARRAY_DLL_DEFAULT CdBaseZStream: public CdRecodeStream
	{
	public:
		CdBaseZStream(CdStream &vStream);

	protected:
		z_stream fZStream;
	};


	/// Input stream for zlib
	class COREARRAY_DLL_DEFAULT CdZDeflate: public CdBaseZStream
	{
	public:

	#ifndef Z_DEFAULT_MEMORY
	#   define Z_DEFAULT_MEMORY   8
	#endif

		CdZDeflate(CdStream &Dest, TLevel Level);
		CdZDeflate(CdStream &Dest, TLevel Level, int windowBits,
			int memLevel=Z_DEFAULT_MEMORY, int Strategy=Z_DEFAULT_STRATEGY);
		virtual ~CdZDeflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(SIZE64 NewSize);
		virtual void Close();

		ssize_t Pending();
    	COREARRAY_INLINE bool HaveClosed() const { return fHaveClosed; }
		TdCompressRemainder *PtrExtRec;

	protected:
		C_UInt8 fBuffer[8192]; // Z deflate buffer, don't modify
		bool fHaveClosed;

		/// Write Z_FINISH to the ZIP compressed stream
		void SyncFinish();
	};

	/// Output stream for zlib
	class COREARRAY_DLL_DEFAULT CdZInflate: public CdBaseZStream
	{
	public:
		CdZInflate(CdStream &Source);
		CdZInflate(CdStream &Source, int windowBits);
		virtual ~CdZInflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);

	protected:
		C_UInt8 fBuffer[16384];  // 2^14, 16K
		SIZE64 fCurPosition;
	};



	/// Input stream for zlib with the support of random access
	class COREARRAY_DLL_DEFAULT CdZRA_Deflate:
		protected CdRA_Write, public CdZDeflate
	{
	public:
		CdZRA_Deflate(CdStream &Dest, TLevel Level,
			TBlockSize BlockSize);

		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual void Close();

	protected:
		ssize_t fBufferSize;
		ssize_t fBlockZIPSize, fCurBlockZIPSize;

		/// write the magic number
		virtual void WriteMagicNumber(CdStream &Stream);
		/// finish and close a ZIP compressed block
		void SyncFinishBlock();
	};


	/// Output stream for zlib with the support of random access
	class COREARRAY_DLL_DEFAULT CdZRA_Inflate:
		protected CdRA_Read, public CdZInflate
	{
	public:
		CdZRA_Inflate(CdStream &Source);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

	protected:
		/// read the magic number on Stream
		virtual void ReadMagicNumber(CdStream &Stream);
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



	// =====================================================================
	// The classes of LZ4 stream
	// =====================================================================

	/**
	 *  The wrapper of LZ4 algorithm (http://code.google.com/p/lz4),
	 *  real-time data compression/decompression.
	**/
	class COREARRAY_DLL_DEFAULT CdBaseLZ4Stream: public CdRecodeStream
	{
	public:
		/// to specify the chunk size
		enum TLZ4Chunk
		{
			bsUnknown = -1,
			bs64KB    =  0,   //< chunk size = 64KB
			bs256KB   =  1,   //< chunk size = 256KB
			bs1MB     =  2,   //< chunk size = 1MB
			bs4MB     =  3,   //< chunk size = 4MB
			bsDefault =  1,   //< the default chunk size (256K)
			bsFirst   =  0,
			bsLast    =  3
		};

		CdBaseLZ4Stream(CdStream &vStream);
	};

	/// Compression of LZ4 algorithm
	class COREARRAY_DLL_DEFAULT CdLZ4Deflate: public CdBaseLZ4Stream
	{
	public:
		CdLZ4Deflate(CdStream &Dest, CdRecodeStream::TLevel level,
			TLZ4Chunk chunk);
		virtual ~CdLZ4Deflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(SIZE64 NewSize);
		void Close();
		int Pending() { return 0; }

		COREARRAY_INLINE TLZ4Chunk Chunk() const { return fChunk; }
		COREARRAY_INLINE ssize_t ChunkSize() const { return fChunkSize; }
		COREARRAY_INLINE CdRecodeStream::TLevel Level() const { return fLevel; }
		COREARRAY_INLINE bool HaveClosed() const { return fHaveClosed; }
		TdCompressRemainder *PtrExtRec;

	protected:
		CdRecodeStream::TLevel fLevel;
		TLZ4Chunk fChunk;
		LZ4F_preferences_t lz4_pref;
		LZ4F_compressionContext_t lz4_context;
		C_UInt8 *fCompress;
		ssize_t fChunkUsed;
		ssize_t fChunkSize, fRawChunkSize;
		bool fHaveClosed;
	};

	/// Decompression of LZ4 algorithm
	class COREARRAY_DLL_DEFAULT CdLZ4Inflate: public CdBaseLZ4Stream
	{
	public:
		CdLZ4Inflate(CdStream &Source);
		virtual ~CdLZ4Inflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);

	protected:
		LZ4F_decompressionContext_t lz4_context;
		SIZE64 fCurPosition;
		C_UInt8 fBuffer[16384];  // 2^14, 16K
		C_UInt8 *fBufPtr, *fBufEnd;
	};


	/// make sure LZ4_COMPRESSBOUND(*) < 64KiB
	#define LZ4RA_RAW_BUFFER_SIZE    0xFE00
	#define LZ4RA_LZ4_BUFFER_SIZE    LZ4_COMPRESSBOUND(LZ4RA_RAW_BUFFER_SIZE)

	/// Input stream for zlib with the support of random access
	class COREARRAY_DLL_DEFAULT CdLZ4RA_Deflate:
		protected CdRA_Write, public CdBaseLZ4Stream
	{
	public:
		CdLZ4RA_Deflate(CdStream &Dest, TLevel Level,
			TBlockSize BlockSize);
		virtual ~CdLZ4RA_Deflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(SIZE64 NewSize);
		virtual void Close();

		ssize_t Pending() { return 0; }
		COREARRAY_INLINE bool HaveClosed() const { return fHaveClosed; }
		COREARRAY_INLINE CdRecodeStream::TLevel Level() const { return fLevel; }

		TdCompressRemainder *PtrExtRec;

	protected:
		/// the compression level
		CdRecodeStream::TLevel fLevel;
		/// LZ4 algorithm pointer
		void *fLZ4Ptr;
		/// the buffer for uncompressed data, since LZ4_compress_continue needs the last block
		char fRawBuffer[2][LZ4RA_RAW_BUFFER_SIZE];
		/// indicator for double buffer, 0: fRawBuffer[0], 1: fRawBuffer[1]
		int _IdxRaw;
		/// the unused size of the raw buffer
		ssize_t fUnusedRawSize;

		bool fHaveClosed;
		ssize_t fBlockLZ4Size, fCurBlockLZ4Size;

		/// write the magic number
		virtual void WriteMagicNumber(CdStream &Stream);
		/// compressing
		void Compressing(int bufsize);
	};

	/// Output stream for LZ4 with the support of random access
	class COREARRAY_DLL_DEFAULT CdLZ4RA_Inflate:
		protected CdRA_Read, public CdBaseLZ4Stream
	{
	public:
		CdLZ4RA_Inflate(CdStream &Source);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);

		COREARRAY_INLINE CdRecodeStream::TLevel Level() const { return fLevel; }

	protected:
		/// the compression level
		CdRecodeStream::TLevel fLevel;
		/// the decompression algorithm
		LZ4_streamDecode_t lz4_body;
		/// the buffer for uncompressed data, since LZ4_decompress_safe_continue needs the last block
		char fRawBuffer[2][LZ4RA_RAW_BUFFER_SIZE];
		/// indicator for double buffer, 0: fRawBuffer[0], 1: fRawBuffer[1]
		int _IdxRaw;
		/// the current position
		SIZE64 fCurPosition;
		/// index with in the raw buffer
		ssize_t iRaw, CntRaw;

		/// read the magic number on Stream
		virtual void ReadMagicNumber(CdStream &Stream);
	};


	/// Exception for LZ4 stream
	class COREARRAY_DLL_EXPORT ELZ4Error: public ErrStream
	{
	public:
		ELZ4Error(): ErrStream()
			{ }
		ELZ4Error(const char *fmt, ...): ErrStream()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ELZ4Error(const std::string &msg): ErrStream()
			{ fMessage = msg; }
		ELZ4Error(LZ4F_errorCode_t err)
			{ fMessage = LZ4F_getErrorName(err); }
	};



	// =====================================================================
	// GDS block stream
	// =====================================================================

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
			static const SIZE64 HeadSize = GDS_POS_SIZE + GDS_BLOCK_ID_SIZE;

			TBlockInfo *Next;
			SIZE64 BlockStart, BlockSize;	// Position in Block
			SIZE64 StreamStart, StreamNext;	// Stream Position
			bool Head;

			TBlockInfo();
			SIZE64 AbsStart();
			void SetSize(CdStream &Stream, SIZE64 _Size);
			void SetNext(CdStream &Stream, SIZE64 _Next);
			void SetSize2(CdStream &Stream, SIZE64 _Size, SIZE64 _Next);
		};

		CdBlockStream(CdBlockCollection &vCollection);
		virtual ~CdBlockStream();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);
        void SetSizeOnly(SIZE64 NewSize);

		void SyncSizeInfo();

		bool ReadOnly() const;
		int ListCount() const;

		COREARRAY_INLINE TdGDSBlockID ID() const { return fID; }
		COREARRAY_INLINE SIZE64 Capacity() const { return fBlockCapacity; }
		COREARRAY_INLINE SIZE64 Size() const { return fBlockSize; }
		COREARRAY_INLINE CdBlockCollection &Collection() const { return fCollection; }
		COREARRAY_INLINE const TBlockInfo *List() const { return fList; }

	protected:
		CdBlockCollection &fCollection;
		TdGDSBlockID fID;
		TBlockInfo *fList, *fCurrent;
		SIZE64 fPosition, fBlockCapacity;
		TdGDSPos fBlockSize;

	private:
    	bool fNeedSyncSize;
		TBlockInfo *_FindCur(const SIZE64 Pos);
	};

	/// The pointer to the chunk stream
	typedef CdBlockStream* PdBlockStream;
	/// The pointer to the block info in the chunk stream
	typedef CdBlockStream::TBlockInfo* PdBlockStream_BlockInfo;


	/// a collection of stream block
	class COREARRAY_DLL_DEFAULT CdBlockCollection: public CdAbstract
	{
	public:
		friend class CdBlockStream;

		CdBlockCollection(const SIZE64 vCodeStart=0);
		virtual ~CdBlockCollection();

		void LoadStream(CdStream *vStream, bool vReadOnly);
		void WriteStream(CdStream *vStream);
		void Clear();

    	CdBlockStream *NewBlockStream();
    	void DeleteBlockStream(TdGDSBlockID id);
		CdBlockStream *operator[] (const TdGDSBlockID &id); // always return an object
		bool HaveID(TdGDSBlockID id);

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
		TdGDSBlockID vNextID;
	};
}

#endif /* _HEADER_COREARRAY_STREAM_ */
