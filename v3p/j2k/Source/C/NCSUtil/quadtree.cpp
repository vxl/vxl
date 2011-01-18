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
** FILE:    NCSQTree.cpp
** CREATED: Thu Feb 25 09:19:00 WST 1999
** AUTHOR:  Mark Sheridan
** PURPOSE: Implementation of the NCSQTree class
** EDITS:
** [01]	 ny	31Oct00	Merge WinCE/PALM SDK changes
 *******************************************************/
#include "NCSUtil.h"
#include "NCSQuadTree.h"

#define PointInRectUTM(  X,  Y,  tlx,  tly,  brx,  bry ) \
	(( X >= tlx ) &&	( X <= brx ) && ( Y <= tly ) && ( Y >= bry ))

#define PointInRectRAW(  X,  Y,  tlx,  tly,  brx,  bry ) \
	(( X >= tlx ) &&	( X <= brx ) && ( Y >= tly ) && ( Y <= bry ))

#define PointInRectLL(  X,  Y,  tlx,  tly,  brx,  bry ) \
	(NOT YET IMPLEMENTED)

typedef struct NCSQTreeBalanceData {
	IEEE8 dWorldTLX;
	IEEE8 dWorldTLY;
	IEEE8 dWorldBRX;
	IEEE8 dWorldBRY;
	NCSCoordSys nCoordSys;
} NCSQTreeBalanceData;

typedef struct NCSQTreeCopyData {
	NCSQTreeNode    *pFromRootNode;
	NCSQTreeNode    *pToRootNode;
	class CNCSQTree *pQuadTree;
} NCSQTreeCopyData;

CNCSQTree::CNCSQTree()
{
	m_CoordSys = NCSCS_RAW;
	m_WorldTLX = 0.0;
	m_WorldTLY = 0.0;
	m_WorldBRX = 0.0;
	m_WorldBRY = 0.0;
	m_pRootNode = (NCSQTreeNode *)NULL;
	m_nAlgorithm = NCSQTALG_POINT;
	m_CurrDrawLevel = 0;
	NCSMutexInit(&m_TraverseMutex);
}

CNCSQTree::~CNCSQTree()
{
	FreeNode(m_pRootNode);
	NCSMutexFini(&m_TraverseMutex);
}

NCSQTreeNode *CNCSQTree::CreateNode(IEEE8 dWorldX, IEEE8 dWorldY, void *pData)
{
	NCSQTreeNode *pNode = (NCSQTreeNode *) NCSMalloc((unsigned)sizeof(struct NCSQTreeNode), 1);
	if (pNode) {
		NCSQTreeSetNodeData(pNode, pData);
		pNode->dWorldX = dWorldX;
		pNode->dWorldY = dWorldY;
		pNode->nw = (NCSQTreeNode *)NULL;
		pNode->ne = (NCSQTreeNode *)NULL;
		pNode->sw = (NCSQTreeNode *)NULL;
		pNode->se = (NCSQTreeNode *)NULL;
	}
	return pNode;
}

NCSQTreeNode * CNCSQTree::FreeNode( NCSQTreeNode *pNode )
{
    /*Check if it was an empty tree. If not then go to nw, then ne, then sw finaly se.*/
    if(pNode!=NULL) {
        pNode->nw = FreeNode(pNode->nw);         /* Free the nw */
        pNode->ne = FreeNode(pNode->ne);         /* Free the ne */
        pNode->sw = FreeNode(pNode->sw);         /* Free the sw */
        pNode->se = FreeNode(pNode->se);         /* Free the se */

		FreeNodeData(pNode->pData);
        NCSFree(pNode);                          /* Free this node */
    }
    else 
		return pNode;							 /* If it was empty tree go back. */

    return pNode;
}

NCSError CNCSQTree::Traverse(NCSQTreeNodeFunc pFunction, void *pData)
{
	NCSMutexBegin(&m_TraverseMutex);

	TraverseNode(m_pRootNode, pFunction, pData);

	NCSMutexEnd(&m_TraverseMutex);
	return(NCS_SUCCESS);	
}

NCSQTreeNode *CNCSQTree::TraverseNode(NCSQTreeNode *pNode, NCSQTreeNodeFunc pFunction, void *pData)
{
	if(pNode!=NULL) {
        pNode->nw = TraverseNode(pNode->nw, pFunction, pData);      /* Free the nw */
        pNode->ne = TraverseNode(pNode->ne, pFunction, pData);      /* Free the ne */
        pNode->sw = TraverseNode(pNode->sw, pFunction, pData);      /* Free the sw */
        pNode->se = TraverseNode(pNode->se, pFunction, pData);      /* Free the se */
		(pFunction)(pNode, pData);									/* Call the supplied function pointer */
    }
    return pNode;
}

