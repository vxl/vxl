/////////////////////////////////////////////////////////////////////////////
#ifndef __COMISSUE_ERROR_H__
#define __COMISSUE_ERROR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Definition of standard function that is used by smart 
// pointers to report about error HRESULT code. This function
// throws exception using RaiseException function. This 
// exception could be catched using 
// __except (CATCH_COM_ERROR)
// code. If ST_IGNORE_COM_EXCEPTIONS is defined this function
// does nothing (no exception is thrown).
extern void __stdcall _com_issue_error(HRESULT hResult);

// Definition of standard function that is used by smart 
// pointers to report about error HRESULT code. This function
// throws exception using RaiseException function. This 
// exception could be catched using 
// __except (CATCH_COM_ERROR)
// code. If ST_IGNORE_COM_EXCEPTIONS is defined this function
// does nothing (no exception is thrown).
extern void __stdcall _com_issue_errorex(HRESULT hResult, IUnknown* pUnknown, REFIID refid);

// Checks whether the exception corresponds to COM error or
// not and fill lastComError variable with information about
// COM error. This function should be called from __except.
// Returns true is dwCode equals 
extern int ComErrorFilterFunction(DWORD dwCode, LPEXCEPTION_POINTERS info);

// This macro represents right calling of ComErrorFilterFunction
// function from __except. Just write
// __except (CATCH_COM_ERROR)
// to catch COM error exceptions.
#define CATCH_COM_ERROR ComErrorFilterFunction(GetExceptionCode(), GetExceptionInformation())


#endif // __COMISSUE_ERROR_H__
