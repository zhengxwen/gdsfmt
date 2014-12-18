// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dPlatform.h: Functions for independent platforms
//
// Copyright (C) 2007 - 2015	Xiuwen Zheng
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
 *	\file     dPlatform.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2015
 *	\brief    Functions for independent platforms
 *	\details
**/

#ifndef _HEADER_COREARRAY_PLATFORM_
#define _HEADER_COREARRAY_PLATFORM_

#include "dType.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <climits>
#include <limits>
#include <vector>
#include <queue>

#if defined(COREARRAY_USING_R)
#   include <R_ext/Arith.h>
#endif

#if defined(COREARRAY_PLATFORM_WINDOWS)
#   include <windows.h>
#endif

#ifdef COREARRAY_POSIX_THREAD
#   include <pthread.h>
#endif



namespace CoreArray
{
	using namespace std;


	// =====================================================================
	// Floating point number
	// =====================================================================

	/// NaN
	extern const double NaN;
	/// Positive infinity
	extern const double Infinity;
	/// Negative infinity
	extern const double NegInfinity;

	/// 
	enum TFPClass
	{
		fpFinite = 0,    //< a finite number
		fpPosInf = 1,    //< +INF
		fpNegInf = 2,    //< -INF
		fpNaN    = 3     //< NaN
	};

	COREARRAY_DLL_DEFAULT TFPClass FloatClassify(const float val);
	COREARRAY_DLL_DEFAULT TFPClass FloatClassify(const double val);
	COREARRAY_DLL_DEFAULT TFPClass FloatClassify(const long double val);

	/// whether it is a finite number
	COREARRAY_DLL_DEFAULT bool IsFinite(const float val);
	/// whether it is a finite number
	COREARRAY_DLL_DEFAULT bool IsFinite(const double val);
	/// whether it is a finite number
	COREARRAY_DLL_DEFAULT bool IsFinite(const long double val);

	/// whether it is a NaN
	COREARRAY_DLL_DEFAULT bool IsNaN(const float val);
	/// whether it is a NaN
	COREARRAY_DLL_DEFAULT bool IsNaN(const double val);
	/// whether it is a NaN
	COREARRAY_DLL_DEFAULT bool IsNaN(const long double val);

	/// whether it is positive infinity
	COREARRAY_DLL_DEFAULT bool IsInf(const float val);
	/// whether it is positive infinity
	COREARRAY_DLL_DEFAULT bool IsInf(const double val);
	/// whether it is positive infinity
	COREARRAY_DLL_DEFAULT bool IsInf(const long double val);

	/// whether it is negative infinity
	COREARRAY_DLL_DEFAULT bool IsNegInf(const float val);
	/// whether it is negative infinity
	COREARRAY_DLL_DEFAULT bool IsNegInf(const double val);
	/// whether it is negative infinity
	COREARRAY_DLL_DEFAULT bool IsNegInf(const long double val);

	// whether v1 == v2, considering NaN
	COREARRAY_DLL_DEFAULT bool EqaulFloat(const float v1, const float v2);
	COREARRAY_DLL_DEFAULT bool EqaulFloat(const double v1, const double v2);
	COREARRAY_DLL_DEFAULT bool EqaulFloat(const long double v1, const long double v2);



	// =====================================================================
	// Format a string
	// =====================================================================

	/// format a string
	COREARRAY_DLL_DEFAULT string Format(const char *fmt, ...);
	/// format a string assuming a short string with at most 64 characters
	COREARRAY_DLL_DEFAULT string _FmtNum(const char *fmt, ...);



	// =====================================================================
	// Floating point number <--> string
	// =====================================================================

	/// float --> string
	COREARRAY_DLL_DEFAULT string FloatToStr(const float val);
	/// double --> string
	COREARRAY_DLL_DEFAULT string FloatToStr(const double val);
	/// long double --> string
	COREARRAY_DLL_DEFAULT string FloatToStr(const long double val);
	/// string --> double
	COREARRAY_DLL_DEFAULT double StrToFloat(char const* str);
	/// string --> double
	COREARRAY_DLL_DEFAULT bool StrToFloat(char const* str, double *rv);
	/// string --> double, with a default value if fails
	COREARRAY_DLL_DEFAULT double StrToFloatDef(char const* str,
		const double Default);



