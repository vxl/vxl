/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Andrew Hewett
 *
 *  Purpose: 
 *  Definitions of "well known" DICOM Unique Indentifiers,
 *  routines for finding and creating UIDs.
 *
 */

#ifndef DCUID_H
#define DCUID_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"

#define INCLUDE_CSTDLIB
#include "ofstdinc.h"

#ifdef HAVE_UNISTD_H
BEGIN_EXTERN_C
#include <unistd.h> /* for NULL */
END_EXTERN_C
#endif


/*
** dcmFindNameOfUID(const char* uid)
** Return the name of a UID.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns NULL if the UID is not known.
*/

const char* dcmFindNameOfUID(const char* uid);

//
// dcmFindUIDFromName(const char* name)
// Return the UID of a name.
// Performs a table lookup and returns a pointer to a read-only string.
// Returns NULL if the name is not known.
//

const char * dcmFindUIDFromName(const char * name);


/*
** The global variable dcmStorageSOPClassUIDs is an array of 
** string pointers containing the UIDs of all known Storage SOP
** Classes.  The global variable numberOfDcmStorageStopClassUIDs
** defines the size of the array.
*/

extern const char* dcmStorageSOPClassUIDs[];
extern const int numberOfDcmStorageSOPClassUIDs;

/*
** dcmIsaStorageSOPClassUID(const char* uid)
** Returns true if the uid is one of the Storage SOP Classes.
** Performs a table lookup in the dcmStorageSOPClassUIDs table.
*/
OFBool dcmIsaStorageSOPClassUID(const char* uid);

/*
** The global variable dcmImageSOPClassUIDs is an array of 
** string pointers containing the UIDs of all known Image SOP
** Classes.  The global variable numberOfDcmImageSOPClassUIDs
** defines the size of the array.
** NOTE: this list represents a subset of the dcmStorageSOPClassUIDs list
*/

extern const char* dcmImageSOPClassUIDs[];
extern const int numberOfDcmImageSOPClassUIDs;


/*              
** char* generateUniqueIdentifier(char* uid)
** Creates a Unique Identifer in uid and returns uid.
** uid must be at least 65 bytes. Care is taken to make sure
** that the generated UID is 64 characters or less.
** 
** If a prefix string is not passed as the second argument a 
** default of SITE_INSTANCE_UID_ROOT (see below) will be used.
** Otherwise the supplied prefix string will appear at the beginning
** of uid.
**
** The UID is created by appending to the prefix the following:
**      the host id (if obtainable, zero otherwise)
**      the process id (if obtainable, zero otherwise)
**      the system calendar time
**      an accumulating counter for this process
*/
char* dcmGenerateUniqueIdentifier(char* uid, const char* prefix=NULL);

/* 
 * dcmSOPClassUIDToModality
 * performs a table lookup and returns a short modality identifier
 * that can be used for building file names etc.
 * Identifiers are defined for all storage SOP classes.
 * Returns NULL if no modality identifier found or sopClassUID==NULL.
 */
const char *dcmSOPClassUIDToModality(const char *sopClassUID);

/*
 * dcmGuessModalityBytes
 * performs a table lookup and returns a guessed average 
 * file size for the given SOP class.
 * Average sizes are defined for all storage SOP classes.
 */
unsigned long dcmGuessModalityBytes(const char *sopClassUID);

/*
** String Constants
*/

/* 
** OFFIS UID is: 1.2.276.0.7230010
** UID Root for dcmtk DICOM project:   1.2.276.0.7230010.3
**          for OFFIS GO-Kard project: 1.2.276.0.7230010.8
*/

/* NOTE: Implementation version name VR=SH may not be longer than 16 chars
 *   The second name is used to identify files written without dcmdata
 *   (i.e. using the --bit-preserving switch in various tools)
 */
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME   "OFFIS_DCMTK_352"
#define OFFIS_DTK_IMPLEMENTATION_VERSION_NAME2  "OFFIS_DCMBP_352"
#define OFFIS_DCMTK_RELEASEDATE                 "2002-12-23"

#define OFFIS_UID_ROOT                          "1.2.276.0.7230010.3"
#define OFFIS_DCMTK_VERSION                     "3.5.2"
#define OFFIS_DCMTK_VERSION_NUMBER              352
#define OFFIS_IMPLEMENTATION_CLASS_UID          OFFIS_UID_ROOT ".0." OFFIS_DCMTK_VERSION
#define OFFIS_INSTANCE_CREATOR_UID              OFFIS_IMPLEMENTATION_CLASS_UID

