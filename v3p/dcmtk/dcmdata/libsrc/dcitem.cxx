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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: class DcmItem
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcitem.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "ofstdinc.h"

#include "dcdebug.h"
#include "dcdefine.h"    /* for memzero() */
#include "dcdeftag.h"    /* for name constants */
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcobject.h"
#include "dcostrma.h"    /* for class DcmOutputStream */
#include "dcovlay.h"
#include "dcpixel.h"
#include "dcsequen.h"
#include "dcswap.h"
#include "dcvr.h"
#include "dcvrae.h"
#include "dcvras.h"
#include "dcvrat.h"
#include "dcvrcs.h"
#include "dcvrda.h"
#include "dcvrds.h"
#include "dcvrdt.h"
#include "dcvrfd.h"
#include "dcvrfl.h"
#include "dcvris.h"
#include "dcvrlo.h"
#include "dcvrlt.h"
#include "dcvrobow.h"
#include "dcvrof.h"
#include "dcvrpn.h"
#include "dcvrsh.h"
#include "dcvrsl.h"
#include "dcvrss.h"
#include "dcvrst.h"
#include "dcvrtm.h"
#include "dcvrui.h"
#include "dcvrul.h"
#include "dcvrulup.h"
#include "dcvrus.h"
#include "dcvrut.h"
#include "dcxfer.h"
#include "ofstream.h"
#include "ofstring.h"


// ********************************


DcmItem::DcmItem()
  : DcmObject(ItemTag),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(0),
    privateCreatorCache()
{
    elementList = new DcmList;
}


DcmItem::DcmItem(const DcmTag &tag,
                 const Uint32 len)
  : DcmObject(tag, len),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(0),
    privateCreatorCache()
{
    elementList = new DcmList;
}


DcmItem::DcmItem(const DcmItem &old)
  : DcmObject(old),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(old.fStartPosition),
    privateCreatorCache()
{
    elementList = new DcmList;
    switch (old.ident())
    {
        case EVR_item:
        case EVR_dirRecord:
        case EVR_dataset:
        case EVR_metainfo:
            if (!old.elementList->empty())
            {
                DcmObject *oldDO;
                DcmObject *newDO;
                elementList->seek(ELP_first);
                old.elementList->seek(ELP_first);
                do {
                    oldDO = old.elementList->get();
                    newDO = copyDcmObject(oldDO);
                    elementList->insert(newDO, ELP_next);
                } while (old.elementList->seek(ELP_next));
            }
            break;
        default: // should never happen
            break;
    }
}


DcmItem::~DcmItem()
{
    DcmObject *dO;
    elementList->seek(ELP_first);
    while (!elementList->empty())
    {
        dO = elementList->remove();
        if (dO != (DcmObject*)NULL)
            delete dO;
    }
    delete elementList;
}


// ********************************


OFBool DcmItem::foundVR(char *atposition)
{
    char c1 =  atposition[0];
    char c2 = atposition[1];
    OFBool valid = OFFalse;

    if (isalpha(c1) && isalpha(c2))
    {
        char vrName[3];
        vrName[0] = c1;
        vrName[1] = c2;
        vrName[2] = '\0';

        /* is this VR name a standard VR descriptor */
        DcmVR vr(vrName);
        valid = vr.isStandard();
    } else {
        /* cannot be a valid VR name since non-characters */
        valid = OFFalse;
    }
    return valid;
}


// ********************************


E_TransferSyntax DcmItem::checkTransferSyntax(DcmInputStream & inStream)
{
    E_TransferSyntax transferSyntax;
    char tagAndVR[6];

    /* read 6 bytes from the input stream (try to read tag and VR (data type)) */
    inStream.mark();
    inStream.read(tagAndVR, 6);               // check Tag & VR
    inStream.putback();

    /* create two tag variables (one for little, one for big */
    /* endian) in order to figure out, if there is a valid tag */
    char c1 = tagAndVR[0];
    char c2 = tagAndVR[1];
    char c3 = tagAndVR[2];
    char c4 = tagAndVR[3];
    Uint16 t1 = (c1 & 0xff) + ((c2 & 0xff) << 8);  // explicit little endian
    Uint16 t2 = (c3 & 0xff) + ((c4 & 0xff) << 8);  // conversion
    DcmTag taglittle(t1, t2);
    DcmTag tagbig(swapShort(t1), swapShort(t2));

    /* now we want to determine the transfer syntax which was used to code the information in the stream. */
    /* The decision is based on two questions: a) Did we encounter a valid tag? and b) Do the last 2 bytes */
    /* which were read from the stream represent a valid VR? In certain special cases, where the transfer */
    /* cannot be determined without doubt, we want to guess the most probable transfer syntax. */

    /* if both tag variables show an error, we encountered an invalid tag */
    if ((taglittle.error().bad()) && (tagbig.error().bad()))
    {
        /* in case we encountered an invalid tag, we want to assume that the used transfer syntax */
        /* is a little endian transfer syntax. Now we have to figure out, if it is an implicit or */
        /* explicit transfer syntax. Hence, check if the last 2 bytes represent a valid VR. */
        if (foundVR(&tagAndVR[4]))
        {
            /* if the last 2 bytes represent a valid VR, we assume that the used */
            /* transfer syntax is the little endian explicit transfer syntax. */
            transferSyntax = EXS_LittleEndianExplicit;
        } else {
            /* if the last 2 bytes did not represent a valid VR, we assume that the */
            /* used transfer syntax is the little endian implicit transfer syntax. */
            transferSyntax = EXS_LittleEndianImplicit;
        }
    }
    /* if at least one tag variable did not show an error, we encountered a valid tag */
    else
    {
        /* in case we encountered a valid tag, we want to figure out, if it is an implicit or */
        /* explicit transfer syntax. Hence, check if the last 2 bytes represent a valid VR. */
        if (foundVR(&tagAndVR[4]))
        {
            /* having figured out that the last 2 bytes represent a valid */
            /* VR, we need to find out which of the two tags was valid */
            if (taglittle.error().bad())
            {
                /* if the litte endian tag was invalid, the transfer syntax is big endian explicit */
                transferSyntax = EXS_BigEndianExplicit;
            }
            else if (tagbig.error().bad())
            {
                /* if the big endian tag was invalid, the transfer syntax is little endian explicit */
                transferSyntax = EXS_LittleEndianExplicit;
            } else {
                /* if both tags were valid, we take a look at the group numbers. Since */
                /* group 0008 is much more probable than group 0800 for the first tag */
                /* we specify the following: */
                if ((taglittle.getGTag() > 0xff)&&(tagbig.getGTag() <= 0xff)) transferSyntax = EXS_BigEndianExplicit;
                else transferSyntax = EXS_LittleEndianExplicit;
            }
        } else {
            /* having figured out that the last 2 bytes do not represent a */
            /* valid VR, we need to find out which of the two tags was valid */
            if (taglittle.error().bad())
            {
                /* if the litte endian tag was invalid, the transfer syntax is big endian implicit */
                transferSyntax = EXS_BigEndianImplicit;
            }
            else if (tagbig.error().bad())
            {
                /* if the big endian tag was invalid, the transfer syntax is little endian implicit */
                transferSyntax = EXS_LittleEndianImplicit;
            } else {
                /* if both tags were valid, we take a look at the group numbers. Since */
                /* group 0008 is much more probable than group 0800 for the first tag */
                /* we specify the following: */
                if ((taglittle.getGTag() > 0xff)&&(tagbig.getGTag() <= 0xff)) transferSyntax = EXS_BigEndianImplicit;
                else transferSyntax = EXS_LittleEndianImplicit;
            }
        }
    }
    /* dump information on a certain debug level */
    debug(3, ("found TransferSyntax=(%s)", DcmXfer(transferSyntax).getXferName()));

    /* return determined transfer syntax */
    return transferSyntax;
} // DcmItem::checkTransferSyntax


// ********************************


