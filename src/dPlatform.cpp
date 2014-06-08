// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dPlatform.cpp: Functions for independent platforms
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

#include <CoreDEF.h>
#include <dPlatform.h>
#include <math.h>
#include <time.h>

#include <float.h>
#ifdef COREARRAY_SUN
#  include <ieeefp.h>
#endif

#include <sys/stat.h>

#if defined(COREARRAY_MINGW32) || defined(USING_COREARRAY_MSCLIB)
#include <process.h>
#  ifndef USING_COREARRAY_MSCLIB
#    define USING_COREARRAY_MSCLIB
#  endif
#endif

#ifdef COREARRAY_UNIX

	#include <cerrno>
	#include <fcntl.h>
	#include <unistd.h>
	#include <langinfo.h>
	#include <sys/types.h>

	#if defined(COREARRAY_BSD) || defined(COREARRAY_MACOS)
	#  include <sys/sysctl.h>
	#endif

	#if !defined(COREARRAY_CYGWIN) && !defined(COREARRAY_MACOS)
	#  include <sys/sysinfo.h>
	#endif

#endif



namespace CoreArray
{
	namespace _INTERNAL
	{
		class dPlatformInitObject
		{
		public:
		#if defined(COREARRAY_WINDOWS)
			UINT DefaultSystemCodePage;
		#elif defined(COREARRAY_UNIX)
		#else
		#endif
			dPlatformInitObject()
			{
			#if defined(COREARRAY_WINDOWS)
				DefaultSystemCodePage = GetACP();
			#elif defined(COREARRAY_UNIX)
			#else
			#endif
			}
			~dPlatformInitObject()
			{
			}
		#if defined(COREARRAY_WINDOWS)
		#elif defined(COREARRAY_UNIX)
			static void LocaleConversionError()
				{ throw ErrOSError("Locale Conversion Error!"); }
		#else
		#endif
		};

		dPlatformInitObject Init;
	}
}


using namespace std;
using namespace CoreArray;
using namespace CoreArray::_INTERNAL;


#ifndef COREARRAY_NO_EXTENDED_TYPES

#ifndef COREARRAY_HAVE_INT128

// int128_t

int128_t & int128_t::operator= (Int64 val)
{
	Low = val; High = (val >= 0) ? 0 : -1;
	return *this;
}

int128_t & int128_t::operator= (const uint128_t &val)
{
	Low = val.Low; High = val.High;
	return *this;
}

int128_t::operator Int64() const
{
	return Low;
}

// uint128_t

uint128_t & uint128_t::operator= (UInt64 val)
{
	Low = val; High = 0;
	return *this;
}

uint128_t & uint128_t::operator= (const int128_t &val)
{
	Low = val.Low; High = val.High;
	return *this;
}

uint128_t::operator UInt64() const
{
	return Low;
}

#endif


#ifndef COREARRAY_HAVE_FLOAT128

namespace CoreArray
{

#ifndef COREARRAY_SUN
#  pragma pack(push, 1)
#else
#  pragma pack(1)
#endif
typedef union
{
	double value;
	struct
	{
	#ifdef COREARRAY_LITTLE_ENDIAN
		unsigned int fraction1:32;
		unsigned int fraction0:20;
		unsigned int exponent :11;
		unsigned int sign     : 1;
	#elif defined(COREARRAY_BIG_ENDIAN)
		unsigned int sign     : 1;
		unsigned int exponent :11;
		unsigned int fraction0:20;
		unsigned int fraction1:32;
	#else
	#  error "Unsupported Endianness!"
	#endif
	} num;
} structDouble;

typedef union
{
	long double value;
	struct
	{
	#ifdef COREARRAY_LITTLE_ENDIAN
		unsigned int fraction1:32;
		unsigned int fraction0:31;
		unsigned int exponent :16;
		unsigned int sign     : 1;
	#elif defined(COREARRAY_BIG_ENDIAN)
		unsigned int sign     : 1;
		unsigned int exponent :16;
		unsigned int fraction0:31;
		unsigned int fraction1:32;
	#else
	#  error "Unsupported Endianness!"
	#endif
	} num;
} structLongDouble;

typedef union
{
	struct
	{
	#ifdef COREARRAY_LITTLE_ENDIAN
		UInt16 fraction4;
		UInt32 fraction3;
		unsigned int fraction2:1;
		unsigned int fraction1:32;
		unsigned int fraction0:31;
		unsigned int exponent :15;
		unsigned int sign     : 1;
	#elif defined(COREARRAY_BIG_ENDIAN)
		unsigned int sign     : 1;
		unsigned int exponent :15;
		unsigned int fraction0:31;
		unsigned int fraction1:32;
		unsigned int fraction2:1;
		UInt32 fraction3;
		UInt16 fraction4;
	#else
	#  error "Unsupported Endianness!"
	#endif
	} numlong;

	struct
	{
	#ifdef COREARRAY_LITTLE_ENDIAN
		UInt16 fraction4;
		UInt32 fraction3;
		unsigned int fraction2:12;
		unsigned int fraction1:32;
		unsigned int fraction0:20;
		unsigned int exponent :15;
		unsigned int sign     : 1;
	#elif defined(COREARRAY_BIG_ENDIAN)
		unsigned int sign     : 1;
		unsigned int exponent :15;
		unsigned int fraction0:20;
		unsigned int fraction1:32;
		unsigned int fraction2:12;
		UInt32 fraction3;
		UInt16 fraction4;
	#else
	#  error "Unsupported Endianness!"
	#endif
	} numdouble;

	struct
	{
	#ifdef COREARRAY_LITTLE_ENDIAN
		UInt16 fraction2;
		UInt32 fraction1;
		UInt64 fraction0;
		UInt16 flag;
	#elif defined(COREARRAY_BIG_ENDIAN)
		UInt16 flag;
		UInt64 fraction0;
		UInt32 fraction1;
		UInt16 fraction2;
	#else
	#  error "Unsupported Endianness!"
	#endif
	} numout;
} structFloat128;

#ifndef COREARRAY_SUN
#  pragma pack(pop)
#else
#  pragma pack(8)
#endif

}


#ifndef COREARRAY_HAVE_INT128
Float128::Float128(const Int128 &val)
{

}

Float128::Float128(const UInt128 &val)
{

}
#endif

Float128::Float128(long double val)
{
    *this = val;
}

Float128 & Float128::operator= (long double val)
{
#ifndef COREARRAY_LONGFLOAT_IS_DOUBLE
	structLongDouble *s = (structLongDouble *)((void*)&val);
	structFloat128 *d = (structFloat128 *)((void*)this);
	d->numlong.sign = s->num.sign;
	d->numlong.exponent = s->num.exponent;
	d->numlong.fraction0 = s->num.fraction0;
	d->numlong.fraction1 = s->num.fraction1;
	d->numlong.fraction2 = 0;
	d->numlong.fraction3 = 0;
	d->numlong.fraction4 = 0;
#else
	structDouble *s = (structDouble *)((void*)&val);
	structFloat128 *d = (structFloat128 *)((void*)this);
	d->numdouble.sign = s->num.sign;
	d->numdouble.exponent = s->num.exponent;
	d->numdouble.fraction0 = s->num.fraction0;
	d->numdouble.fraction1 = s->num.fraction1;
	d->numdouble.fraction2 = 0;
	d->numdouble.fraction3 = 0;
	d->numdouble.fraction4 = 0;
#endif
	return *this;
}

Float128::operator long double() const
{
	long double rv;
#ifndef COREARRAY_LONGFLOAT_IS_DOUBLE
	structFloat128 *s = (structFloat128 *)((void*)this);
	structLongDouble *d = (structLongDouble *)((void*)&rv);
	d->num.sign = s->numlong.sign;
	d->num.exponent = s->numlong.exponent;
	d->num.fraction0 = s->numlong.fraction0;
	d->num.fraction1 = s->numlong.fraction1;
#else
	structFloat128 *s = (structFloat128 *)((void*)this);
	structDouble *d = (structDouble *)((void*)&rv);
	d->num.sign = s->numdouble.sign;
	d->num.exponent = s->numdouble.exponent;
	d->num.fraction0 = s->numdouble.fraction0;
	d->num.fraction1 = s->numdouble.fraction1;
#endif
	return *this;
}

