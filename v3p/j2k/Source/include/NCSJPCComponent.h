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
** FILE:     $Archive: /NCS/Source/include/NCSJPCComponent.h $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCComponent class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCCOMPONENT_H
#define NCSJPCCOMPONENT_H

#ifndef NCSJPCCODMARKER_H
#include "NCSJPCCODMarker.h"
#endif // NCSJPCCODMARKER_H
#ifndef NCSJPCQCDMARKER_H
#include "NCSJPCQCDMarker.h"
#endif // NCSJPCQCDMARKER_H
#ifndef NCSJPCRESOLUTION_H
#include "NCSJPCResolution.h"
#endif // NCSJPCRESOLUTION_H
#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H

	/**
	 * CNCSJPCComponent class - the JPC Tile Component.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.20 $ $Author: tom.lynch $ $Date: 2005/05/24 01:53:46 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCComponent: public CNCSJPCNode {
public:

	static const UINT32 CREATE;
	static const UINT32 ADDREF;
	static const UINT32 UNREF;
	static const UINT32 STATS;
	static const UINT32 REQUEST;
	static const UINT32 CANCEL;

		/** Tile-Part for this component */
	class CNCSJPCTilePartHeader *m_pTilePart;

		/** Component index */
	UINT16	m_iComponent;
		/** Component is paletted */
	bool	m_bPaletted;

		/** CodingStyle */
	CNCSJPCCODMarker	m_CodingStyle;
		/** QuantizationStyle */
	CNCSJPCQCDMarker	m_QuantizationStyle;
		/** Decomposition Resolution levels */
	std::vector<CNCSJPCResolution*> m_Resolutions;

		/** 
		 * Constructor
		 * @param pTilePart		Parent tile part
		 * @param iComponent	Component index
		 */
	CNCSJPCComponent(CNCSJPCTilePartHeader *pTilePart, UINT16 iComponent);
		/** Virtual destructor */
	virtual ~CNCSJPCComponent();

		/** 
		 * Get X0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetX0();
		/** 
		 * Get Y0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetY0();
		/** 
		 * Get X1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetX1();
		/** 
		 * Get Y1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetY1();
		/**
		 * Get the context for the given ContextID
		 * @param		nCtx		ContextID to retrive context
		 * @param		bAutoConstruct Autoconstruct a new context for this ContextID if one doesn't exist
		 * @return		Context*	Context for given ID.
		 */
	virtual CNCSJPCNode::Context *GetContext(ContextID nCtx, bool bAutoConstruct = true);
		/** 
		 * Link this node to the view.
		 * @param		nCtx		Read context
		 * @param		nResolution Input resolution level.
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool Link(CNCSJPCNode::ContextID nCtx, UINT32 nResolution, INT32 nXCMul, INT32 nYCMul);
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
	void TraversePyramid(UINT32 nResolution, UINT32 Flags, INT32 nTopX, INT32 nLeftY, INT32 nBottomX, INT32 nRightY, UINT32 nViewSizeX, UINT32 nViewSizeY, UINT32 &nBlocksTotal, UINT32 &nBlocksAvailable);
protected:
	static CNCSJPCNodeTracker	sm_Tracker;
	
	class NCSJPC_EXPORT_ALL Context: public CNCSJPCNode::Context {
	public:
		INT32 m_nXCMul;
		INT32 m_nYCMul;
		Context();
		virtual ~Context();
	};

};

#endif // !NCSJPCCOMPONENT_H
