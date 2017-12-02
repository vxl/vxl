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

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_WINDOWS_H
#include <windows.h>     /* this includes either winsock.h or winsock2.h */
#else
#ifdef HAVE_WINSOCK_H
#include <winsock.h>     /* include winsock.h directly i.e. on MacOS */
#endif
#endif

#ifdef _WIN32
#include <process.h>     /* needed for declaration of getpid() */
#endif

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CTIME
#include "ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_LIBC_H
#include <libc.h>
#endif
#ifndef _WIN32
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#ifndef DCOMPAT_SYS_SOCKET_H_
#define DCOMPAT_SYS_SOCKET_H_
/* some systems don't protect sys/socket.h (e.g. DEC Ultrix) */
#include <sys/socket.h>
#endif
#endif
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
END_EXTERN_C

#include "ofstream.h"
#include "dcuid.h"
#include "ofthread.h"
#include "ofcrc32.h"

struct UIDNameMap {
    const char* uid;
    const char* name;
};

//
// It is very important that the names of the UIDs may not use the following
// characters: space  (  )  [  ], =  <  >

static const UIDNameMap uidNameMap[] = {
    { UID_StandardApplicationContext,                         "StandardApplicationContext" },
    { UID_LittleEndianImplicitTransferSyntax,                 "LittleEndianImplicit" },
    { UID_LittleEndianExplicitTransferSyntax,                 "LittleEndianExplicit" },
    { UID_BigEndianExplicitTransferSyntax,                    "BigEndianExplicit" },
    { UID_JPEGProcess1TransferSyntax,                         "JPEGBaseline" },
    { UID_JPEGProcess2_4TransferSyntax,                       "JPEGExtended:Process2+4" },
    { UID_JPEGProcess3_5TransferSyntax,                       "JPEGExtended:Process3+5" },
    { UID_JPEGProcess6_8TransferSyntax,                       "JPEGSpectralSelection:Non-hierarchical:Process6+8" },
    { UID_JPEGProcess7_9TransferSyntax,                       "JPEGSpectralSelection:Non-hierarchical:Process7+9" },
    { UID_JPEGProcess10_12TransferSyntax,                     "JPEGFullProgression:Non-hierarchical:Process10+12" },
    { UID_JPEGProcess11_13TransferSyntax,                     "JPEGFullProgression:Non-hierarchical:Process11+13" },
    { UID_JPEGProcess14TransferSyntax,                        "JPEGLossless:Non-hierarchical:Process14" },
    { UID_JPEGProcess15TransferSyntax,                        "JPEGLossless:Non-hierarchical:Process15" },
    { UID_JPEGProcess16_18TransferSyntax,                     "JPEGExtended:Hierarchical:Process16+18" },
    { UID_JPEGProcess17_19TransferSyntax,                     "JPEGExtended:Hierarchical:Process17+19" },
    { UID_JPEGProcess20_22TransferSyntax,                     "JPEGSpectralSelection:Hierarchical:Process20+22" },
    { UID_JPEGProcess21_23TransferSyntax,                     "JPEGSpectralSelection:Hierarchical:Process21+23" },
    { UID_JPEGProcess24_26TransferSyntax,                     "JPEGFullProgression:Hierarchical:Process24+26" },
    { UID_JPEGProcess25_27TransferSyntax,                     "JPEGFullProgression:Hierarchical:Process25+27" },
    { UID_JPEGProcess28TransferSyntax,                        "JPEGLossless:Hierarchical:Process28" },
    { UID_JPEGProcess29TransferSyntax,                        "JPEGLossless:Hierarchical:Process29" },
    { UID_JPEGProcess14SV1TransferSyntax,                     "JPEGLossless:Non-hierarchical-1stOrderPrediction" },
    { UID_JPEGLSLossless,                                     "JPEGLSLossless" },
    { UID_JPEGLSLossy,                                        "JPEGLSLossy" },
    { UID_RLELossless,                                        "RLELossless" },
    { UID_DeflatedExplicitVRLittleEndianTransferSyntax,       "DeflatedLittleEndianExplicit" },
    { UID_JPEG2000LosslessOnlyTransferSyntax,                 "JPEG2000LosslessOnly" },
    { UID_JPEG2000TransferSyntax,                             "JPEG2000" },

    // Storage
    { UID_AmbulatoryECGWaveformStorage,                       "AmbulatoryECGWaveformStorage" },
    { UID_BasicTextSR,                                        "BasicTextSR" },
    { UID_BasicVoiceAudioWaveformStorage,                     "BasicVoiceAudioWaveformStorage" },
    { UID_CTImageStorage,                                     "CTImageStorage" },
    { UID_CardiacElectrophysiologyWaveformStorage,            "CardiacElectrophysiologyWaveformStorage" },
    { UID_ComprehensiveSR,                                    "ComprehensiveSR" },
    { UID_ComputedRadiographyImageStorage,                    "ComputedRadiographyImageStorage" },
    { UID_DigitalIntraOralXRayImageStorageForPresentation,    "DigitalIntraOralXRayImageStorageForPresentation" },
    { UID_DigitalIntraOralXRayImageStorageForProcessing,      "DigitalIntraOralXRayImageStorageForProcessing" },
    { UID_DigitalMammographyXRayImageStorageForPresentation,  "DigitalMammographyXRayImageStorageForPresentation" },
    { UID_DigitalMammographyXRayImageStorageForProcessing,    "DigitalMammographyXRayImageStorageForProcessing" },
    { UID_DigitalXRayImageStorageForPresentation,             "DigitalXRayImageStorageForPresentation" },
    { UID_DigitalXRayImageStorageForProcessing,               "DigitalXRayImageStorageForProcessing" },
    { UID_EnhancedMRImageStorage,                             "EnhancedMRImageStorage" },
    { UID_EnhancedSR,                                         "EnhancedSR" },
    { UID_GeneralECGWaveformStorage,                          "GeneralECGWaveformStorage" },
    { UID_GrayscaleSoftcopyPresentationStateStorage,          "GrayscaleSoftcopyPresentationStateStorage" },
    { UID_HardcopyColorImageStorage,                          "HardcopyColorImageStorage" },
    { UID_HardcopyGrayscaleImageStorage,                      "HardcopyGrayscaleImageStorage" },
    { UID_HemodynamicWaveformStorage,                         "HemodynamicWaveformStorage" },
    { UID_KeyObjectSelectionDocument,                         "KeyObjectSelectionDocument" },
    { UID_MRImageStorage,                                     "MRImageStorage" },
    { UID_MRSpectroscopyStorage,                              "MRSpectroscopyStorage" },
    { UID_MammographyCADSR,                                   "MammographyCADSR" },
    { UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage, "MultiframeGrayscaleByteSecondaryCaptureImageStorage" },
    { UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage, "MultiframeGrayscaleWordSecondaryCaptureImageStorage" },
    { UID_MultiframeSingleBitSecondaryCaptureImageStorage,    "MultiframeSingleBitSecondaryCaptureImageStorage" },
    { UID_MultiframeTrueColorSecondaryCaptureImageStorage,    "MultiframeTrueColorSecondaryCaptureImageStorage" },
    { UID_NuclearMedicineImageStorage,                        "NuclearMedicineImageStorage" },
    { UID_PETCurveStorage,                                    "PETCurveStorage" },
    { UID_PETImageStorage,                                    "PETImageStorage" },
    { UID_RETIRED_NuclearMedicineImageStorage,                "RETIRED_NuclearMedicineImageStorage" },
    { UID_RETIRED_UltrasoundImageStorage,                     "RETIRED_UltrasoundImageStorage" },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,           "RETIRED_UltrasoundMultiframeImageStorage" },
    { UID_RETIRED_VLImageStorage,                             "RETIRED_VLImageStorage" },
    { UID_RETIRED_VLMultiFrameImageStorage,                   "RETIRED_VLMultiFrameImageStorage" },
    { UID_RETIRED_XRayAngiographicBiPlaneImageStorage,        "RETIRED_XRayAngiographicBiPlaneImageStorage" },
    { UID_RTBeamsTreatmentRecordStorage,                      "RTBeamsTreatmentRecordStorage" },
    { UID_RTBrachyTreatmentRecordStorage,                     "RTBrachyTreatmentRecordStorage" },
    { UID_RTDoseStorage,                                      "RTDoseStorage" },
    { UID_RTImageStorage,                                     "RTImageStorage" },
    { UID_RTPlanStorage,                                      "RTPlanStorage" },
    { UID_RTStructureSetStorage,                              "RTStructureSetStorage" },
    { UID_RTTreatmentSummaryRecordStorage,                    "RTTreatmentSummaryRecordStorage" },
    { UID_RawDataStorage,                                     "RawDataStorage" },
    { UID_SecondaryCaptureImageStorage,                       "SecondaryCaptureImageStorage" },
    { UID_StandaloneCurveStorage,                             "StandaloneCurveStorage" },
    { UID_StandaloneModalityLUTStorage,                       "StandaloneModalityLUTStorage" },
    { UID_StandaloneOverlayStorage,                           "StandaloneOverlayStorage" },
    { UID_StandaloneVOILUTStorage,                            "StandaloneVOILUTStorage" },
    { UID_StoredPrintStorage,                                 "StoredPrintStorage" },
    { UID_TwelveLeadECGWaveformStorage,                       "TwelveLeadECGWaveformStorage" },
    { UID_UltrasoundImageStorage,                             "UltrasoundImageStorage" },
    { UID_UltrasoundMultiframeImageStorage,                   "UltrasoundMultiframeImageStorage" },
    { UID_VLEndoscopicImageStorage,                           "VLEndoscopicImageStorage" },
    { UID_VLMicroscopicImageStorage,                          "VLMicroscopicImageStorage" },
    { UID_VLPhotographicImageStorage,                         "VLPhotographicImageStorage" },
    { UID_VLSlideCoordinatesMicroscopicImageStorage,          "VLSlideCoordinatesMicroscopicImageStorage" },
    { UID_XRayAngiographicImageStorage,                       "XRayAngiographicImageStorage" },
    { UID_XRayFluoroscopyImageStorage,                        "XRayFluoroscopyImageStorage" },

    // Query/Retrieve
    { UID_FINDModalityWorklistInformationModel,               "FINDModalityWorklistInformationModel" },
    { UID_FINDPatientRootQueryRetrieveInformationModel,       "FINDPatientRootQueryRetrieveInformationModel" },
    { UID_FINDPatientStudyOnlyQueryRetrieveInformationModel,  "FINDPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_FINDStudyRootQueryRetrieveInformationModel,         "FINDStudyRootQueryRetrieveInformationModel" },
    { UID_GETPatientRootQueryRetrieveInformationModel,        "GETPatientRootQueryRetrieveInformationModel" },
    { UID_GETPatientStudyOnlyQueryRetrieveInformationModel,   "GETPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_GETStudyRootQueryRetrieveInformationModel,          "GETStudyRootQueryRetrieveInformationModel" },
    { UID_MOVEPatientRootQueryRetrieveInformationModel,       "MOVEPatientRootQueryRetrieveInformationModel" },
    { UID_MOVEPatientStudyOnlyQueryRetrieveInformationModel,  "MOVEPatientStudyOnlyQueryRetrieveInformationModel" },
    { UID_MOVEStudyRootQueryRetrieveInformationModel,         "MOVEStudyRootQueryRetrieveInformationModel" },
    { UID_FINDGeneralPurposeWorklistInformationModel,         "FINDGeneralPurposeWorklistInformationModel" },

    // Print
    { UID_BasicAnnotationBoxSOPClass,                         "BasicAnnotationBoxSOPClass" },
    { UID_BasicColorImageBoxSOPClass,                         "BasicColorImageBoxSOPClass" },
    { UID_BasicColorPrintManagementMetaSOPClass,              "BasicColorPrintManagementMetaSOPClass" },
    { UID_BasicFilmBoxSOPClass,                               "BasicFilmBoxSOPClass" },
    { UID_BasicFilmSessionSOPClass,                           "BasicFilmSessionSOPClass" },
    { UID_BasicGrayscaleImageBoxSOPClass,                     "BasicGrayscaleImageBoxSOPClass" },
    { UID_BasicGrayscalePrintManagementMetaSOPClass,          "BasicGrayscalePrintManagementMetaSOPClass" },
    { UID_BasicPrintImageOverlayBoxSOPClass,                  "BasicPrintImageOverlayBoxSOPClass" },
    { UID_ImageOverlayBoxSOPClass,                            "ImageOverlayBoxSOPClass" },
    { UID_PresentationLUTSOPClass,                            "PresentationLUTSOPClass" },
    { UID_PrintJobSOPClass,                                   "PrintJobSOPClass" },
    { UID_PrintQueueManagementSOPClass,                       "PrintQueueManagementSOPClass" },
    { UID_PrintQueueSOPInstance,                              "PrintQueueSOPInstance" },
    { UID_PrinterConfigurationRetrievalSOPClass,              "PrinterConfigurationRetrievalSOPClass" },
    { UID_PrinterConfigurationRetrievalSOPInstance,           "PrinterConfigurationRetrievalSOPInstance" },
    { UID_PrinterSOPClass,                                    "PrinterSOPClass" },
    { UID_PrinterSOPInstance,                                 "PrinterSOPInstance" },
    { UID_PullPrintRequestSOPClass,                           "PullPrintRequestSOPClass" },
    { UID_PullStoredPrintManagementMetaSOPClass,              "PullStoredPrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedColorPrintManagementMetaSOPClass, "RETIRED_ReferencedColorPrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass, "RETIRED_ReferencedGrayscalePrintManagementMetaSOPClass" },
    { UID_RETIRED_ReferencedImageBoxSOPClass,                  "RETIRED_ReferencedImageBoxSOPClass" },
    { UID_VOILUTBoxSOPClass,                                  "VOILUTBoxSOPClass" },

    // Storage Commitment
    { UID_RETIRED_StorageCommitmentPullModelSOPClass,         "RETIRED_StorageCommitmentPullModelSOPClass" },
    { UID_RETIRED_StorageCommitmentPullModelSOPInstance,      "RETIRED_StorageCommitmentPullModelSOPInstance" },
    { UID_StorageCommitmentPushModelSOPClass,                 "StorageCommitmentPushModelSOPClass" },
    { UID_StorageCommitmentPushModelSOPInstance,              "StorageCommitmentPushModelSOPInstance" },

    // MPPS
    { UID_ModalityPerformedProcedureStepNotificationSOPClass, "ModalityPerformedProcedureStepNotificationSOPClass" },
    { UID_ModalityPerformedProcedureStepRetrieveSOPClass,     "ModalityPerformedProcedureStepRetrieveSOPClass" },
    { UID_ModalityPerformedProcedureStepSOPClass,             "ModalityPerformedProcedureStepSOPClass" },

    // Detached Management
    { UID_DetachedInterpretationManagementSOPClass,           "DetachedInterpretationManagementSOPClass" },
    { UID_DetachedPatientManagementMetaSOPClass,              "DetachedPatientManagementMetaSOPClass" },
    { UID_DetachedPatientManagementSOPClass,                  "DetachedPatientManagementSOPClass" },
    { UID_DetachedResultsManagementMetaSOPClass,              "DetachedResultsManagementMetaSOPClass" },
    { UID_DetachedResultsManagementSOPClass,                  "DetachedResultsManagementSOPClass" },
    { UID_DetachedStudyManagementMetaSOPClass,                "DetachedStudyManagementMetaSOPClass" },
    { UID_DetachedStudyManagementSOPClass,                    "DetachedStudyManagementSOPClass" },
    { UID_DetachedVisitManagementSOPClass,                    "DetachedVisitManagementSOPClass" },

    // General Purpose Worklist (Supplement 52 final text)
    { UID_GeneralPurposeScheduledProcedureStepSOPClass,       "GeneralPurposeScheduledProcedureStepSOPClass" },
    { UID_GeneralPurposePerformedProcedureStepSOPClass,       "GeneralPurposePerformedProcedureStepSOPClass" },
    { UID_GeneralPurposeWorklistManagementMetaSOPClass,       "GeneralPurposeWorklistManagementMetaSOPClass" },

    // other
    { UID_BasicDirectoryStorageSOPClass,                      "BasicDirectoryStorageSOPClass" },
    { UID_BasicStudyContentNotificationSOPClass,              "BasicStudyContentNotificationSOPClass" },
    { UID_StudyComponentManagementSOPClass,                   "StudyComponentManagementSOPClass" },
    { UID_VerificationSOPClass,                               "VerificationSOPClass" },

    // DICOM Controlled Terminology
    { UID_DICOMControlledTerminologyCodingScheme,             "DICOMControlledTerminologyCodingScheme" },

    // supplements
    { UID_DRAFT_SRTextStorage,                                "DRAFT_SRTextStorage" },
    { UID_DRAFT_SRAudioStorage,                               "DRAFT_SRAudioStorage" },
    { UID_DRAFT_SRDetailStorage,                              "DRAFT_SRDetailStorage" },
    { UID_DRAFT_SRComprehensiveStorage,                       "DRAFT_SRComprehensiveStorage" },
    { UID_DRAFT_WaveformStorage,                              "DRAFT_WaveformStorage" },

    { NULL, NULL }
};

static const int uidNameMap_size = ( sizeof(uidNameMap) / sizeof(UIDNameMap) );

/*
** The global variable dcmStorageSOPClassUIDs is an array of
** string pointers containing the UIDs of all known Storage SOP
** Classes.  The global variable numberOfDcmStorageSOPClassUIDs
** defines the size of the array.
*/

const char* dcmStorageSOPClassUIDs[] = {
    // basic directory storage is a special case - we cannot
    // transmit a DICOMDIR by c-store because of the absolute
    // file position offsets within the DICOMDIR.
    // UID_BasicDirectoryStorageSOPClass,

    UID_AmbulatoryECGWaveformStorage,
    UID_BasicTextSR,
    UID_BasicVoiceAudioWaveformStorage,
    UID_CTImageStorage,
    UID_CardiacElectrophysiologyWaveformStorage,
    UID_ComprehensiveSR,
    UID_ComputedRadiographyImageStorage,
/* disabled draft storage SOP classes to keep the number of storage transfer
 * syntaxes <= 64.  If we have more than 64 storage transfer syntaxes, tools
 * such as storescu will fail because they attempt to negotiate two
 * presentation contexts for each SOP class, and there is a total limit of
 * 128 contexts for one association.
 *
    UID_DRAFT_SRAudioStorage,
    UID_DRAFT_SRComprehensiveStorage,
    UID_DRAFT_SRDetailStorage,
    UID_DRAFT_SRTextStorage,
    UID_DRAFT_WaveformStorage,
 */
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_EnhancedMRImageStorage,
    UID_EnhancedSR,
    UID_GeneralECGWaveformStorage,
    UID_GrayscaleSoftcopyPresentationStateStorage,
    UID_HardcopyColorImageStorage,
    UID_HardcopyGrayscaleImageStorage,
    UID_HemodynamicWaveformStorage,
    UID_KeyObjectSelectionDocument,
    UID_MRImageStorage,
    UID_MRSpectroscopyStorage,
    UID_MammographyCADSR,
    UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    UID_MultiframeSingleBitSecondaryCaptureImageStorage,
    UID_MultiframeTrueColorSecondaryCaptureImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_PETCurveStorage,
    UID_PETImageStorage,
    UID_RETIRED_NuclearMedicineImageStorage,
    UID_RETIRED_UltrasoundImageStorage,
    UID_RETIRED_UltrasoundMultiframeImageStorage,
/* disabled draft storage SOP classes to keep the number of storage transfer
 * syntaxes <= 64.  If we have more than 64 storage transfer syntaxes, tools
 * such as storescu will fail because they attempt to negotiate two
 * presentation contexts for each SOP class, and there is a total limit of
 * 128 contexts for one association.
 *
    UID_RETIRED_VLImageStorage,
    UID_RETIRED_VLMultiFrameImageStorage,
 */
    UID_RETIRED_XRayAngiographicBiPlaneImageStorage,
    UID_RTBeamsTreatmentRecordStorage,
    UID_RTBrachyTreatmentRecordStorage,
    UID_RTDoseStorage,
    UID_RTImageStorage,
    UID_RTPlanStorage,
    UID_RTStructureSetStorage,
    UID_RTTreatmentSummaryRecordStorage,
    UID_RawDataStorage,
    UID_SecondaryCaptureImageStorage,
    UID_StandaloneCurveStorage,
    UID_StandaloneModalityLUTStorage,
    UID_StandaloneOverlayStorage,
    UID_StandaloneVOILUTStorage,
    UID_StoredPrintStorage,
    UID_TwelveLeadECGWaveformStorage,
    UID_UltrasoundImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_VLEndoscopicImageStorage,
    UID_VLMicroscopicImageStorage,
    UID_VLPhotographicImageStorage,
    UID_VLSlideCoordinatesMicroscopicImageStorage,
    UID_XRayAngiographicImageStorage,
    UID_XRayFluoroscopyImageStorage,

    NULL
};

const int numberOfDcmStorageSOPClassUIDs =
    (sizeof(dcmStorageSOPClassUIDs) / sizeof(const char*)) - 1;


/*
** The global variable dcmImageSOPClassUIDs is an array of
** string pointers containing the UIDs of all known Image SOP
** Classes.  The instances of SOP Classes in this list can be
** referenced from DICOMDIR IMAGE records.
**
** The dcmgpdir program uses this list to determine what kind of
** objects can be referenced from IMAGE records.
** Be _very_ careful when adding SOP Classes to this list!!
**
** The global variable numberOfDcmImageSOPClassUIDs
** defines the size of the array.
** NOTE: this list represents a subset of the dcmStorageSOPClassUIDs list
*/

const char* dcmImageSOPClassUIDs[] = {
    UID_CTImageStorage,
    UID_ComputedRadiographyImageStorage,
    UID_DigitalIntraOralXRayImageStorageForPresentation,
    UID_DigitalIntraOralXRayImageStorageForProcessing,
    UID_DigitalMammographyXRayImageStorageForPresentation,
    UID_DigitalMammographyXRayImageStorageForProcessing,
    UID_DigitalXRayImageStorageForPresentation,
    UID_DigitalXRayImageStorageForProcessing,
    UID_EnhancedMRImageStorage,
    UID_HardcopyColorImageStorage,
    UID_HardcopyGrayscaleImageStorage,
    UID_MRImageStorage,
    UID_NuclearMedicineImageStorage,
    UID_PETCurveStorage,
    UID_PETImageStorage,
    UID_RETIRED_NuclearMedicineImageStorage,
    UID_RETIRED_UltrasoundImageStorage,
    UID_RETIRED_UltrasoundMultiframeImageStorage,
    UID_RETIRED_VLImageStorage,
    UID_RETIRED_VLMultiFrameImageStorage,
    UID_RETIRED_XRayAngiographicBiPlaneImageStorage,
    UID_RTImageStorage,
    UID_SecondaryCaptureImageStorage,
    UID_UltrasoundImageStorage,
    UID_UltrasoundMultiframeImageStorage,
    UID_VLEndoscopicImageStorage,
    UID_VLMicroscopicImageStorage,
    UID_VLPhotographicImageStorage,
    UID_VLSlideCoordinatesMicroscopicImageStorage,
    UID_XRayAngiographicImageStorage,
    UID_XRayFluoroscopyImageStorage,
    UID_MultiframeSingleBitSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    UID_MultiframeTrueColorSecondaryCaptureImageStorage,

    NULL
};

const int numberOfDcmImageSOPClassUIDs =
    (sizeof(dcmImageSOPClassUIDs) / sizeof(const char*)) - 1;


typedef struct {
    const char *sopClass;
    const char *modality;
    unsigned long averageSize;  /* can be way, way out */
} DcmModalityTable;

/*
** The modalities table defines a short character code for each
** Storage SOP Class for use in filenames.
** It also gives a typical size for each SOP Instance.  This will
** ususally be way out, but is useful in user interfaces to give an
** idea of progress when receiving an image (C-STORE does not indicate
** the size of an image being transmitted).
*/
static const DcmModalityTable modalities[] = {
    { UID_AmbulatoryECGWaveformStorage,                        "ECA", 4096 },
    { UID_BasicTextSR,                                         "SRt", 4096 },
    { UID_BasicVoiceAudioWaveformStorage,                      "AUV", 4096 },
    { UID_CTImageStorage,                                      "CT",  2 *  512 *  512 },
    { UID_CardiacElectrophysiologyWaveformStorage,             "WVc", 4096 },
    { UID_ComprehensiveSR,                                     "SRc", 4096 },
    { UID_ComputedRadiographyImageStorage,                     "CR",  2 * 2048 * 2048 },
    { UID_DRAFT_SRAudioStorage,                                "SRw", 4096 },
    { UID_DRAFT_SRComprehensiveStorage,                        "SRx", 4096 },
    { UID_DRAFT_SRDetailStorage,                               "SRy", 4096 },
    { UID_DRAFT_SRTextStorage,                                 "SRz", 4096 },
    { UID_DRAFT_WaveformStorage,                               "WVd", 4096 },
    { UID_DigitalIntraOralXRayImageStorageForPresentation,     "DXo", 2 * 1024 * 1024 },
    { UID_DigitalIntraOralXRayImageStorageForProcessing,       "DPo", 2 * 1024 * 1024 },
    { UID_DigitalMammographyXRayImageStorageForPresentation,   "DXm", 2 * 4096 * 4096 },
    { UID_DigitalMammographyXRayImageStorageForProcessing,     "DPm", 2 * 4096 * 4096 },
    { UID_DigitalXRayImageStorageForPresentation,              "DX",  2 * 2048 * 2048 },
    { UID_DigitalXRayImageStorageForProcessing,                "DP",  2 * 2048 * 2048 },
    { UID_EnhancedMRImageStorage,                              "MRe", 512 * 512 * 256 },
    { UID_EnhancedSR,                                          "SRe", 4096 },
    { UID_GeneralECGWaveformStorage,                           "ECG", 4096 },
    { UID_GrayscaleSoftcopyPresentationStateStorage,           "PSg", 4096 },
    { UID_HardcopyColorImageStorage,                           "HC",  4096 },
    { UID_HardcopyGrayscaleImageStorage,                       "HG",  4096 },
    { UID_HemodynamicWaveformStorage,                          "WVh", 4096 },
    { UID_KeyObjectSelectionDocument,                          "SRk", 4096 },
    { UID_MRImageStorage,                                      "MR",  2 * 256 * 256 },
    { UID_MRSpectroscopyStorage,                               "MRs", 512 * 512 * 256 },
    { UID_MammographyCADSR,                                    "SRm", 4096 },
    { UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage, "SCb",  1 * 512 * 512 },
    { UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage, "SCw",  2 * 512 * 512 },
    { UID_MultiframeSingleBitSecondaryCaptureImageStorage,     "SCs",  1024 * 1024 },  /* roughly an A4 300dpi scan */
    { UID_MultiframeTrueColorSecondaryCaptureImageStorage,     "SCc",  3 * 512 * 512 },
    { UID_NuclearMedicineImageStorage,                         "NM",  2 * 64 * 64 },
    { UID_PETCurveStorage,                                     "PC",  4096 },
    { UID_PETImageStorage,                                     "PI",  512*512*2 },
    { UID_RETIRED_NuclearMedicineImageStorage,                 "NMr", 2 * 64 * 64 },
    { UID_RETIRED_UltrasoundImageStorage,                      "USr", 1 * 512 * 512 },
    { UID_RETIRED_UltrasoundMultiframeImageStorage,            "USr", 1 * 512 * 512 },
    { UID_RETIRED_VLImageStorage,                              "VLr", 768 * 576 * 3 },
    { UID_RETIRED_VLMultiFrameImageStorage,                    "VMr", 768 * 576 * 3 },
    { UID_RETIRED_XRayAngiographicBiPlaneImageStorage,         "XB",  2 * 512 * 512 },
    { UID_RTBeamsTreatmentRecordStorage,                       "RTb", 4096 },
    { UID_RTBrachyTreatmentRecordStorage,                      "RTr", 4096 },
    { UID_RTDoseStorage,                                       "RD",  4096 },
    { UID_RTImageStorage,                                      "RI",  4096 },
    { UID_RTPlanStorage,                                       "RP" , 4096 },
    { UID_RTStructureSetStorage,                               "RS",  4096 },
    { UID_RTTreatmentSummaryRecordStorage,                     "RTs", 4096 },
    { UID_RawDataStorage,                                      "RAW", 512 * 512 * 256 },
    { UID_SecondaryCaptureImageStorage,                        "SC",  2 * 512 * 512 },
    { UID_StandaloneCurveStorage,                              "CV",  4096 },
    { UID_StandaloneModalityLUTStorage,                        "ML",  4096*2 },
    { UID_StandaloneOverlayStorage,                            "OV",  512 * 512 },
    { UID_StandaloneVOILUTStorage,                             "VO",  4096*2 },
    { UID_StoredPrintStorage,                                  "SP",  4096 },
    { UID_TwelveLeadECGWaveformStorage,                        "TLE", 4096 },
    { UID_UltrasoundImageStorage,                              "US",  1 * 512 * 512 },
    { UID_UltrasoundMultiframeImageStorage,                    "US",  1 * 512 * 512 },
    { UID_VLEndoscopicImageStorage,                            "VLe", 768 * 576 * 3 },
    { UID_VLMicroscopicImageStorage,                           "VLm", 768 * 576 * 3 },
    { UID_VLPhotographicImageStorage,                          "VLp", 768 * 576 * 3 },
    { UID_VLSlideCoordinatesMicroscopicImageStorage,           "VMs", 768 * 576 * 3 },
    { UID_XRayAngiographicImageStorage,                        "XA",  2 * 512 * 512 },
    { UID_XRayFluoroscopyImageStorage,                         "RF",  2 * 512 * 512 }
};

static const int numberOfDcmModalityTableEntries =
    (sizeof(modalities) / sizeof(DcmModalityTable));


/*
 * Public Function Prototypes
 */


const char *dcmSOPClassUIDToModality(const char *sopClassUID)
{
    if (sopClassUID == NULL) return NULL;
    for (int i = 0; i < numberOfDcmModalityTableEntries; i++)
    {
      if (strcmp(modalities[i].sopClass, sopClassUID) == 0) return modalities[i].modality;
    }
    return NULL;
}

unsigned long dcmGuessModalityBytes(const char *sopClassUID)
{
    unsigned long nbytes = 1048576; /* default: 1 MByte */

    if (sopClassUID == NULL) return nbytes;

    int found=0;
    for (int i = 0; (!found && (i < numberOfDcmModalityTableEntries)); i++)
    {
    found = (strcmp(modalities[i].sopClass, sopClassUID) == 0);
    if (found) nbytes = modalities[i].averageSize;
    }

    return nbytes;
}


/*
** dcmFindNameOfUID(const char* uid)
** Return the name of a UID.
** Performs a table lookup and returns a pointer to a read-only string.
** Returns NULL of the UID is not known.
*/

const char*
dcmFindNameOfUID(const char* uid)
{
    int i = 0;
    if (uid == NULL) return NULL;
    for (i=0; i<uidNameMap_size; i++) {
        if (uidNameMap[i].uid != NULL && strcmp(uid, uidNameMap[i].uid) == 0) {
            return uidNameMap[i].name;
        }
    }
    return NULL;
}

//
// dcmFindUIDFromName(const char* name)
// Return the UID of a name.
// Performs a table lookup and returns a pointer to a read-only string.
// Returns NULL of the name is not known.
//

const char *
dcmFindUIDFromName(const char * name)
{
    if (name == NULL) return NULL;
    for (int i = 0; i < uidNameMap_size; i++)
    {
        if (uidNameMap[i].name != NULL && strcmp(name, uidNameMap[i].name) == 0)
            return uidNameMap[i].uid;
    }
    return NULL;
}


/*
** dcmIsaStorageSOPClassUID(const char* uid)
** Returns true if the uid is one of the Storage SOP Classes.
** Performs a table lookup in the dcmStorageSOPClassUIDs table.
*/
OFBool
dcmIsaStorageSOPClassUID(const char* uid)
{
    int i = 0;
    if (uid == NULL) return OFFalse;
    for (i=0; i<numberOfDcmStorageSOPClassUIDs; i++) {
        if (dcmStorageSOPClassUIDs[i] != NULL
        && strcmp(uid, dcmStorageSOPClassUIDs[i]) == 0) {
            return OFTrue;
        }
    }
    return OFFalse;
}

// ********************************

#ifndef HAVE_GETHOSTID
#ifdef HAVE_SYSINFO

#include <sys/systeminfo.h>
static long gethostid(void)
{
    char buf[128];
    if (sysinfo(SI_HW_SERIAL, buf, 128) == -1) {
       ofConsole.lockCerr() << "sysinfo: " << strerror(errno) << endl;
       ofConsole.unlockCerr();
       exit(1);
    }
#ifdef HAVE_STRTOUL
    return(strtoul(buf, NULL, 0));
#else
    long result;
    sscanf(buf, "%ld", &result);
    return result;
#endif
}

#else // !HAVE_SYSINFO

/*
** There is no implementation of gethostid() and we cannot implement it in
** terms of sysinfo() so define a workaround.
*/
#if (defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME)) || defined(HAVE_WINDOWS_H)

// 16K should be large enough to handle everything pointed to by a struct hostent
#define GETHOSTBYNAME_R_BUFSIZE 16384

/* on Windows systems specify a routine to determine the MAC address of the Ethernet adapter */
#ifdef HAVE_WINDOWS_H

#include <snmp.h>

typedef int(WINAPI *pSnmpUtilOidCpy) (
        OUT AsnObjectIdentifier *pOidDst,
        IN AsnObjectIdentifier *pOidSrc);

typedef int(WINAPI *pSnmpUtilOidNCmp) (
        IN AsnObjectIdentifier *pOid1,
        IN AsnObjectIdentifier *pOid2,
        IN UINT nSubIds);

typedef void(WINAPI *pSnmpUtilVarBindFree) (
        IN OUT SnmpVarBind *pVb);

typedef bool(WINAPI *pSnmpExtensionInit) (
        IN DWORD dwTimeZeroReference,
        OUT HANDLE *hPollForTrapEvent,
        OUT AsnObjectIdentifier *supportedView);

typedef bool(WINAPI *pSnmpExtensionTrap) (
        OUT AsnObjectIdentifier *enterprise,
        OUT AsnInteger32 *genericTrap,
        OUT AsnInteger32 *specificTrap,
        OUT AsnTimeticks *timeStamp,
        OUT SnmpVarBindList *variableBindings);

typedef bool(WINAPI *pSnmpExtensionQuery) (
        IN BYTE requestType,
        IN OUT SnmpVarBindList *variableBindings,
        OUT AsnInteger32 *errorStatus,
        OUT AsnInteger32 *errorIndex);

typedef bool(WINAPI *pSnmpExtensionInitEx) (
        OUT AsnObjectIdentifier *supportedView);

typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER    NameBuff[30];
} ASTAT, *PASTAT;

