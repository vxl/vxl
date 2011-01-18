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
** FILE:   	ecw_jni.c
** CREATED:	4 Jan 2000
** AUTHOR: 	Mark Sheridan
** PURPOSE:	Java wrappers for JNI usage of the ECW library in Java.
** EDITS:
** 
** [01] 01Nov00 mjs Rewrote to be JVM 1.1 compliant.
** [02] 08Mar04 mjs Removed old 1.1 spec stuff, now needs a 1.2 compatible virtual machine.
**
*******************************************************/

#include "NCSErrors.h"
#include "NCSECWClient.h"
#include "NCSMalloc.h"
#include "NCSUtil.h"
#include "NCSBuildNumber.h"
#include "JNCSFile.h"
#include "NCSDynamicLib.h"

#ifndef JNI_VERSION_1_2
#error You must compile this class against a 1.2 virtual machine specification
#endif

static JavaVM *pJavaVirtualMachineInst = NULL;
struct NCSJNIFieldIDs *pGlobalJNCSFieldIDs = NULL;

typedef struct NCSJNIFieldIDs {
	jfieldID jIDNativeDataPointer;
	jfieldID jIDWidth;
	jfieldID jIDHeight;	
	jfieldID jIDNumberOfBands;
	jfieldID jIDCompressionRate;
	jfieldID jIDCellIncrementX;
	jfieldID jIDCellIncrementY;
	jfieldID jIDCellSizeUnits;
	jfieldID jIDOriginX;
	jfieldID jIDOriginY;
	jfieldID jIDDatum;
	jfieldID jIDProjection;
	jfieldID jIDFilename;
	jfieldID jIDIsOpen;
	jmethodID jIDRefreshUpdateMethod;
	jfieldID jIDFileType;
	jfieldID jIDFileMimeType;
} NCSJNIFieldIDs;

// This is the object specific data structure, its cached.
typedef struct NCSJNIInfo {
	//jobject  ECWFile;
	NCSFileView *pFileView;
} NCSJNIInfo;

void NCSJNIThrowException(JNIEnv *pEnv, const char *pSignature, const char *pMessage)
{
	jclass jExceptionClass = NULL;

	if ((*pEnv)->ExceptionCheck(pEnv)) {
		(*pEnv)->ExceptionDescribe(pEnv);
		(*pEnv)->ExceptionClear(pEnv);
	}

	jExceptionClass = (*pEnv)->FindClass(pEnv, pSignature);

	if (jExceptionClass != NULL) {
		(*pEnv)->ThrowNew(pEnv, jExceptionClass, pMessage); 
	}
	(*pEnv)->DeleteLocalRef(pEnv, jExceptionClass);
}

NCSError NCSCreateJNIInfoStruct(JNIEnv *pEnv, jobject ECWFile, NCSFileView *pNCSFileView, NCSJNIInfo **pReturn)
{
	NCSJNIInfo *pJNIInfo;
	char *pErrorString = NULL;
	
	pJNIInfo = (NCSJNIInfo *)NCSMalloc(sizeof(NCSJNIInfo), TRUE);
	if (pJNIInfo != NULL)
	{
		pJNIInfo->pFileView = pNCSFileView;
		//pJNIInfo->ECWFile = (void *)(*pEnv)->NewGlobalRef(pEnv, ECWFile);
		*pReturn = pJNIInfo;
	}
	else
	{
		return NCS_COULDNT_ALLOC_MEMORY;
	}
	return NCS_SUCCESS;
}

/*
 * Class:     None
 * Method:    JNI_OnLoad
 * Signature: ()Z
 */

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *pJVM, void *reserved)
{
	JNIEnv *pEnv;

	if ((*pJVM)->GetEnv(pJVM, (void **)&pEnv, JNI_VERSION_1_2)) {
		NCSJNIThrowException(pEnv, "java/lang/Exception", "JNCS classes require a version 1.2 or higher virtual machine.");
		return JNI_ERR;
	}
	else {
		pJavaVirtualMachineInst = pJVM;
#ifndef WIN32
		NCSecwInitInternal();
#endif
	}
	return JNI_VERSION_1_2;
}


