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
 *  Purpose: Implementation of class DcmElement
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "ofstdinc.h"

#include "ofstd.h"
#include "dcdefine.h"    /* for memzero() */
#include "dcelem.h"
#include "dcobject.h"
#include "dcswap.h"
#include "dcdebug.h"
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcostrma.h"    /* for class DcmOutputStream */


//
// CLASS DcmElement
//


DcmElement::DcmElement(const DcmTag &tag,
                       const Uint32 len)
  : DcmObject(tag, len),
    fByteOrder(gLocalByteOrder),
    fLoadValue(NULL),
    fValue(NULL)
{
}


DcmElement::DcmElement(const DcmElement &elem)
  : DcmObject(elem),
    fByteOrder(elem.fByteOrder),
    fLoadValue(NULL),
    fValue(NULL)
{
    if (elem.fValue)
    {
        DcmVR vr(elem.getVR());
        const unsigned short pad = (vr.isaString()) ? 1 : 0;

        // The next lines are a special version of newValueField().
        // newValueField() cannot be used because it is virtual and it does
        // not allocate enough bytes for strings. The number of pad bytes
        // is added to the Length for this purpose.
        if (Length & 1)
        {
            fValue = new Uint8[Length + 1 + pad]; // protocol error: odd value length
            if (fValue)
                fValue[Length] = 0;
            Length++;           // make Length even
        }
        else
            fValue = new Uint8[Length + pad];

        if (!fValue)
            errorFlag = EC_MemoryExhausted;

        if (pad && fValue)
            fValue[Length] = 0;

        memcpy(fValue, elem.fValue, size_t(Length + pad));
    }

    if (elem.fLoadValue)
        fLoadValue = elem.fLoadValue->clone();
}


DcmElement &DcmElement::operator=(const DcmElement &obj)
{
    DcmObject::operator=(obj);
    fByteOrder = obj.fByteOrder;
    fLoadValue = NULL;
    fValue = NULL;

    if (obj.fValue)
    {
        DcmVR vr(obj.getVR());
        const unsigned short pad = (vr.isaString()) ? 1 : 0;

        // The next lines are a special version of newValueField().
        // newValueField() cannot be used because it is virtual and it does
        // not allocate enough bytes for strings. The number of pad bytes
        // is added to the Length for this purpose.

        if (Length & 1)
        {
            fValue = new Uint8[Length + 1 + pad]; // protocol error: odd value length
            if (fValue)
                fValue[Length] = 0;
            Length++;           // make Length even
        }
        else
            fValue = new Uint8[Length + pad];

        if (!fValue)
            errorFlag = EC_MemoryExhausted;

        if (pad && fValue)
            fValue[Length] = 0;

        memcpy(fValue, obj.fValue, size_t(Length+pad));
    }

    if (obj.fLoadValue)
        fLoadValue = obj.fLoadValue->clone();

    return *this;
}


DcmElement::~DcmElement()
{
    delete[] fValue;
    delete fLoadValue;
}


// ********************************


OFCondition DcmElement::clear()
{
    errorFlag = EC_Normal;
    delete[] fValue;
    fValue = NULL;
    delete fLoadValue;
    fLoadValue = NULL;
    Length = 0;
    return errorFlag;
}


// ********************************


Uint32 DcmElement::calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    DcmXfer xferSyn(xfer);
    return getLength(xfer, enctype) + xferSyn.sizeofTagHeader(getVR());
}


OFBool DcmElement::canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax /*oldXfer*/)
{
    return newXfer != EXS_Unknown;
}


