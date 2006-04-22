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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmMetaInfo
 *
 */


#ifndef DCMETINF_H
#define DCMETINF_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dctypes.h"
#include "dcitem.h"


#define DCM_Magic                       "DICM"
#define DCM_MagicLen                    4
#define DCM_PreambleLen                 128
#define META_HEADER_DEFAULT_TRANSFERSYNTAX EXS_LittleEndianExplicit


/** a class representing the DICOM file meta information header
 */
class DcmMetaInfo
  : public DcmItem
{

  public:

    /** default constructor
     */
    DcmMetaInfo();

    /** copy constructor
     *  @param old item to be copied
     */
    DcmMetaInfo(const DcmMetaInfo &old);

    /** destructor
     */
    virtual ~DcmMetaInfo();

    /** get type identifier
     *  @return type identifier of this class (EVR_item)
     */
    virtual DcmEVR ident() const;

    E_TransferSyntax getOriginalXfer() const;

    /** print meta information header to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual void transferInit();
    virtual void transferEnd();

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write meta information header to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype = EET_UndefinedLength);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);


  private:

    void setPreamble();

    OFBool checkAndReadPreamble(DcmInputStream &inStream,
                                E_TransferSyntax &newxfer);  // out

    OFBool nextTagIsMeta(DcmInputStream &inStream);

    OFCondition readGroupLength(DcmInputStream &inStream,       // inout
                                const E_TransferSyntax xfer,    // in
                                const DcmTagKey &xtag,          // in
                                const E_GrpLenEncoding glenc,   // in
                                Uint32 &headerLen,              // out
                                Uint32 &bytesRead,              // out
                                const Uint32 maxReadLength = DCM_MaxReadLength);   // in

    char filePreamble[DCM_PreambleLen + DCM_MagicLen];
    OFBool preambleUsed;
    E_TransferState fPreambleTransferState;
    E_TransferSyntax Xfer;
};

#endif // DCMETINF_H
