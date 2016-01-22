// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dSerial.h: Serialization between class objects and stream data
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
 *	\file     dSerial.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2016
 *	\brief    Serialization between class objects and stream data
 *	\details
**/


#ifndef _HEADER_COREARRAY_SERIAL_
#define _HEADER_COREARRAY_SERIAL_

#include "dBase.h"


namespace CoreArray
{
	// =====================================================================
	// Serialization -- Type ID
	// =====================================================================

	/// Type ID used in CdSerialization
	/** Users should not change the values or order. **/
	enum TdSerialTypeID
	{
		osUnknown    = 0,  ///< Unknown type id

		// Record
		osRecord     = 1,  ///< Long record block (unlimited size)
		osShortRec   = 2,  ///< Short record block (size < 256 bytes)
		osNameSpace  = 3,  ///< Name space block

		// Integer
		osInt8       = 4,  ///< Signed integer of 8 bits
		osUInt8      = 5,  ///< Unsigned integer of 8 bits
		osInt16      = 6,  ///< Signed integer of 16 bits
		osUInt16     = 7,  ///< Unsigned integer of 16 bits
		osInt32      = 8,  ///< Signed integer of 32 bits
		osUInt32     = 9,  ///< Unsigned integer of 32 bits
		osInt64      = 10, ///< Signed integer of 64 bits
		osUInt64     = 11, ///< Unsigned integer of 64 bits

		// be prepared for future use
		// osInt128     = 12, ///< Signed integer of 128 bits
		// osUInt128    = 13, ///< Unsigned integer of 128 bits

		os16Packed   = 14, ///< Unsigned integer of 16 bits, packed in stream
		os32Packed   = 15, ///< Unsigned integer of 32 bits, packed in stream
		os64Packed   = 16, ///< Unsigned integer of 64 bits, packed in stream

		// be prepared for future use
		// os128Packed  = 17, ///< Unsigned integer of 128 bits, packed in stream

		// Float
		osFloat32    = 18, ///< Float number of single precision (32 bits)
		osFloat64    = 19, ///< Float number of double precision (64 bits)

		// be prepared for future use
		// osFloat128   = 20, ///< Float number of quadruple precision (128 bits)

		// String
		osStrUTF8    = 21, ///< UTF-8 string
		osStrUTF16   = 22, ///< UTF-16 string
		osStrUTF32   = 23, ///< UTF-32 string

		// others
		osGDSPos     = 24  ///< TdGDSPos
	};



	// =====================================================================
	// Serialization -- Root class
	// =====================================================================

	// pre-defined class
	class CdObjClassMgr;

	/// Exception for CoreArray serialization object
	class COREARRAY_DLL_EXPORT ErrSerial: public ErrCoreArray
	{
	public:
		ErrSerial(): ErrCoreArray()
			{ }
		ErrSerial(const char *fmt, ...): ErrCoreArray()
			{ _COREARRAY_ERRMACRO_(fmt); }
		ErrSerial(const std::string &msg): ErrCoreArray()
			{ fMessage = msg; }
	};


	/// Serialization between a class object and stream data
	/** CdSerial provides functions for loading and saving an object from or to
	 *  stream data, which supports elementary types (e.g integer, float) and
	 *  complex structure.
	 *  The term "serialization" means the reversible deconstruction of a set
	 *  of C++ data structures to a sequence of bytes, and then reconstituting
	 *  an equivalent structure in another program context.
	**/
	class COREARRAY_DLL_DEFAULT CdSerialization: public CdAbstract
	{
	public:
		/// Constructor
		/** \param vBufStream    a stream buffer
		 *  \param vLog          a log object, if NULL a new log object is created
		 *  \param vClassMgr     specify the class manager, if NULL then use the default manager
		**/
		CdSerialization(CdBufStream *vBufStream, CdLogRecord *vLog,
			CdObjClassMgr *vClassMgr=NULL);
		/// Constructor
		/** \param vStream       a stream object
		 *  \param vLog          a log object, if NULL a new log object is created
		 *  \param vClassMgr     specify the class manager, if NULL then use the default manager
		**/
		CdSerialization(CdStream *vStream, CdLogRecord *vLog,
			CdObjClassMgr *vClassMgr=NULL);
		/// Destructor
		virtual ~CdSerialization();

		/// get the buffer object with little endianness
		COREARRAY_FORCEINLINE BYTE_LE<CdBufStream> &Storage()
			{ return fStorage; }

		/// get the buffer object
		COREARRAY_FORCEINLINE CdBufStream &BufStream()
			{ return *fStorage.Stream; }

