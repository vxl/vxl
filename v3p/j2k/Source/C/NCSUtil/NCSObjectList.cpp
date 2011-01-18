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


#include "NCSUtil.h"
#include "NCSObjectList.h"

CNCSObjectList::CNCSObjectList()
{
	m_pList = NULL;
	m_pCurrNode = NULL;
	m_pLastNode = NULL;
	m_nNodes = NCS_OBJECT_BAD_NODE;
}

CNCSObjectList::~CNCSObjectList()
{
	//Class that derives must manually free the object within each node in the list.
	//the destructor will clean up the rest of the list, including freeing the
	//strings within the nodes and freeing the nodes.
	RemoveAllNodes();
}

void CNCSObjectList::DeleteNode(NCSObjectListNode *pNode)
{
	if (pNode)
	{
		if (pNode->pStringID)
			NCSFree(pNode->pStringID);

		delete pNode;
	}
}

INT32 CNCSObjectList::Add(char *pStringID, void *pObject)
{
	INT32 nIndex=NCS_OBJECT_BAD_NODE;

	if (!pStringID || !pObject)
		return nIndex;

	if (m_pList == NULL)
	{
		m_pList = new NCSObjectListNode;
		if (m_pList)	//first object to be added
		{
			m_nNodes++;
			nIndex = m_pList->index = m_nNodes;
			m_pList->pObject = pObject;
			m_pList->pStringID = NCSStrDup(pStringID);
			m_pList->next = NULL;
			m_pList->prev = NULL;

			m_pLastNode = m_pList;
			m_pCurrNode = m_pList;
		}
	}
	else
	{
		NCSObjectListNode *pNode;
		pNode = new NCSObjectListNode;
		if (pNode)
		{
			m_nNodes++;
			nIndex = pNode->index = (m_pLastNode->index+1);
			pNode->pObject = pObject;
			pNode->pStringID = NCSStrDup(pStringID);
			pNode->next = NULL;
			pNode->prev = m_pLastNode;

			//Attach to the end of the list
			m_pLastNode->next = pNode;
			m_pLastNode = pNode;
		}
	}

	return nIndex;
}

void * CNCSObjectList::Remove(char *pString)
{
	NCSObjectListNode *pNode = FindNode(pString);
	void *pObject = NULL;

	if (pNode)
	{
		pObject = RemoveNode(pNode);
		AdjustNodeIndexes();
	}

	return pObject;
}

void * CNCSObjectList::Remove(INT32 nIndex)
{
	NCSObjectListNode *pNode = FindNode(nIndex);
	void *pObject = NULL;

	if (pNode)
	{
		pObject = RemoveNode(pNode);
		AdjustNodeIndexes();
	}

	return pObject;
}

void * CNCSObjectList::Remove(void *pObj)
{
	NCSObjectListNode *pNode = m_pList;
	BOOLEAN found = FALSE;
	void *pObject = NULL;

	//find the node then delete it
	while (pObj && pNode && !found)
	{
		if (pNode->pObject == pObj)
			found = TRUE;
		else
			pNode = pNode->next;
	}

	if (pNode)
	{
		pObject = RemoveNode(pNode);
		AdjustNodeIndexes();
	}

	return pObject;
}

void CNCSObjectList::RemoveAllNodes()
{
	NCSObjectListNode *pNode = m_pList;
	NCSObjectListNode *pNext = NULL;

	while (pNode)
	{
		pNext = pNode->next;
		DeleteNode(pNode);
		pNode = pNext;
	}

	m_pList = m_pCurrNode = m_pLastNode = NULL;
	m_nNodes = NCS_OBJECT_BAD_NODE;
}

void * CNCSObjectList::RemoveNode(NCSObjectListNode *pNode)
{
	NCSObjectListNode *pPrev, *pNext;
	void *pObject = NULL;

	pPrev = pNode->prev;
	pNext = pNode->next;

	if (pPrev && pNext)			//node between two nodes in the list
	{
		pPrev->next = pNext;
		pNext->prev = pPrev;
	}
	else if (!pPrev && pNext)	//node at head of the list, and > 1 nodes in list
	{
		pNext->prev = NULL;
		m_pList		= pNext;
		m_pCurrNode = pNext;
	}
	else if (!pPrev && !pNext)	//only node in the list
	{
		m_pList = NULL;
		m_pCurrNode = NULL;
		m_pLastNode = NULL;
	}
	else						//node is a the tail of the list
	{
		pPrev->next = NULL;
		m_pCurrNode = pPrev;
		m_pLastNode = pPrev;
	}

	//Destroy the node
	pObject = pNode->pObject;
	DeleteNode(pNode);
	pNode = NULL;
	m_nNodes--;

	if (m_pList == NULL)
		m_nNodes = NCS_OBJECT_BAD_NODE;

	return pObject;
}

