/********************************************************
** Copyright 2000 Earth Resource Mapping Ltd.
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
********************************************************/

#include <windows.h>
#ifndef NCS_MINDEP_BUILD
#include <excpt.h>
#if !defined(_WIN32_WCE)
#include <imagehlp.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#endif //!NCS_MINDEP_BUILD
#include "NCSUtil.h"

#ifndef NCS_MINDEP_BUILD
#define MakePtr(cast,ptr,addValue)      (cast)( (DWORD)(ptr) + (DWORD)(addValue))
#define MakePtrDiff(cast,ptr,subValue ) (cast)( (DWORD)(ptr) - (DWORD)(subValue))
#define FRAME_SIZE                      (sizeof(DWORD) * 2)

static void Printf(char *pMessage,const char *sFmt,...)
{
	char buf[1024];
	va_list argptr;

	va_start(argptr,sFmt);
	vsprintf(buf, sFmt, argptr);
	va_end(argptr);

	strcat(pMessage, buf);
}

static void MemoryDump(void *pIP, char *pMessage, int nBytes)
{
	if(pIP) {
		unsigned char buf[1024];
		SIZE_T dwRead = 0;

		strcat(pMessage, "Dump: ");
		if(ReadProcessMemory(GetCurrentProcess(),
							 (const VOID*)pIP,
							 buf, min(nBytes, sizeof(buf)),
							 &dwRead)) {
			DWORD i;

			for(i = 0; i < dwRead; i++) {
				char msg[16];
				sprintf(msg, "%.2lx", (unsigned long)buf[i]);
				strcat(pMessage, msg);
			}
		} else {
			strcat(pMessage, "N/A");
		}
		strcat(pMessage, "\n");
	}
}

static void StackTracer(CONTEXT *pCtx, char *pMessage, int MaxFrames)
{
#if !defined(_WIN32_WCE)
  CONTEXT     Ctx;
  STACKFRAME  Stk;
#ifdef _M_AMD64
  DWORD64       symDisplacement,dwInstance;
#else
  DWORD       symDisplacement,dwInstance;
#endif
  HANDLE      hProcess = GetCurrentProcess();
  int nFrames;
  char Path[MAX_PATH];
  static BYTE    symbolBuffer[ sizeof(IMAGEHLP_SYMBOL) + 512 ];

  Printf(pMessage,"Stack Trace:\n");

  // initialise IMAGEHLP
  SymInitialize(hProcess,NULL,TRUE);

  // init the argument contexts.
  memset(&Stk,0,sizeof(STACKFRAME));
  memset(&Ctx,0,sizeof(CONTEXT));	
  
  // funny - if I copy the struct wholesale I get crashes...
#ifdef _M_AMD64
  Ctx.Rsp = pCtx->Rsp;
  Ctx.Rbp = pCtx->Rbp;
  Ctx.Rip = pCtx->Rip;
  Stk.AddrStack.Offset = pCtx->Rsp;
  Stk.AddrStack.Mode   = AddrModeFlat;
  Stk.AddrFrame.Offset = pCtx->Rbp;
  Stk.AddrFrame.Mode   = AddrModeFlat;
  Stk.AddrPC.Offset    = pCtx->Rip;
  Stk.AddrPC.Mode      = AddrModeFlat;    
#else
  Ctx.Esp = pCtx->Esp;
  Ctx.Ebp = pCtx->Ebp;
  Ctx.Eip = pCtx->Eip;
  Stk.AddrStack.Offset = pCtx->Esp;
  Stk.AddrStack.Mode   = AddrModeFlat;
  Stk.AddrFrame.Offset = pCtx->Ebp;
  Stk.AddrFrame.Mode   = AddrModeFlat;
  Stk.AddrPC.Offset    = pCtx->Eip;
  Stk.AddrPC.Mode      = AddrModeFlat;    
#endif

  // run the loop. Just that.
  for(nFrames = 0; StackWalk(IMAGE_FILE_MACHINE_I386,hProcess,
		        NULL,&Stk,&Ctx,
   				(PREAD_PROCESS_MEMORY_ROUTINE)ReadProcessMemory,
				(PFUNCTION_TABLE_ACCESS_ROUTINE)SymFunctionTableAccess,
				SymGetModuleBase,NULL) && (nFrames < MaxFrames);nFrames++)
  {
	 PIMAGEHLP_SYMBOL pSymbol;

    dwInstance = SymGetModuleBase(hProcess,Stk.AddrPC.Offset);    	
	// invalid address - no module covers it.
    if (!dwInstance)
	  continue;

    // get name of dll. No Module - no valid frame.
    if (!GetModuleFileNameA((HINSTANCE)dwInstance,Path,sizeof(Path)))
	  continue;	  

	// get the symbol name
    pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
    pSymbol->MaxNameLength = sizeof(symbolBuffer)-sizeof(PIMAGEHLP_SYMBOL);

    if (SymGetSymFromAddr(GetCurrentProcess(), 
					      Stk.AddrPC.Offset,
					     &symDisplacement,
					      pSymbol))        
	  Printf(pMessage,"Function: %s\n",pSymbol->Name+1);
	else 
	  Printf(pMessage,"Function: <NOSYMBOL>\n");

	// show the header line
	Printf(pMessage,"File: %s\nRVA: %8.8X HINSTANCE: %8.8X BP: %8.8X\n",
           Path,Stk.AddrPC.Offset - dwInstance,
		   dwInstance,Stk.AddrFrame.Offset);
	MemoryDump((void*)Stk.AddrPC.Offset, pMessage, 32);

	nFrames ++;
	Printf(pMessage,"\n");
  }
  SymCleanup(GetCurrentProcess());
#endif
}
#endif //!NCS_MINDEP_BUILD

DWORD NCSDbgGetExceptionInfoMsg(EXCEPTION_POINTERS *pExceptionPtr, char *pMessage)
{
#if !defined(_WIN32_WCE)&&!defined(NCS_MINDEP_BUILD)
	char msg[1024];
	void *pIP = pExceptionPtr->ExceptionRecord->ExceptionAddress;

	sprintf(pMessage, "\nAn error has occured in this application.  If you receive this message,\n"
					  "please report it at \"http://www.ermapper.com/support/supportform/\" to assist\n"
					  "with fixing this error.\n\n");
	
	switch(pExceptionPtr->ExceptionRecord->ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:
				sprintf(msg, "An Access Violation Exception occurred at code address 0x%I64x attempting to\n"
							 "%s memory address 0x%I64x.\n\n",
						(UINT64)pIP, 
						(pExceptionPtr->ExceptionRecord->ExceptionInformation[0] == 0) ? "READ from" : "WRITE to",
						(UINT64)pExceptionPtr->ExceptionRecord->ExceptionInformation[1]);
			break;
		default:
				sprintf(msg, "An Exception (number 0x%lx) occurred at address 0x%I64x\n\n", 
							pExceptionPtr->ExceptionRecord->ExceptionCode, (UINT64)pIP);
			break;
	}
	strcat(pMessage, msg);
#ifdef _M_AMD64
	MemoryDump((void*)pExceptionPtr->ContextRecord->Rip, pMessage, 32);
#else
	MemoryDump((void*)pExceptionPtr->ContextRecord->Eip, pMessage, 32);
#endif
	strcat(pMessage, "\n");
	StackTracer(pExceptionPtr->ContextRecord, pMessage, 8);
#endif

	return(EXCEPTION_EXECUTE_HANDLER);
}
