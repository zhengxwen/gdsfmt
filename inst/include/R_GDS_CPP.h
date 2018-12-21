// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// R_GDS_CPP.h: C interface to gdsfmt dynamic library + CPP classes
//
// Copyright (C) 2014-2018    Xiuwen Zheng
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
 *	\file     R_GDS_CPP.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2014 - 2018
 *	\brief    C interface to gdsfmt dynamic library + CPP classes
 *	\details
**/

#ifndef _HEADER_R_GDS_CPP_
#define _HEADER_R_GDS_CPP_

#include <R_GDS.h>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <climits>


namespace CoreArray
{
	// ==================================================================
	// ==================================================================

	/// try block for CoreArray library
	#define CORE_TRY    \
		try {

	/// try block for CoreArray library
	#define COREARRAY_TRY    \
		bool has_error = false; \
		SEXP rv_ans = R_NilValue; \
		CORE_TRY


	/// catch block for CoreArray library
	#define CORE_CATCH(cmd)    \
		} \
		catch (std::exception &E) { \
			GDS_SetError(E.what()); cmd; \
		} \
		catch (const char *E) { \
			GDS_SetError(E); cmd; \
		} \
		catch (...) { \
			GDS_SetError("unknown error!"); cmd; \
		} \

	/// catch block for CoreArray library
	#define COREARRAY_CATCH    \
		CORE_CATCH(has_error = true); \
		if (has_error) error(GDS_GetError()); \
		return rv_ans;



	// ==================================================================
	// ==================================================================

	// [[ ********
	#ifndef COREARRAY_GDSFMT_PACKAGE

	/// Error Macro
	#define _COREARRAY_ERRMACRO_(x) { \
		va_list args; va_start(args, x); \
		Init(x, args); \
		va_end(args); \
	}

	/// The root class of exception for CoreArray library
	class ErrCoreArray: public std::exception
	{
	public:
		ErrCoreArray() {}
		ErrCoreArray(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		ErrCoreArray(const std::string &msg) { fMessage = msg; }

		virtual const char *what() const throw()
			{ return fMessage.c_str(); }
		virtual ~ErrCoreArray() throw() {};

	protected:
		std::string fMessage;

		void Init(const char *fmt, va_list arglist)
		{
			char buf[1024];
			vsnprintf(buf, sizeof(buf), fmt, arglist);
			fMessage = buf;
		}
	};


	/// automatic mutex object
	struct TdAutoMutex
	{
		PdThreadMutex mutex;

		TdAutoMutex(PdThreadMutex m)
			{ mutex = m; if (m) GDS_Parallel_LockMutex(m); }
		~TdAutoMutex()
			{ if (mutex) GDS_Parallel_UnlockMutex(mutex); }

		COREARRAY_INLINE void Reset(PdThreadMutex m)
		{
			if (m != mutex)
			{
				if (mutex) GDS_Parallel_UnlockMutex(mutex);
				mutex = m;
				if (m) GDS_Parallel_LockMutex(m);
			}
		}
	};


	/// the class of read array
	class CArrayRead
	{
	public:
		CArrayRead(PdAbstractArray Obj, int Margin, C_SVType SVType,
			const C_BOOL *const Selection[], bool buf_if_need=true)
		{
			_Obj = GDS_ArrayRead_Init(Obj, Margin, SVType, Selection,
				buf_if_need);
			if (!_Obj)
				throw ErrCoreArray("Error 'initialize CArrayRead'.");
		}
		~CArrayRead()
		{
			GDS_ArrayRead_Free(_Obj);
		}

		/// read data
		void Read(void *Buffer)
		{
			GDS_ArrayRead_Read(_Obj, Buffer);
		}

		/// return true, if it is of the end
		bool Eof()
		{
			return GDS_ArrayRead_Eof(_Obj);
		}

	protected:
		PdArrayRead _Obj;
	};

	#else  // COREARRAY_GDSFMT_PACKAGE

	/// error exception for the gdsfmt package
	class COREARRAY_DLL_EXPORT ErrGDSFmt: public ErrCoreArray
	{
	public:
		ErrGDSFmt() {}
		ErrGDSFmt(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		ErrGDSFmt(const std::string &msg) { fMessage = msg; }
	};

	#endif  // COREARRAY_GDSFMT_PACKAGE
	// ]] ********

}

#endif /* _HEADER_R_GDS_CPP_ */
