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
 *	\file     dStream.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007 - 2017
 *	\brief    Stream classes and functions
 *	\details
**/

#ifndef _HEADER_COREARRAY_STREAM_
#define _HEADER_COREARRAY_STREAM_

#include "dBase.h"
#include "dSerial.h"

// zlib library
#ifdef COREARRAY_USE_ZLIB_EXT
#       include <zlib.h>
#   else
#       include "../ZLIB/zlib.h"
#endif

// LZ4 library
#ifndef COREARRAY_NO_LZ4
#   include "../LZ4/lz4.h"
#   include "../LZ4/lz4hc.h"
#   include "../LZ4/lz4frame.h"
#endif

// lzma library
#ifndef COREARRAY_NO_LZMA
#   ifdef COREARRAY_USE_LZMA_EXT
#       include <lzma.h>
#   else
#       include "../XZ/api/lzma.h"
#   endif
#endif


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
		virtual ~CdMemoryStream();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);

        void *BufPointer();

	protected:
		void *fBuffer;
		ssize_t fCapacity, fPosition;
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
			clUnknown  = -1,   //< unknown or unspecified compression level
			clMin      =  0,   //< minimal data compression, but possibly allow checksum procedure
			clFast     =  1,   //< fast mode when compressing
			clDefault  =  2,   //< default mode with high compression ratio
			clMax      =  3,   //< maximize the compression ratio
			clUltra    =  4,   //< ultra-mode
			clUltraMax =  5,   //< maximize the ultra mode
			clCustom   =  6    //< use a customized level
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

		inline void UpdateStreamPosition();
	};


	/// structure with compression level
	class COREARRAY_DLL_DEFAULT CdRecodeLevel
	{
	public:
		CdRecodeLevel(CdRecodeStream::TLevel level);

		COREARRAY_INLINE CdRecodeStream::TLevel Level() const { return fLevel; }

	protected:
		CdRecodeStream::TLevel fLevel;
	};


	/// Exception for ZIP stream
	class COREARRAY_DLL_EXPORT ErrRecodeStream: public ErrStream
	{
	public:
		ErrRecodeStream(): ErrStream()
			{ }
		ErrRecodeStream(const std::string &msg): ErrStream()
			{ fMessage = msg; }
		ErrRecodeStream(const char *fmt, ...): ErrStream()
			{ _COREARRAY_ERRMACRO_(fmt); }
	};



	// =====================================================================
	// Algorithm of random access
	// =====================================================================

	/// The algorithm of random access on independent compressed blocks
	class COREARRAY_DLL_DEFAULT CdRAAlgorithm
	{
	public:
		/// the size of independent compressed data block, don't try to modify
		enum TBlockSize
		{
			raUnknown = -1,   ///< unknown or unspecified size
			ra16KB    =  0,   ///< 16 KiB
			ra32KB    =  1,   ///< 32 KiB
			ra64KB    =  2,   ///< 64 KiB
			ra128KB   =  3,   ///< 128 KiB
			ra256KB   =  4,   ///< 256 KiB
			ra512KB   =  5,   ///< 512 KiB
			ra1MB     =  6,   ///< 1 MiB
			ra2MB     =  7,   ///< 2 MiB
			ra4MB     =  8,   ///< 4 MiB
			ra8MB     =  9,   ///< 8 MiB, the maximum value in the current format
			raFirst   =  0,   ///< the first valid value
			raLast    =  9,   ///< the last valid value
			raDefault =  4    ///< the default value
		};

		/// constructor
		CdRAAlgorithm(CdRecodeStream &owner);
		/// compression block information
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
		/// constructor
		CdRA_Read(CdRecodeStream *owner);
		/// destructor
		~CdRA_Read();

		/// load all block information
		void GetUpdated();
		/// get block lists
		void GetBlockInfo(vector<SIZE64> &RawSize, vector<SIZE64> &CmpSize);

	protected:
		/// the version number
		C_UInt8 fVersion;
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
		/// the start position of indexing information, ZERO for no indexing
		SIZE64 fIndexingStart;

		/// indexing structure
		struct TIndex {
			SIZE64 RawStart;  ///< the starting position of uncompressed block
			SIZE64 CmpStart;  ///< the starting position of compressed block
		};
		/// indexing for fast seeking consisting of uncompressed and compressed starts
		TIndex *fIndex;
		/// the available size for the variable fIndex
		ssize_t fIndexSize;

		/// initialize the stream with magic number and others
		void InitReadStream();
		/// seek in the stream, return true to require reset deflate algorithm
		bool SeekStream(SIZE64 Position);
		/// go to the next block
		bool NextBlock();
		/// Binary search in the list of indexing among low..high
		void BinSearch(SIZE64 Position, ssize_t low, ssize_t high);
		/// read the magic number on Stream, return true if succeeds
		virtual bool ReadMagicNumber(CdStream &Stream) = 0;
		/// load the indexing information for version 0x11
		void LoadIndexing();

	private:
		/// get the header of block used in Version_1.0
		inline void GetBlockHeader_v1_0();
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
		/// the version number, 0x11 by default
		C_UInt8 fVersion;
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
		/// save block info in version 0x11
		vector<C_UInt64> fBlockInfoList;
		/// add indexing info to fBlockInfoList
		inline void AddBlockInfo(C_UInt32 CmpLen, C_UInt32 RawLen);

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
	class COREARRAY_DLL_DEFAULT CdZEncoder:
		public CdBaseZStream, public CdRecodeLevel
	{
	public:

	#ifndef Z_DEFAULT_MEMORY
	#   define Z_DEFAULT_MEMORY   8
	#endif

		CdZEncoder(CdStream &Dest, TLevel Level);
		CdZEncoder(CdStream &Dest, TLevel Level, int windowBits,
			int memLevel=Z_DEFAULT_MEMORY, int Strategy=Z_DEFAULT_STRATEGY);
		virtual ~CdZEncoder();

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
	class COREARRAY_DLL_DEFAULT CdZDecoder: public CdBaseZStream
	{
	public:
		CdZDecoder(CdStream &Source);
		CdZDecoder(CdStream &Source, int windowBits);
		virtual ~CdZDecoder();

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
	class COREARRAY_DLL_DEFAULT CdZEncoder_RA:
		protected CdRA_Write, public CdZEncoder
	{
	public:
		CdZEncoder_RA(CdStream &Dest, TLevel Level, TBlockSize BlockSize);

		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual void Close();

		/// Copy from a CdStream object
		/** \param Source  a stream object
		 *  \param Pos     the starting position
		 *  \param Count   the number of bytes, -1 for all data starting from Pos
		**/
		virtual void CopyFrom(CdStream &Source, SIZE64 Pos, SIZE64 Count);

	protected:
		ssize_t fBufferSize;
		ssize_t fBlockZIPSize, fCurBlockZIPSize;

		/// write the magic number
		virtual void WriteMagicNumber(CdStream &Stream);
		/// finish and close a ZIP compressed block
		void SyncFinishBlock();
	};


	/// Output stream for zlib with the support of random access
	class COREARRAY_DLL_DEFAULT CdZDecoder_RA:
		public CdRA_Read, public CdZDecoder
	{
	public:
		friend class CdZEncoder_RA;

		CdZDecoder_RA(CdStream &Source);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

	protected:
		/// read the magic number on Stream
		virtual bool ReadMagicNumber(CdStream &Stream);
		/// reset the variables internally
		void Reset();
	};


	/// Exception for ZIP stream
	class COREARRAY_DLL_EXPORT EZLibError: public ErrRecodeStream
	{
	public:
		EZLibError(int Code);
		EZLibError(const char *fmt, ...): ErrRecodeStream()
			{ fErrCode = -1; _COREARRAY_ERRMACRO_(fmt); }
		int ErrCode() const { return fErrCode; };

	protected:
		int fErrCode;
	};



	// =====================================================================
	// The classes of LZ4 stream
	// =====================================================================

#ifndef COREARRAY_NO_LZ4

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
	class COREARRAY_DLL_DEFAULT CdLZ4Encoder:
		public CdBaseLZ4Stream, public CdRecodeLevel
	{
	public:
		CdLZ4Encoder(CdStream &Dest, CdRecodeStream::TLevel level,
			TLZ4Chunk chunk);
		virtual ~CdLZ4Encoder();

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
		TLZ4Chunk fChunk;
		LZ4F_preferences_t lz4_pref;
		LZ4F_compressionContext_t lz4_context;
		C_UInt8 *fCompress;
		ssize_t fChunkUsed;
		ssize_t fChunkSize, fRawChunkSize;
		bool fHaveClosed;
	};

	/// Decompression of LZ4 algorithm
	class COREARRAY_DLL_DEFAULT CdLZ4Decoder: public CdBaseLZ4Stream
	{
	public:
		CdLZ4Decoder(CdStream &Source);
		virtual ~CdLZ4Decoder();

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
	class COREARRAY_DLL_DEFAULT CdLZ4Encoder_RA:
		protected CdRA_Write, public CdBaseLZ4Stream, public CdRecodeLevel
	{
	public:
		CdLZ4Encoder_RA(CdStream &Dest, TLevel Level,
			TBlockSize BlockSize);
		virtual ~CdLZ4Encoder_RA();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(SIZE64 NewSize);
		virtual void Close();

		/// Copy from a CdStream object
		/** \param Source  a stream object
		 *  \param Pos     the starting position
		 *  \param Count   the number of bytes, -1 for all data starting from Pos
		**/
		virtual void CopyFrom(CdStream &Source, SIZE64 Pos, SIZE64 Count);

		ssize_t Pending() { return 0; }
		COREARRAY_INLINE bool HaveClosed() const { return fHaveClosed; }
		COREARRAY_INLINE CdRecodeStream::TLevel Level() const { return fLevel; }

		TdCompressRemainder *PtrExtRec;

	protected:
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
	class COREARRAY_DLL_DEFAULT CdLZ4Decoder_RA:
		public CdRA_Read, public CdBaseLZ4Stream
	{
	public:
		friend class CdLZ4Encoder_RA;

		CdLZ4Decoder_RA(CdStream &Source);

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
		virtual bool ReadMagicNumber(CdStream &Stream);
		/// reset the variables internally
		void Reset();
	};


	/// Exception for LZ4 stream
	class COREARRAY_DLL_EXPORT ELZ4Error: public ErrRecodeStream
	{
	public:
		ELZ4Error(): ErrRecodeStream()
			{ }
		ELZ4Error(const char *fmt, ...): ErrRecodeStream()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ELZ4Error(const std::string &msg): ErrRecodeStream()
			{ fMessage = msg; }
		ELZ4Error(LZ4F_errorCode_t err)
			{ fMessage = LZ4F_getErrorName(err); }
	};

#endif


	// =====================================================================
	// The classes of xz/lzma stream
	// =====================================================================

#ifndef COREARRAY_NO_LZMA

	/** The wrapper of lzma algorithm **/
	class COREARRAY_DLL_DEFAULT CdBaseXZStream: public CdRecodeStream
	{
	public:
		CdBaseXZStream(CdStream &vStream);

	protected:
		lzma_stream fXZStream;
	};


	/// Input stream for xz/lzma
	class COREARRAY_DLL_DEFAULT CdXZEncoder:
		public CdBaseXZStream, public CdRecodeLevel
	{
	public:
		CdXZEncoder(CdStream &Dest, TLevel Level);
		CdXZEncoder(CdStream &Dest, int DictKB);
		virtual ~CdXZEncoder();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(SIZE64 NewSize);
		virtual void Close();

		ssize_t Pending();
    	COREARRAY_INLINE bool HaveClosed() const { return fHaveClosed; }
		TdCompressRemainder *PtrExtRec;

	protected:
		bool fHaveClosed;
		void SyncFinish();
		void InitXZStream();
	};


	/// Output stream for xz/lzma
	class COREARRAY_DLL_DEFAULT CdXZDecoder: public CdBaseXZStream
	{
	public:
		CdXZDecoder(CdStream &Source);
		virtual ~CdXZDecoder();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual SIZE64 GetSize();
		virtual void SetSize(SIZE64 NewSize);

	protected:
		C_UInt8 fBuffer[16384];  // 2^14, 16K
		SIZE64 fCurPosition;
	};


	/// Input stream for xz/lzma with the support of random access
	class COREARRAY_DLL_DEFAULT CdXZEncoder_RA:
		protected CdRA_Write, public CdXZEncoder
	{
	public:
		CdXZEncoder_RA(CdStream &Dest, TLevel Level, TBlockSize BlockSize);

		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual void Close();

		/// Copy from a CdStream object
		/** \param Source  a stream object
		 *  \param Pos     the starting position
		 *  \param Count   the number of bytes, -1 for all data starting from Pos
		**/
		virtual void CopyFrom(CdStream &Source, SIZE64 Pos, SIZE64 Count);

	protected:
		ssize_t fBufferSize;
		ssize_t fBlockZIPSize, fCurBlockZIPSize;

		/// write the magic number
		virtual void WriteMagicNumber(CdStream &Stream);
		/// finish and close a ZIP compressed block
		void SyncFinishBlock();
	};


	/// Output stream for xz/lzma with the support of random access
	class COREARRAY_DLL_DEFAULT CdXZDecoder_RA:
		public CdRA_Read, public CdXZDecoder
	{
	public:
		friend class CdXZEncoder_RA;

		CdXZDecoder_RA(CdStream &Source);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

	protected:
		/// read the magic number on Stream
		virtual bool ReadMagicNumber(CdStream &Stream);
		/// reset the variables internally
		void Reset();
	};


	/// Exception for XZ stream
	class COREARRAY_DLL_EXPORT EXZError: public ErrRecodeStream
	{
	public:
		EXZError(): ErrRecodeStream()
			{ }
		EXZError(const char *fmt, ...): ErrRecodeStream()
			{ _COREARRAY_ERRMACRO_(fmt); }
		EXZError(const std::string &msg): ErrRecodeStream()
			{ fMessage = msg; }
		EXZError(int code): ErrRecodeStream()
			{ fMessage = Format("xz stream error: %d", code); }
	};

#endif



	// =====================================================================
	// GDS block stream
	// =====================================================================

	class COREARRAY_DLL_DEFAULT CdBlockCollection;

	/// 0x7FFF,FFFF,FFFF
	extern const C_Int64 GDS_STREAM_POS_MASK;
	/// 0x8000,0000,0000
	extern const C_Int64 GDS_STREAM_POS_MASK_HEAD_BIT;


	/// The chunk stream in a GDS file
	class COREARRAY_DLL_DEFAULT CdBlockStream: public CdStream
	{
	public:
		friend class CdBlockCollection;

		struct TBlockInfo
		{
		public:
			static const SIZE64 HEAD_SIZE = GDS_BLOCK_ID_SIZE + GDS_POS_SIZE;

			TBlockInfo *Next;
			SIZE64 BlockStart, BlockSize;	// Position in Block
			SIZE64 StreamStart, StreamNext;	// Stream Position
			bool Head;

			TBlockInfo();
			SIZE64 AbsStart() const;  /// the start position with overhead
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
		SIZE64 GetSize() const;

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

		void LoadStream(CdStream *vStream, bool vReadOnly, bool vAllowError,
			CdLogRecord *Log);
		void WriteStream(CdStream *vStream);
		void Clear();

    	CdBlockStream *NewBlockStream();
		/// remove the stream object associated with ID
    	void DeleteBlockStream(TdGDSBlockID id);

		/// get a stream object from ID, always return an object
		CdBlockStream *operator[] (const TdGDSBlockID &id);
		/// return true if ID exists
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
		COREARRAY_INLINE const CdBlockStream::TBlockInfo* UnusedBlock() const
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
