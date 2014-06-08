// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dBase.h: Fundamental classes for CoreArray library
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
 *	\file     dBase.h
 *	\author   Xiuwen Zheng
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Basic classes for CoreArray library
 *	\details
**/


#ifndef _dBase_H_
#define _dBase_H_

#include <dPlatform.h>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <limits>
#include <exception>

namespace CoreArray
{
	/// A macro of CoreArray library version
	/** You can use COREARRAY_VERSION in $if expressions to test the runtime
	 *  library version level independently of the compiler version level.
	 *  For example:  #if (COREARRAY_VERSION >= 0x0100) ... #endif
	**/
	#define COREARRAY_VERSION        0x0100

	/// A string of CoreArray library version
	#define COREARRAY_VERSION_STR    "v1.00 (release)"

	/// A macro of CoreArray class version
	#define COREARRAY_CLASS_VERSION  0x0100


	/// Default buffer size in TBufdStream, 4K
	const ssize_t DefaultBufSize      = 4*1024;
	/// Default large size of buffer in TBufdStream, 128K
	const ssize_t DefaultBufLargeSize = 128*1024;
	/// Default small size of buffer in TBufdStream, 1K
	const ssize_t DefaultBufSmallSize = 1024;

	/// Aligned bytes of stream buffer
	const size_t BufStreamAlign = 4;  // 2^4 = 16 bytes aligned


	/// Class or file version
	/** The first byte is major version ID, and the second byte is
	 *  minor version ID.
	**/
	typedef UInt16 TdVersion;

	/// Type ID used in CdSerial
	/** Users should not change the values or order. **/
	enum TdTypeID
	{
		osUnknown    = 0,  ///< Unknown type id

		// Record
		osRecord     = 1,  ///< Long record block
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

		#ifndef COREARRAY_NO_EXTENDED_TYPES
		osInt128     = 12, ///< Signed integer of 128 bits
		osUInt128    = 13, ///< Unsigned integer of 128 bits
		#endif

		os16Packed   = 14, ///< Unsigned integer of 16 bits, packed in stream
		os32Packed   = 15, ///< Unsigned integer of 32 bits, packed in stream
		os64Packed   = 16, ///< Unsigned integer of 64 bits, packed in stream

		#ifndef COREARRAY_NO_EXTENDED_TYPES
		os128Packed  = 17, ///< Unsigned integer of 128 bits, packed in stream
		#endif

		// Float
		osFloat32    = 18, ///< Float number of single precision (32 bits)
		osFloat64    = 19, ///< Float number of double precision (64 bits)

		#ifndef COREARRAY_NO_EXTENDED_TYPES
		osFloat128   = 20, ///< Float number of quadruple precision (128 bits)
		#endif

		// String
		osStrUTF8    = 21, ///< UTF-8 string
		osStrUTF16   = 22, ///< UTF-16 string
		osStrUTF32   = 23, ///< UTF-32 string

		// others
		osStreamPos  = 24  ///< Integer of stream position, 6 bytes
	};

	/// Validate TdTypeID
	#define COREARRAY_VALID_TYPEID(x) ((osUnknown<(x)) && ((x)<=osStreamPos))


	// predefined classes

	class CdAbstract;
	class CdObject;
	class CdSerial;
	class CdObjClassMgr;

	namespace _Internal_
	{
		/// Access CdObject::LoadStruct
		void CdObject_LoadStruct(CdObject &Obj, CdSerial &Reader, TdVersion Version);
		/// Access CdObject::SaveStruct
		void CdObject_SaveStruct(CdObject &Obj, CdSerial &Writer, bool IncludeName);
	}


	/// The abstract root class for all CoreArray classes
	class CdAbstract {};


	/// The root class for CoreArray object
	/** CdObject contains a sub-system of class serialization, which allows to
	 *  load and save its data to a stream.
	**/
	class CdObject: public CdAbstract
	{
	public:
		/// Access CdObject::LoadStruct
		friend void _Internal_::CdObject_LoadStruct(CdObject &Obj,
			CdSerial &Reader, TdVersion Version);
		/// Access CdObject::SaveStruct
		friend void _Internal_::CdObject_SaveStruct(CdObject &Obj,
			CdSerial &Writer, bool IncludeName);

		/// Constructor, do nothing
		CdObject() {}
		/// Destructor, do nothing
		virtual ~CdObject() {}

		/// Return a string specifying the class name in stream
		virtual char const* dName();
		/// Return a string specifying the class name
		virtual char const* dTraitName();

		/// Return version of the class
		virtual TdVersion dVersion();
		/// Return version of the class, saved in stream
		virtual TdVersion SaveVersion();

	protected:
		// Load functions
		/// Call ::LoadBefore, ::Loading, ::LoadAfter
		void LoadStruct(CdSerial &Reader, TdVersion Version);

		/// The method called before serialization
		/** \param Reader
		 *  \param Version
		**/
		virtual void LoadBefore(CdSerial &Reader, TdVersion Version);

		/// The method of serializing the object
		/** \param 
		 *  \param 
		**/
		virtual void Loading(CdSerial &Reader, TdVersion Version);

		/// The method called after serialization
		/** \param Reader
		 *  \param Version
		**/
		virtual void LoadAfter(CdSerial &Reader, TdVersion Version);

		// Save functions
		/// Prepare Writer, and call ::SaveBefore, ::Saving, ::SaveAfter
		virtual void SaveStruct(CdSerial &Writer, bool IncludeName);
		virtual void SaveBefore(CdSerial &Writer);
		virtual void Saving(CdSerial &Writer);
		virtual void SaveAfter(CdSerial &Writer);
	private:
		CdObject& operator= (const CdObject& m);
		CdObject& operator= (CdObject& m);
	};


	/// The abstract root of class manager 
	class CdAbstractManager: public CdAbstract {};

	/// The abstract root class of item for CdAbstractManager
	class CdAbstractItem: public CdAbstract {};


	/// A notification object
	template<class TSender=CdObject> struct TdOnNotify
	{
	public:
		COREARRAY_INLINE TdOnNotify() { Clear(); }
		COREARRAY_INLINE operator bool() const { return (Event!=NULL); }
		COREARRAY_INLINE void Clear() { Obj = NULL; Event = NULL; }
		COREARRAY_INLINE void Notify(TSender *Sender)
			{ if (this) if (Event) (Obj->*Event)(Sender); }
		template<class T> COREARRAY_INLINE TdOnNotify &Set( T *const vObj,
				void (T::*vEvent)(TSender*) )
			{ Obj = (CdObject*)vObj; Event = (TdOnDo)vEvent; return *this; }

	private:
		typedef void (CdObject::*TdOnDo)(TSender *);
		CdObject * Obj;
		TdOnDo Event;
	};


	// CdRef

	class CdRef;

	namespace _Internal_
	{
		/// Increase the reference of Obj immediately, without any checking
		void DirectAddRef(CdRef &Obj);
		/// Decrease the reference of Obj immediately, without any checking
		void DirectDecRef(CdRef &Obj);
	}

	/// The root class of the reference object
	/** A reference counter is added to the class. **/
	class CdRef
	{
	public:
		/// Increase the reference of Obj immediately, without any checking
		friend void _Internal_::DirectAddRef(CdRef &Obj);
		/// Decrease the reference of Obj immediately, without any checking
		friend void _Internal_::DirectDecRef(CdRef &Obj);

		/// Constructor, setting Reference = 0
		CdRef();
		/// Destructor, do nothing
		virtual ~CdRef();

		/// Increase the reference count
		void AddRef();
		/// Decrease the reference count, if the count is ZERO, free the object
		ssize_t Release();

		/// Return the count
		COREARRAY_INLINE ssize_t Reference() const { return fReference; }

