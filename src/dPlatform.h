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
 *	\file     dPlatform.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Functions for independent platforms
 *	\details
**/

#ifndef _HEADER_COREARRAY_PLATFORM_
#define _HEADER_COREARRAY_PLATFORM_

#include <dType.h>
#include <dString.h>
#include <string.h>
#   include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
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

	extern const double NaN;
	extern const double Infinity;
	extern const double NegInfinity;

	enum TFPClass
	{
		fpFinite=0,    //< a finite number
		fpPosInf,      //< +INF
		fpNegInf,      //< -INF
		fpNaN          //< NaN
	};

	COREARRAY_DLL_DEFAULT TFPClass FloatClassify(const float val);
	COREARRAY_DLL_DEFAULT TFPClass FloatClassify(const double val);
	COREARRAY_DLL_DEFAULT TFPClass FloatClassify(const long double val);

	// whether it is finite
	COREARRAY_DLL_DEFAULT bool IsFinite(const float val);
	COREARRAY_DLL_DEFAULT bool IsFinite(const double val);
	COREARRAY_DLL_DEFAULT bool IsFinite(const long double val);

	// whether it is NaN
	COREARRAY_DLL_DEFAULT bool IsNaN(const float val);
	COREARRAY_DLL_DEFAULT bool IsNaN(const double val);
	COREARRAY_DLL_DEFAULT bool IsNaN(const long double val);

	// whether it is +infinity
	COREARRAY_DLL_DEFAULT bool IsInf(const float val);
	COREARRAY_DLL_DEFAULT bool IsInf(const double val);
	COREARRAY_DLL_DEFAULT bool IsInf(const long double val);

	// whether it is -infinity
	COREARRAY_DLL_DEFAULT bool IsNegInf(const float val);
	COREARRAY_DLL_DEFAULT bool IsNegInf(const double val);
	COREARRAY_DLL_DEFAULT bool IsNegInf(const long double val);

	// whether v1 == v2, considering NaN
	COREARRAY_DLL_DEFAULT bool EqaulFloat(const float v1, const float v2);
	COREARRAY_DLL_DEFAULT bool EqaulFloat(const double v1, const double v2);
	COREARRAY_DLL_DEFAULT bool EqaulFloat(const long double v1, const long double v2);

	/// Make FPU exception enable
	COREARRAY_DLL_DEFAULT void EnableFPUException();
	/// Make FPU exception disable
	COREARRAY_DLL_DEFAULT void DisableFPUException();
	/// Reset default FPU exception
	COREARRAY_DLL_DEFAULT void DefaultFPUControl();


	// =====================================================================
	// Format a string
	// =====================================================================

	/// format a string
	COREARRAY_DLL_DEFAULT UTF8String Format(const char *fmt, ...);
	/// format a string assuming a short string with at most 64 characters
	COREARRAY_DLL_DEFAULT UTF8String _FmtNum(const char *fmt, ...);


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
	COREARRAY_DLL_DEFAULT UTF16String PCharToUTF16(const char *s);
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

		// portable "pthread_cond_t" in Win32
		// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
		//     3.3. The Generation Count Solution
		int waiters_count_;  //< Count of the number of waiters.
		CRITICAL_SECTION waiters_count_lock_;  //< Serialize access to <waiters_count_>.
		int release_count_;  //< Number of threads to release via a <pthread_cond_broadcast>
		int wait_generation_count_;  //< Keeps track of the current "generation" so that
				// we don't allow one thread to steal all the "releases" from the broadcast.
		HANDLE event_;  //< A manual-reset event that's used to block and release waiting threads.

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


	COREARRAY_DLL_DEFAULT void SwapData(void *d1, void *d2, size_t len);



	namespace _INTERNAL
	{
		// Type Convert

		template<typename DestT, typename SourceT,
			int DTrait = TdTraits<DestT>::trVal,
			int STrait = TdTraits<SourceT>::trVal >
		struct COREARRAY_DLL_DEFAULT TValCvt
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const SourceT &val) { return val; }
			COREARRAY_FORCEINLINE static void Array(DestT *p, SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = *s++; }
		};

		template<typename DestT, typename SourceT>
			struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, SourceT, COREARRAY_TR_INTEGER, COREARRAY_TR_FLOAT>
		{
		#ifdef COREARRAY_CC_GNU
			COREARRAY_FORCEINLINE static DestT Cvt(const SourceT &val)
				{ return DestT(typename TdTraits<DestT>::TType(val)); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = DestT(typename TdTraits<DestT>::TType(*s++)); }
		#else
			COREARRAY_FORCEINLINE static DestT Cvt(const SourceT &val)
				{ return DestT(TdTraits<DestT>::TType(val)); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = DestT(TdTraits<DestT>::TType(*s++)); }
		#endif
		};

		// Type Convert: Source UTF8String

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF8String, COREARRAY_TR_INTEGER, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF8String &val)
				{ return StrToInt(val.c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF8String *s, ssize_t L)
				{ for (; L > 0; p++, s++, L--) *p = StrToInt(s->c_str()); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF8*, COREARRAY_TR_INTEGER, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF8 * val)
				{ return StrToInt(val); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF8 **s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToInt(*s++); }
		};

		template<typename SourceT> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF8String, SourceT, COREARRAY_TR_STRING, COREARRAY_TR_INTEGER>
		{
			COREARRAY_FORCEINLINE static UTF8String Cvt(const SourceT val)
				{ return IntToStr(val); }
			COREARRAY_FORCEINLINE static void Array(UTF8String *p, SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = IntToStr(*s++); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF8String, COREARRAY_TR_FLOAT, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF8String &val)
				{ return StrToFloat(val.c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF8String *s, ssize_t L)
				{ for (; L > 0; p++, s++, L--) *p = StrToFloat(s->c_str()); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF8*, COREARRAY_TR_FLOAT, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF8 *val)
				{ return StrToFloat(val); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF8 **s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToFloat(*s++); }
		};

		template<typename SourceT> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF8String, SourceT, COREARRAY_TR_STRING, COREARRAY_TR_FLOAT>
		{
			COREARRAY_FORCEINLINE static UTF8String Cvt(const SourceT &val)
				{ return FloatToStr(val); }
			COREARRAY_FORCEINLINE static void Array(UTF8String *p, const SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = FloatToStr(*s++); }
		};

		template<> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF16String, UTF8String, COREARRAY_TR_STRING, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static UTF16String Cvt(const UTF8String &val)
				{ return CoreArray::UTF8ToUTF16(val); }
			COREARRAY_FORCEINLINE static void Array(UTF16String *p, UTF8String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = CoreArray::UTF8ToUTF16(*s++); }
		};

		template<> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF32String, UTF8String, COREARRAY_TR_STRING, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static UTF32String Cvt(const UTF8String &val)
				{ return CoreArray::UTF8ToUTF32(val); }
			COREARRAY_FORCEINLINE static void Array(UTF32String *p, UTF8String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = CoreArray::UTF8ToUTF32(*s++); }
		};

		// Type Convert: Source UTF16String

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF16String, COREARRAY_TR_INTEGER, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF16String &val)
				{ return StrToInt(UTF16ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF16String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToInt(UTF16ToUTF8(*s++).c_str()); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF16*, COREARRAY_TR_INTEGER, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF16 *val)
				{ return StrToInt(UTF16ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF16 **s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToInt(UTF16ToUTF8(*s++).c_str()); }
		};

		template<typename SourceT> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF16String, SourceT, COREARRAY_TR_STRING, COREARRAY_TR_INTEGER>
		{
			COREARRAY_FORCEINLINE static UTF16String Cvt(const SourceT val)
				{ return ASC16(IntToStr(val)); }
			COREARRAY_FORCEINLINE static void Array(UTF16String *p, SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = ASC16(IntToStr(*s++)); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF16String, COREARRAY_TR_FLOAT, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF16String &val)
				{ return StrToFloat(UTF16ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF16String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToFloat(UTF16ToUTF8(*s++).c_str()); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF16*, COREARRAY_TR_FLOAT, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF16 *val)
				{ return StrToFloat(UTF16ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF16 **s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToFloat(UTF16ToUTF8(*s++).c_str()); }
		};

		template<typename SourceT> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF16String, SourceT, COREARRAY_TR_STRING, COREARRAY_TR_FLOAT>
		{
			COREARRAY_FORCEINLINE static UTF16String Cvt(const SourceT &val)
				{ return ASC16(FloatToStr(val)); }
			COREARRAY_FORCEINLINE static void Array(UTF16String *p, const SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = ASC16(FloatToStr(*s++)); }
		};

		template<> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF8String, UTF16String, COREARRAY_TR_STRING, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static UTF8String Cvt(const UTF16String &val)
				{ return CoreArray::UTF16ToUTF8(val); }
			COREARRAY_FORCEINLINE static void Array(UTF16String *p, UTF8String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = CoreArray::UTF8ToUTF16(*s++); }
		};

		template<> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF32String, UTF16String, COREARRAY_TR_STRING, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static UTF32String Cvt(const UTF16String &val)
				{ return CoreArray::UTF16ToUTF32(val); }
			COREARRAY_FORCEINLINE static void Array(UTF16String *p, UTF32String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = CoreArray::UTF32ToUTF16(*s++); }
		};

		// Type Convert: Source UTF32String

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF32String, COREARRAY_TR_INTEGER, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF32String &val)
				{ return StrToInt(UTF32ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF32String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToInt(UTF32ToUTF8(*s++).c_str()); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF32*, COREARRAY_TR_INTEGER, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF32 *val)
				{ return StrToInt(UTF32ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF32 **s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToInt(UTF32ToUTF8(*s++).c_str()); }
		};

		template<typename SourceT> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF32String, SourceT, COREARRAY_TR_STRING, COREARRAY_TR_INTEGER>
		{
			COREARRAY_FORCEINLINE static UTF32String Cvt(const SourceT val)
				{ return ASC32(IntToStr(val)); }
			COREARRAY_FORCEINLINE static void Array(UTF32String *p, SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = ASC32(IntToStr(*s++)); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF32String, COREARRAY_TR_FLOAT, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF32String &val)
				{ return StrToFloat(UTF32ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF32String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToFloat(UTF32ToUTF8(*s++).c_str()); }
		};

		template<typename DestT> struct COREARRAY_DLL_DEFAULT
			TValCvt<DestT, UTF32*, COREARRAY_TR_FLOAT, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static DestT Cvt(const UTF32 *val)
				{ return StrToFloat(UTF32ToUTF8(val).c_str()); }
			COREARRAY_FORCEINLINE static void Array(DestT *p, UTF32 **s, ssize_t L)
				{ for (; L > 0; L--) *p++ = StrToFloat(UTF32ToUTF8(*s++).c_str()); }
		};

		template<typename SourceT> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF32String, SourceT, COREARRAY_TR_STRING, COREARRAY_TR_FLOAT>
		{
			COREARRAY_FORCEINLINE static UTF32String Cvt(const SourceT &val)
				{ return ASC32(FloatToStr(val)); }
			COREARRAY_FORCEINLINE static void Array(UTF32String *p, const SourceT *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = ASC32(FloatToStr(*s++)); }
		};

		template<> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF8String, UTF32String, COREARRAY_TR_STRING, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static UTF8String Cvt(const UTF32String &val)
				{ return CoreArray::UTF32ToUTF8(val); }
			COREARRAY_FORCEINLINE static void Array(UTF8String *p, UTF32String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = CoreArray::UTF32ToUTF8(*s++); }
		};

		template<> struct COREARRAY_DLL_DEFAULT
			TValCvt<UTF16String, UTF32String, COREARRAY_TR_STRING, COREARRAY_TR_STRING>
		{
			COREARRAY_FORCEINLINE static UTF16String Cvt(const UTF32String &val)
				{ return CoreArray::UTF32ToUTF16(val); }
			COREARRAY_FORCEINLINE static void Array(UTF16String *p, UTF32String *s, ssize_t L)
				{ for (; L > 0; L--) *p++ = CoreArray::UTF32ToUTF16(*s++); }
		};
	}

	/// Conversion from SourceT to DestT
	/** \tparam  DestT    type of destination
	 *  \tparam  SourceT  type of source
	**/
	template<typename DestT, typename SourceT>
	static COREARRAY_FORCEINLINE DestT ValCvt(const SourceT &val)
		{ return _INTERNAL::TValCvt<DestT, SourceT>::Cvt(val); }

	/// Conversion from SourceT to DestT
	/** \tparam  DestT    type of destination
	 *  \tparam  SourceT  type of source
	**/
	template<typename DestT, typename SourceT>
	static COREARRAY_FORCEINLINE void ValCvtArray(DestT *p, SourceT *s, ssize_t L)
		{ _INTERNAL::TValCvt<DestT, SourceT>::Array(p, s, L); }



	// --------------------------------------------------------------------

	/// Unaligned C_Int16 -- get
	static COREARRAY_FORCEINLINE C_Int16 GET_VAL_UNALIGNED_PTR(const C_Int16 *p)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_Int16(s[0]) | (C_Int16(s[1]) << 8);
		} else
			return *p;
	}

	/// Unaligned C_UInt16 -- get
	static COREARRAY_FORCEINLINE C_UInt16 GET_VAL_UNALIGNED_PTR(const C_UInt16 *p)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_UInt16(s[0]) | (C_UInt16(s[1]) << 8);
		} else
			return *p;
	}

	/// Unaligned C_Int32 -- get
	static COREARRAY_FORCEINLINE C_Int32 GET_VAL_UNALIGNED_PTR(const C_Int32 *p)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_Int32(s[0]) | (C_Int32(s[1]) << 8) |
				(C_Int32(s[2]) << 16) | (C_Int32(s[3]) << 24);
		} else
			return *p;
	}

	/// Unaligned C_UInt32 -- get
	static COREARRAY_FORCEINLINE C_UInt32 GET_VAL_UNALIGNED_PTR(const C_UInt32 *p)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s = (const C_UInt8*)p;
			return C_UInt32(s[0]) | (C_UInt32(s[1]) << 8) |
				(C_UInt32(s[2]) << 16) | (C_UInt32(s[3]) << 24);
		} else
			return *p;
	}

	/// Unaligned C_Int16 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_PTR(C_Int16 *p, C_Int16 val)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1];
		} else
			*p = val;
	}

	/// Unaligned C_UInt16 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_PTR(C_UInt16 *p, C_UInt16 val)
	{
		if (size_t(p) & 0x01)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1];
		} else
			*p = val;
	}

	/// Unaligned C_Int32 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_PTR(C_Int32 *p, C_Int32 val)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1]; s2[2] = s1[2]; s2[3] = s1[3];
		} else
			*p = val;
	}

	/// Unaligned C_UInt32 -- set
	static COREARRAY_FORCEINLINE void SET_VAL_UNALIGNED_PTR(C_UInt32 *p, C_UInt32 val)
	{
		if (size_t(p) & 0x03)
		{
			const C_UInt8 *s1 = (const C_UInt8*)&val;
			C_UInt8 *s2 = (C_UInt8*)p;
			s2[0] = s1[0]; s2[1] = s1[1]; s2[2] = s1[2]; s2[3] = s1[3];
		} else
			*p = val;
	}




	// --------------------------------------------------------------------
	// Endianness

	#ifdef COREARRAY_ENDIAN_LITTLE

		#define COREARRAY_ENDIAN_CVT(x, size)
		#define COREARRAY_ENDIAN_CVT_I16(x)    (x)
		#define COREARRAY_ENDIAN_CVT_I32(x)    (x)
		#define COREARRAY_ENDIAN_CVT_I64(x)    (x)
		#define COREARRAY_ENDIAN_CVT_F32(x)    (x)
		#define COREARRAY_ENDIAN_CVT_F64(x)    (x)
		#define COREARRAY_ENDIAN_CVT_S8(x)     (x)
		#define COREARRAY_ENDIAN_CVT_S16(x)    (x)
		#define COREARRAY_ENDIAN_CVT_S32(x)    (x)
		#define COREARRAY_ENDIAN_VAL(x)        (x)
		#define COREARRAY_ENDIAN_VAL_PTR(x)
		#define COREARRAY_ENDIAN_ARRAY(x, size)

	#elif defined(COREARRAY_ENDIAN_BIG)

		#define COREARRAY_ENDIAN_CVT        CoreArray::Endian_Cvt
		#define COREARRAY_ENDIAN_CVT_I16    CoreArray::Endian_Cvt_I16
		#define COREARRAY_ENDIAN_CVT_I32    CoreArray::Endian_Cvt_I32
		#define COREARRAY_ENDIAN_CVT_I64    CoreArray::Endian_Cvt_I64
		#define COREARRAY_ENDIAN_CVT_F32    CoreArray::Endian_Cvt_F32
		#define COREARRAY_ENDIAN_CVT_F64    CoreArray::Endian_Cvt_F64
		#define COREARRAY_ENDIAN_CVT_S16    CoreArray::Endian_Cvt_S16
		#define COREARRAY_ENDIAN_CVT_S32    CoreArray::Endian_Cvt_S32
		#define COREARRAY_ENDIAN_VAL        CoreArray::Endian_Val
		#define COREARRAY_ENDIAN_VAL_PTR    CoreArray::Endian_Val_Ptr
		#define COREARRAY_ENDIAN_ARRAY      CoreArray::Endian_Array

		COREARRAY_DLL_DEFAULT void Endian_Cvt(void *x, size_t size);
		COREARRAY_DLL_DEFAULT C_UInt16 Endian_Cvt_I16(C_UInt16 x);
		COREARRAY_DLL_DEFAULT C_UInt32 Endian_Cvt_I32(C_UInt32 x);
		COREARRAY_DLL_DEFAULT C_UInt64 Endian_Cvt_I64(C_UInt64 x);
		COREARRAY_DLL_DEFAULT C_Float32 Endian_Cvt_F32(C_Float32 x);
		COREARRAY_DLL_DEFAULT C_Float64 Endian_Cvt_F64(C_Float64 x);
		COREARRAY_DLL_DEFAULT UTF16String Endian_Cvt_S16(const UTF16String& x);
		COREARRAY_DLL_DEFAULT UTF32String Endian_Cvt_S32(const UTF32String& x);


		namespace _INTERNAL
		{
			// Endianness Conversion
			template<typename TYPE> struct COREARRAY_DLL_DEFAULT TEndianValCvt
			{
				COREARRAY_FORCEINLINE static TYPE Cvt(const TYPE &val) { return val; }
				COREARRAY_FORCEINLINE static void Array(TYPE *p, ssize_t L) { }
			};

			template<> struct COREARRAY_DLL_DEFAULT TEndianValCvt<C_Int16>
			{
				COREARRAY_FORCEINLINE static C_Int16 Cvt(C_Int16 val)
					{ return COREARRAY_ENDIAN_CVT_I16(val); }
				COREARRAY_FORCEINLINE static void Array(C_Int16 *p, ssize_t L)
					{ for (; L > 0; L--, p++) *p = COREARRAY_ENDIAN_CVT_I16(*p); }
			};
			template<> struct COREARRAY_DLL_DEFAULT TEndianValCvt<C_UInt16>
			{
				COREARRAY_FORCEINLINE static C_UInt16 Cvt(C_UInt16 val)
					{ return COREARRAY_ENDIAN_CVT_I16(val); }
				COREARRAY_FORCEINLINE static void Array(C_UInt16 *p, ssize_t L)
					{ for (; L > 0; L--, p++) *p = COREARRAY_ENDIAN_CVT_I16(*p); }
			};

			template<> struct COREARRAY_DLL_DEFAULT TEndianValCvt<C_Int32>
			{
				COREARRAY_FORCEINLINE static C_Int32 Cvt(C_Int32 val)
					{ return COREARRAY_ENDIAN_CVT_I32(val); }
				COREARRAY_FORCEINLINE static void Array(C_Int32 *p, ssize_t L)
					{ for (; L > 0; L--, p++) *p = COREARRAY_ENDIAN_CVT_I32(*p); }
			};
			template<> struct COREARRAY_DLL_DEFAULT TEndianValCvt<C_UInt32>
			{
				COREARRAY_FORCEINLINE static C_UInt32 Cvt(C_UInt32 val)
					{ return COREARRAY_ENDIAN_CVT_I32(val); }
				COREARRAY_FORCEINLINE static void Array(C_UInt32 *p, ssize_t L)
					{ for (; L > 0; L--, p++) *p = COREARRAY_ENDIAN_CVT_I32(*p); }
			};

			template<> struct COREARRAY_DLL_DEFAULT TEndianValCvt<C_Int64>
			{
				COREARRAY_FORCEINLINE static C_Int64 Cvt(C_Int64 val)
					{ return COREARRAY_ENDIAN_CVT_I64(val); }
				COREARRAY_FORCEINLINE static void Array(C_Int64 *p, ssize_t L)
					{ for (; L > 0; L--, p++) *p = COREARRAY_ENDIAN_CVT_I64(*p); }
			};
			template<> struct COREARRAY_DLL_DEFAULT TEndianValCvt<C_UInt64>
			{
				COREARRAY_FORCEINLINE static C_UInt64 Cvt(C_UInt64 val)
					{ return COREARRAY_ENDIAN_CVT_I64(val); }
				COREARRAY_FORCEINLINE static void Array(C_UInt64 *p, ssize_t L)
					{ for (; L > 0; L--, p++) *p = COREARRAY_ENDIAN_CVT_I64(*p); }
			};
		}


		template<typename TYPE>
		static COREARRAY_FORCEINLINE TYPE Endian_Val(const TYPE &val)
			{ return _INTERNAL::TEndianValCvt<TYPE>::Cvt(val); }

		template<typename TYPE>
		static COREARRAY_FORCEINLINE void Endian_Array(TYPE *p, ssize_t L)
			{ return _INTERNAL::TEndianValCvt<TYPE>::Array(p, L); }

	#else
	#  error "Unknown endianness"
    #endif
}

#endif /* _HEADER_COREARRAY_PLATFORM_ */
