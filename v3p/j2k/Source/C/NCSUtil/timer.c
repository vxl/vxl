/********************************************************
** Copyright 1999 Earth Resource Mapping Ltd.
** This document contains proprietary source code of
** Earth Resource Mapping Ltd, and can only be used under
** one of the three licenses as described in the 
** license.txt file supplied with this distribution. 
** See separate license.txt file for license details 
** and conditions.
**
** This software is covered by US patent #6,442,298,
** #6,102,897 and #6,633,688.  Rights to use these patents 
** is included in the license agreements.
**
** FILE:   	NCSUtil/timer.h
** CREATED:	Wed Apr 21 00:00:00 WST 1999
** AUTHOR: 	Simon Cope
** PURPOSE:	NCS Timer handling.
** EDITS:
** [01]	 ny	31Oct00	Merge WinCE/PALM SDK changes
** [02] mjs 18Feb03 Added NCSTimerGetData() and NCSTimerSetData() functions.
 *******************************************************/

#include "NCSTimer.h"

struct NCSTimer {
	NCSThread		tid;
	BOOLEAN			bRun;
	BOOLEAN			bClear;
	BOOLEAN			bRunning;
	void			(*pFunc)(void*);
	void			*pData;
	NCSTimeStampMs	tsNextEvent;
	NCSTimeStampMs	tsPeriod;
};

static void NCSTimerFunc(NCSTimer *pTimer);

/*
** Create a Timer.  Actual timer period is tsPeriod + exec time of pFunc.
*/
NCSTimer *NCSTimerCreate(NCSTimeStampMs	tsPeriod,
						 void			(*pFunc)(void*),
						 void			*pData)
{
	NCSTimer *pTimer;

	if((pTimer = (NCSTimer *)NCSMalloc(sizeof(NCSTimer), TRUE)) != NULL) {
		pTimer->tsPeriod = tsPeriod;
		pTimer->pFunc = pFunc;
		pTimer->pData = pData;

		pTimer->bRun = TRUE;
		pTimer->bClear = FALSE;
		pTimer->bRunning = FALSE;

		if(NCSThreadSpawn(&(pTimer->tid), (void(*)(void*))NCSTimerFunc, pTimer, FALSE)) {
			return(pTimer);
		}
		NCSFree(pTimer);
	}
	return((NCSTimer*)NULL);
}

/*
** Start the given timer.
*/
void NCSTimerStart(NCSTimer *pTimer)
{
	if(pTimer && !pTimer->bRunning) {
		pTimer->tsNextEvent = NCSGetTimeStampMs() + pTimer->tsPeriod;
		pTimer->bRunning = TRUE;
	}
}

/*
** Stop the given timer.
*/
void NCSTimerStop(NCSTimer *pTimer)
{
	if(pTimer) {
		pTimer->bRunning = FALSE;
	}
}

/*
** Destroy the given timer.
*/
void NCSTimerDestroy(NCSTimer *pTimer)
{
	if(pTimer) {
		pTimer->bRun = FALSE;

		while(!pTimer->bClear && NCSThreadIsRunning(&(pTimer->tid))) {
			NCSSleep(50);
		}
		NCSThreadFreeInfo(&(pTimer->tid));
		NCSFree(pTimer);
	}
}

/*
** Get the data pointer for this timer.
*/
void *NCSTimerGetData(NCSTimer *pTimer)
{
	return (pTimer == NULL ? NULL : pTimer->pData);
}

/*
** Set the new data pointer for this timer.
*/
void NCSTimerSetData(NCSTimer *pTimer, void *pData)
{
	if (pTimer) {
		pTimer->pData = pData;
	}
}

/*
** Timer thread function.
*/
static void NCSTimerFunc(NCSTimer *pTimer)
{
	if(pTimer) {
		while(pTimer->bRun) {
			if((NCSGetTimeStampMs() > pTimer->tsNextEvent) && pTimer->bRunning) {
				(*pTimer->pFunc)(pTimer->pData);
				pTimer->tsNextEvent = NCSGetTimeStampMs() + pTimer->tsPeriod;
			} else {
				NCSSleep(100);
			}
		}
		pTimer->bClear = TRUE;
	}
}
