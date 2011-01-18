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

#ifndef NCSJPCSUBBAND_H
#define NCSJPCSUBBAND_H

#include "NCSJPCTagTree.h"
#include "NCSJPCTypes.h"
#include "NCSJPCNodeTiler.h"
//#include "NCSJPCCodeBlock.h"

	/**
	 * CNCSJPCSubBand class - the JPC codestream SubBand.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.30 $ $Author: simon $ $Date: 2005/07/13 02:51:07 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCSubBand: public CNCSJPCNodeTiler {
public:
		/** Precincts for this SubBand */
	class CNCSJPCPrecinct	*m_pPrecinct;

		/** Type of subband */
	NCSJPCSubBandType	m_eType;

		/** Layer # each codeblock is first included in */
	CNCSJPCTagTree	m_LayerInclusion;
		/** # of zero bit planes for each codeblock */
	CNCSJPCTagTree	m_ZeroPlanes;
	
		/** CodeBlock data */
	std::vector<class CNCSJPCCodeBlock> *m_pCodeBlocks;

		/** 
		 * Constructor 
		 * @param pPrecinct		Parent precinct
		 * @param eType			SubBand type LL,LH,HL,HH
		 */
	CNCSJPCSubBand(class CNCSJPCPrecinct *pPrecinct, NCSJPCSubBandType eType);
		/** Virtual destructor */
	virtual ~CNCSJPCSubBand();

		/** 
		 * Get X0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetX0(class CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType);
		/** 
		 * Get X0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetX0(class CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType);
		/** 
		 * Get X0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetX0();
		/** 
		 * Get Y0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetY0(class CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType);
		/** 
		 * Get Y0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetY0(class CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType);
		/** 
		 * Get Y0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetY0();
		/** 
		 * Get X1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetX1(class CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType);
		/** 
		 * Get X1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetX1(class CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType);
		/** 
		 * Get X1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetX1();
		/** 
		 * Get Y1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetY1(class CNCSJPCResolution *pResolution, INT32 nPrecinct, NCSJPCSubBandType eType);
		/** 
		 * Get Y1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetY1(class CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY, NCSJPCSubBandType eType);
		/** 
		 * Get Y1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetY1();
		/** 
		 * Get Width of this node.
		 * @return      INT32		Width.
		 */
//	static INT32 GetWidth(class CNCSJPCResolution *pResolution, INT32 nPrecinct, Type eType);
		/** 
		 * Get Height of this node.
		 * @return      INT32		Width.
		 */
//	static INT32 GetHeight(class CNCSJPCResolution *pResolution, INT32 nPrecinct, Type eType);

		/** 
		 * Get codeblock Width.
		 * @return      INT32		Un-clipped Width of codeblocks.
		 */
	virtual UINT32 GetCBWidth();
		/** 
		 * Get codeblock Height.
		 * @return      INT32		Un-clipped height of codeblocks.
		 */
	virtual UINT32 GetCBHeight();
		/** 
		 * Get number of codeblocks wide.
		 * @return      INT32		Nr of codeblocks across.
		 */
	virtual UINT32 GetNumCBWide();
		/** 
		 * Get number of codeblocks high.
		 * @return      INT32		Nr of codeblocks high.
		 */
	virtual UINT32 GetNumCBHigh();

		/** 
		 * Get the orientation value from the given subband type
		 * @param		etype		NCS SubBand type.
		 * @return      UINT8		SubBand orientation value.
		 */
	static NCS_INLINE UINT8 GetOrient(NCSJPCSubBandType eType) {
		switch(eType) {
			default:
			case NCSJPC_LL: return(0);
			case NCSJPC_LH: return(1);
			case NCSJPC_HL: return(2);
			case NCSJPC_HH: return(3);
		}
	};

protected:
	static CNCSJPCNodeTracker	sm_Tracker;

		/** 
		 * Get normal Node Width.
		 * @return      INT32		Un-clipped Width of codeblocks.
		 */
	virtual UINT32 GetNodeWidth(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetCBWidth()); 
		};
		/** 
		 * Get normal Node Height.
		 * @return      INT32		Un-clipped height of codeblocks.
		 */
	virtual UINT32 GetNodeHeight(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetCBHeight()); 
		};
		/** 
		 * Get number of nodes wide.
		 * @return      INT32		Nr of codeblocks across.
		 */
	virtual UINT32 GetNumNodesWide(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetNumCBWide()); 
		};
		/** 
		 * Get number of nodes high.
		 * @return      INT32		Nr of codeblocks high.
		 */
	virtual UINT32 GetNumNodesHigh(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetNumCBHigh()); 
		};
		/** 
		 * Get pointer to specific node.
		 * @param		UINT32		Node nr
		 * @return      CNCSJPCNode * Ptr to node.
		 */
	virtual CNCSJPCNode *GetNodePtr(UINT32 nNode, UINT16 iComponent = 0);

private:
	TNCSCachedValue<UINT32> m_NumCBWide;
	TNCSCachedValue<UINT32> m_NumCBHigh;
};

#endif // !NCSJPCSUBBAND_H
