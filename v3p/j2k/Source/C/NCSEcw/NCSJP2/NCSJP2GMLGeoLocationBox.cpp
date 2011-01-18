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
** FILE:     $Archive: /NCS/Source/C/NCSEcw/NCSJP2/NCSJP2GMLGeoLocationBox.cpp $
** CREATED:  04/12/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJP2GMLGeoLocationBox class implementation
** EDITS:    [xx] ddMmmyy NAME COMMENTS
 *******************************************************/

#define _USE_MATH_DEFINES
#include <math.h>
#include "NCSJP2File.h"
#include "../../NCSGDT2/NCSGDTEpsg.h"

#ifdef NCSJPC_USE_TINYXML
#include "../../tinyxml/tinyxml.h"
#include "../../tinyxml/tinyxml.cpp"
#include "../../tinyxml/tinyxmlparser.cpp"
#include "../../tinyxml/tinyxmlerror.cpp"
#ifndef TIXML_USE_STL
#	include "../../tinyxml/tinystr.cpp"
#endif
#endif

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#undef Deg2Rad
#undef Rad2Deg
#define Deg2Rad(x) (x * M_PI / 180.0)
#define Rad2Deg(x) (x * 180.0 / M_PI)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Constructor
CNCSJP2File::CNCSJP2GMLGeoLocationBox::CNCSJP2GMLGeoLocationBox()
{
	NCSInitFileInfoEx(&m_GMLFileInfo); 
}

// Destructor
CNCSJP2File::CNCSJP2GMLGeoLocationBox::~CNCSJP2GMLGeoLocationBox()
{
	NCSFreeFileInfoEx(&m_GMLFileInfo);
}

void CNCSJP2File::CNCSJP2GMLGeoLocationBox::UpdateXLBox(void)
{
	char buf[1024];
	
	if(FormatXML(buf,sizeof(buf)) == NCS_SUCCESS) 
	{
		CNCSJP2Box::UpdateXLBox();
		m_nXLBox += strlen(buf);
		m_bValid = true;
	} 
	else 
	{
		m_nXLBox = 0;
		m_bValid = false;
	}
}

