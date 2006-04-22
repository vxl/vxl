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
 *  Purpose:
 *    This file contains the interface to routines which provide
 *    DICOM object encoding/decoding, search and lookup facilities.
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstd.h"
#include "ofstream.h"
#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcswap.h"
#include "dcdebug.h"
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcostrma.h"    /* for class DcmOutputStream */

#define INCLUDE_CSTDIO
#define INCLUDE_IOMANIP
#include "ofstdinc.h"


/*
** Should automatic correction be applied to input data (e.g. stripping
** of padding blanks, removal of blanks in UIDs, etc).
*/
OFGlobal<OFBool> dcmEnableAutomaticInputDataCorrection(OFTrue);

/*
** Handling of illegal odd-length attributes: If flag is true, odd lengths
** are respected (i.e. an odd number of bytes is read from the input stream.)
** After successful reading, padding to even number of bytes is enforced
** by adding a zero pad byte if dcmEnableAutomaticInputDataCorrection is true.
** Otherwise the odd number of bytes remains as read.
**
** If flag is false, old (pre DCMTK 3.5.2) behaviour applies: The length field
** implicitly incremented and an even number of bytes is read from the stream.
*/
OFGlobal<OFBool> dcmAcceptOddAttributeLength(OFTrue);


// ****** public methods **********************************


DcmObject::DcmObject(const DcmTag &tag,
                     const Uint32 len)
  : Tag(tag),
    Length(len),
    fTransferState(ERW_init),
    errorFlag(EC_Normal),
    fTransferredBytes(0)
{
}


DcmObject::DcmObject(const DcmObject &obj)
  : Tag(obj.Tag),
    Length(obj.Length),
    fTransferState(obj.fTransferState),
    errorFlag(obj.errorFlag),
    fTransferredBytes(obj.fTransferredBytes)
{
}


DcmObject::~DcmObject()
{
}


DcmObject &DcmObject::operator=(const DcmObject &obj)
{
    Tag = obj.Tag;
    Length = obj.Length;
    errorFlag = obj.errorFlag;
    fTransferState = obj.fTransferState;
    fTransferredBytes = obj.fTransferredBytes;
    return *this;
}


// ********************************


void DcmObject::transferInit()
{
    fTransferState = ERW_init;
    fTransferredBytes = 0;
}


void DcmObject::transferEnd()
{
    fTransferState = ERW_notInitialized;
}


// ********************************


DcmObject * DcmObject::nextInContainer(const DcmObject * /*obj*/)
{
    return NULL;
}


OFCondition DcmObject::nextObject(DcmStack & /*stack*/,
                                  const OFBool /*intoSub*/)
{
    return EC_TagNotFound;
}


// ********************************


OFCondition DcmObject::search(const DcmTagKey &/*tag*/,
                              DcmStack &/*resultStack*/,
                              E_SearchMode /*mode*/,
                              OFBool /*searchIntoSub*/)
{
   return EC_TagNotFound;
}


OFCondition DcmObject::searchErrors(DcmStack &resultStack)
{
    if (errorFlag.bad())
        resultStack.push(this);
    return errorFlag;
}


// ********************************


OFCondition DcmObject::writeXML(ostream & /*out*/,
                                const size_t /*flags*/)
{
    return EC_IllegalCall;
}


// ***********************************************************
// ****** protected methods **********************************
// ***********************************************************


void DcmObject::printNestingLevel(ostream &out,
                                  const size_t flags,
                                  const int level)
{
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* special treatment for the last entry on the level */
        if (flags & DCMTypes::PF_lastEntry)
        {
            /* show vertical bar for the tree structure */
            for (int i = 2; i < level; i++)
                out << "| ";
            /* show juncture sign for the last entry */
            if (level > 0)
                out << "+ ";
        } else {
            /* show vertical bar for the tree structure */
            for (int i = 1; i < level; i++)
                out << "| ";
        }
    } else {
        /* show nesting level */
        for (int i = 1; i < level; i++)
            out << "  ";
    }
}


void DcmObject::printInfoLineStart(ostream &out,
                                   const size_t flags,
                                   const int level,
                                   DcmTag *tag)
{
    /* default: use object's tag */
    if (tag == NULL)
        tag = &Tag;
    DcmVR vr(tag->getVR());
    /* show nesting level */
    printNestingLevel(out, flags, level);
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* print tag name */
        out << tag->getTagName() << ' ';
        /* add padding spaces if required */
        const signed long padLength = 35 - strlen(tag->getTagName()) - 2 * level;
        if (padLength > 0)
            out << OFString((size_t)padLength, ' ');
    } else {
        /* print line start: tag and VR */
        out << hex << setfill('0') << "("
            << setw(4) << tag->getGTag() << ","
            << setw(4) << tag->getETag() << ") "
            << dec << setfill(' ')
            << vr.getVRName() << " ";
    }
}