#define OFFIS_PRIVATE_CODING_SCHEME_UID_ROOT    OFFIS_UID_ROOT ".0.0."
#define OFFIS_PRIVATE_CODING_SCHEME_VERSION     "1"
#define OFFIS_PRIVATE_CODING_SCHEME_CREATOR_UID OFFIS_PRIVATE_CODING_SCHEME_UID_ROOT OFFIS_PRIVATE_CODING_SCHEME_VERSION

/*
** Each site should define its own SITE_UID_ROOT
*/
#ifndef SITE_UID_ROOT
#define SITE_UID_ROOT                           OFFIS_UID_ROOT  /* default */
#endif

/*
** Useful UID prefixes. These can be whatever you want.
**
** These site UIDs are arbitrary, non-standard, with no meaning
** and can be changed at any time.  Do _not_ rely on these values.  
** Do _not_ assume any semantics when using these suffixes.
**
*/

#define SITE_STUDY_UID_ROOT                     SITE_UID_ROOT ".1.2"
#define SITE_SERIES_UID_ROOT                    SITE_UID_ROOT ".1.3"
#define SITE_INSTANCE_UID_ROOT                  SITE_UID_ROOT ".1.4"

/*
** A private SOP Class UID which can be used in a file meta-header when
** no real SOP Class is stored in the file. -- NON-STANDARD
*/
#define UID_PrivateGenericFileSOPClass          SITE_UID_ROOT ".1.0.1"


/*
** DICOM Defined Standard Application Context UID
*/

#define UID_StandardApplicationContext          "1.2.840.10008.3.1.1.1"

/*
** Defined Transfer Syntax UIDs
*/

/* Implicit VR Little Endian: Default Transfer Syntax for DICOM */
#define UID_LittleEndianImplicitTransferSyntax  "1.2.840.10008.1.2"
/* Explicit VR Little Endian */
#define UID_LittleEndianExplicitTransferSyntax  "1.2.840.10008.1.2.1"
/* Explicit VR Big Endian */
#define UID_BigEndianExplicitTransferSyntax     "1.2.840.10008.1.2.2"
/* JPEG Baseline (Process 1): Default Transfer Syntax
   for Lossy JPEG 8 Bit Image Compression */
#define UID_JPEGProcess1TransferSyntax          "1.2.840.10008.1.2.4.50"
/* JPEG Extended (Process 2 & 4): Default Transfer Syntax 
   for Lossy JPEG 12 Bit Image Compression (Process 4 only) */
#define UID_JPEGProcess2_4TransferSyntax        "1.2.840.10008.1.2.4.51"
/* JPEG Extended (Process 3 & 5) */
#define UID_JPEGProcess3_5TransferSyntax        "1.2.840.10008.1.2.4.52"
/* JPEG Spectral Selection, Non-Hierarchical (Process 6 & 8) */
#define UID_JPEGProcess6_8TransferSyntax        "1.2.840.10008.1.2.4.53"
/* JPEG Spectral Selection, Non-Hierarchical (Process 7 & 9) */
#define UID_JPEGProcess7_9TransferSyntax        "1.2.840.10008.1.2.4.54"
/* JPEG Full Progression, Non-Hierarchical (Process 10 & 12) */
#define UID_JPEGProcess10_12TransferSyntax      "1.2.840.10008.1.2.4.55"
/* JPEG Full Progression, Non-Hierarchical (Process 11 & 13) */
#define UID_JPEGProcess11_13TransferSyntax      "1.2.840.10008.1.2.4.56"
/* JPEG Lossless, Non-Hierarchical (Process 14) */
#define UID_JPEGProcess14TransferSyntax         "1.2.840.10008.1.2.4.57"
/* JPEG Lossless, Non-Hierarchical (Process 15) */
#define UID_JPEGProcess15TransferSyntax         "1.2.840.10008.1.2.4.58"
/* JPEG Extended, Hierarchical (Process 16 & 18) */
#define UID_JPEGProcess16_18TransferSyntax      "1.2.840.10008.1.2.4.59"
/* JPEG Extended, Hierarchical (Process 17 & 19) */
#define UID_JPEGProcess17_19TransferSyntax      "1.2.840.10008.1.2.4.60"
/* JPEG Spectral Selection, Hierarchical (Process 20 & 22) */
#define UID_JPEGProcess20_22TransferSyntax      "1.2.840.10008.1.2.4.61"
/* JPEG Spectral Selection, Hierarchical (Process 21 & 23) */
#define UID_JPEGProcess21_23TransferSyntax      "1.2.840.10008.1.2.4.62"
/* JPEG Full Progression, Hierarchical (Process 24 & 26) */
#define UID_JPEGProcess24_26TransferSyntax      "1.2.840.10008.1.2.4.63"
/* JPEG Full Progression, Hierarchical (Process 25 & 27) */
#define UID_JPEGProcess25_27TransferSyntax      "1.2.840.10008.1.2.4.64"
/* JPEG Lossless, Hierarchical (Process 28) */
#define UID_JPEGProcess28TransferSyntax         "1.2.840.10008.1.2.4.65"
/* JPEG Lossless, Hierarchical (Process 29) */
#define UID_JPEGProcess29TransferSyntax         "1.2.840.10008.1.2.4.66"
/* JPEG Lossless, Non-Hierarchical, First-Order Prediction (Process 14 
   [Selection Value 1]): Default Transfer Syntax for Lossless JPEG Image Compression */
