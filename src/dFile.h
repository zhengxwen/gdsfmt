// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dFile.h: Functions and classes for CoreArray Genomic Data Structure (GDS)
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
 *	\file     dFile.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2014
 *	\brief    Functions and classes for CoreArray Genomic Data Structure (GDS)
 *	\details
**/

#ifndef _dFile_H_
#define _dFile_H_

#include <dBase.h>
#include <dStream.h>


namespace CoreArray
{
	class CdGDSObj;

	/// Data pipe
	class CdPipeMgrItem: public CdAbstractItem
	{
	public:
		friend class CdStreamPipeMgr;
		friend class CdGDSObj;

		/// Constructor
		CdPipeMgrItem();
		/// Destructor
		virtual ~CdPipeMgrItem();

		/// Create a new CdPipeMgrItem object
        virtual CdPipeMgrItem *NewOne() = 0;

		/// Return the name of coder
		virtual const char *Coder() const = 0;
		/// Return the description of coder
		virtual const char *Description() const = 0;
		/// Return whether or not Mode is self
		virtual bool Equal(const char *Mode) const = 0;

		virtual void PushReadPipe(CBufdStream &buf) = 0;
		virtual void PushWritePipe(CBufdStream &buf) = 0;
		virtual void PopPipe(CBufdStream &buf) = 0;
		virtual bool WriteMode(CBufdStream &buf) const = 0;
		virtual void ClosePipe(CBufdStream &buf) = 0;

    	void UpdateStreamSize();
		COREARRAY_INLINE SIZE64 StreamTotalIn() const { return fStreamTotalIn; }
		COREARRAY_INLINE SIZE64 StreamTotalOut() const { return fStreamTotalOut; }

		COREARRAY_INLINE CdGDSObj *Owner() { return fOwner; }
		COREARRAY_INLINE TdCompressRemainder &Remainder() { return fRemainder; }

	protected:
    	CdGDSObj *fOwner;
		SIZE64 fStreamTotalIn, fStreamTotalOut;
		TdCompressRemainder fRemainder;

		virtual CdPipeMgrItem *Match(const char *Mode) = 0;
		virtual bool GetStreamInfo(CBufdStream *Filter) = 0;
		virtual void UpdateStreamInfo(CdStream &Stream) = 0;
		virtual void LoadStream(CdSerial &Reader, TdVersion Version);
		virtual void SaveStream(CdSerial &Writer);

		static int Which(const char *txt, const char **Strs, int nStrs);
		static bool EqualStrNoCase(const char *s1, const char *s2);
	};


	/// The manager of stream pipes
	class CdStreamPipeMgr: public CdAbstractManager
	{
	public:
		CdStreamPipeMgr();
		~CdStreamPipeMgr();

		void Register(CdPipeMgrItem *vNewPipe);
		CdPipeMgrItem *Match(CdGDSObj &Obj, const char *Mode);

		COREARRAY_INLINE const std::vector<CdPipeMgrItem*> &RegList() const
			{ return fRegList; }

	protected:
		std::vector<CdPipeMgrItem*> fRegList;
	};

	extern CdStreamPipeMgr dStreamPipeMgr;



	class CdGDSFolder;
	class CdGDSFile;

	/// Attribute class for CdGDSObj
	class CdObjAttr: public CdObject
	{
	public:
		friend class CdGDSObj;
		friend class CdGDSFolder;

		CdObjAttr(CdGDSObj &vOwner);
		virtual ~CdObjAttr();

    	void Assign(CdObjAttr &Source);

		TdsAny &Add(const UTF16String &Name);
		int IndexName(const UTF16String &Name);
		bool HasName(const UTF16String &Name) { return IndexName(Name)>=0; }

		void Delete(const UTF16String &Name);
		void Delete(int Index);
		void Clear();

		COREARRAY_INLINE size_t Count() const { return fList.size(); }
        void Changed();

		COREARRAY_INLINE CdGDSObj &Owner() const { return fOwner; }

		TdsAny & operator[](const UTF16String &Name);
		TdsAny & operator[](int Index);

