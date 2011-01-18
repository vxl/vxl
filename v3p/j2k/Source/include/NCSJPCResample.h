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
** FILE:     $Archive: /NCS/Source/include/NCSJPCResample.h $
** CREATED:  12/02/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCResample class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCRESAMPLE_H
#define NCSJPCRESAMPLE_H

#ifndef NCSJPCPRECINCT_H
#include "NCSJPCPrecinct.h"
#endif // NCSJPCPRECINCT_H
#ifndef NCSJPCNODETILER_H
#include "NCSJPCNodeTiler.h"
#endif // NCSJPCNODETILER_H
#ifndef NCSJPC_H
#include "NCSJPC.h"
#endif // NCSJPC_H
#ifndef NCSECWCLIENT_H
#include "NCSECWClient.h"
#endif // NCSECWCLIENT_H
#ifndef NCSJPCMCTNODE_H
#include "NCSJPCMCTNode.h"
#endif // NCSJPCMCTNODE_H
#ifndef NCSJP2FILE_H
#include "NCSJP2File.h"
#endif // NCSJP2FILE_H

	/**
	 * CNCSJPCResample class - Merges multiple tiles together, and resamples the output to the specified view..
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.14 $ $Author: simon $ $Date: 2005/01/17 09:11:23 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCResample: public CNCSJPCNodeTiler {
public:
		/** Default constructor */
	CNCSJPCResample();
		/** Virtual destructor */
	virtual ~CNCSJPCResample();

		/** 
		 * Link this node to the view.
		 * @param		nCtx		Read context
		 * @param		JPC			Codestream object.
		 * @param		pView		Current View to resample to
		 * @param		iComponent	Component index
		 * @param		iChannel	Channel index
		 & @param		pMCT		CNCSJPCMCTNode when MCT is required
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool Link(ContextID nCtx, CNCSJPC &JPC, class CNCSJP2FileView *pFileView, UINT16 iComponent, UINT16 iChannel, CNCSJP2File::CNCSJP2HeaderBox &Header);
		/** 
		 * UnLink a node as the unput to this node.
		 * @param		nCtx		Read context
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool UnLink(ContextID nCtx, UINT16 nInputs = 0);
		/** 
		 * Read a BufferType line from the input.
		 * @param		nCtx		Read context
		 * @param		pDst		Destination buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool ReadLine(ContextID nCtx, CNCSJPCBuffer *pDst, UINT16 iComponent);

	static UINT8 CalculateResolutionLevel(CNCSJPCComponent *pComponent, INT32 nTopX, INT32 nLeftY, INT32 nBottomX, INT32 nRightY, UINT32 nSizeX, UINT32 nSizeY);
protected:
	static CNCSJPCNodeTracker	sm_Tracker;

	class NCSJPC_EXPORT_ALL Context: public CNCSJPCNode::Context {
	public:
		const NCSFileViewSetInfo *m_pView;

		class TileInput {
		public:
			class ChannelInput {
			public:
				INT32 m_nResSample;
				IEEE8 m_dResSampleFactorX;
				IEEE8 m_dResSampleFactorY;

				UINT32 m_nResolutionWidth;
				UINT32 m_nResolutionHeight;	

				UINT32 m_nMax;
				INT32 m_nMin;

				CNCSJPCNode		*m_pInput;

				ChannelInput();
			};
			std::vector<ChannelInput> m_CI;			
				// Multi-component transform, only if enabled in COD
			CNCSJPCMCTNode	*m_pMCT;
				// Color-space transformation (eg, YCrCb->sRGB), only if enabled
			CNCSJPCNode		*m_pCSC;
			TileInput();
		};

		std::vector<TileInput> m_Tiles;

		Context();
		virtual ~Context();
	};

		/** Clamp output data to valid range */
	bool m_bClamp;

	CNCSJPC *m_pJPC;
		/**
		 * Get the context for the given ContextID
		 * @param		nCtx		ContextID to retrive context
		 * @param		bAutoConstruct Autoconstruct a new context for this ContextID if one doesn't exist
		 * @return		Context*	Context for given ID.
		 */
	virtual CNCSJPCNode::Context *GetContext(ContextID nCtx, bool bAutoConstruct = true);
	
	CNCSJPCNode *BuildInputChain(ContextID nCtx, CNCSJPC &JPC, class CNCSJP2FileView *pFileView, UINT16 iComponent, UINT16 iChannel, CNCSJP2File::CNCSJP2HeaderBox &Header, CNCSJPCTilePartHeader *pTilePart);
		/** 
		 * Get normal Node Width.
		 * @return      INT32		Un-clipped Width of codeblocks.
		 */
	virtual UINT32 GetNodeWidth(UINT16 iComponent = 0);
		/** 
		 * Get normal Node Height.
		 * @return      INT32		Un-clipped height of codeblocks.
		 */
	virtual UINT32 GetNodeHeight(UINT16 iComponent = 0);
		/** 
		 * Get number of nodes wide.
		 * @return      INT32		Nr of codeblocks across.
		 */
	virtual UINT32 GetNumNodesWide(UINT16 iComponent = 0);
		/** 
		 * Get number of nodes high.
		 * @return      INT32		Nr of codeblocks high.
		 */
	virtual UINT32 GetNumNodesHigh(UINT16 iComponent = 0);
		/** 
		 * Get pointer to specific node.
		 * @param		UINT32		Node nr
		 * @return      CNCSJPCNode * Ptr to node.
		 */
	virtual CNCSJPCNode *GetNodePtr(UINT32 nNode, UINT16 iComponent = 0);
};

#endif // !NCSJPCRESAMPLE_H
