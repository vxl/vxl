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
** FILE:     $Archive: /NCS/Source/include/NCSJPCTagTree.h $
** CREATED:  28/01/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCTagTree class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCTAGTREE_H
#define NCSJPCTAGTREE_H

#ifndef NCSJPCIOSTREAM_H
#include "NCSJPCIOStream.h"
#endif
#ifdef NOTDEF
#ifndef NCSJPCDUMP_H
#include "NCSJPCDump.h"
#endif // NCSJPCDUMP_H
#endif

#include <vector>

	/**
	 * CNCSJPCTagTree class - the JPC codestream TagTree Header.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.15 $ $Author: simon $ $Date: 2005/01/17 09:11:24 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCTagTree
#ifdef NOTDEF
: public CNCSJPCDump 
#endif //_DEBUG
{
public:
		/** Default constructor, initialises members */
	CNCSJPCTagTree();
		/** Virtual destructor */
	virtual ~CNCSJPCTagTree();

		/** 
		 * Set the dimensions of the 2D tag tree.
		 * @param		nWidth		Number of columns in the tag tree.
		 * @param		nHeight		Number of rows in the tag tree.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual void SetDimensions(INT32 nWidth, INT32 nHeight);
		/** 
		 * Parse the tree from the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		nX			X coordinate of cell to terminate parsing on.
		 * @param		nY			Y coordinate of cell to terminate parsing on.
		 * @param		nAbort		Abort parsing when cell value >= this value
		 * @return      bool	NCS_SUCCESS, or Error code on failure.
		 */
	virtual bool Parse(CNCSJPCIOStream &Stream, INT32 nX, INT32 nY, INT32 nAbort, bool &bVal);
		/** 
		 * UnParse the tree to the JPC codestream.
		 * @param		Stream		IOStream to use to parse file.
		 * @param		nX			X coordinate of cell to terminate unparsing on.
		 * @param		nY			Y coordinate of cell to terminate unparsing on.
		 * @return      bool	NCS_SUCCESS, or Error code on failure.
		 */
	virtual bool UnParse(CNCSJPCIOStream &Stream, INT32 nX, INT32 nY, INT32 nThreshold);
	virtual bool SetValue(INT32 nX, INT32 nY, INT32 value);

		/** 
		 * Dump the tag tree.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual bool Dump();
protected:
    class Node {
    public:
        Node();

		void Dump(void); 

	    Node *m_pParent;

	    UINT16 m_nValue;
	    UINT16 m_nState; // lower bound on the value associated with this node

	    bool m_bKnown;
    };

protected:
	std::vector<Node> m_Nodes;

	UINT32  m_nWidth;
	UINT32  m_nHeight;
	UINT16	m_nLevels;
};

#endif // NCSJPCTAGTREE_H
