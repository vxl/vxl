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
** FILE:     $Archive: /NCS/Source/include/NCSJPCRect.h $
** CREATED:  14/03/2003 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJPCNode class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSJPCRECT_H
#define NCSJPCRECT_H

#ifndef NCSJPCTYPES_H
#include "NCSJPCTypes.h"
#endif // NCSJPCTYPES_H

	/**
	 * CNCSJPCRect class - the base JPC rectangle class.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.7 $ $Author: tom.lynch $ $Date: 2005/06/10 00:44:34 $ 
	 */	
class NCSJPC_EXPORT_ALL CNCSJPCRect {
public:
		/** Default constructor, initialises members */
	CNCSJPCRect() { 	
		m_X0.m_Value = (-1 * (signed)0x7fffffff - 1);
		m_Y0.m_Value = (-1 * (signed)0x7fffffff - 1);
		m_X1.m_Value = (-1 * (signed)0x7fffffff - 1);
		m_Y1.m_Value = (-1 * (signed)0x7fffffff - 1);
	};
		/** constructor, specify rect values */
	CNCSJPCRect(INT32 nX0, INT32 nY0, INT32 nX1, INT32 nY1) { m_X0 = nX0; m_Y0 = nY0; m_X1 = nX1; m_Y1 = nY1; };
		/** constructor, specify rect values */
	CNCSJPCRect(INT32 nX0, INT32 nY0, UINT32 nWidth, UINT32 nHeight) { m_X0 = nX0; m_Y0 = nY0; m_X1 = nX0 + nWidth; m_Y1 = nY0 + nHeight; };
		/** Virtual destructor */
	virtual ~CNCSJPCRect() {};
	
		/** 
		 * Get X0 of this rect.
		 * @return      INT32		Coordinate value.
		 */
//	virtual INT32 GetX0() { return(m_X0.Cached() ? m_X0 : 99999999); };
	virtual __inline INT32 GetX0() { return(m_X0); };
		/** 
		 * Get Y0 of this rect.
		 * @return      INT32		Coordinate value.
		 */
//	virtual INT32 GetY0() { return(m_Y0.Cached() ? m_Y0 : 99999999); };
	virtual __inline INT32 GetY0() { return(m_Y0); };
		/** 
		 * Get X1 of this rect.
		 * @return      INT32		Coordinate value.
		 */
//	virtual INT32 GetX1() { return(m_X1.Cached() ? m_X1 : 99999999); };
	virtual __inline INT32 GetX1() { return(m_X1); };
		/** 
		 * Get Y1 of this rect.
		 * @return      INT32		Coordinate value.
		 */
//	virtual INT32 GetY1() { return(m_Y1.Cached() ? m_Y1 : 99999999); };
	virtual __inline INT32 GetY1() { return(m_Y1); };
		/** 
		 * Get Width of this rect.
		 * @return      INT32		Width value.
		 */
	virtual __inline UINT32 GetWidth() { return(GetX1() - GetX0()); };
		/** 
		 * Get Height of this rect.
		 * @return      INT32		Height value.
		 */
	virtual __inline UINT32 GetHeight() { return(GetY1() - GetY0());  };

		/** 
		 * EQ operator.
		 * @return      bool		TF Rect is equal.
		 */
	bool operator==(const CNCSJPCRect& rect) const {
		return(m_X0 == rect.m_X0 && m_Y0 == rect.m_Y0 && m_X1 == rect.m_X1 && m_Y1 == rect.m_Y1);
	} 
		/** 
		 * NEQ operator.
		 * @return      bool		TF Rect is not equal.
		 */
	bool operator!=(const CNCSJPCRect& rect) const {
		return(m_X0 != rect.m_X0 || m_Y0 != rect.m_Y0 || m_X1 != rect.m_X1 || m_Y1 != rect.m_Y1);
	} 
		/** 
		 * LT operator.
		 * @return      bool		TF Rect is LT.
		 */
	bool operator<(const CNCSJPCRect& rect) const {
		return(m_X0 < rect.m_X0 || m_Y0 < rect.m_Y0);
	} 

		/** 
		 * IsValid()
		 * @return		bool		TF Rect is valid (all points calculated)
		 */
	bool IsValid(void) {
		return(m_X0.Cached() && m_Y0.Cached() && m_X1.Cached() && m_Y1.Cached());
	}
protected:
		/** X0 of rectangle */
	TNCSCachedValue<INT32> m_X0;
		/** Y0 of rectangle */
	TNCSCachedValue<INT32> m_Y0;
		/** non-inclusive X1 of rectangle */
	TNCSCachedValue<INT32> m_X1;
		/** non-inclusive Y1 of rectangle */
	TNCSCachedValue<INT32> m_Y1;
		/** Width of rectangle */
//	TNCSCachedValue<UINT32> m_Height;
		/** Height of rectangle */
//	TNCSCachedValue<UINT32> m_Width;

};

#endif // !NCSJPCRECT_H
