// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dParallel.h: Functions for parallel computing
//
// Copyright (C) 2007 - 2015	Xiuwen Zheng
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
 *	\file     dParallel.h
 *	\author   Xiuwen Zheng [zhengx@u.washington.edu]
 *	\version  1.0
 *	\date     2007 - 2015
 *	\brief    Functions for parallel computing
 *	\details
 *  \todo     To improve the support of reading a GDS file in parallel
 *  \todo     To support parallel reading and writing a GDS file simultaneously
**/

#ifndef _HEADER_PARALLEL_
#define _HEADER_PARALLEL_

#include "dPlatform.h"
#include "dTrait.h"

#include <vector>
#include <memory>
#include <algorithm>
#ifndef COREARRAY_NO_STD_IN_OUT
#	include <iostream>
#endif


namespace CoreArray
{
	/// The basic class for progress object
	class COREARRAY_DLL_DEFAULT CdBaseProgression
	{
	public:
		/// The mode of percent increasement
		enum TPercentMode {
			tp01 = 0,	//< The increasement is 1%
			tp10 = 1,	//< The increasement is 10%
			tp25 = 2,	//< The increasement is 25%
			tp50 = 3	//< The increasement is 50%
		};

		/// The associated information
		std::string Info;

		/// Constructor
		CdBaseProgression(TPercentMode permode = tp01);
		virtual ~CdBaseProgression() {}

		void Init(C_Int64 TotalCnt);
		bool Forward(C_Int64 step = 1);
		virtual void ShowProgress();

		/// Return the current mode of increasement
		COREARRAY_INLINE TPercentMode Mode() const { return fMode; }
		/// Set the current mode of increasement
		void SetMode(TPercentMode mode);
        /// Return the current percentile
		COREARRAY_INLINE int Percent() const { return fPercent; }
		/// Return the total number
		COREARRAY_INLINE C_Int64 Total() const { return fTotal; }

	protected:
		TPercentMode fMode;
		C_Int64 fTotal, vProg[101], vCurrent, *vptrProg;
		int fPercent;
	};

	class COREARRAY_DLL_DEFAULT CdConsoleProgress: public CdBaseProgression
	{
	public:
		CdConsoleProgress(TPercentMode permode = tp01);

		virtual void ShowProgress();
    };


	namespace Parallel
	{
		/// Exceptions for conversion
		class COREARRAY_DLL_EXPORT ErrParallel: public ErrCoreArray
		{
		public:
			ErrParallel(): ErrCoreArray()
				{ }
			ErrParallel(const char *fmt, ...): ErrCoreArray()
				{ _COREARRAY_ERRMACRO_(fmt); }
			ErrParallel(const std::string &msg): ErrCoreArray()
				{ fMessage = msg; }
		};


	#ifndef COREARRAY_NO_STD_IN_OUT
		#define COREARRAY_PARALLEL_TRY		try {
		#define COREARRAY_PARALLEL_CATCH	\
			} catch(std::exception &E) {	\
				std::cerr << E.what() << std::endl; std::exit(-1); \
			} catch(const char *E) { \
				std::cerr << E << std::endl; std::exit(-1); \
			} catch(const int E) { \
				std::cerr << "Error: " << E << std::endl; std::exit(-1); \
			} catch(...) { \
				std::exit(-1); \
			}
	#else
		#define COREARRAY_PARALLEL_TRY		{
		#define COREARRAY_PARALLEL_CATCH	}
	#endif


        // Parallel Mechanism

		class CParallelBase;

		namespace _INTERNAL
		{
			template<class TCLASS> struct COREARRAY_DLL_DEFAULT _pThreadStructEx
			{
				TCLASS * obj;
				void (TCLASS::*proc)(CdThread *, int);
				int ThreadIndex;
				CParallelBase *cpBase;
			};

			template<class TCLASS> COREARRAY_DLL_DEFAULT
				int _pDoThreadEx(CdThread *Thread, _pThreadStructEx<TCLASS> Data)
			{
				Data.cpBase->InitThread();

				COREARRAY_PARALLEL_TRY
                	(Data.obj->*Data.proc)(Thread, Data.ThreadIndex);
				COREARRAY_PARALLEL_CATCH

				Data.cpBase->DoneThread();
				return 0;
			}
		}