/*
 * Class:     None
 * Method:    JNI_OnUnload
 * Signature: ()Z
 */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *pJVM, void *reserved)
{
#ifndef WIN32
		NCSecwShutdown();
#endif
		NCSFree((void *)pGlobalJNCSFieldIDs);
	return;
}

/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    NCSJNIInit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSFile_NCSJNIInit
  (JNIEnv *pEnv, jclass ECWFileClass)
{
	NCSJNIFieldIDs *pJNIInfo = NULL;
	char *pErrorString = NULL;

	if (!pGlobalJNCSFieldIDs) {

		pJNIInfo = (NCSJNIFieldIDs *)NCSMalloc(sizeof(NCSJNIFieldIDs), TRUE);

		// Get all the field ids of the ECWFile object
		pJNIInfo->jIDNativeDataPointer	= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "nativeDataPointer", "J" );
		pJNIInfo->jIDWidth				= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "width", "I" );
		pJNIInfo->jIDHeight				= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "height", "I" );
		pJNIInfo->jIDNumberOfBands		= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "numBands", "I" );
		pJNIInfo->jIDCompressionRate	= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "compressionRate", "D" );
		pJNIInfo->jIDCellIncrementX		= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "cellIncrementX", "D" );
		pJNIInfo->jIDCellIncrementY		= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "cellIncrementY", "D" );
		pJNIInfo->jIDCellSizeUnits		= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "cellSizeUnits", "I" );
		pJNIInfo->jIDOriginX			= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "originX", "D" );
		pJNIInfo->jIDOriginY			= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "originY", "D" );
		pJNIInfo->jIDDatum				= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "datum", "Ljava/lang/String;" );
		pJNIInfo->jIDProjection			= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "projection", "Ljava/lang/String;" );
		pJNIInfo->jIDFilename			= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "fileName", "Ljava/lang/String;" );
		pJNIInfo->jIDIsOpen				= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "bIsOpen", "Z" );
		pJNIInfo->jIDRefreshUpdateMethod= (*pEnv)->GetMethodID(pEnv, ECWFileClass, "refreshUpdate", "(IIDDDD)V");

		pJNIInfo->jIDFileType			= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "fileType", "I" );
		pJNIInfo->jIDFileMimeType		= (*pEnv)->GetFieldID(pEnv, ECWFileClass, "mimeType", "Ljava/lang/String;" );


		// Do some error checking
		if (!pJNIInfo->jIDNativeDataPointer)
			pErrorString = "Could not determine fieldID for 'nativeDataPointer' in ECWFile object.";
		if (!pJNIInfo->jIDWidth)
			pErrorString = "Could not determine fieldID for 'width' in ECWFile object.";
		if (!pJNIInfo->jIDHeight)
			pErrorString = "Could not determine fieldID for 'height' in ECWFile object.";
		if (!pJNIInfo->jIDNumberOfBands)
			pErrorString = "Could not determine fieldID for 'numBands' in ECWFile object.";
		if (!pJNIInfo->jIDCompressionRate)
			pErrorString = "Could not determine fieldID for 'compressionRate' in ECWFile object.";
		if (!pJNIInfo->jIDCellIncrementX)
			pErrorString = "Could not determine fieldID for 'cellIncrementX' in ECWFile object.";
		if (!pJNIInfo->jIDCellIncrementY)
			pErrorString = "Could not determine fieldID for 'cellIncrementY' in ECWFile object.";
		if (!pJNIInfo->jIDCellSizeUnits)
			pErrorString = "Could not determine fieldID for 'cellSizeUnits' in ECWFile object.";
		if (!pJNIInfo->jIDOriginX)
			pErrorString = "Could not determine fieldID for 'originX' in ECWFile object.";
		if (!pJNIInfo->jIDOriginY)
			pErrorString = "Could not determine fieldID for 'originY' in ECWFile object.";
		if (!pJNIInfo->jIDDatum)
			pErrorString = "Could not determine fieldID for 'datum' in ECWFile object.";
		if (!pJNIInfo->jIDProjection)
			pErrorString = "Could not determine fieldID for 'projection' in ECWFile object.";
		if (!pJNIInfo->jIDFilename)
			pErrorString = "Could not determine fieldID for 'fileName' in ECWFile object.";
		if (!pJNIInfo->jIDIsOpen)
			pErrorString = "Could not determine fieldID for 'bIsOpen' in ECWFile object.";
		if (!pJNIInfo->jIDFileType)
			pErrorString = "Could not determine fieldID for 'fileType' in ECWFile object.";
		if (!pJNIInfo->jIDFileMimeType)
			pErrorString = "Could not determine fieldID for 'mimeType' in ECWFile object.";

		if (pErrorString) {
#ifdef WIN32
			MessageBox(NULL, OS_STRING(pErrorString), NCS_T("JNCSClass Library (JNI)"), MB_OK);
#else
			fprintf(stderr, "JNCSClass Library (JNI) : %s\n", pErrorString);
#endif
			NCSFormatErrorText(NCS_JNI_ERROR, pErrorString);
			return NCS_JNI_ERROR;
		}
		else {
			pGlobalJNCSFieldIDs = pJNIInfo;
			return NCS_SUCCESS;
		}
	}
	else {
		return NCS_SUCCESS;
	}
}

