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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2SuperBox.cpp $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJP2SuperBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJP2File.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJP2SuperBox::CNCSJP2SuperBox()
{
	m_bIsSuperBox = true;
}

// Destructor
CNCSJP2SuperBox::~CNCSJP2SuperBox()
{
	m_OtherBoxes.clear();
	while(m_OwnedBoxes.size()) {
		CNCSJP2BoxList::iterator pCur = m_OwnedBoxes.begin();
		CNCSJP2Box *pBox = *pCur;
		m_OwnedBoxes.erase(pCur);
		delete pBox;
	}
}

// Set which boxes can appear in this box
CNCSError CNCSJP2SuperBox::SetBoxes(CNCSJP2Box *pFirst, ...)
{
	va_list va;
	
	va_start(va, pFirst);

	CNCSJP2Box *pBox = pFirst;

	while(pBox) {
		m_Boxes.push_back(pBox);
		pBox = va_arg(va, CNCSJP2Box*);
	}
	va_end(va);
	return(NCS_SUCCESS);
}

void CNCSJP2SuperBox::UpdateXLBox(void)
{
	CNCSJP2Box::UpdateXLBox();
	CNCSJP2BoxList::iterator pCur = m_Boxes.begin();

	while(pCur != m_Boxes.end()) {
		if((*pCur)->m_bValid) {
			(*pCur)->UpdateXLBox();
			m_nXLBox += (*pCur)->m_nXLBox;
		}
		pCur++;
	}
	pCur = m_OtherBoxes.begin();

	while(pCur != m_OtherBoxes.end()) {
		if((*pCur)->m_bValid) {
			(*pCur)->UpdateXLBox();
			m_nXLBox += (*pCur)->m_nXLBox;
		}
		pCur++;
	}
}

// Parse the superbox from the stream.
CNCSError CNCSJP2SuperBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;
	UINT64 nStart, nCurrent;
		
	nStart = Stream.Tell();
	if(nStart != -1) {
		nCurrent = nStart;
			
		while(nCurrent < nStart + m_nLDBox) {
			CNCSJP2BoxList::iterator pCur = m_Boxes.begin();

			CNCSJP2Box Box;

			Error = Box.Parse(JP2File, Stream);

			if(Error == NCS_SUCCESS) {
				bool bFound = false;

				while(pCur != m_Boxes.end()) {
					if(Required(*(*pCur), Box)) {
						*(*pCur) = Box;
						Error = (*pCur)->Parse(JP2File, Stream);
						bFound = true;
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
						fprintf(stderr, "Parsed Box, %c%c%c%c\r\n", 
										((char*)&Box.m_nTBox)[3], 
										((char*)&Box.m_nTBox)[2],
										((char*)&Box.m_nTBox)[1],
										((char*)&Box.m_nTBox)[0]);
						fflush(stderr);
#endif // NCS_BUILD_WITH_STDERR_DEBUG_INFO
						break;
					}
					pCur++;
				}
				if(!bFound) {
					// "special" boxes
					if(Box.m_nTBox == CNCSJP2File::CNCSJP2UUIDBox::sm_nTBox) {
						CNCSJP2File::CNCSJP2UUIDBox *pBox = new CNCSJP2File::CNCSJP2UUIDBox();
						*(CNCSJP2Box*)pBox = Box;
						Error = pBox->Parse(JP2File, Stream);
						m_OtherBoxes.push_back(pBox);
						m_OwnedBoxes.push_back(pBox);
					} else if(Box.m_nTBox == CNCSJP2File::CNCSJP2XMLBox::sm_nTBox) {
						CNCSJP2File::CNCSJP2XMLBox *pBox = new CNCSJP2File::CNCSJP2XMLBox();
						*(CNCSJP2Box*)pBox = Box;
						Error = pBox->Parse(JP2File, Stream);
						m_OtherBoxes.push_back(pBox);
						m_OwnedBoxes.push_back(pBox);
					} else {
						CNCSJP2Box *pBox = new CNCSJP2Box;
						*pBox = Box;
						m_OtherBoxes.push_back(pBox);
						m_OwnedBoxes.push_back(pBox);
					}
					// Didn't recognise the box, skip past it
					Stream.Seek(Box.m_nBoxOffset + Box.m_nXLBox, CNCSJPCIOStream::START);
					Error = Stream.GetError();
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
					fprintf(stderr, "Skipped Box, %c%c%c%c\r\n", 
									((char*)&Box.m_nTBox)[3], 
									((char*)&Box.m_nTBox)[2],
									((char*)&Box.m_nTBox)[1],
									((char*)&Box.m_nTBox)[0]);
					fflush(stderr);
#endif // NCS_BUILD_WITH_STDERR_DEBUG_INFO
				}
			}
			if(Error == NCS_SUCCESS) {
				nCurrent = Stream.Tell();
				if(nCurrent == -1) {
					Error = NCS_FILEIO_ERROR;
					break;
				}
			} else {
				break;
			}
		}
	} else {
		Error = NCS_FILEIO_ERROR;
	}
	if(Error == NCS_SUCCESS) {	
		m_bValid = true;
	}
	return(Error);
}