DcmObject* DcmItem::copyDcmObject(DcmObject *oldObj)
{
    DcmObject *newObj = (DcmObject*)NULL;
    switch (oldObj->ident())
    {
         // byte strings:
        case EVR_AE :
            newObj = new DcmApplicationEntity(*(DcmApplicationEntity*)(oldObj));
            break;
        case EVR_AS :
            newObj = new DcmAgeString(*(DcmAgeString*)oldObj);
            break;
        case EVR_CS :
            newObj = new DcmCodeString(*(DcmCodeString*)oldObj);
            break;
        case EVR_DA :
            newObj = new DcmDate(*(DcmDate*)oldObj);
            break;
        case EVR_DS :
            newObj = new DcmDecimalString(*(DcmDecimalString*)oldObj);
            break;
        case EVR_DT :
            newObj = new DcmDateTime(*(DcmDateTime*)oldObj);
            break;
        case EVR_IS :
            newObj = new DcmIntegerString(*(DcmIntegerString*)oldObj);
            break;
        case EVR_TM :
            newObj = new DcmTime(*(DcmTime*)oldObj);
            break;
        case EVR_UI :
            newObj = new DcmUniqueIdentifier(*(DcmUniqueIdentifier*)oldObj);
            break;

        // character strings:
        case EVR_LO :
            newObj = new DcmLongString(*(DcmLongString*)oldObj);
            break;
        case EVR_LT :
            newObj = new DcmLongText(*(DcmLongText*)oldObj);
            break;
        case EVR_PN :
            newObj = new DcmPersonName(*(DcmPersonName*)oldObj);
            break;
        case EVR_SH :
            newObj = new DcmShortString(*(DcmShortString*)oldObj);
            break;
        case EVR_ST :
            newObj = new DcmShortText(*(DcmShortText*)oldObj);
            break;
        case EVR_UT:
            newObj = new DcmUnlimitedText(*(DcmUnlimitedText*)oldObj);
            break;

        // depending on byte order:
        case EVR_AT :
            newObj = new DcmAttributeTag(*(DcmAttributeTag*)oldObj);
            break;
        case EVR_SS :
            newObj = new DcmSignedShort(*(DcmSignedShort*)oldObj);
            break;
        case EVR_xs :
        case EVR_US :
            newObj = new DcmUnsignedShort(*(DcmUnsignedShort*)oldObj);
            break;
        case EVR_SL :
            newObj = new DcmSignedLong(*(DcmSignedLong*)oldObj);
            break;
        case EVR_up : // for (0004,eeee)
            newObj = new DcmUnsignedLongOffset(*(DcmUnsignedLongOffset*)oldObj);
            break;
        case EVR_UL :
            newObj = new DcmUnsignedLong(*(DcmUnsignedLong*)oldObj);
            break;
        case EVR_FL :
            newObj = new DcmFloatingPointSingle(*(DcmFloatingPointSingle*)oldObj);
            break;
        case EVR_FD :
            newObj = new DcmFloatingPointDouble(*(DcmFloatingPointDouble*)oldObj);
            break;
        case EVR_OF :
            newObj = new DcmOtherFloat(*(DcmOtherFloat*)oldObj);
            break;

        // sequence:
        case EVR_SQ :
            newObj = new DcmSequenceOfItems(*(DcmSequenceOfItems*)oldObj);
            break;

        // 8 or 16 bit data:
        case EVR_OB :
        case EVR_OW :
        case EVR_ox :
        case EVR_UN :
        case EVR_UNKNOWN :   // treat unknown elements as OB
        case EVR_UNKNOWN2B : // treat unknown elements as OB
            newObj = new DcmOtherByteOtherWord(*(DcmOtherByteOtherWord*)oldObj);
            break;

        // pixel data
        case EVR_PixelData:
            newObj = new DcmPixelData(*(DcmPixelData *)oldObj);
            break;

        // overlay data
        case EVR_OverlayData:
            newObj = new DcmOverlayData(*(DcmOverlayData *)oldObj);
            break;

        case EVR_na :
        default :
            ofConsole.lockCerr() << "Warning: DcmItem: unable to copy unsupported element("
                                 << hex << setfill('0')
                                 << setw(4) << oldObj->getGTag() << ","
                                 << setw(4) << oldObj->getETag()
                                 << dec << setfill(' ')
                                 << ") with ident()=" << DcmVR(oldObj->ident()).getVRName() << endl;
            ofConsole.unlockCerr();
            break;
    }
    return newObj;
}


// ********************************


DcmEVR DcmItem::ident() const
{
    return EVR_item;
}


unsigned long DcmItem::getVM()
{
    return 1;
}


// ********************************


void DcmItem::print(ostream &out,
                    const size_t flags,
                    const int level,
                    const char *pixelFileName,
                    size_t *pixelCounter)
{
    /* print item start line */
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* empty text */
        printInfoLine(out, flags, level);
        /* print item content */
        if (!elementList->empty())
        {
            /* reset internal flags */
            const size_t newFlags = flags & ~DCMTypes::PF_lastEntry;
            /* print attributes on this level */
            DcmObject *dO;
            elementList->seek(ELP_first);
            OFBool ok;
            do {
                dO = elementList->get();
                ok = (elementList->seek(ELP_next) != NULL);
                if (!ok)
                    dO->print(out, newFlags | DCMTypes::PF_lastEntry, level + 1, pixelFileName, pixelCounter);
                else
                    dO->print(out, newFlags, level + 1, pixelFileName, pixelCounter);
            } while (ok);
        }
    } else {
        OFOStringStream oss;
        oss << "(Item with ";
        if (Length == DCM_UndefinedLength)
            oss << "undefined";
        else
            oss << "explicit";
        oss << " length #=" << card() << ")" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print item content */
        if (!elementList->empty())
        {
            DcmObject *dO;
            elementList->seek(ELP_first);
            do {
                dO = elementList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (elementList->seek(ELP_next));
        }
        /* print item end line */
        DcmTag delimItemTag(DCM_ItemDelimitationItem);
        if (Length == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "(ItemDelimitationItem)", &delimItemTag);
        else
            printInfoLine(out, flags, level, "(ItemDelimitationItem for re-encoding)", &delimItemTag);
    }
}


// ********************************


OFCondition DcmItem::writeXML(ostream &out,
                              const size_t flags)
{
    /* XML start tag for "item" */
    out << "<item";
    /* cardinality (number of attributes) = 1..n */
    out << " card=\"" << card() << "\"";
    /* value length in bytes = 0..max (if not undefined) */
    if (Length != DCM_UndefinedLength)
        out << " len=\"" << Length << "\"";
    out << ">" << endl;
    /* write item content */
    if (!elementList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            dO->writeXML(out, flags);
        } while (elementList->seek(ELP_next));
    }
    /* XML end tag for "item" */
    out << "</item>" << endl;
    /* always report success */
    return EC_Normal;
}


// ********************************


OFBool DcmItem::canWriteXfer(const E_TransferSyntax newXfer,
                             const E_TransferSyntax oldXfer)
{
    OFBool canWrite = OFTrue;
    if (newXfer == EXS_Unknown)
        canWrite = OFFalse;
    else if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            canWrite = dO->canWriteXfer(newXfer, oldXfer);
        } while (elementList->seek(ELP_next) && canWrite);
    }
    return canWrite;
}


// ********************************


Uint32 DcmItem::calcElementLength(const E_TransferSyntax xfer,
                                  const E_EncodingType enctype)
{
    Uint32 itemlen = 0;
    DcmXfer xferSyn(xfer);
    itemlen = getLength(xfer, enctype) + xferSyn.sizeofTagHeader(getVR());
    if (enctype == EET_UndefinedLength)
        itemlen += 8;
    return itemlen;
}


// ********************************


Uint32 DcmItem::getLength(const E_TransferSyntax xfer,
                          const E_EncodingType enctype)
{
    Uint32 itemlen = 0;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            itemlen += dO->calcElementLength(xfer, enctype);
        } while (elementList->seek(ELP_next));
    }
    return itemlen;
}


// ********************************