// Parse the box from the stream
CNCSError CNCSJP2File::CNCSJP2GMLGeoLocationBox::Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	fprintf(stderr,"Parsing GML box information\n");
#endif

	CNCSError Error(NCS_SUCCESS);
	m_bValid = false;

	double dRegX = 0.0;
	double dRegY = 0.0;
	double p1[3];
	double p2[3];
	UINT32 nEPSGCode = 0;
	int nResults = 0;
	bool bSRSAttributePresent = false;
	UINT32 nImageWidth = JP2File.m_FileInfo.nSizeX;
	UINT32 nImageHeight = JP2File.m_FileInfo.nSizeY;

	NCSJP2_CHECKIO_BEGIN(Error, Stream);
		char buf[1024 + 1];
		Stream.Read(buf, NCSMin((UINT32)m_nLDBox, sizeof(buf)-1));
		buf[NCSMin((UINT32)m_nLDBox, sizeof(buf)-1)] = '\0';

        TiXmlDocument doc;
		doc.Parse(buf);
		TiXmlHandle docHandle(&doc);
		TiXmlElement *GeoLocation_1 = docHandle.FirstChild("JPEG2000_GeoLocation").FirstChild("gml:RectifiedGrid").Element();//.FirstChild( "Element" ).Child( "Child", 1 ).Element();
		if(GeoLocation_1 && GeoLocation_1->Attribute("gml:id") && !stricmp(GeoLocation_1->Attribute("gml:id"), "JPEG2000_GeoLocation_1")) {
			TiXmlElement *OriginPoint = docHandle.FirstChild("JPEG2000_GeoLocation").FirstChild("gml:RectifiedGrid").FirstChild("gml:origin").FirstChild("gml:Point").Element();
			if(OriginPoint && OriginPoint->Attribute("gml:id") && !stricmp(OriginPoint->Attribute("gml:id"), "JPEG2000_Origin")) {
				const char *pTxt = OriginPoint->Attribute("srsName");
				if(pTxt) {
					nResults += sscanf(pTxt, "epsg:%ld", &nEPSGCode);
					bSRSAttributePresent = true;
				}
				TiXmlText *Coords = docHandle.FirstChild("JPEG2000_GeoLocation").FirstChild("gml:RectifiedGrid").FirstChild("gml:origin").FirstChild("gml:Point").FirstChild("gml:coordinates").FirstChild().Text();
				if(Coords) {
					pTxt = Coords->Value();
					if(pTxt) {
						nResults += sscanf(pTxt, "%lf,%lf", &dRegX, &dRegY);
					}
				}
			}
			TiXmlElement *offsetVector = docHandle.FirstChild("JPEG2000_GeoLocation").FirstChild("gml:RectifiedGrid").FirstChild("gml:offsetVector").Element();
			if(offsetVector && offsetVector->Attribute("gml:id") && !stricmp(offsetVector->Attribute("gml:id"), "p1")) {
				TiXmlText *Coords = docHandle.FirstChild("JPEG2000_GeoLocation").FirstChild("gml:RectifiedGrid").FirstChild("gml:offsetVector").FirstChild().Text();
				if(Coords) {
					const char *pTxt = Coords->Value();
					if(pTxt) {
						nResults += sscanf(pTxt, "%lf,%lf,%lf", &p1[0], &p1[1], &p1[2]);
					}
				}

				offsetVector = (TiXmlElement*)offsetVector->NextSibling("gml:offsetVector");
				if(offsetVector && offsetVector->Attribute("gml:id") && !stricmp(offsetVector->Attribute("gml:id"), "p2")) {
					TiXmlText *Coords = ((TiXmlElement*)offsetVector->FirstChild())->ToText();
					if(Coords) {
						const char *pTxt = Coords->Value();
						if(pTxt) {
							nResults += sscanf(pTxt, "%lf,%lf,%lf", &p2[0], &p2[1], &p2[2]);
						}
					}
				}
			}
		}
	NCSJP2_CHECKIO_END();

	if((nResults == 9 && bSRSAttributePresent) || (nResults == 8 && !bSRSAttributePresent)) {
		IEEE8 dRegistrationX = dRegX + nImageHeight * p1[0];
		IEEE8 dRegistrationY = dRegY + nImageHeight * p1[1];

		if(p1[2] == 0.0 && p2[2] == 0.0) {
//				p1[0] = sin(Deg2Rad(dCWRotationDegrees)) * dCellSizeX;
//				p1[1] = cos(Deg2Rad(dCWRotationDegrees)) * dCellSizeY;
//				p2[0] = cos(Deg2Rad(dCWRotationDegrees)) * dCellSizeX; 
//				p2[1] = -sin(Deg2Rad(dCWRotationDegrees)) * dCellSizeY;

			double dCWRotationDegrees = Rad2Deg(atan(p1[0] / p2[0]));
			double dCellSizeX = p2[0] / cos(atan(p1[0] / p2[0]));
			double dCellSizeY = p1[1] / cos(atan(p1[0] / p2[0]));

			m_GMLFileInfo.fOriginX = dRegistrationX;
			m_GMLFileInfo.fOriginY = dRegistrationY;
			m_GMLFileInfo.fCellIncrementX = dCellSizeX;
			m_GMLFileInfo.fCellIncrementY = dCellSizeY;
			m_GMLFileInfo.fCWRotationDegrees = dCWRotationDegrees;

			CNCSGDTEPSG& Epsg = *CNCSGDTEPSG::Instance();
			char *pProjection = NULL;
			char *pDatum = NULL;
			NCSFree(m_GMLFileInfo.szProjection);
			NCSFree(m_GMLFileInfo.szDatum);
			if (bSRSAttributePresent && nEPSGCode && 
				(Epsg.GetProjectionAndDatum(nEPSGCode, &pProjection, &pDatum) == NCS_SUCCESS))
			{
				if(pProjection && pDatum) 
				{
					m_GMLFileInfo.szProjection= NCSStrDup(pProjection);
					m_GMLFileInfo.szDatum = NCSStrDup(pDatum);
					NCSFree(pProjection);
					NCSFree(pDatum);
				}
				else if (nEPSGCode) //EPSG code present but invalid or unrecognised?
				{
					char szEPSG[32];
					*szEPSG = '\0';
					sprintf(szEPSG,"epsg:%ld",nEPSGCode);
					m_GMLFileInfo.szProjection = NCSStrDup(szEPSG);
					m_GMLFileInfo.szDatum = NCSStrDup(szEPSG);
				}
			}
			else
			{
				m_GMLFileInfo.szDatum = NCSStrDup("RAW");
				m_GMLFileInfo.szProjection = NCSStrDup("RAW");
			}
			if(stricmp(m_GMLFileInfo.szProjection, "GEODETIC") == 0) 
				m_GMLFileInfo.eCellSizeUnits = ECW_CELL_UNITS_DEGREES;
			else 
				m_GMLFileInfo.eCellSizeUnits = ECW_CELL_UNITS_METERS;
		}
		else return NCS_JP2_GEODATA_READ_ERROR;
	}
	else return NCS_JP2_GEODATA_READ_ERROR;
	m_bValid = true;
	NCSStandardizeFileInfoEx(&m_GMLFileInfo);

	return NCS_SUCCESS;
}