Float128 Float128::min()
{
	Float128 rv;
	structFloat128 *d = (structFloat128 *)((void*)&rv);
	d->numout.flag = 0xFFFE;
	d->numout.fraction0 = TdTraits<UInt64>::Max();
	d->numout.fraction1 = TdTraits<UInt32>::Max();
	d->numout.fraction2 = TdTraits<UInt16>::Max();
	return rv;
}

Float128 Float128::max()
{
	Float128 rv;
	structFloat128 *d = (structFloat128 *)((void*)&rv);
	d->numout.flag = 0x7FFE;
	d->numout.fraction0 = TdTraits<UInt64>::Max();
	d->numout.fraction1 = TdTraits<UInt32>::Max();
	d->numout.fraction2 = TdTraits<UInt16>::Max();
	return rv;
}

#endif

#endif


// Math Operation

const double CoreArray::NaN =
	std::numeric_limits<double>::quiet_NaN();
const double CoreArray::Infinity =
	std::numeric_limits<double>::infinity();
const double CoreArray::NegInfinity =
	-std::numeric_limits<double>::infinity();


TFPClass CoreArray::FloatClassify(const float val)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		switch (fpclass(val))
		{
			case _FPCLASS_PINF: return fpPosInf;
			case _FPCLASS_NINF: return fpNegInf;
		#ifdef _FPCLASS_UNSUP
			case _FPCLASS_UNSUP:
		#endif
			case _FPCLASS_SNAN: case _FPCLASS_QNAN: return fpNaN;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_SUN)
		switch (fpclass(val))
		{
			case FP_SNAN:
			case FP_QNAN: return fpNaN;
			case FP_NINF: return fpNegInf;
			case FP_PINF: return fpPosInf;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_GNUG)
		switch (fpclassify(val))
		{
			case FP_INFINITE: return signbit(val) ? fpNegInf : fpPosInf;
			case FP_NAN: return fpNaN;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		switch (_fpclass(val))
		{
			case _FPCLASS_PINF:
				return fpPosInf;
			case _FPCLASS_NINF:
				return fpNegInf;
		#ifdef _FPCLASS_UNSUP
			case _FPCLASS_UNSUP:
		#endif
			case _FPCLASS_SNAN: case _FPCLASS_QNAN:
				return fpNaN;
			default: return fpFinite;
		}
	#else
		#error "Not support!"
	#endif
}

TFPClass CoreArray::FloatClassify(const double val)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		switch (fpclass(val))
		{
			case _FPCLASS_PINF:
				return fpPosInf;
			case _FPCLASS_NINF:
				return fpNegInf;
		#ifdef _FPCLASS_UNSUP
			case _FPCLASS_UNSUP:
		#endif
			case _FPCLASS_SNAN: case _FPCLASS_QNAN:
				return fpNaN;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_SUN)
		switch (fpclass(val))
		{
			case FP_SNAN:
			case FP_QNAN: return fpNaN;
			case FP_NINF: return fpNegInf;
			case FP_PINF: return fpPosInf;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_GNUG)
		switch (fpclassify(val))
		{
			case FP_INFINITE: return signbit(val) ? fpNegInf : fpPosInf;
			case FP_NAN: return fpNaN;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		switch (_fpclass(val))
		{
			case _FPCLASS_PINF:
				return fpPosInf;
			case _FPCLASS_NINF:
				return fpNegInf;
		#ifdef _FPCLASS_UNSUP
			case _FPCLASS_UNSUP:
		#endif
			case _FPCLASS_SNAN: case _FPCLASS_QNAN:
				return fpNaN;
			default: return fpFinite;
		}
	#else
		#error "Not support!"
	#endif
}

