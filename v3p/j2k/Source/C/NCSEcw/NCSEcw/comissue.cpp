#include "stdafx.h"
#include "comissue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define EXCEPTION_COMERROR	0x8000
//////////////////////////////////////////////////////////////////////
// COM error handlers
//////////////////////////////////////////////////////////////////////

void __stdcall _com_issue_error(HRESULT hResult)
{
#ifndef ST_IGNORE_COM_EXCEPTIONS
	DWORD *data = new DWORD[1];
	data[0] = hResult;
	ATLTRACE(_T("_com_issue_error(%08x)\n"), (DWORD)hResult);
	RaiseException(EXCEPTION_COMERROR, 0, 1, data);
#endif /*ST_IGNORE_COM_EXCEPTIONS*/
}

void __stdcall _com_issue_errorex(HRESULT hResult, IUnknown* pUnknown, REFIID refid)
{
#ifndef ST_IGNORE_COM_EXCEPTIONS
	DWORD *data = new DWORD[1];
	data[0] = hResult;
	ATLTRACE(_T("_com_issue_errorex(%08x)\n"), (DWORD)hResult);
	RaiseException(EXCEPTION_COMERROR, 0, 1, data);
#endif /*ST_IGNORE_COM_EXCEPTIONS*/
}

int ComErrorFilterFunction(DWORD dwCode, LPEXCEPTION_POINTERS info)
{
	HRESULT hrResult = info->ExceptionRecord->ExceptionInformation[0];
	return (dwCode==EXCEPTION_COMERROR);
}

