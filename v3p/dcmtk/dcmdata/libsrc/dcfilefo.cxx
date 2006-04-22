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
 *  Purpose: class DcmFileFormat
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CTIME
#include "ofstdinc.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "ofstream.h"
#include "dcfilefo.h"
#include "dcitem.h"
#include "dcxfer.h"
#include "dcvrobow.h"
#include "dcvrui.h"
#include "dcvrul.h"
#include "dcvrus.h"
#include "dcvrae.h"
#include "dcvrsh.h"
#include "dcmetinf.h"
#include "dcdebug.h"

#include "dcdeftag.h"
#include "dcuid.h"
#include "dcostrma.h"    /* for class DcmOutputStream */
#include "dcostrmf.h"    /* for class DcmOutputFileStream */
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcistrmf.h"    /* for class DcmInputFileStream */


// ********************************


DcmFileFormat::DcmFileFormat()
  : DcmSequenceOfItems(InternalUseTag)
{
    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert(MetaInfo);
    DcmDataset *Dataset = new DcmDataset();
    DcmSequenceOfItems::itemList->insert(Dataset);
}


DcmFileFormat::DcmFileFormat(DcmDataset *dataset)
    : DcmSequenceOfItems(InternalUseTag)
{
    DcmMetaInfo *MetaInfo = new DcmMetaInfo();
    DcmSequenceOfItems::itemList->insert(MetaInfo);
    DcmDataset *newDataset;

    if (dataset == (DcmDataset*)NULL)
        newDataset = new DcmDataset();
    else
        newDataset = new DcmDataset(*dataset);
    DcmSequenceOfItems::itemList->insert(newDataset);
}


DcmFileFormat::DcmFileFormat(const DcmFileFormat &old)
  : DcmSequenceOfItems(old)
{
}


DcmFileFormat::~DcmFileFormat()
{
}


DcmFileFormat &DcmFileFormat::operator=(const DcmFileFormat &obj)
{
    DcmSequenceOfItems::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmFileFormat::ident() const
{
    return EVR_fileFormat;
}


// ********************************


void DcmFileFormat::print(ostream &out,
                          const size_t flags,
                          const int level,
                          const char *pixelFileName,
                          size_t *pixelCounter)
{
    out << endl;
    printNestingLevel(out, flags, level);
    out << "# Dicom-File-Format" << endl;
    if (!itemList->empty())
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            dO->print(out, flags, level, pixelFileName, pixelCounter);
        } while (itemList->seek(ELP_next));
    } else {
        printNestingLevel(out, flags, level);
        out << "# Dicom-File-Format has been erased";
        out << endl;
    }
}


// ********************************


OFCondition DcmFileFormat::writeXML(ostream &out,
                                    const size_t flags)
{
    OFCondition result = EC_CorruptedData;
    /* XML start tag for "file-format" */
    out << "<file-format>" << endl;
    if (!itemList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            dO->writeXML(out, flags);
        } while (itemList->seek(ELP_next));
        result = EC_Normal;
    }
    /* XML end tag for "file-format" */
    out << "</file-format>" << endl;
    return result;
}


// ********************************


