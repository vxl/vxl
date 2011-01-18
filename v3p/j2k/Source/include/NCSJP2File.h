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
** FILE:     $Archive: /NCS/Source/include/NCSJP2File.h $
** CREATED:  28/11/2002 3:27:34 PM
** AUTHOR:   Simon Cope
** PURPOSE:  CNCSJP2File class header
** EDITS:    [xx] ddMmmyy NAME COMMENTS
**			 [01] 03Aug04 tfl  Added integer member to NCSJP2PCSBox
**			 [02] 07Dec04 tfl  Moved GeodataUsage enum declaration to NCSECWClient.h
**			 [03] 17Dec04 tfl  Added static CNCSJP2File::Shutdown() function called by CNCSJP2FileView::Shutdown()
 *******************************************************/

#ifndef NCSJP2FILE_H
#define NCSJP2FILE_H

#ifndef NCSJP2DEFS_H
#include "NCSJPCDefs.h"
#endif // NCSJP2DEFS_H

#ifndef NCSECWCLIENT_H
#include "NCSECWClient.h"
#endif // NCSECWCLIENT_H

//#ifndef NCSECW_H
//#include "NCSEcw.h"
//#endif // NCSECW_H

// Our own 64bit IOSTREAM class
#ifndef NCSJPCFileIOSTREAM_H
#include "NCSJPCFileIOStream.h"
#endif // NCSJPCFileIOSTREAM_H

// JP2 Box class
#ifndef NCSJP2BOX_H
#include "NCSJP2Box.h"
#endif // NCSJP2BOX_H

// JP2 SuperBox class
#ifndef NCSJP2SUPERBOX_H
#include "NCSJP2SuperBox.h"
#endif // NCSJP2SUPERBOX_H

// JPC ComponentDepthType
#ifndef NCSJPCCOMPONENTDEPTHTYPE_H
#include "NCSJPCComponentDepthType.h"
#endif // NCSJPCCOMPONENTDEPTHTYPE_H

// JPC codestream classes
#ifndef NCSJPC_H
#include "NCSJPC.h"
#endif // NCSJPC_H

#define ECW_COMPRESS_SDK_LIMIT_MASK			0xfe43218a
#define UUID_LEN							16
#define DEFAULT_GEODATA_PRECISION			0.000000001F

/**
 * CNCSJP2File class - the JP2 file class.
 * Note, the JP2File class is a superbox!
 * The JP2File is the base shared file object used by multiple CNCSJP2FileView classes to 
 * access a JP2 file.
 * 
 * @author       Simon Cope
 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
 */
class NCSJPC_EXPORT CNCSJP2File: public CNCSJP2SuperBox, public CNCSJPCNodeTiler {
public:
	
	/**
	 * CNCSJP2FileVector class 
	 * The JP2FileVector is a vector containing all the CNCSJP2File object instances.
	 * There is only one instance of this class.
	 * 
	 * @author       Simon Cope
	 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
	 */
	class NCSJPC_EXPORT_ALL CNCSJP2FileVector: public std::vector<CNCSJP2File *> {
	public:
			/** Default constructor, initialises ECW library */
		CNCSJP2FileVector() {
#ifndef LIBECWJ2
			NCSecwInitInternal();
#endif
		}
			/** Virtual destructor, clean up */
		virtual ~CNCSJP2FileVector() { 
			CloseAll(); 
#ifndef LIBECWJ2
			NCSecwShutdownInternal();
#endif
		};
			/** 
			 * CloseAll() - closes all open CNCSJP2File instances
			 */
		void CloseAll(void);
			/** 
			 * Lock() - Lock the vector
			 */
		void Lock(void) {
			NCSecwGlobalLock();
		};
			/** 
			 * TryLock() - Try to Lock the vector
			 */
		bool TryLock(void) {
			return(NCSecwGlobalTryLock() ? true : false);
		};
			/** 
			 * UnLock() - Unlock the vector
			 */
		void UnLock(void) {
			NCSecwGlobalUnLock();
		};
	};
		/**
		 * CNCSJP2SignatureBox class - the JP2 signature box.  This box is always present.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2SignatureBox: public CNCSJP2Box {
	public:
			/** Box type */ 
		static UINT32	sm_nTBox;
			/** JP2 file signature */
		static UINT32	sm_JP2Signature;

			/** Signature as read from file */
		UINT32		m_nSignature;

			/** Default constructor, initialises members */
		CNCSJP2SignatureBox();
			/** Virtual destructor */
		virtual ~CNCSJP2SignatureBox();
			/** 
			 * Update the extended length of the box prior to writing
			 */
		virtual void UpdateXLBox(void);

			/** 
			 * Parse the signature box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the signature box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
	};

		/**
		 * CNCSJP2FileTypeBox class - the JP2 file type box class.  This box is always present.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2FileTypeBox: public CNCSJP2Box {
	public:
			/** Compatibility list entry, 4 chars as UINT32 */
		typedef UINT32 CNCSJP2CLEntry;
			/** Compatibility list */
		typedef std::list<CNCSJP2CLEntry> CNCSJP2CLList;

			/** Box type */
		static UINT32	sm_nTBox;
			/** JP2 Brand */
		static UINT32	sm_JP2Brand;

			/** Brand as read from file */
		UINT32			m_Brand;
			/** Minor version */
		UINT32			m_nMinV;
			/** Compatibility list - other files may be readable as JP2, eg JPX files */
		CNCSJP2CLList	m_CLList;