/* get the MAC address of the (first) Ethernet adapter (6 bytes) */
static unsigned char *getMACAddress(unsigned char buffer[6])
{
    OFBool success = OFFalse;
    /* init return variable */
    memset(buffer, 0, sizeof(buffer));
    NCB ncb;
    memset(&ncb, 0, sizeof(ncb));
    /* reset the LAN adapter */
    ncb.ncb_command = NCBRESET;
    /* it is considered bad practice to hardcode the LANA number (should enumerate
       adapters first), but at least this approach also works on Windows 9x */
    ncb.ncb_lana_num = 0;
    if (Netbios(&ncb) == NRC_GOODRET)
    {
        ASTAT Adapter;
        /* prepare to get the adapter status block */
        memset(&ncb, 0, sizeof(ncb));
        ncb.ncb_command = NCBASTAT;
        /* it is considered bad practice to hardcode the LANA number (should enumerate
           adapters first), but at least this approach also works on Windows 9x */
        ncb.ncb_lana_num = 0;
        strcpy((char *)ncb.ncb_callname, "*");
        ncb.ncb_buffer = (unsigned char *)&Adapter;
        ncb.ncb_length = sizeof(Adapter);
        /* get the adapter's info */
        if (Netbios(&ncb) == 0)
        {
            /* store the MAC address */
            buffer[0] = Adapter.adapt.adapter_address[0];
            buffer[1] = Adapter.adapt.adapter_address[1];
            buffer[2] = Adapter.adapt.adapter_address[2];
            buffer[3] = Adapter.adapt.adapter_address[3];
            buffer[4] = Adapter.adapt.adapter_address[4];
            buffer[5] = Adapter.adapt.adapter_address[5];
            success = OFTrue;
        }
    }
    /* check whether NetBIOS routines succeeded, if not try the SNMP approach */
    if (!success)
    {
        HINSTANCE m_hInst1, m_hInst2;
        /* load the "SNMP Utility Library" dll and get the addresses of the functions necessary */
        m_hInst1 = LoadLibrary("snmpapi.dll");
        if (m_hInst1 >= (HINSTANCE)HINSTANCE_ERROR)
        {
            pSnmpUtilOidCpy m_Copy = (pSnmpUtilOidCpy)GetProcAddress(m_hInst1, "SnmpUtilOidCpy");
            pSnmpUtilOidNCmp m_Compare = (pSnmpUtilOidNCmp)GetProcAddress(m_hInst1, "SnmpUtilOidNCmp");
            pSnmpUtilVarBindFree m_BindFree = (pSnmpUtilVarBindFree)GetProcAddress(m_hInst1, "SnmpUtilVarBindFree");
            /* load the "SNMP Internet MIB" dll and get the addresses of the functions necessary */
            m_hInst2 = LoadLibrary("inetmib1.dll");
            if (m_hInst2 >= (HINSTANCE)HINSTANCE_ERROR)
            {
                HANDLE PollForTrapEvent;
                AsnObjectIdentifier SupportedView;
                UINT OID_ifEntryType[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 3};
                UINT OID_ifEntryNum[] = {1, 3, 6, 1, 2, 1, 2, 1};
                UINT OID_ipMACEntAddr[] = {1, 3, 6, 1, 2, 1, 2, 2, 1, 6};
                AsnObjectIdentifier MIB_ifMACEntAddr = {sizeof(OID_ipMACEntAddr) / sizeof(UINT), OID_ipMACEntAddr};
                AsnObjectIdentifier MIB_ifEntryType = {sizeof(OID_ifEntryType) / sizeof(UINT), OID_ifEntryType};
                AsnObjectIdentifier MIB_ifEntryNum = {sizeof(OID_ifEntryNum) / sizeof(UINT), OID_ifEntryNum};
                SnmpVarBindList varBindList;
                SnmpVarBind varBind[2];
                AsnInteger32 errorStatus;
                AsnInteger32 errorIndex;
                AsnObjectIdentifier MIB_NULL = {0, 0};
                int ret;
                int dtmp;
                int i = 0, j = 0;
                bool found = false;
                pSnmpExtensionInit m_Init = (pSnmpExtensionInit)GetProcAddress(m_hInst2, "SnmpExtensionInit");
                pSnmpExtensionInitEx m_InitEx = (pSnmpExtensionInitEx)GetProcAddress(m_hInst2, "SnmpExtensionInitEx");
                pSnmpExtensionQuery m_Query = (pSnmpExtensionQuery)GetProcAddress(m_hInst2, "SnmpExtensionQuery");
                pSnmpExtensionTrap m_Trap = (pSnmpExtensionTrap)GetProcAddress(m_hInst2, "SnmpExtensionTrap");
                m_Init(GetTickCount(), &PollForTrapEvent, &SupportedView);
                /* initialize the variable list to be retrieved by m_Query */
                varBindList.list = varBind;
                varBind[0].name = MIB_NULL;
                varBind[1].name = MIB_NULL;
                /* copy in the OID to find the number of entries in the interface table */
                varBindList.len = 1;        /* only retrieving one item */
                m_Copy(&varBind[0].name, &MIB_ifEntryNum);
                ret = m_Query(SNMP_PDU_GETNEXT, &varBindList, &errorStatus, &errorIndex);
                varBindList.len = 2;
                /* copy in the OID of ifType, the type of interface */
                m_Copy(&varBind[0].name, &MIB_ifEntryType);
                /* copy in the OID of ifPhysAddress, the address */
                m_Copy(&varBind[1].name, &MIB_ifMACEntAddr);
                do {
                    /* Submit the query.  Responses will be loaded into varBindList.
                       We can expect this call to succeed a # of times corresponding
                       to the # of adapters reported to be in the system */
                    ret = m_Query(SNMP_PDU_GETNEXT, &varBindList, &errorStatus, &errorIndex);
                    if (!ret)
                        ret = 1;
                    else
                    {
                        /* confirm that the proper type has been returned */
                        ret = m_Compare(&varBind[0].name, &MIB_ifEntryType, MIB_ifEntryType.idLength);
                    }
                    if (!ret)
                    {
                        j++;
                        dtmp = varBind[0].value.asnValue.number;
                        /* type 6 describes ethernet interfaces */
                        if (dtmp == 6)
                        {
                            /* confirm that we have an address here */
                            ret = m_Compare(&varBind[1].name, &MIB_ifMACEntAddr,MIB_ifMACEntAddr.idLength);
                            if ((!ret) && (varBind[1].value.asnValue.address.stream != NULL))
                            {
                                if ((varBind[1].value.asnValue.address.stream[0] == 0x44) &&
                                    (varBind[1].value.asnValue.address.stream[1] == 0x45) &&
                                    (varBind[1].value.asnValue.address.stream[2] == 0x53) &&
                                    (varBind[1].value.asnValue.address.stream[3] == 0x54) &&
                                    (varBind[1].value.asnValue.address.stream[4] == 0x00))
                                {
                                    /* ignore all dial-up networking adapters */
                                    continue;
                                }
                                if ((varBind[1].value.asnValue.address.stream[0] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[1] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[2] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[3] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[4] == 0x00) &&
                                    (varBind[1].value.asnValue.address.stream[5] == 0x00))
                                {
                                    /* ignore NULL addresses returned by other network interfaces */
                                    continue;
                                }
                                /* store the MAC address */
                                buffer[0] = varBind[1].value.asnValue.address.stream[0];
                                buffer[1] = varBind[1].value.asnValue.address.stream[1];
                                buffer[2] = varBind[1].value.asnValue.address.stream[2];
                                buffer[3] = varBind[1].value.asnValue.address.stream[3];
                                buffer[4] = varBind[1].value.asnValue.address.stream[4];
                                buffer[5] = varBind[1].value.asnValue.address.stream[5];
                                ret = 1;    // we found an address -> exit
                            }
                        }
                    }
                } while (!ret);  /* Stop only on an error. An error will occur when we
                                    go exhaust the list of interfaces to be examined */
                FreeLibrary(m_hInst2);
                /* free the bindings */
                m_BindFree(&varBind[0]);
                m_BindFree(&varBind[1]);
            }
            FreeLibrary(m_hInst1);
        }
    }
    return buffer;
}
#endif