	// =====================================================================
	// Integer <--> string
	// =====================================================================

	/// int8 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_Int8 val);
	/// uint8 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_UInt8 val);
	/// int16 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_Int16 val);
	/// uint16 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_UInt16 val);
	/// int32 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_Int32 val);
	/// uint32 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_UInt32 val);
	/// int64 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_Int64 val);
	/// uint64 --> string
	COREARRAY_DLL_DEFAULT string IntToStr(const C_UInt64 val);
	/// string --> long
	COREARRAY_DLL_DEFAULT long StrToInt(char const* str);
	/// string --> long
	COREARRAY_DLL_DEFAULT bool StrToInt(char const* str, long *rv);
	/// string --> long, with a default value if fails
	COREARRAY_DLL_DEFAULT long StrToIntDef(char const* str, const long Default);



	// =====================================================================
	// String conversion
	// =====================================================================

	/// UTF8 --> UTF16
	COREARRAY_DLL_DEFAULT UTF16String UTF8ToUTF16(const UTF8String &s);
	/// UTF8 --> UTF32
	COREARRAY_DLL_DEFAULT UTF32String UTF8ToUTF32(const UTF8String &s);
	/// UTF16 --> UTF8
	COREARRAY_DLL_DEFAULT UTF8String UTF16ToUTF8(const UTF16String &s);
	/// UTF16 --> UTF32
	COREARRAY_DLL_DEFAULT UTF32String UTF16ToUTF32(const UTF16String &s);
	/// UTF32 --> UTF8
	COREARRAY_DLL_DEFAULT UTF8String UTF32ToUTF8(const UTF32String &s);
	/// UTF32 --> UTF16
	COREARRAY_DLL_DEFAULT UTF16String UTF32ToUTF16(const UTF32String &s);

	/// string --> string
	COREARRAY_DLL_DEFAULT RawString RawText(const string &s);
	/// UTF8 --> string
	// COREARRAY_DLL_DEFAULT RawString RawText(const UTF8String &s);
	/// UTF16 --> string
	COREARRAY_DLL_DEFAULT RawString RawText(const UTF16String &s);
	/// UTF32 --> string
	COREARRAY_DLL_DEFAULT RawString RawText(const UTF32String &s);

	///
	COREARRAY_DLL_DEFAULT UTF8String UTF8Text(const char *s);
	COREARRAY_DLL_DEFAULT UTF8String UTF8Text(const string &s);
	// COREARRAY_DLL_DEFAULT UTF8String UTF8Text(const UTF8String &s);
	COREARRAY_DLL_DEFAULT UTF8String UTF8Text(const UTF16String &s);
	COREARRAY_DLL_DEFAULT UTF8String UTF8Text(const UTF32String &s);
	
	///
	COREARRAY_DLL_DEFAULT UTF16String UTF16Text(const char *s);
	COREARRAY_DLL_DEFAULT UTF16String UTF16Text(const string &s);
	// COREARRAY_DLL_DEFAULT UTF16String UTF16Text(const UTF8String &s);
	COREARRAY_DLL_DEFAULT UTF16String UTF16Text(const UTF16String &s);
	COREARRAY_DLL_DEFAULT UTF16String UTF16Text(const UTF32String &s);

	///
	COREARRAY_DLL_DEFAULT UTF32String UTF32Text(const char *s);
	COREARRAY_DLL_DEFAULT UTF32String UTF32Text(const string &s);
	// COREARRAY_DLL_DEFAULT UTF32String UTF32Text(const UTF8String &s);
	COREARRAY_DLL_DEFAULT UTF32String UTF32Text(const UTF16String &s);
	COREARRAY_DLL_DEFAULT UTF32String UTF32Text(const UTF32String &s);


	/// ASC character --> UTF8
	COREARRAY_DLL_DEFAULT COREARRAY_INLINE UTF8String ASC(const char *s)
		{ return UTF8String(s, s+strlen(s)); }
	/// ASC character --> UTF8
	COREARRAY_DLL_DEFAULT COREARRAY_INLINE UTF8String ASC(const string &s)
		{ return UTF8String(s.begin(), s.end()); }
	/// ASC character --> UTF16
	COREARRAY_DLL_DEFAULT COREARRAY_INLINE UTF16String ASC16(const char *s)
		{ return UTF16String(s, s+strlen(s)); }
	/// ASC character --> UTF16
	COREARRAY_DLL_DEFAULT COREARRAY_INLINE UTF16String ASC16(const string &s)
		{ return UTF16String(s.begin(), s.end()); }
	/// ASC character --> UTF32
	COREARRAY_DLL_DEFAULT COREARRAY_INLINE UTF32String ASC32(const char *s)
		{ return UTF32String(s, s+strlen(s)); }
	/// ASC character --> UTF32
	COREARRAY_DLL_DEFAULT COREARRAY_INLINE UTF32String ASC32(const string &s)
		{ return UTF32String(s.begin(), s.end()); }