			/** Default constructor, initialises members */
		CNCSJP2FileTypeBox();
			/** Virtual destructor */
		virtual ~CNCSJP2FileTypeBox();
			/** 
			 * Update the extended length of the box prior to writing
			 */
		virtual void UpdateXLBox(void);

			/** 
			 * Parse the file type box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the file type box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
	};
		/**
		 * CNCSJP2HeaderBox class - the JP2 file header box class.  This box is always present.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2HeaderBox: public CNCSJP2SuperBox {
	public:
			/**
			 * CNCSJP2ImageHeaderBox class - the JP2 file image header box class.  This box is always present.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2ImageHeaderBox: public CNCSJP2Box, public CNCSJPCComponentDepthType {
		public:
				/** Box type */
			static UINT32	sm_nTBox;

				/** Image Width */
			UINT32			m_nWidth;
				/** Image Height */
			UINT32			m_nHeight;
				/** Number of image components, eg RGB = 3 */
			UINT16			m_nComponents;
				/** Compression type, for a JP2 == 7 */
			UINT8			m_nCompressionType;
				/** Actual colorspace in image is known, 0 == known & ColourSpaceBox available, 1 == unkown */
			UINT8			m_nUnknownColorspace;
				/** Intellectual property, 0 = none, 1 - IPRBox present */
			UINT8			m_nIPR;

				/** Default constructor, initialises members */
			CNCSJP2ImageHeaderBox();
				/** Virtual destructor */
			virtual ~CNCSJP2ImageHeaderBox();
				/** 
				 * Update the extended length of the box prior to writing
				 */
			virtual void UpdateXLBox(void);

				/** 
				 * Parse the image header box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
				/** 
				 * UnParse the image header box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		};
			/**
			 * CNCSJP2BitsPerComponentBox class - the JP2 file bits per component box class.
			 * This box is only present if m_nBitsPerComponent in the IMageHeaderBox is 255, indicating
			 * each component may be a different bit depth (eg, 5/6/6 RGB).
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2BitsPerComponentBox: public CNCSJP2Box {
		public:
				/** Box type */
			static UINT32	sm_nTBox;

				/** Bit depth for each component */
			std::vector<CNCSJPCComponentDepthType>	m_Bits;

				/** Default constructor, initialised members */
			CNCSJP2BitsPerComponentBox();
				/** Virtual destructor */
			virtual ~CNCSJP2BitsPerComponentBox();
				/** 
				 * Update the extended length of the box prior to writing
				 */
			virtual void UpdateXLBox(void);

				/** 
				 * Parse the bits per component box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
				/** 
				 * UnParse the bits per component box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		};
			/**
			 * CNCSJP2ColorSpecificationBox class - the JP2 file color specification box class.
			 * This box is only present when m_nUnknownColorspace in the ImageHeaderBox == 0, 
			 * indicating the colorspace of the image is known.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2ColorSpecificationBox: public CNCSJP2Box {
		public:
				/** Box type */
			static UINT32	sm_nTBox;

				/** ColorSpace type, may be predefined or an ICC mapping */
			typedef enum {
					/** Either sRGB or GREYSCALE colorspace */
				ENUMERATED_COLORSPACE		= 1,
					/** Rstricted ICC color profile is present */
				RESTRICTED_ICC_PROFILE		= 2,
					/** Full ICC color profile is present */
				ICC_PROFILE		= 3			
			} Type;
				/** Enumerated colorspace types */
			typedef enum {
					/** sRGB colorspace */
				sRGB						= 16,
					/** GREYSCALE colorspace */
				GREYSCALE					= 17,
					/** YCbCr colorspace */
				YCbCr						= 18,
					/** sYCbCr colorspace */
				sYCC						= 22
			} EnumeratedType;

				/** ColorSpace method, either ENUMERATED or ICC */
			Type		m_eMethod;
				/** Precedence, always = 0 & is ignored */
			UINT8		m_nPrecedence;
				/** Approximation, always = 0 & is ignored */
			UINT8		m_nApproximation;
				/** If m_nMethod == ENUMERATED, this specifies colorspace */
			EnumeratedType		m_eEnumeratedColorspace;
				/** If m_nMethod == ICC, this specifies ICC profile */
			void		*m_pICCProfile;
				/** Length in bytes of the in-memory ICC profile */
			UINT32		m_nICCLen;

				/** Default constructor, initialises members */
			CNCSJP2ColorSpecificationBox();
				/** Cirtual destructor */
			virtual ~CNCSJP2ColorSpecificationBox();
				/** 
				 * Update the extended length of the box prior to writing
				 */
			virtual void UpdateXLBox(void);
				/** 
				 * Parse the color specification box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
				/** 
				 * UnParse the color specification box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		};
			/**
			 * CNCSJP2PaletteBox class - the JP2 file palette box class.
			 * This box is always present with a component mapping box.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2PaletteBox: public CNCSJP2Box {
		public:
			typedef std::vector<void*> PaletteEntry;
				/** Box type */
			static UINT32	sm_nTBox;

				/** Number of entries in the palette, 1-1024 */
			UINT16		m_nEntries;
				/** Number of components generated by palette, eg RGB = 3 */
			UINT8		m_nComponents;
				/** Bit depth of each generated component, 1-38 bits, bit7 = sign */
			std::vector<CNCSJPCComponentDepthType>	m_Bits;
				/** Palette entries */
			std::vector<PaletteEntry> m_Entries;