OFCondition DcmFileFormat::checkValue(DcmMetaInfo *metainfo,
                                      DcmDataset *dataset,
                                      const DcmTagKey &atagkey,
                                      DcmObject *obj,
                                      const E_TransferSyntax oxfer)
    /*
     * This function checks if a particular data element of the meta header information is existent
     * in metainfo. If the element is not existent, it will be inserted. Additionally, this function
     * makes sure that the corresponding data element will contain a correct value.
     *
     * Parameters:
     *   metainfo - [in] The meta header information.
     *   dataset  - [in] The data set information.
     *   atagkey  - [in] Tag of the data element which shall be checked.
     *   obj      - [in] Data object from metainfo which represents the data element that shall be checked.
     *                   Equals NULL, if this data element is not existent in the meta header information.
     *   oxfer    - [in] The transfer syntax which shall be used.
     */
{
    /* initialize result value */
    OFCondition l_error = EC_Normal;

    /* if there is meta header information and also data set information, do something */
    if (metainfo != (DcmMetaInfo*)NULL && dataset != (DcmDataset*)NULL)
    {
        /* initialize variables */
        DcmStack stack;
        DcmTag tag(atagkey);
        if (obj != (DcmObject*)NULL)
            tag = obj->getTag();

        DcmTagKey xtag = tag.getXTag();
        DcmElement *elem = (DcmElement*)obj;

        /* go ahaed and scrutinize one particular data element (depending on xtag) */
        if (xtag == DCM_MetaElementGroupLength)             // (0002,0000)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmUnsignedLong(tag);
                metainfo->insert(elem, OFTrue);
            }
            Uint32 temp = 0;
            if (elem->getLength() == 0 && elem->ident() == EVR_UL)
                ((DcmUnsignedLong*)elem)->putUint32Array(&temp, 1);
            // the calculation of actual group length value is contained in validateMetaInfo()
        }
        else if (xtag == DCM_FileMetaInformationVersion)    // (0002,0001)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmOtherByteOtherWord(tag);
                metainfo->insert(elem, OFTrue);
            }
            Uint8 version[2] = {0,1};
            if (elem->getLength() == 0 && elem->ident() == EVR_OB)
                ((DcmOtherByteOtherWord*)elem)->putUint8Array(version, 2);

            // check version of meta header
            Uint8 *currVers;
            l_error = ((DcmOtherByteOtherWord*)elem)->getUint8Array(currVers);
            if (((currVers[0] & version[0] & 0xff) == version[0]) &&
                ((currVers[1] & version[1] & 0xff) == version[1]))
            {
                debug(2, ("DcmFileFormat::checkValue() Version of MetaHeader is ok: 0x%2.2x%2.2x",
                        currVers[1], currVers[0]));
            } else {
                currVers[0] = currVers[0] | version[0]; // direct manipulation
                currVers[1] = currVers[1] | version[1]; // of data
                ostream& localCerr = ofConsole.lockCerr();
                localCerr << "Warning: dcfilefo: unknown Version of MetaHeader detected: 0x";
                localCerr << hex << setfill('0')
                          << setw(2) << (int)currVers[1]
                          << setw(2) << (int)currVers[0] << " supported: 0x"
                          << setw(2) << (int)version[1]
                          << setw(2) << (int)version[0]
                          << dec << setfill(' ') << endl;
                ofConsole.unlockCerr();
            }
        }
        else if (xtag == DCM_MediaStorageSOPClassUID)       // (0002,0002)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->getLength() == 0 && elem->ident() == EVR_UI) {
                if (dataset->search(DCM_SOPClassUID, stack).good())
                {
                    char *uid = NULL;
                    l_error = ((DcmUniqueIdentifier*)stack.top())->getString(uid);
                    ((DcmUniqueIdentifier*)elem)->putString(uid);
                    debug(2, ("DcmFileFormat::checkValue() use SOPClassUID [%s]", uid));
                }
                else
                {
                    ((DcmUniqueIdentifier*)elem)->putString(
                        UID_PrivateGenericFileSOPClass);
                    debug(2, ("DcmFileFormat::checkValue() No SOP Class UID in Dataset, using PrivateGenericFileSOPClass"));
                }
            }
        }
        else if (xtag == DCM_MediaStorageSOPInstanceUID)    // (0002,0003)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->getLength() == 0 && elem->ident() == EVR_UI)
            {
                if (dataset->search(DCM_SOPInstanceUID, stack).good())
                {
                    char* uid = NULL;
                    l_error =((DcmUniqueIdentifier*)stack.top())->getString(uid);
                    ((DcmUniqueIdentifier*)elem)->putString(uid);
                    debug(2, ("DcmFileFormat::checkValue() use SOPInstanceUID [%s] from Dataset", uid));
                } else {
                    char uid[128];
                    dcmGenerateUniqueIdentifier(uid);       // from dcuid.h
                    ((DcmUniqueIdentifier*)elem)->putString(uid);
                    debug(2, ("DcmFileFormat::checkValue() use new generated SOPInstanceUID [%s]", uid));
                }
            }
        }
        else if (xtag == DCM_TransferSyntaxUID)             // (0002,0010)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_UI)
            {
#ifdef DEBUG
char * uidtmp = NULL;
((DcmUniqueIdentifier*)elem)->getString(uidtmp);
Cdebug(2,  uidtmp != (char*)NULL,
       ("DcmFileFormat::checkValue() found old transfer-syntax: [%s]",uidtmp));
#endif
                DcmXfer dcXfer(oxfer);
                const char *uid = dcXfer.getXferID();
                elem->putString(uid);
                debug(2,("DcmFileFormat::checkValue() use new transfer-syntax [%s] on writing following Dataset",
                        dcXfer.getXferName()));
            }
        }
        else if (xtag == DCM_ImplementationClassUID)        // (0002,0012)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_UI)
            {
                const char *uid = OFFIS_IMPLEMENTATION_CLASS_UID;
                ((DcmUniqueIdentifier*)elem)->putString(uid);
            }
        }
        else if (xtag == DCM_ImplementationVersionName)     // (0002,0013)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmShortString(tag);
                metainfo->insert(elem, OFTrue);
            }
            if (elem->ident() == EVR_SH)
            {
                const char *uid = OFFIS_DTK_IMPLEMENTATION_VERSION_NAME;
                ((DcmShortString*)elem)->putString(uid);
            }
        }
        else if (xtag == DCM_SourceApplicationEntityTitle)  // (0002,0016)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmApplicationEntity(tag);
                metainfo->insert(elem, OFTrue);
            }
            ofConsole.lockCerr() << "Error: dcfilefo: I don't know how to handle DCM_SourceApplicationEntityTitle!" << endl;
            ofConsole.unlockCerr();
        }
        else if (xtag == DCM_PrivateInformationCreatorUID)  // (0002,0100)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmUniqueIdentifier(tag);
                metainfo->insert(elem, OFTrue);
            }
            ofConsole.lockCerr() << "Error: dcfilefo: I don't know how to handle DCM_PrivateInformationCreatorUID!" << endl;
            ofConsole.unlockCerr();
        }
        else if (xtag == DCM_PrivateInformation)            // (0002,0102)
        {
            if (elem == (DcmElement*)NULL)
            {
                elem = new DcmOtherByteOtherWord(tag);
                metainfo->insert(elem, OFTrue);
            }
            ofConsole.lockCerr() << "Warning: dcfilefo: I don't know how to handle DCM_PrivateInformation!" << endl;
            ofConsole.unlockCerr();
        } else {
            ofConsole.lockCerr() << "Warning: dcfilefo: I don't know how to handle " << tag.getTagName() << endl;
            ofConsole.unlockCerr();
        }

        /* if at this point elem still equals NULL, something is fishy */
        if (elem == (DcmElement*)NULL)
            l_error = EC_InvalidVR;
    } else {
    /* (i.e. there is either no meta header information or no data set information */
        l_error = EC_IllegalCall;
    }

    /* return result value */
    return l_error;
}