BOOLEAN CNCSObjectList::Move(INT32 nSrcIndex, INT32 nDestIndex)
{
	BOOLEAN bFound = FALSE;

	if (nDestIndex < NCS_OBJECT_FIRST_NODE || m_pList == NULL)
		return FALSE;

	NCSObjectListNode *pDelNode = FindNode(nSrcIndex);
	if (pDelNode == NULL)
		return FALSE;

	//first find the position in the list (nDestIndex) where the
	//node should be placed and save a pointer to this position.
	NCSObjectListNode *pNode = m_pList;
	NCSObjectListNode *pInsert = pNode;

	while (pNode && !bFound)
	{
		//pInsert = pNode;	//insert before this node
		if ((nDestIndex == pNode->index)/* || (nDestIndex < pNode->index)*/)
			bFound = TRUE;
		else
			pNode = pNode->next;

		pInsert = pNode;
	}

	//insert the node at the appropriate position in the list
	NCSObjectListNode *pNewNode = new NCSObjectListNode;
	if (pNewNode)
	{
		m_nNodes++;
		pNewNode->index = m_nNodes;
		pNewNode->pStringID = NCSStrDup(pDelNode->pStringID);
		pNewNode->pObject = pDelNode->pObject;
		pNewNode->prev = pNewNode->next = NULL;
		
		if (pInsert == NULL)				//has to be the last node in list
		{
			pNewNode->next = NULL;
			pNewNode->prev = m_pLastNode;
			m_pLastNode->next = pNewNode;
			m_pLastNode = pNewNode;
		}
		else if (pInsert->prev == NULL)		//inserting before the first node
		{
			m_pList = pNewNode;
			pInsert->prev = pNewNode;
			pNewNode->prev = NULL;
			pNewNode->next = pInsert;
		}
		else if (nSrcIndex < nDestIndex)	//inserting between two nodes, after node
		{
			if (pInsert->next == NULL)
			{
				m_pLastNode = pNewNode;
				pNewNode->next = NULL;
				pNewNode->prev = pInsert;
				pInsert->next = pNewNode;
			}
			else
			{
				pNewNode->next = pInsert->next;
				pNewNode->prev = pInsert;
				pInsert->next->prev = pNewNode;
				pInsert->next = pNewNode;
			}
		}
		else								//insert before the node
		{
			pInsert->prev->next = pNewNode;
			pNewNode->prev = pInsert->prev;
			pInsert->prev = pNewNode;
			pNewNode->next = pInsert;
		}
	}
	else
		return FALSE;

	//delete the node corresponding to the source index
	RemoveNode(pDelNode);
	AdjustNodeIndexes();

	return TRUE;
}

//adjusts the node indexes so they are consecutive and sequential.
//should be called after a node is deleted from the list
void CNCSObjectList::AdjustNodeIndexes()
{
	NCSObjectListNode *pNode = m_pList;

	m_nNodes = NCS_OBJECT_BAD_NODE;

	while (pNode)
	{
		m_nNodes++;
		pNode->index = m_nNodes;
		pNode = pNode->next;
	}
}

BOOLEAN CNCSObjectList::isEmpty()
{
	if (m_nNodes == NCS_OBJECT_BAD_NODE) //or (m_pList == NULL)
		return TRUE;
	else 
		return FALSE;
}

void * CNCSObjectList::Get(INT32 nIndex)
{
	NCSObjectListNode *pObject = FindNode(nIndex);
	if (pObject)
		return pObject->pObject;
	else
		return NULL;
}

void * CNCSObjectList::Get(char *pStringID)
{
	NCSObjectListNode *pObject = FindNode(pStringID);
	if (pObject)
		return pObject->pObject;
	else
		return NULL;
}

INT32 CNCSObjectList::GetIndex(char *pStringID)
{
	NCSObjectListNode *pObject = FindNode(pStringID);
	if (pObject)
		return pObject->index;
	else
		return NCS_OBJECT_BAD_NODE;
}

char * CNCSObjectList::GetStringID(INT32 nIndex)
{
	NCSObjectListNode *pObject = FindNode(nIndex);
	if (pObject)
		return pObject->pStringID;
	else
		return NULL;
}

INT32 CNCSObjectList::GetNumberNodes()
{
	INT32 nNodes = 0;

	NCSObjectListNode *pStart = m_pList;
	while (pStart)
	{
		pStart = pStart->next;
		nNodes++;
	}

	return nNodes;
}

NCSObjectListNode * CNCSObjectList::FindNode(INT32 nIndex)
{
	NCSObjectListNode *pNode = m_pList;
	BOOLEAN found = FALSE;

	while (pNode && !found)
	{
		if (pNode->index == nIndex)
			found = TRUE;
		else
			pNode = pNode->next;
	}

	return pNode;
}

NCSObjectListNode * CNCSObjectList::FindNode(char *pStringID)
{
	NCSObjectListNode *pNode = m_pList;
	BOOLEAN found = FALSE;

	if (!pStringID)
		return NULL;

	while (pNode && !found)
	{
		if (stricmp(pNode->pStringID, pStringID) == 0)
			found = TRUE;
		else
			pNode = pNode->next;
	}

	return pNode;
}

void * CNCSObjectList::GetFirstNode()
{
	m_pCurrNode = m_pList;
	if (m_pCurrNode)
		return m_pCurrNode->pObject;

	return NULL;
}

void * CNCSObjectList::GetNextNode()
{
	if (m_pCurrNode->next)
	{
		m_pCurrNode = m_pCurrNode->next;
		if (m_pCurrNode)
			return m_pCurrNode->pObject;
	}

	return NULL;
}