		COREARRAY_INLINE UTF16String &Names(int Index) { return fList[Index]->name; }
		void SetName(const UTF16String &OldName, const UTF16String &NewName);
		void SetName(int Index, const UTF16String &NewName);

	protected:
		struct TdPair {
			UTF16String name;
			TdsAny val;
		};

		CdGDSObj &fOwner;
		std::vector<TdPair*> fList;
		virtual void LoadAfter(CdSerial &Reader, const TdVersion Version);
		virtual void SaveAfter(CdSerial &Writer);

	private:
		std::vector<TdPair*>::iterator Find(const UTF16String &Name);
        void xValidateName(const UTF16String &name);
	};

	
	/// CoreArray GDS object
	class CdGDSObj: public CdObjMsg
	{
	public:
    	friend class CdPipeMgrItem;
		friend class CdObjAttr;
		friend class CdGDSFolder;
		friend class CdGDSVirtualFolder;
		friend class CdGDSFile;

		CdGDSObj();
		virtual ~CdGDSObj();

		virtual void SaveStruct(CdSerial &Writer, bool IncludeName);

		virtual CdGDSObj *NewOne(void *Param = NULL);
		virtual void AssignOne(CdGDSObj &Source, void *Param = NULL);

		virtual UTF16String Name() const;
		UTF16String FullName(const UTF16String &Delimiter) const;
		UTF16String FullName(const char *Delimiter = "/") const
			{ return FullName(PChartoUTF16(Delimiter)); }

		virtual void SetName(const UTF16String &NewName);

		void MoveTo(CdGDSFolder &folder);

		virtual void Synchronize();

		CdGDSFile *GDSFile();
		COREARRAY_INLINE CdObjAttr &Attribute() { return fAttr; }
		COREARRAY_INLINE CdBlockStream *GDSStream() const { return fGDSStream; }
		COREARRAY_INLINE CdGDSFolder *Folder() const { return fFolder; }
		COREARRAY_INLINE CdPipeMgrItem *PipeInfo() { return fPipeInfo; }

	protected:
		CdObjAttr fAttr;
		CdGDSFolder *fFolder;
		CdBlockStream *fGDSStream;
		CdPipeMgrItem *fPipeInfo;
		bool fChanged;

		virtual void LoadBefore(CdSerial &Reader, TdVersion Version);
		virtual void LoadAfter(CdSerial &Reader, TdVersion Version);
		virtual void SaveBefore(CdSerial &Writer);
		virtual void SaveAfter(CdSerial &Writer);
		virtual void GetPipeInfo();

		COREARRAY_INLINE bool _GetStreamPipeInfo(CBufdStream *buf, bool Close)
		{
			if (Close && buf)
				fPipeInfo->ClosePipe(*buf);
			return fPipeInfo->GetStreamInfo(buf);
		}
		COREARRAY_INLINE void _UpdateStreamPipeInfo(CdStream &Stream)
		{
			fPipeInfo->UpdateStreamInfo(Stream);
		}

		void _CheckGDSStream();
		static void _RaiseInvalidAssign(const std::string &msg);

		void SaveToBlockStream();
		virtual bool IsWithClassName() { return true; }

	private:
		static void _GDSObjInitProc(CdObjClassMgr &Sender, CdObject *dObj,
			void *Data);
	};

	/// The pointer to a GDS object
	typedef CdGDSObj* PdGDSObj;


	/// the GDS object without stream name
	class CdGDSObjNoCName: public CdGDSObj
	{
	public:
		CdGDSObjNoCName(): CdGDSObj() {}

	protected:
		virtual bool IsWithClassName() { return false; }
	};


	/// A label node for CoreArray GDS format
	class CdGDSLabel: public CdGDSObjNoCName
	{
	public:
        virtual CdGDSObj *NewOne(void *Param = NULL);
	};