	private:
		ssize_t fReference;
	};


	// TdObjRef

	/// Combination of CdObject and CdRef
	/** Allow CoreArray object to have a reference counter. **/
	class CdObjRef: public CdRef, public CdObject { };


	template<class T> struct TdAutoRef
	{
	public:
		COREARRAY_INLINE TdAutoRef() { fobj = NULL; }
		COREARRAY_INLINE TdAutoRef(T *vObj) { (fobj = vObj)->AddRef(); }
		COREARRAY_INLINE ~TdAutoRef() { fobj->Release(); }

		COREARRAY_INLINE T * get()
			{ return fobj; }
		COREARRAY_INLINE T* operator=(T *_Right)
		{
			if (_Right != fobj)
			{
				if (fobj) fobj->Release();
				fobj = _Right->AddRef();
			}
			return _Right;
		}
		COREARRAY_INLINE T &operator*() const
			{ return *fobj; }
		COREARRAY_INLINE T *operator->() const
			{ return fobj; }
		COREARRAY_INLINE bool operator==(const T *_Right)
			{ return fobj==_Right; }
		COREARRAY_INLINE bool operator==(const TdAutoRef<T> &_Right)
			{ return fobj==_Right.fobj; }
		COREARRAY_INLINE bool operator!=(const T *_Right)
			{ return fobj!=_Right; }
		COREARRAY_INLINE bool operator!=(const TdAutoRef<T> &_Right)
			{ return fobj!=_Right.fobj; }
	private:
		T *fobj;
	};


	template<class T> struct TdTryFinal
	{
	public:
		TdTryFinal(T &val, T begin, T end)
			{ _val = &val; *_val = begin; _end = end; }
		~TdTryFinal() { *_val = _end; }
	private:
		T *_val;
        T _end;
	};


	// CdObjMsg

	class CdObjMsg;

	/// A broadcast structure
	struct TdOnBroadcast
	{
	public:
		COREARRAY_INLINE TdOnBroadcast() { Obj = NULL; Event = NULL; }

		COREARRAY_INLINE void Clear() { Obj = NULL; Event = NULL; }
		template<class T> COREARRAY_INLINE TdOnBroadcast &Set(T *const vObj,
				void (T::*vEvent)(CdObjMsg*, Int32, void*))
			{ Obj = (CdObjMsg*)vObj; Event = (TdOnDo)vEvent; return *this; }

		void Notify(CdObjMsg *Sender, Int32 MsgCode, void *Param);

		COREARRAY_INLINE operator bool() const { return (Event!=NULL); }
		COREARRAY_INLINE bool operator== (const TdOnBroadcast &v) const
			{ return ((Obj==v.Obj) && (Event==v.Event)); }
		COREARRAY_INLINE bool operator!= (const TdOnBroadcast &v) const
			{ return ((Obj!=v.Obj) || (Event!=v.Event)); }
	private:
		typedef void (CdObjMsg::*TdOnDo)(CdObjMsg *, Int32, void *);
		CdObjMsg * Obj;
		TdOnDo Event;
	};


	/// The CoreArray class with broadcast ability
	/** The class allow users to add multiple broadcast event handlers. **/
	class CdObjMsg: public CdObjRef
	{
	public:
		/// Constructor
		CdObjMsg();
		/// Destructor
		virtual ~CdObjMsg();

		/// Add a message receiver
		void AddMsg(const TdOnBroadcast &MsgObj);
		/// Add a message receiver, template
		template<class T> COREARRAY_INLINE void AddMsgEx(T *const vObj,
			void (T::*vEvent)(CdObjMsg*, Int32, void*))
		{
			TdOnBroadcast Do;
			Do.Set<T>(vObj, vEvent);
			AddMsg(Do);
		}

		/// Remove a message receiver
		void RemoveMsg(const TdOnBroadcast &MsgObj);
		/// Remove a message receiver, template
		template<class T> COREARRAY_INLINE void RemoveMsgEx(T *const vObj,
			void (T::*vEvent)(CdObjMsg*, Int32, void*))
		{
			TdOnBroadcast Do;
			Do.Set<T>(vObj, vEvent);
			RemoveMsg(Do);
		}

		/// Begin to block all messages
		void BeginMsg();
		/// End to block all messages
		/** If any message has been blocked after calling ::BeginMsg(), then
         *  a NULL message (calling ::Notify(0, NULL)) will be sent out.
        **/
		void EndMsg();

		/// Broadcast a message
		void Notify(Int32 MsgCode, void *Param = NULL);

		/// Broadcast a message of Int32 value
		COREARRAY_INLINE void Notify32(Int32 MsgCode, const Int32 Value)
			{ Notify(MsgCode, (void*)&Value); }
		/// Broadcast a message of Int64 value
		COREARRAY_INLINE void Notify64(Int32 MsgCode, const Int64 Value)
			{ Notify(MsgCode, (void*)&Value); }
		/// Broadcast a message of an array of Int32 value
		COREARRAY_INLINE void Notify32(Int32 MsgCode, Int32 const *Param)
			{ Notify(MsgCode, (void*)Param); }
		/// Broadcast a message of an array of Int64 value
		COREARRAY_INLINE void Notify64(Int32 MsgCode, Int64 const *Param)
			{ Notify(MsgCode, (void*)Param); }

    	/// Return a vector of message receivers
		COREARRAY_INLINE const std::vector<TdOnBroadcast> &MsgList() const
			{ return fMsgList; }
	protected:
		/// Determine messages to be sent (if return true), or blocked
		virtual bool MsgFilter(Int32 MsgCode, void *Param);

	private:
		std::vector<TdOnBroadcast> fMsgList;
		int vMsgRef;
		bool vMsgFlag;
	};


	/// An object of message, automatically calling BeginMsg and EndMsg 
	struct TdAutoObjMsg
	{
	public:
		TdAutoObjMsg(CdObjMsg *vObj)
		{
			Obj = vObj;
			if (Obj) Obj->BeginMsg();
		}
		~TdAutoObjMsg()
		{
			if (Obj) Obj->EndMsg();
			Obj = NULL;
		}
	private:
		CdObjMsg *Obj;
	};


	/// Log record
	class CdLogRecord: public CdObjRef
	{
	public:
		static const int logCustom	=	-1; ///< user-customized
		static const int logInfo	=	0;  ///< information
		static const int logError	=	1;  ///< error
		static const int logWarn	=	2;  ///< warning
		static const int logHint	=	3;  ///< hint

		/// Record item
		struct TdItem
		{
			UTF8String Msg;  ///< the message
			Int32 Type;      ///< the type of message
			TdItem() { Type = logCustom; }
		};

		/// Add a message
		void Add(const char *const str, Int32 vType=logError);
		/// Add a message
		void Add(std::string &str, Int32 vType=logError);
		/// Add a message
		void Add(Int32 vType, const char *fmt, ...);

		/// Return a vector of TdItem (record item)
		COREARRAY_INLINE std::vector<TdItem> &List() { return fList; }
	protected:
		std::vector<TdItem> fList;

		virtual void LoadBefore(CdSerial &Reader, TdVersion Version);
		virtual void SaveBefore(CdSerial &Writer);
	};


	/// Pointer of 64 bits, used in CdStream and TdAllocator
	typedef Int64 TdPtr64;

	/// Type of stream position in CoreArray GDS format
	typedef TdNumber<Int64, 6> TdPosType;


	/// The root class of CoreArray stream
	/** CdStream provides basic functions of stream. **/
	class CdStream: public CdRef
	{
	public:
		/// Constructor
		CdStream();
		/// Destructor
		virtual ~CdStream();