		/// get the stream object
		COREARRAY_FORCEINLINE CdStream &Stream()
			{ return *fStorage.Stream->Stream(); }

		/// get the log recorder
		COREARRAY_INLINE CdLogRecord &Log()
			{ return *fLog; }
		/// set log recorder
        void SetLog(CdLogRecord &vLog);

		/// get the class manager
		COREARRAY_INLINE CdObjClassMgr *ClassMgr() const
			{ return fClassMgr; }

		/// Return whether there is a name in the current namespace
		bool HaveProperty(const char *Name);
		/// Return type id of the variable with Name, if not exist then throw an exception
		TdSerialTypeID PropTypeID(const char *Name);
		/// Return the position of the variable with Name, if not exist then throw an exception
		SIZE64 PropPosition(const char *Name);
		/// Get the information of variable with Name, if not exist then throw an exception
		void PropInfo(const char *Name,
			TdSerialTypeID &TypeID, SIZE64 &Start, SIZE64 &Length);

	protected:
		/// a buffer object with little endianness
		BYTE_LE<CdBufStream> fStorage;
		/// the class manager
		CdObjClassMgr* fClassMgr;
		/// the log recorder
		CdLogRecord *fLog;

		/// variable information
		class TVariable
		{
		public:
			std::string Name;       ///< variable name
			TdSerialTypeID TypeID;  ///< type id
			SIZE64 Start;           ///< the starting position
			SIZE64 Length;          ///< the stream length
			TVariable *Next;        ///< next object in a list

			TVariable();
			virtual ~TVariable();
		};

		/// the collection of variables in a block
		class CVarList
		{
		public:
			TVariable *VarHead;  ///< the head of a list of variables
			TVariable *VarTail;  ///< the tail of a list of variables
			SIZE64 Start;   ///< the starting position in a stream
			SIZE64 Length;  ///< the block length in a stream
			int VarCount;   ///< the number of variables, or -1 indicating no name space
			CVarList *Next; ///< next object in a list

			CVarList();
			~CVarList();

			/// return a pointer to TVariable with Name, NULL if not exist
			TVariable* Name2Variable(const char *Name);
			/// remove variables in VarHead
			void ClearVarList();
		};

		/// a list of block collection
		CVarList *fStructListHead;

		/// create a new CVarList in fStructListHead
		CVarList &PushStruct();
		/// delete the top CVarList in fStructListHead
		void PopStruct();
		/// get the current variable structure (throw exception if fStructListHead==NULL)
		CVarList &CurrentStruct();
	};



	// =====================================================================
	// Serialization -- Writer
	// =====================================================================

	/// Serialization from a class object to stream data
	class COREARRAY_DLL_DEFAULT CdWriter: public CdSerialization
	{
	public:
		struct TdVar;
		friend struct TdVar;

		/// Constructor
		/** \param vBufStream    a stream buffer
		 *  \param vLog          a log object, if NULL a new log object is created
		 *  \param vClassMgr     specify the class manager, if NULL then use the default manager
		**/
		CdWriter(CdBufStream *vBufStream, CdLogRecord *vLog,
			CdObjClassMgr *vClassMgr=NULL);
		/// Constructor
		/** \param vStream       a stream object
		 *  \param vLog          a log object, if NULL a new log object is created
		 *  \param vClassMgr     specify the class manager, if NULL then use the default manager
		**/
		CdWriter(CdStream *vStream, CdLogRecord *vLog,
			CdObjClassMgr *vClassMgr=NULL);
		/// Destructor
		virtual ~CdWriter();

		/// Begin a block
		void BeginStruct();
		/// Begin a namespace (i.e., a block with name spaces)
		void BeginNameSpace();
		/// End a block or namespace
		void EndStruct();

		/// Write a class name
		void WriteClassName(const char *Name);

		/// Return a TdVar object providing stream operators
		TdVar &operator[] (const char *Name);

	protected:

		/// Write a property name
		void WritePropName(const char *Name);

		/// Add a new variable to the current structure
		TVariable* NewVar(const char *Name, TdSerialTypeID TypeID, SIZE64 Size);

	public:
        /// Connecting CdWriter with a variable (little endian)
		struct TdVar
		{
		public:
            friend class CdWriter;

			TdVar();

			// integer
			void operator << (C_Int8 val);
			void operator << (C_UInt8 val);
			void operator << (C_Int16 val);
			void operator << (C_UInt16 val);
			void operator << (C_Int32 val);
			void operator << (C_UInt32 val);
			void operator << (C_Int64 val);
			void operator << (C_UInt64 val);

