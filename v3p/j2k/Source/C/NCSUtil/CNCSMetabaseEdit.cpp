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
**
** FILE:   	NCSUtil\CNCSMetabaseEdit.cpp
** CREATED:	10Apr00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Metabase manipulation routines
** EDITS:
** [01] 11Jul00  ny	Redesign class to do CoInitialize()
**					in constructor and CoUninitialize()
**					in destructor
** [02] 13Jun01 sjc Fixed interface leaks
** [03] 14Jul01 sjc Added static lock - can overload MetaBase under NT4
**					if too many threads access it at once.
 *******************************************************/

#include <windows.h>
#include <atlbase.h>
#include <initguid.h>
#include <stdio.h>
#include <string.h>
#include <objbase.h>
#include "CNCSMetabaseEdit.h"

CNCSMutex CNCSMetabaseEdit::sm_Mutex;	//[03]

#ifdef NOTDEF

static void printError(char *msg,
		       DWORD errorCode)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	// Display the string.
	printf("%s error is %s",msg,(LPCTSTR)lpMsgBuf);
	// Free the buffer.
	LocalFree( lpMsgBuf );
}

#endif

CNCSMetabaseEdit::CNCSMetabaseEdit()
{
	_szPath[0]      = '\0';
	_szData[0]      = '\0';
	_isOpen         = false;
	pIABase		  = NULL;
	CoInitialize(NULL);
	_isInit         = true;
}

CNCSMetabaseEdit::~CNCSMetabaseEdit()
{
	close();	//[02]

	if (_isInit)
		CoUninitialize();
}

HRESULT CNCSMetabaseEdit::save()
{
	if (_isOpen)
		return (pIABase->SaveData());
	return ERROR_SUCCESS;
}

HRESULT CNCSMetabaseEdit::close()
{
	HRESULT hr = ERROR_SUCCESS;
	if (_isOpen) {
		if(pIABase) {
			hr = pIABase->CloseKey(metaHandle);
			metaHandle = 0;
			pIABase->Release();//[02]
			pIABase = NULL;
		}
		sm_Mutex.UnLock();	//[03]
		_isOpen = false;
	}
	return(hr);
}

HRESULT CNCSMetabaseEdit::open(char *pathName,
			       DWORD permission,
			       DWORD timeout)
{
	HRESULT hRes;

	close();//[02]

	sm_Mutex.Lock();			//[03]

	strcpy(_szPath, pathName);
	hRes = CoCreateInstance(CLSID_MSAdminBase,
							NULL,
							CLSCTX_ALL,
							IID_IMSAdminBase,
							(void**)&(pIABase));
  

	if (SUCCEEDED(hRes)) {
		USES_CONVERSION;
		hRes = pIABase->OpenKey(METADATA_MASTER_ROOT_HANDLE,
								A2W(pathName),
								permission,
								timeout,
								&metaHandle);
		if (SUCCEEDED(hRes)) {
			_isOpen         = true;
		} else {
			pIABase->Release();//[02]
			pIABase = NULL;//[02]
		}
	}
	if(!_isOpen) {			//[03]
		sm_Mutex.UnLock();	//[03]
	}						//[03]

	return hRes;
}


HRESULT CNCSMetabaseEdit::get(char *dataName,
							  METADATA_RECORD *MDRecord,
							  DWORD *MDRecordLen)
{
	if (_isOpen) {
		strcpy(_szData, dataName);
		USES_CONVERSION;
		HRESULT hr;
		//NTE_BAD_KEYSET == NT4 CryptoApi issues, just keep retrying until it works
		while(NTE_BAD_KEYSET == (hr = pIABase->GetData(metaHandle,
													   A2W(dataName),
													   MDRecord,
													   MDRecordLen))) {
			Sleep(10);	
		}
		return(hr);
	}
	return ERROR_SUCCESS;
}

HRESULT CNCSMetabaseEdit::set(char *dataName,
							  METADATA_RECORD *MDRecord)
{
	if (_isOpen) {
		strcpy(_szData, dataName);
		USES_CONVERSION;
		return (pIABase->SetData(metaHandle,
								 A2W(dataName),
								 MDRecord));
	}
	return ERROR_SUCCESS;
}

HRESULT CNCSMetabaseEdit::add(char *keyName)
{
	if (_isOpen) {
		//strcpy(_szData, dataName);
		USES_CONVERSION;
		return (pIABase->AddKey(metaHandle,
								A2W(keyName)));
	}
	return ERROR_SUCCESS;
}

HRESULT CNCSMetabaseEdit::Delete(char *keyName)
{
	if (_isOpen) {
		USES_CONVERSION;
		return (pIABase->DeleteKey(metaHandle,
								   A2W(keyName)));
	}
	return ERROR_SUCCESS;
}