		/// Read block of data, and return number of read in bytes
		virtual ssize_t Read(void *Buffer, ssize_t Count) = 0;
		/// Write block of data, and return number of write in bytes
		virtual ssize_t Write(void *const Buffer, ssize_t Count) = 0;

		virtual TdPtr64 Seek(const TdPtr64 Offset, TdSysSeekOrg Origin) = 0;
		virtual TdPtr64 GetSize();
		virtual void SetSize(const TdPtr64 NewSize) = 0;

		/// Read block of data, or throw an exception if fail
		void ReadBuffer(void *Buffer, ssize_t Count);
		/// Write block of data, or throw an exception if fail
		void WriteBuffer(void *const Buffer, ssize_t Count);
		/// Copy from a CdStream object
		TdPtr64 CopyFrom(CdStream &Source, TdPtr64 Count=-1);

		/// Return the current position
		COREARRAY_INLINE TdPtr64 Position()
			{ return Seek(0, soCurrent); }
		/// Reset the current position
		COREARRAY_INLINE void SetPosition(const TdPtr64 pos)
			{ Seek(pos, soBeginning); }

		/// Read an unsigned integer of 8 bits
		UInt8 rUInt8();
		/// Read an unsigned integer of 16 bits, taking endianness into account
		UInt16 rUInt16();
		/// Read an unsigned integer of 32 bits, taking endianness into account
		UInt32 rUInt32();
		/// Read an unsigned integer of 64 bits, taking endianness into account
		UInt64 rUInt64();
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Read an unsigned integer of 128 bits, taking endianness into account
		UInt128 rUInt128();
		#endif

		/// Read a float number of single precision
		Float32 rFloat32();
		/// Read a float number of double precision
		Float64 rFloat64();
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Read a float number of quadruple precision
		Float128 rFloat128();
		#endif

		/// Write an unsigned integer of 8 bits
		void wUInt8(UInt8 val);
		/// Write an unsigned integer of 16 bits, taking endianness into account
		void wUInt16(UInt16 val);
		/// Write an unsigned integer of 32 bits, taking endianness into account
		void wUInt32(UInt32 val);
		/// Write an unsigned integer of 64 bits, taking endianness into account
		void wUInt64(UInt64 val);
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Write an unsigned integer of 128 bits, taking endianness into account
		void wUInt128(UInt128 val);
		#endif

		/// Write a float number of single precision
		void wFloat32(const Float32 &val);
		/// Write a float number of double precision
		void wFloat64(const Float64 &val);
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Write a float number of quadruple precision
		void wFloat128(const Float128 &val);
		#endif

	private:
		CdStream& operator= (const CdStream& m);
		CdStream& operator= (CdStream& m);
	};


	/// an operator, to read a signed integer of 8 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Int8& out)
    	{ out = s.rUInt8(); return s; }
	/// an operator, to read an unsigned integer of 8 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, UInt8& out)
		{ out = s.rUInt8(); return s; }
	/// an operator, to read a signed integer of 16 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Int16& out)
		{ out = s.rUInt16(); return s; }
	/// an operator, to read an unsigned integer of 16 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, UInt16& out)
		{ out = s.rUInt16(); return s; }
	/// an operator, to read a signed integer of 32 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Int32& out)
		{ out = s.rUInt32(); return s; }
	/// an operator, to read an unsigned integer of 32 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, UInt32& out)
		{ out = s.rUInt32(); return s; }
	/// an operator, to read a signed integer of 64 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Int64& out)
		{ out = s.rUInt64(); return s; }
	/// an operator, to read an unsigned integer of 64 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, UInt64& out)
		{ out = s.rUInt64(); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to read a signed integer of 128 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Int128& out)
		{ out = s.rUInt128(); return s; }
	/// an operator, to read an unsigned integer of 128 bits from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, UInt128& out)
		{ out = s.rUInt128(); return s; }
	#endif
	/// an operator, to read a float number of single precision from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Float32& out)
		{ out = s.rFloat32(); return s; }
	/// an operator, to read a float number of double precision from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Float64& out)
		{ out = s.rFloat64(); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to read a float number of quadruple precision from a stream
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, Float128& out)
		{ out = s.rFloat128(); return s; }
	#endif
	/// an operator, to read TdPosType from a stream (6 bytes)
	COREARRAY_INLINE CdStream& operator>> (CdStream &s, TdPosType& out)
	{
		UInt64 L = 0;
		s.ReadBuffer((void*)&L, TdPosType::size);
	#if defined(COREARRAY_LITTLE_ENDIAN)
		out = L;
	#elif defined(COREARRAY_BIG_ENDIAN)
		out = COREARRAY_ENDIAN_CVT64(L);
	#else
	#  error "Unsupported Endianness!"
	#endif
		return s;
	}

	/// an operator, to write a signed integer of 8 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const Int8 in)
		{ s.wUInt8(in); return s; }
	/// an operator, to write an unsigned integer of 8 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const UInt8 in)
		{ s.wUInt8(in); return s; }
	/// an operator, to write a signed integer of 16 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const Int16 in)
		{ s.wUInt16(in); return s; }
	/// an operator, to write an unsigned integer of 16 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const UInt16 in)
		{ s.wUInt16(in); return s; }
	/// an operator, to write a signed integer of 32 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const Int32 in)
		{ s.wUInt32(in); return s; }
	/// an operator, to write an unsigned integer of 32 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const UInt32 in)
		{ s.wUInt32(in); return s; }
	/// an operator, to write a signed integer of 64 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const Int64 in)
		{ s.wUInt64(in); return s; }
	/// an operator, to write an unsigned integer of 64 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const UInt64 in)
		{ s.wUInt64(in); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to write a signed integer of 128 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, Int128 in)
		{ s.wUInt128(UInt128(in)); return s; }
	/// an operator, to write an unsigned integer of 128 bits to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, UInt128 in)
		{ s.wUInt128(in); return s; }
	#endif
	/// an operator, to write a float number of single precision to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const Float32 &in)
		{ s.wFloat32(in); return s; }
	/// an operator, to write a float number of double precision to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const Float64 &in)
		{ s.wFloat64(in); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to write a float number of quadruple precision to a stream
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const Float128 &in)
		{ s.wFloat128(in); return s; }
	#endif
	/// an operator, to write TdPosType to a stream (6 bytes)
	COREARRAY_INLINE CdStream& operator<< (CdStream &s, const TdPosType &in)
	{
	#if defined(COREARRAY_LITTLE_ENDIAN)
		UInt64 L = in;
	#elif defined(COREARRAY_BIG_ENDIAN)
		UInt64 L = COREARRAY_ENDIAN_CVT64(in);
	#else
	#  error "Unsupported Endianness!"
	#endif
		s.WriteBuffer((void*)&L, TdPosType::size);
		return s;
	}


	// CBufdStream

	class CBufdStream;

	/// The root class of stream pipe
	class CdStreamPipe: public CdAbstractItem
	{
	public:
    	CdStreamPipe() {}
		virtual ~CdStreamPipe() {}
		friend class CBufdStream;
	protected:
		virtual CdStream* InitPipe(CBufdStream *Filter) = 0;
		virtual CdStream* FreePipe() = 0;
	};

	/// The class adds a buffer to a stream
	class CBufdStream: public CdRef
	{
	public:
		struct TRWBit
		{
			unsigned char Value;
			unsigned char Offset;
		};

		friend class CdObjClassMgr;
		friend class CdObject;

		/// Constructor
		/** \param vStream    an stream (it can be NULL)
		 *  \param vBufSize   buffer size
		**/
		CBufdStream(CdStream* vStream, ssize_t vBufSize = DefaultBufSize);
		/// Destructor
		virtual ~CBufdStream();

		virtual CBufdStream* NewFilter();
		void RefreshStream();
		void FlushBuffer();
		void FlushWrite();