	// =====================================================================
	// Time Date --> String
	// =====================================================================

	/// convert the date and time information to a string
	COREARRAY_DLL_DEFAULT string NowDateToStr();



	// =====================================================================
	// String split
	// =====================================================================

	extern const char *const sLineBreak;
	extern const char *const sFileSep;



	// =====================================================================
	// File Functions
	// =====================================================================

	/// 64-bit signed integer, used in stream
	typedef C_Int64 SIZE64;

	#if defined(COREARRAY_PLATFORM_WINDOWS)
		typedef HANDLE TSysHandle;
	#else
		typedef int TSysHandle;
	#endif
	extern const TSysHandle NullSysHandle;

	enum TSysOpenMode { fmRead, fmWrite, fmReadWrite };
	enum TSysShareMode { saNone, saRead, saWrite, saReadWrite };
	enum TdSysSeekOrg { soBeginning=0, soCurrent, soEnd };

	COREARRAY_DLL_DEFAULT TSysHandle SysCreateFile(char const* const AFileName,
		C_UInt32 Mode);
	COREARRAY_DLL_DEFAULT TSysHandle SysOpenFile(char const* const AFileName,
		enum TSysOpenMode mode, enum TSysShareMode smode);

	COREARRAY_DLL_DEFAULT bool SysCloseHandle(TSysHandle Handle);
	COREARRAY_DLL_DEFAULT size_t SysHandleRead(TSysHandle Handle, void *Buffer,
		size_t Count);
	COREARRAY_DLL_DEFAULT size_t SysHandleWrite(TSysHandle Handle,
		const void* Buffer, size_t Count);
	COREARRAY_DLL_DEFAULT C_Int64 SysHandleSeek(TSysHandle Handle,
		C_Int64 Offset, enum TdSysSeekOrg sk);
	COREARRAY_DLL_DEFAULT bool SysHandleSetSize(TSysHandle Handle,
		C_Int64 NewSize);

	/// get a temporary file name
	COREARRAY_DLL_DEFAULT string TempFileName(const char *prefix,
		const char *tempdir);
	/// test if the file exists
	COREARRAY_DLL_DEFAULT bool FileExists(const string &FileName);



	// =====================================================================
	// CoreArray Stream
	// =====================================================================

	/// Stream position in CoreArray GDS format (signature: 0x504F53 = POS)
	typedef TdInteger<C_Int64, 0x504F53> TdGDSPos;

	/// The precision of TdGDSPos: the number of bytes
	static const ssize_t GDS_POS_SIZE = 6;

	/// Block ID in CoreArray GDS format (signature: 0x424944 = BID)
	/** it is used in TdBlockCollection and TdBlockStream  **/
	typedef TdInteger<C_UInt32, 0x424944> TdGDSBlockID;

	/// The precision of TdGDSBlockID: the number of bytes
	static const ssize_t GDS_BLOCK_ID_SIZE = 4;



	// =====================================================================
	// Error message
	// =====================================================================

	/// Return the last code from the OS
	COREARRAY_DLL_DEFAULT int GetLastOSError();
	/// Format a system error message
	COREARRAY_DLL_DEFAULT string SysErrMessage(int err);
	/// Format the last system error message
	COREARRAY_DLL_DEFAULT string LastSysErrMsg();

	/// Raise an exception with the last Operating System error code
	template<class X> COREARRAY_DLL_DEFAULT void RaiseLastOSError()
		{ throw X(LastSysErrMsg()); }



	namespace Mach
	{
		/// Return the number of available (logical) CPU cores in the system
		/** return 0, if unable to determine. **/
		COREARRAY_DLL_DEFAULT int GetCPU_NumOfCores();

