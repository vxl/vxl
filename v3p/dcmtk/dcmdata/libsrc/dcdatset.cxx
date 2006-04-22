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
 *  Purpose: Implementation of class DcmDataset
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"

#include "ofstream.h"
#include "ofstack.h"
#include "ofstd.h"

#include "dcdatset.h"
#include "dcxfer.h"
#include "dcvrus.h"
#include "dcdebug.h"
#include "dcpixel.h"
#include "dcdeftag.h"
#include "dcostrma.h"    /* for class DcmOutputStream */
#include "dcostrmf.h"    /* for class DcmOutputFileStream */
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcistrmf.h"    /* for class DcmInputFileStream */


// ********************************


DcmDataset::DcmDataset()
  : DcmItem(ItemTag, DCM_UndefinedLength),
    Xfer(EXS_Unknown)
{
}


DcmDataset::DcmDataset(const DcmDataset &old)
  : DcmItem(old),
    Xfer(old.Xfer)
{
}


DcmDataset::~DcmDataset()
{
}


// ********************************


DcmEVR DcmDataset::ident() const
{
    return EVR_dataset;
}


E_TransferSyntax DcmDataset::getOriginalXfer() const
{
    return Xfer;
}


// ********************************


Uint32 DcmDataset::calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    return DcmItem::getLength(xfer, enctype);
}


// ********************************


OFBool DcmDataset::canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer)
{
    register E_TransferSyntax originalXfer = Xfer;
    if (newXfer == EXS_Unknown)
        return OFFalse;
    if (Xfer == EXS_Unknown)
        originalXfer = oldXfer;

    /* Check stream compression for this transfer syntax */
    DcmXfer xf(newXfer);
    if (xf.getStreamCompression() == ESC_unsupported)
        return OFFalse;

    return DcmItem::canWriteXfer(newXfer, originalXfer);
}


// ********************************


void DcmDataset::print(ostream &out,
                       const size_t flags,
                       const int level,
                       const char *pixelFileName,
                       size_t *pixelCounter)
{
    out << endl;
    printNestingLevel(out, flags, level);
    out << "# Dicom-Data-Set" << endl;
    printNestingLevel(out, flags, level);
    out << "# Used TransferSyntax: " << DcmXfer(Xfer).getXferName();
    out << endl;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
        } while (elementList->seek(ELP_next));
    }
}


// ********************************


OFCondition DcmDataset::writeXML(ostream &out,
                                 const size_t flags)
{
    OFString xmlString;
    DcmXfer xfer(Xfer);
    /* XML start tag for "data-set" */
    out << "<data-set xfer=\"" << xfer.getXferID() << "\"";
    out << " name=\"" << OFStandard::convertToMarkupString(xfer.getXferName(), xmlString) << "\">" << endl;
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
    /* XML end tag for "data-set" */
    out << "</data-set>" << endl;
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmDataset::read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc,
                             const Uint32 maxReadLength)
{
    /* check if the stream variable reported an error */
    errorFlag = inStream.status();
    /* if the stream did not report an error but the stream */
    /* is empty, set the error flag correspondingly */
    if (errorFlag.good() && inStream.eos())
        errorFlag = EC_EndOfStream;
    /* else if the stream did not report an error but the transfer */
    /* state does not equal ERW_ready, go ahead and do something */
    else if (errorFlag.good() && fTransferState != ERW_ready)
    {
        /* if the transfer state is ERW_init, go ahead and check the transfer syntax which was passed */
        if (fTransferState == ERW_init)
        {
            /* if the transfer syntax which was passed equals EXS_Unknown we want to */
            /* determine the transfer syntax from the information in the stream itself. */
            /* If the transfer syntax is given, we want to use it. */
            if (xfer == EXS_Unknown)
                Xfer = checkTransferSyntax(inStream);
            else
                Xfer = xfer;
            /* Check stream compression for this transfer syntax */
            DcmXfer xf(Xfer);
            E_StreamCompression sc = xf.getStreamCompression();
            switch (sc)
            {
                case ESC_none:
                  // nothing to do
                  break;
                case ESC_unsupported:
                  // stream compressed transfer syntax that we cannot create; bail out.
                  if (errorFlag.good())
                      errorFlag = EC_UnsupportedEncoding;
                  break;
                default:
                  // supported stream compressed transfer syntax, install filter
                  errorFlag = inStream.installCompressionFilter(sc);
                  break;
            }
        }
        /* pass processing the task to class DcmItem */
        if (errorFlag.good())
            errorFlag = DcmItem::read(inStream, Xfer, glenc, maxReadLength);
    }

    /* if the error flag shows ok or that the end of the stream was encountered, */
    /* we have read information for this particular data set or command; in this */
    /* case, we need to do something for the current dataset object */
    if (errorFlag.good() || errorFlag == EC_EndOfStream)
    {
        /* set the error flag to ok */
        errorFlag = EC_Normal;

        /* take care of group length (according to what is specified */
        /* in glenc) and padding elements (don't change anything) */
        computeGroupLengthAndPadding(glenc, EPD_noChange, Xfer);

        /* and set the transfer state to ERW_ready to indicate that the data set is complete */
        fTransferState = ERW_ready;
    }

    /* dump information if required */
    debug(3, ("DcmDataset::read: At End: errorFlag = %s", errorFlag.text()));

    /* return result flag */
    return errorFlag;
}