				/** Default constructor, initialises members */
			CNCSJP2PaletteBox();
				/** Virtual destructor */
			virtual ~CNCSJP2PaletteBox();

				/** 
				 * Parse the palette box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
#ifndef NCSJPC_LEAN_AND_MEAN
				/** 
				 * UnParse the palette box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
#endif //!NCSJPC_LEAN_AND_MEAN
		};
			/**
			 * CNCSJP2ComponentMappingBox class - the JP2 file component mapping box class.
			 * This box is always present with a palette box to define channel usage.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2ComponentMappingBox: public CNCSJP2Box {
		public:
				/** Type of component */
			typedef enum {
					/** This is the direct component value, eg Red */
				DIRECT		= 0,
					/** This component is a palette index */
				PALETTE		= 1
			} Type;

				/** Component mapping information  */
			typedef struct {
					/** 0-based index of the component this mapping is for */
				UINT16	m_iComponent;
					/** Type of component */
				Type	m_nType;
					/** Component "column" index into palette */
				UINT8	m_iPalette;
			} ComponentMapping;

				/** Box type */
			static UINT32	sm_nTBox;

				/** Number of entries in the componentmapping */
			UINT16		m_nEntries;

				/** Component->Palette mappings */
			std::vector<ComponentMapping>	m_Mapping;

				/** Default constructor, initialises members */
			CNCSJP2ComponentMappingBox();
				/** Cirtual destructor */
			virtual ~CNCSJP2ComponentMappingBox();

				/** 
				 * Parse the component mapping box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
#ifndef NCSJPC_LEAN_AND_MEAN
				/** 
				 * UnParse the component mapping box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
#endif //!NCSJPC_LEAN_AND_MEAN
		};
			/**
			 * CNCSJP2ChannelDefinitionBox class - the JP2 file channel definition box class.
			 * This box is optional.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2ChannelDefinitionBox: public CNCSJP2Box {
		public:
				/** Channel type */
			typedef enum {
					/** Color value */
				COLOR			= 0,
					/** Opacity value */
				OPACITY			= 1,
					/** Premultiplied opacity value */
				PREMUL_OPACITY	= 2,
					/** Channel type is unknown */
				UNKNOWN			= 0xffff
			} Type;

				/** Color channel this channel is associated with */
			typedef enum {
					/** The entire image, eg RGBA */
				WHOLE_IMAGE		= 0,
					/** Red channel */
				R				= 1,
					/** Green channel */
				G				= 2,
					/** Blue channel */
				B				= 3,
					/** Luminance/Greyscale channel */
				Y				= 1,
					/** Cb channel */
				Cb				= 2,
					/** Cr channel */
				Cr				= 3,
					/** No channel associated */
				NONE			= 0xffff
			} Association;

				/** A channel definition */
			typedef struct {
					/** 0-based index of channel this definition is for */
				UINT16		m_iChannel;
					/** Channel type */
				Type		m_eType;
					/** Channel color component association */
				Association	m_eAssociation;
			} ChannelDefinition;

				/** Box type */
			static UINT32	sm_nTBox;

				/** Number of definitions */
			UINT16		m_nEntries;

				/** Channel Definitiona*/
			std::vector<ChannelDefinition>	m_Definitions;

				/** Default constructor, initialises members */
			CNCSJP2ChannelDefinitionBox();
				/** Cirtual destructor */
			virtual ~CNCSJP2ChannelDefinitionBox();

				/** 
				 * Get the ChannelDefinition for the specified channel index.
				 * @param		iChannel	Channel index
				 * @return      const ChannelDefinition*	ChannelDefinition, or NULL on failure.
				 */
			virtual const ChannelDefinition *GetDefinitition(UINT16 iChannel);
				/** 
				 * Update the extended length of the box prior to writing
				 */
			virtual void UpdateXLBox(void);
				/** 
				 * Parse the channel definition box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
				/** 
				 * UnParse the channel definition box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		};
			/**
			 * CNCSJP2ResolutionBox class - the JP2 file resolution box class.
			 * This box is optional.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2ResolutionBox: public CNCSJP2SuperBox {
		public:
				/**
				 * CNCSJP2CaptureResolutionBox class - the JP2 file capture resolution box class.
				 * This box is optional.
				 *
				 * Actual resolution is in grid points per meter, calculated as:
				 *
				 * Rc = (RcN / RcD) * 10^RcE
				 * 
				 * @author       Simon Cope
				 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
				 */	
			class NCSJPC_EXPORT_ALL CNCSJP2CaptureResolutionBox: public CNCSJP2Box {
			public:
					/** Box type */
				static UINT32	sm_nTBox;

					/** Vertical grid resolution numerator */
				UINT16		m_nVRcN;
					/** Vertical grid resolution denominator */
				UINT16		m_nVRcD;
					/** Vertical grid resolution exponent, two's compliment */
				INT8		m_nVRcE;
					/** Horizontal grid resolution numerator */
				UINT16		m_nHRcN;
					/** Horizontal grid resolution denominator */
				UINT16		m_nHRcD;
					/** Horizontal grid resolution exponent, two's compliment */
				INT8		m_nHRcE;

				/** Default constructor, initialises members */
				CNCSJP2CaptureResolutionBox();
					/** Cirtual destructor */
				virtual ~CNCSJP2CaptureResolutionBox();