#ifdef HAVE_PROTOTYPE_GETHOSTID
/* CW10 has a prototype but no implementation (gethostid() is already declared extern */
long gethostid(void)
#else
static long gethostid(void)
#endif
{
    long result = 0;
#if defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME)
    char name[1024];
    struct hostent *hent = NULL;
    char **p = NULL;
    struct in_addr in;
#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD(1, 1);
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif
    /*
    ** Define the hostid to be the system's main TCP/IP address.
    ** This is not perfect but it is better than nothing (i.e. using zero)
    */
    if (gethostname(name, 1024) == 0)
    {
#if defined(_REENTRANT) && !defined(_WIN32) && !defined(__CYGWIN__)
        // use gethostbyname_r instead of gethostbyname
        int h_errnop=0;
        struct hostent theHostent;
        char buffer[GETHOSTBYNAME_R_BUFSIZE];
        if ((hent = gethostbyname_r(name, &theHostent, buffer, GETHOSTBYNAME_R_BUFSIZE, &h_errnop)) != NULL)
#else
        if ((hent = gethostbyname(name)) != NULL)
#endif
        {
            p = hent->h_addr_list;
            if (p && *p)
            {
                (void) memcpy(&in.s_addr, *p, sizeof(in.s_addr));
                result = (long)in.s_addr;
            }
        }
    }
#ifdef HAVE_WINSOCK_H
    WSACleanup();
#endif
#endif /* defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME) */
/* on Windows systems determine some system specific information (e.g. MAC address) */
#ifdef HAVE_WINDOWS_H
    OFCRC32 crc;
    /* get some processor specific information in addition to the MAC address */
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    /* get volume information of the system drive */
    char systemDrive[MAX_PATH];
    DWORD serialNumber = 0;
    if (GetSystemDirectory(systemDrive, sizeof(systemDrive)) >= 0)
    {
        /* check for proper pathname */
        if ((strlen(systemDrive) >= 3) && (systemDrive[1] == ':') && (systemDrive[2] == '\\'))
        {
            /* truncate the pathname directly after the drive specification */
            systemDrive[3] = 0;
            if (!GetVolumeInformation(systemDrive, NULL, 0, &serialNumber, NULL, NULL, NULL, 0))
                serialNumber = 0;
        }
    }
    unsigned char buffer[6];
    /* concatenate the host specific elements and compute a 32-bit checksum */
    crc.addBlock(&result /*ip address*/, sizeof(result));
    crc.addBlock(getMACAddress(buffer), sizeof(buffer));
    crc.addBlock(&serialNumber, sizeof(serialNumber));
    crc.addBlock(&systemInfo.wProcessorLevel, sizeof(systemInfo.wProcessorLevel));
    crc.addBlock(&systemInfo.wProcessorRevision, sizeof(systemInfo.wProcessorRevision));
    crc.addBlock(&systemInfo.dwProcessorType, sizeof(systemInfo.dwProcessorType));
    result = (long)crc.getCRC32();
#endif
    /* 'artificial' hostid: on Windows system a CRC32 checksum over some host specific
       information (e.g. MAC address), the 4 bytes TCP/IP address otherwise.
    */
    return result;
}