			// floating point number
			void operator << (C_Float32 val);
			void operator << (C_Float64 val);

			// string
			void operator << (const UTF8String &val);
			void operator << (const UTF16String &val);
			void operator << (const UTF32String &val);

			// packed integer
			void Wp16b(C_UInt16 val);
			void Wp32b(C_UInt32 val);
			void Wp64b(C_UInt64 val);

			// new data space
			void NewStruct();
			void NewShortRec(void *Ptr, size_t Size);
			void NewShortRec(const C_Int32 *pval, size_t n);
			void NewShortRec(const C_Int64 *pval, size_t n);
			void NewNameSpace();

			// others
			void operator << (const TdGDSPos &val);
			
			// TODO
			void operator << (const TdGDSBlockID &val);

			///
			void NewAutoArray(const C_Int32 *pval, size_t n);
			void NewAutoArray(const C_Int64 *pval, size_t n);

		private:
			CdWriter *Obj;
			const char *Name;
		};

	private:
        TdVar fVar;
	};



	// =====================================================================
	// Serialization -- Reader
	// =====================================================================

	/// Serialization from a class object to stream data
	class COREARRAY_DLL_DEFAULT CdReader: public CdSerialization
	{
	public:
		struct TdVar;
		friend struct CdReader::TdVar;
		friend class CdObjClassMgr;

		/// Constructor
		/** \param vBufStream    a stream buffer
		 *  \param vLog          a log object, if NULL a new log object is created
		 *  \param vClassMgr     specify the class manager, if NULL then use the default manager
		**/
		CdReader(CdBufStream *vBufStream, CdLogRecord *vLog,
			CdObjClassMgr *vClassMgr=NULL);
		/// Constructor
		/** \param vStream       a stream object
		 *  \param vLog          a log object, if NULL a new log object is created
		 *  \param vClassMgr     specify the class manager, if NULL then use the default manager
		**/
		CdReader(CdStream *vStream, CdLogRecord *vLog,
			CdObjClassMgr *vClassMgr=NULL);
		/// Destructor
		virtual ~CdReader();

		/// begin a block
		SIZE64 BeginStruct();
		/// begin a namespace
		SIZE64 BeginNameSpace();
		/// end a block or namespace
		void EndStruct();

		/// Read a class name
		std::string ReadClassName();

		/// Return a TdVar object, which provides functions of variables
		TdVar &operator[] (const char *vName);

	protected:

		/// Variable with various types
		template<typename TYPE> class TVar: public TVariable
		{
		public:
			TYPE Data;
		};

		// short buffer with 255 bytes at most, used in osShortRec
		struct TShortRec
		{
			C_UInt8 Size;
			C_UInt8 Buffer[255];
		};

		/// Add a new variable to the current structure
		template<typename TYPE>
			TYPE & NewVar(string &Name, TdSerialTypeID TypeID)
		{
			CVarList &Cur = CurrentStruct();
			// no "Cur.VarCount ++" since 'Cur.VarCount' is fixed

			TVar<TYPE> *p = new TVar<TYPE>;
			p->Name = Name;
			p->TypeID = TypeID;
			p->Start = fStorage.Position();
			p->Length = 0;

			if (Cur.VarTail == NULL)
			{
				Cur.VarHead = Cur.VarTail = p;
			} else {
				Cur.VarTail->Next = p;
				Cur.VarTail = p;
			}

			return p->Data;
		}

		/// begin the name space without initializing name space
		SIZE64 _BeginNameSpace();
		/// Initialize variables in the name space
		void _InitNameSpace();

		/// Read a property name
		std::string ReadPropName();

		/// find variable with Name
		TVariable* FindVar(const char *Name);

	public:
        /// Connecting CdReader with a variable (little endian)
		struct TdVar
		{
		public:
            friend class CdReader;

			TdVar();

			// integer
			void operator >> (C_Int8 &val);
			void operator >> (C_UInt8 &val);
			void operator >> (C_Int16 &val);
			void operator >> (C_UInt16 &val);
			void operator >> (C_Int32 &val);
			void operator >> (C_UInt32 &val);
			void operator >> (C_Int64 &val);
			void operator >> (C_UInt64 &val);

			// floating point number
			void operator >> (C_Float32 &val);
			void operator >> (C_Float64 &val);

			// string
			void operator >> (UTF8String &val);
			void operator >> (UTF16String &val);
			void operator >> (UTF32String &val);

