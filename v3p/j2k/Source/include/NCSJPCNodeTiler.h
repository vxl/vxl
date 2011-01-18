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
** FILE:     $Archive: /NCS/Source/include/NCSJPCSubBand.h $
** CREATED:  13/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCSubBand class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCNODETILER_H
#define NCSJPCNODETILER_H

#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H

	/**
	 * CNCSJPCNodeTiler class - manages tiled Nodes.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.8 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCNodeTiler: public CNCSJPCNode {
public:
		/** 
		 * Constructor 
		 */
	CNCSJPCNodeTiler();
		/** Virtual destructor */
	virtual ~CNCSJPCNodeTiler();

		/** 
		 * Get normal Node Width.
		 * @return      INT32		Un-clipped Width of codeblocks.
		 */
	virtual UINT32 GetNodeWidth(UINT16 iComponent = 0) = 0;
		/** 
		 * Get normal Node Height.
		 * @return      INT32		Un-clipped height of codeblocks.
		 */
	virtual UINT32 GetNodeHeight(UINT16 iComponent = 0) = 0;
		/** 
		 * Get number of nodes wide.
		 * @return      INT32		Nr of codeblocks across.
		 */
	virtual UINT32 GetNumNodesWide(UINT16 iComponent = 0) = 0;
		/** 
		 * Get number of nodes high.
		 * @return      INT32		Nr of codeblocks high.
		 */
	virtual UINT32 GetNumNodesHigh(UINT16 iComponent = 0) = 0;
		/** 
		 * Get pointer to specific node.
		 * @param		UINT32		Node nr
		 * @return      CNCSJPCNode * Ptr to node.
		 */
	virtual CNCSJPCNode *GetNodePtr(UINT32 nNode, UINT16 iComponent = 0) = 0;
		/** 
		 * UnLink a node as the unput to this node.
		 * @param		nCtx		Read context
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool UnLink(ContextID nCtx, UINT16 nInputs = 0);

		/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		Read context
		 * @param		pDst		Destination buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent);
		/** 
		 * Write a BufferType line to the output.
		 * @param		nCtx		Context
		 * @param		pSrc		Source buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent);
protected:
	Context	*m_pCtx;
};

#endif // !NCSJPCNODETILER_H