// ********************************


OFCondition DcmFileFormat::validateMetaInfo(E_TransferSyntax oxfer)
    /*
     * This function makes sure that all data elements of the meta header information are existent
     * in metainfo and contain correct values.
     *
     * Parameters:
     *   oxfer          - [in] The transfer syntax which shall be used.
     */
{
    /* initialize some variables */
    OFCondition l_error = EC_Normal;
    DcmMetaInfo *metinf = getMetaInfo();
    DcmDataset *datset = getDataset();

    /* if there is meta header information and data set information, do something */
    if (metinf != (DcmMetaInfo*)NULL && datset != (DcmDataset*)NULL)
    {
        DcmStack stack;

        /* in the following, we want to make sure all elements of the meta header */
        /* are existent in metinf and contain correct values */

        /* DCM_MetaElementGroupLength */
        metinf->search(DCM_MetaElementGroupLength, stack, ESM_fromHere, OFFalse);
        checkValue(metinf, datset, DCM_MetaElementGroupLength, stack.top(), oxfer);

        /* DCM_FileMetaInformationVersion */
        metinf->search(DCM_FileMetaInformationVersion, stack, ESM_fromHere, OFFalse);
        checkValue(metinf, datset, DCM_FileMetaInformationVersion, stack.top(), oxfer);

        /* DCM_MediaStorageSOPClassUID */
        metinf->search(DCM_MediaStorageSOPClassUID, stack, ESM_fromHere, OFFalse);
        checkValue(metinf, datset, DCM_MediaStorageSOPClassUID, stack.top(), oxfer);

        /* DCM_MediaStorageSOPInstanceUID */
        metinf->search(DCM_MediaStorageSOPInstanceUID, stack, ESM_fromHere, OFFalse);
        checkValue(metinf, datset, DCM_MediaStorageSOPInstanceUID, stack.top(), oxfer);

        /* DCM_TransferSyntaxUID */
        metinf->search(DCM_TransferSyntaxUID, stack, ESM_fromHere, OFFalse);
        checkValue(metinf, datset, DCM_TransferSyntaxUID, stack.top(), oxfer);

        /* DCM_ImplementationClassUID */
        metinf->search(DCM_ImplementationClassUID, stack, ESM_fromHere, OFFalse);
        checkValue(metinf, datset, DCM_ImplementationClassUID, stack.top(), oxfer);

        /* DCM_ImplementationVersionName */
        metinf->search(DCM_ImplementationVersionName, stack, ESM_fromHere, OFFalse);
        checkValue(metinf, datset, DCM_ImplementationVersionName, stack.top(), oxfer);

        /* dump some information if reuqired */
        debug(2, ("DcmFileFormat: found %ld Elements in DcmMetaInfo metinf.",
                metinf->card()));

        /* calculate new GroupLength for meta header */
        if (metinf->computeGroupLengthAndPadding(EGL_withGL, EPD_noChange,
            META_HEADER_DEFAULT_TRANSFERSYNTAX, EET_UndefinedLength).bad())
        {
            ofConsole.lockCerr() << "Error: DcmFileFormat::validateMetaInfo(): group length of Meta Information Header not adapted." << endl;
            ofConsole.unlockCerr();
        }
    } else {
        /* (i.e. there is either no meta header information or no data set information, or both are missing) */
        l_error = EC_CorruptedData;
    }

    /* return result value */
    return l_error;
}