/*
 * There is so much that can go wrong in this call, that is why there is so much error checking.
 *
 * Class:     None
 * Method:    NCSJNIRefreshCallback
 * Signature: 
 */
NCSEcwReadStatus NCSJNIRefreshCallback(NCSFileView *pNCSFileView)
{
	NCSFileViewSetInfo	*pViewInfo;
	NCSJNIInfo *pJNIInfo;
	jint nError;
	JNIEnv *pEnv;
	jclass EcwObjectClass = NULL;
	jobject pECWFile = NULL;
	
	NCScbmGetViewInfo(pNCSFileView, &pViewInfo);

	// Must attach to the vm thread before calling any java methods.
	nError = (*pJavaVirtualMachineInst)->AttachCurrentThread(pJavaVirtualMachineInst, (void **)&pEnv, NULL);
	
	if (nError != 0) {
		char Message[] = "The ECW JNI interface could not attach to the current thread in\n"
						 "the Java virtual machine. Please refer to the Image Web Server\n"
						 "Java SDK documentation for more information about JVM threads.\n\n"
						 "Progressive imagery will not be available.";
#ifdef WIN32
		MessageBox(GetActiveWindow(), OS_STRING(Message), NCS_T("JNCSFile VM Error"), MB_OK);
#else
		fprintf(stderr, "JNCSFile VM Error : %s\n", Message);
#endif
		return NCSECW_READ_FAILED;
	}

	// Make sure we got a view info struct.
	if (!pViewInfo) {
		NCSJNIThrowException(pEnv, "java/lang/Exception", "ECW JNI component could not obtain the NCSViewInfo pointer from the NCSFileView. No refreshUpdate() will occur.");
		(*pJavaVirtualMachineInst)->DetachCurrentThread(pJavaVirtualMachineInst);
		return NCSECW_READ_FAILED;
	}

	// The file is the global reference stashed in the client data pointer.
	pECWFile = (jobject)pViewInfo->pClientData;
	
	// Check to make sure that the ECW object has not been free'd or garbaged collected. This is only valid for JNI 1.2
	if ((*pEnv)->IsSameObject(pEnv, pECWFile/*pViewInfo->pClientData*/, NULL) == JNI_TRUE) {
		(*pJavaVirtualMachineInst)->DetachCurrentThread(pJavaVirtualMachineInst);
		return NCSECW_READ_FAILED;
	}

	// Use the valid reference to the object, to obtain the data pointer.
	pJNIInfo = (NCSJNIInfo *)(*pEnv)->GetLongField(pEnv, pViewInfo->pClientData, pGlobalJNCSFieldIDs->jIDNativeDataPointer);

	if (!pJNIInfo) {
		NCSJNIThrowException(pEnv, "java/lang/Exception", "The ECW JNI component could not obtain the client data pointer from the NCSViewInfo struct. No refreshUpdate() will occur.");
		(*pJavaVirtualMachineInst)->DetachCurrentThread(pJavaVirtualMachineInst);
		return NCSECW_READ_FAILED;
	}
	
	EcwObjectClass = (*pEnv)->GetObjectClass(pEnv, pECWFile/*pJNIInfo->ECWFile*/);
	if (EcwObjectClass == NULL) {
		NCSJNIThrowException(pEnv, "java/lang/Exception", "The ECW JNI component could not determine the class signature of the ECW object.");
		(*pJavaVirtualMachineInst)->DetachCurrentThread(pJavaVirtualMachineInst);
		return NCSECW_READ_FAILED;
	}

	(*pEnv)->DeleteLocalRef(pEnv, EcwObjectClass);

	//Call the  "refreshUpdate" method on this object, if it implements the JNCSProgressiveUpdate interface.
	if (pGlobalJNCSFieldIDs->jIDRefreshUpdateMethod) {
		(*pEnv)->CallVoidMethod(pEnv, \
								pECWFile, \
								pGlobalJNCSFieldIDs->jIDRefreshUpdateMethod, \
								pViewInfo->nSizeX, pViewInfo->nSizeY, \
								pViewInfo->fTopX, pViewInfo->fLeftY, \
								pViewInfo->fBottomX, pViewInfo->fRightY);
	}
	else {
		NCSJNIThrowException(pEnv, "java/lang/ClassNotFoundException", "'refreshUpdate' method not found. Derived classes must implement the interface 'JNCSProgressiveUpdate' to use progressive imagery.");
		(*pJavaVirtualMachineInst)->DetachCurrentThread(pJavaVirtualMachineInst);
		return NCSECW_READ_FAILED;
	}

	(*pJavaVirtualMachineInst)->DetachCurrentThread(pJavaVirtualMachineInst);

	return NCSECW_READ_OK;
}