		/// Read block of data, or throw an exception if fail
		void Read(void *Buf, ssize_t Count);
		/// Write block of data, or throw an exception if fail
		void Write(void const* Buf, ssize_t Count);
		/// Copy from a CBufdStream object
		void CopyFrom(CBufdStream &Source, TdPtr64 Count=-1);
		/// Truncate the stream
		void Truncate();

		/// Read the next character, or -1 if fail
		int rByteL();
		/// Read and return the next character without extracting it
		int Peek();

		/// Read an unsigned integer of 8 bits
		UInt8 rUInt8();
		/// Read an unsigned integer of 16 bits, taking endianness into account
		UInt16 rUInt16();
		/// Read an unsigned integer of 32 bits, taking endianness into account
		UInt32 rUInt32();
		/// Read an unsigned integer of 64 bits, taking endianness into account
		UInt64 rUInt64();
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Read an unsigned integer of 128 bits, taking endianness into account
		UInt128 rUInt128();
		#endif
		/// Read a packed unsigned integer of 16 bits, taking endianness into account
		UInt16 rPack16u();
		/// Read a packed unsigned integer of 32 bits, taking endianness into account
		UInt32 rPack32u();
		/// Read a packed unsigned integer of 64 bits, taking endianness into account
		UInt64 rPack64u();
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Read a packed unsigned integer of 128 bits, taking endianness into account
		UInt128 rPack128u();
		#endif
		/// Read a float number of single precision
		Float32 rFloat32();
		/// Read a float number of double precision
		Float64 rFloat64();
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Read a float number of quadruple precision
		Float128 rFloat128();
		#endif
		/// Read a UTF-8 string
		UTF8String rStrUTF8();
		/// Read a UTF-16 string
		UTF16String rStrUTF16();
		/// Read a UTF-32 string
		UTF32String rStrUTF32();

		/// Write an unsigned integer of 8 bits
		void wUInt8(UInt8 val);
		/// Write an unsigned integer of 16 bits, taking endianness into account
		void wUInt16(UInt16 val);
		/// Write an unsigned integer of 32 bits, taking endianness into account
		void wUInt32(UInt32 val);
		/// Write an unsigned integer of 64 bits, taking endianness into account
		void wUInt64(UInt64 val);
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Write an unsigned integer of 64 bits, taking endianness into account
		void wUInt128(UInt128 val);
		#endif
		/// Write a packed unsigned integer of 16 bits, taking endianness into account
		void wPack16u(UInt16 Value);
		/// Write a packed unsigned integer of 32 bits, taking endianness into account
		void wPack32u(UInt32 Value);
		/// Write a packed unsigned integer of 64 bits, taking endianness into account
		void wPack64u(UInt64 Value);
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Write a packed unsigned integer of 128 bits, taking endianness into account
		void wPack128u(UInt128 Value);
		#endif

		/// Write a float number of single precision
		void wFloat32(const Float32 &val);
		/// Write a float number of double precision
		void wFloat64(const Float64 &val);
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		/// Write a float number of quadruple precision
		void wFloat128(const Float128 &val);
		#endif

		/// Write a UTF-8 string
		void wStrUTF8(const UTF8 *Value);
		/// Write a UTF-16 string
		void wStrUTF16(const UTF16 *Value);
		/// Write a UTF-32 string
		void wStrUTF32(const UTF32 *Value);

		//
		void InitRBit(TRWBit &Rec);
		unsigned rBits(unsigned char Bits, TRWBit &Rec);

		void InitWBit(TRWBit &Rec);
		void DoneWBit(TRWBit &Rec);
		void wBits(unsigned Value, unsigned char Bits, TRWBit &Rec);
		void wCopy(CBufdStream &Source, TdPtr64 Size);

		void ClearPipe();
		void PushPipe(CdStreamPipe* APipe);
		void PopPipe();

		COREARRAY_INLINE TdPtr64 &Position() { return fPosition; }
		COREARRAY_INLINE void SetPosition(const TdPtr64 pos) { fPosition = pos; }

		COREARRAY_INLINE CdStream *Stream() const { return fStream; }
		void SetStream(CdStream* Value);
		COREARRAY_INLINE CdStream *BaseStream() const { return fBaseStream; }

		COREARRAY_INLINE ssize_t BufSize() const { return fBufSize; }
		void SetBufSize(const ssize_t NewBufSize);
		virtual TdPtr64 GetSize();
		virtual void SetSize(const TdPtr64 Value);

		TdOnNotify<CBufdStream> OnFlush;
	protected:
		CdStream *fStream, *fBaseStream;
		TdPtr64 fPosition;
		TdPtr64 vActualPos, vBufStart, vBufEnd;
		ssize_t fBufSize;

		/// Read a property name
		std::string rPropName();
		/// Write a property name
		void wPropName(const char *Name);
		/// Read an object name
		std::string rObjName();
		/// Write an object name
		void wObjName(const char *Name);

		ssize_t ReadStream();
		void WriteStream();

	private:
		char *vBuffer;
		bool vBufWriteFlag;
		std::vector<CdStreamPipe*> vPipeItems;
	};