OFCondition DcmItem::computeGroupLengthAndPadding(const E_GrpLenEncoding glenc,
                                                  const E_PaddingEncoding padenc,
                                                  const E_TransferSyntax xfer,
                                                  const E_EncodingType enctype,
                                                  const Uint32 padlen,
                                                  const Uint32 subPadlen,
                                                  Uint32 instanceLength)
{
    /* if certain conditions are met, this is considered to be an illegal call. */
    if ((padenc == EPD_withPadding && (padlen % 2 || subPadlen % 2)) ||
        ((glenc == EGL_recalcGL || glenc == EGL_withGL ||
          padenc == EPD_withPadding) && xfer == EXS_Unknown))
        return EC_IllegalCall;

    /* if the caller specified that group length tags and padding */
    /* tags are not supposed to be changed, there is nothing to do. */
    if (glenc == EGL_noChange && padenc == EPD_noChange)
        return EC_Normal;

    /* if we get to this point, we need to do something. First of all, set the error indicator to normal. */
    OFCondition l_error = EC_Normal;

    /* if there are elements in this item... */
    if (!elementList->empty())
    {
        /* initialize some variables */
        DcmObject *dO;
        OFBool beginning = OFTrue;
        Uint16 lastGrp = 0x0000;
        Uint16 actGrp;
        DcmUnsignedLong * actGLElem = NULL;
        DcmUnsignedLong * paddingGL = NULL;
        Uint32 grplen = 0;
        DcmXfer xferSyn(xfer);

        /* determine the current seek mode and set the list pointer to the first element */
        E_ListPos seekmode = ELP_next;
        elementList->seek(ELP_first);

        /* start a loop: we want to go through all elements as long as everything is okay */
        do
        {
            /* set the seek mode to "next" again, in case it has been modified in the last iteration */
            seekmode = ELP_next;

            /* get the current element and assign it to a local variable */
            dO = elementList->get();

            /* if the current element is a sequence, compute group length and padding for the sub sequence */
            if (dO->getVR() == EVR_SQ)
            {
                Uint32 templen = instanceLength + xferSyn.sizeofTagHeader(EVR_SQ);
                l_error =
                    ((DcmSequenceOfItems*)dO)->computeGroupLengthAndPadding
                    (glenc, padenc, xfer, enctype, subPadlen, subPadlen,
                     templen);
            }

            /* if everything is ok so far */
            if (l_error.good())
            {
                /* in case one of the following two conditions is met */
                /*  (i) the caller specified that we want to add or remove group length elements and the current */
                /*      element's tag shows that it is a group length element (tag's element number equals 0x0000) */
                /*  (ii) the caller specified that we want to add or remove padding elements and the current */
                /*      element's tag shows that it is a padding element (tag is (0xfffc,0xfffc) */
                /* then we want to delete the current (group length or padding) element */
                if (((glenc ==  EGL_withGL || glenc == EGL_withoutGL) && dO->getETag() == 0x0000) ||
                    (padenc != EPD_noChange && dO->getTag() == DCM_DataSetTrailingPadding))
                {
                    delete elementList->remove();
                    seekmode = ELP_atpos;           // remove = 1 forward
                    dO = NULL;
                }
                /* if the above mentioned conditions are not met but the caller specified that we want to add group */
                /* length tags for every group or that we want to recalculate values for existing group length tags */
                else  if (glenc == EGL_withGL || glenc == EGL_recalcGL)
                {
                    /* we need to determine the current element's group number */
                    actGrp = dO->getGTag();

                    /* and if the group number is different from the last remembered group number or */
                    /* if this id the very first element that is treated then we've found a new group */
                    if (actGrp!=lastGrp || beginning) // new Group found
                    {
                        /* set beginning to false in order to specify that the */
                        /* very first element has already been treated */
                        beginning = OFFalse;

                        /* if the current element is a group length element) and it's data type */
                        /* is not UL replace this element with one that has a UL datatype since */
                        /* group length elements are supposed to have this data type */
                        if (dO->getETag() == 0x0000 && dO->ident() != EVR_UL)
                        {
                            delete elementList->remove();
                            DcmTag tagUL(actGrp, 0x0000, EVR_UL);
                            DcmUnsignedLong *dUL = new DcmUnsignedLong(tagUL);
                            elementList->insert(dUL, ELP_prev);
                            dO = dUL;
                            ofConsole.lockCerr() << "Info: DcmItem::computeGroupLengthAndPadding()" << endl
                                                 << " Group Length with VR other than UL found, corrected." << endl;
                            ofConsole.unlockCerr();
                        }
                        /* if the above mentioned condition is not met but the caller specified */
                        /* that we want to add group length elements, we need to add such an element */
                        else if (glenc == EGL_withGL)
                        {
                            // Create GroupLength element
                            DcmTag tagUL(actGrp, 0x0000, EVR_UL);
                            DcmUnsignedLong *dUL = new DcmUnsignedLong(tagUL);
                            // insert new GroupLength element
                            elementList->insert(dUL, ELP_prev);
                            dO = dUL;
                        }

                        /* in case we want to add padding elements and the current element is a */
                        /* padding element we want to remember the padding element so that the */
                        /* group length of this element can be stored later */
                        if (padenc == EPD_withPadding && actGrp == 0xfffc)
                            paddingGL = (DcmUnsignedLong *)dO;

                        /* if actGLElem conatins a valid pointer it was set in one of the last iterations */
                        /* to the group lenght element of the last group. We need to write the current computed */
                        /* group length value to this element. */
                        if (actGLElem != (DcmUnsignedLong*)NULL)
                        {
                            actGLElem->putUint32(grplen);
                            debug(2, ("DcmItem::computeGroupLengthAndPadding() Length of Group 0x%4.4x len=%lu", actGLElem->getGTag(), grplen));
                        }

                        /* set the group length value to 0 since it is the beginning of the new group */
                        grplen = 0;

                        /* if the current element is a group length element, remember its address for later */
                        /* (we need to assign the group length value to this element in a subsequent iteration) */
                        /* in case the current element (at the beginning of the group) is not a group length */
                        /* element, set the actGLElem pointer to NULL. */
                        if (dO->getETag() == 0x0000)
                            actGLElem = (DcmUnsignedLong*)dO;
                        else
                            actGLElem = NULL;
                    }
                    /* if this is not a new group, calculate the element's length and add it */
                    /* to the currently computed group length value */
                    else
                        grplen += dO->calcElementLength(xfer, enctype);

                    /* remember the current element's group number so that it is possible to */
                    /* figure out if a new group is treated in the following iteration */
                    lastGrp = actGrp;
                }
            }
        } while (l_error.good() && elementList->seek(seekmode));

        /* if there was no error and the caller specified that we want to add or recalculate */
        /* group length tags and if actGLElem has a valid value, we need to add the above */
        /* computed group length value to the last group's group length element */
        if (l_error.good() && (glenc == EGL_withGL || glenc == EGL_recalcGL) && actGLElem)
            actGLElem->putUint32(grplen);

        /* if the caller specified that we want to add padding elements and */
        /* if the length up to which shall be padded does not equal 0 we might */
        /* have to add a padding element */
        if (padenc == EPD_withPadding && padlen)
        {
            /* calculate how much space the entire padding element is supposed to occupy */
            Uint32 padding;
            if (ident() == EVR_dataset)
            {
                instanceLength += calcElementLength(xfer, enctype);
                padding = padlen - (instanceLength % padlen);
            } else
                padding = padlen - (getLength(xfer, enctype) % padlen);

            /* if now padding does not equal padlen we need to create a padding element. (if both values are equal */
            /* the element does have the exact required padlen length and does not need a padding element.) */
            if (padding != padlen)
            {
                /* Create new padding element */
                DcmOtherByteOtherWord * paddingEl = new DcmOtherByteOtherWord(DCM_DataSetTrailingPadding);

                /* calculate the length of the new element */
                Uint32 tmplen = paddingEl->calcElementLength(xfer, enctype);

                /* in case padding is smaller than the header of the padding element, we */
                /* need to increase padding (the value which specifies how much space the */
                /* entire padding element is supposed to occupy) until it is no longer smaller */
                while (tmplen > padding)
                    padding += padlen;

                /* determine the amount of bytes that have to be added to the */
                /* padding element so that it has the correct size */
                padding -= tmplen;

                /* create an array of a corresponding size and set the arrayfields */
                Uint8 * padBytes = new Uint8[padding];
                memzero(padBytes, size_t(padding));

                /* set information in the above created padding element (size and actual value) */
                paddingEl->putUint8Array(padBytes, padding);

                /* delete the above created array */
                delete[] padBytes;

                /* insert the padding element into this */
                insert(paddingEl);

                /* finally we need to update the group length for the padding element if it exists */
                if (paddingGL)
                {
                    Uint32 len;
                    paddingGL->getUint32(len);
                    len += paddingEl->calcElementLength(xfer, enctype);
                    paddingGL->putUint32(len);
                }
            }
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmItem::readTagAndLength(DcmInputStream &inStream,
                                      const E_TransferSyntax xfer,
                                      DcmTag &tag,
                                      Uint32 &length,
                                      Uint32 &bytesRead)
{
    OFCondition l_error = EC_Normal;
    Uint32 valueLength = 0;
    DcmEVR nxtobj = EVR_UNKNOWN;
    Uint16 groupTag = 0xffff;
    Uint16 elementTag = 0xffff;

    /* Create a DcmXfer object based on the transfer syntax which was passed */
    DcmXfer xferSyn(xfer);

    /* dump some information if required */
    debug(4, ("DcmItem::readTagAndLength() read transfer syntax %s", xferSyn.getXferName()));

    /* bail out if at end of stream */
    if (inStream.eos())
        return EC_EndOfStream;

    /* check if either 4 (for implicit transfer syntaxes) or 6 (for explicit transfer */
    /* syntaxes) bytes are available in (i.e. can be read from) inStream. if an error */
    /* occurred while performing this check return this error */
    if (inStream.avail() < (xferSyn.isExplicitVR() ? 6u:4u))
        return EC_StreamNotifyClient;

    /* determine the byte ordering of the transfer syntax which was passed; */
    /* if the byte ordering is unknown, this is an illegal call. */
    const E_ByteOrder byteOrder = xferSyn.getByteOrder();
    if (byteOrder == EBO_unknown)
        return EC_IllegalCall;

    /* read tag information (4 bytes) from inStream and create a corresponding DcmTag object */
    inStream.mark();
    inStream.read(&groupTag, 2);
    inStream.read(&elementTag, 2);
    swapIfNecessary(gLocalByteOrder, byteOrder, &groupTag, 2, 2);
    swapIfNecessary(gLocalByteOrder, byteOrder, &elementTag, 2, 2);
    // tag has been read
    bytesRead = 4;
    DcmTag newTag(groupTag, elementTag);

    /* if the transfer syntax which was passed is an explicit VR syntax and if the current */
    /* item is not a delimitation item (note that delimitation items do not have a VR), go */
    /* ahead and read 2 bytes from inStream. These 2 bytes contain this item's VR value. */
    if (xferSyn.isExplicitVR() && newTag.getEVR() != EVR_na)
    {
        char vrstr[3];
        vrstr[2] = '\0';

        /* read 2 bytes */
        inStream.read(vrstr, 2);

        /* create a corresponding DcmVR object */
        DcmVR vr(vrstr);

        /* if the VR which was read is not a standard VR, print a warning */
        if (!vr.isStandard())
        {
            ostream &localCerr = ofConsole.lockCerr();
            localCerr << "Warning: parsing attribute: " << newTag.getXTag() <<
                " non-standard VR encountered: '" << vrstr << "', assuming ";
            if (vr.usesExtendedLengthEncoding())
                localCerr << "4 byte length field" << endl;
            else
                localCerr << "2 byte length field" << endl;
            ofConsole.unlockCerr();
        }

        /* set the VR which was read in the above created tag object. */
        newTag.setVR(vr);

        /* increase counter by 2 */
        bytesRead += 2;
    }

    /* special handling for private elements */
    if ((newTag.getGroup() & 1) && (newTag.getElement() >= 0x1000))
    {
        const char *pc = privateCreatorCache.findPrivateCreator(newTag);
        if (pc)
        {
            // we have a private creator for this element
            newTag.setPrivateCreator(pc);

            if (xferSyn.isImplicitVR())
            {
                // try to update VR from dictionary now that private creator is known
                newTag.lookupVRinDictionary();
            }
        }
    }

    /* determine this item's VR */
    nxtobj = newTag.getEVR();

    /* the next thing we want to do is read the value in the length field from inStream. */
    /* determine if there is a corresponging amount of bytes (for the length field) still */
    /* available in inStream. if not, return an error. */
    if (inStream.avail() < xferSyn.sizeofTagHeader(nxtobj) - bytesRead)
    {
        inStream.putback();    // the UnsetPutbackMark is in readSubElement
        bytesRead = 0;
        l_error = EC_StreamNotifyClient;
        return l_error;
    }

    /* read the value in the length field. In some cases, it is 4 bytes wide, in other */
    /* cases only 2 bytes (see DICOM standard (year 2000) part 5, section 7.1.1) (or the */
    /* corresponding section in a later version of the standard) */
    if (xferSyn.isImplicitVR() || nxtobj == EVR_na)   //note that delimitation items don't have a VR
    {
        inStream.read(&valueLength, 4);            //length field is 4 bytes wide
        swapIfNecessary(gLocalByteOrder, byteOrder, &valueLength, 4, 4);
        bytesRead += 4;
    } else {                                       //the transfer syntax is explicit VR
        DcmVR vr(newTag.getEVR());
        if (vr.usesExtendedLengthEncoding())
        {
            Uint16 reserved;
            inStream.read(&reserved, 2);           // 2 reserved bytes
            inStream.read(&valueLength, 4);        // length field is 4 bytes wide
            swapIfNecessary(gLocalByteOrder, byteOrder, &valueLength, 4, 4);
            bytesRead += 6;
        } else {
            Uint16 tmpValueLength;
            inStream.read(&tmpValueLength, 2);     // length field is 2 bytes wide
            swapIfNecessary(gLocalByteOrder, byteOrder, &tmpValueLength, 2, 2);
            bytesRead += 2;
            valueLength = tmpValueLength;
        }
    }
    /* if the value in length is odd, print an error message */
    if ((valueLength & 1)&&(valueLength != (Uint32)-1))
    {
        ofConsole.lockCerr() << "Warning: parse error in DICOM object: length of tag " << newTag << " is odd" << endl;
        ofConsole.unlockCerr();
    }
    /* assign values to out parameter */
    length = valueLength;
    tag = newTag;

    /* return return value */
    return l_error;
}


// ********************************


OFCondition DcmItem::readSubElement(DcmInputStream &inStream,
                                    DcmTag &newTag,
                                    const Uint32 newLength,
                                    const E_TransferSyntax xfer,
                                    const E_GrpLenEncoding glenc,
                                    const Uint32 maxReadLength)
{
    DcmElement *subElem = NULL;

    /* create a new DcmElement* object with corresponding tag and */
    /* length; the object will be accessible through subElem */
    OFCondition l_error = newDicomElement(subElem, newTag, newLength);

    /* if no error occurred and subElem does not equal NULL, go ahead */
    if (l_error.good() && subElem != (DcmElement*)NULL)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture

        /* insert the new element into the (sorted) element list and */
        /* assign information which was read from the instream to it */
        subElem->transferInit();
        /* we need to read the content of the attribute, no matter if */
        /* inserting the attribute succeeds or fails */
        l_error = subElem->read(inStream, xfer, glenc, maxReadLength);
        // try to insert element into item. Note that
        // "elementList->insert(subElem, ELP_next)" would be faster,
        // but this is better since this insert-function creates a
        // sorted element list.
        // We insert the element even if subElem->read() reported an error
        // because otherwise I/O suspension would fail.
        OFCondition temp_error = insert(subElem, OFFalse, OFTrue);

        if (temp_error.bad())
        {
            // produce diagnostics
            ofConsole.lockCerr() << "Warning: element " << newTag
               << " found twice in one dataset/item, ignoring second entry" << endl;
            ofConsole.unlockCerr();
            delete subElem;
        }
    }
    /* else if an error occurred, try to recover from this error */
    else if (l_error == EC_InvalidTag)
    {
        /* This is the second Putback operation on the putback mark in */
        /* readTagAndLength but it is impossible that both can be executed */
        /* without setting the Mark twice. */
        inStream.putback();
        ofConsole.lockCerr() << "Warning: DcmItem: parse error occurred: " <<  newTag << endl;
        ofConsole.unlockCerr();
    }
    else if (l_error != EC_ItemEnd)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
        ofConsole.lockCerr() << "Error: DcmItem: cannot create SubElement: " <<  newTag << endl;
        ofConsole.unlockCerr();
    } else {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
    }

    /* return result value */
    return l_error;
}


// ********************************


OFCondition DcmItem::read(DcmInputStream & inStream,
                          const E_TransferSyntax xfer,
                          const E_GrpLenEncoding glenc,
                          const Uint32 maxReadLength)
{
    /* check if this is an illegal call; if so set the error flag and do nothing, else go ahead */
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* figure out if the stream reported an error */
        errorFlag = inStream.status();
        /* if the stream reported an error or if it is the end of the */
        /* stream, set the error flag correspondingly; else go ahead */
        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        else if (errorFlag.good() && fTransferState != ERW_ready)
        {
            /* if the transfer state of this item is ERW_init, get its start */
            /* position in the stream and set the transfer state to ERW_inWork */
            if (fTransferState == ERW_init)
            {
                fStartPosition = inStream.tell();  // start position of this item
                fTransferState = ERW_inWork;
            }
            DcmTag newTag;
            /* start a loop in order to read all elements (attributes) which are contained in the inStream */
            while (inStream.good() && (fTransferredBytes < Length || !lastElementComplete))
            {
                /* initialize variables */
                Uint32 newValueLength = 0;
                Uint32 bytes_tagAndLen = 0;
                /* if the reading of the last element was complete, go ahead and read the next element */
                if (lastElementComplete)
                {
                    /* read this element's tag and length information (and */
                    /* possibly also VR information) from the inStream */
                    errorFlag = readTagAndLength(inStream, xfer, newTag, newValueLength, bytes_tagAndLen);
                    /* increase counter correpsondingly */
                    fTransferredBytes += bytes_tagAndLen;

                    /* if there was an error while we were reading from the stream, terminate the while-loop */
                    /* (note that if the last element had been read from the instream in the last iteration, */
                    /* another iteration will be started, and of course then readTagAndLength(...) above will */
                    /* return that it encountered the end of the stream. It is only then (and here) when the */
                    /* while loop will be terminated.) */
                    if (errorFlag.bad())
                        break;
                    /* If we get to this point, we just started reading the first part */
                    /* of an element; hence, lastElementComplete is not longer true */
                    lastElementComplete = OFFalse;
                    /* read the actual data value which belongs to this element */
                    /* (attribute) and insert this information into the elementList */
                    errorFlag = readSubElement(inStream, newTag, newValueLength, xfer, glenc, maxReadLength);
                    /* if reading was successful, we read the entire data value information */
                    /* for this element; hence lastElementComplete is true again */
                    if (errorFlag.good())
                        lastElementComplete = OFTrue;
                } else {
                    /* if lastElementComplete is false, we have only read the current element's */
                    /* tag and length (and possibly VR) information as well as maybe some data */
                    /* data value information. We need to continue reading the data value */
                    /* information for this particular element. */
                    errorFlag = elementList->get()->read(inStream, xfer, glenc, maxReadLength);
                    /* if reading was successful, we read the entire information */
                    /* for this element; hence lastElementComplete is true */
                    if (errorFlag.good())
                        lastElementComplete = OFTrue;
                }
                /* remember how many bytes were read */
                fTransferredBytes = inStream.tell() - fStartPosition;
                if (errorFlag.good())
                {
                    // If we completed one element, update the private tag cache.
                    if (lastElementComplete)
                        privateCreatorCache.updateCache(elementList->get());
                } else
                    break; // if some error was encountered terminate the while-loop
            } //while

            /* determine an appropriate result value; note that if the above called read function */
            /* encountered the end of the stream before all information for this element could be */
            /* read from the stream, the errorFlag has already been set to EC_StreamNotifyClient. */
            if ((fTransferredBytes < Length || !lastElementComplete) && errorFlag.good())
                errorFlag = EC_StreamNotifyClient;
            if (errorFlag.good() && inStream.eos())
                errorFlag = EC_EndOfStream;
        } // else errorFlag
        /* modify the result value: two kinds of special error codes do not count as an error */
        if (errorFlag == EC_ItemEnd || errorFlag == EC_EndOfStream)
            errorFlag = EC_Normal;
        /* if at this point the error flag indicates success, the item has */
        /* been read completely; hence, set the transfer state to ERW_ready. */
        /* Note that all information for this element could be read from the */
        /* stream, the errorFlag is still set to EC_StreamNotifyClient. */
        if (errorFlag.good())
            fTransferState = ERW_ready;
    }
    /* return result value */
    return errorFlag;
} // DcmItem::read()


