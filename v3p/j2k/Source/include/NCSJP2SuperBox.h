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
** FILE:     $Archive: /NCS/Source/include/NCSJP2SuperBox.h $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJP2SuperBox class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJP2SUPERBOX_H
#define NCSJP2SUPERBOX_H

// JP2 Box class
#ifndef NCSJP2BOX_H
#include "NCSJP2Box.h"
#endif // NCSJP2BOX_H

/**
 * CNCSJP2SuperBox class - a box containing a collection of JP2 file boxes.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.10 $ $Author: simon $ $Date: 2005/01/17 09:11:17 $ 
 */
class NCSJPC_EXPORT_ALL CNCSJP2SuperBox: public CNCSJP2Box {
public:
		/** List of other box(es) that we found in this superbox - UUID, XML etc */
	CNCSJP2BoxList m_OtherBoxes;	
		/** List of other box(es) that we own and must delete on destruction */
	CNCSJP2BoxList m_OwnedBoxes;	

		/** Default constructor */
	CNCSJP2SuperBox();
		/** Virtual destructor */
	virtual ~CNCSJP2SuperBox();

		/** 
		 * Set which Box(es) can exist within this superbox
		 * @param       pFirst		NULL terminated varargs list of boxes that can exist in this superbox
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError SetBoxes(CNCSJP2Box *pFirst, ...);
		/** 
		 * Parse the base box from the JP2 file.
		 * @param		JP2File		JP2 file being parsed
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual void UpdateXLBox(void);

	virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		/** 
		 * UnParse the box to the JP2 file.
		 * @param		JP2File		JP2 file being parsed
		 * @param		Stream		IOStream to use to parse file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);

		/** 
		 * Get a box of the specified type from within this superbox.  Recursive.
		 * @param		nTBox		JP2 box type to find
		 * @param		pLast		Last box returned to iterate.
		 * @param		pbSkip		Skip a box.
		 * @return      CNCSJP2Box*	Box instance, or NULL on failure.
		 */
	CNCSJP2Box *GetBox(UINT32 nTBox, CNCSJP2Box *pLast = NULL, bool *pbSkip = NULL);
protected:
		/** List of box(es) that can exist within this superbox */
	CNCSJP2BoxList m_Boxes;	

		/** 
		 * Is this box required.  This test is in a separate method to allow derived classes of
		 * the superbox to do specific tests, eg for a GeoJP2-style UUID box.
		 * 
		 * @param		TestBox		Box being tested against
		 * @param		ThisBox		Box being tested
		 * @return      bool		true/false "Required" test result.
		 */
	virtual bool Required(CNCSJP2Box &TestBox, CNCSJP2Box &ThisBox);
};

#endif // !NCSJP2SUPERBOX_H
