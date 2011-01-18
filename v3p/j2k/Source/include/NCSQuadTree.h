/********************************************************
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
** FILE:    NCSQTree.h
** CREATED: Thu Feb 25 09:19:00 WST 1999
** AUTHOR:  Mark Sheridan
** PURPOSE: Generic Quad tree class for NCS. User should
**			inherit directly from this class and supply
**			the required virtual functions
** EDITS:
 *******************************************************/

#ifndef NCSQUADTREE_H
#define NCSQUADTREE_H

#include "NCSTypes.h"
#include "NCSErrors.h"
#include "NCSDefs.h"
#include "NCSMalloc.h"
#include "NCSMutex.h"
#include <math.h>
#if (defined(WIN32) && !defined(_WIN32_WCE))
#include <crtdbg.h>
#endif

#define NCSQTreeGetNodeQuad1(pNode) (pNode->nw)
#define NCSQTreeGetNodeQuad2(pNode) (pNode->ne)
#define NCSQTreeGetNodeQuad3(pNode) (pNode->se)
#define NCSQTreeGetNodeQuad4(pNode) (pNode->sw)
#define NCSQTreeGetNodeData(pNode)  (pNode->pData)
#define NCSQTreeSetNodeData(pNode, pData)  \
	 pNode->pData = (void *)pData

typedef struct NCSQTreeNode {
	void   *pData;				// Pointer to generic data pointer
	UINT32 nID;					// Unique ID of this node
	IEEE8  dWorldX;				// World X coordinate of node
	IEEE8  dWorldY;				// World Y coordinate of node
	struct NCSQTreeNode *nw;	// Pointer to Node in North West
	struct NCSQTreeNode *ne;	// Pointer to Node in North East
	struct NCSQTreeNode *sw;	// Pointer to Node in South West
	struct NCSQTreeNode *se;	// Pointer to Node in South East
} NCSQTreeNode;

typedef enum NCSQTreeAlg{		// The algorithm used to construct the Quad Tree
	NCSQTALG_PRKD,				// PRKD tree algorithm - NOT YET IMPLEMENTED
	NCSQTALG_PR,				// PR tree algorithm
	NCSQTALG_POINT,				// Simple point algorithm
	NCSQTALG_KD,				// KD tree algorithm - NOT YET IMPLEMENTED
	NCSQTALG_MX,				// MX tree algorithm - NOT YET IMPLEMENTED
	NCSQTALG_PMR,				// PMR tree algorithm - NOT YET IMPLEMENTED
	NCSQTALG_SIMPLE_EDGES,		// Edge algorithm - NOT YET IMPLEMENTED
	NCSQTALG_SIMPLE_GRID		// Grid algorithm - NOT YET IMPLEMENTED
} NCSQTreeAlg;

typedef NCSError (*NCSQTreeNodeFunc)(NCSQTreeNode *pNode, void *pInfo);

class __declspec( dllexport ) CNCSQTree  
{

public:
	// Initialisation
	CNCSQTree();
	virtual ~CNCSQTree();
	NCSError InitExtents(NCSCoordSys CSysType, IEEE8 tlx, IEEE8 tly, IEEE8 brx, IEEE8 bry );

	// Node functions
	NCSError	Print( void );
	NCSError	Draw(void *pDrawInfo);
	void		*GetNodeData( NCSQTreeNode *pNode);
	void		SetNodeData (NCSQTreeNode *pNode, void *pData);
	NCSQTreeNode *FindNode( IEEE8 dWorldX, IEEE8 dWorldY, IEEE8 dRadiusX, IEEE8 dRadiusY );

	// Virtual functions
	virtual	NCSError DrawNode(void *pDrawInfo) = 0;
	virtual NCSError PrintNode(NCSQTreeNode *pNode) = 0;
	virtual void FreeNodeData( void *pData );

	// Node creation-free
	NCSQTreeNode * FreeNode(NCSQTreeNode *pNode);
	NCSQTreeNode * AddNode( void *pNodeData , IEEE8 dWorldX, IEEE8 dWorldY );	

	// Algorithm functions
	NCSError SetAlgorithm(NCSQTreeAlg nAlg);
	NCSQTreeAlg GetAlgorithm();

	// Tree traversal - use custom traverse function "pFunction".
	NCSError Traverse(NCSQTreeNodeFunc pFunction, void *pData);

	// Miscellaneous routines
	NCSError Balance(void);

private:
	NCSQTreeNode * DrawInternal( void *pDrawInfo, NCSQTreeNode *pRoot);
	NCSQTreeNode * FindNodeInternal (NCSQTreeNode *pRoot, IEEE8 latitude, IEEE8 longitude);
	NCSQTreeNode * AddNodeInternalPR( NCSQTreeNode *pRootNode, void *pData, IEEE8 latitude, IEEE8 longitude, IEEE8 tlx, IEEE8 tly, IEEE8 brx, IEEE8 bry);
	NCSQTreeNode * AddNodeInternalPOINT( NCSQTreeNode *pRootNode, void *pData, IEEE8 latitude, IEEE8 longitude);
	NCSQTreeNode * PrintInternal(NCSQTreeNode *pRoot);
	NCSQTreeNode * TraverseNode(NCSQTreeNode *pNode, NCSQTreeNodeFunc pFunction, void *pData);
	NCSQTreeNode * CreateNode(IEEE8 dWorldX, IEEE8 dWorldY, void *pData);

	static NCSError NCSQTreeCalcExtentsFn(NCSQTreeNode *pNode, void *pInfo);
	static NCSError NCSQTreeCopyFn(NCSQTreeNode *pNode, void *pInfo);
	
	IEEE8	m_WorldTLX;
	IEEE8	m_WorldTLY;
	IEEE8	m_WorldBRX;
	IEEE8	m_WorldBRY;
	INT32	m_NrNodes;
	UINT16	m_CurrDrawLevel;
	IEEE8	m_RadiusDistanceX;
	IEEE8	m_RadiusDistanceY;

	NCSQTreeAlg		m_nAlgorithm;
	NCSQTreeNode	* m_pRootNode;
	NCSCoordSys		m_CoordSys;
	NCSQTreeNode	*m_pFindNodeResult;
	NCSMutex		m_TraverseMutex;
};

#endif
