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
** FILE:   	NCSOutputFile.cpp
** CREATED:	12 Jan 2000
** AUTHOR: 	Mark Sheridan
** PURPOSE:	Output routines for JPEG for NCSRenderer class
** EDITS:
** [01] sjc 04Oct00 Set ptr to NULL after delete
** [02] sjc 04Oct00 Fixed typo in error message
** [03] sjc 04Oct00 Fixed typo - DlOpen() was outside if(!) test
** [04]  ny 12Mar01 jpeglib.h needs stdio.h
** [05]  ny 20Apr01 Buffer size did not take into account number of bands (sjc)
*******************************************************/


#ifdef WIN32
	#define _WIN32_WINNT 0x0500	// For MB_SERVICE_NOTIFICATION
	#define XMD_H
	#include <stdio.h> /**[04]**/
	#include "../../libjpeg/jpeglib.h"
#elif defined( macintosh )
	#include <stdio.h>
	extern "C" {
		#include "jpeglib.h"
	}
	#include <string.h>
#elif defined(_WIN32_WCE)
	#define _WIN32_WINNT 0x0500	// For MB_SERVICE_NOTIFICATION
	#define XMD_H
	#include <stdio.h> /**[04]**/
	#include "../../libjpeg/jpeglib.h"
#endif

#include <setjmp.h>

#include "NCSUtil.h"
#include "NCSRenderer.h"

//#ifdef WIN32
//#include "WINWRAP.H"
//#endif

#ifdef MACINTOSH
	#undef jpeg_create_compress
	void jpeg_create_compress(jpeg_compress_struct *cinfo);

	void jpeg_create_compress(jpeg_compress_struct *cinfo) {
		jpeg_CreateCompress((cinfo), JPEG_LIB_VERSION, (size_t) sizeof(struct jpeg_compress_struct));
	}
#endif

typedef struct NCSJpegFuncTable {
	void (*CreateCompressFn)(jpeg_compress_struct *pJpegCompressObject);
	void (*StartCompressFn)(jpeg_compress_struct *pJpegCompressObject, boolean bWriteAllTables);
	void (*FinishCompressFn)(jpeg_compress_struct *pJpegCompressObject);
	void (*DestroyCompressFn)(jpeg_compress_struct *pJpegCompressObject);
	JDIMENSION (*WriteScanlinesFn)(jpeg_compress_struct *pJpegCompressObject, JSAMPARRAY DataArray, JDIMENSION nNrLines);
	void (*StdIODestFn)(jpeg_compress_struct *pJpegCompressObject, FILE *pOutputFile);
	void (*AbortCompressFn)(jpeg_compress_struct *pJpegCompressObject);
	jpeg_error_mgr * (*StdErrorFn)(jpeg_error_mgr * err);
	void (*SetQualityFn)(jpeg_compress_struct *pJpegCompressObject, int nQuality, boolean nForceBaseline);
	void (*SetDefaultsFn)(jpeg_compress_struct *pJpegCompressObject);
} NCSJpegFuncTable;

struct NCSJpegErrorMgr {
    struct jpeg_error_mgr pub;  /* "public" fields      */
    jmp_buf setjmp_buffer;      /* for return to caller */
};

typedef struct NCSJpegErrorMgr *NCSJpegErrorPtr;

METHODDEF(void)
NCSErrorAbort (j_common_ptr cinfo)
{
    char buffer[1024];
    NCSJpegErrorPtr myerr = (NCSJpegErrorPtr) cinfo->err;
    (*cinfo->err->format_message) (cinfo, buffer);
    return;
}

typedef class NCSJpeg {

	public:
		NCSJpegFuncTable *m_pLibTable;
		NCS_DLHANDLE m_hLibHandle;
		NCSJpeg()
		{
			m_pLibTable= (NCSJpegFuncTable *)NULL;
			m_hLibHandle = NULL;
			Init();
		}
		virtual ~NCSJpeg()
		{
			Fini();
		}
		void Init(void);
		void Fini(void)
		{
			if (m_hLibHandle) {
				NCSDlClose(m_hLibHandle);
				NCSFree(m_pLibTable);
			}
		}
} NCSJpeg;