#define UID_JPEGProcess14SV1TransferSyntax      "1.2.840.10008.1.2.4.70"
/* JPEG-LS Lossless Image Compression */
#define UID_JPEGLSLossless                      "1.2.840.10008.1.2.4.80"
/* JPEG-LS Lossy (Near-Lossless) Image Compression */
#define UID_JPEGLSLossy                         "1.2.840.10008.1.2.4.81"
/* RLE Lossless */
#define UID_RLELossless                         "1.2.840.10008.1.2.5"
/* Deflated Explicit VR Little Endian */
#define UID_DeflatedExplicitVRLittleEndianTransferSyntax "1.2.840.10008.1.2.1.99"
/* JPEG 2000 Image Compression (Lossless Only) */
#define UID_JPEG2000LosslessOnlyTransferSyntax  "1.2.840.10008.1.2.4.90"
/* JPEG 2000 Image Compression (Lossless or Lossy) */
#define UID_JPEG2000TransferSyntax              "1.2.840.10008.1.2.4.91"


/*
** Defined SOP UIDs according to 2001 DICOM edition
*/

// Storage
#define UID_StoredPrintStorage                                     "1.2.840.10008.5.1.1.27"
#define UID_HardcopyGrayscaleImageStorage                          "1.2.840.10008.5.1.1.29"
#define UID_HardcopyColorImageStorage                              "1.2.840.10008.5.1.1.30"
#define UID_ComputedRadiographyImageStorage                        "1.2.840.10008.5.1.4.1.1.1"
#define UID_DigitalXRayImageStorageForPresentation                 "1.2.840.10008.5.1.4.1.1.1.1"
#define UID_DigitalXRayImageStorageForProcessing                   "1.2.840.10008.5.1.4.1.1.1.1.1"
#define UID_DigitalMammographyXRayImageStorageForPresentation      "1.2.840.10008.5.1.4.1.1.1.2"
#define UID_DigitalMammographyXRayImageStorageForProcessing        "1.2.840.10008.5.1.4.1.1.1.2.1"
#define UID_DigitalIntraOralXRayImageStorageForPresentation        "1.2.840.10008.5.1.4.1.1.1.3"
#define UID_DigitalIntraOralXRayImageStorageForProcessing          "1.2.840.10008.5.1.4.1.1.1.3.1"
#define UID_CTImageStorage                                         "1.2.840.10008.5.1.4.1.1.2"
#define UID_RETIRED_UltrasoundMultiframeImageStorage               "1.2.840.10008.5.1.4.1.1.3"
#define UID_UltrasoundMultiframeImageStorage                       "1.2.840.10008.5.1.4.1.1.3.1"
#define UID_MRImageStorage                                         "1.2.840.10008.5.1.4.1.1.4"
#define UID_RETIRED_NuclearMedicineImageStorage                    "1.2.840.10008.5.1.4.1.1.5"
#define UID_RETIRED_UltrasoundImageStorage                         "1.2.840.10008.5.1.4.1.1.6"
#define UID_UltrasoundImageStorage                                 "1.2.840.10008.5.1.4.1.1.6.1"
#define UID_SecondaryCaptureImageStorage                           "1.2.840.10008.5.1.4.1.1.7"
#define UID_StandaloneOverlayStorage                               "1.2.840.10008.5.1.4.1.1.8"
#define UID_StandaloneCurveStorage                                 "1.2.840.10008.5.1.4.1.1.9"
#define UID_TwelveLeadECGWaveformStorage                           "1.2.840.10008.5.1.4.1.1.9.1.1"
#define UID_GeneralECGWaveformStorage                              "1.2.840.10008.5.1.4.1.1.9.1.2"
#define UID_AmbulatoryECGWaveformStorage                           "1.2.840.10008.5.1.4.1.1.9.1.3"
#define UID_HemodynamicWaveformStorage                             "1.2.840.10008.5.1.4.1.1.9.2.1"
#define UID_CardiacElectrophysiologyWaveformStorage                "1.2.840.10008.5.1.4.1.1.9.3.1"
#define UID_BasicVoiceAudioWaveformStorage                         "1.2.840.10008.5.1.4.1.1.9.4.1"
#define UID_StandaloneModalityLUTStorage                           "1.2.840.10008.5.1.4.1.1.10"
#define UID_StandaloneVOILUTStorage                                "1.2.840.10008.5.1.4.1.1.11"
#define UID_GrayscaleSoftcopyPresentationStateStorage              "1.2.840.10008.5.1.4.1.1.11.1"
#define UID_XRayAngiographicImageStorage                           "1.2.840.10008.5.1.4.1.1.12.1"
#define UID_XRayFluoroscopyImageStorage                            "1.2.840.10008.5.1.4.1.1.12.2"
#define UID_RETIRED_XRayAngiographicBiPlaneImageStorage            "1.2.840.10008.5.1.4.1.1.12.3"
#define UID_NuclearMedicineImageStorage                            "1.2.840.10008.5.1.4.1.1.20"
#define UID_RETIRED_VLImageStorage                                 "1.2.840.10008.5.1.4.1.1.77.1" 
#define UID_VLEndoscopicImageStorage                               "1.2.840.10008.5.1.4.1.1.77.1.1"
#define UID_VLMicroscopicImageStorage                              "1.2.840.10008.5.1.4.1.1.77.1.2"
#define UID_VLSlideCoordinatesMicroscopicImageStorage              "1.2.840.10008.5.1.4.1.1.77.1.3"
#define UID_VLPhotographicImageStorage                             "1.2.840.10008.5.1.4.1.1.77.1.4"
#define UID_RETIRED_VLMultiFrameImageStorage                       "1.2.840.10008.5.1.4.1.1.77.2"
#define UID_BasicTextSR                                            "1.2.840.10008.5.1.4.1.1.88.11"
#define UID_EnhancedSR                                             "1.2.840.10008.5.1.4.1.1.88.22" 
#define UID_ComprehensiveSR                                        "1.2.840.10008.5.1.4.1.1.88.33" 
#define UID_PETImageStorage                                        "1.2.840.10008.5.1.4.1.1.128"
#define UID_PETCurveStorage                                        "1.2.840.10008.5.1.4.1.1.129"
#define UID_RTImageStorage                                         "1.2.840.10008.5.1.4.1.1.481.1"
#define UID_RTDoseStorage                                          "1.2.840.10008.5.1.4.1.1.481.2"
#define UID_RTStructureSetStorage                                  "1.2.840.10008.5.1.4.1.1.481.3"
#define UID_RTBeamsTreatmentRecordStorage                          "1.2.840.10008.5.1.4.1.1.481.4"
#define UID_RTPlanStorage                                          "1.2.840.10008.5.1.4.1.1.481.5"
#define UID_RTBrachyTreatmentRecordStorage                         "1.2.840.10008.5.1.4.1.1.481.6"
#define UID_RTTreatmentSummaryRecordStorage                        "1.2.840.10008.5.1.4.1.1.481.7"