// UnParse the box to the stream.
CNCSError CNCSJP2SuperBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	INT64 nStart = Stream.Tell();

	CNCSJP2BoxList::iterator pCur = m_Boxes.begin();

	while(pCur != m_Boxes.end()) {
		if((*pCur)->m_bValid) {
			// Make sure any auxilary XML, UUID etc boxes are written out before the Codestream box.
			if((*pCur)->m_nTBox == CNCSJP2File::CNCSJP2ContiguousCodestreamBox::sm_nTBox) {
				CNCSJP2BoxList::iterator pCur2 = m_OtherBoxes.begin();

				while(pCur2 != m_OtherBoxes.end()) {
					if((*pCur2)->m_bValid) {
						(*pCur2)->UpdateXLBox();
						if((*pCur2)->m_bValid) {
							if((*pCur2)->m_bIsSuperBox) {
								CNCSJP2Box Box = *(*pCur2);
								Error = Box.UnParse(JP2File, Stream);
							}
							Error = (*pCur2)->UnParse(JP2File, Stream);
							if(Error != NCS_SUCCESS) {
								break;
							}
						}
					}
					pCur2++;
				}
			}
			(*pCur)->UpdateXLBox();
			if((*pCur)->m_bValid) {
				if((*pCur)->m_bIsSuperBox) {
					CNCSJP2Box Box = *(*pCur);
					Error = Box.UnParse(JP2File, Stream);
				}
				Error = (*pCur)->UnParse(JP2File, Stream);
				if(Error != NCS_SUCCESS) {
					break;
				}
			}
		}
		pCur++;
	}
	if(Error == NCS_SUCCESS) {
		INT64 nEnd = Stream.Tell();
		if(nEnd != -1) {
			m_nXLBox = nEnd - nStart;
//			if(Stream.Seek(nStart, CNCSJPCIOStream::START)) {
//				Error = CNCSJP2Box::UnParse(JP2File, Stream);
//				if(Error == NCS_SUCCESS) {
//					Stream.Seek(nEnd, CNCSJPCIOStream::START);
//					Error = Stream.GetError();
//				}
//			} else {
//				Error = Stream.GetError();
//			}
		}
	}
	return(Error);
}

CNCSJP2Box *CNCSJP2SuperBox::GetBox(UINT32 nTBox, CNCSJP2Box *pLast, bool *pbSkip)
{	
	bool bSkip;
	if(!pbSkip) {
		pbSkip = &bSkip;
	}
	*pbSkip = pLast ? true : false;
	CNCSJP2BoxList::iterator pCur = m_Boxes.begin();
	CNCSJP2BoxList::iterator pEnd = m_Boxes.end();
	while(pCur != pEnd) {
		CNCSJP2Box *pBox = (*pCur);
		if(pBox->m_nTBox == nTBox) {
			if(*pbSkip) {
				if(pLast == pBox) {
					*pbSkip = false;
				}
			} else {
				return(pBox);
			}
		}
		if(pBox->m_bIsSuperBox) {
			pBox = ((CNCSJP2SuperBox*)pBox)->GetBox(nTBox, pLast, pbSkip);
			if(pBox) {
				return(pBox);
			}
		}
		pCur++;
	}
	pCur = m_OtherBoxes.begin();
	pEnd = m_OtherBoxes.end();
	while(pCur != pEnd) {
		CNCSJP2Box *pBox = (*pCur);
		if(pBox->m_nTBox == nTBox) {
			if(*pbSkip) {
				if(pLast == pBox) {
					*pbSkip = false;
				}
			} else {
				return(pBox);
			}
		}
		if(pBox->m_bIsSuperBox) {
			pBox = ((CNCSJP2SuperBox*)pBox)->GetBox(nTBox, pLast, pbSkip);
			if(pBox) {
				return(pBox);
			}
		}
		pCur++;
	}
	return(NULL);
}

bool CNCSJP2SuperBox::Required(CNCSJP2Box &TestBox, CNCSJP2Box &ThisBox)
{
	if(TestBox.m_nTBox == ThisBox.m_nTBox && TestBox.m_bValid == false && TestBox.m_bHaveBox == false) {
		return(true);
	}
	return(false);
}
