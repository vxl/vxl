#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "bwm_process.h"

static UINT RunBackgroundTaskProc(LPVOID pParam);

bwm_process::bwm_process()
{
  HANDLE hKillBackgroundThreadEvent = hDoneBackgroundThreadEvent = NULL;
  lpBackgroundThread = NULL;
  return;
}

bwm_process::~bwm_process()
{
  StopBackgroundTask();
  return;
}

int bwm_process::StartBackgroundTask()
{
  CWinThread *lpBackgroundThread = 0;
  if (lpBackgroundThread != NULL) {
    if (IsBackgroundTaskRunning() != FALSE)
      return -1;
    // If the thread has ended call StopBackgroundTask in order
    // to cleanup some stuff
    StopBackgroundTask();
  }
  //create kill thread event
  hKillBackgroundThreadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  if (hKillBackgroundThreadEvent == NULL)
    return -2;
  //create work-done thread event
  hDoneBackgroundThreadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  if (hDoneBackgroundThreadEvent == NULL) {
    ::CloseHandle(hKillBackgroundThreadEvent);
    hKillBackgroundThreadEvent = NULL;
    return -3;
  }
  //create thread itself
  lpBackgroundThread = AfxBeginThread(RunBackgroundTaskProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  if (lpBackgroundThread == NULL) {
    ::CloseHandle(hDoneBackgroundThreadEvent);
    hDoneBackgroundThreadEvent = NULL;
    ::CloseHandle(hKillBackgroundThreadEvent);
    hKillBackgroundThreadEvent = NULL;
    return -4;
  }
  lpBackgroundThread->m_bAutoDelete = FALSE;
  //resume thread
  ::ResumeThread(lpBackgroundThread->m_hThread);
  return 1;
}

int bwm_process::StopBackgroundTask(int nTimeOut)
{
  MSG msg;
  DWORD dwExitCode;
  int tim, nTimeCounter;

  if (lpBackgroundThread == NULL)
    return -1;
  //I dont know why sometimes finished threads don't get signalled
  if ((::WaitForSingleObject(hDoneBackgroundThreadEvent, 0) != WAIT_OBJECT_0) &&
      (::GetExitCodeThread(lpBackgroundThread->m_hThread, &dwExitCode) && dwExitCode==STILL_ACTIVE))
  {
    //thread is still active...
    //send kill event
    ::SetEvent(hKillBackgroundThreadEvent);
    //wait for thread completion
    for (nTimeCounter=nTimeOut;;)
    {
      if (nTimeOut > 0) {
        if ((tim = nTimeCounter) > 100)
          tim = 100;
      }
      else
        tim = 0;
      if (::WaitForSingleObject(hDoneBackgroundThreadEvent, tim) == WAIT_OBJECT_0)
        break;
      if (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) != 0) {
        if (AfxGetThread()->PumpMessage() == FALSE) {
          ::TerminateThread(lpBackgroundThread->m_hThread, 0);
          ::PostQuitMessage(msg.wParam);
          break;
        }
      }
      if (::GetExitCodeThread(lpBackgroundThread->m_hThread, &dwExitCode) && dwExitCode!=STILL_ACTIVE)
        break;
      if (nTimeOut > 0) {
        if ((nTimeCounter -= tim) == 0) {
          ::TerminateThread(lpBackgroundThread->m_hThread, 0);
          break;
        }
      }
    }
  }
  //cleanup
  if (lpBackgroundThread != NULL) {
    delete lpBackgroundThread;
    lpBackgroundThread = NULL;
  }
  if (hDoneBackgroundThreadEvent != NULL) {
    ::CloseHandle(hDoneBackgroundThreadEvent);
    hDoneBackgroundThreadEvent = NULL;
  }
  if (hKillBackgroundThreadEvent != NULL) {
    ::CloseHandle(hKillBackgroundThreadEvent);
    hKillBackgroundThreadEvent = NULL;
  }
  return 1;
}

bool bwm_process::CheckForBackgroundTaskAbort()
{
  if (lpBackgroundThread == NULL)
    return TRUE;
  if (::WaitForSingleObject(hKillBackgroundThreadEvent, 0) == WAIT_OBJECT_0)
    return TRUE;
  return FALSE;
}

bool bwm_process::IsBackgroundTaskRunning()
{
  DWORD dwExitCode;

  if (lpBackgroundThread == NULL)
    return FALSE;
  if (::WaitForSingleObject(hDoneBackgroundThreadEvent, 0) == WAIT_OBJECT_0)
    return FALSE;
  if (::GetExitCodeThread(lpBackgroundThread->m_hThread, &dwExitCode) && dwExitCode!=STILL_ACTIVE)
    return FALSE;
  return TRUE;
}

static UINT RunBackgroundTaskProc(LPVOID pParam)
{
  bwm_process *bt_dlg;

  bt_dlg = (bwm_process*)pParam;
  bt_dlg->RunBackgroundTask();
  ::SetEvent(bt_dlg->hDoneBackgroundThreadEvent);
  return 1;
}