					/** 
					 * Parse the capture resolution box from the JP2 file.
					 * @param		JP2File		JP2 file being parsed
					 * @param		Stream		IOStream to use to parse file.
					 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
					 */
				virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
					/** 
					 * UnParse the capture resolution box to the JP2 file.
					 * @param		JP2File		JP2 file being parsed
					 * @param		Stream		IOStream to use to parse file.
					 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
					 */
				virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			};
				/**
				 * CNCSJP2DefaultDisplayResolutionBox class - the JP2 file default display resolution box class.
				 * This box is optional.
				 *
				 * Actual resolution is in grid points per meter, calculated as:
				 *
				 * Rc = (RcN / RcD) * 10^RcE
				 * 
				 * @author       Simon Cope
				 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
				 */	
			class NCSJPC_EXPORT_ALL CNCSJP2DefaultDisplayResolutionBox: public CNCSJP2CaptureResolutionBox {
			public:
					/** Box type */
				static UINT32	sm_nTBox;

				/** Default constructor, initialises members */
				CNCSJP2DefaultDisplayResolutionBox();
					/** Cirtual destructor */
				virtual ~CNCSJP2DefaultDisplayResolutionBox();
			};
				/** Box Type */
			static UINT32	sm_nTBox;

				/** CaptureResolutionBox within ResolutionBox */
			CNCSJP2CaptureResolutionBox m_Capture;
				/** DefaultDisplayResolutionBox within ResolutionBox */
			CNCSJP2DefaultDisplayResolutionBox m_DefaultDisplay;