void CNCSQTree::FreeNodeData( void *pData )
{
	if (pData) {
		NCSFree(pData);
	}
}

NCSQTreeNode * CNCSQTree::AddNode( void *pData, IEEE8 dWorldX, IEEE8 dWorldY )
{
	NCSQTreeNode *pNode = NULL;

	switch(m_nAlgorithm) {
	case NCSQTALG_POINT:
		pNode = AddNodeInternalPOINT( m_pRootNode, pData, dWorldX, dWorldY);
		break;
	case NCSQTALG_PR:
		pNode = AddNodeInternalPR( m_pRootNode, pData, dWorldX, dWorldY,
							 m_WorldTLX, m_WorldTLY, m_WorldBRX, m_WorldBRY);
	break;
	}
	return(pNode);
}

NCSQTreeNode * CNCSQTree::AddNodeInternalPR(
		NCSQTreeNode *pRootNode,
		void *pData, IEEE8 latitude, IEEE8 longitude,
		IEEE8 tlx, IEEE8 tly, IEEE8 brx, IEEE8 bry )
{
	IEEE8 Dx, Dy;

	if((tlx==0.0)&&(tly==0.0)&&(brx==0.0)&&(bry==0.0)) {
#ifdef WIN32
		MessageBox(NULL, OS_STRING("Error: no extents in AddNodeInternalPR."), OS_STRING("DEBUG"), MB_OK);
#endif
		return pRootNode; // IGNORE IT FOR NOW!
	}

	Dx = fabs(brx - tlx);
	Dy = fabs(bry - tly);

	switch (m_CoordSys) {
	case NCSCS_RAW :
		if( pRootNode == (NCSQTreeNode *)NULL) {

			pRootNode = CreateNode(latitude, longitude, pData);

			if (m_pRootNode == (NCSQTreeNode *)NULL) {
				m_pRootNode = pRootNode;
			}
		} 

		else if ( PointInRectRAW(latitude, longitude, tlx, tly, tlx+Dx/2, tly+Dy/2) ) {	// NW
			pRootNode->nw=AddNodeInternalPR(pRootNode->nw, pData, latitude, longitude,
										tlx, tly, tlx+Dx/2, tly+Dy/2);
		}
		else if ( PointInRectRAW(latitude, longitude, tlx+Dx/2,tly,brx,bry+Dy/2) ) {		// NE
			pRootNode->ne=AddNodeInternalPR(pRootNode->ne, pData, latitude, longitude,
										tlx+Dx/2,tly,brx,bry+Dy/2);
		}
		else if ( PointInRectRAW(latitude, longitude, tlx, tly+Dy/2,tlx+Dx/2,bry ) ) {		// SW
			pRootNode->sw=AddNodeInternalPR(pRootNode->sw, pData, latitude, longitude,
										tlx,tly+Dy/2,tlx+Dx/2,bry);
		}
		else if ( PointInRectRAW(latitude, longitude, tlx+Dx/2,tly+Dy/2,brx,bry) ) {		// SE
			pRootNode->se=AddNodeInternalPR(pRootNode->se, pData, latitude, longitude,
										tlx+Dx/2,tly+Dy/2,brx,bry);
		}
		else {
			_RPT2(_CRT_WARN, "SYMBOL NOT FITTING IN QUAD TREE - %lf %lf\n", latitude, longitude );
			return NULL;
		}
	break;

	case NCSCS_LL:
	case NCSCS_UTM:
	break;
	}

	return pRootNode;    /*return root pointer */
}

NCSQTreeNode * CNCSQTree::AddNodeInternalPOINT(
		NCSQTreeNode *pRootNode,
		void *pData, IEEE8 latitude, IEEE8 longitude )
{

	if( pRootNode == (NCSQTreeNode *)NULL) {

		pRootNode = CreateNode(latitude, longitude, pData);

		if (m_pRootNode == (NCSQTreeNode *)NULL) {
			m_pRootNode = pRootNode;
		}
	}

	/*if latitude and longitude equal or greater than the current node then go to nw*/
	else if((latitude >= pRootNode->dWorldX) && (longitude >= pRootNode->dWorldY)) {
		 pRootNode->nw=AddNodeInternalPOINT(pRootNode->nw, pData, latitude, longitude );
	}

	/*if latitude greater or equal to and longitude is less than current node, go ne*/
	else if((latitude >= pRootNode->dWorldX)&&(longitude < pRootNode->dWorldY)) {
		pRootNode->ne = AddNodeInternalPOINT(pRootNode->ne, pData, latitude, longitude );
	}

	/*if latitude is less or equal and longitude is greater than current node, then go sw*/
	else if((latitude <= pRootNode->dWorldX)&&(longitude > pRootNode->dWorldY)) {
		pRootNode->sw = AddNodeInternalPOINT(pRootNode->sw, pData, latitude, longitude );
	}
	/*else both latitude and longitude is less than current node go se*/
	else {
		pRootNode->se = AddNodeInternalPOINT(pRootNode->se, pData, latitude, longitude );
	}

	return pRootNode;    /*return root pointer */
}