// ********************************


E_TransferSyntax DcmFileFormat::lookForXfer(DcmMetaInfo *metainfo)
{
    E_TransferSyntax newxfer = EXS_Unknown;
    DcmStack stack;
    if (metainfo && metainfo->search(DCM_TransferSyntaxUID, stack).good())
    {
        DcmUniqueIdentifier *xferUI = (DcmUniqueIdentifier*)(stack.top());
        if (xferUI->getTag().getXTag() == DCM_TransferSyntaxUID)
        {
            char * xferid = NULL;
            xferUI->getString(xferid);     // auslesen der ID
            DcmXfer localXfer(xferid);      // dekodieren in E_TransferSyntax
            newxfer = localXfer.getXfer();
            debug(4, ("DcmFileFormat::lookForXfer() detected xfer=%d=[%s] in MetaInfo",
                newxfer, localXfer.getXferName()));
        }
    }
    return newxfer;
}


// ********************************


Uint32 DcmFileFormat::calcElementLength(const E_TransferSyntax xfer,
                                        const E_EncodingType enctype)
{
    return getMetaInfo()->calcElementLength(xfer, enctype) +
        getDataset()->calcElementLength(xfer, enctype);
}


// ********************************


OFBool DcmFileFormat::canWriteXfer(const E_TransferSyntax newXfer,
                                   const E_TransferSyntax oldXfer)
{
    DcmDataset *dataset = getDataset();

    if (dataset)
        return dataset->canWriteXfer(newXfer, oldXfer);
    else
        return OFFalse;
}


// ********************************


OFCondition DcmFileFormat::read(DcmInputStream &inStream,
                                const E_TransferSyntax xfer,
                                const E_GrpLenEncoding glenc,
                                const Uint32 maxReadLength)

{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = inStream.status();

        E_TransferSyntax newxfer = xfer;
        DcmDataset *dataset = NULL;

        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        else if (errorFlag.good() && fTransferState != ERW_ready)
        {
            // the new data is added to the end
            itemList->seek(ELP_last);

            DcmMetaInfo *metaInfo = getMetaInfo();
            if (metaInfo == NULL && fTransferState == ERW_init)
            {
                metaInfo = new DcmMetaInfo();
                itemList->insert(metaInfo, ELP_first);
            }
            if (metaInfo && metaInfo->transferState() != ERW_ready)
            {
                errorFlag = metaInfo->read(inStream, xfer, glenc, maxReadLength);
            }

            // read MetaInfo() from Tag(0002,0010) and determine xfer
            newxfer = lookForXfer(metaInfo);
            if (errorFlag.good() && (!metaInfo || metaInfo->transferState() == ERW_ready))
            {
                dataset = getDataset();
                if (dataset == NULL && fTransferState == ERW_init)
                {
                    dataset = new DcmDataset();
                    itemList->seek (ELP_first);
                    itemList->insert(dataset, ELP_next);
                }
                if (dataset && dataset->transferState() != ERW_ready)
                {
                    errorFlag = dataset->read(inStream, newxfer, glenc, maxReadLength);
                }
            }
        }
        if (fTransferState == ERW_init)
            fTransferState = ERW_inWork;

        if (dataset && dataset->transferState() == ERW_ready)
            fTransferState = ERW_ready;
    }
    return errorFlag;
}  // DcmFileFormat::read()