				/** Default constructor, initialises members */
			CNCSJP2ResolutionBox();
				/** Virtual destructor */
			virtual ~CNCSJP2ResolutionBox();
		};

			/** Box Type */
		static UINT32	sm_nTBox;

			/** ImageHeaderBox within HeaderBox */
		CNCSJP2ImageHeaderBox m_ImageHeader;
			/** Optional BitsPerComponentBox within HeaderBox */
		CNCSJP2BitsPerComponentBox m_BitsPerComponent;
			/** Optional ColorSpecificationBox within HeaderBox */
		CNCSJP2ColorSpecificationBox m_ColorSpecification; 
			/** Optional PaletteBox within HeaderBox */
		CNCSJP2PaletteBox m_Palette;
			/** OPtional ComponentMappingBox within HeaderBox */
		CNCSJP2ComponentMappingBox m_ComponentMapping;
			/** Optional ChannelDefinitionBox within HeaderBox */
		CNCSJP2ChannelDefinitionBox m_ChannelDefinition;
			/** OPtional ResolutionBox within HeaderBox */
		CNCSJP2ResolutionBox m_Resolution;

			/** Default contructor, initialises members */
		CNCSJP2HeaderBox();
			/** Virtual destructor */
		virtual ~CNCSJP2HeaderBox();
	};
		/**
		 * CNCSJP2ContiguousCodestreamBox class - the JP2 codestream box.  This box is always present.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2ContiguousCodestreamBox: public CNCSJP2Box, public CNCSJPC {
	public:
			/** Box type */ 
		static UINT32	sm_nTBox;

			/** Default constructor, initialises members */
		CNCSJP2ContiguousCodestreamBox();
			/** Virtual destructor */
		virtual ~CNCSJP2ContiguousCodestreamBox();

			/** 
			 * Parse the JPC box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the JPC box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
	};
		/**
		 * CNCSJP2IntellectualPropertyBox class - the JP2 IPR box.  This box is optional.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2IntellectualPropertyBox: public CNCSJP2Box {
	public:
			/** Box type */ 
		static UINT32	sm_nTBox;

			/** Default constructor, initialises members */
		CNCSJP2IntellectualPropertyBox();
			/** Virtual destructor */
		virtual ~CNCSJP2IntellectualPropertyBox();
			/** 
			 * Parse the JPC box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
	};
		/**
		 * CNCSJP2XMLBox class - the JP2 XML box.  This box is optional, and there may be more than one.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2XMLBox: public CNCSJP2Box {
	public:
			/** Box type */ 
		static UINT32	sm_nTBox;

			/** Default constructor, initialises members */
		CNCSJP2XMLBox();
			/** Virtual destructor */
		virtual ~CNCSJP2XMLBox();
			/** 
			 * Parse the XML box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the XML box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
	};
		/**
		 * CNCSJP2UUIDBox class - the JP2 UUID box.  This box is optional, and there may be more than one.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2UUIDBox: public CNCSJP2Box {
	public:
			/** Box type */ 
		static UINT32	sm_nTBox;
			/** UUID */
		NCSUUID	m_UUID;
			/** UUID Box data */
		void *m_pData;
			/** UUID Box data Length*/
		INT32 m_nLength;

			/** Default constructor, initialises members */
		CNCSJP2UUIDBox();
			/** Virtual destructor */
		virtual ~CNCSJP2UUIDBox();

			/** 
			 * Parse the UUID box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the UUID box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
	};
		/**
		 * CNCSJP2UUIDInfoBox class - the JP2 UUIDInfo box.  This box is optional, and there may be more than one.
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2UUIDInfoBox: public CNCSJP2SuperBox {
	public:
			/**
			 * CNCSJP2UUIDListBox class - the JP2 file UUIDList box class.
			 * This box is required within a UUIDInfo box.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2UUIDListBox: public CNCSJP2Box {
		public:
			
				/** Box type */
			static UINT32	sm_nTBox;
				/** Number of entries */
			UINT16		m_nEntries;
				/** UUIDs */
			std::vector<NCSUUID> m_UUIDs;
			
				/** Default constructor, initialises members */
			CNCSJP2UUIDListBox();
				/** Virtual destructor */
			virtual ~CNCSJP2UUIDListBox();

				/** 
				 * Parse the UUID List box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
				/**
				 * UnParse the UUID list box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		};
			/**
			 * CNCSJP2DataEntryURLBox class - the JP2 file DateEntryURL box class.
			 * This box is required within a UUIDInfo box.
			 * 
			 * @author       Simon Cope
			 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
			 */	
		class NCSJPC_EXPORT_ALL CNCSJP2DataEntryURLBox: public CNCSJP2Box {
		public:
			
				/** Box type */
			static UINT32	sm_nTBox;

				/** Version */
			UINT8		m_nVersion;
				/** Flags */
			UINT8		m_Flags[3];
				/** Location URL */
			UINT8		*m_pLocation;

				/** Default constructor, initialises members */
			CNCSJP2DataEntryURLBox();
				/** Cirtual destructor */
			virtual ~CNCSJP2DataEntryURLBox();

				/** 
				 * Parse the Data Entry URL box from the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
				/**
				 * UnParse the Data Entry URL box to the JP2 file.
				 * @param		JP2File		JP2 file being parsed
				 * @param		Stream		IOStream to use to parse file.
				 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
				 */
			virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
		};
			/** Box Type */
		static UINT32	sm_nTBox;
				/** UUIDListBox within UUIDInfo */
		CNCSJP2UUIDListBox m_List;
				/** DataEntryURLBox within the UUIDInfo */
		CNCSJP2DataEntryURLBox m_DE;

				/** Default constructor, initialises members */
		CNCSJP2UUIDInfoBox();
			/** Virtual destructor */
		virtual ~CNCSJP2UUIDInfoBox();
	};
		/**
		 * CNCSJP2PCSBox class - the JP2 PCS GeoKey box.  This box is optional
		 * This implements a quick and dirty GeoTIFF tag parser rather than rely on a 3rd party 
		 * TIFF implementation such as TIFFlib.  Alternatively, an application can read the box contents 
		 * directly to implement more reobust/thorough parsing of the PCS tag values.
		 * 
		 * @author       Simon Cope & Tom Lynch
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2PCSBox: public CNCSJP2UUIDBox {
	public:
			/** Relevent TIFF tag names */
		typedef enum {
				/** Model Pixel Scale */
			ModelPixelScale		= 0x830e,
				/** Model Tie Point */
			ModelTiePoint		= 0x8482,
				/** Model Transformantion */
			ModelTransformation = 0x85d8,
				/** GetKey Directory */
			GeoKeyDirectory		= 0x87af,
				/** GeoKey Double Parameters */
			GeoDoubleParams		= 0x87b0,
				/** GetKey ASCII Parameters */
			GeoASCIIParams		= 0x87b1,
		} TIFFTagNames;

			/** TIFF Tag structure */
		typedef struct {
				/** Tag value */
			UINT16 Tag;
				/** Tag type */
			UINT16 Type;
				/** Number of values */
			UINT32 Count;
				/** IFD offset to values */
			UINT32 Offset;
				/** Vector of the parsed double values */
			std::vector<double>	Values;
		} TIFFTag;

			/** GeoKey value union */
		typedef union {
				/** An integer value - u16 */
			UINT16		nValue;
				/** A double value */
			double		dValue;
				/** A string value */
			char		*pValue;
		} GeoKeyValue;

			/** GetKey type enum = u16, double, ASCII */
		typedef enum {
				/** An integer value */
			GKT_SHORT	= 0,
				/** A double value */
			GKT_DOUBLE	= GeoDoubleParams,
				/** A string value */
			GKT_ASCII   = GeoASCIIParams
		} GeoKeyType;

			/** An GeoKey value */
		typedef struct {
				/** The Key ID */
			UINT16 KeyID;
				/** The Key Type */
			GeoKeyType Type;
				/** Number of values */
			UINT16 Count;

				/** The value */
			GeoKeyValue v;
		} GeoKey;

			/** UUID for the PCS box */
		static NCSUUID sm_UUID;

			/** Default constructor, initialises members */
		CNCSJP2PCSBox();
			/** Virtual destructor */
		virtual ~CNCSJP2PCSBox();

			/** Update the extended length for the box before writing */
		virtual void UpdateXLBox(void);
			/** 
			 * Parse the PCS GeoKey box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the PCS GeoKey box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * get an NCSFileViewFileInfoEx structure equivalent of the parsed PCS box.
			 * @param		FileInfo	Info struct to fill in
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		CNCSError GetFileInfo(NCSFileViewFileInfoEx &FileInfo);
			/** 
			 * Setup the PCS box from an NCSFileViewFileInfoEx structure prior to writing out
			 * @param		FileInfo	Info struct to create PCS box from
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		CNCSError SetFileInfo(NCSFileViewFileInfoEx &FileInfo);
			/**
			 * Set the precision used in processing registration by the PCS box.
			 * @param		fPrecision	The value below which to discard registration values
			 */
		static void SetPrecision(IEEE4 fPrecision);
	private:
			/** 
			 * get a tag from it's name 
			 * @param		eTag		Tag name to get tag for
			 * @return      TIFFTag*	TIFF Tag, or NULL on failure.
			 */
		TIFFTag *GetTag(TIFFTagNames eTag);
			/** 
			 * get a GetKey from it's key name
			 * @param		Key			GetKey name to get
			 * @return      GeoKey*		GeoKey, or NULL on failure.
			 */
		GeoKey *GetKey(UINT16 Key);
			/** 
			 * Process the GeoTIFF Info 
			 * @return      CNCSError	NCS_SUCCESS, or error on failure
			 */
		CNCSError ProcessGeoTIFFInfo();
			/** 
			 * Format the GetTIFF Info
			 * @param		pBuf		Info buffer
			 * @return      CNCSError	NCS_SUCCESS, or error on failure\
			 */
		CNCSError FormatGeoTIFFInfo(UINT8 *pBuf, char *szFilename);

			/** PCSFileInfo */
		NCSFileViewFileInfoEx m_PCSFileInfo;
			/** Vector of the TIFF Tags */
		std::vector<TIFFTag> m_TIFFTags;
			/** Vector of the GeoKeys */
		std::vector<GeoKey> m_GeoKeys;
			/** Actual GeoTIFF box length - FIXME is this required? */
		UINT32 m_nGeoTIFFBoxLength;
			/** Geodata precision */
		static IEEE4 sm_fGeodataPrecisionEpsilon;
	};
		/**
		 * CNCSJP2GMLGeoLocationBox class - the JP2 GML box.  This box is optional
		 * 
		 * @author       Simon Cope
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2GMLGeoLocationBox: public CNCSJP2XMLBox {
	public:
			/** Default constructor, initialises members */
		CNCSJP2GMLGeoLocationBox();
			/** Virtual destructor */
		virtual ~CNCSJP2GMLGeoLocationBox();

			/** Update the extended length of the box prior to writing */
		virtual void UpdateXLBox(void);
			/** 
			 * Parse the GML GeoLocation box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the GML GeoLocation box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);

			/** 
			 * get an NCSFileViewFileInfoEx structure equivalent of the parsed GML box.
			 * @param		FileInfo	Info struct to fill in
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		CNCSError GetFileInfo(NCSFileViewFileInfoEx &FileInfo);
			/** 
			 * Setup the GML box from an NCSFileViewFileInfoEx structure prior to writing out
			 * @param		FileInfo	Info struct to create PCS box from
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		CNCSError SetFileInfo(NCSFileViewFileInfoEx &FileInfo);
	private:
			/** GML FileInfo structure */
		NCSFileViewFileInfoEx	m_GMLFileInfo;
			/** 
			 * Setup the GML box prior to writing out
			 * @param		pBuf		Buffer to format XMl into 
			 * @param		nBufLen		Buffer length
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */			
		CNCSError FormatXML(char *pBuf, UINT32 nBufLen);
	};
		/**
		 * CNCSJP2WorldBox class - the JP2 World file box.  This box is optional
		 * 
		 * @author       Tom Lynch
		 * @version      $Revision: 1.67 $ $Author: PCDEV\tom.lynch $ $Date: 2005/12/22 05:50:11 $ 
		 */	
	class NCSJPC_EXPORT_ALL CNCSJP2WorldBox: public CNCSJP2UUIDBox {
	public:
			/** UUID for the World box */
		static NCSUUID sm_UUID;
			/** Default constructor, initialises members */
		CNCSJP2WorldBox();
			/** Virtual destructor */
		virtual ~CNCSJP2WorldBox();
			/** Update the extended length for the box before writing */
		virtual void UpdateXLBox(void);
			/** 
			 * Parse the World box from the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError Parse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * UnParse the World box to the JP2 file.
			 * @param		JP2File		JP2 file being parsed
			 * @param		Stream		IOStream to use to parse file.
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		virtual CNCSError UnParse(class CNCSJP2File &JP2File, CNCSJPCIOStream &Stream);
			/** 
			 * get an NCSFileViewFileInfoEx structure equivalent of the parsed World box.
			 * @param		FileInfo	Info struct to fill in
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		CNCSError GetFileInfo(NCSFileViewFileInfoEx &FileInfo);
			/** 
			 * Setup the World box from an NCSFileViewFileInfoEx structure prior to writing out
			 * @param		FileInfo	Info struct to create World box from
			 * @return      CNCSError	NCS_SUCCESS, or Error code on failure.
			 */
		CNCSError SetFileInfo(NCSFileViewFileInfoEx &FileInfo);
			/**
			 * Set the precision used in processing registration by the World box.
			 * @param		fPrecision	The value below which to discard registration values
			 */
		static void SetPrecision(IEEE4 fPrecision);
	private:
		CNCSError ProcessWorldInfo();
			/** 
			 * Format the GetTIFF Info
			 * @param		pBuf		Info buffer
			 * @return      CNCSError	NCS_SUCCESS, or error on failure\
			 */
		CNCSError FormatWorldInfo(UINT8 *pBuf);

			/** WorldFileInfo */
		NCSFileViewFileInfoEx m_WorldFileInfo;
			/** Accumulated length of box */
		UINT32 m_nWorldBoxLength;
			/** World chunk "feature flags" */
		UINT8 m_pFeatureFlags[8];
			/** Geodata precision */
		static IEEE4 sm_fGeodataPrecisionEpsilon;
	};
		/** ECW FileInfo Structure */
	NCSFileViewFileInfoEx m_FileInfo;
		/** Number of refs to this file */
	INT32				m_nRefs;

		/** SignatureBox within JP2File */
	CNCSJP2SignatureBox		m_Signature;
		/** FileTypeBox within JP2File */
	CNCSJP2FileTypeBox		m_FileType;
		/** HeaderBox within JP2File */
	CNCSJP2HeaderBox		m_Header;
		/** ContiguousCodestreamBox ie "JPC" within JP2File */
	CNCSJP2ContiguousCodestreamBox	m_Codestream;
		/** IntellectualPropertyBox within JP2File */
	CNCSJP2IntellectualPropertyBox m_IPR;
		/** GML geolocation box */
	CNCSJP2GMLGeoLocationBox m_GML;
		/** PCS Geokey box */
	CNCSJP2PCSBox m_PCS;

		/** Compression IEEE4 lossless via MES decomposition */
