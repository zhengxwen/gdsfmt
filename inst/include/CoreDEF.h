// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// CoreDEF.h: CoreArray library global macro
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
 *	\file     CoreDEF.h
 *	\author   Xiuwen Zheng [zhengxwen@gmail.com]
 *	\version  1.0
 *	\date     2007-2020
 *	\brief    CoreArray library global macro
 *	\details
**/

/**
 *  \page macros Macros
 *
 *  \section usermacros User-Defined Macros:
 *
 *  \subsection no_inline COREARRAY_NO_FORCEINLINE
 *  If defined, set "COREARRAY_FORCEINLINE = COREARRAY_INLINE"
 *
 *  \subsection no_simd COREARRAY_NO_SIMD
 *  If defined, undefine SIMD macros to disable SIMD-specific codes
 *
 *  \subsection no_std_in_out COREARRAY_NO_STD_IN_OUT
 *  If defined, remove the codes linked to standard input and output streams
 *
 *  \subsection using_r USING_R or COREARRAY_USING_R
 *  If defined, the code is indeed being used with R
 *
 *  \subsection using_log COREARRAY_CODE_USING_LOG
 *  If defined, the procedure of loading GDS files will be recorded
 *
 *  \subsection compression COREARRAY_NO_LZ4
 *  If defined, no LZ4 compression method
 *
 *  \subsection compression COREARRAY_NO_LZMA
 *  If defined, no xz/lzma compression method
 *
 *  \subsection compression COREARRAY_USE_ZLIB_EXT
 *  If defined, uses the libz head file in the default path (e.g., the include path in the operating system)
 *
 *  \subsection compression COREARRAY_USE_LZMA_EXT
 *  If defined, uses the liblzma head file in the default path (e.g., the include path in the operating system)
 *
 *  \subsection compression COREARRAY_NO_COMPILER_OPTIMIZE
 *  If defined, does not include #pragma GCC optimize("O3") or similar
 *
 *  \subsection compression COREARRAY_NO_TARGET
 *  If defined, does not use __attribute__((target())) or __attribute__((target_clones()))
 *
**/


#ifndef _HEADER_COREARRAY_MACRO_
#define _HEADER_COREARRAY_MACRO_


// ===========================================================================
// if the code is indeed being used with R
// Reference: http://cran.r-project.org/doc/manuals/r-release/R-exts.html#Platform-and-version-information
// ===========================================================================

// indicating being used with R
#if defined(USING_R) || defined(COREARRAY_USING_R)
#
#   ifndef COREARRAY_USING_R
#       define COREARRAY_USING_R
#   endif
#
#   define COREARRAY_HAVE_INLINE
#   define COREARRAY_INLINE    R_INLINE
#   define COREARRAY_NO_FORCEINLINE
#   define COREARRAY_NO_FLATTEN
#   define COREARRAY_NO_STD_IN_OUT
#   define COREARRAY_CODE_USING_LOG
#
#   include <Rconfig.h>
#
#   ifdef WORDS_BIGENDIAN
#       define COREARRAY_ENDIAN_BIG
#       ifdef COREARRAY_ENDIAN_LITTLE
#           undef COREARRAY_ENDIAN_LITTLE
#       endif
#       ifdef COREARRAY_ENDIAN_UNKNOWN
#           undef COREARRAY_ENDIAN_UNKNOWN
#       endif
#   else
#       define COREARRAY_ENDIAN_LITTLE
#       ifdef COREARRAY_ENDIAN_BIG
#           undef COREARRAY_ENDIAN_BIG
#       endif
#       ifdef COREARRAY_ENDIAN_UNKNOWN
#           undef COREARRAY_ENDIAN_UNKNOWN
#       endif
#   endif
#
#   ifdef HAVE_VISIBILITY_ATTRIBUTE
#       include <R_ext/Visibility.h>
#       define COREARRAY_DLL_EXPORT     attribute_visible
#       define COREARRAY_DLL_LOCAL      attribute_hidden
#       define COREARRAY_DLL_DEFAULT    COREARRAY_DLL_LOCAL
#   else
#       define COREARRAY_DLL_EXPORT
#       define COREARRAY_DLL_LOCAL
#       define COREARRAY_DLL_DEFAULT
#   endif
#
#endif