// ********************************


OFCondition DcmFileFormat::write(DcmOutputStream &outStream,
                                 const E_TransferSyntax oxfer,
                                 const E_EncodingType enctype)
{
    return write(outStream, oxfer, enctype, EGL_recalcGL, EPD_noChange);
}


OFCondition DcmFileFormat::write(DcmOutputStream &outStream,
                                 const E_TransferSyntax oxfer,
                                 const E_EncodingType enctype,
                                 const E_GrpLenEncoding glenc,
                                 const E_PaddingEncoding padenc,
                                 const Uint32 padlen,
                                 const Uint32 subPadlen,
                                 Uint32 instanceLength)
    /*
     * This function writes data values which are contained in this to the stream which is
     * passed as first argument. With regard to the writing of information, the other parameters
     * which are passed are accounted for. The function will return EC_Normal, if the information
     * from all elements of this data set has been written to the buffer, and it will return some
     * other (error) value if there was an error.
     *
     * Parameters:
     *   outStream      - [inout] The stream that the information will be written to.
     *   oxfer          - [in] The transfer syntax which shall be used.
     *   enctype        - [in] Encoding type for sequences. Specifies how sequences will be handled.
     *   glenc          - [in] Encoding type for group length. Specifies what will be done with group length tags.
     *   padenc         - [in] Encoding type for padding. Specifies what will be done with padding tags.
     *   padlen         - [in] The length up to which the dataset shall be padded, if padding is desired.
     *   subPadlen      - [in] For sequences (i.e. sub elements), the length up to which item shall be padded,
     *                         if padding is desired.
     *   instanceLength - [in] Number of extra bytes added to the item/dataset length used when computing the
     *                         padding. This parameter is for instance used to pass the length of the file meta
     *                         header from the DcmFileFormat to the DcmDataset object.
     */
{
    /* if the transfer state of this is not initialized, this is an illegal call */
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, do something */

        /* assign data set and the meta information header to local variables */
        DcmDataset *dataset = getDataset();
        DcmMetaInfo *metainfo = getMetaInfo();
        /* Determine the transfer syntax which shall be used. Either we use the one which was passed, */
        /* or (if it's an unknown transfer syntax) we use the data set's original transfer syntax. */
        E_TransferSyntax outxfer = oxfer;
        if (outxfer == EXS_Unknown && dataset)
            outxfer = dataset->getOriginalXfer();
        /* check if the stream reported an error so far */
        errorFlag = outStream.status();
        /* check if we can actually write data to the stream; in certain cases we cannot. */
        if (outxfer == EXS_Unknown || outxfer == EXS_BigEndianImplicit)
            errorFlag = EC_IllegalCall;
        else if (itemList->empty())
            errorFlag = EC_CorruptedData;
        else if (errorFlag.good() && fTransferState != ERW_ready)
        {
            /* in this case we can write data to the stream */

            /* if this function was called for the first time for the dataset object, the transferState is */
            /* still set to ERW_init. In this case, we need to validate the meta header information, set the */
            /* item list pointer to the fist element and we need to set the transfer state to ERW_inWork. */
            if (fTransferState == ERW_init)
            {
                validateMetaInfo(outxfer);
                itemList->seek(ELP_first);
                fTransferState = ERW_inWork;
            }
            /* if the transfer state is set to ERW_inWork, we need to write the */
            /* information which is included in this to the buffer which was passed. */
            if (fTransferState == ERW_inWork)
            {
                /* write meta header information */
                errorFlag = metainfo->write(outStream, outxfer, enctype);
                /* recalculate the instance length */
                instanceLength += metainfo->calcElementLength(outxfer, enctype);
                /* if everything is ok, write the data set */
                if (errorFlag.good())
                    errorFlag = dataset->write(outStream, outxfer, enctype, glenc, padenc, padlen,
                                               subPadlen, instanceLength);
                /* if everything is ok, set the transfer state to ERW_ready */
                if (errorFlag.good())
                    fTransferState = ERW_ready;
            }
        }
        /* in case the transfer syntax which shall be used is indeed the */
        /* BigEndianImplicit transfer syntax dump some error information */
        if (outxfer == EXS_BigEndianImplicit)
        {
            ofConsole.lockCerr() << "Error: DcmFileFormat::write() illegal TransferSyntax(BI) used" << endl;
            ofConsole.unlockCerr();
        }
    }
    /* return result value */
    return errorFlag;
}


// ********************************