NCSError CNCSQTree::Print( void )
{
	PrintInternal(m_pRootNode);
	return(NCS_SUCCESS);
}

NCSQTreeNode *CNCSQTree::PrintInternal( NCSQTreeNode *pRoot )
{
	if(pRoot!=NULL) {
		pRoot->nw = PrintInternal(pRoot->nw);     /* Travel to the left */
		pRoot->ne = PrintInternal(pRoot->ne);
		pRoot->sw = PrintInternal(pRoot->sw);     /* then to the right */
		pRoot->se = PrintInternal(pRoot->se);
		PrintNode(pRoot);
	}
	return (pRoot);
}

void CNCSQTree::SetNodeData( NCSQTreeNode *pNode, void *pData )
{
	if (pNode) {
		if (pNode->pData) {
			FreeNodeData(pData);
		}
		pNode->pData = (void *)pData;
	}
}

void * CNCSQTree::GetNodeData( NCSQTreeNode *pNode )
{
	if (pNode) {
		return(pNode->pData);
	}
	else {
		return((void *)NULL);
	}
}

NCSError CNCSQTree::SetAlgorithm( NCSQTreeAlg nAlg )
{
	if ( nAlg == NCSQTALG_POINT || nAlg == NCSQTALG_PR ) {
		m_nAlgorithm = nAlg;
		return(NCS_SUCCESS);
	}
	return(NCS_QT_TYPE_UNSUPPORTED);
}

NCSQTreeAlg CNCSQTree::GetAlgorithm( void )
{
	return(m_nAlgorithm);
}

NCSQTreeNode *CNCSQTree::FindNode(IEEE8 dWorldX, IEEE8 dWorldY, IEEE8 dRadiusX, IEEE8 dRadiusY )
{
	m_RadiusDistanceX = dRadiusX;
	m_RadiusDistanceY = dRadiusY;
	m_pFindNodeResult = NULL;
	FindNodeInternal(m_pRootNode, dWorldX, dWorldY );
	return(m_pFindNodeResult);	
}

NCSQTreeNode *CNCSQTree::FindNodeInternal( NCSQTreeNode *pRoot, IEEE8 latitude, IEEE8 longitude )
{
	if((pRoot!=NULL) && !m_pFindNodeResult) {

		// Find the node within the radius distance
		IEEE8 distX = fabs(pRoot->dWorldX - latitude);
		IEEE8 distY = fabs(pRoot->dWorldY - longitude);

		// If the radius^2 < x^2 + y^2 then it is within the radius
		//if ((m_RadiusDistance*m_RadiusDistance) > ((distX*distX)+(distY*distY)) ) {
		//	m_pFindNodeResult = pRoot;
		//	return(pRoot);
		//}

		if ((m_RadiusDistanceX > distX) && (m_RadiusDistanceY > distY)) {
			m_pFindNodeResult = pRoot;
			return(pRoot);
		}

		pRoot->nw = FindNodeInternal(pRoot->nw, latitude, longitude );
		pRoot->ne = FindNodeInternal(pRoot->ne, latitude, longitude );
		pRoot->sw = FindNodeInternal(pRoot->sw, latitude, longitude );
		pRoot->se = FindNodeInternal(pRoot->se, latitude, longitude );
	}
	return (pRoot);
}

NCSError CNCSQTree::Draw( void *pDrawInfo )
{
	m_NrNodes = 0;
	m_CurrDrawLevel = 0;

	DrawInternal(pDrawInfo, m_pRootNode);
	
	_RPT1(_CRT_WARN, "NCSQTree::Draw() had %d nodes.\n", m_NrNodes);

	return(NCS_SUCCESS);
}

