/********************************************************** 
** Copyright 1998 Earth Resource Mapping Ltd.
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
** FILE:   	NCSBlockFile.h
** CREATED:	30 Jun 2004
** AUTHOR: 	Simon Cope
** PURPOSE:	
** EDITS:
*******************************************************/

#ifndef NCSBLOCKFILE_H
#define NCSBLOCKFILE_H

#undef NCS_ODBC

#include <string>

#include "NCSEcw.h"
#include "NCSECWClient.h"
#include "NCSJP2File.h"

#ifdef NCS_ODBC
#include "sql.h"
#include "SQLEXT.H"
#endif // NCS_ODBC

/**
 * CNCSBlockFile class - JP2/ECW File abstraction for packet stream server implementations.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.10 $ $Author: russell $ $Date: 2005/08/12 06:11:26 $ 
 */
class NCS_EXPORT CNCSBlockFile {
public:
		/** Resolution level structure. */
	typedef struct {
			/** Level number. */
		UINT8	m_nLevel;
			/** Level Width. */
		UINT32	m_nWidth;
			/** Level Height. */
		UINT32	m_nHeight;
			/** Codeblock Width. */
		UINT16	m_nBlockWidth;
			/** Codeblock Height. */
		UINT16	m_nBlockHeight;
			/** # Codeblocks Across. */
		UINT32	m_nBlocksWide;
			/** # Codeblocks Down. */
		UINT32	m_nBlocksHigh;

			/** First packet/block number in level. */
		UINT32	m_nFirstPacketNr;
			/** # packets/blocks in level. */
		UINT32	m_nNrPackets;
	} ResolutionLevel;

		/** Default constructor. */
	CNCSBlockFile();
		/** virtual default destructor. */
	virtual ~CNCSBlockFile();

		/** 
		 * Open a file, optionally reading the header only.
		 * @param		pFilename	JP2/ECW file to open
		 * @param		bHeaderOnly	Only read in the header.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	CNCSError Open(char *pFilename, bool bHeaderOnly);

		/** 
		 * Close file.
		 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
		 */
	CNCSError Close();

		/** 
		 * Do we have the "block" table.
		 * @return      bool	true if we have the block table.
		 */
	bool HaveBlockTable();
		/** 
		 * Get the memory image of the file.
		 * @param		nLength		Returned length of image
		 * @return      void *		NCSMalloc()'d pointer to image.
		 */
	void *GetMemImage(UINT32 &nLength);
		/** 
		 * Get length of a packet in the file.
		 * @param		nPacket		Absolute Packet number to determine length for
		 * @return      UINT32		Packet length or 0 on failure.
		 */
	UINT32 GetPacketLength(NCSBlockId nPacket);
		/** 
		 * Read a packet from the file.
		 * @param		nPacket		Absolute Packet number to determine length for
		 * @param		nLength		Returned length of packet (header+data)
		 * @return      void *		NCSMalloc()'d pointer to read packet (header+data).
		 */
	void *GetPacket(NCSBlockId nPacket, UINT32 &nLength);
		/** 
		 * Get Zero (empty) packet from the file.
		 * @param		nLength		Returned length of packet (header+data)
		 * @return      void *		NCSMalloc()'d pointer to read packet (header+data).
		 */
	void *GetZeroPacket(UINT32 &nLength);

		/** 
		 * Get total # of packets/blocks in the file.
		 * @return      UINT32		Number of packets/blocks in the file.
		 */
	UINT32 GetNrPackets();

		/** 
		 * Get the file info structure.
		 * @return      NCSFileViewFileInfoEx *  Pointer to file info structure.
		 */

	NCSFileViewFileInfoEx *GetFileInfo();

		/** 
		 * Get resolution levels for the file.
		 * @return      std:vector<ResolutionLevel>	STL vector of levels.
		 */
	std::vector<ResolutionLevel> GetLevels();

		/** 
		 * Can we support Image Web Server security on thie file.
		 * @param		bIsJP2		Set to true on return if file is a JP2 compatible file
		 * @param		po			Set to Progression Order on return.
		 * @return      bool		True if we can do advanced security on this file.
		 */
	bool AdvancedSecurityCompatible( bool &bIsJP2, CNCSJPCProgressionOrderType::Type &po, std::string &sError );
		/** 
		 * Can we support Image Web Server security on thie file.
		 * @return      bool		True if we can do advanced security on this file.
		 */
	bool AdvancedSecurityCompatible() {
		bool bIsJP2;
		CNCSJPCProgressionOrderType::Type po;
		std::string sError;
		return AdvancedSecurityCompatible( bIsJP2, po, sError );
	}

		/** 
		 * Get the type of the open file.
		 * @return	NCSFileType	The files type.
		 */
	NCSFileType GetFileType();
	std::string m_sFileName;

private:
		/** JP2 file pointer */
	CNCSJP2File *m_pJP2File;
	void *m_pJP2MemImage;
		/** ECW File pointer */
	NCSFile	*m_pNCSFile;
#ifdef NCS_ODBC
	SQLHENV     m_hODBCEnv;
	SQLHDBC		m_hODBCCon;
	SQLHSTMT	m_hODBCCmd;
#endif // NCS_ODBC
};

#endif /* NCSBLOCKFILE_H */