    /// Abstract folder class for CoreArray GDS format
	class CdGDSAbsFolder: public CdGDSObj
	{
	public:
		virtual CdGDSObj *AddFolder(const UTF16String &Name) = 0;
		virtual CdGDSObj *AddObj(const UTF16String &Name, CdGDSObj *val=NULL) = 0;
		virtual CdGDSObj *InsertObj(int index, const UTF16String &Name,
			CdGDSObj *val=NULL) = 0;

		virtual void DeleteObj(int Index, bool force=true) = 0;
		virtual void DeleteObj(CdGDSObj *val, bool force=true) = 0;
		virtual void ClearObj(bool force=true) = 0;

		virtual CdGDSObj *ObjItem(int Index) = 0;
		virtual CdGDSObj *ObjItem(const UTF16String &Name) = 0;

		virtual CdGDSObj *ObjItemEx(int Index) = 0;
		virtual CdGDSObj *ObjItemEx(const UTF16String &Name) = 0;

		virtual CdGDSObj *Path(const UTF16String &FullName) = 0;
		virtual CdGDSObj *PathEx(const UTF16String &FullName) = 0;

		virtual int IndexObj(CdGDSObj *Obj) = 0;
		virtual bool HasChild(CdGDSObj *Obj, bool SubFolder = true) = 0;

		virtual int NodeCount() = 0;
	};


    /// Folder class for CoreArray GDS format
	class CdGDSFolder: public CdGDSAbsFolder
	{
	public:
		friend class CdGDSObj;
		friend class CdGDSFile;

		virtual ~CdGDSFolder();

        virtual CdGDSObj *NewOne(void *Param = NULL);
		void AssignOneEx(CdGDSFolder &Source);

		virtual CdGDSObj *AddFolder(const UTF16String &Name);

		virtual CdGDSObj *AddObj(const UTF16String &Name, CdGDSObj *val=NULL);

		virtual CdGDSObj *InsertObj(int index, const UTF16String &Name,
			CdGDSObj *val=NULL);

		virtual void DeleteObj(int Index, bool force=true);
		virtual void DeleteObj(CdGDSObj *val, bool force=true);
		virtual void ClearObj(bool force=true);

		virtual CdGDSObj *ObjItem(int Index);
		virtual CdGDSObj *ObjItem(const UTF16String &Name);

		virtual CdGDSObj *ObjItemEx(int Index);
		virtual CdGDSObj *ObjItemEx(const UTF16String &Name);

		virtual CdGDSObj *Path(const UTF16String &FullName);
		virtual CdGDSObj *PathEx(const UTF16String &FullName);

		virtual int IndexObj(CdGDSObj *Obj);
		virtual bool HasChild(CdGDSObj *Obj, bool SubFolder = true);

		virtual int NodeCount() { return fList.size(); }

		CdGDSFolder &DirItem(int Index);
		CdGDSFolder &DirItem(const UTF16String &Name);

		COREARRAY_INLINE CdGDSObj *operator[] (int Index)
			{ return ObjItem(Index); }
		COREARRAY_INLINE CdGDSObj *operator[] (const UTF16String &Name)
			{ return ObjItem(Name); }

		static void SplitPath(const UTF16String &FullName, UTF16String &Path,
        	UTF16String &Name);

	protected:
		struct TNode
		{
		public:
			enum {
				FLAG_TYPE_CLASS           = 0,
				FLAG_TYPE_LABEL           = 1,
				FLAG_TYPE_FOLDER          = 2,
				FLAG_TYPE_VIRTUAL_FOLDER  = 3,
				FLAG_TYPE_STREAM          = 4,
				FLAG_TYPE_MASK            = 0x0F
			};

			CdGDSObj *Obj;
			TdBlockID StreamID;
			UInt32 Flag;
			UTF16String Name;
			SIZE64 _pos;

			TNode();
			bool IsFlagType(UInt32 val) const;
			void SetFlagType(UInt32 val);
		};
		std::vector<TNode> fList;

		virtual void LoadAfter(CdSerial &Reader, TdVersion Version);
		virtual void SaveAfter(CdSerial &Writer);
		virtual bool IsWithClassName() { return false; }

