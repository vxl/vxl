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
** FILE:     $Archive: /NCS/Source/include/NCSJPCPrecinct.h $
** CREATED:  11/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCPrecinct class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCPRECINCT_H
#define NCSJPCPRECINCT_H

#ifndef NCSJPCSUBBAND_H
#include "NCSJPCSubBand.h"
#endif // NCSJPCSUBBAND_H
#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H
#include "NCSJPCProgression.h"

	/**
	 * CNCSJPCPrecinct class - the JPC codestream Precinct.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.30 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCPrecinct: public CNCSJPCNode {
public:
		/** Resolution level for this precinct */
	class CNCSJPCResolution *m_pResolution;
		/** Precinct number within this resolution */
	UINT32	m_nPrecinct;

		/** SubBands in this precinct */
	CNCSJPCSubBand *m_SubBands[4];

		/** Packet number for each layer in this precinct */
	std::vector<UINT32> m_Packets;

	UINT16	m_nProgressionLayer;
		/** Precinct has zero width or height */
	bool	m_bZeroSize;		
		/** 
	     * Constructor 
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 */
	CNCSJPCPrecinct(class CNCSJPCResolution *pResolution, UINT32 nPrecinct, bool bCreateSubBands = false);
		/** Virtual destructor */
	virtual ~CNCSJPCPrecinct();

		/** 
		 * Get X0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetX0(CNCSJPCResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get X0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 GetX0(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);

		/** 
		 * Get X0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetX0();

		/** 
		 * Get Y0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetY0(CNCSJPCResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get Y0 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 GetY0(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);

		/** 
		 * Get Y0 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetY0();

		/** 
		 * Get X1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetX1(CNCSJPCResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get X1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 GetX1(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);
		/** 
		 * Get X1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetX1();
		/** 
		 * Get Y1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
	static INT32 GetY1(CNCSJPCResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get Y1 of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinctX	Precinct X within resolution level
		 * @param nPrecinctY	Precinct Y within resolution level
		 * @return      INT32	Coordinate value.
		 */
	static INT32 GetY1(CNCSJPCResolution *pResolution, INT32 nPrecinctX, INT32 nPrecinctY);

		/** 
		 * Get Y1 of this node.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetY1();
		/** 
		 * Get Width of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Width.
		 */
//	static INT32 GetWidth(CNCSJPCResolution *pResolution, INT32 nPrecinct);
		/** 
		 * Get Height of specified precinct.
		 * @param pResolution	Parent resolution level
		 * @param nPrecinct		Precinct number within resolution level
		 * @return      INT32		Coordinate value.
		 */
//	static INT32 GetHeight(CNCSJPCResolution *pResolution, INT32 nPrecinct);

		/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		read context
		 * @param		pDst		Destination buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent);

	void AddRef();
	void UnRef();
	UINT32 NrRefs();
	static bool HaveZeroRefs();
		/**
		 * Create the subbands
		 */
	virtual bool CreateSubBands(bool bRead);
		/**
		 * Destroy the subbands
		 */
	virtual bool DestroySubBands();
protected:
	static CNCSJPCNodeTracker	sm_Tracker;
	static UINT32				sm_nZeroRefs;

		/** Packet Read error is stored here */
	CNCSError	m_Error;
		/** Reference count */
	UINT32		m_nRefs;
		/**
		 * Read the packets for this precinct
		 */	
	virtual bool ReadPackets();	


};

class NCSJPC_EXPORT_ALL CNCSJPCPrecinctMap {
public:
/*	const CNCSJPCPrecinct *&operator[](const UINT& _Key) {
		std::map<UINT32, CNCSJPCPrecinct*>::iterator it = find(_Key);
		if(it == end()) {
			static CNCSJPCPrecinct *NP = NULL;
			return(NP);
		} else {
			return(it->second);
		}
	}*/
	CNCSJPCPrecinctMap();
	virtual ~CNCSJPCPrecinctMap();
	void Init(class CNCSJPCResolution *pResolution);
	void ResetProgressionLayer(void);
	CNCSJPCPrecinct *find(UINT32 nPrecinct);
	CNCSJPCPrecinct *find(UINT32 x, UINT32 y);
	CNCSJPCPrecinct *findPacketPrecinct(UINT32 nPacket);
	bool empty(UINT32 nY);
	__inline UINT32 size(void) {
		return(m_nWidth * m_nHeight);
	}	
	void remove(UINT32 nPrecinct);
	void remove(UINT32 x, UINT32 y);
	void remove(CNCSJPCPrecinct *p);
	void insert(UINT32 nPrecinct, CNCSJPCPrecinct *p);
	void insert(UINT32 x, UINT32 y, CNCSJPCPrecinct *p);
		/** 
		 * UnLink all precincts.
		 * @param		nCtx		Read context
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool UnLink(CNCSJPCNode::ContextID nCtx, UINT16 nInputs = 0);

private:
	class NCSJPC_EXPORT_ALL CNCSJPCPrecinctMapRow {
	public:
		typedef std::map<UINT32, CNCSJPCPrecinct*>::iterator CNCSJPCPrecinctMapColumnIterator;

		std::map<UINT32, CNCSJPCPrecinct*> m_Columns;

		CNCSJPCPrecinctMapRow();
		virtual ~CNCSJPCPrecinctMapRow();
	};
	typedef std::vector<CNCSJPCPrecinctMapRow>::iterator CNCSJPCPrecinctMapRowIterator;
	UINT32 m_nWidth;
	UINT32 m_nHeight;
	class CNCSJPCResolution *m_pResolution;

	std::vector<CNCSJPCPrecinctMapRow> m_Rows;
};

#endif // !NCSJPCPRECINCT_H
