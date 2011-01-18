// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#if !defined(AFX_STDAFX_H__1BA90D28_DF6F_11D3_A6C3_005004055C6C__INCLUDED_)
#define AFX_STDAFX_H__1BA90D28_DF6F_11D3_A6C3_005004055C6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef STRICT
#define STRICT
#endif //STRICT

#if _WIN32_WCE == 201
#error ATL is not supported for Palm-Size PC
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#define _ATL_APARTMENT_THREADED
#if defined(_WIN32_WCE)
#undef _WIN32_WINNT
#endif

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1BA90D28_DF6F_11D3_A6C3_005004055C6C__INCLUDED)