OFCondition DcmFileFormat::loadFile(const char *fileName,
                                    const E_TransferSyntax readXfer,
                                    const E_GrpLenEncoding groupLength,
                                    const Uint32 maxReadLength,
                                    OFBool isDataset)
{
    if (isDataset)
        return getDataset()->loadFile(fileName, readXfer, groupLength, maxReadLength);

    OFCondition l_error = EC_IllegalParameter;
    /* check parameters first */
    if ((fileName != NULL) && (strlen(fileName) > 0))
    {
        /* open file for input */
        DcmInputFileStream fileStream(fileName);
        /* check stream status */
        l_error = fileStream.status();
        if (l_error.good())
        {
            /* read data from file */
            transferInit();
            l_error = read(fileStream, readXfer, groupLength, maxReadLength);
            transferEnd();
        }
    }
    return l_error;
}


OFCondition DcmFileFormat::saveFile(const char *fileName,
                                    const E_TransferSyntax writeXfer,
                                    const E_EncodingType encodingType,
                                    const E_GrpLenEncoding groupLength,
                                    const E_PaddingEncoding padEncoding,
                                    const Uint32 padLength,
                                    const Uint32 subPadLength,
                                    OFBool isDataset)
{
    if (isDataset)
    {
        return getDataset()->saveFile(fileName, writeXfer, encodingType, groupLength,
            padEncoding, padLength, subPadLength);
    }
    OFCondition l_error = EC_IllegalParameter;
    /* check parameters first */
    if ((fileName != NULL) && (strlen(fileName) > 0))
    {
        /* open file for output */
        DcmOutputFileStream fileStream(fileName);

        /* check stream status */
        l_error = fileStream.status();
        if (l_error.good())
        {
            /* write data to file */
            transferInit();
            l_error = write(fileStream, writeXfer, encodingType, groupLength, padEncoding, padLength, subPadLength);
            transferEnd();
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmFileFormat::insertItem(DcmItem * /*item*/,
                                      const unsigned long /*where*/)
{
    ofConsole.lockCerr() << "Warning: illegal call of DcmFileFormat::insert(DcmItem*,Uin32)" << endl;
    ofConsole.unlockCerr();
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


DcmItem *DcmFileFormat::remove(const unsigned long /*num*/)
{
    ofConsole.lockCerr() << "Warning: illegal call of DcmFileFormat::remove(Uint32)" << endl;
    ofConsole.unlockCerr();
    errorFlag = EC_IllegalCall;
    return (DcmItem*)NULL;
}


// ********************************


DcmItem *DcmFileFormat::remove(DcmItem* /*item*/)
{
    ofConsole.lockCerr() << "Warning: illegal call of DcmFileFormat::remove(DcmItem*)" << endl;
    ofConsole.unlockCerr();
    errorFlag = EC_IllegalCall;
    return (DcmItem*)NULL;
}


// ********************************


OFCondition DcmFileFormat::clear()
{
    ofConsole.lockCerr() << "Warning: illegal call of DcmFileFormat::clear()" << endl;
    ofConsole.unlockCerr();
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


DcmMetaInfo *DcmFileFormat::getMetaInfo()
{
    errorFlag = EC_Normal;
    DcmMetaInfo *meta = (DcmMetaInfo*)NULL;
    if (itemList->seek_to(0) != (DcmItem*)NULL && itemList->get()->ident() == EVR_metainfo)
        meta = (DcmMetaInfo*)(itemList->get());
    else
        errorFlag = EC_IllegalCall;
    return meta;
}


// ********************************


DcmDataset *DcmFileFormat::getDataset()
{
    errorFlag = EC_Normal;
    DcmDataset *data = (DcmDataset*)NULL;
    if (itemList->seek_to(1) != (DcmItem*)NULL && itemList->get()->ident() == EVR_dataset)
        data = (DcmDataset*)(itemList->get());
    else
        errorFlag = EC_IllegalCall;
    return data;
}


// ********************************


DcmDataset *DcmFileFormat::getAndRemoveDataset()
{
    errorFlag = EC_Normal;
    DcmDataset *data = (DcmDataset*)NULL;
    if (itemList->seek_to(1) != (DcmItem*)NULL && itemList->get()->ident() == EVR_dataset)
    {
        data = (DcmDataset*)(itemList->remove());
        DcmDataset *Dataset = new DcmDataset();
        DcmSequenceOfItems::itemList->insert(Dataset, ELP_last);
    }
    else
        errorFlag = EC_IllegalCall;
    return data;
}