// ********************************


OFCondition DcmDataset::write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype)
{
    return write(outStream, oxfer, enctype, EGL_recalcGL);
}


OFCondition DcmDataset::write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              const E_GrpLenEncoding glenc,
                              const E_PaddingEncoding padenc,
                              const Uint32 padlen,
                              const Uint32 subPadlen,
                              Uint32 instanceLength)
{
  /* if the transfer state of this is not initialized, this is an illegal call */
  if (fTransferState == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    /* check if the stream reported an error so far; if not, we can go ahead and write some data to it */
    errorFlag = outStream.status();

    if (errorFlag.good() && fTransferState != ERW_ready)
    {
      /* Determine the transfer syntax which shall be used. Either we use the one which was passed, */
      /* or (if it's an unknown transfer syntax) we use the one which is contained in this->Xfer. */
      E_TransferSyntax newXfer = oxfer;
      if (newXfer == EXS_Unknown)
        newXfer = Xfer;

      /* if this function was called for the first time for the dataset object, the transferState is still */
      /* set to ERW_init. In this case, we need to take care of group length and padding elements according */
      /* to the strategies which are specified in glenc and padenc. Additionally, we need to set the element */
      /* list pointer of this data set to the fist element and we need to set the transfer state to ERW_inWork */
      /* so that this scenario will only be executed once for this data set object. */
      if (fTransferState == ERW_init)
      {
        /* Check stream compression for this transfer syntax */
        DcmXfer xf(newXfer);
        E_StreamCompression sc = xf.getStreamCompression();
        switch (sc)
        {
          case ESC_none:
            // nothing to do
            break;
          case ESC_unsupported:
            // stream compressed transfer syntax that we cannot create; bail out.
            if (errorFlag.good())
              errorFlag = EC_UnsupportedEncoding;
            break;
          default:
            // supported stream compressed transfer syntax, install filter
            errorFlag = outStream.installCompressionFilter(sc);
            break;
        }

        /* take care of group length and padding elements, according to what is specified in glenc and padenc */
        computeGroupLengthAndPadding(glenc, padenc, newXfer, enctype, padlen, subPadlen, instanceLength);
        elementList->seek(ELP_first);
        fTransferState = ERW_inWork;
      }

      /* if the transfer state is set to ERW_inWork, we need to write the information which */
      /* is included in this data set's element list into the buffer which was passed. */
      if (fTransferState == ERW_inWork)
      {
        // Remember that elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          /* as long as everything is ok, go through all elements of this data */
          /* set and write the corresponding information to the buffer */
          DcmObject *dO;
          do
          {
            dO = elementList->get();
            errorFlag = dO->write(outStream, newXfer, enctype);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }

        /* if all the information in this has been written to the */
        /* buffer set this data set's transfer state to ERW_ready */
        if (errorFlag.good())
          fTransferState = ERW_ready;
      }
    }
  }

  /* return the corresponding result value */
  return errorFlag;
}


