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
** FILE:     $Archive: /NCS/Source/include/NCSJPCResolution.h $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCResolution class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCRESOLUTION_H
#define NCSJPCRESOLUTION_H

#ifndef NCSJPCPRECINCT_H
#include "NCSJPCPrecinct.h"
#endif // NCSJPCPRECINCT_H
#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H
#ifndef NCSJPCPLTMARKER_H
#include "NCSJPCPLTMarker.h"
#endif // NCSJPCPLTMARKER_H

	/**
	 * CNCSJPCResolution class - the JPC Resolution level.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.42 $ $Author: simon $ $Date: 2005/07/13 02:51:07 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCResolution: public CNCSJPCNodeTiler {
public:
		/** Component for this resolution */
	class CNCSJPCComponent *m_pComponent;
		/** Resolution level */
	UINT8	m_nResolution;
		/** Compression target ratio */
	UINT16	m_nCompressionRatio;

		/** Precincts */
	CNCSJPCPrecinctMap m_Precincts;
	//std::vector<CNCSJPCPrecinct*> m_Precincts;
		
//	CNCSJPCIOStreamVector m_EncoderFiles;
	//char			*m_pEncoderFileName;
	UINT32			m_nEncoderPLTStartIndex;
//	std::vector<UINT32>	m_EncoderPLTIndex;
//	CNCSJPCIOStreamVector m_EncoderPLTFiles;
	//char			*m_pEncoderPLTFileName;

		/** 
		 * constructor 
		 * @param pComponent	Parent component
		 * @param nResolution	Resolution level, 0-32
		 */
	CNCSJPCResolution(class CNCSJPCComponent *pComponent, UINT8 nResolution);
		/** Virtual destructor */
	virtual ~CNCSJPCResolution();

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
		 * Get Precinct Width.
		 * @return      UINT32		Un-clipped Width of precincts.
		 */
	virtual UINT32 GetPrecinctWidth();
		/** 
		 * Get Precinct Height.
		 * @return      UINT32		Un-clipped height of precincts.
		 */
	virtual UINT32 GetPrecinctHeight();
		/** 
		 * Get number of precincts wide.
		 * @return      INT32		Nr of precincts across.
		 */
	virtual UINT32 GetNumPrecinctsWide();
		/** 
		 * Get number of precincts high.
		 * @return      UINT32		Nr of precincts high.
		 */
	virtual UINT32 GetNumPrecinctsHigh();
		/** 
		 * Get codeblock Width.
		 * @return      UINT32		Un-clipped Width of codeblocks.
		 */
	virtual UINT32 GetCBWidth();
		/** 
		 * Get codeblock Height.
		 * @return      UINT32		Un-clipped height of codeblocks.
		 */
	virtual UINT32 GetCBHeight();	
	
		/** 
		 * Get CPRL/PCRL/RPCL Progression resolution multiplier.
		 * @return      INT32		Progression resolution multiplier.
		 */
	virtual INT32 GetResMultiplier();
		/** 
		 * Get CPRL/PCRL/RPCL Progression XRsiz multiplier.
		 * @return      INT32		Progression XRsiz multiplier.
		 */
	virtual INT32 GetXRsizResMultiplier();
		/** 
		 * Get CPRL/PCRL/RPCL Progression YRsiz multiplier.
		 * @return      INT32		Progression YRsiz multiplier.
		 */
	virtual INT32 GetYRsizResMultiplier();
		/** 
		 * Get CPRL/PCRL/RPCL Progression DivX factor.
		 * @return      INT32		Progression DixX Factor.
		 */
	virtual INT32 GetDivX();
		/** 
		 * Get CPRL/PCRL/RPCL Progression DivY factor.
		 * @return      INT32		Progression DixY Factor.
		 */
	virtual INT32 GetDivY();

		/** 
		 * Get CPRL/PCRL/RPCL Progression X origin check OK test result.
		 * @return      INT32		Progression DixX Factor.
		 */
	virtual bool XOriginCheckOk();
		/** 
		 * Get CPRL/PCRL/RPCL Progression Y origin check OK test result.
		 * @return      INT32		Progression DixY Factor.
		 */
	virtual bool YOriginCheckOk();
		/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		Read context
		 * @param		pDst		Destination buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
		/** 
		 * Get X0 of top left precinct for this resolution.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetPX0();
		/** 
		 * Get Y0 of top left precinct for this resolution.
		 * @return      INT32		Coordinate value.
		 */
	virtual INT32 GetPY0();

	virtual bool ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent);
		/** 
		 * Write a BufferType line to the output.
		 * @param		nCtx		Context
		 * @param		pSrc		Source buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent);

		/** 
		 * UnLink input nodes.
		 * @param		nCtx		Read context
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool UnLink(ContextID nCtx, UINT16 nInputs = 0);

	virtual bool WritePrecinctLayerPLT(CNCSJPC &JPC, CNCSJPCIOStream &Stream, CNCSJPCTilePartHeader &TilePart, CNCSJPCPLTMarker &PLT, CNCSJPCPacketLengthType &Len, INT64 &nSOTOffset, UINT32 &nBytes, UINT32 p, UINT32 l);
	virtual bool WriteLayerPLTs(CNCSJPC &JPC, CNCSJPCIOStream &Stream, CNCSJPCTilePartHeader &TilePart, CNCSJPCPLTMarker &PLT, CNCSJPCPacketLengthType &Len, INT64 &nSOTOffset, UINT32 &nBytes, UINT32 l);
	virtual bool WritePrecinctLayerPacket(CNCSJPC &JPC, CNCSJPCIOStream &Stream, UINT32 p, UINT32 l);
	virtual bool WriteLayerPackets(CNCSJPC &JPC, CNCSJPCIOStream &Stream, UINT32 l);

protected:
	static CNCSJPCNodeTracker	sm_Tracker;

	CNCSJPCBuffer	m_EncoderLL;
	CNCSJPCBuffer	m_EncoderLH;
	CNCSJPCBuffer	m_EncoderHL;
	CNCSJPCBuffer	m_EncoderHH;

	class NCSJPC_EXPORT_ALL Context: public CNCSJPCNode::Context {
	public:
			/** 2-line Output Buffer */
		CNCSJPCBuffer m_OutputBuffer;

		CNCSJPCBufferCache m_State;
		INT32			m_nStateU0;
		INT32			m_nStateU1;
		INT32			m_nStateV;

		Context();
		virtual ~Context();
	};

		/**
		 * Get the context for the given ContextID
		 * @param		nCtx		ContextID to retrive context
		 * @param		bAutoConstruct Autoconstruct a new context for this ContextID if one doesn't exist
		 * @return		Context*	Context for given ID.
		 */
	virtual CNCSJPCNode::Context *GetContext(ContextID nCtx, bool bAutoConstruct = true);

		/** 
		 * Read a BufferType line from the given subband.
		 * @param		nCtx		Read Context
		 * @param		nX			X coordinate of node within it's resolution rect.
		 * @param		nY			Y coordinate of node within it's resolution rect.
		 * @param		nWidth		Number of pixels to read.
		 * @param		eType		Buffer type
		 * @param		pBuffer		Pointer to destination buffer.
		 * @param		eSubBandType SubBand index enum.
		 * @return      bool		true on succes, else false.
		 */
	bool ReadSubBandLine(ContextID nCtx, CNCSJPCBuffer *pDst, NCSJPCSubBandType eSubBandType);
		/** 
		 * Write a BufferType line to the given subband.
		 * @param		nCtx		Context
		 * @param		pSrc		Source buffer.
		 * @param		eSubBandType SubBand index enum.
		 * @return      bool		true on succes, else false.
		 */
	bool WriteSubBandLine(ContextID nCtx, CNCSJPCBuffer *pSrc, NCSJPCSubBandType eSubBandType);
		/** 
		 * Get normal Node Width.
		 * @return      INT32		Un-clipped Width of codeblocks.
		 */
	virtual UINT32 GetNodeWidth(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetPrecinctWidth()); 
		};
		/** 
		 * Get normal Node Height.
		 * @return      INT32		Un-clipped height of codeblocks.
		 */
	virtual UINT32 GetNodeHeight(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetPrecinctHeight()); 
		};
		/** 
		 * Get number of nodes wide.
		 * @return      INT32		Nr of codeblocks across.
		 */
	virtual UINT32 GetNumNodesWide(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetNumPrecinctsWide()); 
		};
		/** 
		 * Get number of nodes high.
		 * @return      INT32		Nr of codeblocks high.
		 */
	virtual UINT32 GetNumNodesHigh(UINT16 iComponent = 0) { 
			iComponent;//Keep compiler happy
			return(GetNumPrecinctsHigh()); 
		};
		/** 
		 * Get pointer to specific node.
		 * @param		UINT32		Node nr
		 * @return      CNCSJPCNode * Ptr to node.
		 */
	virtual __inline CNCSJPCNode *GetNodePtr(UINT32 nNode, UINT16 iComponent = 0) {
					iComponent;//Keep compiler happy
					return(m_Precincts.find(nNode));
				}
	virtual bool INTERLEAVE_2D(ContextID nCtx,
								INT32 u0,
								INT32 u1,
								INT32 v0,
								INT32 v1,
								CNCSJPCBuffer &a);
	virtual bool SR_2D(ContextID nCtx,
					  INT32 u0,
					  INT32 u1,
					  INT32 v0,
					  INT32 v1,
					  CNCSJPCBuffer &Dst);
	virtual bool HOR_SR(ContextID nCtx,
						CNCSJPCBuffer &a,
						INT32 u0,
						INT32 u1,
						INT32 v0,
						INT32 v1);
	virtual CNCSJPCBuffer *GET_STATE_BUFFER(ContextID nCtx,
											INT32 u0, 
											INT32 u1, 
											INT32 v,
											CNCSJPCBuffer::Type eType);
	virtual bool VER_SR_INPUT2(ContextID nCtx,
							  INT32 u0, 
							  INT32 u1,
							  CNCSJPCBuffer *pDst,
							  CNCSJPCBuffer::Type eType);
	virtual bool VER_SR(ContextID nCtx,
						INT32 u0,
						INT32 u1,
						INT32 v0,
						INT32 v1,
						CNCSJPCBuffer &Dst);

	virtual bool SD_2D_OUTPUT4(ContextID nCtx, 
							   INT32 u0, 
							   INT32 u1, 
							   INT32 v,
							   CNCSJPCBuffer::Type eType);
	virtual bool SD_2D(ContextID nCtx,
					   CNCSJPCBuffer *pSrc);
private:
	TNCSCachedValue<UINT32> m_CBWidth;
	TNCSCachedValue<UINT32> m_CBHeight;
	TNCSCachedValue<UINT32> m_PrecinctWidth;
	TNCSCachedValue<UINT32> m_PrecinctHeight;
	TNCSCachedValue<INT32> m_NumPrecinctsWide;
	TNCSCachedValue<INT32> m_NumPrecinctsHigh;

	TNCSCachedValue<INT32> m_ResMultiplier;
	TNCSCachedValue<INT32> m_XRsizResMultiplier;
	TNCSCachedValue<INT32> m_YRsizResMultiplier;
	TNCSCachedValue<INT32> m_DivX;
	TNCSCachedValue<INT32> m_DivY;

	TNCSCachedValue<bool> m_XOriginCheckOk;
	TNCSCachedValue<bool> m_YOriginCheckOk;
	TNCSCachedValue<INT32> m_PX0;
	TNCSCachedValue<INT32> m_PY0;
};

#endif // !NCSJPCRESOLUTION_H
