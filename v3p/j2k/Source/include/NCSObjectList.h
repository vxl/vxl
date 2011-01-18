/*******************************************************
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
** FILE:   	CNCSObjectList.cpp
** CREATED:	Wed Oct 18 2000
** AUTHOR: 	Jeff Pudwell
** PURPOSE:	Class to store lists of Objects.
**		
** EDITS:
*******************************************************/


#ifndef NCSOBJECTLIST_H_
#define NCSOBJECTLIST_H_


#include "NCSTypes.h"
#include "NCSDefs.h"
#include "NCSMalloc.h"
#include "NCSUtil.h"

#ifdef MACINTOSH
#define NCS_OBJECT_BAD_NODE		(INT32)-1
#define NCS_OBJECT_FIRST_NODE	 (INT32)0
#else
#define NCS_OBJECT_BAD_NODE		-1
#define NCS_OBJECT_FIRST_NODE	 0
#endif

struct NCSObjectListNode
{
	INT32 index;
	char *pStringID;
	void *pObject;
	NCSObjectListNode *next;
	NCSObjectListNode *prev;
};


class NCS_EXPORT CNCSObjectList
{
	public:
		CNCSObjectList();		
		virtual ~CNCSObjectList();

	protected:
		INT32 Add(char *pStringID, void *pObject);
		void * Remove(char *pString);
		void * Remove(INT32 nIndex);
		void * Remove(void *pObj);
		void RemoveAllNodes();
		BOOLEAN Move(INT32 nSrcIndex, INT32 nDestIndex);
		BOOLEAN isEmpty(); 

		void * Get(INT32 nIndex);
		void * Get(char *pStringID);
		INT32 GetNumberNodes();

		INT32  GetIndex(char *pStringID);
		char * GetStringID(INT32 nIndex);
		
		void * GetFirstNode();
		void * GetNextNode();
		
		void * RemoveNode(NCSObjectListNode *pNode);
		void DeleteNode(NCSObjectListNode *pNode);
		NCSObjectListNode * FindNode(INT32 nIndex);
		NCSObjectListNode * FindNode(char *pStringID);

	private:
		NCSObjectListNode *m_pList;
		NCSObjectListNode *m_pCurrNode;
		NCSObjectListNode *m_pLastNode;
		INT32 m_nNodes;

		void AdjustNodeIndexes();
};


#endif	//NCSOBJECTLIST_H_