//	bool				m_bIEEE4_MES;
		/** Compression IEEE8 lossless via MES decomposition */
//	bool				m_bIEEE8_MES;

		/** IOStream */
	CNCSJPCIOStream *m_pStream;
		/** Did we allocate the stream? */
	bool	m_bOurStream;

		/** Component index for each output channel - 1:1 unless a palette is present */
	std::vector<UINT16>	m_ChannelComponentIndex;

		/** Default constructor, initialises members */
	CNCSJP2File();
		/** Virtual Destructor */
	virtual ~CNCSJP2File();

#ifdef NCS_BUILD_UNICODE
		/** 
		 * Open the JP2 file for Parsing/UnParsing.
		 * @param		pFileName	Full path name of JP2 file to open
		 * @param		bWrite		If true, opens for writing.
		 * @return      CNCSError	NCS_SUCCESS or error code on failuer.
		 */
	virtual CNCSError Open(wchar_t *pFileName, bool bWrite = false);
#endif
		/** 
		 * Open the JP2 file for Parsing/UnParsing.
		 * @param		pFileName	Full path name of JP2 file to open
		 * @param		bWrite		If true, opens for writing.
		 * @return      CNCSError	NCS_SUCCESS or error code on failuer.
		 */
	virtual CNCSError Open(char *pFileName, bool bWrite = false);
		/** 
		 * Open the JP2 file from the specified stream.
		 * @param		Stream		Stream of JP2 file to open
		 * @return      CNCSError	NCS_SUCCESS or error code on failuer.
		 */
	virtual CNCSError Open(CNCSJPCIOStream &Stream);
		/** 
		 * Close the JP2 file.
		 * @param		bFreeCache	Free the cache & delete file object if last close
		 * @return      CNCSError	NCS_SUCCESS or error code on failuer.
		 */
	virtual CNCSError Close(bool bFreeCache = false);

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
		 * Get the memory image of the file.
		 * @param		pLength		Returned length of image
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
		 * @param		pLength		Returned length of packet (header+data)
		 * @return      void *		NCSMalloc()'d pointer to read packet (header+data).
		 */
	void *GetPacket(NCSBlockId nPacket, UINT32 &nLength);
		/** 
		 * Get number of a packet in the file.
		 * @return      UINT32		Number of Packets or 0 on failure.
		 */
	UINT32 GetNrPackets();

		/** 
		 * Get a pointer to a UUID box bsed on the specified UUID.  Iterate via the pLast paramter.
		 * @param		uuid		UUID to find UUID box for
		 * @param		pLast		Pointer to last returned UUID box (or NULL on first call), used to iterate through multiple UUID boxes with the same UUID
		 * @return      CNCSJP2UUIDBox*		Pointer to UUID Box.
		 */
	CNCSJP2UUIDBox *GetUUIDBox(NCSUUID uuid, CNCSJP2UUIDBox *pLast = NULL);
		/** 
		 * Get a pointer to an XML box.  Iterate via the pLast paramter.
		 * @param		pLast		Pointer to last returned XML box (or NULL on first call), used to iterate through multiple XML boxes
		 * @return      CNCSJP2XMLBox*		Pointer to XML Box.
		 */
	CNCSJP2XMLBox *GetXMLBox(CNCSJP2XMLBox *pLast = NULL);

		/** 
		 * Set the compression parameters (dimensions, registration, bitdepth etc).
		 *precinct sizes 
		 * @param		Info		FileInfo structure
		 * @return      CNCSError	NCS_SUCCESS, or error on failure 
		 */
	CNCSError SetCompress(NCSFileViewFileInfoEx &Info);
		/** 
		 * Write the next line in BIL format into the file.
		 * @param		nCtx				Unique Context ID
		 * @param		pSrc				Source buffer
		 * @param		iComponent			Component index
		 * @return      bool				true on success, else false.
		 */
	bool WriteLine(ContextID nCtx, CNCSJPCBuffer *pSrc, UINT16 iComponent);
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

		/** 
		 * Enable unlimited compression - NOTE YOU MUST COMPLY WITH THE EULA.
		 */
	static void SetKeySize(void);
		/** 
		 * Purge any unused buffers, codeblocks etc if memory useage exceeds acceptable limites
		 */
	static void Purge();