		void _Clear();

	private:
		bool _HasName(const UTF16String &Name);
		TNode &_NameItem(const UTF16String &Name);
		void _LoadItem(TNode &I);
		void _UpdateAll();
		std::vector<TNode>::iterator _FindObj(CdGDSObj *Obj);
	};

	/// The pointer to a GDS folder
	typedef CdGDSFolder* PdGDSFolder;


	/// GDS virtual folder linking to another GDS file
	class CdGDSVirtualFolder: public CdGDSAbsFolder
	{
	public:
		CdGDSVirtualFolder();
		virtual ~CdGDSVirtualFolder();

		const UTF8String &LinkFileName() const
			{ return fLinkFileName; }
		void SetLinkFile(const UTF8String &FileName);

		bool IsLoaded(bool Silent);

		virtual CdGDSObj *AddFolder(const UTF16String &Name);
		virtual CdGDSObj *AddObj(const UTF16String &Name, CdGDSObj *val=NULL);
		virtual CdGDSObj *InsertObj(int index, const UTF16String &Name,
			CdGDSObj *val=NULL);

		virtual void DeleteObj(int Index, bool force=true);
		virtual void DeleteObj(CdGDSObj *val, bool force=true);
		virtual void ClearObj(bool force=true);

		virtual CdGDSObj *ObjItem(int Index);
		virtual CdGDSObj *ObjItem(const UTF16String &Name);

		virtual CdGDSObj *ObjItemEx(int Index);
		virtual CdGDSObj *ObjItemEx(const UTF16String &Name);

		virtual CdGDSObj *Path(const UTF16String &FullName);
		virtual CdGDSObj *PathEx(const UTF16String &FullName);

		virtual int IndexObj(CdGDSObj *Obj);
		virtual bool HasChild(CdGDSObj *Obj, bool SubFolder=true);

		virtual int NodeCount();

		virtual void LoadAfter(CdSerial &Reader, const TdVersion Version);
		virtual void SaveAfter(CdSerial &Writer);
		virtual bool IsWithClassName() { return false; }
		virtual void Synchronize();

		const string &ErrMsg() const { return fErrMsg; }

	protected:
		UTF8String fLinkFileName;
		CdGDSFile *fLinkFile;
		bool fHasTried;
		string fErrMsg;

	private:
		void _CheckLinked();
	};


	/// Stream container for CoreArray GDS format
	class CdGDSStreamContainer: public CdGDSObjNoCName
	{
	public:
		CdGDSStreamContainer();
		virtual ~CdGDSStreamContainer();

		/// Return a string specifying the class name in stream
		virtual char const* dName();
		/// Return a string specifying the class name
		virtual char const* dTraitName();

    	/// new a CdVector<T> object
		virtual CdGDSObj *NewOne(void *Param = NULL);
		/// Assignment
		virtual void AssignOne(CdGDSObj &Source, void *Param = NULL);

		void CopyFrom(CBufdStream &Source, SIZE64 Count=-1);
		void CopyFrom(CdStream &Source, SIZE64 Count=-1);

		void CopyTo(CBufdStream &Dest, SIZE64 Count=-1);
		void CopyTo(CdStream &Dest, SIZE64 Count=-1);

		SIZE64 GetSize();
		COREARRAY_INLINE CBufdStream *BufStream() { return fBufStream; }

		virtual void SetPackedMode(const char *Mode);
		virtual void CloseWriter();

	protected:
		CBufdStream *fBufStream;
		CdBlockStream *vAlloc_Stream;
		bool fNeedUpdate;
		TdBlockID vAllocID;
		SIZE64 vAlloc_Ptr;

		virtual void LoadAfter(CdSerial &Reader, TdVersion Version);
		virtual void SaveStruct(CdSerial &Writer, bool IncludeName);
		virtual void SaveAfter(CdSerial &Writer);
	};

	/// The pointer to a stream container
	typedef CdGDSStreamContainer* PdGDSStreamContainer;


