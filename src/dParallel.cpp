// ===========================================================
//     _/_/_/   _/_/_/  _/_/_/_/    _/_/_/_/  _/_/_/   _/_/_/
//      _/    _/       _/             _/    _/    _/   _/   _/
//     _/    _/       _/_/_/_/       _/    _/    _/   _/_/_/
//    _/    _/       _/             _/    _/    _/   _/
// _/_/_/   _/_/_/  _/_/_/_/_/     _/     _/_/_/   _/_/
// ===========================================================
//
// dParallel.cpp: Functions for parallel computing
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


#include <dParallel.h>


namespace CoreArray
{
	namespace Parallel
	{
		namespace _INTERNAL
		{
			struct _pThreadStruct
			{
				void (*proc)(CoreArray::CdThread *, int, void*);
				int ThreadIndex;
				void *Param;
				CoreArray::Parallel::CParallelBase *cpBase;
			};

			int _pDoThread(CoreArray::CdThread *Thread, _pThreadStruct Data)
			{
				Data.cpBase->InitThread();

				COREARRAY_PARALLEL_TRY
					Data.proc(Thread, Data.ThreadIndex, Data.Param);
				COREARRAY_PARALLEL_CATCH

				Data.cpBase->DoneThread();
				return 0;
			}
		}
	}
}

using namespace std;
using namespace CoreArray;
using namespace CoreArray::Parallel;


// CdBaseProgression

static const char *errPercentMode = "Invalid TPercentMode value!";
static const int TotalProg[4] = { 100, 10, 4, 2 };
static const int IncProg[4] = { 1, 10, 25, 50 };

CdBaseProgression::CdBaseProgression(TPercentMode permode)
{
	if ((permode < tp01) || (permode > tp50))
		throw ErrCoreArray(errPercentMode);
	fMode = permode;
	Init(0);
}

void CdBaseProgression::Init(Int64 TotalCnt)
{
	if (TotalCnt < 0) TotalCnt = 0;
	fTotal = TotalCnt;

	long double step = TotalCnt / double(TotalProg[fMode]);
	long double start = 0.1;
	int i;
	for (i=0; i < TotalProg[fMode]; i++)
	{
		start += step;
		vProg[i] = (Int64)(start);
	}
	vProg[i] = TdTraits<Int64>::Max();
    vCurrent = 0;
	vptrProg = &vProg[0];
	fPercent = 0;
}

bool CdBaseProgression::Forward(Int64 step)
{
	vCurrent += step;
	if (vCurrent >= *vptrProg)
	{
		while (vCurrent >= *vptrProg)
        {
			vptrProg++;
			fPercent += IncProg[fMode];
		}
        ShowProgress();
		return true;
	}
	return false;
}

void CdBaseProgression::ShowProgress()
{
	// do nothing ...
}

void CdBaseProgression::SetMode(TPercentMode mode)
{
	if ((mode < tp01) || (mode > tp50))
		throw ErrCoreArray(errPercentMode);
	fMode = mode;
	Init(fTotal);
}


// CdConsoleProgress

CdConsoleProgress::CdConsoleProgress(TPercentMode permode):
	CdBaseProgression(permode)
{ }

void CdConsoleProgress::ShowProgress()
{
	#ifndef COREARRAY_NO_STD_IN_OUT

	if (Info.empty())
	{
		cout << NowDateToStr() << "\t" << fPercent << "%" << endl;
	} else {
		cout << Info << "\t" << NowDateToStr() << "\t" << fPercent << "%" << endl;
    }

	#endif
}


// CParallelBase

static const char *errNThread = "Invalid # of threads (%d)";

CParallelBase::CParallelBase(int _nThread)
{
	if (_nThread < 1)
		throw ErrParallel(errNThread, _nThread);
	fnThread = _nThread;
	fProgress = NULL;
}

CParallelBase::~CParallelBase()
{
	CloseThreads();
}

void CParallelBase::InitThread()
{
	DisableFPUException();
}

void CParallelBase::DoneThread()
{
	// do nothing ...
}

void CParallelBase::CloseThreads()
{
	vector<CdThread*>::iterator it;
	for (it=fThreads.begin(); it!=fThreads.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
	fThreads.clear();
}

void CParallelBase::SetnThread(int _nThread)
{
	CloseThreads();
	if (_nThread < 1)
    	throw ErrParallel(errNThread, _nThread);
	fnThread = _nThread;
}

void CParallelBase::AutoSetnThread()
{
	fnThread = Mach::GetNumberOfCPU();
	if (fnThread < 1) fnThread = 1;
}

void CParallelBase::DoThreads(CParallelBase::TProc Proc, void *param)
{
	if (!Proc) return;
	CloseThreads();

	if (fnThread > 1)
	{
		fThreads.resize(fnThread-1);
		for (int i=0; i < fnThread-1; i++)
		{
			_INTERNAL::_pThreadStruct pd;
			pd.proc = Proc;
			pd.ThreadIndex = i+1;
			pd.cpBase = this;
			pd.Param = param;
			fThreads[i] = new CdThread;
			fThreads[i]->BeginThread(_INTERNAL::_pDoThread, pd);
		}
	}

	InitThread();

	COREARRAY_PARALLEL_TRY
		Proc(NULL, 0, param);
	COREARRAY_PARALLEL_CATCH

	DoneThread();

	for (int i=0; i < fnThread-1; i++)
		fThreads[i]->EndThread();

	CloseThreads();
}

void CParallelBase::SetProgress(CdBaseProgression *Val)
{
	if (fProgress) delete fProgress;
	fProgress = Val;
}

void CParallelBase::SetConsoleProgress(CdBaseProgression::TPercentMode mode)
{
	if (dynamic_cast<CdConsoleProgress*>(fProgress))
    	return;
	if (fProgress) delete fProgress;
    fProgress = new CdConsoleProgress(mode);
}


// CParallelSection

CParallelSection::CParallelSection(int _nThread): CParallelBase(_nThread)
{
	_ptr = NULL;
}

CParallelSection::~CParallelSection() {}


// CParallelQueue

CParallelQueue::CParallelQueue(int _nThread):
	CParallelBase(_nThread), CdThreadsSuspending()
{
	_ptr = NULL;
}

CParallelQueue::~CParallelQueue() {}


// CParallelQueueEx

CParallelQueueEx::CParallelQueueEx(int _nThread):
	CParallelQueue(_nThread)
{}