// ********************************


OFCondition DcmItem::write(DcmOutputStream &outStream,
                           const E_TransferSyntax oxfer,
                           const E_EncodingType enctype)
{
  if (fTransferState == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    errorFlag = outStream.status();
    if (errorFlag.good() && fTransferState != ERW_ready)
    {
      if (fTransferState == ERW_init)
      {
        if (outStream.avail() >= 8)
        {
          if (enctype == EET_ExplicitLength)
            Length = getLength(oxfer, enctype);
          else
            Length = DCM_UndefinedLength;
          errorFlag = writeTag(outStream, Tag, oxfer);
          Uint32 valueLength = Length;
          DcmXfer outXfer(oxfer);
          const E_ByteOrder oByteOrder = outXfer.getByteOrder();
          if (oByteOrder == EBO_unknown) return EC_IllegalCall;
          swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
          outStream.write(&valueLength, 4); // 4 bytes length
          elementList->seek(ELP_first);
          fTransferState = ERW_inWork;
        } else
          errorFlag = EC_StreamNotifyClient;
      }
      if (fTransferState == ERW_inWork)
      {
        // elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          DcmObject *dO = NULL;
          do
          {
              dO = elementList->get();
              if (dO->transferState() != ERW_ready)
                errorFlag = dO->write(outStream, oxfer, enctype);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }
        if (errorFlag.good())
        {
          fTransferState = ERW_ready;
          if (Length == DCM_UndefinedLength)
          {
            if (outStream.avail() >= 8)
            {
                // write Item delimitation
                DcmTag delim(DCM_ItemDelimitationItem);
                errorFlag = writeTag(outStream, delim, oxfer);
                Uint32 delimLen = 0L;
                outStream.write(&delimLen, 4); // 4 bytes length
            }
            else
            {
                // Every subelement of the item is written but it
                // is not possible to write the delimination item into the buffer.
                errorFlag = EC_StreamNotifyClient;
                fTransferState = ERW_inWork;
            }
          }
        }
      }
    }
  }
  return errorFlag;
}

