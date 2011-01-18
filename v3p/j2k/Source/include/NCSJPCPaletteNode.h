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
** FILE:     $Archive: /NCS/Source/include/NCSJPCPaletteNode.h $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCPaletteNode class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPALETTENODE_H
#define NCSJPCPALETTENODE_H

#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H
#ifndef NCSJP2FILE_H
#include "NCSJP2File.h"
#endif // NCSJP2FILE_H

	/**
	 * CNCSJPCPaletteNode class - Palette node.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.5 $ $Author: russell_solaris $ $Date: 2005/07/22 05:06:14 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPaletteNode: public CNCSJPCNode {
public:
		/** Default constructor, initialises members */
	CNCSJPCPaletteNode();
		/** Virtual destructor */
	virtual ~CNCSJPCPaletteNode();

		/** 
		 * Link a node as the unput to this node.
		 * @param		nCtx		Read context
		 * @param		nInputs		Number of inputs to this node == 1
		 * @param		pInput		First CNCSJPCNode to use as input to this node.
		 * @param		Header		CNCSJP2HeaderBox
		 * @param		iComponent	Output Component index
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool Link(ContextID nCtx, UINT16 nInputs, CNCSJPCNode *pInput, class CNCSJP2File::CNCSJP2HeaderBox &Header, INT32 iComponent);

	/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		Read context
		 * @param		pDst		Destination buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent);
protected:
	static CNCSJPCNodeTracker	sm_Tracker;

	class NCSJPC_EXPORT_ALL Context: public CNCSJPCNode::Context {
	public:
		NCSEcwCellType m_eType;
		void	*m_pEntries;
		Context();
		virtual ~Context();
	};
		/**
		 * Get the context for the given ContextID
		 * @param		nCtx		ContextID to retrive context
		 * @param		bAutoConstruct Autoconstruct a new context for this ContextID if one doesn't exist
		 * @return		Context*	Context for given ID.
		 */
	virtual CNCSJPCNode::Context *GetContext(ContextID nCtx, bool bAutoConstruct = true);
};

#endif // !NCSJPCPALETTENODE_H
