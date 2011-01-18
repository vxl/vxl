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
** FILE:     $Archive: /NCS/Source/include/NCSJPCICC2RGBNode.h $
** CREATED:  20/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCICC2RGBNode class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCICCNODE_H
#define NCSJPCICCNODE_H

#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H

#ifdef NCSJPC_USE_LCMS
#include "lcms.h"
#endif // NCSJPC_USE_LCMS
	/**
	 * CNCSJPCICC2RGBNode class - ICC profile to RGB colorspace conversion node using 
	 * the LittleCM library available from www.littlecms.com.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: russell_linux $ $Date: 2005/01/18 03:09:04 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCICC2RGBNode: public CNCSJPCNode {
public:
		/** Default constructor, initialises members */
	CNCSJPCICC2RGBNode(class CNCSJPC &JPC, void *pICCProfile, UINT32 nLen);
		/** Virtual destructor */
	virtual ~CNCSJPCICC2RGBNode();

#ifdef NCSJPC_USE_LCMS
		/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		Read context
		 * @param		pDst		Destination buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent);
protected:
	cmsHPROFILE m_ICCProfile;
	cmsHPROFILE m_sRGBProfile;
	cmsHTRANSFORM m_Transform;
	class CNCSJPC *m_pJPC;

	static CNCSJPCNodeTracker	sm_Tracker;
#endif //NCSJPC_USE_LCMS
};

#endif // !NCSJPCICCNODE_H