// ********************************

OFCondition DcmItem::writeSignatureFormat(DcmOutputStream &outStream,
                                          const E_TransferSyntax oxfer,
                                          const E_EncodingType enctype)
{
  if (fTransferState == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    errorFlag = outStream.status();
    if (errorFlag.good() && fTransferState != ERW_ready)
    {
      if (fTransferState == ERW_init)
      {
        if (outStream.avail() >= 4)
        {
          if (enctype == EET_ExplicitLength)
            Length = getLength(oxfer, enctype);
          else
            Length = DCM_UndefinedLength;
          errorFlag = writeTag(outStream, Tag, oxfer);
          /* we don't write the item length */
          elementList->seek(ELP_first);
          fTransferState = ERW_inWork;
        } else
          errorFlag = EC_StreamNotifyClient;
      }
      if (fTransferState == ERW_inWork)
      {
        // elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          DcmObject *dO = NULL;
          do
          {
            dO = elementList->get();
            if (dO->transferState() != ERW_ready)
              errorFlag = dO->writeSignatureFormat(outStream, oxfer, enctype);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }
        if (errorFlag.good())
        {
          fTransferState = ERW_ready;
          /* we don't write an item delimitation even if the item has undefined length */
        }
      }
    }
  }
  return errorFlag;
}


// ********************************


void DcmItem::transferInit()
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastElementComplete = OFTrue;
    privateCreatorCache.clear();
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            elementList->get()->transferInit();
        } while (elementList->seek(ELP_next));
    }
}


// ********************************


void DcmItem::transferEnd()
{
    DcmObject::transferEnd();
    privateCreatorCache.clear();
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            elementList->get()->transferEnd();
        } while (elementList->seek(ELP_next));
    }
}


// ********************************


unsigned long DcmItem::card() const
{
    return elementList->card();
}


// ********************************


OFCondition DcmItem::insert(DcmElement *elem,
                            OFBool replaceOld,
                            OFBool checkInsertOrder)
{
    /* initialize error flag with ok */
    errorFlag = EC_Normal;
    /* do something only if the pointer which was passed does not equal NULL */
    if (elem != (DcmElement*)NULL)
    {
        DcmElement *dE;
        E_ListPos seekmode = ELP_last;
        /* iterate through elementList (from the last element to the first) */
        do {
            /* get current element from elementList */
            dE = (DcmElement*)(elementList->seek(seekmode));
            /* if there is no element, i.e. elementList is empty */
            if (dE == (DcmElement*)NULL)
            {
                /* insert new element at the beginning of elementList */
                elementList->insert(elem, ELP_first);
                if (checkInsertOrder)
                {
                  // check if we have inserted at the end of the list
                  if (elem != (DcmElement*)(elementList->seek(ELP_last)))
                  {
                    // produce diagnostics
                    ofConsole.lockCerr()
                       << "Warning: dataset not in ascending tag order, at element "
                       << elem->getTag() << endl;
                    ofConsole.unlockCerr();
                  }
                }
                /* dump some information if required */
                debug(3, ("DcmItem::Insert() element (0x%4.4x,0x%4.4x) / VR=\"%s\" at beginning inserted",
                        elem->getGTag(), elem->getETag(), DcmVR(elem->getVR()).getVRName()));
                /* terminate do-while-loop */
                break;
            }
            /* else if the new element's tag is greater than the current element's tag */
            /* (i.e. we have found the position where the new element shall be inserted) */
            else if (elem->getTag() > dE->getTag())
            {
                /* insert the new element after the current element */
                elementList->insert(elem, ELP_next);
                if (checkInsertOrder)
                {
                  // check if we have inserted at the end of the list
                  if (elem != (DcmElement*)(elementList->seek(ELP_last)))
                  {
                    // produce diagnostics
                    ofConsole.lockCerr()
                       << "Warning: dataset not in ascending tag order, at element "
                       << elem->getTag() << endl;
                    ofConsole.unlockCerr();
                  }
                }
                /* dump some information if required */
                debug(3, ("DcmItem::Insert() element (0x%4.4x,0x%4.4x) / VR=\"%s\" inserted",
                        elem->getGTag(), elem->getETag(),
                        DcmVR(elem->getVR()).getVRName()));
                /* terminate do-while-loop */
                break;
            }
            /* else if the current element and the new element show the same tag */
            else if (elem->getTag() == dE->getTag())
            {
                /* if new and current element are not identical */
                if (elem != dE)
                {
                    /* if the current (old) element shall be replaced */
                    if (replaceOld)
                    {
                        /* remove current element from list */
                        DcmObject *remObj = elementList->remove();

                        /* now the following holds: remObj == dE and elementList */
                        /* points to the element after the former current element. */

                        /* dump some information if required */
                        debug(3, ("DcmItem::insert:element (0x%4.4x,0x%4.4x) VR=\"%s\" p=%p removed",
                                remObj->getGTag(), remObj->getETag(),
                                DcmVR(remObj->getVR()).getVRName(), remObj));

                        /* if the pointer to the removed object does not */
                        /* equal NULL (the usual case), delete this object */
                        /* and dump some information if required */
                        if (remObj != (DcmObject*)NULL)
                        {
                            delete remObj;
                            debug(3, ("DcmItem::insert:element p=%p deleted", remObj));
                        }
                        /* insert the new element before the current element */
                        elementList->insert(elem, ELP_prev);
                        /* dump some information if required */
                        debug(3, ("DcmItem::insert() element (0x%4.4x,0x%4.4x) VR=\"%s\" p=%p replaced older one",
                                elem->getGTag(), elem->getETag(),
                                DcmVR(elem->getVR()).getVRName(), elem));
                    }   // if (replaceOld)
                    /* or else, i.e. the current element shall not be replaced by the new element */
                    else {
                        /* set the error flag correspondingly; we do not */
                        /* allow two elements with the same tag in elementList */
                        errorFlag = EC_DoubledTag;
                    }   // if (!replaceOld)
                }   // if (elem != dE)
                /* if the new and the current element are identical, the caller tries to insert */
                /* one element twice. Most probably an application error. */
                else {
                    errorFlag = EC_DoubledTag;
                }
                /* terminate do-while-loop */
                break;
            }
            /* set the seek mode to "get the previous element" */
            seekmode = ELP_prev;
        } while (dE);
    }
    /* if the pointer which was passed equals NULL, this is an illegal call */
    else
        errorFlag = EC_IllegalCall;
    /* return result value */
    return errorFlag;
}


// ********************************


DcmElement *DcmItem::getElement(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = (DcmElement*)(elementList->seek_to(num));
    // liest Element aus Liste
    if (elem == (DcmElement*)NULL)
        errorFlag = EC_IllegalCall;
    return elem;
}


// ********************************


DcmObject *DcmItem::nextInContainer(const DcmObject *obj)
{
    if (!obj)
        return elementList->get(ELP_first);
    else
    {
        if (elementList->get() != obj)
        {
            for (DcmObject * search_obj = elementList->seek(ELP_first);
                 search_obj && search_obj != obj;
                 search_obj = elementList->seek(ELP_next)
                ) {
                /* do nothing, just keep iterating */
            }
        }
        return elementList->seek(ELP_next);
    }
}


// ********************************


OFCondition DcmItem::nextObject(DcmStack &stack,
                                const OFBool intoSub)
{
    OFCondition l_error = EC_Normal;
    DcmObject * container = NULL;
    DcmObject * obj = NULL;
    DcmObject * result = NULL;
    OFBool examSub = intoSub;

    if (stack.empty())
    {
        stack.push(this);
        examSub = OFTrue;
    }

    obj = stack.top();
    if (obj->isLeaf() || !intoSub)
    {
        stack.pop();
        if (stack.card() > 0)
        {
            container = stack.top();
            result = container->nextInContainer(obj);
        }
    } else if (examSub)
        result = obj->nextInContainer(NULL);

    if (result)
        stack.push(result);
    else if (intoSub)
        l_error = nextUp(stack);
    else
        l_error = EC_SequEnd;

    return l_error;
}


// ********************************


DcmElement *DcmItem::remove(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = (DcmElement*)(elementList->seek_to(num));
    // read element from list
    if (elem != (DcmElement*)NULL)
        elementList->remove();          // removes element from list but does not delete it
    else
        errorFlag = EC_IllegalCall;
    return elem;
}