/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWOpen
 * Signature: (Ljava/lang/String;Z)I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSFile_ECWOpen
  (JNIEnv *pEnv, jobject JNCSFile, jstring Filename, jboolean bProgressive)
{
	const char *pFilename = (*pEnv)->GetStringUTFChars(pEnv, Filename, (jboolean *)NULL);
	NCSFileView *pNCSFileView = NULL;
	NCSFileViewSetInfo *pNCSFileViewSetInfo = NULL;
	NCSError nError;

	if (bProgressive) {
		nError = NCScbmOpenFileView((char *)pFilename, &pNCSFileView, NCSJNIRefreshCallback);
	}
	else {
		nError = NCScbmOpenFileView((char *)pFilename, &pNCSFileView, NULL);
	}

	if (NCS_FAILED(nError)) {
		// Return the short filename, since people dont like diplaying the full ecwp url
		char	*pProtocol, *pHost, *pECWFilename, *pShortFileName;
		int		nProtocolLength, nHostLength, nFilenameLength;

		NCSecwNetBreakdownUrl((char *)pFilename, &pProtocol, &nProtocolLength,
									 &pHost, &nHostLength,
									 &pECWFilename, &nFilenameLength);

		if (pECWFilename && nFilenameLength > 0 && (strstr(pECWFilename, "/") || strstr(pECWFilename, "\\")))
		{
			int len = strlen(pECWFilename), index = 0;
			for (index=len; index > 0; index--)
			{
				pShortFileName = &pECWFilename[index];
				if (pECWFilename[index] == '\\' || pECWFilename[index] == '/')
				{
					pShortFileName ++;
					break;
				}
			}
		}
		else 
		{
			pShortFileName = (char *)pFilename;
		}
		
		NCSFormatErrorText(NCS_FILE_OPEN_FAILED, pShortFileName, NCSGetLastErrorText(nError));
		return NCS_FILE_OPEN_FAILED;
	}
	else {
		NCSJNIInfo *pJNIInfo = NULL;
		char *pMimeType = NULL;
		NCSFileViewFileInfo	*pNCSFileInfo = NULL;
		
		nError = NCSCreateJNIInfoStruct(pEnv, JNCSFile, pNCSFileView, &pJNIInfo);
		NCScbmGetViewFileInfo(pNCSFileView, &pNCSFileInfo);

		// Set the properties in the actual Java object
		(*pEnv)->SetIntField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDWidth, (jint)pNCSFileInfo->nSizeX	);
		(*pEnv)->SetIntField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDHeight, (jint)pNCSFileInfo->nSizeY );
		(*pEnv)->SetIntField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNumberOfBands, (jint)pNCSFileInfo->nBands);
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCompressionRate, (jdouble)pNCSFileInfo->nCompressionRate );
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCellIncrementX, (jdouble)pNCSFileInfo->fCellIncrementX );
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCellIncrementY, (jdouble)pNCSFileInfo->fCellIncrementY );
		(*pEnv)->SetIntField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCellSizeUnits, (jint)pNCSFileInfo->eCellSizeUnits);
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDOriginX, (jdouble)pNCSFileInfo->fOriginX );
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDOriginY, (jdouble)pNCSFileInfo->fOriginY );
		(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDDatum, (*pEnv)->NewStringUTF(pEnv, pNCSFileInfo->szDatum));
		(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDProjection, (*pEnv)->NewStringUTF(pEnv, pNCSFileInfo->szProjection));
		(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDFilename, Filename);
		(*pEnv)->SetBooleanField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDIsOpen, JNI_TRUE);
		(*pEnv)->SetLongField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNativeDataPointer, (jlong)pJNIInfo);

		(*pEnv)->SetIntField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDFileType, (jint)NCScbmGetFileType(pNCSFileView) );
		pMimeType = NCScbmGetFileMimeType(pNCSFileView);
		if( pMimeType ) {
			(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDFileMimeType, (*pEnv)->NewStringUTF(pEnv, pMimeType) );
			NCSFree( pMimeType );
			pMimeType = NULL;
		} else {
			(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDFileMimeType, NULL );
		}
	}

	(*pEnv)->ReleaseStringUTFChars(pEnv, Filename, (const char *)pFilename);

	NCScbmGetViewInfo(pNCSFileView, &pNCSFileViewSetInfo);

	pNCSFileViewSetInfo->pClientData = (void *)(*pEnv)->NewGlobalRef(pEnv, JNCSFile);

	return NCS_SUCCESS;
}

