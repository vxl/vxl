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
** FILE:     $Archive: /NCS/Source/include/NCSJPCMCTNode.h $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCMCTNode class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCMCTNODE_H
#define NCSJPCMCTNODE_H

#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H

	/**
	 * CNCSJPCMCTNode class - Multiple Component Transform.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.11 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCMCTNode: public CNCSJPCNode {
public:
		/** Default constructor, initialises members */
	CNCSJPCMCTNode();
		/** Virtual destructor */
	virtual ~CNCSJPCMCTNode();

		/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		Read context
		 * @param		pDst		Destination buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent);
		/** 
		 * Write a BufferType line .
		 * @param		nCtx		Write context
		 * @param		pSrc		Source buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent);
protected:
	static CNCSJPCNodeTracker	sm_Tracker;

		/** 
		 * Read lines from the input.
		 * @param		nCtx		ContextID
		 * @param		pCtx		Pointer to Read context
		 * @param		pBuf0		Pointer to buffer0.
		 * @param		pBuf1		Pointer to buffer1.
		 * @param		pBuf2		Pointer to buffer2.
		 * @return      bool		true on succes, else false.
		 */
	bool ReadInputLines(ContextID nCtx, Context *pCtx, CNCSJPCBuffer *pBuf0, CNCSJPCBuffer *pBuf1, CNCSJPCBuffer *pBuf2);
};

#endif // !NCSJPCMCTNODE_H