// ********************************


DcmElement *DcmItem::remove(DcmObject *elem)
{
    errorFlag = EC_IllegalCall;
    if (!elementList->empty() && elem != (DcmObject*)NULL)
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (dO == elem)
            {
                elementList->remove();     // removes element from list but does not delete it
                errorFlag = EC_Normal;
                break;
            }
        } while (elementList->seek(ELP_next));
    }
    if (errorFlag == EC_IllegalCall)
        return (DcmElement*)NULL;
    else
        return (DcmElement*)elem;
}


// ********************************


DcmElement *DcmItem::remove(const DcmTagKey &tag)
{
    errorFlag = EC_TagNotFound;
    DcmObject *dO = (DcmObject*)NULL;
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (dO->getTag() == tag)
            {
                elementList->remove();     // removes element from list but does not delete it
                errorFlag = EC_Normal;
                break;
            }
        } while (elementList->seek(ELP_next));
    }

    if (errorFlag == EC_TagNotFound)
        return (DcmElement*)NULL;
    else
        return (DcmElement*)dO;
}


// ********************************


OFCondition DcmItem::clear()
{
    errorFlag = EC_Normal;
    DcmObject *dO;
    elementList->seek(ELP_first);
    while (!elementList->empty())
    {
        dO = elementList->remove();
        if (dO != (DcmObject*)NULL)
            delete dO;                          // also delete sub elements
    }
    Length = 0;

    return errorFlag;
}


// ********************************


OFCondition DcmItem::verify(const OFBool autocorrect)
{
    debug(3, ("DcmItem::verify() Tag=(0x%4.4x,0x%4.4x) \"%s\" \"%s\"",
            getGTag(), getETag(), DcmVR(getVR()).getVRName(), Tag.getTagName()));

    errorFlag = EC_Normal;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (dO->verify(autocorrect).bad())
                errorFlag = EC_CorruptedData;
        } while (elementList->seek(ELP_next));
    }
    if (autocorrect == OFTrue)
        Length = getLength();
    return errorFlag;
}


// ********************************

    // Vorbedingung: elementList ist nicht leer!
    // Ergebnis:     - return EC_Normal;
    //                 gefundener Element-Zeiger auf resultStack
    //               - return EC_TagNotFound;
    //                 resultStack unveraendert
    // Weitersuchen: schreibe Zeiger des Sub-Elementes auf resultStack und
    //               starte dann Sub-Suche

OFCondition DcmItem::searchSubFromHere(const DcmTagKey &tag,
                                       DcmStack &resultStack,
                                       OFBool searchIntoSub)
{
    DcmObject *dO;
    OFCondition l_error = EC_TagNotFound;
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (searchIntoSub)
            {
                resultStack.push(dO);
                if (dO->getTag() == tag)
                    l_error = EC_Normal;
                else
                    l_error = dO->search(tag, resultStack, ESM_fromStackTop, OFTrue);
                if (l_error.bad())
                    resultStack.pop();
            } else {
                if (dO->getTag() == tag)
                {
                    resultStack.push(dO);
                    l_error = EC_Normal;
                }
            }
        } while (l_error.bad() && elementList->seek(ELP_next));
        Cdebug(4, l_error==EC_Normal && dO->getTag()==tag,
               ("DcmItem::searchSubFromHere() Search-Tag=(%4.4x,%4.4x)"
                " found!", tag.getGroup(), tag.getElement()));
    }
    return l_error;
}


// ********************************


OFCondition DcmItem::search(const DcmTagKey &tag,
                            DcmStack &resultStack,
                            E_SearchMode mode,
                            OFBool searchIntoSub)
{
    DcmObject *dO = (DcmObject*)NULL;
    OFCondition l_error = EC_TagNotFound;
    if (mode == ESM_afterStackTop && resultStack.top() == this)
    {
        l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
    }
    else if (!elementList->empty())
    {
        if (mode == ESM_fromHere || resultStack.empty())
        {
            resultStack.clear();
            l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
        }
        else if (mode == ESM_fromStackTop)
        {
            dO = resultStack.top();
            if (dO == this)
                l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
            else
            {   // gehe direkt zu Sub-Baum und suche dort weiter
                l_error = dO->search(tag, resultStack, mode, searchIntoSub);
// The next two lines destroy the stack->so delete them
//                if (l_error.bad()) // raeumt nur die oberste Stackebene
//                    resultStack.pop();      // ab; der Rest ist unveraendert
            }
        }
        else if (mode == ESM_afterStackTop && searchIntoSub)
        {
            // resultStack enthaelt Zielinformationen:
            // - stelle Zustand der letzen Suche in den einzelnen Suchroutinen
            //   wieder her
            // - finde Position von dO in Baum-Struktur
            //   1. suche eigenen Stack-Eintrag
            //      - bei Fehlschlag Suche beenden
            //   2. nehme naechsthoeheren Eintrag dnO
            //   3. stelle eigene Liste auf Position von dnO
            //   4. starte Suche ab dnO

            unsigned long i = resultStack.card();
            while (i > 0 && (dO = resultStack.elem(i-1)) != this)
            {
                i--;
            }
            if (dO != this && resultStack.card() > 0)
            {                            // oberste Ebene steht nie in resultStack
                i = resultStack.card()+1;// zeige jetzt auf hoechste Ebene+1
                dO = this;               // Treffer der hoechsten Ebene!
            }
            if (dO == this)
            {
                if (i == 1)                   // habe resultStack.top() gefunden
                    l_error = EC_TagNotFound; // markiere als kein Treffer, s.o.
                else                          //   siehe oben
                {
                    E_SearchMode submode = mode;
                    OFBool searchNode = OFTrue;
                    DcmObject *dnO;
                    dnO = resultStack.elem(i - 2); // Knoten der naechsten Ebene
                    elementList->seek(ELP_first);
                    do {
                        dO = elementList->get();
                        searchNode = searchNode ? (dO != dnO) : OFFalse;
                        if (!searchNode)
                        {                             // suche jetzt weiter
                            if (submode == ESM_fromStackTop)
                                resultStack.push(dO); // Stack aktualisieren
                            if (submode == ESM_fromStackTop && dO->getTag() == tag)
                                l_error = EC_Normal;
                            else
                                l_error = dO->search(tag, resultStack, submode, OFTrue);
                            if (l_error.bad())
                                resultStack.pop();
                            else
                                break;
                            submode = ESM_fromStackTop; // ab hier normale Suche
                        }
                    } while (elementList->seek(ELP_next));
                }
            } else
                l_error = EC_IllegalCall;
        } // (mode == ESM_afterStackTop
        else
            l_error = EC_IllegalCall;
    }
    return l_error;
}


// ********************************


OFCondition DcmItem::searchErrors(DcmStack &resultStack)
{
    OFCondition l_error = errorFlag;
    DcmObject *dO = (DcmObject*)NULL;
    if (errorFlag.bad())
        resultStack.push(this);
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            OFCondition err = EC_Normal;
            dO = elementList->get();
            if ((err = dO->searchErrors(resultStack)).bad())
                l_error = err;
        } while (elementList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFCondition DcmItem::loadAllDataIntoMemory()
{
    OFCondition l_error = EC_Normal;
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            OFCondition err = EC_Normal;
            DcmObject *dO = elementList->get();
            if ((err = dO->loadAllDataIntoMemory()).bad())
                l_error = err;
        } while (elementList->seek(ELP_next));
    }
    return l_error;
}


// ********************************

//
// Support functions

DcmElement *newDicomElement(const DcmTag &tag,
                             const Uint32 length)
{
    DcmElement *newElement = NULL;
    newDicomElement(newElement, tag, length);
    return newElement;
}


// ********************************