		/// Return the size in byte of level-n cache memory
		/** \param level    (=0 for l1i cache, =1 for l1d cache, =2 for l2 cache)
		 *  \return cache size, or 0 if unable to determine.
		**/
		COREARRAY_DLL_DEFAULT C_UInt64 GetCPU_LevelCache(int level);
	}


	// Processes
	
	#if defined(COREARRAY_PLATFORM_UNIX)
		typedef pid_t TProcessID;
	#elif defined(COREARRAY_PLATFORM_WINDOWS)
		typedef DWORD TProcessID;
	#endif

	/// Get the current process id
	COREARRAY_DLL_DEFAULT TProcessID GetCurrentProcessID();



	// Thread structure, classes, functions

	/// The thread mutex object
	struct COREARRAY_DLL_DEFAULT CdThreadMutex
	{
	public:
	#ifdef COREARRAY_PLATFORM_WINDOWS
		#ifdef COREARRAY_CC_GNU_MINGW32
			typedef CRITICAL_SECTION TdMutex;
			// typedef HANDLE TdMutex;
		#else
			typedef RTL_CRITICAL_SECTION TdMutex;
		#endif
	#elif defined(COREARRAY_POSIX_THREAD)
		typedef pthread_mutex_t TdMutex;
	#else
		"..."
	#endif
		CdThreadMutex();
		~CdThreadMutex();
		void Lock();
		void Unlock();
		bool TryLock();
		COREARRAY_INLINE TdMutex &Mutex() { return mutex; }
	private:
		TdMutex mutex;
	};

	/// The pointer to a thread mutex object
	typedef CdThreadMutex* PdThreadMutex;



	/// The auto object for locking and unlocking a mutex object
	struct COREARRAY_DLL_DEFAULT TdAutoMutex
	{
		CdThreadMutex * mutex;
		TdAutoMutex(CdThreadMutex *m) { mutex = m; if (m) m->Lock(); }
		~TdAutoMutex() { if (mutex) mutex->Unlock(); }

		/// Reset the mutex object
		void Reset(CdThreadMutex *m)
		{
			if (m != mutex)
			{
				if (mutex) mutex->Unlock();
				mutex = m;
				if (m) m->Lock();
			}
		}
	};



	class COREARRAY_DLL_DEFAULT CdThread;

	typedef int (*TdThreadProc)(CdThread *Thread, void *Data);

	namespace _INTERNAL
	{
		class COREARRAY_DLL_DEFAULT CdThBasic {
        public:
			virtual ~CdThBasic() {}
		};

		template<typename Tx> class CdThBasicEx: public CdThBasic {
		public:
			Tx Data;
		};

		struct COREARRAY_DLL_DEFAULT TdThreadData
		{
			CdThread *thread;
			TdThreadProc proc;
			void *Data;
		};

		template<typename Tx> struct TdThreadDataEx
		{
			typedef int (*TdProc)(CdThread *Thread, Tx Data);
			TdProc proc;
			Tx Data;
		};

		template<typename Tx>
		int _pTdThreadEx(CdThread *Thread, void *Data) {
			CdThBasicEx< _INTERNAL::TdThreadDataEx<Tx> > *p =
				(CdThBasicEx< _INTERNAL::TdThreadDataEx<Tx> >*)Data;
			return (*p->Data.proc)(Thread, p->Data.Data);
		}
	}

    /// Thread class
	class COREARRAY_DLL_DEFAULT CdThread
	{
	public:
		friend class CdThreadsSuspending;

	#if defined(COREARRAY_POSIX_THREAD)
		typedef pthread_t TStruct;
	#elif defined(COREARRAY_PLATFORM_WINDOWS)
		typedef struct {
			HANDLE Handle;
			DWORD ThreadID;
		} TStruct;
	#else
		XXXX
	#endif

		CdThread();
		CdThread(TdThreadProc proc, void *Data);
		virtual ~CdThread();

		void BeginThread();
		template<typename Tx>
			void BeginThread(int (*proc)(CdThread *, Tx), Tx val)
		{
        	_INTERNAL::CdThBasicEx< _INTERNAL::TdThreadDataEx<Tx> > *p =
				new _INTERNAL::CdThBasicEx< _INTERNAL::TdThreadDataEx<Tx> >;
			p->Data.proc = proc; p->Data.Data = val;
			vData.thread = this; vData.proc = _INTERNAL::_pTdThreadEx<Tx>;
			vPrivate = p; vData.Data = (void*)p;
			_BeginThread();
		}

