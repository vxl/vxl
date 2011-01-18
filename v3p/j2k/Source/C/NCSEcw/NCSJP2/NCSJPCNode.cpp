/********************************************************
** Copyright 2003 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCNode.cpp $
** CREATED:  14/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCNode class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCNode.h"
#include "NCSMisc.h"
#include "NCSObject.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 


//INT32 CNCSJPCNode::CNCSJPCNodeTracker::sm_nNodesTotal = 0;	
INT64 CNCSJPCNode::CNCSJPCNodeTracker::sm_nMemTotal = 0;	
//std::vector<CNCSJPCNode::CNCSJPCNodeTracker*> *CNCSJPCNode::CNCSJPCNodeTracker::sm_pTrackers = NULL;

CNCSJPCNode::CNCSJPCNodeTracker::CNCSJPCNodeTracker(char *pClassName, size_t nNodeSize)
{
//	m_nNodesTotal = 0;
	m_nMemTotal = 0;
	strcpy(m_szNodeName, pClassName);
	m_nNodeSize = (INT32)nNodeSize;
/*	if(sm_pTrackers == NULL) {
		sm_pTrackers = new std::vector<CNCSJPCNode::CNCSJPCNodeTracker*>;
	}
	sm_pTrackers->push_back(this);*/
}

CNCSJPCNode::CNCSJPCNodeTracker::~CNCSJPCNodeTracker()
{
/*	if(sm_pTrackers) {
		std::vector<CNCSJPCNodeTracker*>::iterator pCur = sm_pTrackers->begin();
		std::vector<CNCSJPCNodeTracker*>::iterator pEnd = sm_pTrackers->end();

		while(pCur != pEnd) {
			if((*pCur) == this) {
				sm_pTrackers->erase(pCur);
				break;
			}
			pCur++;
		}
		if(sm_pTrackers->size() == 0) {
			delete sm_pTrackers;
			sm_pTrackers = NULL;
		}
	}*/
}

void CNCSJPCNode::CNCSJPCNodeTracker::Add() { 
	AddMem(m_nNodeSize);
//	m_nNodesTotal++;
//	sm_nNodesTotal++;
}

void CNCSJPCNode::CNCSJPCNodeTracker::Remove() {
	RemoveMem(m_nNodeSize);
//	m_nNodesTotal--;
//	sm_nNodesTotal--;
}

void CNCSJPCNode::CNCSJPCNodeTracker::AddMem(UINT64 nSize) { 
	m_nMemTotal += nSize; 
	sm_nMemTotal += nSize; 
}

void CNCSJPCNode::CNCSJPCNodeTracker::RemoveMem(UINT64 nSize) {
	m_nMemTotal -= nSize; 
	sm_nMemTotal -= nSize; 
}

// Constructor
CNCSJPCNode::CNCSJPCNode()
{
	m_pContext = NULL;
}

// Destructor
CNCSJPCNode::~CNCSJPCNode()
{
	if(m_pContext) {
		m_pContext->clear();
		delete m_pContext;
	}
}

// Link a node as the unput to this node.
bool CNCSJPCNode::Link(ContextID nCtx,
					   UINT16 nInputs,
					   CNCSJPCNode *pInput0, 
					   ...)
{
	va_list va;
	va_start(va, pInput0);
	Context *pCtx = GetContext(nCtx);
	
	if(pCtx->NrInputs() < 1) {
		pCtx->AddInput(pInput0);
	} else {
		pCtx->SetInput(0, pInput0);
	}

	for(UINT32 i = 1; i < nInputs; i++) {
		if(pCtx->NrInputs() < i + 1) {
			pCtx->AddInput(va_arg(va, class CNCSJPCNode *));
		} else {
			pCtx->SetInput(i, va_arg(va, class CNCSJPCNode *));
		}
	}
	va_end(va);
	return(true);
}

bool CNCSJPCNode::UnLink(ContextID nCtx, UINT16 nInputs)
{
	Context *pCtx = GetContext(nCtx);
	bool bRet = true;
	if(pCtx) {
		for(UINT32 i = 0; i < NCSMax(nInputs, pCtx->NrInputs()); i++) {
			if(pCtx->GetInput(i)) {
				bRet &= pCtx->GetInput(i)->UnLink(nCtx);
			}
		}
		(*m_pContext).erase(nCtx); 
	}
	return(bRet);
}