// Storage SOP Classes from Supplement 49 (final text)
#define UID_EnhancedMRImageStorage                                  "1.2.840.10008.5.1.4.1.1.4.1" 
#define UID_MRSpectroscopyStorage                                   "1.2.840.10008.5.1.4.1.1.4.2" 
#define UID_RawDataStorage                                          "1.2.840.10008.5.1.4.1.1.66"

// Storage SOP Class from Supplement 50 (final text)
#define UID_MammographyCADSR                                       "1.2.840.10008.5.1.4.1.1.88.50"

// Storage SOP Classes from Supplement 57 (final text)
#define UID_MultiframeSingleBitSecondaryCaptureImageStorage        "1.2.840.10008.5.1.4.1.1.7.1"
#define UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage    "1.2.840.10008.5.1.4.1.1.7.2"
#define UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage    "1.2.840.10008.5.1.4.1.1.7.3"
#define UID_MultiframeTrueColorSecondaryCaptureImageStorage        "1.2.840.10008.5.1.4.1.1.7.4"

// Storage SOP Class from Supplement 59 (final text)
#define UID_KeyObjectSelectionDocument                             "1.2.840.10008.5.1.4.1.1.88.59"


// Query/Retrieve
#define UID_FINDPatientRootQueryRetrieveInformationModel           "1.2.840.10008.5.1.4.1.2.1.1"
#define UID_MOVEPatientRootQueryRetrieveInformationModel           "1.2.840.10008.5.1.4.1.2.1.2"
#define UID_GETPatientRootQueryRetrieveInformationModel            "1.2.840.10008.5.1.4.1.2.1.3"
#define UID_FINDStudyRootQueryRetrieveInformationModel             "1.2.840.10008.5.1.4.1.2.2.1"
#define UID_MOVEStudyRootQueryRetrieveInformationModel             "1.2.840.10008.5.1.4.1.2.2.2"
#define UID_GETStudyRootQueryRetrieveInformationModel              "1.2.840.10008.5.1.4.1.2.2.3"
#define UID_FINDPatientStudyOnlyQueryRetrieveInformationModel      "1.2.840.10008.5.1.4.1.2.3.1"
#define UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel      "1.2.840.10008.5.1.4.1.2.3.2"
#define UID_GETPatientStudyOnlyQueryRetrieveInformationModel       "1.2.840.10008.5.1.4.1.2.3.3"
#define UID_FINDModalityWorklistInformationModel                   "1.2.840.10008.5.1.4.31"
// Supplement 52 (final text)
#define UID_FINDGeneralPurposeWorklistInformationModel "1.2.840.10008.5.1.4.32.1" 