void NCSJpeg::Init(void)
{
#ifdef WIN32
	if (!m_hLibHandle) {
		char FileName[1024] = { '\0' };
		char *pLibPath = NULL;
		INT32 i=0;
#ifdef _DEBUG
		char *pLibraryName = "libjpegd.dll";
#else
		char *pLibraryName = "libjpeg.dll";
#endif
		
		// Look in the default directories first (system32, cwd etc).
		m_hLibHandle = NCSDlOpen(pLibraryName);

		if (!m_hLibHandle) {
			// Find the GDT lib. It should be in the client base dir from the registry.
			if (NCSPrefGetString("ClientBaseDir", &pLibPath) == NCS_SUCCESS) {
				sprintf(FileName, "%s\\%s\0", pLibPath, pLibraryName);
				NCSFree(pLibPath);
			} // if that failed try current user (Win2000)
			else if (NCSPrefGetUserString("ClientBaseDir", &pLibPath) == NCS_SUCCESS) {
				sprintf(FileName, "%s\\%s\0", pLibPath, pLibraryName);
				NCSFree(pLibPath);
			}

			m_hLibHandle = NCSDlOpen(FileName);		//[03]
		}


		// Last ditch effort, this is mostly for development environ, pick the ecw module and find the dir it is in and use that.
		// Could also use NCSView.dll, NP_NCS.dll, NCSUtil.dll, NCSCnet.dll
		if (!m_hLibHandle) {
#ifdef _DEBUG
			HMODULE hModule = GetModuleHandle( NCS_T("NCSEcwd.dll") );
#else
			HMODULE hModule = GetModuleHandle( NCS_T("NCSEcw.dll") );
#endif
			// Check if the jpeg library is in the same dir as the view manager module
			if (hModule) {
				DWORD nResult = GetModuleFileName(hModule,  OS_STRING(FileName), 1024 );
				if (nResult) {
					for (i=nResult; i>0; i--) {
						if (FileName[i] == '\\') {
							i++;
							break;
						}
					}
				}
				if (i) {
					sprintf(&FileName[i], "%s\0", pLibraryName);
				}
				else {
					FileName[0] = '\0';
				}
			}
			m_hLibHandle = NCSDlOpen(FileName);
		}

		// Put any more libjpeg search scenarios here.

		if (m_hLibHandle) {
			
			NCSJpegFuncTable *pTable = NULL;
			pTable = (NCSJpegFuncTable *)NCSMalloc(sizeof(NCSJpegFuncTable), TRUE);

			*(NCS_FUNCADDR*)&(pTable->CreateCompressFn) = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_CreateCompress");
			*(NCS_FUNCADDR*)&(pTable->StartCompressFn)  = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_start_compress");
			*(NCS_FUNCADDR*)&(pTable->FinishCompressFn) = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_finish_compress");
			*(NCS_FUNCADDR*)&(pTable->DestroyCompressFn)= NCSDlGetFuncAddress(m_hLibHandle, "jpeg_destroy_compress");
			*(NCS_FUNCADDR*)&(pTable->WriteScanlinesFn) = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_write_scanlines");
			*(NCS_FUNCADDR*)&(pTable->StdIODestFn)      = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_stdio_dest");
			*(NCS_FUNCADDR*)&(pTable->StdErrorFn)       = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_std_error");
			*(NCS_FUNCADDR*)&(pTable->AbortCompressFn)  = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_abort_compress");
			*(NCS_FUNCADDR*)&(pTable->SetQualityFn)     = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_set_quality");
			*(NCS_FUNCADDR*)&(pTable->SetDefaultsFn)    = NCSDlGetFuncAddress(m_hLibHandle, "jpeg_set_defaults");
			
			// Quick check, if the library was there, all the functions should load.
			if (!pTable->CreateCompressFn || !pTable->StartCompressFn   ||
				!pTable->FinishCompressFn || !pTable->DestroyCompressFn ||
				!pTable->WriteScanlinesFn || !pTable->StdIODestFn       ||
				!pTable->AbortCompressFn  || !pTable->SetQualityFn      ||
				!pTable->SetDefaultsFn) {
#ifdef DEBUG
//				if (!FIsService())
#if !defined(_WIN32_WCE)
					::MessageBox(NULL, "Some or all of the JPEG image library functions could not be loaded, so image operations may not be available. Please check your version of libjpeg.dll (6b)", "NCS Renderer", MB_OK|MB_SERVICE_NOTIFICATION);
#else
					::MessageBox(NULL, NCS_T("Some or all of the JPEG image library functions could not be loaded, so image operations may not be available. Please check your version of libjpeg.dll (6b)"), NCS_T("NCS Renderer"), MB_OK);
#endif
#endif
			}
			m_pLibTable = pTable;
		}
		else {
			m_pLibTable = NULL;
		}
	}
#endif //WIN32

#ifdef MACINTOSH
		NCSJpegFuncTable *pTable = NULL;
		pTable = (NCSJpegFuncTable *)NCSMalloc(sizeof(NCSJpegFuncTable), TRUE);

/*#define pNCSJpegOutput->m_pLibTable->StdErrorFn jpeg_std_error
#define pNCSJpegOutput->m_pLibTable->CreateCompressFn jpeg_CreateCompress
#define pNCSJpegOutput->m_pLibTable->StartCompressFn jpeg_start_compress
#define pNCSJpegOutput->m_pLibTable->FinishCompressFn jpeg_finish_compress
#define pNCSJpegOutput->m_pLibTable->DestroyCompressFn jpeg_destroy_compress
#define pNCSJpegOutput->m_pLibTable->WriteScanlinesFn jpeg_write_scanlines
#define pNCSJpegOutput->m_pLibTable->StdIODestFn jpeg_stdio_dest
#define pNCSJpegOutput->m_pLibTable->StdErrorFn) jpeg_std_error
#define pNCSJpegOutput->m_pLibTable->AbortCompressFn jpeg_abort_compress
#define pNCSJpegOutput->m_pLibTable->SetQualityFn jpeg_set_quality
#define pNCSJpegOutput->m_pLibTable->SetDefaultsFn jpeg_set_defaults*/

		(pTable->CreateCompressFn) = &jpeg_create_compress;
		(pTable->StartCompressFn)  = &jpeg_start_compress;
		(pTable->FinishCompressFn) = &jpeg_finish_compress;
		(pTable->DestroyCompressFn)= &jpeg_destroy_compress;
		(pTable->WriteScanlinesFn) = &jpeg_write_scanlines;
		(pTable->StdIODestFn)      = &jpeg_stdio_dest;
		(pTable->StdErrorFn)       = &jpeg_std_error;
		(pTable->AbortCompressFn)  = &jpeg_abort_compress;
		(pTable->SetQualityFn)     = &jpeg_set_quality;
		(pTable->SetDefaultsFn)    = &jpeg_set_defaults;
		
		// Quick check, if the library was there, all the functions should load.
		if (!pTable->CreateCompressFn || !pTable->StartCompressFn   ||
			!pTable->FinishCompressFn || !pTable->DestroyCompressFn ||
			!pTable->WriteScanlinesFn || !pTable->StdIODestFn       ||
			!pTable->AbortCompressFn  || !pTable->SetQualityFn      ||
			!pTable->SetDefaultsFn) {
		}
		m_pLibTable = pTable;

#endif //MACINTOSH

}