OFCondition DcmElement::detachValueField(OFBool copy)
{
    OFCondition l_error = EC_Normal;
    if (Length != 0)
    {
        if (copy)
        {
            if (!fValue)
                l_error = loadValue();
            Uint8 * newValue = new Uint8[Length];
            memcpy(newValue, fValue, size_t(Length));
            fValue = newValue;
        } else {
            fValue = NULL;
            Length = 0;
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmElement::getUint8(Uint8 & /*val*/,
                                 const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint16(Sint16 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint16(Uint16 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint32(Sint32 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint32(Uint32 & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat32(Float32 & /*val*/,
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat64(Float64 & /*val*/,
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getTagVal(DcmTagKey & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getOFString(OFString &/*val*/,
                                    const unsigned long /*pos*/,
                                    OFBool /*normalize*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getString(char * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getOFStringArray(OFString &value,
                                         OFBool normalize)
{
    /* this is a general implementation which is only used when the derived
       VR class does not reimplement it
     */
    OFString string;
    unsigned long i = 0;
    const unsigned long count = getVM();
    /* iterate over all values and convert them to a character string */
    while ((i < count) && (errorFlag = getOFString(string, i, normalize)).good())
    {
        /* initialize result string */
        if (i == 0)
        {
            /* reserve number of bytes expected (heuristic) */
            value.reserve((unsigned int)getLength());
            value.clear();
        } else
            value += '\\';
        /* append current value to the result string */
        value += string;
        i++;
    }
    return errorFlag;
}


OFCondition DcmElement::getUint8Array(Uint8 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint16Array(Sint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint16Array(Uint16 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getSint32Array(Sint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getUint32Array(Uint32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat32Array(Float32 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::getFloat64Array(Float64 * &/*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


void *DcmElement::getValue(const E_ByteOrder newByteOrder)
{
    /* initialize return value */
    Uint8 * value = NULL;
    /* if the byte ordering is unknown, this is an illegal call */
    if (newByteOrder == EBO_unknown)
        errorFlag = EC_IllegalCall;
    else
    {
        /* in case this call is not illegal, we need to do something. First of all, set the error flag to ok */
        errorFlag =  EC_Normal;
        /* do something only if the length of this element's value does not equal (i.e. is greater than) 0 */
        if (Length != 0)
        {
            /* if the value has not yet been loaded, do so now */
            if (!fValue)
                errorFlag = loadValue();
            /* if everything is ok */
            if (errorFlag.good())
            {
                /* if this element's value's byte ordering does not correspond to the */
                /* desired byte ordering, we need to rearrange this value's bytes and */
                /* set its byte order indicator variable correspondingly */
                if (newByteOrder != fByteOrder)
                {
                    swapIfNecessary(newByteOrder, fByteOrder, fValue,
                                    Length, Tag.getVR().getValueWidth());
                    fByteOrder = newByteOrder;
                }
                /* if everything is ok, assign the current value to the result variable */
                if (errorFlag.good())
                    value = fValue;
            }
        }
    }
    /* return result */
    return value;
}


// ********************************


OFCondition DcmElement::loadAllDataIntoMemory()
{
    errorFlag = EC_Normal;
    if (!fValue && (Length != 0))
        errorFlag = loadValue();
    return errorFlag;
}


OFCondition DcmElement::loadValue(DcmInputStream *inStream)
{
    /* initiailze return value */
    errorFlag = EC_Normal;
    /* only if the length of this element does not equal 0, read information */
    if (Length != 0)
    {
        DcmInputStream *readStream = inStream;
        OFBool isStreamNew = OFFalse;
        /* if the NULL pointer was passed and the fLoadValue contains a non NULL pointer */
        if (!readStream && fLoadValue)
        {
            /* we need to read information from the stream which is */
            /* accessible through fLoadValue. Hence, reassign readStream */
            readStream = fLoadValue->create();
            /* set isStreamNew to OFTrue */
            isStreamNew = OFTrue;
            /* and delete the fLoadValue object which is superfluous now */
            delete fLoadValue;
            fLoadValue = NULL;
        }
        /* if we have a stream from which we can read */
        if (readStream)
        {
            /* check if the stream reported an error */
            errorFlag = readStream->status();
            /* if we encountered the end of the stream, set the error flag correspondingly */
            if (errorFlag.good() && readStream->eos())
                errorFlag = EC_EndOfStream;
            /* if we did not encounter the end of the stream and no error occurred so far, go ahead */
            else if (errorFlag.good())
            {
                /* if the object which holds this element's value does not yet exist, create it */
                if (!fValue)
                    fValue = newValueField();

                /* if there is still no such object, the memory must be exhausted */
                if (!fValue)
                    errorFlag = EC_MemoryExhausted;
                /* else (i.e. we have an object which can be used to capture this element's */
                /* value) we need to read a certain amount of bytes from the stream */
                else
                {
                    /* determine how many bytes shall be read from the stream */
                    Uint32 readLength = Length - fTransferredBytes;

                    /* read a corresponding amount of bytes from the stream, store the information in fvalue */
                    /* increase the counter that counts how many bytes were actually read */
                    fTransferredBytes += readStream->read(&fValue[fTransferredBytes], readLength);

                    /* if we have read all the bytes which make up this element's value */
                    if (Length == fTransferredBytes)
                    {
                        /* call a function which performs certain operations on the information which was read */
                        postLoadValue();
                        errorFlag = EC_Normal;
                    }
                    /* else set the return value correspondingly */
                    else if (readStream->eos())
                        errorFlag = EC_InvalidStream; // premature end of stream
                    else
                        errorFlag = EC_StreamNotifyClient;
                }
            }
            /* if we created the stream from which information was read in this */
            /* function, we need to we need to delete this object here as well */
            if (isStreamNew)
                delete readStream;
        }
    }
    /* return result value */
    return errorFlag;
}


// ********************************


Uint8 *DcmElement::newValueField()
{
    Uint8 * value;
    /* if this element's lenght is odd */
    if (Length & 1)
    {
        /* create an array of Length+1 bytes */
        value = new Uint8[Length + 1];    // protocol error: odd value length
        /* if creation was successful, set last byte to 0 (in order to initialize this byte) */
        /* (no value will be assigned to this byte later, since Length was odd) */
        if (value)
            value[Length] = 0;
        /* enforce old (pre DCMTK 3.5.2) behaviour ? */
        if (! dcmAcceptOddAttributeLength.get())
        {
            Length++;  // make Length even
        }
    }
    /* if this element's length is even, create a corresponding array of Lenght bytes */
    else
        value = new Uint8[Length];
    /* if creation was not successful set member error flag correspondingly */
    if (!value)
        errorFlag = EC_MemoryExhausted;
    /* return byte array */
    return value;
}


// ********************************


void DcmElement::postLoadValue()
{
    if (dcmEnableAutomaticInputDataCorrection.get())
    {
        // newValueField always allocates an even number of bytes
        // and sets the pad byte to zero, so we can safely increase Length here
        if (Length & 1)
            Length++;
    }
}


// ********************************


OFCondition DcmElement::changeValue(const void *value,
                                    const Uint32 position,
                                    const Uint32 num)
{
    OFBool done = OFFalse;
    errorFlag = EC_Normal;
    if (position % num != 0 || Length % num != 0 || position > Length)
        errorFlag = EC_IllegalCall;
    else if (position == Length)
    {
        if (Length == 0)
        {
            errorFlag = putValue(value, num);
            done = OFTrue;
        } else {
            // load value (if not loaded yet)
            if (!fValue)
                loadValue();
            // allocate new memory for value
            Uint8 * newValue = new Uint8[Length + num];
            if (!newValue)
                errorFlag = EC_MemoryExhausted;
            if (errorFlag.good())
            {
                // swap to local byte order
                swapIfNecessary(gLocalByteOrder, fByteOrder, fValue,
                                Length, Tag.getVR().getValueWidth());
                fByteOrder = gLocalByteOrder;
                // copy old value in the beginning of new value
                memcpy(newValue, fValue, size_t(Length));
                // set parameter value in the extension
                memcpy(&newValue[Length], (const Uint8*)value, size_t(num));
                delete[] fValue;
                fValue = newValue;
                Length += num;
            }
            done = OFTrue;
        }
    }
    // copy value at position
    if (!done && errorFlag.good())
    {
        // swap to local byte order
        swapIfNecessary(gLocalByteOrder, fByteOrder, fValue,
                        Length, Tag.getVR().getValueWidth());
        memcpy(&fValue[position], (const Uint8 *)value, size_t(num));
        fByteOrder = gLocalByteOrder;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmElement::putOFStringArray(const OFString& /* stringValue*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putString(const char * /*val*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint16(const Sint16 /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint16(const Uint16 /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint32(const Sint32 /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint32(const Uint32 /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat32(const Float32 /*val*/,
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat64(const Float64 /*val*/,
                                   const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putTagVal(const DcmTagKey & /*val*/,
                                  const unsigned long /*pos*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint8Array(const Uint8 * /*val*/,
                                      const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint16Array(const Sint16 * /*val*/,
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint16Array(const Uint16 * /*val*/,
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putSint32Array(const Sint32 * /*val*/,
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putUint32Array(const Uint32 * /*val*/,
                                       const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat32Array(const Float32 * /*val*/,
                                        const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putFloat64Array(const Float64 * /*val*/,
                                        const unsigned long /*num*/)
{
    errorFlag = EC_IllegalCall;
    return errorFlag;
}


OFCondition DcmElement::putValue(const void * newValue,
                                 const Uint32 length)
{
    errorFlag = EC_Normal;

    if (fValue)
        delete[] fValue;
    fValue = NULL;

    if (fLoadValue)
        delete fLoadValue;
    fLoadValue = NULL;

    Length = length;

    if (length != 0)
    {
        fValue = newValueField();

        // newValueField always allocates an even number of bytes
        // and sets the pad byte to zero, so we can safely increase Length here
        if (Length & 1)
            Length++;

        // copy length (which may be odd), not Length (which is always even)
        if (fValue)
            memcpy(fValue, newValue, size_t(length));
        else
            errorFlag = EC_MemoryExhausted;
    }
    fByteOrder = gLocalByteOrder;
    return errorFlag;
}


// ********************************


OFCondition DcmElement::createEmptyValue(const Uint32 length)
{
    errorFlag = EC_Normal;
    if (fValue)
        delete[] fValue;
    fValue = NULL;
    if (fLoadValue)
        delete fLoadValue;
    fLoadValue = NULL;
    Length = length;

    if (length != 0)
    {
        fValue = newValueField();
        // newValueField always allocates an even number of bytes
        // and sets the pad byte to zero, so we can safely increase Length here
        if (Length & 1)
            Length++;
        // initialize <length> bytes (which may be odd), not Length (which is always even)
        if (fValue)
            memzero(fValue, size_t(length));
        else
            errorFlag = EC_MemoryExhausted;
    }
    fByteOrder = gLocalByteOrder;
    return errorFlag;
}


// ********************************


OFCondition DcmElement::read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding /*glenc*/,
                             const Uint32 maxReadLength)
{
    /* if this element's transfer state shows ERW_notInitialized, this is an illegal call */
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, go ahead and create a DcmXfer */
        /* object based on the transfer syntax which was passed */
        DcmXfer inXfer(ixfer);
        /* determine the transfer syntax's byte ordering */
        fByteOrder = inXfer.getByteOrder();
        /* check if the stream reported an error */
        errorFlag = inStream.status();
        /* if we encountered the end of the stream, set the error flag correspondingly */
        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        /* if we did not encounter the end of the stream and no error occurred so far, go ahead */
        else if (errorFlag.good())
        {
            /* if the transfer state is ERW_init, we need to prepare */
            /* the reading of this element's value from the stream */
            if (fTransferState == ERW_init)
            {
                /* if the Length of this element's value is greater than the amount of bytes we */
                /* can read from the stream and if the stream has random access, we want to create */
                /* a DcmInputStreamFactory object that enables us to read this element's value later. */
                /* This new object will be stored (together with the position where we have to start */
                /* reading the value) in the member variable fLoadValue. */
                if (Length > maxReadLength)
                {
                    /* try to create a stream factory to read the value later */
                    delete fLoadValue;
                    fLoadValue = inStream.newFactory();

                    if (fLoadValue)
                    {
                        Uint32 skipped = inStream.skip(Length);
                        if (skipped < Length)
                        {
                            errorFlag = EC_InvalidStream;  // attribute larger than remaining bytes in file
                            /* Print an error message when too few bytes are available in the file in order to
                             * distinguish this problem from any other generic "InvalidStream" problem. */
                             ofConsole.lockCerr() << "ERROR: " << Tag.getTagName() << Tag.getXTag() << " larger ("
                                                  << Length << ") that remaining bytes in file" << endl;
                             ofConsole.unlockCerr();
                        }
                    }
                }
                /* if there is already a value for this element, delete this value */
                delete[] fValue;
                /* set the transfer state to ERW_inWork */
                fTransferState = ERW_inWork;
            }
            /* if the transfer state is ERW_inWork and we are not supposed */
            /* to read this element's value later, read the value now */
            if (fTransferState == ERW_inWork && !fLoadValue)
                errorFlag = loadValue(&inStream);
            /* if the amount of transferred bytes equals the Length of this element */
            /* or the object which contains information to read the value of this */
            /* element later is existent, set the transfer state to ERW_ready */
            if (fTransferredBytes == Length || fLoadValue)
                fTransferState = ERW_ready;
        }
    }

    /* return result value */
    return errorFlag;
}


// ********************************


void DcmElement::swapValueField(size_t valueWidth)
{
    if (Length != 0)
    {
        if (!fValue)
            errorFlag = loadValue();

        if (errorFlag.good())
            swapBytes(fValue, Length, valueWidth);
    }
}


// ********************************


void DcmElement::transferInit()
{
    DcmObject::transferInit();
    fTransferredBytes = 0;
}


// ********************************


OFCondition DcmElement::write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType /*enctype*/)
{
    /* In case the transfer state is not initialized, this is an illegal call */
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, we need to do something. First */
        /* of all, check the error state of the stream that was passed */
        /* only do something if the error state of the stream is ok */
        errorFlag = outStream.status();
        if (errorFlag.good())
        {
            /* create an object that represents the transfer syntax */
            DcmXfer outXfer(oxfer);
            /* get this element's value. Mind the byte ordering (little */
            /* or big endian) of the transfer syntax which shall be used */
            Uint8 *value = (Uint8 *)getValue(outXfer.getByteOrder());
            /* if this element's transfer state is ERW_init (i.e. it has not yet been written to */
            /* the stream) and if the outstream provides enough space for tag and length information */
            /* write tag and length information to it, do something */
            if (fTransferState == ERW_init)
            {
                if (outStream.avail() >= DCM_TagInfoLength)
                {
                    /* if there is no value, Length (member variable) shall be set to 0 */
                    if (!value) Length = 0;
                    /* remember how many bytes have been written to the stream, currently none so far */
                    Uint32 writtenBytes = 0;
                    /* write tag and length information (and possibly also data type information) to the stream, */
                    /* mind the transfer syntax and remember the amount of bytes that have been written */
                    errorFlag = writeTagAndLength(outStream, oxfer, writtenBytes);
                    /* if the writing was successful, set this element's transfer */
                    /* state to ERW_inWork and the amount of transferred bytes to 0 */
                    if (errorFlag.good())
                    {
                        fTransferState = ERW_inWork;
                        fTransferredBytes = 0;
                    }
                } else
                    errorFlag = EC_StreamNotifyClient;
            }
            /* if there is a value that has to be written to the stream */
            /* and if this element's transfer state is ERW_inWork */
            if (value && fTransferState == ERW_inWork)
            {
                /* write as many bytes as possible to the stream starting at value[fTransferredBytes] */
                /* (note that the bytes value[0] to value[fTransferredBytes-1] have already been */
                /* written to the stream) */
                Uint32 len = outStream.write(&value[fTransferredBytes], Length - fTransferredBytes);
                /* increase the amount of bytes which have been transfered correspondingly */
                fTransferredBytes += len;
                /* see if there is something fishy with the stream */
                errorFlag = outStream.status();
                /* if the amount of transferred bytes equals the length of the element's value, the */
                /* entire value has been written to the stream. Thus, this element's transfer state */
                /* has to be set to ERW_ready. If this is not the case but the error flag still shows */
                /* an ok value, there was no more space in the stream and a corresponding return value */
                /* has to be set. (Isn't the "else if" part superfluous?!?) */
                if (fTransferredBytes == Length)
                    fTransferState = ERW_ready;
                else if (errorFlag.good())
                    errorFlag = EC_StreamNotifyClient;
            }
        }
    }
    /* return result value */
    return errorFlag;
}


OFCondition DcmElement::writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype)
{
    // for normal DICOM elements (everything except sequences), the data
    // stream used for digital signature creation or verification is
    // identical to the stream used for network communication or media
    // storage.
    return write(outStream, oxfer, enctype);
}


// ********************************


void DcmElement::writeXMLStartTag(ostream &out,
                                  const size_t /*flags*/,
                                  const char *attrText)
{
    OFString xmlString;
    DcmVR vr(Tag.getVR());
    /* write standardized XML start tag for all element types */
    out << "<element";
    /* attribute tag = (gggg,eeee) */
    out << " tag=\"";
    out << hex << setfill('0')
        << setw(4) << Tag.getGTag() << ","
        << setw(4) << Tag.getETag() << "\""
        << dec << setfill(' ');
    /* value representation = VR */
    out << " vr=\"" << vr.getVRName() << "\"";
    /* value multiplicity = 1..n */
    out << " vm=\"" << getVM() << "\"";
    /* value length in bytes = 0..max */
    out << " len=\"" << Length << "\"";
    /* tag name (if known) */
    out << " name=\"" << OFStandard::convertToMarkupString(Tag.getTagName(), xmlString) << "\"";
    /* value loaded = no (or absent)*/
    if (!valueLoaded())
        out << " loaded=\"no\"";
    /* write additional attributes (if any) */
    if ((attrText != NULL) && (strlen(attrText) > 0))
        out << " " << attrText;
    out << ">";
}


void DcmElement::writeXMLEndTag(ostream &out,
                                const size_t /*flags*/)
{
    /* write standardized XML end tag for all element types */
    out << "</element>" << endl;
}


OFCondition DcmElement::writeXML(ostream &out,
                                 const size_t flags)
{
    /* XML start tag: <element tag="gggg,eeee" vr="XX" ...> */
    writeXMLStartTag(out, flags);
    /* write element value (if loaded) */
    if (valueLoaded())
    {
        OFString value, xmlString;
        if (getOFStringArray(value).good())
            out << OFStandard::convertToMarkupString(value, xmlString);
    }
    /* XML end tag: </element> */
    writeXMLEndTag(out, flags);
    /* always report success */
    return EC_Normal;
}
