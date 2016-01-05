#ifndef bwm_process_h_
#define bwm_process_h_

#ifdef _MSC_VER
#pragma once
#endif // _MSC_VER

#if 0
#ifndef __AFXWIN_H__
   #error include 'StdAfx.h' before including this file for PCH
#endif
#endif // 0

class CWinThread;

class bwm_process
{
 public:
  bwm_process();
  ~bwm_process();

  // overridable
  virtual void RunBackgroundTask() = 0;

  int StartBackgroundTask();
  int StopBackgroundTask(int nTimeOut=0);
  bool CheckForBackgroundTaskAbort();
  bool IsBackgroundTaskRunning();

  /////////////////////////////////////////////////////////////////////////////
  // Internal data
 public:
  CWinThread *lpBackgroundThread;
  void* hKillBackgroundThreadEvent;
  void* hDoneBackgroundThreadEvent;
};

#endif
