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
 *  Purpose: Interface of class DcmDicomDir
 *
 */

#ifndef DCDICDIR_H
#define DCDICDIR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dctypes.h"
#include "dcitem.h"
#include "dcfilefo.h"
#include "dcsequen.h"
#include "dcdirrec.h"
#include "dcvrulup.h"


#define DEFAULT_DICOMDIR_NAME  "DICOMDIR"
#define TEMPNAME_TEMPLATE      "DDXXXXXX"
#define DICOMDIR_BACKUP_SUFFIX "."
#define DICOMDIR_DEFAULT_TRANSFERSYNTAX  EXS_LittleEndianExplicit


typedef struct
{
    DcmItem *item;
    Uint32  fileOffset;
} ItemOffset;


class DcmDicomDir
{
  private:

 // --- declarations to avoid compiler warnings

    DcmDicomDir &operator=(const DcmDicomDir &);

  protected:
    OFCondition errorFlag;
    char * dicomDirFileName;
    OFBool modified;              // wird wo gebraucht ?
    OFBool mustCreateNewDir;
    DcmFileFormat * DirFile;
    DcmDirectoryRecord * RootRec;
    DcmSequenceOfItems * MRDRSeq;

    // Manipulation der internen Datenelemente:
    OFCondition createNewElements(const char* fileSetID);  // in
    DcmDataset& getDataset(void);

    // Seiteneffekt-freie Methoden zur Manipulation und Konversion:
    DcmSequenceOfItems&    getDirRecSeq(      DcmDataset &dset );   // inout
    DcmUnsignedLongOffset* lookForOffsetElem( DcmObject *obj,       // in
                                              const DcmTagKey &offsetTag );// in
    DcmDirectoryRecord*    recurseMatchFile(  DcmDirectoryRecord* startRec,// in
                                              char *filename );            // in
    DcmDirectoryRecord*    searchMatchFile(   DcmSequenceOfItems& recSeq,  // in
                                              char *filename );            // in
    OFCondition resolveGivenOffsets( DcmObject *startPoint,         // inout
                                     ItemOffset *itOffsets,         // in
                                     const unsigned long numOffsets,  // in
                                     const DcmTagKey &offsetTag );  // in
    OFCondition resolveAllOffsets(   DcmDataset &dset );            // inout
    OFCondition linkMRDRtoRecord(    DcmDirectoryRecord *dRec );    // inout
    OFCondition moveRecordToTree(    DcmDirectoryRecord *startRec,  // in
                                     DcmSequenceOfItems &fromDirSQ, // inout
                                     DcmDirectoryRecord *toRecord );// inout
    OFCondition moveMRDRbetweenSQs(  DcmSequenceOfItems &fromSQ,    // in
                                     DcmSequenceOfItems &toDirSQ ); // inout
    Uint32     lengthUntilSQ(       DcmDataset &dset,              // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    Uint32     lengthOfRecord(      DcmItem *item,                 // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    OFCondition convertGivenPointer( DcmObject *startPoint,         // inout
                                     ItemOffset *itOffsets,         // in
                                     const unsigned long numOffsets,  // in
                                     const DcmTagKey &offsetTag );  // in
    OFCondition convertAllPointer(   DcmDataset &dset,              // inout
                                     Uint32 beginOfFileSet,        // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype );      // in
    OFCondition copyRecordPtrToSQ(   DcmDirectoryRecord *record,    // in
                                     DcmSequenceOfItems &toDirSQ,   // inout
                                     DcmDirectoryRecord **firstRec, // out
                                     DcmDirectoryRecord **lastRec );// out
    OFCondition insertMediaSOPUID(   DcmMetaInfo &metaInfo );       // inout
    OFCondition countMRDRRefs(       DcmDirectoryRecord *startRec,  // in
                                     ItemOffset *refCounter,        // inout
                                     const unsigned long numCounters );   // in
    OFCondition checkMRDRRefCounter( DcmDirectoryRecord *startRec,  // in
                                     ItemOffset *refCounter,        // inout
                                     const unsigned long numCounters );    // in

    // komplette Reorganisation der verwalteten Directory Records (Seiteneffekt)
    OFCondition convertLinearToTree();
    OFCondition convertTreeToLinear( Uint32 beginOfFileSet,         // in
                                     E_TransferSyntax oxfer,        // in
                                     E_EncodingType enctype,        // in
                                     E_GrpLenEncoding glenc,        // in
                                     DcmSequenceOfItems &unresRecs);// inout

public:
    DcmDicomDir();
    DcmDicomDir( const char *fileName,
                 const char *fileSetID = (char*)NULL );  // only used for new DICOMDIR
    DcmDicomDir( const DcmDicomDir &newDir );
    virtual ~DcmDicomDir();

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual OFCondition         error();
    virtual DcmFileFormat&      getDirFileFormat();
    virtual const char*         getDirFileName();
    virtual DcmDirectoryRecord& getRootRecord();
    virtual DcmSequenceOfItems& getMRDRSequence();
    virtual DcmDirectoryRecord* matchFilename(     char *filename );
    virtual DcmDirectoryRecord* matchOrCreateMRDR( char *filename );
    virtual OFCondition         write(const E_TransferSyntax oxfer
                                            = DICOMDIR_DEFAULT_TRANSFERSYNTAX,
                                      const E_EncodingType enctype
                                            = EET_UndefinedLength,
                                      const E_GrpLenEncoding glenc
                                            = EGL_withoutGL );
// PENDING: DICOM-konform, aber unvollstaendig
    virtual OFCondition         verify( OFBool autocorrect = OFFalse );
};

#endif // DCDICDIR_H
