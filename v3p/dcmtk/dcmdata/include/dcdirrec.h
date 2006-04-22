/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmDirectoryRecord
 *
 */

#ifndef DCDIRREC_H
#define DCDIRREC_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcitem.h"
#include "dcsequen.h"
#include "dcfilefo.h"


typedef enum {
    ERT_root = 0,
    ERT_Curve = 1,
    ERT_FilmBox = 2,
    ERT_FilmSession = 3,
    ERT_Image = 4,
    ERT_ImageBox = 5,
    ERT_Interpretation = 6,
    ERT_ModalityLut = 7,
    ERT_Mrdr = 8,
    ERT_Overlay = 9,
    ERT_Patient = 10,
    ERT_PrintQueue = 11,
    ERT_Private = 12,
    ERT_Results = 13,
    ERT_Series = 14,
    ERT_Study = 15,
    ERT_StudyComponent = 16,
    ERT_Topic = 17,
    ERT_Visit = 18,
    ERT_VoiLut = 19,
    ERT_StructReport = 20,
    ERT_Presentation = 21,
    ERT_Waveform = 22,
    ERT_RTDose = 23,
    ERT_RTStructureSet = 24,
    ERT_RTPlan = 25,
    ERT_RTTreatRecord = 26,
    ERT_StoredPrint = 27,
    ERT_KeyObjectDoc = 28
} E_DirRecType;


class DcmDicomDir;


class DcmDirectoryRecord : public DcmItem {

    friend class DcmDicomDir;

private:
    char* recordsOriginFile;

 // --- declarations to avoid compiler warnings

    DcmDirectoryRecord &operator=(const DcmDirectoryRecord &);

protected:
    DcmSequenceOfItems* lowerLevelList;
    E_DirRecType DirRecordType;
    DcmDirectoryRecord* referencedMRDR;
    Uint32 numberOfReferences;
    Uint32 offsetInFile;

    // Seiteneffekt-freie Konversions-Methoden:
    E_DirRecType recordNameToType(const char *recordTypeName);
    char *      buildFileName(const char* origName, char* destName );
    OFCondition checkHierarchy(const E_DirRecType upperRecord,
                               const E_DirRecType lowerRecord );

    // Zugriff auf Datenelemente innerhalb des Directory Records:
    OFCondition         setRecordType(E_DirRecType newType );
    E_DirRecType        lookForRecordType();
    OFCondition         setReferencedFileID( const char *referencedFileID );
    const char*         lookForReferencedFileID();
    DcmDirectoryRecord* lookForReferencedMRDR();
    const char*         getReferencedFileName();      // lokal oder in MRDR
    OFCondition         setRecordInUseFlag(const Uint16 newFlag );
    Uint16              lookForRecordInUseFlag();
    Uint32              getFileOffset();
    Uint32              setFileOffset(Uint32 position );


    // Zugriff auf MRDR-Datenelement:
    OFCondition         setNumberOfReferences(  Uint32 newRefNum );
    Uint32              lookForNumberOfReferences();
    Uint32              increaseRefNum();
    Uint32              decreaseRefNum();

    // Verschiedenes:
    OFCondition         fillElementsAndReadSOP(const char *referencedFileID,
                                               const char * sourceFileName);
    OFCondition         masterInsertSub(DcmDirectoryRecord* dirRec,
                                        const unsigned long where
                                        = DCM_EndOfListIndex);
    OFCondition         purgeReferencedFile();

public:
    DcmDirectoryRecord();
    DcmDirectoryRecord(const DcmTag &tag,
                       const Uint32 len);
    DcmDirectoryRecord(const E_DirRecType recordType,
                       const char *referencedFileID,   // Dicom-Format with '\\'
                       const char * sourceFileName);   // OS Format

    DcmDirectoryRecord(const char *recordTypeName,
                       const char *referencedFileID,   // Dicom-Format with '\\'
                       const char * sourceFileName);   // OS Format

    DcmDirectoryRecord(const DcmDirectoryRecord &oldDirRec );
    virtual ~DcmDirectoryRecord();

    virtual DcmEVR ident() const;

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual OFCondition verify(const OFBool autocorrect = OFFalse);
    virtual OFCondition search(const DcmTagKey &xtag,       // in
                               DcmStack &resultStack,       // inout
                               E_SearchMode mode = ESM_fromHere,// in
                               OFBool searchIntoSub = OFTrue );     // in
    virtual OFCondition searchErrors(DcmStack &resultStack );   // inout

    virtual E_DirRecType getRecordType();
    virtual DcmDirectoryRecord* getReferencedMRDR();
    virtual OFCondition assignToMRDR( DcmDirectoryRecord *mrdr );// in
    virtual OFCondition assignToSOPFile(const char *referencedFileID,
                                        const char * sourceFileName);

    // Manipulation of the Lower-Level Directory Entities:

    virtual unsigned long cardSub();
    virtual OFCondition insertSub(DcmDirectoryRecord* dirRec,
                                  unsigned long where = DCM_EndOfListIndex,
                                  OFBool before = OFFalse);
    virtual DcmDirectoryRecord* getSub(const unsigned long num);
    virtual DcmDirectoryRecord* removeSub(const unsigned long num);
    virtual DcmDirectoryRecord* removeSub( DcmDirectoryRecord* dirRec );
    virtual OFCondition deleteSubAndPurgeFile(const unsigned long num);
    virtual OFCondition deleteSubAndPurgeFile( DcmDirectoryRecord* dirRec );
    virtual OFCondition clearSub();

    // note the records origin file
    virtual void setRecordsOriginFile(const char* fname);
    virtual const char* getRecordsOriginFile();

};



#endif // DCDIRREC_H