        /// The base class of parallel computing library, multi-thread
		class COREARRAY_DLL_DEFAULT CParallelBase
		{
		public:
			CParallelBase(int _nThread=1);
			virtual ~CParallelBase();

        	/// Initialize each thread
			void InitThread();
			/// Free resource when a thread finishes
			void DoneThread();
			/// Close all threads
			void CloseThreads();
			/// Return the total number of thread used
			COREARRAY_INLINE int nThread() const { return fnThread; }
			/// Reset the number of thread used
			void SetNumThread(int _nThread);
			/// Automatically determine the number of threads used
			void AutoSetnThread();

			COREARRAY_INLINE CdThreadMutex &Mutex() { return fMutex; }

			/// Create nThread threads (including the main thread), and Proc is called by each thread
			typedef void (*TProc)(CdThread *Thread, int, void *);

			void RunThreads(TProc Proc, void *param);

			/// Create nThread threads (including the main thread), and Proc is called by each
			//  thread closure or delegate for C++
			template<class TCLASS>
				void RunThreads(void (TCLASS::*Proc)(CdThread *, int), TCLASS *obj)
			{
				if (!Proc || !obj) return;
				CloseThreads();

				if (fnThread > 1)
				{
					fThreads.resize(fnThread-1);
					for (int i=0; i < fnThread-1; i++)
					{
						_INTERNAL::_pThreadStructEx<TCLASS> pd;
						pd.obj = obj; pd.proc = Proc;
						pd.ThreadIndex = i+1; pd.cpBase = this;
						fThreads[i] = new CdThread;
						fThreads[i]->BeginThread(_INTERNAL::_pDoThreadEx<TCLASS>, pd);
					}
				}

				InitThread();

				COREARRAY_PARALLEL_TRY
					(obj->*Proc)(NULL, 0);
				COREARRAY_PARALLEL_CATCH

				DoneThread();

				for (int i=0; i < fnThread-1; i++)
					fThreads[i]->EndThread();

				CloseThreads();
			}

			COREARRAY_INLINE CdBaseProgression *Progress() const { return fProgress; }
			void SetProgress(CdBaseProgression *Val);
			void SetConsoleProgress(CdBaseProgression::TPercentMode mode = CdBaseProgression::tp01);

		protected:
			int fnThread;
			std::vector<CdThread*> fThreads;
			CdThreadMutex fMutex;
			CdBaseProgression *fProgress;

			COREARRAY_INLINE void ForwardProgress()
			{
				if (fProgress)
				{
					TdAutoMutex AutoMutex(&fMutex);
					fProgress->Forward();
				}
			}
		};


		///

		class COREARRAY_DLL_DEFAULT CParallelSection: public CParallelBase
		{
		public:
        	/// Constructor
			CParallelSection(int _nThread=1);
			/// Destructor
			virtual ~CParallelSection();

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				void RunThreads(
					size_t TotalSize,
					void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &), TCLASS *Obj,
					OUTTYPE *InBuf, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelSection is working.");
				// Initialize
				if (TotalSize <= 0) return;
				if (fProgress) fProgress->Init(TotalSize);

				_IStruct<TCLASS, TINDEX, OUTTYPE> Rec;
				Rec.Obj = Obj; Rec.Proc = Proc; Rec.InBuf = InBuf;
				Rec.Index = StartIndex; Rec.TotalSize = TotalSize;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelSection>(
					&CParallelSection::_pThread<TCLASS, TINDEX, OUTTYPE>, this);
				// finally
				_ptr = NULL;
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				void RunThreads(
					size_t TotalSize,
					void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &),
					void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int),
					TCLASS *Obj,
					OUTTYPE *InBuf, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelSection is working.");
				// Initialize
				if (TotalSize <= 0) return;
				if (fProgress) fProgress->Init(TotalSize);