/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWClose
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_ermapper_ecw_JNCSFile_ECWClose
  (JNIEnv *pEnv , jobject JNCSFile, jboolean bFreeCache)
{
	NCSError nError;
	NCSJNIInfo *pJNIInfo = NULL;
	
	pJNIInfo = (NCSJNIInfo *)(*pEnv)->GetLongField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNativeDataPointer);
	
	if (pJNIInfo != NULL) {
		NCSFileViewSetInfo	*pViewInfo;

		NCScbmGetViewInfo(pJNIInfo->pFileView, &pViewInfo);
	
		// Clear the Java object members.
		(*pEnv)->SetIntField   (pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDWidth, (jint)0	);
		(*pEnv)->SetIntField   (pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDHeight, (jint)0 );
		(*pEnv)->SetIntField   (pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNumberOfBands, (jint)0);
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCompressionRate, (jdouble)0.0 );
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCellIncrementX, (jdouble)0.0 );
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCellIncrementY, (jdouble)0.0 );
		(*pEnv)->SetIntField   (pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDCellSizeUnits, (jint)0 );
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDOriginX, (jdouble)0.0 );
		(*pEnv)->SetDoubleField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDOriginY, (jdouble)0.0 );
		(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDDatum, (*pEnv)->NewStringUTF(pEnv, ""));
		(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDProjection, (*pEnv)->NewStringUTF(pEnv, ""));
		(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDFilename, NULL);
		(*pEnv)->SetBooleanField(pEnv,JNCSFile, pGlobalJNCSFieldIDs->jIDIsOpen, JNI_FALSE);
		(*pEnv)->SetLongField  (pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNativeDataPointer, (jlong)0);

		(*pEnv)->SetIntField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDFileType, (jint)0 );
		(*pEnv)->SetObjectField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDFileMimeType, NULL );

		// Clean up any global refs
		//(*pEnv)->DeleteGlobalRef(pEnv, pJNIInfo->ECWFile);
		(*pEnv)->DeleteGlobalRef(pEnv, pViewInfo->pClientData);

		nError = NCScbmCloseFileViewEx(pJNIInfo->pFileView, bFreeCache);

		pJNIInfo->pFileView = NULL;
		NCSFree(pJNIInfo);
	}
	return;
}