// ===========================================================================
// if the code is indeed being used with julia
// ===========================================================================

// indicating being used with julia
#if defined(USING_JULIA) || defined(COREARRAY_USING_JULIA)
#
#   ifndef COREARRAY_USING_JULIA
#       define COREARRAY_USING_JULIA
#   endif
#
#   define COREARRAY_HAVE_INLINE
#   define COREARRAY_INLINE    inline
#   define COREARRAY_NO_FORCEINLINE
#   define COREARRAY_NO_FLATTEN
#   define COREARRAY_NO_STD_IN_OUT
#   define COREARRAY_CODE_USING_LOG
#
#endif



// ===========================================================================
// C/C++ Compiler MACRO
// Reference: http://sourceforge.net/p/predef/wiki/Compilers/
// ===========================================================================

#ifdef COREARRAY_PREDEFINED_CC_COMPILER
#   undef COREARRAY_PREDEFINED_CC_COMPILER
#endif


// GNU C/C++
#ifdef __GNUC__
#   define COREARRAY_CC_GNU
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// MinGW32
#ifdef __MINGW32__
#   define COREARRAY_CC_GNU_MINGW32
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// MinGW-w64
#ifdef __MINGW64__
#   define COREARRAY_CC_GNU_MINGW64
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Clang (also defines the GNU C version macros)
#ifdef __clang__
#   define COREARRAY_CC_CLANG
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Microsoft Visual C/C++
#ifdef _MSC_VER
#   define COREARRAY_CC_MSC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Borland C/C++
#ifdef __BORLANDC__
#   define COREARRAY_CC_BORLAND
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Intel C/C++
#if defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC)
#   define COREARRAY_CC_INTEL
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Oracle Solaris Studio
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#   define COREARRAY_CC_SUNPRO
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Oracle Pro*C Precompiler
#ifdef ORA_PROC
#   define COREARRAY_CC_SUNPRO_PREC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// IBM XL C/C++
#if defined(__xlc__) || defined(__xlC__) || defined(__IBMC__) || defined(__IBMCPP__)
#   define COREARRAY_CC_IBM
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// ARM Compiler
#ifdef __CC_ARM
#   define COREARRAY_CC_ARM
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Dignus Systems/C++ for the IBM mainframe
#ifdef __SYSC__
#   define COREARRAY_CC_DIGNUS
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// HP ANSI C/C++
#if defined(__HP_cc) || defined(__HP_aCC)
#   define COREARRAY_CC_HP
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// IAR C/C++
#ifdef __IAR_SYSTEMS_ICC__
#   define COREARRAY_CC_IAR
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// KEIL
#if defined(__CA__) || defined(__KEIL__) || defined(__C166__) || defined(__C51__) || defined(__CX51__)
#   define COREARRAY_CC_KEIL
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Open64
#if defined(__OPEN64__) || defined(__OPENCC__)
#   define COREARRAY_CC_OPEN64
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Portland Group C/C++
#if defined(__PGI) || defined(__PGIC__)
#   define COREARRAY_CC_PGI
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// SAS/C
#if defined(SASC) || defined(__SASC) || defined(__SASC__)
#   define COREARRAY_CC_SAS
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Small Device C Compiler
#ifdef SDCC
#   define COREARRAY_CC_SDCC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Texas Instruments C/C++ Compiler
#ifdef __TI_COMPILER_VERSION__
#   define COREARRAY_CC_TI
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Tiny C
#ifdef __TINYC__
#   define COREARRAY_CC_TINY
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif



// ===========================================================================
// C++ Version
// ===========================================================================

#ifdef __cplusplus
#   define COREARRAY_CPP
#   if __cplusplus >= 201103L
#       define COREARRAY_CPP_V11
#   endif
#   if __cplusplus >= 201402L
#       define COREARRAY_CPP_V14
#   endif
#   if __cplusplus > 201402L
#       define COREARRAY_CPP_V17
#   endif
#endif



// ===========================================================================
// Noexcept Specifier
// ===========================================================================