				_IStructEx<TCLASS, TINDEX, OUTTYPE, THREADDATA> Rec;
				Rec.Obj = Obj; Rec.Proc = Proc;
				Rec.InternalFunc = InternalFunc;
				Rec.InBuf = InBuf;
				Rec.Index = StartIndex; Rec.TotalSize = TotalSize;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelSection>(
					&CParallelSection::_pThreadEx<TCLASS, TINDEX, OUTTYPE, THREADDATA>, this);
				// finally
				_ptr = NULL;
			}

		protected:
			void *_ptr;

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				struct _IStruct
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &);
				OUTTYPE *InBuf;
				TINDEX Index;
				size_t TotalSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				void _pThread(CdThread *Thread, int Index)
			{
				_IStruct<TCLASS, TINDEX, OUTTYPE> &Rec =
                	*((_IStruct<TCLASS, TINDEX, OUTTYPE>*)_ptr);
                TINDEX Idx;
				do {
					fMutex.Lock();
					if (Rec.TotalSize > 0)
					{
						OUTTYPE *pBuf = Rec.InBuf++;
						Idx = Rec.Index; ++Rec.Index; --Rec.TotalSize;
						fMutex.Unlock();
						(Rec.Obj->*Rec.Proc)(Idx, *pBuf);
						ForwardProgress();
					} else {
						fMutex.Unlock();
						break;
					}
				} while (true);
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				struct _IStructEx
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &);
				void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int);
				OUTTYPE *InBuf;
				TINDEX Index;
				size_t TotalSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				void _pThreadEx(CdThread *Thread, int Index)
			{
				_IStructEx<TCLASS, TINDEX, OUTTYPE, THREADDATA> &Rec =
                	*((_IStructEx<TCLASS, TINDEX, OUTTYPE, THREADDATA>*)_ptr);
                // _INTERNAL data for each thread
				THREADDATA ThreadData;
				(Rec.Obj->*Rec.InternalFunc)(ThreadData, Thread, Index);

                TINDEX Idx;
				do {
					fMutex.Lock();
					if (Rec.TotalSize > 0)
					{
						OUTTYPE *pBuf = Rec.InBuf++;
						Idx = Rec.Index; ++Rec.Index; --Rec.TotalSize;
						fMutex.Unlock();
						(Rec.Obj->*Rec.Proc)(Idx, *pBuf, ThreadData);
						ForwardProgress();
					} else {
						fMutex.Unlock();
						break;
					}
				} while (true);
			}
		};



		class COREARRAY_DLL_DEFAULT CParallelSectionEx: public CParallelSection
		{
		public:
        	/// Constructor
			CParallelSectionEx(int _nThread=1): CParallelSection(_nThread) {}

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				void RunThreads(
					size_t TotalSize, size_t SubBufSize,
					void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &), TCLASS *Obj,
					OUTTYPE *InBuf, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelSectionEx is working.");
				if (SubBufSize <= 0)
                	throw ErrParallel("Invalid 'SubBufSize' in the function 'RunThreads'");
				// Initialize
				if (TotalSize <= 0) return;
				if (fProgress) fProgress->Init(TotalSize);

				_IStruct<TCLASS, TINDEX, OUTTYPE> Rec;
				Rec.Obj = Obj; Rec.Proc = Proc; Rec.InBuf = InBuf;
				Rec.Index = StartIndex;
				Rec.TotalSize = TotalSize; Rec.SubBufSize = SubBufSize;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelSectionEx>(
					&CParallelSectionEx::_pThread<TCLASS, TINDEX, OUTTYPE>, this);
				// finally
				_ptr = NULL;
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				void RunThreads(
					size_t TotalSize, size_t SubBufSize,
					void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &),
					void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int),
					TCLASS *Obj,
					OUTTYPE *InBuf, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelSectionEx is working.");
				if (SubBufSize <= 0)
                	throw ErrParallel("Invalid 'SubBufSize' in the function 'RunThreads'");
				// Initialize
				if (TotalSize <= 0) return;
				if (fProgress) fProgress->Init(TotalSize);

				_IStructEx<TCLASS, TINDEX, OUTTYPE, THREADDATA> Rec;
				Rec.Obj = Obj; Rec.Proc = Proc;
				Rec.InternalFunc = InternalFunc;
				Rec.InBuf = InBuf;
				Rec.Index = StartIndex;
				Rec.TotalSize = TotalSize; Rec.SubBufSize = SubBufSize;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelSection>(
					&CParallelSectionEx::_pThreadEx<TCLASS, TINDEX, OUTTYPE, THREADDATA>, this);
				_ptr = NULL;
			}

		protected:
			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				struct _IStruct
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &);
				OUTTYPE *InBuf;
				TINDEX Index;
				size_t TotalSize, SubBufSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				void _pThread(CdThread *Thread, int Index)
			{
				_IStruct<TCLASS, TINDEX, OUTTYPE> &Rec =
                	*((_IStruct<TCLASS, TINDEX, OUTTYPE>*)_ptr);
                TINDEX Idx;
				do {
					fMutex.Lock();
					size_t S = min(Rec.SubBufSize, Rec.TotalSize);
					if (S > 0)
					{
						OUTTYPE *pBuf = Rec.InBuf;
						Rec.InBuf += S;
						Idx = Rec.Index; Rec.Index += S;
						Rec.TotalSize -= S;
						fMutex.Unlock();
						for (size_t i=S; i > 0; i--)
						{
							(Rec.Obj->*Rec.Proc)(Idx, *pBuf++);
							++Idx;
						}
						if (fProgress)
						{
							TdAutoMutex AutoMutex(&fMutex);
							fProgress->Forward(S);
						}
					} else {
						fMutex.Unlock();
						break;
					}
				} while (true);
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				struct _IStructEx
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &);
				void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int);
				OUTTYPE *InBuf;
				TINDEX Index;
				size_t TotalSize, SubBufSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				void _pThreadEx(CdThread *Thread, int Index)
			{
				_IStructEx<TCLASS, TINDEX, OUTTYPE, THREADDATA> &Rec =
                	*((_IStructEx<TCLASS, TINDEX, OUTTYPE, THREADDATA>*)_ptr);
                // _INTERNAL data for each thread
				THREADDATA ThreadData;
				(Rec.Obj->*Rec.InternalFunc)(ThreadData, Thread, Index);

                TINDEX Idx;
				do {
					fMutex.Lock();
					size_t S = min(Rec.SubBufSize, Rec.TotalSize);
					if (S > 0)
					{
						OUTTYPE *pBuf = Rec.InBuf;
						Rec.InBuf += S;
						Idx = Rec.Index; Rec.Index += S;
						Rec.TotalSize -= S;
						fMutex.Unlock();
						for (size_t i = S; i > 0; i--)
						{
							(Rec.Obj->*Rec.Proc)(Idx, *pBuf++, ThreadData);
                            ++Idx;
						}
						if (fProgress)
						{
							TdAutoMutex AutoMutex(&fMutex);
							fProgress->Forward(S);
						}
					} else {
						fMutex.Unlock();
						break;
					}
				} while (true);
			}
		};


		// Queueing model for parallel computing

		class COREARRAY_DLL_DEFAULT CParallelQueue:
			public CParallelBase, protected CdThreadsSuspending
		{
		public:
			CParallelQueue(int _nThread=1);
			virtual ~CParallelQueue();

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
			void RunThreads(
				C_Int64 TotalSize, ssize_t BufSize,
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &),
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *buf, size_t nbuf),
				TCLASS *Obj, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelQueue is working.");
				if (BufSize <= 0)
                    throw ErrParallel("The size of buffer should be > 0.");
				if (TotalSize <= 0) return;
				if (BufSize > TotalSize) BufSize = TotalSize;
				if (fProgress) fProgress->Init(TotalSize);
				// Initialize
				std::vector<OUTTYPE> Buffer(BufSize);
				_IStruct<TCLASS, TINDEX, OUTTYPE> Rec;
				Rec.Obj = Obj;
				Rec.Proc = Proc; Rec.QueueFunc = QueueFunc;
				Rec.Buffer = &(Buffer[0]);
				Rec.Idx = Rec.IdxBase = StartIndex;
				Rec.TotalSize = TotalSize;
				Rec.BufSize = Rec.IndexEnd = BufSize;
				Rec.IndexBase = Rec.WorkingIndex = Rec.FinishIndex = 0;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelQueue>(
					&CParallelQueue::_pThread<TCLASS, TINDEX, OUTTYPE>, this);
				_ptr = NULL;
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
			void RunThreads(
				C_Int64 TotalSize, ssize_t BufSize,
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &),
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *buf, size_t nbuf),
				void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int),
				TCLASS *Obj, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelQueue is working.");
				if (BufSize <= 0)
                    throw ErrParallel("The size of buffer should be > 0.");
				if (TotalSize <= 0) return;
				if (BufSize > TotalSize) BufSize = TotalSize;
				if (fProgress) fProgress->Init(TotalSize);
				// Initialize
				std::vector<OUTTYPE> Buffer(BufSize);
				_IStruct2<TCLASS, TINDEX, OUTTYPE, THREADDATA> Rec;
				Rec.Obj = Obj;
				Rec.Proc = Proc; Rec.QueueFunc = QueueFunc; Rec.InternalFunc = InternalFunc;
				Rec.Buffer = &(Buffer[0]);
				Rec.Idx = Rec.IdxBase = StartIndex;
				Rec.TotalSize = TotalSize;
				Rec.BufSize = Rec.IndexEnd = BufSize;
				Rec.IndexBase = Rec.WorkingIndex = Rec.FinishIndex = 0;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelQueue>(
					&CParallelQueue::_pThread2<TCLASS, TINDEX, OUTTYPE, THREADDATA>, this);
				_ptr = NULL;
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
			void RunThreads(
				C_Int64 TotalSize, ssize_t BufSize,
				void (TCLASS::*Proc)(CdThread *, int, const TINDEX &, OUTTYPE &),
				void (TCLASS::*QueueFunc)(CdThread *, int, const TINDEX &, OUTTYPE *buf, size_t nbuf),
				TCLASS *Obj, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelQueue is working.");
				if (BufSize <= 0)
                    throw ErrParallel("The size of buffer should be > 0.");
				if (TotalSize <= 0) return;
				if (BufSize > TotalSize) BufSize = TotalSize;
				if (fProgress) fProgress->Init(TotalSize);
				// Initialize
				std::vector<OUTTYPE> Buffer(BufSize);
				_IStructEx<TCLASS, TINDEX, OUTTYPE> Rec;
				Rec.Obj = Obj;
				Rec.Proc = Proc; Rec.QueueFunc = QueueFunc;
				Rec.Buffer = &(Buffer[0]);
				Rec.Idx = Rec.IdxBase = StartIndex;
				Rec.TotalSize = TotalSize;
				Rec.BufSize = Rec.IndexEnd = BufSize;
				Rec.IndexBase = Rec.WorkingIndex = Rec.FinishIndex = 0;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelQueue>(
					&CParallelQueue::_pThreadEx<TCLASS, TINDEX, OUTTYPE>, this);
				_ptr = NULL;
			}

		protected:
			void *_ptr;

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				struct _IStruct
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &);
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *buf, size_t nbuf);
				OUTTYPE *Buffer;
				TINDEX Idx, IdxBase;
				C_Int64 TotalSize, IndexBase, IndexEnd;
				C_Int64 WorkingIndex, FinishIndex;
				ssize_t BufSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				void _pThread(CdThread *Thread, int Index)
			{
				_IStruct<TCLASS, TINDEX, OUTTYPE> &Rec =
					*((_IStruct<TCLASS, TINDEX, OUTTYPE>*)_ptr);
				TINDEX Idx;
				do {
					fMutex.Lock();
					if (Rec.WorkingIndex >= Rec.IndexEnd)
					{
						if (Rec.IndexEnd >= Rec.TotalSize)
						{
							fMutex.Unlock(); break;  // exit do-loop
						} else {
							fMutex.Unlock(); Suspend();
                        }
					} else {
						OUTTYPE *pBuf = Rec.Buffer + (Rec.WorkingIndex-Rec.IndexBase);
						Rec.WorkingIndex++;
						Idx = Rec.Idx; ++Rec.Idx;
						fMutex.Unlock();
						// call ...
						(Rec.Obj->*Rec.Proc)(Idx, *pBuf);
						// update FinishIndex
						{
							TdAutoMutex Auto(&fMutex);
							if (fProgress)
								fProgress->Forward();
							if ((++Rec.FinishIndex) >= Rec.IndexEnd)
							{
								size_t OldSize = Rec.IndexEnd - Rec.IndexBase;
								C_Int64 L = std::min(Rec.TotalSize-Rec.IndexEnd, (C_Int64)Rec.BufSize);
								Rec.IndexBase = Rec.IndexEnd;
								Rec.IndexEnd += L;
								Idx = Rec.IdxBase; Rec.IdxBase += L;
								(Rec.Obj->*Rec.QueueFunc)(Idx, Rec.Buffer, OldSize);
								WakeUp();
							}
						}
					}
				} while (true);
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				struct _IStruct2
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &);
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *, size_t);
				void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int);
				OUTTYPE *Buffer;
				TINDEX Idx, IdxBase;
				C_Int64 TotalSize, IndexBase, IndexEnd;
				C_Int64 WorkingIndex, FinishIndex;
				ssize_t BufSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				void _pThread2(CdThread *Thread, int Index)
			{
				_IStruct2<TCLASS, TINDEX, OUTTYPE, THREADDATA> &Rec =
					*((_IStruct2<TCLASS, TINDEX, OUTTYPE, THREADDATA>*)_ptr);
                // _INTERNAL data for each thread
				THREADDATA ThreadData;
				(Rec.Obj->*Rec.InternalFunc)(ThreadData, Thread, Index);

				TINDEX Idx;
				do {
					fMutex.Lock();
					if (Rec.WorkingIndex >= Rec.IndexEnd)
					{
						if (Rec.IndexEnd >= Rec.TotalSize)
						{
							fMutex.Unlock(); break;  // exit do-loop
						} else {
							fMutex.Unlock(); Suspend();
                        }
					} else {
						OUTTYPE *pBuf = Rec.Buffer + (Rec.WorkingIndex-Rec.IndexBase);
						Rec.WorkingIndex++;
						Idx = Rec.Idx; ++Rec.Idx;
						fMutex.Unlock();
						// call ...
						(Rec.Obj->*Rec.Proc)(Idx, *pBuf, ThreadData);
						// update FinishIndex
						{
							TdAutoMutex Auto(&fMutex);
							if (fProgress)
								fProgress->Forward();
							if ((++Rec.FinishIndex) >= Rec.IndexEnd)
							{
								size_t OldSize = Rec.IndexEnd - Rec.IndexBase;
								C_Int64 L = std::min(Rec.TotalSize-Rec.IndexEnd, (C_Int64)Rec.BufSize);
								Rec.IndexBase = Rec.IndexEnd;
								Rec.IndexEnd += L;
								Idx = Rec.IdxBase; Rec.IdxBase += L;
								(Rec.Obj->*Rec.QueueFunc)(Idx, Rec.Buffer, OldSize);
								WakeUp();
							}
						}
					}
				} while (true);
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				struct _IStructEx
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(CdThread *, int, const TINDEX &, OUTTYPE &);
				void (TCLASS::*QueueFunc)(CdThread *, int, const TINDEX &, OUTTYPE *, size_t);
				OUTTYPE *Buffer;
				TINDEX Idx, IdxBase;
				C_Int64 TotalSize, IndexBase, IndexEnd;
				C_Int64 WorkingIndex, FinishIndex;
				ssize_t BufSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				void _pThreadEx(CdThread *Thread, int Index)
			{
				_IStructEx<TCLASS, TINDEX, OUTTYPE> &Rec =
					*((_IStructEx<TCLASS, TINDEX, OUTTYPE>*)_ptr);
				TINDEX Idx;
				do {
					fMutex.Lock();
					if (Rec.WorkingIndex >= Rec.IndexEnd)
					{
						if (Rec.IndexEnd >= Rec.TotalSize)
						{
							fMutex.Unlock(); break;  // exit do-loop
						} else {
							fMutex.Unlock(); Suspend();
                        }
					} else {
						OUTTYPE *pBuf = Rec.Buffer + (Rec.WorkingIndex-Rec.IndexBase);
						Rec.WorkingIndex++;
						Idx = Rec.Idx; ++Rec.Idx;
						fMutex.Unlock();
						// call ...
						(Rec.Obj->*Rec.Proc)(Thread, Index, Idx, *pBuf);
						// update FinishIndex
						{
							TdAutoMutex Auto(&fMutex);
							if (fProgress)
								fProgress->Forward();
							if ((++Rec.FinishIndex) >= Rec.IndexEnd)
							{
								size_t OldSize = Rec.IndexEnd - Rec.IndexBase;
								C_Int64 L = std::min(Rec.TotalSize-Rec.IndexEnd, (C_Int64)Rec.BufSize);
								Rec.IndexBase = Rec.IndexEnd;
								Rec.IndexEnd += L;
								Idx = Rec.IdxBase; Rec.IdxBase += L;
								(Rec.Obj->*Rec.QueueFunc)(Thread, Index, Idx, Rec.Buffer, OldSize);
								WakeUp();
							}
						}
					}
				} while (true);
			}
		};


		class COREARRAY_DLL_DEFAULT CParallelQueueEx: public CParallelQueue
		{
    	public:
			CParallelQueueEx(int _nThread=1);

			template<class TCLASS, typename TINDEX, typename OUTTYPE>
			void RunThreads(
				C_Int64 TotalSize, ssize_t BufSize, ssize_t SubBufSize,
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &),
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *buf, size_t nbuf),
				TCLASS *Obj, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelQueueEx is working.");
				if (BufSize <= 0)
					throw ErrParallel("The size of buffer should be > 0.");
				if (SubBufSize <= 0)
					throw ErrParallel("The size of sub buffer should be > 0.");
				if (TotalSize <= 0) return;
				if (BufSize > TotalSize) BufSize = TotalSize;
				if (SubBufSize > BufSize) SubBufSize = BufSize;
				if (fProgress) fProgress->Init(TotalSize);
				// Initialize
				std::vector<OUTTYPE> Buffer(BufSize);
				_IStruct<TCLASS, TINDEX, OUTTYPE> Rec;
				Rec.Obj = Obj;
				Rec.Proc = Proc; Rec.QueueFunc = QueueFunc;
				Rec.Buffer = &(Buffer[0]);
				Rec.Idx = Rec.IdxBase = StartIndex;
				Rec.TotalSize = TotalSize;
				Rec.BufSize = Rec.IndexEnd = BufSize;
				Rec.SubBufSize = SubBufSize;
				Rec.IndexBase = Rec.WorkingIndex = Rec.FinishIndex = 0;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelQueueEx>(
					&CParallelQueueEx::_pThread<TCLASS, TINDEX, OUTTYPE>, this);
				_ptr = NULL;
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
			void RunThreads(
				C_Int64 TotalSize, ssize_t BufSize, ssize_t SubBufSize,
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &),
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *buf, size_t nbuf),
				void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int),
				TCLASS *Obj, const TINDEX StartIndex = 0)
			{
				if (_ptr)
					throw ErrParallel("CParallelQueue is working.");
				if (BufSize <= 0)
                    throw ErrParallel("The size of buffer should be > 0.");
				if (TotalSize <= 0) return;
				if (BufSize > TotalSize) BufSize = TotalSize;
				if (fProgress) fProgress->Init(TotalSize);
				// Initialize
				std::vector<OUTTYPE> Buffer(BufSize);
				_IStruct2<TCLASS, TINDEX, OUTTYPE, THREADDATA> Rec;
				Rec.Obj = Obj;
				Rec.Proc = Proc; Rec.QueueFunc = QueueFunc; Rec.InternalFunc = InternalFunc;
				Rec.Buffer = &(Buffer[0]);
				Rec.Idx = Rec.IdxBase = StartIndex;
				Rec.TotalSize = TotalSize;
				Rec.BufSize = Rec.IndexEnd = BufSize;
				Rec.IndexBase = Rec.WorkingIndex = Rec.FinishIndex = 0;
                // Working
				_ptr = (void*)&Rec;
				CParallelBase::RunThreads<CParallelQueue>(
					&CParallelQueue::_pThread2<TCLASS, TINDEX, OUTTYPE, THREADDATA>, this);
				_ptr = NULL;
			}

		protected:
			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				struct _IStruct
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &);
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *buf, size_t nbuf);
				OUTTYPE *Buffer;
				TINDEX Idx, IdxBase;
				C_Int64 TotalSize, IndexBase, IndexEnd;
				C_Int64 WorkingIndex, FinishIndex;
				ssize_t BufSize, SubBufSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE>
				void _pThread(CdThread *Thread, int Index)
			{
				_IStruct<TCLASS, TINDEX, OUTTYPE> &Rec =
					*((_IStruct<TCLASS, TINDEX, OUTTYPE>*)_ptr);
				TINDEX Idx;
				do {
					fMutex.Lock();
					if (Rec.WorkingIndex >= Rec.IndexEnd)
					{
						if (Rec.IndexEnd >= Rec.TotalSize)
						{
							fMutex.Unlock(); break;  // exit do-loop
						} else {
							fMutex.Unlock(); Suspend();
                        }
					} else {
						ssize_t tmpL = Rec.IndexEnd - Rec.WorkingIndex;
						if (tmpL > Rec.SubBufSize)
                        	tmpL = Rec.SubBufSize;
						OUTTYPE *pBuf = Rec.Buffer + (Rec.WorkingIndex-Rec.IndexBase);
						Rec.WorkingIndex += tmpL;
						Idx = Rec.Idx; Rec.Idx += tmpL;
						fMutex.Unlock();
						// call ...
						for (ssize_t i = tmpL; i > 0; i--)
						{
							(Rec.Obj->*Rec.Proc)(Idx, *pBuf);
							++Idx; pBuf++;
                        }
						// update FinishIndex
						{
							TdAutoMutex Auto(&fMutex);
							if (fProgress)
								fProgress->Forward(tmpL);
							Rec.FinishIndex += tmpL;
							if (Rec.FinishIndex >= Rec.IndexEnd)
							{
								size_t OldSize = Rec.IndexEnd - Rec.IndexBase;
								C_Int64 L = std::min(Rec.TotalSize-Rec.IndexEnd, (C_Int64)Rec.BufSize);
								Rec.IndexBase = Rec.IndexEnd;
								Rec.IndexEnd += L;
								Idx = Rec.IdxBase; Rec.IdxBase += L;
								(Rec.Obj->*Rec.QueueFunc)(Idx, Rec.Buffer, OldSize);
								WakeUp();
							}
						}
					}
				} while (true);
			}

			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				struct _IStruct2
			{
				TCLASS *Obj;
				void (TCLASS::*Proc)(const TINDEX &, OUTTYPE &, THREADDATA &);
				void (TCLASS::*QueueFunc)(const TINDEX &, OUTTYPE *, size_t);
				void (TCLASS::*InternalFunc)(THREADDATA &, CdThread *, int);
				OUTTYPE *Buffer;
				TINDEX Idx, IdxBase;
				C_Int64 TotalSize, IndexBase, IndexEnd;
				C_Int64 WorkingIndex, FinishIndex;
				ssize_t BufSize, SubBufSize;
			};
			template<class TCLASS, typename TINDEX, typename OUTTYPE, typename THREADDATA>
				void _pThread2(CdThread *Thread, int Index)
			{
				_IStruct2<TCLASS, TINDEX, OUTTYPE, THREADDATA> &Rec =
					*((_IStruct2<TCLASS, TINDEX, OUTTYPE, THREADDATA>*)_ptr);
                // _INTERNAL data for each thread
				THREADDATA ThreadData;
				(Rec.Obj->*Rec.InternalFunc)(ThreadData, Thread, Index);

				TINDEX Idx;
				do {
					fMutex.Lock();
					if (Rec.WorkingIndex >= Rec.IndexEnd)
					{
						if (Rec.IndexEnd >= Rec.TotalSize)
						{
							fMutex.Unlock(); break;  // exit do-loop
						} else {
							fMutex.Unlock(); Suspend();
                        }
					} else {
						ssize_t tmpL = Rec.IndexEnd - Rec.WorkingIndex;
						if (tmpL > Rec.SubBufSize)
                        	tmpL = Rec.SubBufSize;
						OUTTYPE *pBuf = Rec.Buffer + (Rec.WorkingIndex-Rec.IndexBase);
						Rec.WorkingIndex += tmpL;
						Idx = Rec.Idx; Rec.Idx += tmpL;
						fMutex.Unlock();
						// call ...
						for (ssize_t i = tmpL; i > 0; i--)
						{
							(Rec.Obj->*Rec.Proc)(Idx, *pBuf, ThreadData);
							++Idx; pBuf++;
						}
						// update FinishIndex
						{
							TdAutoMutex Auto(&fMutex);
							if (fProgress)
								fProgress->Forward(tmpL);
							Rec.FinishIndex += tmpL;
							if (Rec.FinishIndex >= Rec.IndexEnd)
							{
								size_t OldSize = Rec.IndexEnd - Rec.IndexBase;
								C_Int64 L = std::min(Rec.TotalSize-Rec.IndexEnd, (C_Int64)Rec.BufSize);
								Rec.IndexBase = Rec.IndexEnd;
								Rec.IndexEnd += L;
								Idx = Rec.IdxBase; Rec.IdxBase += L;
								(Rec.Obj->*Rec.QueueFunc)(Idx, Rec.Buffer, OldSize);
								WakeUp();
							}
						}
					}
				} while (true);
			}
        };
	}
}

#endif  /* _HEADER_PARALLEL_ */