	/// The root of a GDS file
	class CdGDSRoot: public CdGDSFolder
	{
	public:
		friend class CdGDSVirtualFolder;

		CdGDSRoot();

		virtual UTF16String Name() const;
		virtual void SetName(const UTF16String &NewName);

	protected:
		CdGDSVirtualFolder *fVFolder;
	};


	// GDS node indicating unknown states
	class CdGDSUnknown: public CdGDSObjNoCName
	{
	public:
		CdGDSUnknown(): CdGDSObjNoCName() {}

		virtual void SaveStruct(CdSerial &Writer, bool IncludeName)
		{
			// do nothing!
		}
	};


	/// CoreArray GDS File
	class CdGDSFile: protected CdBlockCollection
	{
	public:
		friend class CdGDSVirtualFolder;

		enum TdOpenMode { dmCreate, dmOpenRead, dmOpenReadWrite };

		CdGDSFile();
		CdGDSFile(const UTF16String &fn, TdOpenMode Mode);
		CdGDSFile(const char *fn, TdOpenMode Mode);
		virtual ~CdGDSFile();

		void LoadFile(const UTF16String &fn, bool ReadOnly=true);
		void LoadFile(const char *fn, bool ReadOnly=true);
		void LoadFileFork(const char *fn, bool ReadOnly=true);

		void SaveAsFile(const UTF16String &fn);
		void SaveAsFile(const char *fn);

		void DuplicateFile(const UTF16String &fn, bool deep);
		void DuplicateFile(const char *fn, bool deep);

		void SyncFile();
		void CloseFile();

		/// Clean up all fragments
		void TidyUp(bool deep);

		bool Modified();

		/// Return file size of the CdGDSFile object
		SIZE64 GetFileSize();

		int GetNumOfFragment();

		bool IfSupportForking();

		/// Return the file name of the CdGDSFile object
		COREARRAY_INLINE UTF16String &FileName() { return fFileName; }
		COREARRAY_INLINE CdGDSFolder &Root() { return fRoot; }
		COREARRAY_INLINE bool ReadOnly() const { return fReadOnly; }
		COREARRAY_INLINE CdLogRecord &Log() { return *fLog; }
		COREARRAY_INLINE TdVersion Version() const { return fVersion; }

		static const char *GDSFilePrefix();

	protected:
		TdVersion fVersion;
		CdGDSRoot fRoot;
		bool fReadOnly;
		UTF16String fFileName;

		void LoadStream(CdStream* Stream, bool ReadOnly = true);
		void SaveStream(CdStream* Stream);

	private:
        CdLogRecord *fLog;

		void _Init();
		bool _HaveModify(CdGDSFolder *folder);
	};

	/// The pointer to a CoreArray GDS File
	typedef CdGDSFile* PdGDSFile;



	/// Exception for CdGDSObj
	class ErrGDSObj: public Err_dObj
	{
	public:
		ErrGDSObj(): Err_dObj()
			{ }
		ErrGDSObj(const std::string &msg): Err_dObj()
			{ fMessage = msg; }
		ErrGDSObj(const char *fmt, ...): Err_dObj()
			{ _COREARRAY_ERRMACRO_(fmt); }
	};

	/// Exception for stream container
	class ErrGDSStreamContainer: public Err_dObj
	{
	public:
		ErrGDSStreamContainer(): Err_dObj()
			{ }
		ErrGDSStreamContainer(const std::string &msg): Err_dObj()
			{ fMessage = msg; }
		ErrGDSStreamContainer(const char *fmt, ...): Err_dObj()
			{ _COREARRAY_ERRMACRO_(fmt); }
	};

	/// Exception for GDS file
	class ErrGDSFile: public Err_dObj
	{
	public:
		ErrGDSFile(): Err_dObj()
			{ }
		ErrGDSFile(const std::string &msg): Err_dObj()
			{ fMessage = msg; }
		ErrGDSFile(const char *fmt, ...): Err_dObj()
			{ _COREARRAY_ERRMACRO_(fmt); }
	};
}

#endif /* _dFile_H_ */