		int RunThreadSafe();
		virtual int RunThread();
		int EndThread();
		void Terminate();

		COREARRAY_INLINE bool Terminated() const { return terminated; }
		COREARRAY_INLINE TStruct &Thread() { return thread; }
		COREARRAY_INLINE int &ExitCode() { return fExitCode; }
        COREARRAY_INLINE string &ErrorInfo() { return fErrorInfo; }

	protected:
		TStruct thread;
		int fExitCode;
		string fErrorInfo;
		bool terminated;
		_INTERNAL::TdThreadData vData;
		void _BeginThread(); // need vData

	private:
		_INTERNAL::CdThBasic *vPrivate;
		void Done();
	};

	/// The pointer to a thread object
	typedef CdThread* PdThread;


	/// Closure or delegate for C++
	template<class Tx> struct COREARRAY_DLL_DEFAULT TdThreadObjProc
	{
		void (Tx::*proc)(CdThread *);
		Tx * obj;
		COREARRAY_INLINE void Proc(CdThread *thread) { (obj->*proc)(thread); }
	};


	/// Class for suspending and resuming thread
	class COREARRAY_DLL_DEFAULT CdThreadsSuspending
	{
    public:
		CdThreadsSuspending();
        ~CdThreadsSuspending();

		void Suspend();
        void WakeUp();

	protected:

	#if defined(COREARRAY_POSIX_THREAD)

		pthread_mutex_t mutex;
		pthread_cond_t threshold;

	#elif defined(COREARRAY_PLATFORM_WINDOWS)

		int waiters_count_;
		CRITICAL_SECTION waiters_count_lock_;
		int release_count_;
		int wait_generation_count_;
		HANDLE event_;

	#else
		XXXX
	#endif
	};

	typedef CdThreadsSuspending* PdThreadsSuspending;



	// =====================================================================
	// Exception
	// =====================================================================

	/// Error Macro
	#define _COREARRAY_ERRMACRO_(x) { \
		va_list args; va_start(args, x); \
		Init(x, args); \
		va_end(args); \
	}

	/// The root class of exception for CoreArray library
	class COREARRAY_DLL_EXPORT ErrCoreArray: public exception
	{
	public:
		ErrCoreArray(): exception()
			{ }
		ErrCoreArray(const char *fmt, ...): exception()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ErrCoreArray(const string &msg): exception()
			{ fMessage = msg; }
		virtual const char *what() const throw()
			{ return fMessage.c_str(); }
		virtual ~ErrCoreArray() throw()
			{ }

	protected:
		string fMessage;
		void Init(const char *fmt, va_list arglist);
	};


	#ifdef COREARRAY_CODE_DEBUG
	/// Exception for invalid calling
	class COREARRAY_DLL_EXPORT ErrDEBUG: public ErrCoreArray
	{
	public:
		ErrDEBUG(): ErrCoreArray()
			{ }
		ErrDEBUG(const char *fmt, ...): ErrCoreArray()
			{ _COREARRAY_ERRMACRO_(fmt); }
	};
	#endif


	/// Exception for system error
	class COREARRAY_DLL_EXPORT ErrOSError: public ErrCoreArray
	{
	public:
		ErrOSError(): ErrCoreArray()
			{ }
		ErrOSError(const char *fmt, ...): ErrCoreArray()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ErrOSError(const string &msg): ErrCoreArray()
			{ fMessage = msg; }
	};

	/// Exceptions for threads
	class COREARRAY_DLL_EXPORT ErrThread: public ErrOSError
	{
	public:
		ErrThread(): ErrOSError()
			{ }
		ErrThread(const char *fmt, ...): ErrOSError()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ErrThread(const string &msg): ErrOSError()
			{ fMessage = msg; }
	};


	/// Exceptions for conversion
	class COREARRAY_DLL_EXPORT ErrConvert: public ErrCoreArray
	{
	public:
		ErrConvert(): ErrCoreArray()
			{ }
		ErrConvert(const char *fmt, ...): ErrCoreArray()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ErrConvert(const string &msg): ErrCoreArray()
			{ fMessage = msg; }
	};
}

#endif /* _HEADER_COREARRAY_PLATFORM_ */
