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
** FILE:   	NCSUtil\CNCSMetabaseEdit.h
** CREATED:	10Apr00
** AUTHOR: 	Nicholas Yue
** PURPOSE:	Metabase manipulation routines
** EDITS:
** [01] 11Jul00  ny	Added boolean field to indicate if CoInitialize()
**					has been called
(( [02] 14Jul01 sjc Added static mutex
 *******************************************************/
#ifndef CNCSMETABASEEDIT_H
#define CNCSMETABASEEDIT_H

#include <windows.h>
#include <iadmw.h>
#include "NCSDefs.h"
#include "NCSMutex.h"

class NCS_EXPORT CNCSMetabaseEdit {
public :
  CNCSMetabaseEdit();
  ~CNCSMetabaseEdit();
  HRESULT open(char *pathName,
	       DWORD permission,
	       DWORD timeout = 5000);
  HRESULT close();
  HRESULT save();
  HRESULT get(char *dataName,
	      METADATA_RECORD *MDRecord,
	      DWORD *MDRecordLen);
  HRESULT set(char *dataName,
	      METADATA_RECORD *MDRecord);
  HRESULT add(char *keyName);
  HRESULT Delete(char *keyName);

protected :
private :
  char _szPath[1024];
  char _szData[256];
  IMSAdminBase *pIABase;
  METADATA_HANDLE metaHandle;
  bool _isOpen;
  bool _isInit;
  static CNCSMutex sm_Mutex;	//[02]
};
#endif