void DcmObject::printInfoLineEnd(ostream &out,
                                 const size_t flags,
                                 const unsigned long printedLength,
                                 DcmTag *tag)
{
    unsigned long vm = 0;
    unsigned long length = 0;
    /* default: use object's tag, VM and length */
    if (tag == NULL)
    {
        tag = &Tag;
        vm = getVM();
        length = Length;
    }
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* finish the current line */
        out << endl;
    } else {
        /* fill with spaces if necessary */
        if (printedLength < DCM_OptPrintValueLength)
            out << OFString((size_t)(DCM_OptPrintValueLength - printedLength), ' ');
        /* print line end: length, VM and tag name */
        out << " # ";
        if (length == DCM_UndefinedLength)
            out << "u/l";   // means "undefined/length"
        else
            out << setw(3) << length;
        out << ","
            << setw(2) << vm << " "
            << tag->getTagName() << endl;
    }
}


void DcmObject::printInfoLine(ostream &out,
                              const size_t flags,
                              const int level,
                              const char *info,
                              DcmTag *tag)
{
    /* print tag and VR */
    printInfoLineStart(out, flags, level, tag);
    /* check whether info text fits into the limit */
    unsigned long printedLength = 0;
    /* check for valid info text */
    if (info != NULL)
    {
        /* check info text length */
        printedLength = strlen(info);
        if (printedLength > DCM_OptPrintValueLength)
        {
            /* check whether full info text should be printed */
            if ((flags & DCMTypes::PF_shortenLongTagValues) && (printedLength > DCM_OptPrintLineLength))
            {
                char output[DCM_OptPrintLineLength + 1];
                /* truncate info text and append "..." */
                OFStandard::strlcpy(output, info, (size_t)DCM_OptPrintLineLength - 3 /* for "..." */ + 1);
                OFStandard::strlcat(output, "...", (size_t)DCM_OptPrintLineLength + 1);
                out << output;
                printedLength = DCM_OptPrintLineLength;
            } else
                out << info;
        } else
            out << info;
    }
    /* print length, VM and tag name */
    printInfoLineEnd(out, flags, printedLength, tag);
}


// ********************************


OFCondition DcmObject::writeTag(DcmOutputStream &outStream,
                                const DcmTag &tag,
                                const E_TransferSyntax oxfer)
    /*
     * This function writes the tag information which was passed to the stream. When
     * writing information, the transfer syntax which was passed is accounted for.
     *
     * Parameters:
     *   outStream - [out] The stream that the information will be written to.
     *   tag       - [in] The tag which shall be written.
     *   oxfer     - [in] The transfer syntax which shall be used.
     */
{
    /* create an object which represents the transfer syntax */
    DcmXfer outXfer(oxfer);
    /* determine the byte ordering */
    const E_ByteOrder outByteOrder = outXfer.getByteOrder();
    /* if the byte ordering is unknown, this is an illegal call (return error) */
    if (outByteOrder == EBO_unknown)
        return EC_IllegalCall;
    /* determine the group number, mind the transfer syntax and */
    /* write the group number value (2 bytes) to the stream */
    Uint16 groupTag = tag.getGTag();
    swapIfNecessary(outByteOrder, gLocalByteOrder, &groupTag, 2, 2);
    outStream.write(&groupTag, 2);
    /* determine the element number, mind the transfer syntax and */
    /* write the element number value (2 bytes) to the stream */
    Uint16 elementTag = tag.getETag();    // 2 byte length;
    swapIfNecessary(outByteOrder, gLocalByteOrder, &elementTag, 2, 2);
    outStream.write(&elementTag, 2);
    /* if the stream reports an error return this error, else return ok */
    return outStream.status();
}