OFCondition newDicomElement(DcmElement *&newElement,
                            const DcmTag &tag,
                            const Uint32 length)
{
    /* initialize variables */
    OFCondition l_error = EC_Normal;
    newElement = NULL;

    /* depending on the VR of the tag which was passed, create the new object */
    switch (tag.getEVR())
    {
        // byte strings:
        case EVR_AE :
            newElement = new DcmApplicationEntity(tag, length);
            break;
        case EVR_AS :
            newElement = new DcmAgeString(tag, length);
            break;
        case EVR_CS :
            newElement = new DcmCodeString(tag, length);
            break;
        case EVR_DA :
            newElement = new DcmDate(tag, length);
            break;
        case EVR_DS :
            newElement = new DcmDecimalString(tag, length);
            break;
        case EVR_DT :
            newElement = new DcmDateTime(tag, length);
            break;
        case EVR_IS :
            newElement = new DcmIntegerString(tag, length);
            break;
        case EVR_TM :
            newElement = new DcmTime(tag, length);
            break;
        case EVR_UI :
            newElement = new DcmUniqueIdentifier(tag, length);
            break;

        // character strings:
        case EVR_LO :
            newElement = new DcmLongString(tag, length);
            break;
        case EVR_LT :
            newElement = new DcmLongText(tag, length);
            break;
        case EVR_PN :
            newElement = new DcmPersonName(tag, length);
            break;
        case EVR_SH :
            newElement = new DcmShortString(tag, length);
            break;
        case EVR_ST :
            newElement = new DcmShortText(tag, length);
            break;
        case EVR_UT:
            newElement = new DcmUnlimitedText(tag, length);
            break;

        // dependent on byte order:
        case EVR_AT :
            newElement = new DcmAttributeTag(tag, length);
            break;
        case EVR_SS :
            newElement = new DcmSignedShort(tag, length);
            break;
        case EVR_xs : // according to Dicom-Standard V3.0
        case EVR_US :
            newElement = new DcmUnsignedShort(tag, length);
            break;
        case EVR_SL :
            newElement = new DcmSignedLong(tag, length);
            break;
        case EVR_up : // for (0004,eeee) according to Dicom-Standard V3.0
        case EVR_UL :
        {
            // generate Tag with VR from dictionary!
            DcmTag ulupTag(tag.getXTag());
            if (ulupTag.getEVR() == EVR_up)
                newElement = new DcmUnsignedLongOffset(ulupTag, length);
            else
                newElement = new DcmUnsignedLong(tag, length);
        }
        break;
        case EVR_FL:
            newElement = new DcmFloatingPointSingle(tag, length);
            break;
        case EVR_FD :
            newElement = new DcmFloatingPointDouble(tag, length);
            break;
        case EVR_OF:
            newElement = new DcmOtherFloat(tag, length);
            break;

        // sequences and items
        case EVR_SQ :
            newElement = new DcmSequenceOfItems(tag, length);
            break;
        case EVR_na :
            if (tag.getXTag() == DCM_Item)
                l_error = EC_InvalidTag;
            else if (tag.getXTag() == DCM_SequenceDelimitationItem)
                l_error = EC_SequEnd;
            else if (tag.getXTag() == DCM_ItemDelimitationItem)
                l_error = EC_ItemEnd;
            else
                l_error = EC_InvalidTag;
            break;

            // unclear 8- or 16-bit:

        case EVR_ox :
            if (tag == DCM_PixelData)
                newElement = new DcmPixelData(tag, length);
            else if (((tag.getGTag() & 0xffe1) == 0x6000)&&(tag.getETag() == 0x3000)) // DCM_OverlayData
                newElement = new DcmOverlayData(tag, length);
            else
                /* we don't know this element's real transfer syntax, so we just
                 * use the defaults of class DcmOtherByteOtherWord and let the
                 * application handle it.
                 */
                newElement = new DcmOtherByteOtherWord(tag, length);
            break;

        case EVR_OB :
        case EVR_OW :
            if (tag == DCM_PixelData)
                newElement = new DcmPixelData(tag, length);
            else if (((tag.getGTag() & 0xffe1) == 0x6000)&&(tag.getETag() == 0x3000)) // DCM_OverlayData
                newElement = new DcmOverlayData(tag, length);
            else
                if (length == DCM_UndefinedLength) {
                    // The attribute is OB or OW but is encoded with undefined
                    // length.  Assume it is really a sequence so that we can
                    // catch the sequence delimitation item.
                    newElement = new DcmSequenceOfItems(tag, length);
                } else {
                    newElement = new DcmOtherByteOtherWord(tag, length);
                }
            break;

        // read unknown types as byte string:
        case EVR_UNKNOWN :
        case EVR_UNKNOWN2B :
        case EVR_UN :
        default :
            if (length == DCM_UndefinedLength)
            {
                // The attribute is unknown but is encoded with undefined
                // length.  Assume it is really a sequence so that we can
                // catch the sequence delimitation item.
                DcmVR sqVR(EVR_SQ); // we handle this element as SQ, not UN
                DcmTag newTag(tag.getXTag(), sqVR);
                newElement = new DcmSequenceOfItems(newTag, length);
            } else {
                newElement = new DcmOtherByteOtherWord(tag, length);
            }
            break;
    }

    /* return result value */
    return l_error;
}


OFCondition nextUp(DcmStack &stack)
{
    DcmObject *oldContainer = stack.pop();
    if (oldContainer->isLeaf())
        return EC_IllegalCall;
    else if (!stack.empty())
    {
        DcmObject *container = stack.top();
        DcmObject *result = container->nextInContainer(oldContainer);
        if (result)
        {
            stack.push(result);
            return EC_Normal;
        }
        else
            return nextUp(stack);
    }
    return EC_TagNotFound;
}


/*
** Simple tests for existance
*/

OFBool DcmItem::tagExists(const DcmTagKey &key,
                          OFBool searchIntoSub)
{
    DcmStack stack;

    OFCondition ec = search(key, stack, ESM_fromHere, searchIntoSub);
    return ec.good();
}


OFBool DcmItem::tagExistsWithValue(const DcmTagKey &key,
                                   OFBool searchIntoSub)
{
    DcmElement *elem = NULL;
    Uint32 len = 0;
    DcmStack stack;

    OFCondition ec = search(key, stack, ESM_fromHere, searchIntoSub);
    elem = (DcmElement*) stack.top();
    if (ec.good() && elem != NULL)
        len = elem->getLength();

    return ec.good() && (len > 0);
}


// ********************************

/* --- findAndGet functions: find an element and get the value --- */