// UnParse the box to the stream
CNCSError CNCSJP2File::CNCSJP2GMLGeoLocationBox::UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream)
{
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
	fprintf(stderr,"UnParsing GML box information\n");
#endif


	char buf[1024];
	CNCSError Error = FormatXML(buf, sizeof(buf));

	if(Error == NCS_SUCCESS) {
		Error = CNCSJP2Box::UnParse(JP2File, Stream);
		NCSJP2_CHECKIO_BEGIN(Error, Stream);
			Stream.Write(buf, (UINT32)strlen(buf));
		NCSJP2_CHECKIO_END();
	}
	return(Error);
}

// Format the XML corresponding to the FileInfo geolocation info
CNCSError CNCSJP2File::CNCSJP2GMLGeoLocationBox::FormatXML(char *pBuf, UINT32 nBufLen)
{
	if (NCSIsFileInfoExGeoreferenced(&m_GMLFileInfo))
	{
		CNCSGDTEPSG& Epsg = *CNCSGDTEPSG::Instance();
		char szSRSName[32];
		*szSRSName = '\0';
		UINT32 nEPSGCode = Epsg.GetEPSG(m_GMLFileInfo.szProjection, m_GMLFileInfo.szDatum);
		if (nEPSGCode) sprintf(szSRSName," srsName=\"epsg:%ld\"",nEPSGCode);
		else if (strnicmp(m_GMLFileInfo.szProjection,"epsg:",5) == 0) //we have an unknown EPSG
		{
			char *pColon = strchr(m_GMLFileInfo.szProjection,':');
			pColon++;
			nEPSGCode = atoi(pColon); //grab EPSG code
			sprintf(szSRSName," srsName=\"epsg:%ld\"",nEPSGCode);
		}
		double dRegistrationX = m_GMLFileInfo.fOriginX;
		double dRegistrationY = m_GMLFileInfo.fOriginY;
		double dCellSizeX = m_GMLFileInfo.fCellIncrementX;
		double dCellSizeY = m_GMLFileInfo.fCellIncrementY;
		//move rotation to the (-180,180] interval
		while (m_GMLFileInfo.fCWRotationDegrees <= -180.0)
			m_GMLFileInfo.fCWRotationDegrees += 360.0;
		while (m_GMLFileInfo.fCWRotationDegrees > 180.0)
			m_GMLFileInfo.fCWRotationDegrees -= 360.0;
		double dCWRotationDegrees = m_GMLFileInfo.fCWRotationDegrees;
		UINT32 nImageWidth = m_GMLFileInfo.nSizeX;
		UINT32 nImageHeight = m_GMLFileInfo.nSizeY;
		IEEE8 dMeterFactor = 1.0;

		if(m_GMLFileInfo.eCellSizeUnits == ECW_CELL_UNITS_FEET) {
			dCellSizeX *= NCS_FEET_FACTOR;
			dCellSizeY *= NCS_FEET_FACTOR;
			dRegistrationX *= NCS_FEET_FACTOR;
			dRegistrationY *= NCS_FEET_FACTOR;
		}
		
		double p1[3] = { (sin(Deg2Rad(dCWRotationDegrees)) * dCellSizeX), (cos(Deg2Rad(dCWRotationDegrees)) * dCellSizeY), 0.0 }; 
		double p2[3] = { (cos(Deg2Rad(dCWRotationDegrees)) * dCellSizeX), -(sin(Deg2Rad(dCWRotationDegrees)) * dCellSizeY), 0.0 };

		snprintf(pBuf, nBufLen, 
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
					"<JPEG2000_GeoLocation>\r\n"
					"	<gml:RectifiedGrid xmlns:gml=\"http://www.opengis.net/gml\" gml:id=\"JPEG2000_GeoLocation_1\" dimension=\"2\">\r\n"
					"		<gml:origin>\r\n"
					"			<gml:Point gml:id=\"JPEG2000_Origin\"%s>\r\n"
					"				<gml:coordinates>%.13lf,%.13lf</gml:coordinates>\r\n"
					"			</gml:Point>\r\n"
					"			</gml:origin>\r\n"
					"		<gml:offsetVector gml:id=\"p1\">%.13lf,%.13lf,%.13lf</gml:offsetVector>\r\n"
					"		<gml:offsetVector gml:id=\"p2\">%.13lf,%.13lf,%.13lf</gml:offsetVector>\r\n"
					"	</gml:RectifiedGrid>\r\n"
					"</JPEG2000_GeoLocation>\r\n",
					szSRSName,
					dRegistrationX - nImageHeight * p1[0],
					dRegistrationY - nImageHeight * p1[1],
					p1[0], p1[1], p1[2],
					p2[0], p2[1], p2[2]);
		return(NCS_SUCCESS);
	} else {
#ifdef NCS_BUILD_WITH_STDERR_DEBUG_INFO
		fprintf(stderr,"File not georeferenced: no GML box created\n");
#endif
		return(CNCSError(NCS_JP2_GEODATA_NOT_GEOREFERENCED));
	}
}