// Print
#define UID_BasicFilmSessionSOPClass                               "1.2.840.10008.5.1.1.1"
#define UID_BasicFilmBoxSOPClass                                   "1.2.840.10008.5.1.1.2"
#define UID_BasicGrayscaleImageBoxSOPClass                         "1.2.840.10008.5.1.1.4"
#define UID_BasicColorImageBoxSOPClass                             "1.2.840.10008.5.1.1.4.1"
#define UID_RETIRED_ReferencedImageBoxSOPClass                     "1.2.840.10008.5.1.1.4.2"
#define UID_BasicGrayscalePrintManagementMetaSOPClass              "1.2.840.10008.5.1.1.9"
#define UID_RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass "1.2.840.10008.5.1.1.9.1"
#define UID_PrintJobSOPClass                                       "1.2.840.10008.5.1.1.14"
#define UID_BasicAnnotationBoxSOPClass                             "1.2.840.10008.5.1.1.15"
#define UID_PrinterSOPClass                                        "1.2.840.10008.5.1.1.16"
#define UID_PrinterConfigurationRetrievalSOPClass                  "1.2.840.10008.5.1.1.16.376"
#define UID_PrinterSOPInstance                                     "1.2.840.10008.5.1.1.17"
#define UID_PrinterConfigurationRetrievalSOPInstance               "1.2.840.10008.5.1.1.17.376"
#define UID_BasicColorPrintManagementMetaSOPClass                  "1.2.840.10008.5.1.1.18"
#define UID_RETIRED_ReferencedColorPrintManagementMetaSOPClass     "1.2.840.10008.5.1.1.18.1"
#define UID_VOILUTBoxSOPClass                                      "1.2.840.10008.5.1.1.22"
#define UID_PresentationLUTSOPClass                                "1.2.840.10008.5.1.1.23"
#define UID_ImageOverlayBoxSOPClass                                "1.2.840.10008.5.1.1.24"
#define UID_BasicPrintImageOverlayBoxSOPClass                      "1.2.840.10008.5.1.1.24.1"
#define UID_PrintQueueSOPInstance                                  "1.2.840.10008.5.1.1.25"
#define UID_PrintQueueManagementSOPClass                           "1.2.840.10008.5.1.1.26"
#define UID_PullPrintRequestSOPClass                               "1.2.840.10008.5.1.1.31"
#define UID_PullStoredPrintManagementMetaSOPClass                  "1.2.840.10008.5.1.1.32"