#ifdef COREARRAY_CPP_V11
#   define COREARRAY_NOEXCEPT_TRUE     noexcept(true)
#   define COREARRAY_NOEXCEPT_FALSE    noexcept(false)
#else
#   define COREARRAY_NOEXCEPT_TRUE
#   define COREARRAY_NOEXCEPT_FALSE
#endif



// ===========================================================================
// Platform MACRO
// Reference: http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
// ===========================================================================

#ifdef COREARRAY_PREDEFINED_PLATFORM
#   undef COREARRAY_PREDEFINED_PLATFORM
#endif


// Unix-like system
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#   define COREARRAY_PLATFORM_UNIX
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// IBM AIX
#if defined(_AIX)
#   define COREARRAY_PLATFORM_IBMAIX
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// BSD
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#   define COREARRAY_PLATFORM_BSD
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// Hewlett-Packard HP-UX
#if defined(__hpux)
#   define COREARRAY_PLATFORM_HPUX
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// Linux
#if defined(linux) || defined(__linux) || defined(__linux__)
#   define COREARRAY_PLATFORM_LINUX
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// OSX, iOS, and Darwin
#if defined(__APPLE__) && defined(__MACH__)
#   define COREARRAY_PLATFORM_MACOS
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// Solaris
#if defined(sun) || defined(__sun)
#   define COREARRAY_PLATFORM_SUN
#   if defined(__SVR4) || defined(__svr4__)
#       define COREARRAY_PLATFORM_SOLARIS
#   else
#       define COREARRAY_PLATFORM_SUNOS
#   endif
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// Windows with Cygwin (POSIX)
#if defined(__CYGWIN__) && !defined(_WIN32)
#   define COREARRAY_PLATFORM_CYGWIN
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif

// Windows, Cygwin (non-POSIX), and MinGW （Microsoft Windows 32/64-bit)
#if defined(_WIN64)
#   define COREARRAY_PLATFORM_WINDOWS
#   define COREARRAY_PLATFORM_WIN64
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#elif defined(_WIN32)
#   define COREARRAY_PLATFORM_WINDOWS
#   define COREARRAY_PLATFORM_WIN32
#   ifndef COREARRAY_PREDEFINED_PLATFORM
#       define COREARRAY_PREDEFINED_PLATFORM
#   endif
#endif




// ===========================================================================
// MACRO for POSIX thread
// Reference: 
// ===========================================================================

#if defined(posix) || defined(_posix) || defined(__posix)
#   define COREARRAY_POSIX
#endif




// ===========================================================================
// MACRO for POSIX thread
// Reference: 
// ===========================================================================

#if defined(COREARRAY_PLATFORM_UNIX) || defined(COREARRAY_POSIX)
#   define COREARRAY_POSIX_THREAD
#endif




// ===========================================================================
// MACRO for 32-bit or 64-bit registers
// Reference: 
// ===========================================================================

#ifdef __LP64__
#   define COREARRAY_REGISTER_BIT64
#else
#   define COREARRAY_REGISTER_BIT32
#endif




// ===========================================================================
// Streaming SIMD Extensions
// Reference: http://en.wikipedia.org/wiki/Streaming_SIMD_Extensions
// ===========================================================================

#ifdef COREARRAY_PREDEFINED_SIMD
#   undef COREARRAY_PREDEFINED_SIMD
#endif
#
#ifdef __SSE__
#   define COREARRAY_SIMD_SSE
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __SSE2__
#   define COREARRAY_SIMD_SSE2
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __SSE3__
#    define COREARRAY_SIMD_SSE3
#    ifndef COREARRAY_PREDEFINED_SIMD
#        define COREARRAY_PREDEFINED_SIMD
#    endif
#endif
#
#ifdef __SSSE3__
#    define COREARRAY_SIMD_SSSE3
#    ifndef COREARRAY_PREDEFINED_SIMD
#        define COREARRAY_PREDEFINED_SIMD
#    endif
#endif
#
#ifdef __SSE4_1__
#   define COREARRAY_SIMD_SSE4_1
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __SSE4_2__
#   define COREARRAY_SIMD_SSE4_2
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __AVX__
#   define COREARRAY_SIMD_AVX
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __AVX2__
#   define COREARRAY_SIMD_AVX2
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __AVX512F__
#   define COREARRAY_SIMD_AVX512F
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __AVX512BW__
#   define COREARRAY_SIMD_AVX512BW
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __AVX512CD__
#   define COREARRAY_SIMD_AVX512CD
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __AVX512DQ__
#   define COREARRAY_SIMD_AVX512DQ
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __AVX512VL__
#   define COREARRAY_SIMD_AVX512VL
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif

