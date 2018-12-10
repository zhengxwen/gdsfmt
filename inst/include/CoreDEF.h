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
// Copyright (C) 2007-2018    Xiuwen Zheng
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
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2018
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

// ACC
#ifdef _ACC_
#   define COREARRAY_CC_ACC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Altium MicroBlaze C
#if defined(__CMB__) || defined(__CHC__)
#   define COREARRAY_CC_ALTIM_MICROBLAZE
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Amsterdam Compiler Kit
#ifdef __ACK__
#   define COREARRAY_CC_AMSTERDAM
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Aztec C
#if defined(__AZTEC_C__) || defined(AZTEC_C)
#   define COREARRAY_CC_AZTEC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// CC65
#ifdef __CC65__
#   define COREARRAY_CC_6502
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Comeau C++
#ifdef __COMO__
#   define COREARRAY_CC_COMEAU
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Compaq C/C++
#if defined(__DECC) || defined(__DECC_VER)
#   define COREARRAY_CC_COMPAG
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Convex C
#ifdef __convexc__
#   define COREARRAY_CC_CONVEX
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Cray C
#ifdef _CRAYC
#   define COREARRAY_CC_CRAY
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Diab C/C++
#ifdef __DCC__
#   define COREARRAY_CC_DIAB
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// DICE C
#ifdef _DICE
#   define COREARRAY_CC_DICE
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Digital Mars
#ifdef __DMC__
#   define COREARRAY_CC_DIGITAL_MARS
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Dignus Systems/C++
#ifdef __SYSC__
#   define COREARRAY_CC_DIGNUS
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// DJGPP
#if defined(__DJGPP__) || defined(__GO32__)
#   define COREARRAY_CC_DJGPP
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// EDG C++ Frontend
#ifdef __EDG__
#   define COREARRAY_CC_EDG
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// EKOPath
#ifdef __PATHCC__
#   define COREARRAY_CC_EKOPATH
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Fujitsu C++
#ifdef __FCC_VERSION
#   define COREARRAY_CC_FUJITSU
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Green Hill C/C++
#ifdef __ghs__
#   define COREARRAY_CC_GHS
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

// ImageCraft C
#ifdef __IMAGECRAFT__
#   define COREARRAY_CC_IMAGECRAFT
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// KAI C++
#ifdef __KCC
#   define COREARRAY_CC_KAI
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

// LCC
#ifdef __LCC__
#   define COREARRAY_CC_LCC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// MetaWare High C/C++
#ifdef __HIGHC__
#   define COREARRAY_CC_MWHIGH
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Metrowerks CodeWarrior
#if defined(__MWERKS__) || defined(__CWCC__)
#   define COREARRAY_CC_MWERKS
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Microtec C/C++
#ifdef _MRI
#   define COREARRAY_CC_MICROTEC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Microway NDP C
#if defined(__NDPC__) || defined(__NDPX__)
#   define COREARRAY_CC_NDP
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// MIPSpro
#if defined(__sgi) || defined(sgi)
#   define COREARRAY_CC_MIPSPRO
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Miracle C
#ifdef MIRACLE
#   define COREARRAY_CC_MIRACLE
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// MPW C++
#if defined(__MRC__) || defined(MPW_C) || defined(MPW_CPLUS)
#   define COREARRAY_CC_MPW
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Norcroft C
#ifdef __CC_NORCROFT
#   define COREARRAY_CC_NORCROFT
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// NWCC
#ifdef __NWCC__
#   define COREARRAY_CC_NW
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

// Pacific C
#ifdef __PACIFIC__
#   define COREARRAY_CC_PACIFIC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Palm C/C++
#ifdef _PACC_VER
#   define COREARRAY_CC_PALM
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Pelles C
#ifdef __POCC__
#   define COREARRAY_CC_PELLES
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

// Renesas C/C++
#if defined(__RENESAS__) || defined(__HITACHI__)
#   define COREARRAY_CC_RENESAS
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

// SCO OpenServer
#ifdef _SCO_DS
#   define COREARRAY_CC_SCODS
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

// SN Compiler
#ifdef __SNC__
#   define COREARRAY_CC_SN
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Stratus VOS C
#ifdef __VOSC__
#   define COREARRAY_CC_VOS
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Symantec C++
#ifdef __SC__
#   define COREARRAY_CC_SYMANTEC
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// TenDRA C/C++
#ifdef __TenDRA__
#   define COREARRAY_CC_TENDRA
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

// THINK C
#if defined(THINKC3) || defined(THINKC4)
#   define COREARRAY_CC_THINK
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

// Turbo C/C++
#ifdef __TURBOC__
#   define COREARRAY_CC_TURBO
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Ultimate C/C++
#ifdef _UCC
#   define COREARRAY_CC_ULTIMATE
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// USL C
#if defined(__USLC__) || defined(__SCO_VERSION__)
#   define COREARRAY_CC_USL
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// VBCC
#ifdef __VBCC__
#   define COREARRAY_CC_VB
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Watcom C++
#ifdef __WATCOMC__
#   define COREARRAY_CC_WATCOM
#   ifndef COREARRAY_PREDEFINED_CC_COMPILER
#       define COREARRAY_PREDEFINED_CC_COMPILER
#   endif
#endif

// Zortech C++
#ifdef __ZTC__
#   define COREARRAY_CC_ZORTECH
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

#ifdef __LZCNT__
#   define COREARRAY_LZCNT
#endif


#ifdef COREARRAY_SIMD_ATTR_ALIGN
#   undef COREARRAY_SIMD_ATTR_ALIGN
#endif
#
#if defined(__AVX__)
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

// if defined, set "COREARRAY_FORCEINLINE = COREARRAY_INLINE"
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




// CoreArray library code control
#define COREARRAY_CODE_DEBUG
#define COREARRAY_CODE_WARNING


#endif /* _HEADER_COREARRAY_MACRO_ */
