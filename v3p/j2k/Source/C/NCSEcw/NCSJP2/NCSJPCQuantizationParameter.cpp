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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJPCQuantizationeParameter.cpp $
** CREATED:  13/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  NCSJPCQuantizationParameter class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#include "NCSJPCQuantizationParameter.h"
#include "NCSJPC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJPCQuantizationParameter::CNCSJPCQuantizationParameter()
{
	m_Sqc.m_eStyle = NONE;
	m_Sqc.m_nGuardBits = 0;
}

// Destructor
CNCSJPCQuantizationParameter::~CNCSJPCQuantizationParameter()
{
}

// Parse the marker in from the JP2 file.
CNCSError CNCSJPCQuantizationParameter::Parse(CNCSJPC &JPC, CNCSJPCIOStream &Stream, INT32 nLength)
{
	CNCSError Error;
	&JPC;//Keep compiler happy
	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		UINT8 t8;
		UINT16 t16;
		Parameters par;

		NCSJP2_CHECKIO(ReadUINT8(t8));

		m_Sqc.m_nGuardBits = (t8 & 0xe0) >> 5;

		switch(t8 & 0x1f) {
			case 0x0:
				{
					m_Sqc.m_eStyle = NONE;
					for(int s = 0; s < (nLength - 3); s++) {
						NCSJP2_CHECKIO(ReadUINT8(t8));
						par.m_nReversible = t8 >> 3;
						m_SPqc.push_back(par);
					}
				}
				break;

			case 0x1:
					m_Sqc.m_eStyle = SCALAR_DERIVED;
					NCSJP2_CHECKIO(ReadUINT16(t16));
					par.m_Irreversible.m_nMantissa = t16 & 0x07ff;
					par.m_Irreversible.m_nExponent = (UINT8)((t16 & 0xf800) >> 11);
					m_SPqc.push_back(par);
				break;
			
			case 0x2:
				{
					m_Sqc.m_eStyle = SCALAR_EXPOUNDED;
					for(int s = 0; s < ((nLength - 3) / 2); s++) {
						NCSJP2_CHECKIO(ReadUINT16(t16));
						par.m_Irreversible.m_nMantissa = t16 & 0x07ff;
						par.m_Irreversible.m_nExponent = (UINT8)((t16 & 0xf800) >> 11);
						m_SPqc.push_back(par);
					}
				}
				break;
			
			default:
				Error = NCS_FILEIO_ERROR;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}

// UnParse the marker out to the stream.
CNCSError CNCSJPCQuantizationParameter::UnParse(CNCSJPC &JPC, CNCSJPCIOStream &Stream)
{
	CNCSError Error;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		switch(m_Sqc.m_eStyle) {
			case NONE:
				{
					NCSJP2_CHECKIO(WriteUINT8((m_Sqc.m_nGuardBits << 5) | 0x0));
					for(int s = 0; s < 1 + 3 * JPC.GetTile()->m_COD.m_SPcod.m_nLevels; s++) {
						NCSJP2_CHECKIO(WriteUINT8(m_SPqc[s].m_nReversible << 3));
					}
				}
				break;

			case SCALAR_DERIVED:
					NCSJP2_CHECKIO(WriteUINT8((m_Sqc.m_nGuardBits << 5) | 0x1));
					NCSJP2_CHECKIO(WriteUINT16(m_SPqc[0].m_Irreversible.m_nMantissa | (m_SPqc[0].m_Irreversible.m_nExponent << 11)));
				break;
			
			case SCALAR_EXPOUNDED:
				{
					NCSJP2_CHECKIO(WriteUINT8((m_Sqc.m_nGuardBits << 5) | 0x2));
					for(int s = 0; s < 1 + 3 * JPC.GetTile()->m_COD.m_SPcod.m_nLevels; s++) {
						NCSJP2_CHECKIO(WriteUINT16(m_SPqc[0].m_Irreversible.m_nMantissa | (m_SPqc[0].m_Irreversible.m_nExponent << 11)));
					}
				}
				break;
			
			default:
				Error = NCS_FILEIO_ERROR;
		}
	NCSJP2_CHECKIO_END();
	return(Error);
}