private:
	friend class CNCSJP2FileView;
	friend class CNCSJPCGlobalLock;

		/** Static list of files */
	static CNCSJP2FileVector sm_Files;
		/** 
		 * Close the JP2 file
		 * @param		bFreeCache		Free the cached JP2 file data.
		 * @param		bFromDestructor	Being called from destructor.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	virtual CNCSError Close ( bool bFreeCache, bool bFromDestructor);
	//[03]
		/**
		 * Shutdown the resources associated with CNCSJP2File objects.
		 * Called only by CNCSJP2FileView::Shutdown().
		 */
	static void Shutdown();

#ifdef NCS_BUILD_UNICODE
		/** 
		 * Static Open method - returns existing instance if already open
		 * @param		ppFile			Returned CNCSJP2File instance pointer.
		 * @param		pURLPath		Local/remote path/URL.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	static CNCSError sOpen(CNCSJP2File **ppFile, wchar_t *pURLPath);
#else
		/** 
		 * Static Open method - returns existing instance if already open
		 * @param		ppFile			Returned CNCSJP2File instance pointer.
		 * @param		pURLPath		Local/remote path/URL.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	static CNCSError sOpen(CNCSJP2File **ppFile, char *pURLPath);
#endif
		/** 
		 * Static Open method - returns existing instance if already open
		 * @param		ppFile			Returned CNCSJP2File instance pointer.
		 * @param		pStream			Stream to open file from.
		 * @return      CNCSError		NCS_SUCCESS or error code on failure.
		 */
	static CNCSError sOpen(CNCSJP2File **ppFile, CNCSJPCIOStream *pStream);

		/** 
		 * Add a JP2 file box to a memory image
		 * @param		pImage			Existing memory image to append box to.
		 * @param		pLength			Current/returned length of memory image.
		 * @param		pBox			JP2 Box to append to memory image.
		 * @return      void*			Memory image with appended box, else NULL on error.
		 */
	void *GetMemImage_AddBox(void *pImage, UINT32 *pLength, CNCSJP2Box *pBox);
		/** 
		 * Add a JPC codestream marker to a memory image
		 * @param		pImage			Existing memory image to append box to.
		 * @param		pLength			Current/returned length of memory image.
		 * @param		pMarker			JPC codestream marker to append to memory image.
		 * @return      void*			Memory image with appended box, else NULL on error.
		 */
	void *GetMemImage_AddMarker(void *pImage, UINT32 *pLength, CNCSJPCMarker *pMarker);
		/** 
		 * Add the JPC codestream to a memory image (excluding packets)
		 * @param		pImage			Existing memory image to append box to.
		 * @param		pLength			Current/returned length of memory image.
		 * @return      void*			Memory image with appended box, else NULL on error.
		 */
	void *GetMemImage_AddCodestream(void *pImage, UINT32 *pLength);
		/** 
		 * Is the JP2 SDK currently over it's memory budget
		 * @return      bool			true if over budget, else false 
		 */
	static bool OverBudget();

		/** Next line to read/write */
	INT32 m_nNextLine;

		/** Compression profile on writing */
	CNCSJPC::Profile	m_eCompressionProfile;
		/** Compression resolution levels on writing */
	UINT8				m_nCompressionLevels;
		/** Compression quality layers on writing */
	UINT16				m_nCompressionLayers;
		/** Compression precinct width on writing */
	UINT32				m_nCompressionPrecinctWidth;
		/** Compression precinct height on writing */
	UINT32				m_nCompressionPrecinctHeight;
		/** Compression tile width on writing */
	UINT32				m_nCompressionTileWidth;
		/** Compression tile height on writing */
	UINT32				m_nCompressionTileHeight;
		/** Compression output SOP on writing */
	bool				m_bCompressionIncludeSOP;
		/** Compression output EPH on writing */
	bool				m_bCompressionIncludeEPH;
		/** Compression output JPC only on writing (no JP2 wrapper output) */
	bool				m_bCodestreamOnly;
		/** Compression progression order on writing (LRCP, RLCP, RPCL only) */
	CNCSJPCProgressionOrderType::Type	m_eCompressionOrder;
		/** # of layers to decompress on reading - FIXME unimplemented */
	UINT16				m_nDecompressionLayers;
		/** GeoData usage priority */
	static UINT32		sm_nGeodataUsage;
		/** Unlimited compression key - see SetKeySize() */
	static UINT32		sm_nKeySize;
};

	/** 
	 * Initialiase a FileInfo structure
	 * @param		pDst			File info to initialise.
	 */
extern "C" void NCSInitFileInfoEx(NCSFileViewFileInfoEx *pDst);
	/** 
	 * Free a FileInfo structure
	 * @param		pDst			File info to free.
	 */
extern "C" void NCSFreeFileInfoEx(NCSFileViewFileInfoEx *pDst);
	/** 
	 * Copy a FileInfo structure
	 * @param		pDst			File info to copy to.
	 * @param		pSrc			File info to copy from.
	 */
extern "C" void NCSCopyFileInfoEx(NCSFileViewFileInfoEx *pDst, NCSFileViewFileInfoEx *pSrc);
	/** 
	 * check if a FileInfo structure is georeferenced
	 * @param		pInfo			File info to test.
	 * @return		BOOLEAN			TRUE if FileInfo is georeferences (Non RAW/RAW).
	 */
extern "C" BOOLEAN NCSIsFileInfoExGeoreferenced(NCSFileViewFileInfoEx *pInfo);
	/** 
	 * Standardize a FileInfo structure
	 * @param		pInfo			File info to standardize.
	 */
extern "C" void	NCSStandardizeFileInfoEx(NCSFileViewFileInfoEx *pInfo);

#endif // !NCSJP2FILE_H