	/// an operator, to read a signed integer of 8 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Int8& out)
		{ out = s.rUInt8(); return s; }
	/// an operator, to read an unsigned integer of 8 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UInt8& out)
		{ out = s.rUInt8(); return s; }
	/// an operator, to read a signed integer of 16 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Int16& out)
		{ out = s.rUInt16(); return s; }
	/// an operator, to read an unsigned integer of 16 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UInt16& out)
		{ out = s.rUInt16(); return s; }
	/// an operator, to read a signed integer of 32 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Int32& out)
		{ out = s.rUInt32(); return s; }
	/// an operator, to read an unsigned integer of 32 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UInt32& out)
		{ out = s.rUInt32(); return s; }
	/// an operator, to read a signed integer of 64 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Int64& out)
		{ out = s.rUInt64(); return s; }
	/// an operator, to read an unsigned integer of 64 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UInt64& out)
		{ out = s.rUInt64(); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to read a signed integer of 128 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Int128& out)
		{ out = s.rUInt128(); return s; }
	/// an operator, to read an unsigned integer of 128 bits from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UInt128& out)
		{ out = s.rUInt128(); return s; }
	#endif
	/// an operator, to read a float number of single precision from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Float32& out)
		{ out = s.rFloat32(); return s; }
	/// an operator, to read a float number of double precision from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Float64& out)
		{ out = s.rFloat64(); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to read a float number of quadruple precision from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, Float128& out)
		{ out = s.rFloat128(); return s; }
	#endif
	/// an operator, to read TdPosType from a buffer (6 bytes)
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, TdPosType& out)
	{
		UInt64 L = 0;
		s.Read((void*)&L, TdPosType::size);
	#if defined(COREARRAY_LITTLE_ENDIAN)
		out = L;
	#elif defined(COREARRAY_BIG_ENDIAN)
		out = COREARRAY_ENDIAN_CVT64(L);
	#else
	#  error "Unsupported Endianness!"
	#endif
		return s;
	}
	/// an operator, to read a UTF-8 string from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UTF8String& out)
		{ out = s.rStrUTF8(); return s; }
	/// an operator, to read a UTF-16 string from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UTF16String& out)
		{ out = s.rStrUTF16(); return s; }
	/// an operator, to read a UTF-32 string from a buffer
	COREARRAY_INLINE CBufdStream& operator>> (CBufdStream &s, UTF32String& out)
		{ out = s.rStrUTF32(); return s; }

	/// an operator, to write a signed integer of 8 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Int8 in)
		{ s.wUInt8(in); return s; }
	/// an operator, to write an unsigned integer of 8 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UInt8 in)
		{ s.wUInt8(in); return s; }
	/// an operator, to write a signed integer of 16 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Int16 in)
		{ s.wUInt16(in); return s; }
	/// an operator, to write an unsigned integer of 16 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UInt16 in)
		{ s.wUInt16(in); return s; }
	/// an operator, to write a signed integer of 32 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Int32 in)
		{ s.wUInt32(in); return s; }
	/// an operator, to write an unsigned integer of 32 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UInt32 in)
		{ s.wUInt32(in); return s; }
	/// an operator, to write a signed integer of 64 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Int64 in)
		{ s.wUInt64(in); return s; }
	/// an operator, to write an unsigned integer of 64 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UInt64 in)
		{ s.wUInt64(in); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to write a signed integer of 128 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Int128 in)
		{ s.wUInt128(in); return s; }
	/// an operator, to write an unsigned integer of 128 bits to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UInt128 in)
		{ s.wUInt128(in); return s; }
	#endif
	/// an operator, to write a float number of single precision to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Float32 &in)
		{ s.wFloat32(in); return s; }
	/// an operator, to write a float number of double precision to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Float64 &in)
		{ s.wFloat64(in); return s; }
	#ifndef COREARRAY_NO_EXTENDED_TYPES
	/// an operator, to write a float number of quadruple precision to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const Float128 &in)
		{ s.wFloat128(in); return s; }
	#endif
	/// an operator, to write TdPosType to a buffer (6 bytes)
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const TdPosType &in)
	{
	#if defined(COREARRAY_LITTLE_ENDIAN)
		UInt64 L = in;
	#elif defined(COREARRAY_BIG_ENDIAN)
		UInt64 L = COREARRAY_ENDIAN_CVT64(in);
	#else
	#  error "Unsupported Endianness!"
	#endif
		s.Write((void*)&L, TdPosType::size);
		return s;
	}
	/// an operator, to write a UTF-8 string to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UTF8String &in)
		{ s.wStrUTF8(in.c_str()); return s; }
	/// an operator, to write a UTF-16 string to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UTF16String &in)
		{ s.wStrUTF16(in.c_str()); return s; }
	/// an operator, to write a UTF-32 string to a buffer
	COREARRAY_INLINE CBufdStream& operator<< (CBufdStream &s, const UTF32String &in)
		{ s.wStrUTF32(in.c_str()); return s; }



	/// Serialization between a class object and stream data
	/** CdSerial provides functions for loading and saving an object from or to
	 *  stream data, which supports elementary types (e.g integer, float) and
	 *  complex structure.
	 *  The term "serialization" means the reversible deconstruction of a set of
	 *  C++ data structures to a sequence of bytes, and then reconstituting an
	 *  equivalent structure in another program context.
	**/
	class CdSerial: public CBufdStream
	{
	public:
		struct TdVar;
		friend struct CdSerial::TdVar;

		/// Constructor
		/** \param vStream    an stream (it can be NULL)
		 *  \param vBufSize   buffer size
		 *  \param vClassMgr  specify the class manager, if NULL then use the default manager
		**/
		CdSerial(CdStream* vStream, ssize_t vBufSize=DefaultBufSize,
			CdObjClassMgr* vClassMgr=NULL);
		/// Destructor
		virtual ~CdSerial();

		/// Begin a block in read mode
		TdPtr64 rBeginStruct();
		/// Begin a namespace in read mode
		TdPtr64 rBeginNameSpace();
		/// Initialize elements of a namespace in read mode
		void rInitNameSpace();
		/// End a block or namespace in read mode
		void rEndStruct();

		/// Begin a block in write mode
		void wBeginStruct();
		/// Begin a namespace in write mode
		void wBeginNameSpace();
		/// End a block or namespace in write mode
		void wEndStruct();

		/// Return whether there is a name in the current namespace
		bool HasName(const char *Name);
		/// Return type id of the variable with Name, if not exist then return osUnknown
		TdTypeID NameType(const char *Name);
		/// Return the position of the variable with Name, if not exist then return -1
		TdPtr64 NamePosition(const char *Name);

		/// Return an stream which fits the current block
		virtual CdStream* BlockStream();

		/// If the current position is not in the block, then throw an exception
		void CheckInStruct();
		/// Return whether the current position is at the end of the block
		bool EndOfStruct();
		/// Return the information of current block
		void StructInfo(TdPtr64 &Start, TdPtr64 &Length);
    	/// Return the relative position in the block
		TdPtr64 GetRelPos();
    	/// Set the relative position in the block
		void SetRelPos(const TdPtr64 Value);

		/// Log record
		COREARRAY_INLINE CdLogRecord &Log() { return *fLog; }
		/// Set log record
        void SetLog(CdLogRecord &vLog);

		/// Return a TdVar object, which provides functions of variables
		TdVar &operator[] (const char *vName);

		/// Return class manager
		COREARRAY_INLINE CdObjClassMgr * ClassMgr() const { return fClassMgr; }

	protected:

		/// Read a variable with Name
		/** \param Kind    type id
		 *  \param Name    variable name
		 *  \param OutBuf  pointer to the value
		**/
		bool rValue(TdTypeID Kind, char const* Name, void *OutBuf);

		/// Read a variable with Name
		/** \param Kind    type id
		 *  \param Name    variable name
		 *  \param OutBuf  pointer to the value
		 *  \param BufLen  buffer length; if the length of type is fixed, it can be ignored
		**/
		bool rValue(TdTypeID Kind, char const* Name, void *OutBuf, ssize_t BufLen);

		/// Write a variable of Name
		/** \param Kind    type id
		 *  \param Name    variable name
		 *  \param InBuf   pointer to the value
		 *  \param BufLen  buffer length; if the length of type is fixed, it can be ignored
		**/
		void wValue(TdTypeID Kind, const char *Name, const void *InBuf=NULL, ssize_t BufLen=-1);

		/// The basic class of variable in a block
		class CBaseVar
		{
		public:
			TdTypeID Kind;    ///< type ID for this variable
			TdPtr64 Start;    ///< the start position in a stream for this variable
			UTF8String Name;  ///< variable name

			CBaseVar() {}
			virtual ~CBaseVar() {}

			virtual Int64 Int() = 0;
			virtual LongFloat Float() = 0;
			virtual size_t SizeOf() = 0;
			virtual void *PtrData() = 0;

			template<typename TYPE> TYPE get()
				{ return *static_cast<TYPE*>(PtrData()); }
		};
		/// The generic class of variable in a block, which can store any data
		template<typename TYPE> class CVar: public CBaseVar
		{
		public:
			TYPE X;  ///< the data
			virtual Int64 Int() { return ValCvt<Int64, TYPE>(X); }
			virtual LongFloat Float() { return ValCvt<LongFloat, TYPE>(X); }
			virtual size_t SizeOf() { return sizeof(TYPE); }
			virtual void *PtrData() { return &X; };
		};

		/// The collection of variables in a block
		class CVarList
		{
		public:
			std::list<CBaseVar*> VarList;  ///< a list of variables
			TdPtr64 Start;   ///< the start position in a stream
			TdPtr64 Length;  ///< the block length in a stream
			int VarCount;    ///< the number of variables

			/// Constructor
			CVarList() { Start = Length = 0; VarCount = 0; };
			/// Destructor
			~CVarList() { ClearVarList(); };

			/// Add a new variable
			/** \tparam TYPE    data type
			 *  \param  Filter  a filter, usually it is the caller
			 *  \param  ID      type ID
			 *  \param  pos     the start position
			 *  \param  Name    the variable name
			**/
			template<typename TYPE> TYPE & AddVar(CdSerial &Filter,
				TdTypeID ID, TdPtr64 pos, const std::string &Name)
			{
				CVar<TYPE> *p = new CVar<TYPE>;
				p->Kind = ID; p->Start = pos; p->Name = Name;
				_AddVarItem(Filter, p);
				return p->X;
			}

			/// Find the variable with the name
			std::list<CBaseVar*>::iterator Name2Iter(const char *Name);
			/// Clear the variable list
			void ClearVarList();

		protected:
			/// Add a new variable
			/** \param  Filter  a filter, usually it is the caller
			 *  \param  rec     a pointer to a CBaseVar object
			**/
			void _AddVarItem(CdSerial &Filter, CBaseVar *rec);
		};

		CdObjClassMgr* fClassMgr;
		std::list<CVarList> fFilterRec;
		CdLogRecord *fLog;

		CVarList &Current();
	public:
        /// A structure, used for connecting CdSerial and a variable
		struct TdVar
		{
		public:
            friend class CdSerial;

			TdVar() { fFilter = NULL; fName = NULL; }

			COREARRAY_INLINE CdSerial *Filter() { return fFilter; }
			COREARRAY_INLINE const char *Name() { return fName; }

			// operator - Read
			// integer
			COREARRAY_INLINE bool operator >> (Int8 &val)
				{ return fFilter->rValue(osInt8, fName, &val); }
			COREARRAY_INLINE bool operator >> (UInt8 &val)
				{ return fFilter->rValue(osUInt8, fName, &val); }
			COREARRAY_INLINE bool operator >> (Int16 &val)
				{ return fFilter->rValue(osInt16, fName, &val); }
			COREARRAY_INLINE bool operator >> (UInt16 &val)
				{ return fFilter->rValue(osUInt16, fName, &val); }
			COREARRAY_INLINE bool operator >> (Int32 &val)
				{ return fFilter->rValue(osInt32, fName, &val); }
			COREARRAY_INLINE bool operator >> (UInt32 &val)
				{ return fFilter->rValue(osUInt32, fName, &val); }
			COREARRAY_INLINE bool operator >> (Int64 &val)
				{ return fFilter->rValue(osInt64, fName, &val); }
			COREARRAY_INLINE bool operator >> (UInt64 &val)
				{ return fFilter->rValue(osUInt64, fName, &val); }
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			COREARRAY_INLINE bool operator >> (Int128 &val)
				{ return fFilter->rValue(osInt128, fName, &val); }
			COREARRAY_INLINE bool operator >> (UInt128 &val)
				{ return fFilter->rValue(osUInt128, fName, &val); }
			#endif
			COREARRAY_INLINE bool rPack(UInt16 &val)
				{ return fFilter->rValue(os16Packed, fName, &val); }
			COREARRAY_INLINE bool rPack(UInt32 &val)
				{ return fFilter->rValue(os32Packed, fName, &val); }
			COREARRAY_INLINE bool rPack(UInt64 &val)
				{ return fFilter->rValue(os64Packed, fName, &val); }
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			COREARRAY_INLINE bool rPack(UInt128 &val)
				{ return fFilter->rValue(os128Packed, fName, &val); }
			#endif

			// float number
			COREARRAY_INLINE bool operator >> (Float32 &val)
				{ return fFilter->rValue(osFloat32, fName, &val); }
			COREARRAY_INLINE bool operator >> (Float64 &val)
				{ return fFilter->rValue(osFloat64, fName, &val); }
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			COREARRAY_INLINE bool operator >> (Float128 &val)
				{ return fFilter->rValue(osFloat128, fName, &val); }
			#endif

			// string
			COREARRAY_INLINE bool operator >> (UTF8String &val)
				{ return fFilter->rValue(osStrUTF8, fName, &val); }
			COREARRAY_INLINE bool operator >> (UTF16String &val)
				{ return fFilter->rValue(osStrUTF16, fName, &val); }
			COREARRAY_INLINE bool operator >> (UTF32String &val)
				{ return fFilter->rValue(osStrUTF32, fName, &val); }
			// others
			COREARRAY_INLINE bool operator >> (TdPosType &val)
				{ return fFilter->rValue(osStreamPos, fName, &val); }

			COREARRAY_INLINE bool rShortBuffer(void* Buf, ssize_t BufLen)
				{ return fFilter->rValue(osShortRec, fName, Buf, BufLen); }
			bool rShortBuf(Int32 *pval, size_t ValCnt);
			bool rShortBuf(Int64 *pval, size_t ValCnt);

			COREARRAY_INLINE bool rBuffer()
				{ return fFilter->rValue(osRecord, fName, NULL); }
			COREARRAY_INLINE bool rBuffer(void* Buf, ssize_t BufLen)
				{ return fFilter->rValue(osRecord, fName, Buf); }
			bool rBuf(Int32 *pval, size_t ValCnt);
			bool rBuf(Int64 *pval, size_t ValCnt);


			// operator - Write
			// integer
			COREARRAY_INLINE void operator << (const Int8 val)
				{ fFilter->wValue(osInt8, fName, &val); }
			COREARRAY_INLINE void operator << (const UInt8 val)
				{ fFilter->wValue(osUInt8, fName, &val); }
			COREARRAY_INLINE void operator << (const Int16 val)
				{ fFilter->wValue(osInt16, fName, &val); }
			COREARRAY_INLINE void operator << (const UInt16 val)
				{ fFilter->wValue(osUInt16, fName, &val); }
			COREARRAY_INLINE void operator << (const Int32 val)
				{ fFilter->wValue(osInt32, fName, &val); }
			COREARRAY_INLINE void operator << (const UInt32 val)
				{ fFilter->wValue(osUInt32, fName, &val); }
			COREARRAY_INLINE void operator << (const Int64 val)
				{ fFilter->wValue(osInt64, fName, &val); }
			COREARRAY_INLINE void operator << (const UInt64 val)
				{ fFilter->wValue(osUInt64, fName, &val); }
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			COREARRAY_INLINE void operator << (const Int128 &val)
				{ fFilter->wValue(osInt128, fName, &val); }
			COREARRAY_INLINE void operator << (const UInt128 &val)
				{ fFilter->wValue(osUInt128, fName, &val); }
			#endif
			COREARRAY_INLINE void wPack(const UInt16 val)
				{ fFilter->wValue(os16Packed, fName, &val); }
			COREARRAY_INLINE void wPack(const UInt32 val)
				{ fFilter->wValue(os32Packed, fName, &val); }
			COREARRAY_INLINE void wPack(const UInt64 val)
				{ fFilter->wValue(os64Packed, fName, &val); }
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			COREARRAY_INLINE void wPack(const UInt128 &val)
				{ fFilter->wValue(os128Packed, fName, &val); }
			#endif

			// float number
			COREARRAY_INLINE void operator << (const Float32 val)
				{ fFilter->wValue(osFloat32, fName, &val); }
			COREARRAY_INLINE void operator << (const Float64 val)
				{ fFilter->wValue(osFloat64, fName, &val); }
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			COREARRAY_INLINE void operator << (const Float128 &val)
				{ fFilter->wValue(osFloat128, fName, &val); }
			#endif

			// string
			COREARRAY_INLINE void operator << (const UTF8String &val)
				{ fFilter->wValue(osStrUTF8, fName, val.c_str()); }
			COREARRAY_INLINE void operator << (const UTF8 *val)
				{ fFilter->wValue(osStrUTF8, fName, val); }
			COREARRAY_INLINE void operator << (const UTF16String &val)
				{ fFilter->wValue(osStrUTF16, fName, val.c_str()); }
			COREARRAY_INLINE void operator << (const UTF16 *val)
				{ fFilter->wValue(osStrUTF16, fName, val); }
			COREARRAY_INLINE void operator << (const UTF32String &val)
				{ fFilter->wValue(osStrUTF32, fName, val.c_str()); }
			COREARRAY_INLINE void operator << (const UTF32 *val)
				{ fFilter->wValue(osStrUTF32, fName, val); }
			// others
			COREARRAY_INLINE void operator << (const TdPosType &val)
				{ fFilter->wValue(osStreamPos, fName, &val); }

			COREARRAY_INLINE void wShortBuffer(const void* Buf, ssize_t BufLen)
				{ fFilter->wValue(osShortRec, fName, Buf); }
			void wShortBuf(const Int32 *pval, size_t ValCnt);
			void wShortBuf(const Int64 *pval, size_t ValCnt);

			COREARRAY_INLINE void wBuffer()
				{ fFilter->wValue(osRecord, fName, NULL); }
			COREARRAY_INLINE void wBuffer(const void* Buf, ssize_t BufLen)
				{ fFilter->wValue(osRecord, fName, Buf); }
			void wBuf(const Int32 *pval, size_t ValCnt);
			void wBuf(const Int64 *pval, size_t ValCnt);

		private:
			CdSerial *fFilter;
			const char *fName;
		};
	private:
        TdVar fVar;
	};



	// CdObjClassMgr

	/// CoreArray class manager
	class CdObjClassMgr: public CdAbstractManager
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
			const char * Desp;
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
		virtual CdObjRef* ToObj(CdSerial &Reader, TdInit OnInit,
			void *Data, bool Silent);

		const _ClassStruct &ClassStruct(const char *ClassName) const;
			COREARRAY_INLINE const std::map<const char *, _ClassStruct, _strCmp> &
			ClassMap() const { return fClassMap; }
	protected:
		std::map<const char *, _ClassStruct, _strCmp> fClassMap;
	};


	/// The global manager of CoreArray classes
	CdObjClassMgr &dObjManager();


	// CoreArray Exception

	/// Check memory allocation
	#define COREARRAY_ALLOCCHECK(x)	{ if (!x) throw bad_alloc(); }

	/// Exception for CoreArray object
	class Err_dObj: public ErrCoreArray
	{
	public:
		Err_dObj() {}
		Err_dObj(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		Err_dObj(const std::string &msg) { fMessage = msg; }
	};


	/// Exception for CoreArray stream
	class Err_dStream: public ErrCoreArray
	{
	public:
		Err_dStream() {}
		Err_dStream(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		Err_dStream(const std::string &msg) { fMessage = msg; }
	};


	/// Exception for CoreArray buffer object
	class Err_BufStream: public ErrCoreArray
	{
	public:
        Err_BufStream() {}
		Err_BufStream(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		Err_BufStream(const std::string &msg) { fMessage = msg; }
	};


	/// Exception for CoreArray filter object
	class Err_dFilter: public Err_BufStream
	{
	public:
		Err_dFilter() {}
		Err_dFilter(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		Err_dFilter(const std::string &msg) { fMessage = msg; }
	};



	/// Generic data type
	struct TdsAny
	{
	public:
		friend CdSerial& operator>> (CdSerial &s, TdsAny& out);
		friend CdSerial& operator<< (CdSerial &s, TdsAny &in);

		/// Data type for TdsAny
		typedef unsigned char TdsType;

		/// Type ID
		enum {
			dvtAtomicBegin = 0,    ///< the first atomic type

			dvtNULL       = 0,    ///< NULL type

			// integer
			dvtInt8        = 1,    ///< Signed integer of 8 bits
			dvtUInt8       = 2,    ///< Unsigned integer of 8 bits
			dvtInt16       = 3,    ///< Signed integer of 16 bits
			dvtUInt16      = 4,    ///< Unsigned integer of 16 bits
			dvtInt32       = 5,    ///< Signed integer of 32 bits
			dvtUInt32      = 6,    ///< Unsigned integer of 32 bits
			dvtInt64       = 7,    ///< Signed integer of 64 bits
			dvtUInt64      = 8,    ///< Unsigned integer of 64 bits
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			dvtInt128      = 9,    ///< Signed integer of 128 bits
			dvtUInt128     = 10,   ///< Unsigned integer of 128 bits
			#endif

			// float number
			dvtFloat32     = 11,   ///< Float number of single precision (32 bits)
			dvtFloat64     = 12,   ///< Float number of double precision (64 bits)
			#ifndef COREARRAY_NO_EXTENDED_TYPES
			dvtFloat128    = 13,   ///< Float number of quadruple precision (128 bits)
			#endif

			// string
			dvtSString8    = 14,   ///< Short UTF-8 string, the length <= 22
			dvtSString16   = 15,   ///< Short UTF-16 string, the length <= 11
			dvtSString32   = 16,   ///< Short UTF-32 string, the length <= 5
			dvtStr8        = 17,   ///< UTF-8 string
			dvtStr16       = 18,   ///< UTF-16 string
			dvtStr32       = 19,   ///< UTF-32 string

			// other extended type
			dvtBoolean     = 20,   ///< Boolean type
			dvtAtomicLast  = 20,   ///< the last atomic type

			// pointer
			dvtExtFirst    = 32,   ///< the first extended type
			dvtPointer     = 32,   ///< pointer
			dvtArray       = 33,   ///< array of TdsAny, include a length
			dvtObjRef      = 34,   ///< TdObjRef object
			dvtExtLast     = 34    ///< the last extended type
		};

		/// Initialize TdsAny, clear TdsAny with ZERO
		TdsAny();
		/// Free TdsAny
		~TdsAny();

		/// Return type ID
		COREARRAY_INLINE TdsType Type() const { return dsType; }

		/// Type ID of TdsAny to a name
		static const char *dvtNames(int index);


		// integer
		Int8 GetInt8() const;       ///< get Int8, throw an exception if fail
		UInt8 GetUInt8() const;     ///< get UInt8, throw an exception if fail
		Int16 GetInt16() const;     ///< get Int16, throw an exception if fail
		UInt16 GetUInt16() const;   ///< get UInt16, throw an exception if fail
		Int32 GetInt32() const;     ///< get Int32, throw an exception if fail
		UInt32 GetUInt32() const;   ///< get UInt32, throw an exception if fail
		Int64 GetInt64() const;     ///< get Int64, throw an exception if fail
		UInt64 GetUInt64() const;   ///< get UInt64, throw an exception if fail
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		Int128 GetInt128() const;   ///< get Int128, throw an exception if fail
		UInt128 GetUInt128() const; ///< get UInt128, throw an exception if fail
		#endif

		// float number
		Float32 GetFloat32() const;   ///< get Float32, throw an exception if fail
		Float64 GetFloat64() const;   ///< get Float64, throw an exception if fail
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		Float128 GetFloat128() const; ///< get Float128, throw an exception if fail
		#endif

		// string
		UTF8String GetStr8() const;   ///< get UTF8String, throw an exception if fail
		UTF16String GetStr16() const; ///< get UTF16String, throw an exception if fail
		UTF32String GetStr32() const; ///< get UTF32String, throw an exception if fail

		// other extended type
		bool GetBool() const;  ///< get boolean, throw an exception if fail

		// pointer
		const void *GetPtr() const;  ///< get a pointer, throw an exception if fail

		// array
		TdsAny *GetArray() const;  ///< get a pointer
		UInt32 GetArrayLength() const;   ///< get the length of array

		// others
		CdObjRef* GetObj() const; ///< get CdObjRef, throw an exception if fail


		/// define a generic 'Get' method
		template<typename FUNC, typename TYPE, TYPE DEFAULT>
			TYPE Get()
		{
			try {
				return FUNC();
			} catch (ErrCoreArray &E) {
				return DEFAULT;
			}
		}


		void SetEmpty();    ///< set Empty
		// integer
		void SetInt8(const Int8 val);       ///< set Int8
		void SetUInt8(const UInt8 val);     ///< set UInt8
		void SetInt16(const Int16 val);     ///< set Int16
		void SetUInt16(const UInt16 val);   ///< set UInt16
		void SetInt32(const Int32 val);     ///< set Int32
		void SetUInt32(const UInt32 val);   ///< set UInt32
		void SetInt64(const Int64 val);     ///< set Int64
		void SetUInt64(const UInt64 val);   ///< set UInt64
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		void SetInt128(const Int128 &val);   ///< set Int128
		void SetUInt128(const UInt128 &val); ///< set UInt128
		#endif

		// float number
		void SetFloat32(const Float32 val);   ///< set Float32
		void SetFloat64(const Float64 val);   ///< set Float64
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		void SetFloat128(const Float128 &val); ///< set Float128
		#endif

		// string
		void SetStr8(const UTF8String &val);   ///< set UTF8String
		void SetStr16(const UTF16String &val); ///< set UTF16String
		void SetStr32(const UTF32String &val); ///< set UTF32String

		// other extended type
		void SetBool(const bool val);     ///< set boolean

		// pointer
		void SetPtr(const void *ptr);  ///< set a pointer

		// array
		void SetArray(UInt32 size);  ///< set an array
		void SetArray(const Int32 ptr[], UInt32 size);  ///< set an int32 array
		void SetArray(const Int64 ptr[], UInt32 size);  ///< set an int64 array
		void SetArray(const Float64 ptr[], UInt32 size);  ///< set a double array
		void SetArray(const char* const ptr[], UInt32 size);  ///< set a string array
		void SetArray(const std::string ptr[], UInt32 size);  ///< set a string array
		void SetArray(const bool ptr[], UInt32 size);  ///< set a boolean array

		// CdObjRef
		void SetObj(CdObjRef *obj); ///< set a CdObjRef object


		void Assign(const UTF8String &val); ///< auto determine data type

		// check data type
		bool IsNULL() const;  ///< return true, if it is dvtNULL
		bool IsInt() const;    ///< return true, if it is an integer
		bool IsFloat() const;  ///< return true, if it is a float number
		bool IsNumeric() const;    ///< return true, if it is an integer or float number
		bool IsBool() const;   ///< return true, if it is dvtBoolean
		bool IsNaN() const;    ///< return true, if it is a NaN, or not a number
		bool IsNA() const;     ///< return true, if it is a NaN, or dvtNULL
		bool IsString() const;    ///< return true, if it is a string
		bool IsArray() const;  ///< return true, if it is array-type

		bool Packed();  ///< pack the data, if packed return true
		void Swap(TdsAny &D); ///< swap *this and D

		/// compare *this and D (TdsAny)
		/** \param D       a TdsAny data
		 *  \param NALast  NA value will be the last
		 *
		 *  return  1, if *this > D;
		 *  return  0, if *this == D;
         *  return -1, if *this < D
		**/
		int Compare(const TdsAny &D, bool NALast = true);


		// operator
        COREARRAY_INLINE operator Int8() { return GetInt8(); }
        COREARRAY_INLINE operator UInt8() { return GetUInt8(); }
		COREARRAY_INLINE operator Int16() { return GetInt16(); }
		COREARRAY_INLINE operator UInt16() { return GetUInt16(); }
		COREARRAY_INLINE operator Int32() { return GetInt32(); }
		COREARRAY_INLINE operator UInt32() { return GetUInt32(); }
		COREARRAY_INLINE operator Int64() { return GetInt64(); }
		COREARRAY_INLINE operator UInt64() { return GetUInt64(); }
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		COREARRAY_INLINE operator Int128() { return GetInt128(); }
		COREARRAY_INLINE operator UInt128() { return GetUInt128(); }
		#endif

		COREARRAY_INLINE operator Float32() { return GetFloat32(); }
		COREARRAY_INLINE operator Float64() { return GetFloat64(); }
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		COREARRAY_INLINE operator Float128() { return GetFloat128(); }
		#endif

		COREARRAY_INLINE operator UTF8String() { return GetStr8(); }
		COREARRAY_INLINE operator UTF16String() { return GetStr16(); }
		COREARRAY_INLINE operator UTF32String() { return GetStr32(); }

		/// an operator of assignment
		TdsAny & operator= (const TdsAny &_Right);
		TdsAny & operator= (const Int8 val) { SetInt8(val); return *this; }
		TdsAny & operator= (const UInt8 val) { SetUInt8(val); return *this; }
		TdsAny & operator= (const Int16 val) { SetInt16(val); return *this; }
		TdsAny & operator= (const UInt16 val) { SetUInt16(val); return *this; }
		TdsAny & operator= (const Int32 val) { SetInt32(val); return *this; }
		TdsAny & operator= (const UInt32 val) { SetUInt32(val); return *this; }
		TdsAny & operator= (const Int64 val) { SetInt64(val); return *this; }
		TdsAny & operator= (const UInt64 val) { SetUInt64(val); return *this; }
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		TdsAny & operator= (const Int128 &val) { SetInt128(val); return *this; }
		TdsAny & operator= (const UInt128 &val) { SetUInt128(val); return *this; }
		#endif

		TdsAny & operator= (const Float32 val) { SetFloat32(val); return *this; }
		TdsAny & operator= (const Float64 val) { SetFloat64(val); return *this; }
		#ifndef COREARRAY_NO_EXTENDED_TYPES
		TdsAny & operator= (const Float128 &val) { SetFloat128(val); return *this; }
		#endif

		TdsAny & operator= (const UTF8String &val) { SetStr8(val); return *this; }
		TdsAny & operator= (const UTF16String &val) { SetStr16(val); return *this; }
		TdsAny & operator= (const UTF32String &val) { SetStr32(val); return *this; }

		TdsAny & operator= (const void *val) { SetPtr(val); return *this; }

	private:

		TdsType dsType;
		union {
			struct {
				UInt8 Reserved[7];
				union {  // 8-byte aligned
					void *Align;
					UTF8String *ptrStr8;
					UTF16String *ptrStr16;
					UTF32String *ptrStr32;
					CdObjRef *obj;
					const void *const_ptr;
				};
			} aR;
			struct {
				UInt8 SStrLen8;
				UTF8 SStr8[22];
			} aS8;
			struct {
				UInt8 SStrLen16;
				UTF16 SStr16[11];
			} aS16;
			struct {
				UInt8 Reserved1, SStrLen32, Reserved2;
				UTF32 SStr32[5];
			} aS32;
			struct {
				TdsType dsArray;
				UInt8 ReservedArray1, ReservedArray2;
				UInt32 ArrayLength;
				TdsAny *ArrayPtr;
			} aArray;
		};

		template<typename TYPE> TYPE & VAL()
			{
				void *tmp = &aR.Align;
				return *static_cast<TYPE*>(tmp);
			}
		template<typename TYPE> const TYPE & VAL() const
			{
				const void *tmp = &aR.Align;
				return *static_cast<const TYPE*>(tmp);
			}

		void _Done();
	};

	/// an operator, to read TdsAny from a filter
	CdSerial& operator>> (CdSerial &s, TdsAny& out);
	/// an operator, to write TdsAny to a filter
	CdSerial& operator<< (CdSerial &s, TdsAny &in);


	/// Exception for TdsAny
	class Err_dsAny: public ErrConvert
	{
	public:
		Err_dsAny(const char *fmt, ...) { _COREARRAY_ERRMACRO_(fmt); }
		Err_dsAny(TdsAny::TdsType fromType, TdsAny::TdsType toType);
	};
}

#endif /* _dBase_H_ */