TFPClass CoreArray::FloatClassify(const long double val)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		switch (fpclass(val))
		{
			case _FPCLASS_PINF:
				return fpPosInf;
			case _FPCLASS_NINF:
				return fpNegInf;
		#ifdef _FPCLASS_UNSUP
			case _FPCLASS_UNSUP:
		#endif
			case _FPCLASS_SNAN: case _FPCLASS_QNAN:
				return fpNaN;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_SUN)
		switch (fpclass(val))
		{
			case FP_SNAN:
			case FP_QNAN: return fpNaN;
			case FP_NINF: return fpNegInf;
			case FP_PINF: return fpPosInf;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_GNUG)
		switch (fpclassify(val))
		{
			case FP_INFINITE: return signbit(val) ? fpNegInf : fpPosInf;
			case FP_NAN: return fpNaN;
			default: return fpFinite;
		}
	#elif defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		switch (_fpclass(val))
		{
			case _FPCLASS_PINF:
				return fpPosInf;
			case _FPCLASS_NINF:
				return fpNegInf;
		#ifdef _FPCLASS_UNSUP
			case _FPCLASS_UNSUP:
		#endif
			case _FPCLASS_SNAN: case _FPCLASS_QNAN:
            	return fpNaN;
			default: return fpFinite;
		}
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsFinite(const float V)
{
	#if defined(COREARRAY_SUN)
		return (finite(V) != 0);
	#elif defined(COREARRAY_GNUG)
		return isfinite(V);
	#elif defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		return _finite(V);
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsFinite(const double V)
{
	#if defined(COREARRAY_SUN)
		return (finite(V) != 0);
	#elif defined(COREARRAY_GNUG)
		return isfinite(V);
	#elif defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		return _finite(V);
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsFinite(const long double V)
{
	#if defined(COREARRAY_SUN)
		return (finite(V) != 0);
	#elif defined(COREARRAY_GNUG)
		return isfinite(V);
	#elif defined(COREARRAY_BORLANDC)
		return _finitel(V);
	#elif defined(COREARRAY_MSC)
		return _finite(V);
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsNaN(const float V)
{
	#if defined(COREARRAY_SUN)
		return (isnanf(V) != 0);
	#elif defined(COREARRAY_GNUG)
		return isnan(V);
	#elif defined(COREARRAY_BORLANDC)
		return _isnan(V);
	#elif defined(COREARRAY_MSC)
		return _isnan(V);
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsNaN(const double V)
{
	#if defined(COREARRAY_SUN)
		return (isnand(V) != 0);
	#elif defined(COREARRAY_GNUG)
		return isnan(V);
	#elif defined(COREARRAY_BORLANDC)
		return _isnan(V);
	#elif defined(COREARRAY_MSC)
		return _isnan(V);
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsNaN(const long double V)
{
	#if defined(COREARRAY_SUN)
		return (isnand(V) != 0);
	#elif defined(COREARRAY_GNUG)
		return isnan(V);
	#elif defined(COREARRAY_BORLANDC)
		return _isnanl(V);
	#elif defined(COREARRAY_MSC)
		return _isnanl(V);
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsInf(const float V)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		return fpclass(V) == _FPCLASS_PINF;
	#elif defined(COREARRAY_SUN)
		return fpclass(V) == FP_PINF;
	#elif defined(COREARRAY_GNUG)
		return (fpclassify(V) == FP_INFINITE) && !signbit(V);
	#elif defined(COREARRAY_BORLANDC)
		return _fpclass(V) == _FPCLASS_PINF;
	#elif defined(COREARRAY_MSC)
		return _fpclass(V) == _FPCLASS_PINF;
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsInf(const double V)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		return fpclass(V) == _FPCLASS_PINF;
	#elif defined(COREARRAY_SUN)
		return fpclass(V) == FP_PINF;
	#elif defined(COREARRAY_GNUG)
		return (fpclassify(V) == FP_INFINITE) && !signbit(V);
	#elif defined(COREARRAY_BORLANDC)
		return _fpclass(V) == _FPCLASS_PINF;
	#elif defined(COREARRAY_MSC)
		return _fpclass(V) == _FPCLASS_PINF;
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsInf(const long double V)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		return fpclass(V) == _FPCLASS_PINF;
	#elif defined(COREARRAY_SUN)
		return fpclass(V) == FP_PINF;
	#elif defined(COREARRAY_GNUG)
		return (fpclassify(V) == FP_INFINITE) && !signbit(V);
	#elif defined(COREARRAY_BORLANDC)
		return _fpclass(V) == _FPCLASS_PINF;
	#elif defined(COREARRAY_MSC)
		return _fpclass(V) == _FPCLASS_PINF;
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsNegInf(const float V)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		return fpclass(V) == _FPCLASS_NINF;
	#elif defined(COREARRAY_SUN)
		return fpclass(V) == FP_NINF;
	#elif defined(COREARRAY_GNUG)
		return (fpclassify(V) == FP_INFINITE) && signbit(V);
	#elif defined(COREARRAY_BORLANDC)
		return _fpclass(V) == _FPCLASS_NINF;
	#elif defined(COREARRAY_MSC)
		return _fpclass(V) == _FPCLASS_NINF;
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsNegInf(const double V)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		return fpclass(V) == _FPCLASS_NINF;
	#elif defined(COREARRAY_SUN)
		return fpclass(V) == FP_NINF;
	#elif defined(COREARRAY_GNUG)
		return (fpclassify(V) == FP_INFINITE) && signbit(V);
	#elif defined(COREARRAY_BORLANDC)
		return _fpclass(V) == _FPCLASS_NINF;
	#elif defined(COREARRAY_MSC)
		return _fpclass(V) == _FPCLASS_NINF;
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::IsNegInf(const long double V)
{
	#if defined(COREARRAY_GNUG) && defined(COREARRAY_MINGW32)
		return fpclass(V) == _FPCLASS_NINF;
	#elif defined(COREARRAY_SUN)
		return fpclass(V) == FP_NINF;
	#elif defined(COREARRAY_GNUG)
		return (fpclassify(V) == FP_INFINITE) && signbit(V);
	#elif defined(COREARRAY_BORLANDC)
		return _fpclass(V) == _FPCLASS_NINF;
	#elif defined(COREARRAY_MSC)
		return _fpclass(V) == _FPCLASS_NINF;
	#else
		#error "Not support!"
	#endif
}

bool CoreArray::EqaulFloat(const float v1, const float v2)
{
	if (IsFinite(v1))
	{
		if (IsFinite(v2))
			return (v1 == v2);
		else
			return false;
	} else {
		return !IsFinite(v2);
	}
}

bool CoreArray::EqaulFloat(const double v1, const double v2)
{
	if (IsFinite(v1))
	{
		if (IsFinite(v2))
			return (v1 == v2);
		else
			return false;
	} else {
		return !IsFinite(v2);
	}
}

bool CoreArray::EqaulFloat(const long double v1, const long double v2)
{
	if (IsFinite(v1))
	{
		if (IsFinite(v2))
			return (v1 == v2);
		else
			return false;
	} else {
		return !IsFinite(v2);
	}
}

void CoreArray::EnableFPUException()
{
	#if defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		_controlfp(0, MCW_EM);
	#elif defined(COREARRAY_MINGW32)
		_controlfp(0, _MCW_EM);
	#elif defined(COREARRAY_SUN)
		#if defined(__sparc)
			fpsetmask(FP_X_INV | FP_X_DZ | FP_X_OFL | FP_X_UFL | FP_X_IMP);
		#else
			fpsetmask(FP_X_INV | FP_X_DNML | FP_X_DZ | FP_X_OFL | FP_X_UFL | FP_X_IMP);
		#endif
	#elif defined(COREARRAY_GNUG)
		// _controlfp(0, MCW_EM);
	#else
		"..."
	#endif
}

void CoreArray::DisableFPUException()
{
	#if defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		_controlfp(EM_INVALID | EM_DENORMAL | EM_ZERODIVIDE | EM_OVERFLOW |
			EM_UNDERFLOW | EM_INEXACT, MCW_EM);
	#elif defined(COREARRAY_MINGW32)
		_controlfp(_EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE |
			_EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT, _MCW_EM);
	#elif defined(COREARRAY_SUN)
		#if defined(__sparc)
			fpsetmask(~(FP_X_INV | FP_X_DZ | FP_X_OFL | FP_X_UFL | FP_X_IMP));
		#else
			fpsetmask(~(FP_X_INV | FP_X_DNML | FP_X_DZ | FP_X_OFL | FP_X_UFL | FP_X_IMP));
		#endif
	#elif defined(COREARRAY_GNUG)
		// _controlfp
	#else
		"..."
	#endif
}

void CoreArray::DefaultFPUControl()
{
	#if defined(COREARRAY_SUN)
		DisableFPUException();
	#elif defined(COREARRAY_GNUG)
		DisableFPUException();
	#elif defined(COREARRAY_BORLANDC) || defined(COREARRAY_MSC)
		_fpreset();
		DisableFPUException();
	#else
		"..."
	#endif
}


static string fpInfString("+Inf");
static string fpInfString2("Inf");
static string fpNegInfString("-Inf");
static string fpNaNString("NaN");

string CoreArray::FloatToStr(const float val)
{
	switch (FloatClassify(val))
	{
		case fpPosInf: return fpInfString;
		case fpNegInf: return fpNegInfString;
		case fpNaN: return fpNaNString;
		default: return _FmtNum("%.7g", val);
	}
}

string CoreArray::FloatToStr(const double val)
{
	switch (FloatClassify(val))
	{
		case fpPosInf: return fpInfString;
		case fpNegInf: return fpNegInfString;
		case fpNaN: return fpNaNString;
		default: return _FmtNum("%.15g", val);
	}
}

string CoreArray::FloatToStr(const long double val)
{
	switch (FloatClassify(val))
	{
		case fpPosInf: return fpInfString;
		case fpNegInf: return fpNegInfString;
		case fpNaN: return fpNaNString;
	#ifdef COREARRAY_LONGFLOAT_IS_DOUBLE
		default: return _FmtNum("%.15g", val);
	#else
		default: return _FmtNum("%.17g", val);
	#endif
	}
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
string CoreArray::FloatToStr(const Float128 val)
{
    return FloatToStr((long double)val);
}
#endif

string CoreArray::IntToStr(const Int8 val)
{
	return _FmtNum("%d", val);
}

string CoreArray::IntToStr(const UInt8 val)
{
	return _FmtNum("%d", val);
}

string CoreArray::IntToStr(const Int16 val)
{
	return _FmtNum("%d", val);
}

string CoreArray::IntToStr(const UInt16 val)
{
	return _FmtNum("%d", val);
}

string CoreArray::IntToStr(const Int32 val)
{
	return _FmtNum("%d", val);
}

string CoreArray::IntToStr(const UInt32 val)
{
	return _FmtNum("%u", val);
}

string CoreArray::IntToStr(const Int64 val)
{
	return _FmtNum("%lld", val);
}

string CoreArray::IntToStr(const UInt64 val)
{
	return _FmtNum("%llu", val);
}


#ifndef COREARRAY_NO_EXTENDED_TYPES

string CoreArray::IntToStr(const Int128 val)
{
#ifndef COREARRAY_HAVE_INT128
	char buf[64];
	val.toStr(buf);
	return UTF8String(buf);
#else
#  error ""
#endif
}

string CoreArray::IntToStr(const UInt128 val)
{
#ifndef COREARRAY_HAVE_INT128
	char buf[64];
	val.toStr(buf);
	return UTF8String(buf);
#else
#  error ""
#endif
}

#endif


#if defined(COREARRAY_WINDOWS) || defined(COREARRAY_MSDOS)
	const char *const CoreArray::sLineBreak = "\r\n";
	const char *const CoreArray::sFileSep = "\\";
#else
	const char *const CoreArray::sLineBreak = "\n";
	const char *const CoreArray::sFileSep = "/";
#endif

#if defined(COREARRAY_WINDOWS)
	const TSysHandle CoreArray::NullSysHandle = NULL;
#elif defined(COREARRAY_UNIX)
	const TSysHandle CoreArray::NullSysHandle = 0;
#else
	"......"
#endif


#ifdef COREARRAY_UNIX

const size_t ICONVBUFFER = 512;

template<typename OutUTF> struct IConvName {};
template<> struct IConvName<UTF8>
{
	static const char *Name() { return nl_langinfo(CODESET); }
	static size_t Ign(const UTF8* In, size_t InLen)
	{
		// size_t rv = mblen(In, InLen);
		// mblen(NULL, 0);
		// return rv;
		return 1;
	}
};
template<> struct IConvName<UTF16>
{
	static const char *Name() { return "UTF-16LE"; }
	static size_t Ign(const UTF16* In, size_t InLen) { return 1; };
};
template<> struct IConvName<UTF32>
{
	static const char *Name() { return "UTF-32LE"; }
	static size_t Ign(const UTF32* In, size_t InLen) { return 1; };
};

// OutUTF, InUTF should be UTF8, UTF16, or UTF32
template<typename OutUTF, typename InUTF>
	basic_string<OutUTF> UTFCvt(const InUTF *In, size_t InLen)
{
	OutUTF buf[ICONVBUFFER/sizeof(OutUTF)];
	basic_string<OutUTF> rvstr;
	TdICONV cd(IConvName<OutUTF>::Name(), IConvName<InUTF>::Name()); // (to, from)

	const char *src = (char*)In;
	char *dest = (char*)buf;
	size_t SrcLeft = InLen * sizeof(InUTF);
	size_t DestLeft, rv;

	while (SrcLeft > 0)
	{
		dest = (char*)buf; DestLeft = sizeof(buf);
		rv = cd.Cvt(src, SrcLeft, dest, DestLeft);
		if (rv == (size_t)(-1))
		{
			if (errno == E2BIG)
			{
				// Need new room
				rvstr.append(&buf[0], &buf[(sizeof(buf)-DestLeft)/sizeof(OutUTF)]);
			} else {
				// Invalid input character
				cd.Reset();
				size_t cs = IConvName<InUTF>::Ign((InUTF*)src, SrcLeft/sizeof(InUTF));
				SrcLeft -= cs * sizeof(InUTF);
				src += cs * sizeof(InUTF);
				rvstr.append(&buf[0], &buf[(sizeof(buf)-DestLeft)/sizeof(OutUTF)]);
				OutUTF Unknown = '?';
				rvstr.append(cs, Unknown); // Unknown Char Indicator
			}
		} else {
			rvstr.append(&buf[0], &buf[(sizeof(buf)-DestLeft)/sizeof(OutUTF)]);
			break;
		}
	}
	return rvstr;
}

#endif


// ErrCoreArray

void ErrCoreArray::Init(const char *fmt, va_list arglist)
{
	char buf[1024];
	vsnprintf(buf, sizeof(buf), fmt, arglist);
	fMessage = buf;
}


#ifdef COREARRAY_UNIX
TdICONV::TdICONV(const char *to, const char *from)
{
	#ifdef COREARRAY_R_LINK
		fHandle = Riconv_open(to, from);
	#else
		fHandle = iconv_open(to, from);
	#endif
	if (fHandle == (TIconv)(-1))
		Init.LocaleConversionError();
}

TdICONV::~TdICONV()
{
	if (fHandle != (TIconv)(-1))
	{
	#ifdef COREARRAY_R_LINK
		Riconv_close(fHandle);
	#else
		iconv_close(fHandle);
	#endif
    }
}

void TdICONV::Reset()
{
	size_t Zero = 0;
	#ifdef COREARRAY_R_LINK
		Riconv(fHandle, NULL, &Zero, NULL, &Zero);
	#else
		iconv(fHandle, NULL, &Zero, NULL, &Zero);
	#endif
}

size_t TdICONV::Cvt(const char * &inbuf, size_t &inbytesleft,
	char* &outbuf, size_t &outbytesleft)
{
	size_t rv;

#ifdef COREARRAY_R_LINK
		rv = Riconv(fHandle, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
#else
#   if defined(COREARRAY_CYGWIN) || defined(COREARRAY_SUN)
		rv = iconv(fHandle, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
#   else
		rv = iconv(fHandle, (char**)&inbuf, &inbytesleft, &outbuf, &outbytesleft);
#   endif
#endif
	if (rv == (size_t)(-1))
	{
		int err = errno;
		if ((err!=E2BIG) && (err!=EILSEQ) && (err!=EINVAL))
			Init.LocaleConversionError();
	}
	return rv;
}

/*static int iconv_do_one(unsigned int namescount, const char * const *names,
	void *data)
{
	vector<string> *list = (vector<string> *)data;
	for (unsigned int i = 0; i < namescount; i++)
		list->push_back(names[i]);
}

vector<string> TdICONV::List() {
	vector<string> rv;
	iconvlist(iconv_do_one, (void*)&rv);
	return rv;
}
*/
#endif


UTF16String CoreArray::UTF7toUTF16(const char* str)
{
	UTF16String rv(strlen(str), 0);
	const char *p = str;
	for (UTF16String::iterator it = rv.begin(); it != rv.end(); it++)
		*it = (UInt8)*p++;
	return rv;
}

UTF16String CoreArray::UTF7toUTF16(const UTF8String &s)
{
	UTF16String rv(s.size(), 0);
	UTF8String::const_iterator ps = s.begin();
	UTF16String::iterator pp = rv.begin();
	for (; ps != s.end();) *pp++ = (UInt8)*ps++;
	return rv;
}

UTF32String CoreArray::UTF7toUTF32(const char *str)
{
	UTF32String rv(strlen(str), 0);
	const char *p = str;
	for (UTF32String::iterator it = rv.begin(); it != rv.end(); it++)
		*it = (UInt8)*p++;
	return rv;
}

UTF32String CoreArray::UTF7toUTF32(const UTF8String &s)
{
	UTF32String rv(s.size(), 0);
	UTF8String::const_iterator ps = s.begin();
	UTF32String::iterator pp = rv.begin();
	for (; ps != s.end();) *pp++ = (UInt8)*ps++;
	return rv;
}

UTF16String CoreArray::UTF8toUTF16(const UTF8String &s)
{
	if (!s.empty())
	{
	#if defined(COREARRAY_WINDOWS)
		int L = MultiByteToWideChar(Init.DefaultSystemCodePage, 0, s.c_str(),
			s.length(), NULL, 0);
		if (L <= 0)
			RaiseLastOSError<ErrOSError>();
		UTF16String rv(L, 0);
		if (MultiByteToWideChar(Init.DefaultSystemCodePage, 0, s.c_str(),
				s.length(), (wchar_t*)rv.data(), L) <= 0)
			RaiseLastOSError<ErrOSError>();
		return rv;
	#elif defined(COREARRAY_UNIX)
		return UTFCvt<UTF16, UTF8>(s.data(), s.length());
	#else
    	return UTF7toUTF16(s);
	#endif
	} else
		return UTF16String();
}

UTF16String CoreArray::PChartoUTF16(const char * str)
{
	int len = strlen(str);
	if (len > 0)
	{
	#if defined(COREARRAY_WINDOWS)
		int L = MultiByteToWideChar(Init.DefaultSystemCodePage, 0, str,
			len, NULL, 0);
		if (L <= 0)
			RaiseLastOSError<ErrOSError>();
		UTF16String rv(L, 0);
		if (MultiByteToWideChar(Init.DefaultSystemCodePage, 0, str,
				len, (wchar_t*)rv.data(), L) <= 0)
			RaiseLastOSError<ErrOSError>();
		return rv;
	#elif defined(COREARRAY_UNIX)
		return UTFCvt<UTF16, UTF8>(str, strlen(str));
	#else
		return UTF7toUTF16(str);
	#endif
	} else
		return UTF16String();
}

UTF32String CoreArray::UTF8toUTF32(const UTF8String &s)
{
	if (!s.empty())
	{
	#if defined(COREARRAY_WINDOWS)
		UTF16String r = UTF8toUTF16(s);
		return UTF32String(r.begin(), r.end());
	#elif defined(COREARRAY_UNIX)
		return UTFCvt<UTF32, UTF8>(s.data(), s.length());
	#else
		UTF16String r = UTF8toUTF16(s);
		return UTF32String(r.begin(), r.end());
	#endif
	} else
		return UTF32String();
}

UTF8String CoreArray::UTF16toUTF8(const UTF16String &ws)
{
	if (!ws.empty())
	{
	#if defined(COREARRAY_WINDOWS)
		int L = WideCharToMultiByte(Init.DefaultSystemCodePage, 0, ws.c_str(),
			ws.length(), NULL, 0, NULL, NULL);
		if (L <= 0)
			RaiseLastOSError<ErrOSError>();
		UTF8String rv(L, 0);
		if (WideCharToMultiByte(Init.DefaultSystemCodePage, 0, ws.c_str(),
				ws.length(), (char*)rv.data(), L, NULL, NULL) <= 0)
			RaiseLastOSError<ErrOSError>();
		return rv;
	#elif defined(COREARRAY_UNIX)
		return UTFCvt<UTF8, UTF16>(ws.data(), ws.length());
	#else
		return UTF8String(ws.begin(), ws.end());
	#endif
	} else
		return UTF8String();
}

UTF32String CoreArray::UTF16toUTF32(const UTF16String &ws)
{
	if (!ws.empty()) {
	#if defined(COREARRAY_WINDOWS)
		// need to improve
		return UTF32String(ws.begin(), ws.end());
	#elif defined(COREARRAY_UNIX)
		return UTFCvt<UTF32, UTF16>(ws.data(), ws.length());
	#else
		// need to improve
		return UTF32String(ws.begin(), ws.end());
	#endif
	} else
		return UTF32String();
}

UTF8String CoreArray::UTF32toUTF8(const UTF32String &ws)
{
	if (!ws.empty())
	{
	#if defined(COREARRAY_WINDOWS)
		// need to improve
		return UTF16toUTF8(UTF16String(ws.begin(), ws.end()));
	#elif defined(COREARRAY_UNIX)
		return UTFCvt<UTF8, UTF32>(ws.data(), ws.length());
	#else
		// need to improve
		return UTF32String(ws.begin(), ws.end());
	#endif
	} else
		return UTF8String();
}

UTF16String CoreArray::UTF32toUTF16(const UTF32String &ws)
{
	if (!ws.empty())
	{
	#if defined(COREARRAY_WINDOWS)
		// need to improve
		return UTF16String(ws.begin(), ws.end());
	#elif defined(COREARRAY_UNIX)
		return UTFCvt<UTF16, UTF32>(ws.data(), ws.length());
	#else
		// need to improve
		return UTF16String(ws.begin(), ws.end());
	#endif
	} else
		return UTF16String();
}


// System Error information

int CoreArray::GetLastOSError()
{
	#if defined(COREARRAY_WINDOWS)
		return GetLastError();
	#elif defined(COREARRAY_UNIX)
		return errno;
	#else
		"GetLastOSError"
	#endif
}

UTF8String CoreArray::SysErrMessage(int err)
{
	#if defined(COREARRAY_WINDOWS)
		char buf[1024];
		memset((void*)buf, 0, sizeof(buf));
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ARGUMENT_ARRAY, NULL, err, 0, buf, sizeof(buf), NULL);
		return string(buf);
	#elif defined(COREARRAY_UNIX)
		return UTF8String(strerror(err));
	#else
	#endif
}

UTF8String CoreArray::LastSysErrMsg()
{
	return SysErrMessage(GetLastOSError());
}

// File Functions

TSysHandle CoreArray::SysCreateFile(char const* const AFileName, UInt32 Mode)
{
	#if defined(COREARRAY_WINDOWS)
		TSysHandle H;
		H = CreateFile(AFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		return (H != INVALID_HANDLE_VALUE) ? H : NULL;
	#elif defined(COREARRAY_UNIX)
		TSysHandle H;
		#ifdef O_LARGEFILE
			int flag = O_LARGEFILE;
		#else
			int flag = 0;
		#endif
		H = open(AFileName, flag | O_CREAT | O_RDWR | O_TRUNC,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		return (H > 0) ? H : 0;
	#else
		"SysCreateFile"
	#endif
}

#if defined(COREARRAY_WINDOWS)
static const DWORD AccessMode[3] =
	{ GENERIC_READ, GENERIC_WRITE, GENERIC_READ | GENERIC_WRITE };
static const DWORD ShareMode[4] =
	{ 0, FILE_SHARE_READ, FILE_SHARE_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE};
#elif defined(COREARRAY_UNIX)
static const int AccessMode[3] =
	{ O_RDONLY, O_WRONLY, O_RDWR };
#ifdef O_SHARE_NONE
static const int ShareMode[4] =
	{ O_SHARE_NONE, O_SHARE_RDONLY, O_SHARE_WRONLY, O_SHARE_RDWR };
#else
static const int ShareMode[4] =
	{ 0, 0, 0, 0 };
#endif
#else
    "..."
#endif

TSysHandle CoreArray::SysOpenFile(char const* const AFileName,
	enum TSysOpenMode mode, enum TSysShareMode smode)
{
	#if defined(COREARRAY_WINDOWS)
		TSysHandle H;
		H = CreateFile(AFileName, AccessMode[mode], ShareMode[smode], NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		return (H != INVALID_HANDLE_VALUE) ? H : NULL;
	#elif defined(COREARRAY_UNIX)
		TSysHandle H;
		#ifdef O_LARGEFILE
			int flag = O_LARGEFILE;
		#else
			int flag = 0;
		#endif
		H = open(AFileName, flag | AccessMode[mode] | ShareMode[smode]);
		return (H > 0) ? H : 0;
	#else
		"SysOpenFile"
	#endif
}

bool CoreArray::SysCloseHandle(TSysHandle Handle)
{
	#if defined(COREARRAY_WINDOWS)
		return CloseHandle(Handle);
	#elif defined(COREARRAY_UNIX)
		return close(Handle)==0;
	#else
		"SysCloseHandle"
	#endif
}

size_t CoreArray::SysHandleRead(TSysHandle Handle, void *Buffer, size_t Count)
{
	#if defined(COREARRAY_WINDOWS)
		unsigned long rv;
		if (ReadFile(Handle, Buffer, Count, &rv, NULL))
			return rv;
		else
			return 0;
	#elif defined(COREARRAY_UNIX)
		ssize_t rv = read(Handle, Buffer, Count);
		return (rv >= 0) ? rv : 0;
	#else
		"SysHandleRead"
	#endif
}

size_t CoreArray::SysHandleWrite(TSysHandle Handle, const void* Buffer,
	size_t Count)
{
	#if defined(COREARRAY_WINDOWS)
		unsigned long rv;
		if (WriteFile(Handle, Buffer, Count, &rv, NULL))
			return rv;
		else return 0;
	#elif defined(COREARRAY_UNIX)
		ssize_t rv = write(Handle, Buffer, Count);
		return (rv >= 0) ? rv : 0;
	#else
		"SysHandleWrite"
	#endif
}

Int64 CoreArray::SysHandleSeek(TSysHandle Handle, Int64 Offset,
	enum TdSysSeekOrg sk)
{
	#if defined(COREARRAY_WINDOWS)
		Int64 p = Offset;
		DWORD *Lo = (DWORD*)&p;
		DWORD *Hi = ((DWORD*)&p) + 1;

		*Lo = SetFilePointer(Handle, *Lo, (long*)Hi, sk);
		if (*Lo==INVALID_SET_FILE_POINTER && GetLastError()!=0)
			return -1;
		else return p;
	#elif defined(COREARRAY_UNIX)
		#if defined(COREARRAY_CYGWIN) || defined(COREARRAY_MACOS)
			return lseek(Handle, Offset, sk);
		#else
			return lseek64(Handle, Offset, sk);
		#endif
	#else
		"SysHandleSeek"
	#endif
}

bool CoreArray::SysHandleSetSize(TSysHandle Handle, Int64 NewSize)
{
	#if defined(COREARRAY_WINDOWS)
		if (SysHandleSeek(Handle, NewSize, soBeginning)>=0)
			return SetEndOfFile(Handle);
		else
			return false;
	#elif defined(COREARRAY_UNIX)
		#if defined(COREARRAY_CYGWIN) || defined(COREARRAY_MACOS)
			return ftruncate(Handle, NewSize)==0;
		#else
			return ftruncate64(Handle, NewSize)==0;
		#endif
	#else
		"SysHandleSetSize"
	#endif
}

string CoreArray::TempFileName(const char *prefix, const char *tempdir)
{
	string fn;

	if (!tempdir)
		tempdir = "";
	if (*tempdir)
	{
		fn = tempdir;
		if (tempdir[strlen(tempdir)] != sFileSep[0])
			fn.append(sFileSep);
	}
	if (prefix) fn.append(prefix);

    char tmp[64];
	for (int n = 0; n < 10000; n++)
	{
	#if RAND_MAX > 16777215
		sprintf(tmp, "%x", rand());
	#else
		sprintf(tmp, "%x%x", rand(), rand());
	#endif
		// check file exists
		struct stat sb;
		if (stat((fn + tmp).c_str(), &sb) != 0)
        	return fn + tmp;
	}
	throw ErrOSError("No suitable temporary file name.");
}

bool CoreArray::FileExists(const string &FileName)
{
	struct stat sb;
	return (stat(FileName.c_str(), &sb) == 0);
}

// CPU functions

int CoreArray::Mach::GetNumberOfCPU()
{
	#if defined(COREARRAY_WINDOWS)
		SYSTEM_INFO info;
		info.dwNumberOfProcessors = 0;
		GetSystemInfo(&info);
		return info.dwNumberOfProcessors;
	#elif defined(COREARRAY_CYGWIN)
		const char * p = getenv("NUMBER_OF_PROCESSORS");
		if (p) {
			int rv = atoi(p);
			if (rv < 0) rv = -1;
			return rv;
		} else
			return -1;
	#elif defined(COREARRAY_UNIX)
		int numCPUs = sysconf(_SC_NPROCESSORS_ONLN);
		if (numCPUs <= 0) numCPUs = -1;
		return numCPUs;
	#else
		return -1;
	#endif
}


#if defined(COREARRAY_WINDOWS)

enum MY_LOGICAL_PROCESSOR_RELATIONSHIP {
	My_RelationProcessorCore = 0,
	My_RelationNumaNode,
	My_RelationCache,
	My_RelationProcessorPackage
};

enum MY_PROCESSOR_CACHE_TYPE {
	My_CacheUnified = 0,
	My_CacheInstruction,
	My_CacheData,
	My_CacheTrace
};

struct MY_CACHE_DESCRIPTOR {
    BYTE   Level;
    BYTE   Associativity;
    WORD   LineSize;
    DWORD  Size;
    MY_PROCESSOR_CACHE_TYPE Type;
};

typedef struct {
    ULONG_PTR   ProcessorMask;
    MY_LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
    union {
        struct {
            BYTE  Flags;
        } ProcessorCore;
        struct {
            DWORD NodeNumber;
        } NumaNode;
        MY_CACHE_DESCRIPTOR Cache;
        ULONGLONG  Reserved[2];
    } DUMMYUNIONNAME;
} MY_SYSTEM_LOGICAL_PROCESSOR_INFORMATION, *MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;

typedef BOOL (WINAPI *LPFN_GLPI)(
	MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
#endif

int CoreArray::Mach::GetL1CacheMemory()
{
#if defined(COREARRAY_WINDOWS)
	LPFN_GLPI glpi;
	BOOL done = FALSE;
	MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	DWORD returnLength = 0;
	DWORD byteOffset = 0;
	DWORD rv = 0;

	glpi = (LPFN_GLPI)GetProcAddress(
		GetModuleHandle("kernel32"), "GetLogicalProcessorInformation");
	if (glpi == NULL) return -1;

	while (!done)
	{
		DWORD rc = glpi(buffer, &returnLength);
		if (rc == FALSE)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer) free(buffer);
				buffer = (MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
				if (buffer == NULL) return -1;
			} else
				return -1;
		} else
			done = TRUE;
	}

	ptr = buffer;
	while (byteOffset + sizeof(MY_SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
	{
		if (ptr->Relationship == My_RelationCache)
		{
			// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
			if (ptr->Cache.Level == 1)
			{
				if ((rv == 0) || (ptr->Cache.Size < rv))
					rv = ptr->Cache.Size;
			}
		}
		byteOffset += sizeof(MY_SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}

	free(buffer);
	return (rv==0) ? -1 : rv;

#elif defined(COREARRAY_BSD) || defined(COREARRAY_MACOS)
	uint64_t count = 0;
	size_t size = sizeof(count);
	if (sysctlbyname("hw.l1dcachesize", &count, &size, NULL, 0) == -1)
		return -1;
	else
		return count;

#elif defined(COREARRAY_LINUX)
	return -1;

#else
	return -1;
#endif
}

int CoreArray::Mach::GetL2CacheMemory()
{
#if defined(COREARRAY_WINDOWS)
	LPFN_GLPI glpi;
	BOOL done = FALSE;
	MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	DWORD returnLength = 0;
	DWORD byteOffset = 0;
	DWORD rv = 0;

	glpi = (LPFN_GLPI)GetProcAddress(
		GetModuleHandle("kernel32"), "GetLogicalProcessorInformation");
	if (glpi == NULL) return -1;

	while (!done)
	{
		DWORD rc = glpi(buffer, &returnLength);
		if (rc == FALSE)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer) free(buffer);
				buffer = (MY_PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
				if (buffer == NULL) return -1;
			} else
				return -1;
		} else
			done = TRUE;
	}

	ptr = buffer;
	while (byteOffset + sizeof(MY_SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
	{
		if (ptr->Relationship == My_RelationCache)
		{
			// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
			if (ptr->Cache.Level == 2)
			{
				if ((rv == 0) || (ptr->Cache.Size < rv))
					rv = ptr->Cache.Size;
			}
		}
		byteOffset += sizeof(MY_SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}

	free(buffer);
	return (rv==0) ? -1 : rv;

#elif defined(COREARRAY_BSD) || defined(COREARRAY_MACOS)
	uint64_t count = 0;
	size_t size = sizeof(count);
	if (sysctlbyname("hw.l2cachesize", &count, &size, NULL, 0) == -1)
		return -1;
	else
		return count;

#elif defined(COREARRAY_LINUX)
	char line[200];
	FILE *f = fopen ("/proc/cpuinfo", "r");
	if (!f) return -1;
	int rv_cache = 0;
	// analyze
	while (fgets(line, 200, f) != NULL)
	{
		char *s, *value;
		// NOTE: the ':' is the only character we can rely on
		if (!(value = strchr(line, ':')))
			continue;
		// terminate the valuename
		s = value - 1;
		while ((s >= line) && ((*s == ' ') || (*s == '\t'))) s--;
		*(s + 1) = '\0';
		// and strip leading spaces from value
		value += 1;
		while (*value == ' ') value++;
		if ((s = strchr(value,'\n'))) *s='\0';

		if (!strcasecmp(line, "cache size"))
		{
			// cache size ...
			int x;
			if (sscanf(value, "%d", &x))
				rv_cache = x*1024;
			else
				rv_cache = -1;
			break;
		}
	}
	fclose(f);
	return rv_cache;

#else
	return -1;
#endif
}


// CdThreadMutex

CdThreadMutex::CdThreadMutex()
{
	#if defined(COREARRAY_WINDOWS)
		InitializeCriticalSection(&mutex);
		// mutex = CreateMutex(NULL, FALSE, NULL);
		// if (mutex == NULL)
			// RaiseLastOSError<ErrOSError>();
	#elif defined(COREARRAY_POSIX_THREAD)
    	int rv = pthread_mutex_init(&mutex, NULL);
    	if (rv != 0)
    		throw ErrOSError("pthread_mutex_init returns an error code %d.", rv);
	#else
    	"..."
	#endif
}

CdThreadMutex::~CdThreadMutex()
{
	#if defined(COREARRAY_WINDOWS)
		DeleteCriticalSection(&mutex);
		// if (CloseHandle(mutex) == 0)
			// RaiseLastOSError<ErrOSError>();
	#elif defined(COREARRAY_POSIX_THREAD)
		int rv = pthread_mutex_destroy(&mutex);
	   	if (rv != 0)
    		throw ErrOSError("pthread_mutex_destroy returns an error code %d.", rv);
	#else
    	"..."
	#endif
}

void CdThreadMutex::Lock()
{
	#if defined(COREARRAY_WINDOWS)
		EnterCriticalSection(&mutex);
		// if (WaitForSingleObject(mutex, INFINITE) == WAIT_FAILED)
			// RaiseLastOSError<ErrOSError>();
	#elif defined(COREARRAY_POSIX_THREAD)
		int rv = pthread_mutex_lock(&mutex);
	   	if (rv != 0)
    		throw ErrOSError("pthread_mutex_lock returns an error code %d.", rv);
	#else
    	"..."
	#endif
}

void CdThreadMutex::Unlock()
{
	#if defined(COREARRAY_WINDOWS)
		LeaveCriticalSection(&mutex);
		// if (!ReleaseMutex(mutex))
			// RaiseLastOSError<ErrOSError>();
	#elif defined(COREARRAY_POSIX_THREAD)
		int rv = pthread_mutex_unlock(&mutex);
	   	if (rv != 0)
    		throw ErrOSError("pthread_mutex_unlock returns an error code %d.", rv);
	#else
    	"..."
	#endif
}

bool CdThreadMutex::TryLock()
{
	#if defined(COREARRAY_WINDOWS)
		return TryEnterCriticalSection(&mutex);
		// DWORD rv = WaitForSingleObject(mutex, INFINITE);
		// if (rv == WAIT_FAILED)
			// RaiseLastOSError<ErrOSError>();
		// return (rv == WAIT_OBJECT_0);
	#elif defined(COREARRAY_POSIX_THREAD)
		return pthread_mutex_trylock(&mutex) == 0;
	#else
    	"..."
	#endif
}


// CdThread

#if defined(COREARRAY_WINDOWS) && !defined(USING_COREARRAY_MSCLIB)

DWORD WINAPI COREARRAY_CALL_ALIGN ThreadWrap1(LPVOID lpThreadParameter)
{
	DisableFPUException();
	CdThread *p = (CdThread*)lpThreadParameter;
	return p->RunThreadSafe();
}

DWORD WINAPI COREARRAY_CALL_ALIGN ThreadWrap2(LPVOID lpThreadParameter)
{
	TdThreadData *p = (TdThreadData*)lpThreadParameter;
	int &rv = p->thread->ExitCode();
	try {
		DisableFPUException();
		rv = (*p->proc)(p->thread, p->Data);
	}
	catch (exception &E) {
    	p->thread->ErrorInfo() = E.what();
		rv = -1;
	}
	catch (...) {
		rv = -1;
	}
	return rv;
}

#elif defined(USING_COREARRAY_MSCLIB)

unsigned WINAPI COREARRAY_CALL_ALIGN ThreadWrap1(void *lpThreadParameter)
{
	DisableFPUException();
	CdThread *p = (CdThread*)lpThreadParameter;
	ssize_t rd = p->RunThreadSafe();
	_endthreadex(rd);
	return rd;
}

unsigned WINAPI COREARRAY_CALL_ALIGN ThreadWrap2(void *lpThreadParameter)
{
	TdThreadData *p = (TdThreadData*)lpThreadParameter;
	int &rv = p->thread->ExitCode();
	try {
		DisableFPUException();
		rv = (*p->proc)(p->thread, p->Data);
	}
	catch (exception &E) {
    	p->thread->ErrorInfo() = E.what();
		rv = -1;
	}
	catch (...) {
		rv = -1;
	}
	ssize_t rd = rv;
	_endthreadex(rv);
	return rd;
}

#elif defined(COREARRAY_POSIX_THREAD)

extern "C" void * COREARRAY_CALL_ALIGN ThreadWrap1(void *lpThreadParameter)
{
	DisableFPUException();
	CdThread *p = (CdThread*)lpThreadParameter;
	ssize_t rd = p->RunThreadSafe();
	return (void*)rd;
}

extern "C" void * COREARRAY_CALL_ALIGN ThreadWrap2(void *lpThreadParameter)
{
	TdThreadData *p = (TdThreadData*)lpThreadParameter;
	int &rv = p->thread->ExitCode();
	try {
		DisableFPUException();
		rv = (*p->proc)(p->thread, p->Data);
	}
	catch (exception &E) {
    	p->thread->ErrorInfo() = E.what();
		rv = -1;
	}
	catch (...) {
		rv = -1;
	}
	ssize_t rd = rv;
	return (void*)rd;
}

#else
	"..."
#endif


CdThread::CdThread()
{
	terminated = false;
	fExitCode = 0;
	vPrivate = NULL;
	memset(&thread, 0, sizeof(thread));
}

CdThread::CdThread(TdThreadProc proc, void *Data)
{
	terminated = false;
	fExitCode = 0;
	vPrivate = NULL;
	memset(&thread, 0, sizeof(thread));
	vData.thread = this; vData.proc = proc; vData.Data = Data;
	_BeginThread();
}

CdThread::~CdThread()
{
	try {
		Terminate();
		EndThread();
	} catch (...) {
		Done(); throw;
	}
	Done();
	if (vPrivate) delete vPrivate;
}

void CdThread::Done()
{
	#if defined(COREARRAY_WINDOWS)
	if (thread.Handle != NULL) {
		CloseHandle(thread.Handle);
		thread.Handle = NULL;
	}
	#elif defined(COREARRAY_POSIX_THREAD)
	if (thread) {
		pthread_detach(thread);
		thread = 0;
	}
	#else
		"..."
	#endif
}

void CdThread::BeginThread()
{
	#if defined(COREARRAY_WINDOWS)
	if (thread.Handle == NULL)
	{
		SECURITY_ATTRIBUTES attr;
			attr.nLength = sizeof(attr);
			attr.lpSecurityDescriptor = NULL;
			attr.bInheritHandle = true;
		#ifndef USING_COREARRAY_MSCLIB
		thread.Handle = CreateThread(&attr, 0, ThreadWrap1, (void*)this,
			0, &thread.ThreadID);
		#else
		unsigned id = 0;
		thread.Handle = (HANDLE)_beginthreadex(&attr, 0, ThreadWrap1,
			(void*)this, 0, &id);
		thread.ThreadID = id;
		#endif
		if (thread.Handle == NULL) RaiseLastOSError<ErrThread>();
	} else
    	throw ErrThread("BeginThread");
	#elif defined(COREARRAY_POSIX_THREAD)
	if (!thread)
	{
		int rv;
		switch (rv=pthread_create(&thread, NULL, ThreadWrap1, (void*)this))
		{
		default:
			if (rv != 0)
				throw ErrOSError("pthread_create exited with %d", rv);
		}
	} else
    	throw ErrThread("BeginThread");
	#else
		"..."
	#endif
}

void CdThread::_BeginThread()
{
	#if defined(COREARRAY_WINDOWS)
	if (thread.Handle == NULL)
	{
		SECURITY_ATTRIBUTES attr;
			attr.nLength = sizeof(attr);
			attr.lpSecurityDescriptor = NULL;
			attr.bInheritHandle = true;
		#ifndef USING_COREARRAY_MSCLIB
		thread.Handle = CreateThread(&attr, 0, ThreadWrap2, (void*)&vData,
			0, &thread.ThreadID);
		#else
		unsigned id = 0;
		thread.Handle = (HANDLE)_beginthreadex(&attr, 0, ThreadWrap2,
			(void*)&vData, 0, &id);
		thread.ThreadID = id;
		#endif
		if (thread.Handle == NULL) RaiseLastOSError<ErrThread>();
	} else
    	throw ErrThread("_BeginThread");
	#elif defined(COREARRAY_POSIX_THREAD)
	if (!thread)
	{
		int rv=pthread_create(&thread, NULL, ThreadWrap2, (void*)&vData);
		switch (rv)
		{
		default:
			if (rv != 0)
            	throw ErrOSError("pthread_create exited with %d", rv);
		}
	} else
    	throw ErrThread("_BeginThread");
	#else
		"..."
	#endif
}

int CdThread::RunThreadSafe()
{
	try {
		fExitCode = RunThread();
	}
	catch (exception &E) {
    	fErrorInfo = E.what();
		fExitCode = -1;
	}
	catch (...) {
        fExitCode = -1;
    }
	return fExitCode;
}

int CdThread::RunThread()
{
	return 0;
}

void CdThread::Terminate()
{
	terminated = true;
}

int CdThread::EndThread()
{
	#if defined(COREARRAY_WINDOWS)
	if (thread.Handle != NULL)
	{
		if (WaitForSingleObject(thread.Handle, INFINITE) == WAIT_FAILED)
			RaiseLastOSError<ErrThread>();
		Done();
	}
	#elif defined(COREARRAY_POSIX_THREAD)
	if (thread)
	{
		int rv = pthread_join(thread, NULL);
		if (rv != 0)
			throw ErrOSError("pthread_join exited with %d", rv);
		Done();
	}
	#else
		"..."
	#endif
	return fExitCode;
}


// CdThreadSuspending

CdThreadsSuspending::CdThreadsSuspending()
{
#if defined(COREARRAY_WINDOWS)
	waiters_count_ = 0;
	wait_generation_count_ = 0;
	release_count_ = 0;
	// Create a critical section
	InitializeCriticalSection(&waiters_count_lock_);
	// Create a manual-reset event.
	event_ = CreateEvent(NULL,  // no security
						TRUE,  // manual-reset
						FALSE, // non-signaled initially
						NULL); // unnamed
#elif defined(COREARRAY_POSIX_THREAD)
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&threshold, NULL);
#endif
}

CdThreadsSuspending::~CdThreadsSuspending()
{
    WakeUp();
#if defined(COREARRAY_WINDOWS)
	DeleteCriticalSection(&waiters_count_lock_);
	CloseHandle(event_);
#elif defined(COREARRAY_POSIX_THREAD)
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&threshold);
#endif
}

void CdThreadsSuspending::Suspend()
{
#if defined(COREARRAY_WINDOWS)

	// Avoid conditions
	EnterCriticalSection(&waiters_count_lock_);
	// Increment count of waiters
	waiters_count_ ++;
	// Store current generation in our activation record
	int my_generation = wait_generation_count_;
	// Leave conditions
	LeaveCriticalSection(&waiters_count_lock_);

	for (;;)
	{
		// Wait until the event is signaled.
		WaitForSingleObject(event_, INFINITE);
		EnterCriticalSection(&waiters_count_lock_);
		// Exit the loop when the <event_> is signaled and
		// there are still waiting threads from this <wait_generation>
		// that haven't been released from this wait yet.
		bool wait_done = (release_count_ > 0)
                    && (wait_generation_count_ != my_generation);
		LeaveCriticalSection(&waiters_count_lock_);
		if (wait_done) break;
	}

	EnterCriticalSection(&waiters_count_lock_);
	waiters_count_ --;
	release_count_ --;
	bool last_waiter = (release_count_ == 0);
	LeaveCriticalSection(&waiters_count_lock_);

	if (last_waiter)
	{
		// We're the last waiter to be notified, so reset the manual event.
		ResetEvent (event_);
	}

#elif defined(COREARRAY_POSIX_THREAD)

	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&threshold, &mutex);
	pthread_mutex_unlock(&mutex);

#else
	"..."
#endif
}

void CdThreadsSuspending::WakeUp()
{
#if defined(COREARRAY_WINDOWS)

	EnterCriticalSection(&waiters_count_lock_);
	if (waiters_count_ > 0)
	{
		SetEvent(event_);
		// Release all the threads in this generation.
		release_count_ = waiters_count_;
		// Start a new generation.
		wait_generation_count_ ++;
	}
	LeaveCriticalSection(&waiters_count_lock_);

#elif defined(COREARRAY_POSIX_THREAD)

	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&threshold);
	pthread_mutex_unlock(&mutex);

#else
	"..."
#endif
}


// CdThreadPool
/*
CdThreadPool::CdThreadPool(size_t MaxThreads=0, size_t InitThreads=0)
{
	fMaxThreads = MaxThreads;
}

void CdThreadPool::SetMaxThreads(size_t NewMaxThreads)
{

}

CdThreadPool::
*/



string CoreArray::NowDateToStr()
{
	time_t tm;
	time(&tm);
	string rv(ctime(&tm));
	rv.erase(rv.size()-1, 1);
	return rv;
}


static const char *esStrToInt = "Unable to convert string to integer.";
static const char *esStrToFloat = "Unable to convert string to double.";
static const char *esFmtErr = "Invalid parameter 'fmt' of Format.";


void CoreArray::SwapData(void *d1, void *d2, size_t len)
{
	char *p1 = (char*)d1;
	char *p2 = (char*)d2;
	for (; len > 0; len--, p1++, p2++)
    	swap(*p1, *p2);
}

UTF8String CoreArray::Format(const char *fmt, ...)
{
	int L;
	char buf[4096];
	va_list args; va_start(args, fmt);
	L = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	if (L >= 0)
		return UTF8String(buf);
	else
		throw ErrConvert(esFmtErr);
}

UTF8String CoreArray::_FmtNum(const char *fmt, ...)
{
	int L;
	char buf[64];
	va_list args; va_start(args, fmt);
	L = vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	if (L >= 0)
		return UTF8String(buf);
	else
		throw ErrConvert(esFmtErr);
}

long CoreArray::StrToInt(char const* str)
{
	char *p;
	long rv = strtol(str, &p, 10);
	while (*p==' ' || *p=='\t') ++p;
	if (*p)
		throw ErrConvert(esStrToInt);
	else
		return rv;
}

bool CoreArray::StrToInt(char const* str, long *rv)
{
	char *p;
	long r = strtol(str, &p, 10);
	while (*p==' ' || *p=='\t') ++p;
	if (*p) {
    	*rv = -1; return false;
	} else {
		if (rv) *rv = r;
		return true;
    }
}

long CoreArray::StrToIntDef(char const* str, const long Default)
{
	char *p;
	long rv = strtol(str, &p, 10);
	while (*p==' ' || *p=='\t') ++p;
	if (*p)
    	return Default;
	else
		return rv;
}

double CoreArray::StrToFloat(char const* str)
{
	if ((fpInfString==str) || (fpInfString2==str))
		return Infinity;
	else if (fpNegInfString == str)
		return NegInfinity;
	else {
		char *p;
		double rv = strtod(str, &p);
		while (*p==' ' || *p=='\t') ++p;
		if (*p)
			throw ErrConvert(esStrToFloat);
		else
			return rv;
    }
}

bool CoreArray::StrToFloat(char const* str, double *rv)
{
	if (fpInfString == str)
	{
    	*rv = Infinity;
		return true;
	} else if (fpNegInfString == str)
	{
    	*rv = NegInfinity;
		return true;
	} else {
		char *p;
		double r = strtod(str, &p);
		while (*p==' ' || *p=='\t') ++p;
		if (*p) {
			*rv = NaN; return false;
		} else {
			if (rv) *rv = r;
			return true;
		}
	}
}

double CoreArray::StrToFloatDef(char const* str, const double Default)
{
	if (fpInfString == str)
		return Infinity;
	else if (fpNegInfString == str)
		return NegInfinity;
	else {
		char *p;
		double r = strtod(str, &p);
		while (*p==' ' || *p=='\t') ++p;
		if (*p)
			return Default;
		else
			return r;
    }
}


#if defined(COREARRAY_LITTLE_ENDIAN)

#elif defined(COREARRAY_BIG_ENDIAN)

void CoreArray::COREARRAY_ENDIAN_CVT(void *x, size_t size)
{
	UInt8 tmp;
	UInt8 *p1 = (UInt8*)x, *p2 = ((UInt8*)x) + size - 1;
	for (size_t sz = size/2; sz > 0; sz--)
	{
		tmp = *p1; *p1 = *p2; *p2 = tmp;
		p1++; p2++;
	}
}

UInt16 CoreArray::COREARRAY_ENDIAN_CVT16(UInt16 x)
{
	UInt16 I0 = x & 0xFF, I1 = x >> 8;
	return (I0 << 8) | I1;
}

UInt32 CoreArray::COREARRAY_ENDIAN_CVT32(UInt32 x)
{
	UInt32 I0 = x & 0xFF, I1 = (x >> 8) & 0xFF;
	UInt32 I2 = (x >> 16) & 0xFF, I3 = x >> 24;
	return (I0 << 24) | (I1 << 16) | (I2 << 8) | I3;
}

UInt64 CoreArray::COREARRAY_ENDIAN_CVT64(UInt64 x)
{
	UInt64 I0 = x & 0xFFFFFFFF, I1 = x >> 32;
	return COREARRAY_ENDIAN_CVT32(I0 << 32) | COREARRAY_ENDIAN_CVT32(I1);
}

#ifndef COREARRAY_NO_EXTENDED_TYPES
UInt128 CoreArray::COREARRAY_ENDIAN_CVT128(UInt128 x)
{
	COREARRAY_ENDIAN_CVT((void*)&x, 16);
	return x;
}
#endif

#else
#  error "Unknown endianness"
#endif
