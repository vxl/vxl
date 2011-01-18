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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCTagTree.cpp $
** CREATED:  28/01/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCTagTree class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCTagTree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	/** Default constructor, initialises members */

CNCSJPCTagTree::Node::Node()
{
	 m_nValue = 0xFFFF;
	 m_nState = 0;
	 m_bKnown = false;
}

void CNCSJPCTagTree::Node::Dump(void)
{
	if(m_bKnown) {
		fprintf(stdout, "node %p, parent %p, value %d, state %d, known %s\n",
						(void *) this, 
						(void *) m_pParent, 
						m_nValue, 
						m_nState, 
						"true");
	} else {
		fprintf(stdout, "node %p, parent %p, value %d, state %d, known %s\n",
						(void *) this, 
						(void *) m_pParent, 
						m_nValue, 
						m_nState, 
						"false");
	}
}

CNCSJPCTagTree::CNCSJPCTagTree()
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nLevels = 0;
}

	/** Virtual destructor */
CNCSJPCTagTree::~CNCSJPCTagTree()
{
}

inline int sizeL(int size) { return (size + 1) / 2; }

// Set the dimensions of the 2D tag tree.
void CNCSJPCTagTree::SetDimensions(INT32 nWidth, INT32 nHeight)
{	
	int levelWidth [32]; // 32 is maximum depht
	int levelHeight[32];
	int nNodes = 0;
	
	m_nWidth  = nWidth;
	m_nHeight = nHeight;
	m_nLevels = 0;

	levelWidth [0] = nWidth;
	levelHeight[0] = nHeight;

    int n;

	do {
		n = levelWidth[m_nLevels] * levelHeight[m_nLevels];
        nNodes += n;

		levelWidth [m_nLevels + 1] = sizeL(levelWidth [m_nLevels]);
		levelHeight[m_nLevels + 1] = sizeL(levelHeight[m_nLevels]);

        m_nLevels++;
	} while (n > 1);

	m_Nodes.resize(nNodes);

	if(nNodes) {
//	Node *node    = &m_Nodes[0];
//	Node *parent  = &m_Nodes[m_nWidth * m_nHeight];
//	Node *parent0 = parent;
		UINT32 nNode    = 0;
		UINT32 nParent  = m_nWidth * m_nHeight;
		UINT32 nParent0 = nParent;
		
		for (int i = 0; i < m_nLevels - 1; i++) {
			for (int j = 0; j < levelHeight[i]; j++) {
				int k = levelWidth[i];
				while (--k >= 0) {
					m_Nodes[nNode].m_pParent = &m_Nodes[nParent];
					nNode++;
					if (--k >= 0) {
						m_Nodes[nNode].m_pParent = &m_Nodes[nParent];
						nNode++;
					}
					nParent++;
				}
				if ((j & 1) || j == levelHeight[i] - 1) {
					nParent0 = nParent;
				} else {
					nParent = nParent0;
					nParent0 += levelWidth[i];
				}
			}
		}
		m_Nodes[nNode].m_pParent = NULL;
	}
}

// Parse the tree from the JPC codestream.
bool CNCSJPCTagTree::Parse(CNCSJPCIOStream &Stream, INT32 nX, INT32 nY, INT32 nAbort, bool &bVal)
{
	Node *leaf = &m_Nodes[nY * m_nWidth + nX];
	Node *stk[32 - 1];
	Node **stkptr;
	Node *node;

	stkptr = stk;
	node   = leaf;

	while (node->m_pParent)
    {
		*stkptr++ = node;
		node = node->m_pParent;
	}

	int low = 0;
	for (;;)
    {
		if (low > (int)node->m_nState) node->m_nState = (UINT16)low;
        else                     low = node->m_nState;

		while (low < nAbort && low < node->m_nValue)
        {
			bool bBit;
			if(Stream.UnStuff(bBit) == false) {
				return(false);
			}
			if (bBit) {
				node->m_nValue = (UINT16)low;
				node->m_bKnown = true;
			}
			else           low++;
		}
		node->m_nState = (UINT16)low;

		if (stkptr == stk) break;

        node = *--stkptr;
	}
	bVal = node->m_nValue < nAbort;
	return(true);
}

bool CNCSJPCTagTree::SetValue(INT32 nX, INT32 nY, INT32 nValue) 
{
    Node *node = &m_Nodes[nY * m_nWidth + nX];
	while (node && node->m_nValue > nValue) {
        node->m_nValue = (UINT16)nValue;
		node = node->m_pParent;
    }
	return(true);
}

// UnParse the tree to the JPC codestream.
bool CNCSJPCTagTree::UnParse(CNCSJPCIOStream &Stream, INT32 nX, INT32 nY, INT32 nThreshold)
{
	INT32 nLeaf = nY * m_nWidth + nX;
    Node *stk[31];
    Node **stkptr;
    Node *node;
    int low;
	bool bRet = true;

    stkptr = stk;
    node = &m_Nodes[nLeaf];
    while(node->m_pParent) {
        *stkptr++ = node;
        node = node->m_pParent;
    }

    low=0;
    for (;;) {
		if (low > node->m_nState) {
            node->m_nState = (UINT16)low;
        } else {
            low = node->m_nState;
        }
        while (low < nThreshold) {
            if (low >= node->m_nValue) {
				if (!node->m_bKnown) {
                    bRet &= Stream.Stuff(true);
                    node->m_bKnown = true;
                }
                break;
            }
			bRet &= Stream.Stuff(false);
            ++low;
        }
		node->m_nState = (UINT16)low;
        if (stkptr == stk) break;
        node = *--stkptr;
    }
	return(bRet);
}

// Dump the tag tree.  (>=LOG_LEVEL1 to dump tag tree)
bool CNCSJPCTagTree::Dump()
{
	for (UINT32 i = 0; i < m_Nodes.size(); i++) {
		m_Nodes[i].Dump();
	}
		
	return(true);
}
