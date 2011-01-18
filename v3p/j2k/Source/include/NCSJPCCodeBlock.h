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
** FILE:     $Archive: /NCS/Source/include/NCSJPCCodeBlock.h $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCCodeBlock class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCCODEBLOCK_H
#define NCSJPCCODEBLOCK_H

#ifndef NCSJPCTYPES_H
#include "NCSJPCTypes.h"
#endif // NCSJPCTYPES_H
#include "NCSJPCCodingStyleParameter.h"
#include "NCSJPCQuantizationParameter.h"
#ifndef NCSJPCNODE_H
#include "NCSJPCNode.h"
#endif // NCSJPCNODE_H
#ifndef NCSJPCT1CODER_H
#include "NCSJPCT1Coder.h"
#endif // NCSJPCT1CODER_H

	/**
	 * CNCSJPCCodeBlock class - the JPC codestream CodeBlock.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.38 $ $Author: simon $ $Date: 2005/01/17 09:11:17 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCCodeBlock: public CNCSJPCNode {
public:
		/** SubBand for this code block */
	class CNCSJPCSubBand *m_pSubBand;

		/** Lblock value - UINT8 represents <=256bits LENGTH field, = 32 bytes */
	UINT8	m_nLblock;
		/** Codeblock has zero width or height */
	bool	m_bZeroSize;
		/** Segments for this codeblock */	
	std::vector<CNCSJPCSegment>	m_Segments;
		/** Next Segments for this codeblock */	
	std::vector<CNCSJPCSegment>	m_NextSegments;

		/** Codeblock number within subband */
	UINT32	m_nCB;
		/** Layer # first included on */
	UINT32	m_nLayerIncluded;
		/** Number of passes included */
	UINT8	m_nPasses;
		/** Number of zero bits */
	UINT8	m_nZeroBits;

		/** Default Constructor */
	CNCSJPCCodeBlock();
		/** Copy Constructor */
	CNCSJPCCodeBlock(const CNCSJPCCodeBlock &s);
		/** 
		 * Constructor 
		 * @param pSubBand	Parent subband this block belongs to
		 * @param nCB		Codeblock number within this subband
		 */
	CNCSJPCCodeBlock(CNCSJPCSubBand *pSubBand, UINT32 nCB);
		/** Virtual destructor */
	virtual ~CNCSJPCCodeBlock();
		/** 
		 * Init
		 * @param pSubBand	Parent subband this block belongs to
		 * @param nCB		Codeblock number within this subband
		 */
	void Init(CNCSJPCSubBand *pSubBand, UINT32 nCB);

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

	virtual INT32 ReadNewSegs(CNCSJPCIOStream &Stream);
	virtual INT32 WriteNewSegs(CNCSJPCIOStream &Stream);

		/** Decoded codeblock */
	CNCSJPCBuffer	m_DecBuf;

	static CNCSJPCT1Coder	sm_Coder;

protected:
	static CNCSJPCNodeTracker	sm_Tracker;

		/** 
		 * Decode the codeblock.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	CNCSError Decode(Type eType);

private:
	friend class CNCSJP2FileView;
	static NCSTimeStampUs sm_usTotal;
	static NCSTimeStampUs sm_usLast;
	static UINT64 sm_nTotalSamples;
	static UINT64 sm_nLastSamples;
};

#endif // !NCSJPCCODEBLOCK_H