			void BeginStruct();
			void GetShortRec(void* &OutPtr, size_t &Size);
			void GetShortRec(C_Int32 *pval, size_t n);
			void GetShortRec(C_Int64 *pval, size_t n);
			void BeginNameSpace();

			// others
			void operator >> (TdGDSPos &val);
			void operator >> (TdGDSBlockID &val);

			///
			void GetAutoArray(C_Int32 *pval, ssize_t n);
			void GetAutoArray(C_Int64 *pval, ssize_t n);

		private:
			CdReader *Obj;
			const char *Name;
		};

	private:
		TdVar fVar;

		// assign numeric value
		template<typename TYPE> static TYPE Cvt_Num(TVariable* Var)
		{
			switch (Var->TypeID)
			{
			case osInt8:
				return (TYPE)(static_cast<TVar<C_Int8>*>(Var)->Data);
			case osUInt8:
				return (TYPE)(static_cast<TVar<C_UInt8>*>(Var)->Data);
			case osInt16:
				return (TYPE)(static_cast<TVar<C_Int16>*>(Var)->Data);
			case osUInt16: case os16Packed:
				return (TYPE)(static_cast<TVar<C_UInt16>*>(Var)->Data);
			case osInt32:
				return (TYPE)(static_cast<TVar<C_Int32>*>(Var)->Data);
			case osUInt32: case os32Packed:
				return (TYPE)(static_cast<TVar<C_UInt32>*>(Var)->Data);
			case osInt64:
				return (TYPE)(static_cast<TVar<C_Int64>*>(Var)->Data);
			case osUInt64: case os64Packed:
				return (TYPE)(static_cast<TVar<C_UInt64>*>(Var)->Data);
			case osFloat32:
				return (TYPE)(static_cast<TVar<C_Float32>*>(Var)->Data);
			case osFloat64:
				return (TYPE)(static_cast<TVar<C_Float64>*>(Var)->Data);
			default:
				throw ErrSerial("CdReader: '%s' is not numeric.",
					Var->Name.c_str());
			}
		}
	};



	// =====================================================================
	// Serialization -- Class manager
	// =====================================================================

	/// CoreArray class manager
	class COREARRAY_DLL_DEFAULT CdObjClassMgr: public CdAbstractManager
	{
	public:
		typedef CdObjRef* (*TdOnObjCreate)();
		typedef void (*TdInit)(CdObjClassMgr &Sender, CdObject *dObj, void *Data);
		typedef void (CdObject::*TdInitEx)(CdObjClassMgr &Sender, CdObject *dObj, void *Data);

		struct _strCmp
		{
			bool operator()(const char* s1, const char* s2) const;
		};

		/// The registered class type
		enum ClassType
		{
			ctCustom = 0,  ///< user-defined
			ctArray,       ///< array-oriented data
			ctRefArray,    ///< reference
			ctStream       ///< stream
		};

		struct _ClassStruct
		{
			TdOnObjCreate OnCreate;
			const char *Desp;
			ClassType CType;
		};

		/// Constructor
		CdObjClassMgr();
		/// Destructor
		virtual ~CdObjClassMgr();

		/// Register a class
		/** \param ClassName  the name of class
		 *  \param OnCreate   a function of creator
		 *  \param vCType     the type of class
		 *  \param Desp       the description
		**/
		void AddClass(const char *ClassName, TdOnObjCreate OnCreate,
			ClassType vCType, const char *Desp="");
		/// Unregister a class
		void RemoveClass(const char *ClassName);
		/// Unregister all classes
		void Clear();

		/// Return a function corresponding to the class name
		virtual TdOnObjCreate NameToClass(const char *ClassName);
		/// Create an object from a filter
		/** \param Reader  a filter
		 *  \param OnInit  a function of initialization of the object
		 *  \param Data    be passed to OnInit
		**/
		virtual CdObjRef* ToObj(CdReader &Reader, TdInit OnInit,
			void *Data, bool Silent);

		const _ClassStruct &ClassStruct(const char *ClassName) const;
		COREARRAY_INLINE const std::map<const char *, _ClassStruct, _strCmp> &
			ClassMap() const { return fClassMap; }

		void ClassList(vector<string> &Key, vector<string> &Desp);

	protected:
		std::map<const char *, _ClassStruct, _strCmp> fClassMap;
	};


	/// The global manager of CoreArray classes
	COREARRAY_DLL_DEFAULT CdObjClassMgr &dObjManager();

}

#endif /* _HEADER_COREARRAY_SERIAL_ */