OFCondition DcmItem::findAndGetString(const DcmTagKey& tagKey,
                                      const char *&value,
                                      const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getString((char *&)value);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetOFString(const DcmTagKey& tagKey,
                                        OFString &value,
                                        const unsigned long pos,
                                        const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getOFString(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value.clear();
    return status;
}


OFCondition DcmItem::findAndGetOFStringArray(const DcmTagKey& tagKey,
                                             OFString &value,
                                             const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getOFStringArray(value);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value.clear();
    return status;
}


OFCondition DcmItem::findAndGetUint8(const DcmTagKey& tagKey,
                                     Uint8 &value,
                                     const unsigned long pos,
                                     const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        status = EC_IllegalCall;
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        /* get value */
        if (elem != NULL)
            status = elem->getUint8(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetUint8Array(const DcmTagKey& tagKey,
                                          Uint8 *&value,
                                          const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getUint8Array(value);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetUint16(const DcmTagKey& tagKey,
                                      Uint16 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getUint16(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetUint16Array(const DcmTagKey& tagKey,
                                           Uint16 *&value,
                                           const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getUint16Array(value);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetSint16(const DcmTagKey& tagKey,
                                      Sint16 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getSint16(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetSint16Array(const DcmTagKey& tagKey,
                                           Sint16 *&value,
                                           const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getSint16Array(value);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetUint32(const DcmTagKey& tagKey,
                                      Uint32 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getUint32(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetSint32(const DcmTagKey& tagKey,
                                      Sint32 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getSint32(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetLongInt(const DcmTagKey& tagKey,
                                       long int &value,
                                       const unsigned long pos,
                                       const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
        {
            /* distinguish supported VRs */
            switch (elem->ident())
            {
                case EVR_UL:
                case EVR_up:
                    Uint32 ul;
                    status = elem->getUint32(ul, pos);
                    value = (long int)ul;
                    break;
                case EVR_SL:
                case EVR_IS:
                    Sint32 sl;
                    status = elem->getSint32(sl, pos);
                    value = (long int)sl;
                    break;
                case EVR_US:
                case EVR_xs:
                    Uint16 us;
                    status = elem->getUint16(us, pos);
                    value = (long int)us;
                    break;
                case EVR_SS:
                    Sint16 ss;
                    status = elem->getSint16(ss, pos);
                    value = (long int)ss;
                    break;
                default:
                    status = EC_IllegalCall;
                    break;
            }
        } else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetFloat32(const DcmTagKey& tagKey,
                                       Float32 &value,
                                       const unsigned long pos,
                                       const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getFloat32(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetFloat32Array(const DcmTagKey& tagKey,
                                            Float32 *&value,
                                            const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getFloat32Array(value);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetFloat64(const DcmTagKey& tagKey,
                                       Float64 &value,
                                       const unsigned long pos,
                                       const OFBool searchIntoSub)
{
    DcmStack stack;
    /* find element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *elem = (DcmElement *)stack.top();
        /* get value */
        if (elem != NULL)
            status = elem->getFloat64(value, pos);
        else
            status = EC_IllegalCall;
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetSequenceItem(const DcmTagKey &seqTagKey,
                                            DcmItem *&item,
                                            const signed long itemNum)
{
    DcmStack stack;
    /* find sequence */
    OFCondition status = search(seqTagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (status.good())
    {
        DcmSequenceOfItems *seq = (DcmSequenceOfItems *)stack.top();
        /* get item */
        if (seq != NULL)
        {
            const unsigned long count = seq->card();
            /* empty sequence? */
            if (count > 0)
            {
                /* get last item */
                if (itemNum == -1)
                    item = seq->getItem(count - 1);
                /* get specified item */
                else if ((itemNum >= 0) && ((unsigned long)itemNum < count))
                    item = seq->getItem((unsigned long)itemNum);
                /* invalid item number */
                else
                    status = EC_IllegalParameter;
            } else
                status = EC_IllegalParameter;
        } else
            status = EC_IllegalCall;
    }
    /* reset item value */
    if (status.bad())
        item = NULL;
    else if (item == NULL)
        status = EC_IllegalCall;
    return status;
}


// ********************************

/* --- findOrCreate functions: find an element or create a new one --- */

OFCondition DcmItem::findOrCreateSequenceItem(const DcmTag& seqTag,
                                              DcmItem *&item,
                                              const signed long itemNum)
{
    DcmStack stack;
    /* find sequence */
    OFCondition status = search(seqTag, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    DcmSequenceOfItems *seq = NULL;
    /* sequence found? */
    if (status.good())
        seq = (DcmSequenceOfItems *)stack.top();
    else
    {
        /* create new sequence element */
        seq = new DcmSequenceOfItems(seqTag);
        if (seq != NULL)
        {
            /* insert into item/dataset */
            status = insert(seq, OFTrue /*replaceOld*/);
            if (status.bad())
                delete seq;
        } else
            status = EC_MemoryExhausted;
    }
    if (status.good())
    {
        if (seq != NULL)
        {
            const unsigned long count = seq->card();
            /* existing item? */
            if ((count > 0) && (itemNum >= -1) && (itemNum < (signed long)count))
            {
                if (itemNum == -1)
                {
                    /* get last item */
                    item = seq->getItem(count - 1);
                } else {
                    /* get specified item */
                    item = seq->getItem((unsigned long)itemNum);
                }
            /* create new item(s) */
            } else {
                unsigned long i = 0;
                /* create empty trailing items if required */
                const unsigned long itemCount = (itemNum > (signed long)count) ? (itemNum - count + 1) : 1;
                while ((i < itemCount) && (status.good()))
                {
                    item = new DcmItem();
                    if (item != NULL)
                    {
                        /* append new item to end of sequence */
                        status = seq->append(item);
                        if (status.bad())
                            delete item;
                    } else
                        status = EC_MemoryExhausted;
                    i++;
                }
            }
        } else
            status = EC_IllegalCall;
    }
    /* reset item value */
    if (status.bad())
        item = NULL;
    else if (item == NULL)
        status = EC_IllegalCall;
    return status;
}


// ********************************

/* --- putAndInsert functions: put value and insert new element --- */

OFCondition DcmItem::putAndInsertString(const DcmTag& tag,
                                        const char *value,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch (tag.getEVR())
    {
        case EVR_AE:
            elem = new DcmApplicationEntity(tag);
            break;
        case EVR_AS:
            elem = new DcmAgeString(tag);
            break;
        case EVR_CS:
            elem = new DcmCodeString(tag);
            break;
        case EVR_DA:
            elem = new DcmDate(tag);
            break;
        case EVR_DS:
            elem = new DcmDecimalString(tag);
            break;
        case EVR_DT:
            elem = new DcmDateTime(tag);
            break;
        case EVR_FL:
            elem = new DcmFloatingPointSingle(tag);
            break;
        case EVR_FD:
            elem = new DcmFloatingPointDouble(tag);
            break;
        case EVR_IS:
            elem = new DcmIntegerString(tag);
            break;
        case EVR_TM:
            elem = new DcmTime(tag);
            break;
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_LO:
            elem = new DcmLongString(tag);
            break;
        case EVR_LT:
            elem = new DcmLongText(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_SH:
            elem = new DcmShortString(tag);
            break;
        case EVR_ST:
            elem = new DcmShortText(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putString(value);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertOFStringArray(const DcmTag& tag,
                                               const OFString &value,
                                               const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch (tag.getEVR())
    {
        case EVR_AE:
            elem = new DcmApplicationEntity(tag);
            break;
        case EVR_AS:
            elem = new DcmAgeString(tag);
            break;
        case EVR_CS:
            elem = new DcmCodeString(tag);
            break;
        case EVR_DA:
            elem = new DcmDate(tag);
            break;
        case EVR_DS:
            elem = new DcmDecimalString(tag);
            break;
        case EVR_DT:
            elem = new DcmDateTime(tag);
            break;
        case EVR_IS:
            elem = new DcmIntegerString(tag);
            break;
        case EVR_TM:
            elem = new DcmTime(tag);
            break;
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_LO:
            elem = new DcmLongString(tag);
            break;
        case EVR_LT:
            elem = new DcmLongText(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_SH:
            elem = new DcmShortString(tag);
            break;
        case EVR_ST:
            elem = new DcmShortText(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        case EVR_OB:
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putOFStringArray(value);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertUint16(const DcmTag& tag,
                                        const Uint16 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_US)
    {
        DcmElement *elem = new DcmUnsignedShort(tag);
        if (elem != NULL)
        {
            /* put value */
            status = elem->putUint16(value, pos);
            /* insert into dataset/item */
            if (status.good())
                status = insert(elem, replaceOld);
            /* could not be inserted, therefore, delete it immediately */
            if (status.bad())
                delete elem;
        } else
            status = EC_MemoryExhausted;
    }
    return status;
}


OFCondition DcmItem::putAndInsertUint8Array(const DcmTag& tag,
                                            const Uint8 *value,
                                            const unsigned long count,
                                            const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    DcmElement *elem = NULL;
    switch (tag.getEVR())
    {
        case EVR_OB:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_ox:
            elem = new DcmPolymorphOBOW(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putUint8Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertUint16Array(const DcmTag& tag,
                                             const Uint16 *value,
                                             const unsigned long count,
                                             const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch (tag.getEVR())
    {
        case EVR_AT:
            elem = new DcmAttributeTag(tag);
            break;
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_US:
            elem = new DcmUnsignedShort(tag);
            break;
        case EVR_ox:
            elem = new DcmPolymorphOBOW(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putUint16Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertSint16(const DcmTag& tag,
                                        const Sint16 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_SS)
    {
        DcmElement *elem = new DcmSignedShort(tag);
        if (elem != NULL)
        {
            /* put value */
            status = elem->putSint16(value, pos);
            /* insert into dataset/item */
            if (status.good())
                status = insert(elem, replaceOld);
            /* could not be inserted, therefore, delete it immediately */
            if (status.bad())
                delete elem;
        } else
            status = EC_MemoryExhausted;
    }
    return status;
}


OFCondition DcmItem::putAndInsertSint16Array(const DcmTag& tag,
                                             const Sint16 *value,
                                             const unsigned long count,
                                             const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_SS)
    {
        DcmElement *elem = new DcmSignedShort(tag);
        if (elem != NULL)
        {
            /* put value */
            status = elem->putSint16Array(value, count);
            /* insert into dataset/item */
            if (status.good())
                status = insert(elem, replaceOld);
            /* could not be inserted, therefore, delete it immediately */
            if (status.bad())
                delete elem;
        } else
            status = EC_MemoryExhausted;
    }
    return status;
}


OFCondition DcmItem::putAndInsertUint32(const DcmTag& tag,
                                        const Uint32 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_UL)
    {
        DcmElement *elem = new DcmUnsignedLong(tag);
        if (elem != NULL)
        {
            /* put value */
            status = elem->putUint32(value, pos);
            /* insert into dataset/item */
            if (status.good())
                status = insert(elem, replaceOld);
            /* could not be inserted, therefore, delete it immediately */
            if (status.bad())
                delete elem;
        } else
            status = EC_MemoryExhausted;
    }
    return status;
}


OFCondition DcmItem::putAndInsertSint32(const DcmTag& tag,
                                        const Sint32 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_SL)
    {
        DcmElement *elem = new DcmSignedLong(tag);
        if (elem != NULL)
        {
            /* put value */
            status = elem->putSint32(value, pos);
            /* insert into dataset/item */
            if (status.good())
                status = insert(elem, replaceOld);
            /* could not be inserted, therefore, delete it immediately */
            if (status.bad())
                delete elem;
        } else
            status = EC_MemoryExhausted;
    }
    return status;
}


OFCondition DcmItem::putAndInsertFloat32(const DcmTag& tag,
                                         const Float32 value,
                                         const unsigned long pos,
                                         const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    DcmElement *elem = NULL;
    switch (tag.getEVR())
    {
        case EVR_FL:
            elem = new DcmFloatingPointSingle(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putFloat32(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertFloat64(const DcmTag& tag,
                                         const Float64 value,
                                         const unsigned long pos,
                                         const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_FD)
    {
        DcmElement *elem = new DcmFloatingPointDouble(tag);
        if (elem != NULL)
        {
            /* put value */
            status = elem->putFloat64(value, pos);
            /* insert into dataset/item */
            if (status.good())
                status = insert(elem, replaceOld);
            /* could not be inserted, therefore, delete it immediately */
            if (status.bad())
                delete elem;
        } else
            status = EC_MemoryExhausted;
    }
    return status;
}


OFCondition DcmItem::insertEmptyElement(const DcmTag& tag,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch (tag.getEVR())
    {
        case EVR_AE:
            elem = new DcmApplicationEntity(tag);
            break;
        case EVR_AS:
            elem = new DcmAgeString(tag);
            break;
        case EVR_AT:
            elem = new DcmAttributeTag(tag);
            break;
        case EVR_CS:
            elem = new DcmCodeString(tag);
            break;
        case EVR_DA:
            elem = new DcmDate(tag);
            break;
        case EVR_DS:
            elem = new DcmDecimalString(tag);
            break;
        case EVR_DT:
            elem = new DcmDateTime(tag);
            break;
        case EVR_FL:
            elem = new DcmFloatingPointSingle(tag);
            break;
        case EVR_FD:
            elem = new DcmFloatingPointDouble(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        case EVR_IS:
            elem = new DcmIntegerString(tag);
            break;
        case EVR_OB:
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_TM:
            elem = new DcmTime(tag);
            break;
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_LO:
            elem = new DcmLongString(tag);
            break;
        case EVR_LT:
            elem = new DcmLongText(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_SH:
            elem = new DcmShortString(tag);
            break;
        case EVR_SQ :
            elem = new DcmSequenceOfItems(tag);
            break;
        case EVR_ST:
            elem = new DcmShortText(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* insert new element into dataset/item */
        status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFBool DcmItem::containsUnknownVR() const
{
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            if (elementList->get()->containsUnknownVR())
                return OFTrue;
        } while (elementList->seek(ELP_next));
    }
    return OFFalse;
}