OFCondition DcmObject::writeTagAndLength(DcmOutputStream &outStream,
                                         const E_TransferSyntax oxfer,
                                         Uint32 &writtenBytes) const
    /*
     * This function writes this DICOM object's tag and length information to the stream. When
     * writing information, the transfer syntax which was passed is accounted for. If the transfer
     * syntax shows an explicit value representation, the data type of this object is also written
     * to the stream. In general, this function follows the rules which are specified in the DICOM
     * standard (see DICOM standard (year 2000) part 5, section 7) (or the corresponding section
     * in a later version of the standard) concerning the encoding of a data set which shall be
     * transmitted.
     *
     * Parameters:
     *   outStream    - [out] The stream that the information will be written to.
     *   oxfer        - [in] The transfer syntax which shall be used.
     *   writtenBytes - [out] Contains in the end the amount of bytes which have been written to the stream.
     */
{
    /* check the error status of the stream. If it is not ok, nothing can be done */
    OFCondition l_error = outStream.status();
    if (l_error.bad())
    {
        writtenBytes = 0;
    } else {
        /* if the stream is ok, we need to do something */

        /* write the tag information (a total of 4 bytes, group number and element */
        /* number) to the stream. Mind the transfer syntax's byte ordering. */
        l_error = writeTag(outStream, Tag, oxfer);
        writtenBytes = 4;

        /* create an object which represents the transfer syntax */
        DcmXfer oxferSyn(oxfer);

        /* determine the byte ordering */
        const E_ByteOrder oByteOrder = oxferSyn.getByteOrder();

        /* if the byte ordering is unknown, this is an illegal call (return error) */
        if (oByteOrder == EBO_unknown)
            return EC_IllegalCall;

        /* if the transfer syntax is one with explicit value representation */
        /* this value's data type also has to be written to the stream. Do so */
        /* and also write the length information to the stream. */
        if (oxferSyn.isExplicitVR())
        {
            /* Create an object that represents this object's data type */
            DcmVR myvr(getVR());

            /* getValidEVR() will convert datatype "UN" to "OB" if generation of "UN" is disabled */
            DcmEVR vr = myvr.getValidEVR();

            /* get name of data type */
            const char *vrname = myvr.getValidVRName();

            /* write data type name to the stream (a total of 2 bytes) */
            outStream.write(vrname, 2);
            writtenBytes += 2;

            /* create another data type object on the basis of the above created object */
            DcmVR outvr(vr);

            /* in case we are dealing with a transfer syntax with explicit VR (see if above) */
            /* and the actual VR uses extended length encoding (see DICOM standard (year 2000) */
            /* part 5, section 7.1.2) (or the corresponding section in a later version of the */
            /* standard) we have to add 2 reserved bytes (set to a value of 00H) to the data */
            /* type field and the actual length field is 4 bytes wide. Write the corresponding */
            /* information to the stream. */
            if (outvr.usesExtendedLengthEncoding())
            {
                Uint16 reserved = 0;
                outStream.write(&reserved, 2);                                      // write 2 reserved bytes to stream
                Uint32 valueLength = Length;                                        // determine length
                swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);   // mind transfer syntax
                outStream.write(&valueLength, 4);                                   // write length, 4 bytes wide
                writtenBytes += 6;                                                  // remember that 6 bytes were written in total
            }
            /* in case that we are dealing with a transfer syntax with explicit VR (see if above) and */
            /* the actual VR does not use extended length encoding (see DICOM standard (year 2000) */
            /* part 5, section 7.1.2) (or the corresponding section in a later version of the standard) */
            /* we do not have to add reserved bytes to the data type field and the actual length field */
            /* is 2 bytes wide. Write the corresponding information to the stream. */
            else {
                Uint16 valueLength = (Uint16)Length;                                // determine length
                swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 2, 2);   // mind transfer syntax
                outStream.write(&valueLength, 2);                                   // write length, 2 bytes wide
                writtenBytes += 2;                                                  // remember that 2 bytes were written in total
            }
         }
         /* if the transfer syntax is one with implicit value representation this value's data type */
         /* does not have to be written to the stream. Only the length information has to be written */
         /* to the stream. According to the DICOM standard the length field is in this case always 4 */
         /* byte wide. (see DICOM standard (year 2000) part 5, section 7.1.2) (or the corresponding */
         /* section in a later version of the standard) */
         else {
            Uint32 valueLength = Length;                                          // determine length
            swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);     // mind transfer syntax
            outStream.write(&valueLength, 4);                                     // write length, 4 bytes wide
            writtenBytes += 4;                                                    // remember that 4 bytes were written in total
         }
    }

    /* return result */
    return l_error;
}


OFBool DcmObject::isSignable() const
{
    return Tag.isSignable();
}


OFBool DcmObject::containsUnknownVR() const
{
    return Tag.isUnknownVR();
}