// ********************************


OFCondition DcmDataset::writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype)
{
  if (fTransferState == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    E_TransferSyntax newXfer = oxfer;
    if (newXfer == EXS_Unknown)
      newXfer = Xfer;

    errorFlag = outStream.status();
    if (errorFlag.good() && fTransferState != ERW_ready)
    {
      if (fTransferState == ERW_init)
      {
        computeGroupLengthAndPadding(EGL_recalcGL, EPD_noChange, newXfer, enctype, 0, 0, 0);
        elementList->seek(ELP_first);
        fTransferState = ERW_inWork;
      }
      if (fTransferState == ERW_inWork)
      {
        // elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          DcmObject *dO;
          do {
            dO = elementList->get();
            errorFlag = dO->writeSignatureFormat(outStream, newXfer, enctype);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }
        if (errorFlag.good())
          fTransferState = ERW_ready;
      }
    }
  }
  return errorFlag;
}


// ********************************


OFCondition DcmDataset::loadFile(const char *filename,
                                 const E_TransferSyntax readXfer,
                                 const E_GrpLenEncoding groupLength,
                                 const Uint32 maxReadLength)
{
    OFCondition l_error = EC_IllegalParameter;
    /* check parameters first */
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        /* open file for input */
        DcmInputFileStream fileStream(filename);

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


OFCondition DcmDataset::saveFile(const char *fileName,
                                 const E_TransferSyntax writeXfer,
                                 const E_EncodingType encodingType,
                                 const E_GrpLenEncoding groupLength,
                                 const E_PaddingEncoding padEncoding,
                                 const Uint32 padLength,
                                 const Uint32 subPadLength)
{
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


OFCondition DcmDataset::chooseRepresentation(const E_TransferSyntax repType,
                                             const DcmRepresentationParameter *repParam)
{
    OFCondition l_error = EC_Normal;
    OFStack<DcmStack> pixelStack;

    DcmStack resultStack;
    resultStack.push(this);
    while (search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good() && l_error.good())
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData * pixelData = (DcmPixelData *)(resultStack.top());
            if (!pixelData->canChooseRepresentation(repType, repParam))
                l_error = EC_CannotChangeRepresentation;
            pixelStack.push(resultStack);
        }
        else
            l_error = EC_CannotChangeRepresentation;
    }
    if (l_error.good())
    {
        while (pixelStack.size() && l_error.good())
        {
            l_error = ((DcmPixelData*)(pixelStack.top().top()))->
                chooseRepresentation(repType, repParam, pixelStack.top());

#ifdef PIXELSTACK_MEMORY_LEAK_WORKAROUND
            // on certain platforms there seems to be a memory leak
            // at this point since for some reason pixelStack.pop does
            // not completely destruct the DcmStack object taken from the stack.
            // The following work-around should solve this issue.
            pixelStack.top().clear();
#endif

            pixelStack.pop();
        }
    }
    return l_error;
}


OFBool DcmDataset::hasRepresentation(const E_TransferSyntax repType,
                                     const DcmRepresentationParameter *repParam)
{
    OFBool result = OFTrue;
    DcmStack resultStack;

    while (search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good() && result)
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData *pixelData = (DcmPixelData *)(resultStack.top());
            result = pixelData->hasRepresentation(repType, repParam);
        }
        else
            result = OFFalse;
    }
    return result;
}


void DcmDataset::removeAllButCurrentRepresentations()
{
    DcmStack resultStack;

    while (search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good())
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData *pixelData = (DcmPixelData *)(resultStack.top());
            pixelData->removeAllButCurrentRepresentations();
        }
    }
}


void DcmDataset::removeAllButOriginalRepresentations()
{
    DcmStack resultStack;

    while (search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good())
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData *pixelData = (DcmPixelData *)(resultStack.top());
            pixelData->removeAllButOriginalRepresentations();
        }
    }
}