NCSQTreeNode *CNCSQTree::DrawInternal( void *pDrawInfo, NCSQTreeNode *pRoot)
{
	m_CurrDrawLevel ++;

	if(pRoot!=NULL /*&& (m_CurrDrawLevel < 1000000 )*/) {
		pRoot->nw = DrawInternal(pDrawInfo, pRoot->nw);				/* Travel to the left */
		pRoot->ne = DrawInternal(pDrawInfo, pRoot->ne);
		pRoot->sw = DrawInternal(pDrawInfo, pRoot->sw);				/* then to the right */
		pRoot->se = DrawInternal(pDrawInfo, pRoot->se);
		DrawNode(pDrawInfo);
	}
	m_CurrDrawLevel --;
	return (pRoot);
}

NCSError CNCSQTree::InitExtents( NCSCoordSys CSysType, IEEE8 tlx, IEEE8 tly, IEEE8 brx, IEEE8 bry )
{
	m_CoordSys = CSysType;
	m_WorldTLX = tlx;
	m_WorldTLY = tly;
	m_WorldBRX = brx;
	m_WorldBRY = bry;
	return(NCS_SUCCESS);
}

NCSError CNCSQTree::Balance()
{
	// Balancing trees make them faster to access and clip/prune/traverse. 

	// If its a PR tree, rebalance it by getting the optimal extents (via scanning the tree) 
	// and setting these extents, then reinserting all nodes back into the tree.
	if (m_nAlgorithm == NCSQTALG_PR) {

		NCSQTreeBalanceData *pBalanceData = (NCSQTreeBalanceData*)NCSMalloc(sizeof(NCSQTreeBalanceData), 0);
		pBalanceData->dWorldTLX = MAXDOUBLE;
		pBalanceData->dWorldTLY = MAXDOUBLE;
		pBalanceData->dWorldBRX = -MAXDOUBLE;
		pBalanceData->dWorldBRY = -MAXDOUBLE;
		pBalanceData->nCoordSys = m_CoordSys;

		NCSQTreeNode *pTemp = m_pRootNode;

		// Calculate the extents of the unbalanced tree for optimal node insertion
		Traverse(NCSQTreeCalcExtentsFn, (void *)pBalanceData);
		
		// Setup the new balanced tree extents and copy the old tree back into thew new one
		NCSQTreeCopyData *pCopyData = (NCSQTreeCopyData*)NCSMalloc(sizeof(NCSQTreeBalanceData), 0);
		pCopyData->pFromRootNode = pTemp;
		pCopyData->pToRootNode = m_pRootNode;
		pCopyData->pQuadTree = this;
		m_pRootNode = NULL;
		InitExtents(m_CoordSys, pBalanceData->dWorldTLX, pBalanceData->dWorldTLY, pBalanceData->dWorldBRX, pBalanceData->dWorldBRY);

		NCSMutexBegin(&m_TraverseMutex);
		TraverseNode(pTemp, NCSQTreeCopyFn, (void *)pCopyData);
		NCSMutexEnd(&m_TraverseMutex);

		NCSFree(pBalanceData);
		NCSFree(pCopyData);
		FreeNode(pTemp);
	}
	return NCS_SUCCESS;
}

NCSError CNCSQTree::NCSQTreeCalcExtentsFn(NCSQTreeNode *pNode, void *pInfo)
{
	NCSQTreeBalanceData *pBalanceData = (NCSQTreeBalanceData *)pInfo;

	switch (pBalanceData->nCoordSys) {
	case NCSCS_LL:
		_RPT0(_CRT_WARN, "CalcExtentsFn: not implemented for LatLong yet.\n");
		break;
	case NCSCS_UTM:
		_RPT0(_CRT_WARN, "CalcExtentsFn: not implemented for UTM yet.\n");
		break;
	case NCSCS_RAW:
		pBalanceData->dWorldTLX = MIN(pBalanceData->dWorldTLX, pNode->dWorldX);
		pBalanceData->dWorldTLY = MIN(pBalanceData->dWorldTLY, pNode->dWorldY);
		pBalanceData->dWorldBRX = MAX(pBalanceData->dWorldBRX, pNode->dWorldX);
		pBalanceData->dWorldBRY = MAX(pBalanceData->dWorldBRY, pNode->dWorldY);
		break;
	}
	return NCS_SUCCESS;
}

NCSError CNCSQTree::NCSQTreeCopyFn(NCSQTreeNode *pNode, void *pInfo)
{
	NCSQTreeCopyData *pCopyData = (NCSQTreeCopyData *)pInfo;

	pCopyData->pQuadTree->AddNode(pNode->pData, pNode->dWorldX, pNode->dWorldY);

	pNode->pData = NULL;
	return NCS_SUCCESS;
}