/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWSetView
 * Signature: (I[IIIIIDDDDII)I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSFile_ECWSetView
  (JNIEnv *pEnv, jobject ECWFile, jint nBands, jintArray nBandList, jint nDatasetTLX, jint nDatasetTLY, jint nDatasetBRX, jint nDatasetBRY, jdouble dWorldTLX, jdouble dWorldTLY, jdouble dWorldBRX, jdouble dWorldBRY, jint nWidth, jint nHeight)
{
	NCSError nError = NCS_SUCCESS;
	NCSJNIInfo *pJNIInfo = NULL;

	// Sanity check
	if ((*pEnv)->IsInstanceOf(pEnv, ECWFile, (*pEnv)->FindClass(pEnv, "com/ermapper/ecw/JNCSFile")) == JNI_FALSE)
	{
#ifdef WIN32
		MessageBox(NULL, OS_STRING("ECWSetView() error : object is not a JNCSFile instance"), NCS_T("JNCSClass Library (JNI)"), MB_OK);
#else
		fprintf(stderr, "ECWSetView() error : object is not a JNCSFile instance");
#endif
		return NCS_JNI_ERROR;
	}

	pJNIInfo = (NCSJNIInfo *)(*pEnv)->GetLongField(pEnv, ECWFile, pGlobalJNCSFieldIDs->jIDNativeDataPointer);

	if (pJNIInfo) {
		
		jint *pBandBuffer = (jint *)NCSMalloc(sizeof(UINT32)*nBands+1, TRUE);
		(*pEnv)->GetIntArrayRegion(pEnv, nBandList, 0, nBands, pBandBuffer);
		
		nError = NCScbmSetFileViewEx(   pJNIInfo->pFileView, 
										nBands, 
										(UINT32*)pBandBuffer,
										nDatasetTLX, nDatasetTLY, nDatasetBRX, nDatasetBRY,
										nWidth,
										nHeight,
										dWorldTLX,
										dWorldTLY,
										dWorldBRX,
										dWorldBRY);
		NCSFree(pBandBuffer);

	} else {
		NCSFormatErrorText(NCS_JNI_ERROR, "method SetView() could not get native data from JNCSFile object.");
		nError = NCS_JNI_ERROR;
	}
		
	return nError;
}


/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWReadImageRGBA
 * Signature: ([I)I
 */
JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSFile_ECWReadImageRGBA
  (JNIEnv *pEnv, jobject JNCSFile, jintArray pRGBArray, jint width, jint height)
{
	jboolean bIsCopy;
	NCSEcwReadStatus eStatus;
	NCSJNIInfo *pJNIInfo = NULL;
	NCSError nError = NCS_SUCCESS;
	jint *pRGBAPixels;
	jint *pRGBLineArrayPtr = NULL;
	int nIndex;

	pJNIInfo = (NCSJNIInfo *)(*pEnv)->GetLongField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNativeDataPointer);
	
	if (!pJNIInfo) {
		NCSFormatErrorText(NCS_JNI_ERROR, "method readLineRGB() could not get native data from JNCSFile object.");
		nError = NCS_JNI_ERROR;
	}
	else {
		// Lock the primitive array and get a pointer to the memory.
		pRGBAPixels = (jint *)(*pEnv)->GetPrimitiveArrayCritical(pEnv, pRGBArray, &bIsCopy);

		if (pRGBAPixels) {

			pRGBLineArrayPtr = pRGBAPixels;
			for (nIndex = 0; nIndex < height; nIndex ++) {
#if defined(NCSBO_LSBFIRST)
				eStatus = NCScbmReadViewLineBGRA( pJNIInfo->pFileView, (UINT32 *)pRGBLineArrayPtr);
#elif defined(NCSBO_MSBFIRST)
				eStatus = NCScbmReadViewLineBGRA( pJNIInfo->pFileView, (UINT32 *)pRGBLineArrayPtr);
				//eStatus = NCScbmReadViewLineRGBA( pJNIInfo->pFileView, (UINT32 *)pRGBLineArrayPtr);
#endif
				pRGBLineArrayPtr += width;

				if (eStatus == NCSECW_READ_CANCELLED) {
					nError = NCS_SUCCESS;
					break;
				}
				if (eStatus == NCSECW_READ_FAILED) {
					NCSFormatErrorText(NCS_JNI_ERROR, "method readLineRGB() failed (internal error).");
					nError = NCS_JNI_ERROR;
					break;
				}
#ifndef WIN32
				NCSThreadYield();
#endif
			}
		}
		else {
			NCSFormatErrorText(NCS_JNI_ERROR, "method readLineRGB() could not allocate memory for RGB Array.");
			nError = NCS_JNI_ERROR;
		}
		// Copy the array back to the object and free the memory
		(*pEnv)->ReleasePrimitiveArrayCritical(pEnv, pRGBArray, pRGBAPixels, 0);
	}
	return nError;
}


/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWReadLineRGB
 * Signature: ([I)I
 */

JNIEXPORT jint JNICALL Java_com_ermapper_ecw_JNCSFile_ECWReadLineRGBA
  (JNIEnv *pEnv, jobject JNCSFile, jintArray pRGBArray)
{
	jboolean bIsCopy;
	NCSEcwReadStatus eStatus;
	NCSJNIInfo *pJNIInfo = NULL;
	NCSError nError = NCS_SUCCESS;
	jint *pRGBAPixels;

	pJNIInfo = (NCSJNIInfo *)(*pEnv)->GetLongField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNativeDataPointer);
	
	if (!pJNIInfo) {
		NCSFormatErrorText(NCS_JNI_ERROR, "method readLineRGB() could not get native data from JNCSFile object.");
		nError = NCS_JNI_ERROR;
	}
	else {
		// Lock the primitive array and get a pointer to the memory.
		pRGBAPixels = (jint *)(*pEnv)->GetPrimitiveArrayCritical(pEnv, pRGBArray, &bIsCopy);

		if (pRGBAPixels) {
			// Read into a RGBA Java scaneline (which is byte reversed, so go the BGRA here (???)
			eStatus = NCScbmReadViewLineBGRA( pJNIInfo->pFileView, (UINT32 *)pRGBAPixels);

			if (eStatus == NCSECW_READ_FAILED) {
				NCSFormatErrorText(NCS_JNI_ERROR, "method readLineRGB() failed (internal error).");
				nError = NCS_JNI_ERROR;
			}	
		}
		else {
			NCSFormatErrorText(NCS_JNI_ERROR, "method readLineRGB() could not allocate memory for RGB Array.");
			nError = NCS_JNI_ERROR;
		}
		// Copy the array back to the object and free the memory
		(*pEnv)->ReleasePrimitiveArrayCritical(pEnv, pRGBArray, pRGBAPixels, 0);
	}
	return nError;
}

/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWGetErrorString
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ermapper_ecw_JNCSFile_ECWGetErrorString
  (JNIEnv *pEnv, jobject JNCSFile, jint nErrorNumber)
{
	return (*pEnv)->NewStringUTF(pEnv, NCSGetLastErrorText(nErrorNumber));
}


/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWGetLibVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ermapper_ecw_JNCSFile_ECWGetLibVersion
  (JNIEnv *pEnv, jclass clazz)
{
	return (*pEnv)->NewStringUTF(pEnv, NCS_VERSION_STRING_NO_NULL);	
}

/*
 * Class:     com_ermapper_ecw_JNCSFile
 * Method:    ECWGetPercentComplete
 * Signature: ()I
 */

JNIEXPORT jshort JNICALL Java_com_ermapper_ecw_JNCSFile_ECWGetPercentComplete
  (JNIEnv *pEnv, jobject JNCSFile)

{
	NCSFileViewSetInfo *pViewInfo = NULL;
	NCSJNIInfo *pJNIInfo = NULL;

	pJNIInfo = (NCSJNIInfo *)(*pEnv)->GetLongField(pEnv, JNCSFile, pGlobalJNCSFieldIDs->jIDNativeDataPointer);

	if (!pJNIInfo) 
		return 0;

	if (pJNIInfo->pFileView) {
		NCScbmGetViewInfo(pJNIInfo->pFileView, &pViewInfo); 
		if (pViewInfo) {
			return (jshort)((pViewInfo->nBlocksAvailable / (double)pViewInfo->nBlocksInView) * 100);
		}
	}
	else { 
		return 0;
	}

	return 0;
}