#else // !(defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME))
/*
** last chance workaround if there is no other way
*/
#ifdef HAVE_PROTOTYPE_GETHOSTID
/* CW10 has a prototype but no implementation (gethostid() is already declared extern */
long gethostid(void) { return 0; }
#else
static long gethostid(void) { return 0; }
#endif
#endif // !(defined(HAVE_GETHOSTNAME) && defined(HAVE_GETHOSTBYNAME))

#endif // !HAVE_SYSINFO
#else // HAVE_GETHOSTID
#ifndef HAVE_PROTOTYPE_GETHOSTID
extern "C" {
long gethostid(void);
}
#endif // !HAVE_PROTOTYPE_GETHOSTID
#endif // HAVE_GETHOSTID

#ifndef HAVE_GETPID
static int getpid(void) { return 0; }   // workaround for MAC
#endif // !HAVE_GETPID

// ********************************

/*
 * Global variable storing the return value of gethostid().
 * Since the variable is not declared in the header file it can only be used
 * within this source file. Any access to or modification of its value is
 * protected by a mutex (see dcmGenerateUniqueIdentifier()).
 */

static unsigned long hostIdentifier = 0;


/*
** char* generateUniqueIdentifer(char* buf)
** Creates a Unique Identifer in buf and returns buf.
** buf must be at least 65 bytes.
*/