CNCSError CNCSJP2File::CNCSJP2GMLGeoLocationBox::GetFileInfo(NCSFileViewFileInfoEx &FileInfo)
{
	NCSFree(FileInfo.szDatum);
	NCSFree(FileInfo.szProjection);
	FileInfo.szDatum = NCSStrDup(m_GMLFileInfo.szDatum);
	FileInfo.szProjection = NCSStrDup(m_GMLFileInfo.szProjection);
	FileInfo.eCellSizeUnits = m_GMLFileInfo.eCellSizeUnits;
	FileInfo.fCellIncrementX = m_GMLFileInfo.fCellIncrementX;
	FileInfo.fCellIncrementY = m_GMLFileInfo.fCellIncrementY;
	FileInfo.fOriginX = m_GMLFileInfo.fOriginX;
	FileInfo.fOriginY = m_GMLFileInfo.fOriginY;
	FileInfo.fCWRotationDegrees = m_GMLFileInfo.fCWRotationDegrees;
	//NCSCopyFileInfoEx(&FileInfo, &m_FileInfo);
	return(NCS_SUCCESS);
}

CNCSError CNCSJP2File::CNCSJP2GMLGeoLocationBox::SetFileInfo(NCSFileViewFileInfoEx &FileInfo)
{
	NCSCopyFileInfoEx(&m_GMLFileInfo, &FileInfo);
	return(NCS_SUCCESS);
}

/*
void GetDatumProjection(CNCSJP2File::CNCSJP2PCSBox::GeoKey *p, NCSFileViewFileInfoEx &FileInfo)
{
	CNCSGDTEpsg Epsg;
	char *pProjection = NULL;
	char *pDatum = NULL;

	if(Epsg.GetProjDatum(p->v.nValue, &pProjection, &pDatum)) {
		if(pProjection) {
			NCSFree(FileInfo.szProjection);
			FileInfo.szProjection = pProjection;
		}
		if(pDatum) {
			NCSFree(FileInfo.szDatum);
			FileInfo.szDatum = pDatum;
		}
	} else {
		NCSFree(pProjection);
		NCSFree(pDatum);
	}
}

void GetUnits(CNCSJP2File::CNCSJP2PCSBox::GeoKey *p, NCSFileViewFileInfoEx &FileInfo)
{
	switch(p->v.nValue) {
		case Meter:
					FileInfo.eCellSizeUnits = ECW_CELL_UNITS_METERS;
				break;
		case Foot_US_Survey:
					FileInfo.eCellSizeUnits = ECW_CELL_UNITS_FEET;
				break;
		case Foot:
		case Foot_Modified_American:
		case Foot_Clarke:
		case Foot_Indian:
		case Link:
		case Link_Benoit:
		case Link_Sears:
		case Chain_Benoit:
		case Chain_Sears:
		case Yard_Sears:
		case Yard_Indian:
		case Fathom:
		case Mile_International_Nautical:
		default:
					FileInfo.eCellSizeUnits = ECW_CELL_UNITS_UNKNOWN;
				break;
	}
}
*/