#ifdef __FMA__
#   define COREARRAY_SIMD_FMA
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif
#
#ifdef __FMA4__
#   define COREARRAY_SIMD_FMA4
#   ifndef COREARRAY_PREDEFINED_SIMD
#       define COREARRAY_PREDEFINED_SIMD
#   endif
#endif

#ifdef __POPCNT__
#   define COREARRAY_POPCNT
#endif
#
#ifdef __LZCNT__
#   define COREARRAY_LZCNT
#endif


#ifdef COREARRAY_SIMD_ATTR_ALIGN
#   undef COREARRAY_SIMD_ATTR_ALIGN
#endif
#
#if defined(__AVX512F__)
#   define COREARRAY_SIMD_ATTR_ALIGN    __attribute__((aligned(64)))
#elif defined(__AVX__)
#   define COREARRAY_SIMD_ATTR_ALIGN    __attribute__((aligned(32)))
#elif defined(__SSE__)
#   define COREARRAY_SIMD_ATTR_ALIGN    __attribute__((aligned(16)))
#else
#   define COREARRAY_SIMD_ATTR_ALIGN
#endif


#ifdef COREARRAY_NO_SIMD
#   ifdef COREARRAY_PREDEFINED_SIMD
#       undef COREARRAY_PREDEFINED_SIMD
#       undef COREARRAY_SIMD_SSE
#       undef COREARRAY_SIMD_SSE2
#       undef COREARRAY_SIMD_SSE3
#       undef COREARRAY_SIMD_SSSE3
#       undef COREARRAY_SIMD_SSE4_1
#       undef COREARRAY_SIMD_SSE4_2
#       undef COREARRAY_SIMD_AVX
#       undef COREARRAY_SIMD_AVX2
#       undef COREARRAY_SIMD_AVX512F
#       undef COREARRAY_SIMD_AVX512BW
#       undef COREARRAY_SIMD_AVX512CD
#       undef COREARRAY_SIMD_AVX512DQ
#       undef COREARRAY_SIMD_AVX512VL
#       undef COREARRAY_SIMD_FMA
#       undef COREARRAY_SIMD_FMA4
#   endif
#endif




// ===========================================================================
// Detecting the endianness (byte order)
// Reference: http://man7.org/linux/man-pages/man3/endian.3.html
// Q: Windows?
// ===========================================================================