static NCSJpeg *pNCSJpegOutput = NULL;
static void init_destination (j_compress_ptr cinfo) {}
static boolean empty_output_buffer(j_compress_ptr cinfo) { return TRUE; }
static void term_destination(j_compress_ptr cinfo) {}

void CNCSRenderer::FreeJPEGBuffer(UINT8 *pBuffer) {
	if (pBuffer)
		delete pBuffer;
}

NCSError CNCSRenderer::WriteJPEG(char *pFilename, UINT8 **ppBuffer, UINT32 *pBufferLength, INT32 nQuality)
{
	jpeg_compress_struct JpegFileObject;
	struct jpeg_error_mgr jerr;
	FILE *pOutputFile = NULL;
	JSAMPROW row_pointer[1];
	int row_stride;		
	UINT8 *pBuffer = NULL;
	UINT8 *pGreyBuffer = NULL;
	NCSEcwReadStatus nStatus = NCSECW_READ_OK;
	UINT8 *pJpegBuffer = (UINT8*)NULL;
	UINT32 nJpegBufferLength = 0;

	//struct NCSJpegErrorMgr jerr;

	if (!pNCSJpegOutput) {
		pNCSJpegOutput = new NCSJpeg;
		if (!pNCSJpegOutput) {
			NCSFormatErrorText(NCS_UNKNOWN_ERROR, "Could not initialize JPEG engine. (Not enough memory?)");
			return NCS_UNKNOWN_ERROR;
		}
		else {
			pNCSJpegOutput->Init();
		}
	}
	
	if (pNCSJpegOutput->m_pLibTable) {

		if (m_bIsProgressive) {
			NCSFormatErrorText(NCS_ECW_ERROR,  ": ECW images must be opened in non progressive mode to write a JPEG.");
			return NCS_ECW_ERROR;
		}

		//Create a compress structure from the view, mutex the view so it doesn't change.
		if(pFilename) {
			if ((pOutputFile = fopen(pFilename, "wb")) == NULL) {
				NCSFormatErrorText(NCS_FILE_OPEN_FAILED, pFilename, "");
				return NCS_FILE_OPEN_FAILED;
			}
		} else if(!ppBuffer || !pBufferLength) {
			return NCS_INVALID_ARGUMENTS;
		}

		JpegFileObject.err = (pNCSJpegOutput->m_pLibTable->StdErrorFn)(&jerr);
		jerr.error_exit = NCSErrorAbort;
		(pNCSJpegOutput->m_pLibTable->CreateCompressFn)(&JpegFileObject);

		JpegFileObject.image_width = m_nSetViewWidth;
		JpegFileObject.image_height = m_nSetViewHeight;

		if(pOutputFile) {
			(pNCSJpegOutput->m_pLibTable->StdIODestFn)(&JpegFileObject, pOutputFile);
		} else {
			nJpegBufferLength = m_nSetViewNrBands * 2 * JpegFileObject.image_width * JpegFileObject.image_height * sizeof(JOCTET) + 1024; /**[05]**/
			pJpegBuffer = new UINT8[nJpegBufferLength];//(UINT8*)NCSMalloc(nJpegBufferLength, TRUE);
			memset(pJpegBuffer, 0, nJpegBufferLength);

			JpegFileObject.dest = (struct jpeg_destination_mgr *)NCSMalloc(sizeof(struct jpeg_destination_mgr), TRUE);
			JpegFileObject.dest->next_output_byte = (JOCTET *)pJpegBuffer;
			JpegFileObject.dest->free_in_buffer = nJpegBufferLength;
			JpegFileObject.dest->init_destination = init_destination;
			JpegFileObject.dest->empty_output_buffer = empty_output_buffer;
			JpegFileObject.dest->term_destination = term_destination;
		}

		if (m_nSetViewNrBands == 3) {
			JpegFileObject.input_components = 3;
			JpegFileObject.in_color_space = JCS_RGB;
		}
		else if (m_nSetViewNrBands == 1) {
			JpegFileObject.input_components = 1;
			JpegFileObject.in_color_space = JCS_GRAYSCALE;
		}
		else {
			delete pJpegBuffer;
			return NCS_INVALID_BAND_NR;
		}

		// Allocate the buffer
		pBuffer = (UINT8 *)NCSMalloc(sizeof(UINT8) * m_nSetViewWidth * 3, TRUE);
		if (JpegFileObject.in_color_space == JCS_RGB) {
			row_pointer[0] = pBuffer;
		} else if (JpegFileObject.in_color_space == JCS_GRAYSCALE) {
			pGreyBuffer = (UINT8 *)NCSMalloc(sizeof(UINT8) * m_nSetViewWidth, TRUE);
			row_pointer[0] = pGreyBuffer;
		}

		// Set up the default params
		(pNCSJpegOutput->m_pLibTable->SetDefaultsFn)(&JpegFileObject);

		//Set the quality
		(pNCSJpegOutput->m_pLibTable->SetQualityFn)(&JpegFileObject, (int)nQuality, TRUE );
		
		// Start the compressor
		(pNCSJpegOutput->m_pLibTable->StartCompressFn)(&JpegFileObject, TRUE);

		//Write the scanlines
		row_stride = m_nSetViewWidth * 3 * sizeof(UINT8);
		while (JpegFileObject.next_scanline < JpegFileObject.image_height) {
			nStatus = ReadLineRGB(pBuffer);
			if (nStatus != NCSECW_READ_OK) {
				(pNCSJpegOutput->m_pLibTable->AbortCompressFn)(&JpegFileObject);
				break;
			}
			if (JpegFileObject.in_color_space == JCS_GRAYSCALE) {
				UINT8 *pMarker = pBuffer;
				for (int cell = 0; cell < m_nSetViewWidth; cell ++) {
					pGreyBuffer[cell] = *pMarker; pMarker +=3;
				}
			}
			(pNCSJpegOutput->m_pLibTable->WriteScanlinesFn)(&JpegFileObject, &row_pointer[0], 1);
		}

		// Finish up the compresion
		(pNCSJpegOutput->m_pLibTable->FinishCompressFn)(&JpegFileObject);
		if(pOutputFile) {
			fclose(pOutputFile);
		} else {
			*pBufferLength = (UINT32)(nJpegBufferLength - JpegFileObject.dest->free_in_buffer);
			*ppBuffer = pJpegBuffer;
			NCSFree((void*)JpegFileObject.dest);
			JpegFileObject.dest = NULL;
		}
		
		NCSFree(pBuffer);
		if (JpegFileObject.in_color_space == JCS_GRAYSCALE) {
			NCSFree(pGreyBuffer);
		}

		// Destroy the object
		(pNCSJpegOutput->m_pLibTable->DestroyCompressFn)(&JpegFileObject);

		if (nStatus != NCSECW_READ_OK) {
			NCSFormatErrorText(NCS_ECW_ERROR, " : ReadLineRGB() returned an error, JPEG output aborted.");
			return NCS_ECW_ERROR;
		}
  	}
	else {
		NCSFormatErrorText(NCS_ECW_ERROR,  ": could not determine the location of the JPEG library (libjpeg.dll).\n\n"
							"The JPEG library should be located in the directory specified by the registry key\n"
							"HKEY_LOCAL_MACHINE\\Software\\Earth Resource Mapping\\Image Web Server\\ClientBaseDir.");//[02]
		delete pNCSJpegOutput;
		pNCSJpegOutput = NULL;	//[01]
		return NCS_ECW_ERROR;
	}

	return NCS_SUCCESS;
}

