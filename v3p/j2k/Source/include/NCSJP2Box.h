/********************************************************
** Copyright 2002 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/include/NCSJP2Box.h $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJP2Box class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJP2BOX_H
#define NCSJP2BOX_H


// Our own 64bit IOSTREAM class
#ifndef NCSJPCIOSTREAM_H
#include "NCSJPCIOStream.h"
#endif // NCSJPCIOSTREAM_H

// STD list template
#include <list>



/**
 * CNCSJP2Box class - the basic JP2 file box.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.9 $ $Author: simon $ $Date: 2005/01/17 09:07:54 $ 
 */
class NCSJPC_EXPORT_ALL CNCSJP2Box {
public:
		/** A list of boxes. */
	typedef std::vector<CNCSJP2Box *> CNCSJP2BoxList;

		/** Absolute file Offset to the Box in bytes */
	UINT64			m_nBoxOffset;
		/** Length of this box in bytes. */
	UINT64			m_nXLBox;
		/** Type of box, 4 char code stored in a UINT32. */
	UINT32			m_nTBox;
		/** Absolute file Offset to the DBox in bytes */
	UINT64			m_nDBoxOffset;
		/** Length of box data in bytes */
	UINT64			m_nLDBox;
		/** We have parsed this box */
	bool			m_bHaveBox;
		/** This box is valid */
	bool			m_bValid;
		/** Is a superbox */
	bool			m_bIsSuperBox;
		/** Box(es) that must preceed this one in the file */
	CNCSJP2BoxList	m_Prev;
		/** Box(es) that must proceed this one in the file */
	CNCSJP2BoxList	m_Next;


		/** Default constructor for a box.  Initialises members */
	CNCSJP2Box();
		/** Copy constructor for a box.  Initialises members */
	CNCSJP2Box(const CNCSJP2Box &s);
		/** Virtual destructor for a box.  Releases members */
	virtual ~CNCSJP2Box();

		/** 
		 * Set which Box(es) preceed this one in the file 
		 * @param       pFirst		NULL terminated varargs list of boxes that must preceed this one
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError SetPrevBoxes(CNCSJP2Box *pFirst, ...);  
		/** 
		 * Set which Box(es) proceed this one in the file
		 * @param       pFirst		NULL terminated varargs list of boxes that must proceed this one
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError SetNextBoxes(CNCSJP2Box *pFirst, ...); 

		/** 
		 * Update the extended length of the box before writing
		 */
	virtual void UpdateXLBox(void);
		/** 
		 * Parse the base box from the JP2 file.
		 * @param		JP2File		JP2 file being parsed
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		/** 
		 * UnParse the box to the JP2 file.
		 * @param		JP2File		JP2 file being parsed
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);

		/** 
		 * assignment operator
		 * @param       s			Source CNCSJP2Box
		 * @return      CNCSJP2Box&	Reference to post-assigned CNCSJP2Box
		 */
	CNCSJP2Box& operator=(const CNCSJP2Box& s) {
			m_nBoxOffset = s.m_nBoxOffset;
			m_nXLBox = s.m_nXLBox;
			m_nTBox = s.m_nTBox;
			m_nDBoxOffset = s.m_nDBoxOffset;
			m_nLDBox = s.m_nLDBox;
			m_bHaveBox = s.m_bHaveBox;
			m_bValid = s.m_bValid;
			return(*this);
	   };
};

#endif // !NCSJP2BOX_H