#if !defined(COREARRAY_ENDIAN_LITTLE) && !defined(COREARRAY_ENDIAN_BIG) && !defined(COREARRAY_ENDIAN_UNKNOWN)
#
#   ifdef COREARRAY_ENDIAN_LITTLE
#      undef COREARRAY_ENDIAN_LITTLE
#   endif
#   ifdef COREARRAY_ENDIAN_BIG
#      undef COREARRAY_ENDIAN_BIG
#   endif
#   ifdef COREARRAY_ENDIAN_UNKNOWN
#       undef COREARRAY_ENDIAN_UNKNOWN
#   endif
#
#   // detect platforms
#   if defined(COREARRAY_PLATFORM_UNIX)
#
#       if defined(COREARRAY_PLATFORM_MACOS)
#           if defined(__i386__) || defined(__x86_64__) || defined(__arm__)
#               define COREARRAY_ENDIAN_LITTLE
#           elif defined(__ppc__) || defined(__ppc64__)
#               define COREARRAY_ENDIAN_BIG
#           else
#               define COREARRAY_ENDIAN_UNKNOWN
#           endif
#       elif defined(COREARRAY_PLATFORM_SUN)
#           if defined(__i386) || defined(__x86_64) || defined(__amd64)
#               define COREARRAY_ENDIAN_LITTLE
#           elif defined(sparc) || defined(__sparc)
#               define COREARRAY_ENDIAN_BIG
#           else
#               define COREARRAY_ENDIAN_UNKNOWN
#           endif
#       else
#           include <endian.h>
#           ifdef BYTE_ORDER
#               if BYTE_ORDER == LITTLE_ENDIAN
#                   define COREARRAY_ENDIAN_LITTLE
#               elif BYTE_ORDER == BIG_ENDIAN
#                   define COREARRAY_ENDIAN_BIG
#               else
#                   define COREARRAY_ENDIAN_UNKNOWN
#               endif
#           else
#               if __BYTE_ORDER == __LITTLE_ENDIAN
#                   define COREARRAY_ENDIAN_LITTLE
#               elif __BYTE_ORDER == __BIG_ENDIAN
#                   define COREARRAY_ENDIAN_BIG
#               else
#                   define COREARRAY_ENDIAN_UNKNOWN
#               endif
#           endif
#       endif
#
#   elif defined(COREARRAY_PLATFORM_WINDOWS)
#       define COREARRAY_ENDIAN_LITTLE
#
#   else
#       define COREARRAY_ENDIAN_UNKNOWN
#
#   endif
#
#endif




// ===========================================================================
// Function attributes -- inline keyword
// Reference: http://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Inline.html
// ===========================================================================

#if !defined(COREARRAY_USING_R)
#
#   // the C99 keyword inline should be recognized by all compilers now.
#   define COREARRAY_HAVE_INLINE
#   define COREARRAY_INLINE    inline
#
#   // force-inline keyword
#   define COREARRAY_HAVE_FORCEINLINE
#
#   if defined(COREARRAY_CC_BORLAND) || defined(COREARRAY_CC_MSC)
#       define COREARRAY_FORCEINLINE    __forceinline
#   elif (defined(__GNUC__) && ((__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)))
#       define COREARRAY_FORCEINLINE    inline __attribute__((always_inline))
#   elif defined(COREARRAY_CC_SUNPRO)
#       define COREARRAY_FORCEINLINE	COREARRAY_INLINE
#       undef COREARRAY_HAVE_FORCEINLINE
#   else
#       define COREARRAY_FORCEINLINE	COREARRAY_INLINE
#       undef COREARRAY_HAVE_FORCEINLINE
#   endif
#
#endif

// if defined, set "COREARRAY_FORCEINLINE = COREARRAY_INLINE"
#ifdef COREARRAY_NO_FORCEINLINE
#   ifdef COREARRAY_HAVE_FORCEINLINE
#       undef COREARRAY_HAVE_FORCEINLINE
#   endif
#   undef COREARRAY_FORCEINLINE
#   define COREARRAY_FORCEINLINE    COREARRAY_INLINE
#endif

// every call inside this function will be inlined, if possible
#define COREARRAY_HAVE_FLATTEN
#
#if (!defined(COREARRAY_PLATFORM_MACOS) && defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1)))
#   define COREARRAY_FLATTEN    __attribute__((flatten))
#else
#   define COREARRAY_FLATTEN
#   undef COREARRAY_HAVE_FLATTEN
#endif

#ifdef COREARRAY_NO_FLATTEN
#   ifdef COREARRAY_HAVE_FLATTEN
#       undef COREARRAY_HAVE_FLATTEN
#   endif
#   undef COREARRAY_FLATTEN
#   define COREARRAY_FLATTEN
#endif




// ===========================================================================
// Visibility support
// Reference: https://gcc.gnu.org/wiki/Visibility
// ===========================================================================

#if !defined(COREARRAY_USING_R)
#
#   if defined(_WIN32) || defined(__CYGWIN__)
#       ifdef __GNUC__
#           define COREARRAY_DLL_EXPORT  __attribute__((dllexport))
#       else
#           define COREARRAY_DLL_EXPORT  __declspec(dllexport)
#       endif
#       define COREARRAY_DLL_LOCAL
#   else
#       if __GNUC__ >= 4
#           define COREARRAY_DLL_EXPORT  __attribute__((visibility("default")))
#           define COREARRAY_DLL_LOCAL   __attribute__((visibility("hidden")))
#       else
#           define COREARRAY_DLL_EXPORT
#           define COREARRAY_DLL_LOCAL
#       endif
#   endif
#
#   define COREARRAY_DLL_DEFAULT    COREARRAY_DLL_EXPORT
#
#endif




