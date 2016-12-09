// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// CoreArray.h: CoreArray interface
//
// Copyright (C) 2007-2016    Xiuwen Zheng
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
 *	\file     CoreArray.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2016
 *	\brief    CoreArray interface
 *	\details
**/


#ifndef _HEADER_COREARRAY_
#define _HEADER_COREARRAY_

#include "CoreDEF.h"
#include "dType.h"
#include "dTrait.h"
#include "dBit.h"
#include "dPlatform.h"
#include "dParallel.h"
#include "dStream.h"
#include "dEndian.h"
#include "dSerial.h"
#include "dBase.h"
#include "dFile.h"
#include "dStruct.h"
#include "dRealGDS.h"
#include "dBitGDS.h"
#include "dStrGDS.h"
// #include "dVLIntGDS.h"


namespace CoreArray
{
	/// Register CoreArray classes
	COREARRAY_DLL_DEFAULT void RegisterClass();
}

#endif /* _HEADER_COREARRAY_ */
