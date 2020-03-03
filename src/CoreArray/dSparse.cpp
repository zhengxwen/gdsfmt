// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dSparse.cpp: Sparse array in GDS format
//
// Copyright (C) 2020    Xiuwen Zheng
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

#ifndef COREARRAY_COMPILER_OPTIMIZE_FLAG
#   define COREARRAY_COMPILER_OPTIMIZE_FLAG  3
#endif

#include "dSparse.h"


namespace CoreArray
{
	template<typename TClass> static CdObjRef *OnObjCreate()
	{
		return new TClass();
	}

	COREARRAY_DLL_LOCAL void RegisterClass_Sparse()
	{
		#define REG_CLASS(T, CLASS, CType, Desp)	\
			dObjManager().AddClass(TdTraits< T >::StreamName(), \
				OnObjCreate< CLASS >, CdObjClassMgr::CType, Desp)

		// integers
		REG_CLASS(TSpInt8,  CdSparseInt8,  ctArray, "sparse signed integer of 8 bits");
		REG_CLASS(TSpInt16, CdSparseInt16, ctArray, "sparse signed integer of 16 bits");
		REG_CLASS(TSpInt32, CdSparseInt32, ctArray, "sparse signed integer of 32 bits");
		REG_CLASS(TSpInt64, CdSparseInt64, ctArray, "sparse signed integer of 64 bits");
		REG_CLASS(TSpUInt8,  CdSparseUInt8,  ctArray, "sparse unsigned integer of 8 bits");
		REG_CLASS(TSpUInt16, CdSparseUInt16, ctArray, "sparse unsigned integer of 16 bits");
		REG_CLASS(TSpUInt32, CdSparseUInt32, ctArray, "sparse unsigned integer of 32 bits");
		REG_CLASS(TSpUInt64, CdSparseUInt64, ctArray, "sparse unsigned integer of 64 bits");

		// real numbers
		REG_CLASS(TSpReal32, CdSparseReal32, ctArray, "sparse real number (32 bits)");
		REG_CLASS(TSpReal64, CdSparseReal64, ctArray, "sparse real number (64 bits)");

		#undef REG_CLASS
	}


	/// get whether it is a sparse array or not
	bool IsSparseArray(CdGDSObj *Obj)
	{
		if (dynamic_cast<CdContainer*>(Obj))
		{
			CdContainer *p = static_cast<CdContainer*>(Obj);
			int tr = p->TraitFlag();
			return (tr == COREARRAY_TR_SPARSE_INTEGER) ||
				(tr == COREARRAY_TR_SPARSE_REAL);
		} else
			return false;
	}
}
