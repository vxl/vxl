/********************************************************
** Copyright 2001 Earth Resource Mapping Ltd.
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
** FILE:     $Archive: /NCS/Source/C/NCSServerUtil/NCSObject.h $
** CREATED:  3/09/2001 11:46:57 AM
** AUTHOR:   Simon Cope
** PURPOSE:  Base CNCSObject header.
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#ifndef NCSOBJECT_H
#define NCSOBJECT_H

#ifndef NCSTYPES_H
#include "NCSTypes.h"
#endif // NCSTYPES_H

#ifndef NCSDEFS_H
#include "NCSDefs.h"
#endif // NCSDEFS_H

#ifndef NCSTIMESTAMP_H
#include "NCSTimeStamp.h"
#endif // NCSTIMESTAMP_H

/**
 * CNCSObject class.  This class tracks the objects creation time, and if logging is set to LOG_LEVEL3
 * then stats about the objects lifetime are logged with a DEBUG build.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.5 $ $Author: simon $ $Date: 2005/01/17 09:11:24 $ 
 *               
 * @since        2.0
 */
class NCS_EXPORT CNCSObject {
public:
	class NCS_EXPORT CNCSObjectLife {
	public:
		__inline CNCSObjectLife(NCSTimeStampMs &tsLife) {
			m_tsCreated = NCSGetTimeStampMs();
			m_ptsLife = &tsLife;
		};
		__inline CNCSObjectLife(NCSTimeStampMs *ptsLife = NULL) {
			m_tsCreated = NCSGetTimeStampMs();
			m_ptsLife = ptsLife;
		};
		__inline virtual ~CNCSObjectLife() {
			if(m_ptsLife) {
				*m_ptsLife += NCSGetTimeStampMs() - m_tsCreated;
			}
		};
		__inline NCSTimeStampMs CreatedAt(void) {
			return(m_tsCreated);
		};
		__inline NCSTimeStampMs TimeToNow(void) {
			return(NCSGetTimeStampMs() - m_tsCreated);
		};
	protected:
		NCSTimeStampMs m_tsCreated;
		NCSTimeStampMs *m_ptsLife;
	};
	/**
	 * Default Constructor.  Writes the current timestamp at construction time to the log file
	 * if logging set to LOG_LEVEL3 (DEBUG build only).
	 * 
	 * @see          #~CNCSObject()
	 */
	CNCSObject();
	/**
	 * Destructor.  Writes the objects lifetime in ms to the log file if loggin set to LOG_LEVEL3
	 * (DEBUG build only).
	 * 
	 * @see          #CNCSObject()
	 */
	~CNCSObject();

	/**
	 * Get the timestamp for when the objects was constructed.
	 * 
	 * @return       NCSTimeStampMs		Objects construction timestamp.
	 * @see          #TimeToNow()
	 * @see			 #~CNCSObject()
	 */
	NCSTimeStampMs CreatedAt(void);
	/**
	 * Get the objects current lifetime, in ms (ie, current time - construction time in ms).
	 * 
	 * @return       NCSTimeStampMs		Objects current lifetime in ms.
	 * @see          #CreatedAt()
	 * @see			 #~CNCSObject()
	 */
	NCSTimeStampMs TimeToNow(void);

private:
	/**
	 * ObjectLife object - this does the actual work.
	 * 
	 * @see          #CNCSObject()
	 * @see			 #CreatedAt()
	 * @see			 #TimeToNow()
	 */
	//CNCSObjectLife		*m_pLife; 
	// This was changed by Russell.
	// It is only a temp solution but I had a good reason.
	// Ask me before changing back

	/**
	 * Time stamp of when the object was constructed.
	 * 
	 * @see          #CNCSObject()
	 * @see			 #CreatedAt()
	 * @see			 #TimeToNow()
	 */
	NCSTimeStampMs m_tsCreate;
};

#endif // NCSOBJECT_H