#ifdef _REENTRANT
static OFMutex uidCounterMutex;  // mutex protecting access to counterOfCurrentUID and hostIdentifier
#endif

static unsigned int counterOfCurrentUID = 1;

static const int maxUIDLen = 64;    /* A UID may be 64 chars or less */

static char*
stripTrailing(char* s, char c)
{
    int i, n;

    if (s == NULL) return s;

    n = strlen(s);
    for (i = n - 1; (i >= 0) && (s[i] == c); i--)
        s[i] = '\0';
    return s;
}

static void
addUIDComponent(char* uid, const char* s)
{
    int charsLeft = maxUIDLen - strlen(uid);

    if (charsLeft > 0) {
        /* copy into uid as much of the contents of s as possible */
        int slen = strlen(s);
        int use = charsLeft;
        if (slen < charsLeft) use = slen;
            strncat(uid, s, use);
    }

    stripTrailing(uid, '.');
}

inline static unsigned long
forcePositive(long i)
{
    return (i < 0) ? (unsigned long)-i : (unsigned long)i;
}

char* dcmGenerateUniqueIdentifier(char* uid, const char* prefix)
{
    char buf[128]; /* be very safe */

    uid[0] = '\0'; /* initialise */

#ifdef _REENTRANT
    uidCounterMutex.lock();
#endif
    if (hostIdentifier == 0)
        hostIdentifier = (unsigned long)gethostid();
    unsigned int counter = counterOfCurrentUID++;
#ifdef _REENTRANT
    uidCounterMutex.unlock();
#endif

    if (prefix != NULL ) {
        addUIDComponent(uid, prefix);
    } else {
        addUIDComponent(uid, SITE_INSTANCE_UID_ROOT);
    }

    sprintf(buf, ".%lu", hostIdentifier);
    addUIDComponent(uid, buf);

    sprintf(buf, ".%lu", forcePositive(getpid()));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%lu", forcePositive(long(time(NULL))));
    addUIDComponent(uid, buf);

    sprintf(buf, ".%u", counter);

    addUIDComponent(uid, buf);

    return uid;
}