NCSError CNCSRenderer::WriteJPEG(char *pFilename, INT32 nQuality)
{
	return(WriteJPEG(pFilename, (UINT8**)NULL, (UINT32*)NULL, nQuality));
}

NCSError CNCSRenderer::WriteJPEG(UINT8 **ppBuffer, UINT32 *pBufferLength, INT32 nQuality)
{
	return(WriteJPEG((char*)NULL, ppBuffer, pBufferLength, nQuality));
}

NCSError CNCSRenderer::WriteWorldFile(char *pFilename)
{
	// ARCView documentation states a world file is the first letter of the extension, 
	// the last letter of the extension and then a "w" so file.tif becomes file.tfw
	// file.bmp becomes file.bpw etc.

	FILE *pWorldFile = NULL;
	char *pWorldFileName = NULL;
	extern int errno;
	INT32 nLen;
	double dCellSizeX, dCellSizeY;
	INT32 nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY;

	if (!pFilename) {
		NCSFormatErrorText(NCS_INVALID_ARGUMENTS, "WriteWorldFile()");
		return NCS_INVALID_ARGUMENTS;
	}	

	// Work out the new file name
	pWorldFileName = NCSStrDup(pFilename);
	nLen = (INT32)strlen(pWorldFileName);
	pWorldFileName[nLen-2] = pWorldFileName[nLen-1];
	pWorldFileName[nLen-1] = 'w';
	pWorldFile = fopen(pWorldFileName, "w+t");

	// Convert the renderer extents into dataset coordinates
	ConvertWorldToDataset(m_dAdjustedWorldTLX, m_dAdjustedWorldTLY, &nDatasetTLX, &nDatasetTLY);
	ConvertWorldToDataset(m_dAdjustedWorldBRX, m_dAdjustedWorldBRY, &nDatasetBRX, &nDatasetBRY);

	// This is a bit wally, but calculate the cell size based on the underling ecw file cell size
	// as a ratio to the output image size. This doesnt really work for geodetic datasets at the
	// extreme top and bottom of the world. Fix them later.
	dCellSizeX = m_dCellIncrementX / ((IEEE8)m_nAdjustedViewWidth/(IEEE8)(nDatasetBRX-nDatasetTLX));
	dCellSizeY = m_dCellIncrementY / ((IEEE8)m_nAdjustedViewHeight/(IEEE8)(nDatasetBRY-nDatasetTLY));

	if (pWorldFile != NULL) {
		char buffer[128];
		// Write the x cell size
		sprintf(buffer, "%lf\n", dCellSizeX);
		fwrite(buffer, sizeof(char), strlen(buffer), pWorldFile);

		// Rotation for row - not supported yet on ecw files.
		sprintf(buffer, "%lf\n", 0.0);
		fwrite(buffer, sizeof(char), strlen(buffer), pWorldFile);

		// Rotation for column - not supported yet on ecw files.
		sprintf(buffer, "%lf\n", 0.0);
		fwrite(buffer, sizeof(char), strlen(buffer), pWorldFile);

		// Write the y cell size
		sprintf(buffer, "%lf\n", dCellSizeY);
		fwrite(buffer, sizeof(char), strlen(buffer), pWorldFile);

		// Write the x origin value
		sprintf(buffer, "%lf\n", m_dAdjustedWorldTLX);
		fwrite(buffer, sizeof(char), strlen(buffer), pWorldFile);

		// Write the y origin value
		sprintf(buffer, "%lf\n", m_dAdjustedWorldTLY);
		fwrite(buffer, sizeof(char), strlen(buffer), pWorldFile);

		// Close the file
		fclose(pWorldFile);
	}
	else {
		NCSFormatErrorText(NCS_FILE_OPEN_FAILED, pWorldFileName, "");
		return NCS_FILE_OPEN_FAILED;
	}

	return NCS_SUCCESS;
}

void CNCSRenderer::ShutDown( void )
{
	if (pNCSJpegOutput) {
		delete pNCSJpegOutput;
		pNCSJpegOutput = NULL;//[01]
	}
}