// Read a line from the input node.
bool CNCSJPCNode::ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent)
{
	Context *pCtx = GetContext(nCtx);
	bool bRet = false;

	if(pCtx->NrInputs() > 0) {
		bRet = pCtx->GetInput(0)->ReadLine(nCtx, pDst, iComponent);

		if(bRet == false) {
			*(CNCSError*)this = (CNCSError)*pCtx->GetInput(0);
		}
	}
	return(bRet);
}

bool CNCSJPCNode::ReadInputLines(ContextID nCtx,
								 UINT32 nInputs,
								 ...)
								 
{
	bool bRet = false;
	Context *pCtx = GetContext(nCtx);
	if(pCtx) {
		va_list va;
		va_start(va, nInputs);
		for(UINT32 i = 0; i < nInputs; i++) {
			UINT32 nInput = va_arg(va, UINT32);
			CNCSJPCBuffer *pBuf = va_arg(va, CNCSJPCBuffer*);
			bRet = pCtx->GetInput(nInput)->ReadLine(nCtx, pBuf, (UINT16)nInput);
			if(!bRet) {
				*(CNCSError*)this = *pCtx->GetInput(nInput);
			}
		}
		va_end(va);
	}
	return(bRet);
}

CNCSError CNCSJPCNode::GetError(ContextID nCtx)
{
	Context *pCtx = GetContext(nCtx);
	if(pCtx && pCtx->NrInputs() > 0) {
		return(*(CNCSError*)pCtx->GetInput(0));
	} 
	return(NCS_INVALID_PARAMETER);
}


CNCSJPCNode::Context *CNCSJPCNode::GetContext(ContextID nCtx, bool bAutoConstruct)
{
	Context *pCtx = NULL;
	if(m_pContext) {
		pCtx = (*m_pContext)[nCtx].get();
	}
	if(!pCtx && bAutoConstruct) {
		pCtx = new Context;
		SetContext(nCtx, pCtx);
	}
	return(pCtx);
}

void CNCSJPCNode::SetContext(ContextID nCtx, Context *pCtx)
{ 
	if(!m_pContext) {
		m_pContext = new std::map<ContextID, ContextAutoPtr>;
	}
	ContextAutoPtr P(pCtx);  
	(*m_pContext)[nCtx] = P; 
};

CNCSJPCNode::Context::Context() 
{
}

CNCSJPCNode::Context::~Context()
{
}

UINT32 CNCSJPCNode::Context::NrInputs()
{ 
	return((UINT32)m_Inputs.size()); 
}

CNCSJPCNode *CNCSJPCNode::Context::GetInput(UINT32 n)
{ 
	return((n < NrInputs()) ? m_Inputs[n].m_pNode : NULL); 
}

void CNCSJPCNode::Context::SetInput(UINT32 n, CNCSJPCNode *pNode)
{ 
	m_Inputs[n].m_pNode = pNode; 
}

void CNCSJPCNode::Context::SetInputBuffers(UINT32 nInput, UINT8 nBuffers)
{ 
	m_Inputs[nInput].m_Buffers.SetBuffers(nBuffers);
}

CNCSJPCBuffer *CNCSJPCNode::Context::GetInputBuffer(UINT32 nInput, UINT8 nBuffer)
{ 
	return(m_Inputs[nInput].m_Buffers.GetBuffer(nBuffer));
}

CNCSJPCBuffer *CNCSJPCNode::Context::FindInputBuffer(UINT32 nInput, UINT32 &nBuffer, CNCSJPCRect &r, bool &bFound, CNCSJPCBuffer::Type eType)
{
	return(m_Inputs[nInput].m_Buffers.FindBuffer(nBuffer, r, bFound, eType));
}

void CNCSJPCNode::Context::AddInput(CNCSJPCNode *pNode)
{ 
	CNCSJPCNodeInput Input;
	Input.m_pNode = pNode;
	m_Inputs.push_back(Input); 
}

CNCSJPCNode::Context CNCSJPCNode::Context::operator=(const Context &src)
{ 
	m_Inputs = src.m_Inputs; 
	return(*this); 
}

CNCSJPCNode::Context::CNCSJPCNodeInput::~CNCSJPCNodeInput()
{
}

