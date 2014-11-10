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

	class COREARRAY_DLL_DEFAULT CdTransformStream: public CdStream
	{
	public:
		CdTransformStream(CdStream &vStream);
		virtual ~CdTransformStream();

		COREARRAY_INLINE CdStream &Stream() const { return *fStream; }
		COREARRAY_INLINE SIZE64 TotalIn() const { return fTotalIn; }
		COREARRAY_INLINE SIZE64 TotalOut() const { return fTotalOut; }

	protected:
		CdStream *fStream;
		SIZE64 fStreamPos, fStreamBase;
		SIZE64 fTotalIn, fTotalOut;
	};



	// =====================================================================
	// The classes of ZLIB stream
	// =====================================================================

	/** The wrapper of zlib algorithm (http://www.zlib.net). **/
	class COREARRAY_DLL_DEFAULT CdBaseZStream: public CdTransformStream
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
		enum TZLevel {
			zcNone = 0, zcFastest = 1, zcDefault = 2, zcMax = 3,
			zcLevel1, zcLevel2, zcLevel3, zcLevel4, zcLevel5,
			zcLevel6, zcLevel7, zcLevel8, zcLevel9
		};
		enum TZStrategy {
			zsDefault = 0, zsFiltered, zsHuffman, zsRLE, zsFixed
		};

		CdZDeflate(CdStream &Dest, TZLevel DeflateLevel);
		CdZDeflate(CdStream &Dest, TZLevel DeflateLevel,
			int windowBits, int memLevel, TZStrategy Strategy);
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
	/** Each compressed block is ~16K/32K/64K/128K/256K/512K/1M  **/
	class COREARRAY_DLL_DEFAULT CdZRA_Deflate: public CdZDeflate
	{
	public:
		enum TRABlockSize {
			bs16K = 0, bs32K, bs64K, bs128K, bs256K, bs512K, bs1M
		};
		CdZRA_Deflate(CdStream &Dest, TZLevel DeflateLevel,
			TRABlockSize BK);

		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual void Close();

	protected:
		C_Int32 fNumZBlock;
		bool fInitBlockHeader;
		SIZE64 fZBStart, fUBStart;
		ssize_t fBufferSize;
		ssize_t fBlockZIPSize, fCurBlockZIPSize;

		/// Finish and close a ZIP compressed block
		void SyncFinishBlock();
	};


	/// Output stream for zlib with the support of random access
	class COREARRAY_DLL_DEFAULT CdZRA_Inflate: public CdZInflate
	{
	public:
		CdZRA_Inflate(CdStream &Source);

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);

	protected:
		C_Int32 fNumZBlock;  //< the total number of compressed block
		C_Int32 fIdxZBlock;  //< the current index of compressed block
		SIZE64 fCB_ZStart;   //< the starting position of compressed block
		SIZE64 fCB_ZSize;    //< the size of compressed block
		SIZE64 fCB_UZStart;  //< the starting position of uncompressed block
		SIZE64 fCB_UZSize;   //< the size of uncompressed block

	private:
		COREARRAY_INLINE void _ReadBHeader(SIZE64 &ZSize, SIZE64 &UZSize);
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
	class COREARRAY_DLL_DEFAULT CdBaseLZ4Stream: public CdTransformStream
	{
	public:
		/// to specify the chunk size
		enum TLZ4Chunk
		{
			lzDefSize = 1,   //< the default chunk size (256K)
			lz64KB    = 0,   //< chunk size = 64KB
			lz256KB   = 1,   //< chunk size = 256KB
			lz1MB     = 2,   //< chunk size = 1MB
			lz4MB     = 3    //< chunk size = 4MB
		};

		CdBaseLZ4Stream(CdStream &vStream);
		virtual ~CdBaseLZ4Stream();

		COREARRAY_INLINE TLZ4Chunk Chunk() const { return fChunk; }
		COREARRAY_INLINE ssize_t ChunkSize() const { return fChunkSize; }

	protected:
		C_UInt8 *fUncompress;
		C_UInt8 *fCompress;
		TLZ4Chunk fChunk;
		ssize_t fChunkSize, fRawChunkSize;
	};

	/// Compression of LZ4 algorithm
	class COREARRAY_DLL_DEFAULT CdLZ4Deflate: public CdBaseLZ4Stream
	{
	public:
		CdLZ4Deflate(CdStream &Dest, TLZ4Chunk chunk);
		virtual ~CdLZ4Deflate();

		virtual ssize_t Read(void *Buffer, ssize_t Count);
		virtual ssize_t Write(const void *Buffer, ssize_t Count);
		virtual SIZE64 Seek(SIZE64 Offset, TdSysSeekOrg Origin);
		virtual void SetSize(SIZE64 NewSize);
		void Close();

		COREARRAY_INLINE bool HaveClosed() const { return fHaveClosed; }
		TdCompressRemainder *PtrExtRec;

	protected:
		LZ4F_preferences_t lz4_pref;
		LZ4F_compressionContext_t lz4_context;
		ssize_t fChunkUsed;
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
