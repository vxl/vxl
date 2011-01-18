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
** FILE:     $Archive: /NCS/Source/include/NCSJPCProgression.h $
** CREATED:  13/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCProgression class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPROGRESSION_H
#define NCSJPCPROGRESSION_H

#include "NCSDefs.h"
#include "NCSJPCTypes.h"
#include "NCSJPCPOCMarker.h"

	/**
	 * CNCSJPCProgression class - Implements JPC progressions.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.12 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCProgression {
public:
			/** Current Layer being decoded */
	UINT16	m_nCurTile;
			/** Current Layer being decoded */
	UINT16	m_nCurLayer;
		/** Current Resolution being decoded */
	UINT8	m_nCurResolution;
		/** Current Component being decoded */
	UINT16	m_nCurComponent;
		/** Current Precinct X being decoded */
	UINT32	m_nCurPrecinctX;
		/** Current Precinct Y being decoded */
	UINT32	m_nCurPrecinctY;
		/** Current Packet Number */
	NCSJPCPacketId m_nCurPacket;
		/** Current TX being decoded */
	INT32	m_nCurTX;
		/** Current TY being decoded */
	INT32	m_nCurTY;
		/** Current TX increment amount */
	INT32	m_nIncTX;
		/** Current TY increment amount */
	INT32	m_nIncTY;

		/** Default constructor, initialises members */
	CNCSJPCProgression();
		/** Virtual destructor */
	virtual ~CNCSJPCProgression();
	
		/** 
		 * Get the current POC marker PO if present.
		 * @return      CNCSJPCPOCMarker::ProgressionOrder	Current PO.
		 */
	CNCSJPCPOCMarker::ProgressionOrder *CurrentPO(class CNCSJPCTilePartHeader *pMainTP);
		/** 
		 * Increment the current POC marker PO if present and current PO is complete.
		 * @param		bComplete	Flag indicating if current PO is complete
		 * @return      CNCSError	NCS_SUCCESS or error on failure.
		 */
	CNCSError IncrementPO(CNCSJPCTilePartHeader *pMainTP, bool &bComplete);

		/** 
		 * Start the progression order from the beginning.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Start(CNCSJPCTilePartHeader *pMainTP);
	virtual CNCSError Start(CNCSJPCTilePartHeader *pMainTP, INT32 nComponent, INT32 nResolution);

		/** 
		 * Increment the progression order from the current location, parsing packet headers in the process.
		 * @param		pData		Data for Step function
		 * @param		bComplete	Progression order is complete.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	virtual CNCSError Iterate(CNCSJPCTilePartHeader *pMainTP, void *pData, bool &bComplete);

		/** 
		 * Perform the "Step" for this progression.
		 * @return      bool		true, or false on error & m_Error is set to actual error code.
		 */
	virtual bool Step(class CNCSJPCComponent *pComponent,
					  class CNCSJPCResolution *pResolution,
					  class CNCSJPCPrecinct *pPrecinct);
		/** 
		 * Perform the "StepRow" for this progression.
		 * The progression stepper can return true here to skip the entire row
		 * 
		 * @return      bool		true, or false on row not skipped.
		 */
	virtual bool StepRow(class CNCSJPCComponent *pComponent,
						 class CNCSJPCResolution *pResolution);

		/**
		 * Calculate the Precinct 'k' X from the current progression
		 * @return		INT32		Precinct 'k' X
		 */
	UINT32 CalculatePrecinctX(CNCSJPCTilePartHeader *pMainTP, class CNCSJPCComponent *pComponent, class CNCSJPCResolution *pResolution);
		/**
		 * Calculate the Precinct 'k' Y from the current progression
		 * @return		INT32		Precinct 'k' Y
		 */
	UINT32 CalculatePrecinctY(CNCSJPCTilePartHeader *pMainTP, class CNCSJPCComponent *pComponent, class CNCSJPCResolution *pResolution);
protected:
	// Data for step function
	void *m_pData;
	// Progression order is complete
	bool m_bDone;
	// Error code when Step() returns false
	CNCSError m_Error;
};



#endif // !NCSJPCPROGRESSION_H
