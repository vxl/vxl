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
** FILE:     $Archive: /NCS/Source/include/NCSJPCNode.h $
** CREATED:  14/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCNode class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCNODE_H
#define NCSJPCNODE_H

#ifndef NCSJPCBUFFER_H
#include "NCSJPCBuffer.h"
#endif // NCSJPCBUFFER_H

	/**
	 * CNCSJPCMarker class - the base JPC codestream marker.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.35 $ $Author: simon $ $Date: 2005/07/13 02:51:07 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCNode: public CNCSJPCBuffer, public CNCSError {
public:
	class NCSJPC_EXPORT_ALL CNCSJPCNodeTracker {
	public:
		CNCSJPCNodeTracker(char *pClassName, size_t nNodeSize);
		virtual ~CNCSJPCNodeTracker();

		void Add();
		void Remove();
		void AddMem(UINT64 nSize);
		void RemoveMem(UINT64 nSize);

//		INT32 m_nNodesTotal;		// Total # nodes allocated
		INT64 m_nMemTotal;			// Total Mem allocated
		char	m_szNodeName[32];	// Name of this node
		INT32 m_nNodeSize;			// Size of a single node

//		static INT32 sm_nNodesTotal;	// Total # nodes allocated globally
		static INT64 sm_nMemTotal;		// Total # nodes allocated globally
//		static std::vector<CNCSJPCNodeTracker*> *sm_pTrackers;
	};
		/** Default constructor, initialises members */
	CNCSJPCNode();
		/** Virtual destructor */
	virtual ~CNCSJPCNode();
	
	typedef void *ContextID;

	class NCSJPC_EXPORT_ALL Context {
	public:
		Context();
		virtual ~Context();
		virtual UINT32 NrInputs();
		virtual class CNCSJPCNode *GetInput(UINT32 n);
		virtual void SetInput(UINT32 n, CNCSJPCNode *pNode);
		virtual void SetInputBuffers(UINT32 nInput, UINT8 nBuffers);
		virtual CNCSJPCBuffer *GetInputBuffer(UINT32 nInput, UINT8 nBuffer);
		virtual CNCSJPCBuffer *FindInputBuffer(UINT32 nInput, UINT32 &nBuffer, CNCSJPCRect &r, bool &bFound, CNCSJPCBuffer::Type eType);
		virtual void AddInput(CNCSJPCNode *pNode);
		Context operator=(const Context &src);
	private:
		class NCSJPC_EXPORT_ALL CNCSJPCNodeInput {
		public:
			class CNCSJPCNode *m_pNode;
			CNCSJPCBufferCache m_Buffers;

			virtual ~CNCSJPCNodeInput();
		};
			/** Link to input nodes */
		std::vector<CNCSJPCNodeInput>m_Inputs;
	};

		/** 
		 * Link a node as the unput to this node.
		 * @param		nCtx		Read context
		 * @param		nInputs		Number of inputs to this node
		 * @param		pInput		First CNCSJPCNode to use as input to this node.
		 * @param		...			Additional inputs where required (eg, MCT)
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool Link(ContextID nCtx, UINT16 nInputs, CNCSJPCNode *pInput, ...);
		/** 
		 * Add a node as an input to this node.
		 * @param		nCtx		Read context
		 * @param		pNode		Input node to add
		 * @return      bool		true on success, else false on error.
		 */
	virtual bool AddInput(ContextID nCtx, CNCSJPCNode *pInput) { GetContext(nCtx)->AddInput(pInput); return(true); };
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

	virtual bool ReadInputLines(ContextID nCtx, UINT32 ninputs, ...);
		/** 
		 * Write a BufferType line to the output.
		 * @param		nCtx		Context
		 * @param		pSrc		Source buffer.
		 * @param		iComponent	Output Component
		 * @return      bool		true on succes, else false.
		 */
	virtual bool WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent) { 
			nCtx;pSrc;iComponent;//Keep compiler happy
			return(false); 
		};

		/** 
		 * Get the input error 
		 * @param		nCtx		Context id for input
		 * @return		CNCSError	CNCSError value of the input to this node.
		 */
	CNCSError GetError(ContextID nCtx);

protected:
	class ContextAutoPtr: public std::auto_ptr<Context> {
	public:
		ContextAutoPtr() {};
		ContextAutoPtr(Context *s): std::auto_ptr<Context>(s) {};
		ContextAutoPtr(const ContextAutoPtr &s) { 
			ContextAutoPtr P(s.get());
			*this = P;
		};
		~ContextAutoPtr() {};
	};
	std::map<ContextID, ContextAutoPtr> *m_pContext;/*std::auto_ptr<Context>*/

		/**
		 * Get the context for the given ContextID
		 * @param		nCtx		ContextID to retrive context
		 * @param		bAutoConstruct Autoconstruct a new context for this ContextID if one doesn't exist
		 * @return		Context*	Context for given ID.
		 */
	virtual Context *GetContext(ContextID nCtx, bool bAutoConstruct = true);
		/**
		 * Get the context for the given ContextID
		 * @param		nCtx		ContextID to set
		 * @param		pCtx		Context to set
		 * @return		Context*	Context for given ID.
		 */	
	virtual void SetContext(ContextID nCtx, Context *pCtx);
private:
};

#endif // !NCSJPCNODE_H

