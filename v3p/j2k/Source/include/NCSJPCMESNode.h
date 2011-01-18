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
** FILE:     $Archive: /NCS/Source/include/NCSJPCDCShiftNode.h $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCDCShiftNode class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCDCSHIFTNODE_H
#define NCSJPCDCSHIFTNODE_H

#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H

	/**
	 * NCSJPCDCSHIFTNODE_H class - DC level shifting.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.1 $ $Author: simon $ $Date: 2005/01/28 07:36:24 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCDCShiftNode: public CNCSJPCNode {
public:
		/** Default constructor, initialises members */
	CNCSJPCDCShiftNode();
		/** Virtual destructor */
	virtual ~CNCSJPCDCShiftNode();

		/** 
		 * Initialise this node.
		 * @param		nCtx		Read context
		 * @param		JPC			CNCSJPC instance
		 * @param		iComponent	Index of component this node is for.
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool Init(ContextID nCtx, class CNCSJPC &JPC, UINT16 iComponent);

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
		UINT8	m_nSsiz;
		virtual ~Context() {};
	};

		/**
		 * Get the context for the given ContextID
		 * @param		nCtx		ContextID to retrive context
		 * @param		bAutoConstruct Autoconstruct a new context for this ContextID if one doesn't exist
		 * @return		Context*	Context for given ID.
		 */
	virtual CNCSJPCNode::Context *GetContext(ContextID nCtx, bool bAutoConstruct = true);
};

#endif // !NCSJPCDCSHIFTNODE_H