// ===========================================================================
// On the Intel 386, the fastcall attribute causes the compiler to pass the
//   first argument (if of integral type) in the register ECX and the second
//   argument (if of integral type) in the register EDX.
// Reference: https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Function-Attributes.html#Function-Attributes
// ===========================================================================

/// fastcall keyword
#define COREARRAY_HAVE_FASTCALL
#
#if defined(COREARRAY_CC_BORLAND) || defined(COREARRAY_CC_MSC)
#   define COREARRAY_FASTCALL    __fastcall
#elif defined(COREARRAY_CC_SUNPRO)
#   define COREARRAY_FASTCALL
#elif (defined(__GNUC__) && ((__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))) && (defined(__386__) || defined(__i386__))
#   define COREARRAY_FASTCALL    __attribute__((fastcall))
#else
#   define COREARRAY_FASTCALL
#   undef COREARRAY_HAVE_FASTCALL
#endif




// ===========================================================================
// On the Intel x86, it supports mixing legacy codes that run with a 4-byte
//   aligned stack with modern codes that keep a 16-byte stack for SSE
//   compatibility.
// Reference: https://gcc.gnu.org/onlinedocs/gcc-4.7.0/gcc/Function-Attributes.html#Function-Attributes
// Q: need to check whether the platform is Intel x86?
// TODO: need force_align_arg_pointer for AVX, AVX2? No document exists by now
// ===========================================================================

#ifdef COREARRAY_HAVE_CALL_ALIGN
#   undef COREARRAY_HAVE_CALL_ALIGN
#endif

#if defined(__GNUC__) && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__>1) && defined(__MINGW32__)
#   define COREARRAY_CALL_ALIGN    __attribute__((force_align_arg_pointer))
#   define COREARRAY_HAVE_CALL_ALIGN
#else
#   define COREARRAY_CALL_ALIGN
#endif




// ===========================================================================
// Packed attribute
// ===========================================================================

#ifdef COREARRAY_ATTR_PACKED
#   undef COREARRAY_ATTR_PACKED
#endif

#if defined(__GNUC__)
#   define COREARRAY_ATTR_PACKED    __attribute__((packed))
#else
#   define COREARRAY_ATTR_PACKED
#endif




// ===========================================================================
// Compiler optimization flags
// ===========================================================================

#if defined(COREARRAY_COMPILER_OPTIMIZE_FLAG) && !defined(COREARRAY_NO_COMPILER_OPTIMIZE)
#   if defined(__clang__) && !defined(__APPLE__)
#       pragma clang optimize on
#   endif
#   if defined(__GNUC__) && ((__GNUC__>4) || (__GNUC__==4 && __GNUC_MINOR__>=4))
#       if (COREARRAY_COMPILER_OPTIMIZE_FLAG == 0)
#           pragma GCC optimize("O0")
#       elif (COREARRAY_COMPILER_OPTIMIZE_FLAG == 1)
#           pragma GCC optimize("O1")
#       elif (COREARRAY_COMPILER_OPTIMIZE_FLAG == 2)
#           pragma GCC optimize("O2")
#       elif (COREARRAY_COMPILER_OPTIMIZE_FLAG == 3)
#           pragma GCC optimize("O3")
#       elif (COREARRAY_COMPILER_OPTIMIZE_FLAG == 4)
#           pragma GCC optimize("Ofast")
#       else
#           error "COREARRAY_COMPILER_OPTIMIZE_FLAG should be 0,1,2,3 or 4."
#       endif
#   endif
#endif




// ===========================================================================

// CoreArray library code control
#define COREARRAY_CODE_DEBUG
#define COREARRAY_CODE_WARNING


#endif /* _HEADER_COREARRAY_MACRO_ */