// Storage Commitment
#define UID_StorageCommitmentPushModelSOPClass                     "1.2.840.10008.1.20.1"
#define UID_StorageCommitmentPushModelSOPInstance                  "1.2.840.10008.1.20.1.1"
#define UID_RETIRED_StorageCommitmentPullModelSOPClass             "1.2.840.10008.1.20.2"
#define UID_RETIRED_StorageCommitmentPullModelSOPInstance          "1.2.840.10008.1.20.2.1"

// MPPS
#define UID_ModalityPerformedProcedureStepSOPClass                 "1.2.840.10008.3.1.2.3.3"
#define UID_ModalityPerformedProcedureStepRetrieveSOPClass         "1.2.840.10008.3.1.2.3.4"
#define UID_ModalityPerformedProcedureStepNotificationSOPClass     "1.2.840.10008.3.1.2.3.5"

// Detached Management
#define UID_DetachedPatientManagementSOPClass                      "1.2.840.10008.3.1.2.1.1"
#define UID_DetachedPatientManagementMetaSOPClass                  "1.2.840.10008.3.1.2.1.4"
#define UID_DetachedVisitManagementSOPClass                        "1.2.840.10008.3.1.2.2.1"
#define UID_DetachedStudyManagementSOPClass                        "1.2.840.10008.3.1.2.3.1"
#define UID_DetachedResultsManagementSOPClass                      "1.2.840.10008.3.1.2.5.1"
#define UID_DetachedResultsManagementMetaSOPClass                  "1.2.840.10008.3.1.2.5.4"
#define UID_DetachedStudyManagementMetaSOPClass                    "1.2.840.10008.3.1.2.5.5"
#define UID_DetachedInterpretationManagementSOPClass               "1.2.840.10008.3.1.2.6.1"

// General Purpose Worklist (Supplement 52 final text)
#define UID_GeneralPurposeScheduledProcedureStepSOPClass           "1.2.840.10008.5.1.4.32.2"
#define UID_GeneralPurposePerformedProcedureStepSOPClass           "1.2.840.10008.5.1.4.32.3"
#define UID_GeneralPurposeWorklistManagementMetaSOPClass           "1.2.840.10008.5.1.4.32"

// Other
#define UID_VerificationSOPClass                                   "1.2.840.10008.1.1"
// official name in DICOM standard: Media Storage Directory Storage
#define UID_BasicDirectoryStorageSOPClass                          "1.2.840.10008.1.3.10"
#define UID_BasicStudyContentNotificationSOPClass                  "1.2.840.10008.1.9"
#define UID_StudyComponentManagementSOPClass                       "1.2.840.10008.3.1.2.3.2"

// UID for DICOM Controlled Terminology, defined in CP 324
#define UID_DICOMControlledTerminologyCodingScheme                 "1.2.840.10008.2.16.4"

// Private DCMTK UIDs
// Private SOP Class UID used to shutdown external network applications 
#define UID_PrivateShutdownSOPClass                                "1.2.276.0.7230010.3.4.1915765545.18030.917282194.0"


/* DRAFT SUPPLEMENTS - EXPERIMENTAL USE ONLY */

/*
 * The following UIDs were defined in "frozen draft for trial implementation"
 * versions of various DICOM supplements and changed before final text.
 * Since it is likely that trial implementations exist, we leave the UIDs in the dictionary.
 */

/* Supplement 23 Frozen Draft (November 1997) */
#define UID_DRAFT_SRTextStorage                                    "1.2.840.10008.5.1.4.1.1.88.1"
#define UID_DRAFT_SRAudioStorage                                   "1.2.840.10008.5.1.4.1.1.88.2" 
#define UID_DRAFT_SRDetailStorage                                  "1.2.840.10008.5.1.4.1.1.88.3" 
#define UID_DRAFT_SRComprehensiveStorage                           "1.2.840.10008.5.1.4.1.1.88.4"

/* Supplement 30 Draft 08 for Demonstration (October 1997) */
#define UID_DRAFT_WaveformStorage                                  "1.2.840.10008.5.1.4.1.1.9.1"


#endif /* DCUID_H */
