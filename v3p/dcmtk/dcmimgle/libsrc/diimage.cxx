/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomImage (Source)
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dcswap.h"

#include "diimage.h"
#include "diinpxt.h"
#include "didocu.h"
#include "diutils.h"
#include "ofstd.h"

#define INCLUDE_CSTRING
#include "ofstdinc.h"

/*----------------*
 *  constructors  *
 *----------------*/

DiImage::DiImage(const DiDocument *docu,
                 const EI_Status status,
                 const int spp)
  : ImageStatus(status),
    Document(docu),
    FirstFrame(0),
    NumberOfFrames(0),
    RepresentativeFrame(0),
    Rows(0),
    Columns(0),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(0),
    BitsStored(0),
    HighBit(0),
    BitsPerSample(0),
    Polarity(EPP_Normal),
    hasSignedRepresentation(0),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(1),
    InputData(NULL)
{
    if ((Document != NULL) && (ImageStatus == EIS_Normal))
    {
        Sint32 sl = 0;
        if (Document->getValue(DCM_NumberOfFrames, sl))
        {
            if (sl < 1)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'NumberOfFrames' (" << sl << ") "
                                         << "... assuming 1 !" << endl;
                    ofConsole.unlockCerr();
                }
                NumberOfFrames = 1;
            }
            else
                NumberOfFrames = (Uint32)sl;
        }
        else
            NumberOfFrames = 1;
        Uint16 us = 0;
        if (Document->getValue(DCM_RepresentativeFrameNumber, us))
        {
            if (us <= FirstFrame)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'RepresentativeFrameNumber' (" << us << ")" << endl
                                         << "         ... assuming first frame !" << endl;
                    ofConsole.unlockCerr();
                }
                RepresentativeFrame = FirstFrame;
            }
            else if (us > NumberOfFrames)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'RepresentativeFrameNumber' (" << us << ")" << endl
                                         << "         ... assuming last frame !" << endl;
                    ofConsole.unlockCerr();
                }
                RepresentativeFrame = NumberOfFrames - 1;
            }
            else
                RepresentativeFrame = us - 1;
        }
        FirstFrame = (docu->getFrameStart() < NumberOfFrames) ? docu->getFrameStart() : NumberOfFrames - 1;
        /* restrict to actually processed/loaded number of frames */
        NumberOfFrames -= FirstFrame;
        if ((docu->getFrameCount() > 0) && (NumberOfFrames > docu->getFrameCount()))
            NumberOfFrames = docu->getFrameCount();
        /* start from first processed frame (might still exceed number of loaded frames) */
        RepresentativeFrame -= FirstFrame;
        int ok = (Document->getValue(DCM_Rows, Rows) > 0);
        ok &= (Document->getValue(DCM_Columns, Columns) > 0);
        if (!ok || ((Rows > 0) && (Columns > 0)))
        {
            ok &= (Document->getValue(DCM_BitsAllocated, BitsAllocated) > 0);
            ok &= (Document->getValue(DCM_BitsStored, BitsStored) > 0);
            ok &= (Document->getValue(DCM_HighBit, HighBit) > 0);
            ok &= (Document->getValue(DCM_PixelRepresentation, us) > 0);
            BitsPerSample = BitsStored;
            hasSignedRepresentation = (us == 1);
            if ((us != 0) && (us != 1))
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'PixelRepresentation' (" << us << ") "
                                         << "... assuming 'unsigned' (0) !" << endl;
                    ofConsole.unlockCerr();
                }
            }
            if (!(Document->getFlags() & CIF_UsePresentationState))
            {
                hasPixelSpacing = (Document->getValue(DCM_PixelSpacing, PixelHeight, 0) > 0);
                if (hasPixelSpacing)
                {
                    if (Document->getValue(DCM_PixelSpacing, PixelWidth, 1) < 2)
                    {
                        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                        {
                            ofConsole.lockCerr() << "WARNING: missing second value for 'PixelSpacing' ... "
                                                 << "assuming 'Width' = " << PixelWidth << " !" << endl;
                            ofConsole.unlockCerr();
                        }
                    }
                } else {
                    hasImagerPixelSpacing = (Document->getValue(DCM_ImagerPixelSpacing, PixelHeight, 0) > 0);
                    if (hasImagerPixelSpacing)
                    {
                        if (Document->getValue(DCM_ImagerPixelSpacing, PixelWidth, 1) < 2)
                        {
                            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                            {
                                ofConsole.lockCerr() << "WARNING: missing second value for 'ImagerPixelSpacing' ... "
                                                     << "assuming 'Width' = " << PixelWidth << " !" << endl;
                                ofConsole.unlockCerr();
                            }
                        }
                    } else {
                        Sint32 sl2;
                        hasPixelAspectRatio = (Document->getValue(DCM_PixelAspectRatio, sl2, 0) > 0);
                        PixelHeight = sl2;
                        if (hasPixelAspectRatio)
                        {
                            if (Document->getValue(DCM_PixelAspectRatio, sl2, 1) < 2)
                            {
                                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                                {
                                    ofConsole.lockCerr() << "WARNING: missing second value for 'PixelAspectRatio' ... "
                                                         << "assuming 'Width' = " << PixelWidth << " !" << endl;
                                    ofConsole.unlockCerr();
                                }
                            } else
                                PixelWidth = sl2;
                        } else {
                            PixelWidth = 1;
                            PixelHeight = 1;
                        }
                    }
                }
                checkPixelExtension();
            }
            DcmStack pstack;
            // get pixel data (if present)
            if (ok && Document->search(DCM_PixelData, pstack))
            {
                DcmPixelData *pixel = (DcmPixelData *)pstack.top();
                // check whether pixel data exists unencapsulated (decompression already done in DiDocument)
                if ((pixel != NULL) && (DcmXfer(Document->getTransferSyntax()).isNotEncapsulated()))
                    convertPixelData(pixel, spp);
                else
                    ImageStatus = EIS_InvalidValue;
            }
            else
            {
                ImageStatus = EIS_MissingAttribute;
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                {
                    ofConsole.lockCerr() << "ERROR: one or more mandatory attributes are missing in image pixel module !" << endl;
                    ofConsole.unlockCerr();
                }
            }
        }
        else
        {
            ImageStatus = EIS_InvalidValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: invalid value for 'Rows' (" << Rows << ") and/or 'Columns' (" << Columns << ") !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
    else
    {
        ImageStatus = EIS_InvalidDocument;
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: this DICOM document is invalid !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


DiImage::DiImage(const DiDocument *docu,
                 const EI_Status status)
  : ImageStatus(status),
    Document(docu),
    FirstFrame(0),
    NumberOfFrames(0),
    RepresentativeFrame(0),
    Rows(0),
    Columns(0),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(0),
    BitsStored(0),
    HighBit(0),
    BitsPerSample(0),
    Polarity(EPP_Normal),
    hasSignedRepresentation(0),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(1),
    InputData(NULL)
{
}


DiImage::DiImage(const DiImage *image,
                 const unsigned long fstart,
                 const unsigned long fcount)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame + fstart),
    NumberOfFrames(fcount),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(image->Rows),
    Columns(image->Columns),
    PixelWidth(image->PixelWidth),
    PixelHeight(image->PixelHeight),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    Polarity(image->Polarity),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL)
{
}


/* constructor: image scaled */

DiImage::DiImage(const DiImage *image,
                 const Uint16 columns,
                 const Uint16 rows,
                 const int aspect)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame),
    NumberOfFrames(image->NumberOfFrames),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(rows),
    Columns(columns),
    PixelWidth(1),
    PixelHeight(1),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    Polarity(image->Polarity),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(0),
    hasImagerPixelSpacing(0),
    hasPixelAspectRatio(0),
    isOriginal(0),
    InputData(NULL)
{
    const double xfactor = (double)Columns / (double)image->Columns;
    const double yfactor = (double)Rows / (double)image->Rows;
    /* re-compute pixel width and height */
    if (image->hasPixelSpacing)
    {
        hasPixelSpacing = image->hasPixelSpacing;
        PixelWidth = image->PixelWidth * xfactor;
        PixelHeight = image->PixelHeight * yfactor;
    }
    else if (image->hasImagerPixelSpacing)
    {
        hasImagerPixelSpacing = image->hasImagerPixelSpacing;
        PixelWidth = image->PixelWidth * xfactor;
        PixelHeight = image->PixelHeight * yfactor;
    }
    else if (image->hasPixelAspectRatio && !aspect)
    {
        hasPixelAspectRatio = image->hasPixelAspectRatio;
        PixelWidth = image->PixelWidth * xfactor;
        PixelHeight = image->PixelHeight * yfactor;
        /* do not store pixel aspect ratio for square pixels */
        if (PixelWidth == PixelHeight)
            hasPixelAspectRatio = 0;
    }
}


DiImage::DiImage(const DiImage *image,
                 const int degree)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(image->FirstFrame),
    NumberOfFrames(image->NumberOfFrames),
    RepresentativeFrame(image->RepresentativeFrame),
    Rows(((degree == 90) ||(degree == 270)) ? image->Columns : image->Rows),
    Columns(((degree == 90) ||(degree == 270)) ? image->Rows : image->Columns),
    PixelWidth(((degree == 90) ||(degree == 270)) ? image->PixelHeight : image->PixelWidth),
    PixelHeight(((degree == 90) ||(degree == 270)) ? image-> PixelWidth : image->PixelHeight),
    BitsAllocated(image->BitsAllocated),
    BitsStored(image->BitsStored),
    HighBit(image->HighBit),
    BitsPerSample(image->BitsPerSample),
    Polarity(image->Polarity),
    hasSignedRepresentation(image->hasSignedRepresentation),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL)
{
}


DiImage::DiImage(const DiImage *image,
                 const unsigned long frame,
                 const int stored,
                 const int alloc)
  : ImageStatus(image->ImageStatus),
    Document(image->Document),
    FirstFrame(frame),
    NumberOfFrames(1),
    RepresentativeFrame(0),
    Rows(image->Rows),
    Columns(image->Columns),
    PixelWidth(image->PixelWidth),
    PixelHeight(image->PixelHeight),
    BitsAllocated(alloc),
    BitsStored(stored),
    HighBit(stored - 1),
    BitsPerSample(image->BitsPerSample),
    Polarity(image->Polarity),
    hasSignedRepresentation(0),
    hasPixelSpacing(image->hasPixelSpacing),
    hasImagerPixelSpacing(image->hasImagerPixelSpacing),
    hasPixelAspectRatio(image->hasPixelAspectRatio),
    isOriginal(0),
    InputData(NULL)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiImage::~DiImage()
{
    delete InputData;
}


/********************************************************************/


int DiImage::rotate(const int degree)
{
    if ((degree == 90) || (degree == 270))
    {
        Uint16 us = Rows;                   // swap image width and height
        Rows = Columns;
        Columns = us;
        double db = PixelWidth;             // swap pixel width and height
        PixelWidth = PixelHeight;
        PixelHeight = db;
        return 1;
    }
    return 0;
}


/********************************************************************/


void DiImage::deleteInputData()
{
    delete InputData;
    InputData = NULL;
}


void DiImage::checkPixelExtension()
{
    if (hasPixelSpacing || hasImagerPixelSpacing || hasPixelAspectRatio)
    {
        if (PixelHeight == 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: invalid value for 'PixelHeight' (" << PixelHeight << ") "
                                     << "... assuming 1 !" << endl;
                ofConsole.unlockCerr();
            }
            PixelHeight = 1;
        }
        else if (PixelHeight < 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: negative value for 'PixelHeight' (" << PixelHeight << ") "
                                     << "... assuming " << -PixelHeight << " !" << endl;
                ofConsole.unlockCerr();
            }
            PixelHeight = -PixelHeight;
        }
        if (PixelWidth == 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: invalid value for 'PixelWidth' (" << PixelWidth << ") "
                                     << "... assuming 1 !" << endl;
                ofConsole.unlockCerr();
            }
            PixelWidth = 1;
        }
        else if (PixelWidth < 0)
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: negative value for 'PixelWidth' (" << PixelWidth << ") "
                                     << "... assuming " << -PixelWidth << " !" << endl;
                ofConsole.unlockCerr();
            }
            PixelHeight = -PixelHeight;
        }
    }
}


void DiImage::convertPixelData(/*const*/ DcmPixelData *pixel,
                               const int spp /*samplePerPixel*/)
{
    if ((pixel->getVR() == EVR_OW) || ((pixel->getVR() == EVR_OB) && (BitsAllocated <= 8)))
    {
        const unsigned long start = FirstFrame * (unsigned long)Rows * (unsigned long)Columns * (unsigned long)spp;
        const unsigned long count = NumberOfFrames * (unsigned long)Rows * (unsigned long)Columns * (unsigned long)spp;
        if ((BitsAllocated < 1) || (BitsStored < 1) || (BitsAllocated < BitsStored) ||
            (BitsStored > (Uint16)(HighBit + 1)))
        {
            ImageStatus = EIS_InvalidValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: invalid values for 'BitsAllocated' (" << BitsAllocated << "), "
                                     << "'BitsStored' (" << BitsStored << ") and/or 'HighBit' (" << HighBit << ") !" << endl;
                ofConsole.unlockCerr();
            }
            return;
        }
        else if ((pixel->getVR() == EVR_OB) && (BitsAllocated <= 8))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint8, Sint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData  = new DiInputPixelTemplate<Uint8, Uint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else if (BitsStored <= bitsof(Uint8))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData  = new DiInputPixelTemplate<Uint16, Uint8>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else if (BitsStored <= bitsof(Uint16))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint16>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData = new DiInputPixelTemplate<Uint16, Uint16>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else if (BitsStored <= bitsof(Uint32))
        {
            if (hasSignedRepresentation)
                InputData = new DiInputPixelTemplate<Uint16, Sint32>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
            else
                InputData = new DiInputPixelTemplate<Uint16, Uint32>(pixel, BitsAllocated, BitsStored, HighBit, start, count);
        }
        else    /* BitsStored > 32 !! */
        {
            ImageStatus = EIS_NotSupportedValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: invalid value for 'BitsStored' (" << BitsStored << ") "
                                     << "... exceeds " << MAX_BITS << " bit !" << endl;
                ofConsole.unlockCerr();
            }
            return;
        }
        if (InputData == NULL)
        {
            ImageStatus = EIS_MemoryFailure;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: can't allocate memory for input-representation !" << endl;
                ofConsole.unlockCerr();
            }
        }
        else if (InputData->getPixelStart() >= InputData->getCount())
        {
            ImageStatus = EIS_InvalidValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: start offset (" << InputData->getPixelStart()
                                     << ") exceeds number of pixels stored (" << InputData->getCount() << ") " << endl;
                ofConsole.unlockCerr();
            }
        }
    }
    else
    {
        ImageStatus = EIS_NotSupportedValue;
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: 'PixelData' has an other value representation than OB "
                                 << "(with 'BitsAllocated' <= 8) or OW !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


int DiImage::detachPixelData()
{
    if ((Document != NULL) && (Document->getFlags() & CIF_MayDetachPixelData))
    {
        /* get DICOM dataset */
        DcmDataset *dataset = (DcmDataset *)(Document->getDicomObject());
        if (dataset != NULL)
        {
            /* insert new, empty PixelData element */
            dataset->putAndInsertUint16Array(DCM_PixelData, NULL, 0, OFTrue /*replaceOld*/);
#ifdef DEBUG
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
            {
                ofConsole.lockCerr() << "INFO: detach pixel data" << endl;
                ofConsole.unlockCerr();
            }
#endif
            return 1;
        }
    }
    return 0;
}


int DiImage::setColumnRowRatio(const double ratio)
{
    hasPixelAspectRatio = 1;
    hasPixelSpacing = hasImagerPixelSpacing = 0;
    PixelWidth = ratio;
    PixelHeight = 1;
    checkPixelExtension();
    return 1;
}


int DiImage::setRowColumnRatio(const double ratio)
{
    hasPixelAspectRatio = 1;
    hasPixelSpacing = hasImagerPixelSpacing = 0;
    PixelWidth = 1;
    PixelHeight = ratio;
    checkPixelExtension();
    return 1;
}


int DiImage::setPolarity(const EP_Polarity polarity)
{
    if (polarity != Polarity)
    {
        Polarity = polarity;
        return 1;
    }
    return 2;
}


void DiImage::updateImagePixelModuleAttributes(DcmItem &dataset)
{
    /* remove outdated attributes from the dataset */
    delete dataset.remove(DCM_SmallestImagePixelValue);
    delete dataset.remove(DCM_LargestImagePixelValue);
/*
    delete dataset.remove(DCM_PixelPaddingValue);
    delete dataset.remove(DCM_SmallestPixelValueInSeries);
    delete dataset.remove(DCM_LargestPixelValueInSeries);
*/
    /* update PixelAspectRatio & Co. */
    char buffer[32];
    OFStandard::ftoa(buffer, 15, PixelHeight, OFStandard::ftoa_format_f);
    strcat(buffer, "\\");
    OFStandard::ftoa(strchr(buffer, 0), 15, PixelWidth, OFStandard::ftoa_format_f);

    if (hasPixelSpacing)
    {
        dataset.putAndInsertString(DCM_PixelSpacing, buffer);
        dataset.putAndInsertString(DCM_PixelSpacing, buffer);
    } else
        delete dataset.remove(DCM_PixelSpacing);
    if (hasImagerPixelSpacing)
    {
        dataset.putAndInsertString(DCM_ImagerPixelSpacing, buffer);
        dataset.putAndInsertString(DCM_ImagerPixelSpacing, buffer);
    } else
        delete dataset.remove(DCM_ImagerPixelSpacing);
    if (hasPixelAspectRatio)
    {
        dataset.putAndInsertString(DCM_PixelAspectRatio, buffer);
        dataset.putAndInsertString(DCM_PixelAspectRatio, buffer);
    } else
        delete dataset.remove(DCM_PixelAspectRatio);
}


// --- write given frame of the current image to DICOM dataset

int DiImage::writeFrameToDataset(DcmItem &dataset,
                                 const unsigned long frame,
                                 const int bits,
                                 const int planar)
{
    int result = 0;
    const int bitsStored = getBits(bits);
    /* get output pixel data */
    const void *pixel = getOutputData(frame, bitsStored, planar);
    if (pixel != NULL)
    {
        char buffer[32];
        unsigned long count;
        /* write color model dependent attributes */
        if ((getInternalColorModel() == EPI_Monochrome1) || (getInternalColorModel() == EPI_Monochrome2))
        {
            /* monochrome image */
            count = (unsigned long)Columns * (unsigned long)Rows;
            dataset.putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME2");
            dataset.putAndInsertUint16(DCM_SamplesPerPixel, 1);
        } else {
            /* color image */
            count = (unsigned long)Columns * (unsigned long)Rows * 3 /*samples per pixel*/;
            if (getInternalColorModel() == EPI_YBR_Full)
                dataset.putAndInsertString(DCM_PhotometricInterpretation, "YBR_FULL");
            else
                dataset.putAndInsertString(DCM_PhotometricInterpretation, "RGB");
            dataset.putAndInsertUint16(DCM_PlanarConfiguration, planar != 0);
            dataset.putAndInsertUint16(DCM_SamplesPerPixel, 3);
        }
        /* write remaining attributes */
        dataset.putAndInsertUint16(DCM_Columns, Columns);
        dataset.putAndInsertUint16(DCM_Rows, Rows);
        dataset.putAndInsertString(DCM_NumberOfFrames, "1");
        if (bitsStored <= 8)
            dataset.putAndInsertUint16(DCM_BitsAllocated, 8);
        else if (bitsStored <= 16)
            dataset.putAndInsertUint16(DCM_BitsAllocated, 16);
        else
            dataset.putAndInsertUint16(DCM_BitsAllocated, 32);
        dataset.putAndInsertUint16(DCM_BitsStored, bitsStored);
        dataset.putAndInsertUint16(DCM_HighBit, bitsStored - 1);
        dataset.putAndInsertUint16(DCM_PixelRepresentation, 0);
        /* handle VOI transformations */
        if (dataset.tagExists(DCM_WindowCenter) ||
            dataset.tagExists(DCM_WindowWidth) ||
            dataset.tagExists(DCM_VOILUTSequence))
        {
            delete dataset.remove(DCM_VOILUTSequence);
            sprintf(buffer, "%lu", DicomImageClass::maxval(bitsStored, 0) / 2);
            dataset.putAndInsertString(DCM_WindowCenter, buffer);
            sprintf(buffer, "%lu", DicomImageClass::maxval(bitsStored, 0));
            dataset.putAndInsertString(DCM_WindowWidth, buffer);
        }
        delete dataset.remove(DCM_WindowCenterWidthExplanation);
        /* write pixel data (OB or OW) */
        if (bitsStored <= 8)
            dataset.putAndInsertUint8Array(DCM_PixelData, (Uint8 *)pixel, count);
        else
            dataset.putAndInsertUint16Array(DCM_PixelData, (Uint16 *)pixel, count);
        /* update other DICOM attributes */
        updateImagePixelModuleAttributes(dataset);
        result = 1;
    }
    return result;
}


int DiImage::writeBMP(FILE *stream,
                      const unsigned long frame,
                      const int bits)
{
    int result = 0;
    if ((stream != NULL) && ((bits == 8) || (bits == 24)))
    {
        /* create device independent bitmap: palette (8) or truecolor (24) */
        void *data = NULL;
        const unsigned long bytes = createDIB(data, 0, frame, bits, 1 /*upsideDown*/);
        if ((data != NULL) && (bytes > 0))
        {
            /* number of bytes */
            SB_BitmapFileHeader fileHeader;
            SB_BitmapInfoHeader infoHeader;
            Uint32 *palette = (bits == 8) ? new Uint32[256] : (Uint32 *)NULL;
            /* fill bitmap file header with data */
            fileHeader.bfType[0] = 'B';
            fileHeader.bfType[1] = 'M';
            fileHeader.bfSize = 14 /*sizeof(SB_BitmapFileHeader)*/ + 40 /*sizeof(SB_BitmapInfoHeader)*/ + bytes;
            fileHeader.bfReserved1 = 0;
            fileHeader.bfReserved2 = 0;
            fileHeader.bfOffBits = 14 /*sizeof(SB_BitmapFileHeader)*/ + 40 /*sizeof(SB_BitmapInfoHeader)*/;
            /* fill bitmap info header with data */
            infoHeader.biSize = 40 /*sizeof(SB_BitmapInfoHeader)*/;
            infoHeader.biWidth = Columns;
            infoHeader.biHeight = Rows;
            infoHeader.biPlanes = 1;
            infoHeader.biBitCount = bits;
            infoHeader.biCompression = 0;
            infoHeader.biSizeImage = 0;
            infoHeader.biXPelsPerMeter = 0;
            infoHeader.biYPelsPerMeter = 0;
            infoHeader.biClrUsed = 0;
            infoHeader.biClrImportant = 0;
            /* create and fill color palette */
            if (palette != NULL)
            {
                /* add palette size */
                fileHeader.bfSize += 256 * 4;
                fileHeader.bfOffBits += 256 * 4;
                /* fill palette entries with gray values, format: B-G-R-0 */
                for (Uint32 i = 0; i < 256; i++)
                    palette[i] = (i << 16) | (i << 8) | i;
            }
            /* swap bytes if necessary */
            if (gLocalByteOrder != EBO_LittleEndian)
            {
                /* other data elements are always '0' and, therefore, can remain as they are */
                swap4Bytes((Uint8 *)&fileHeader.bfSize);
                swap4Bytes((Uint8 *)&fileHeader.bfOffBits);
                swap4Bytes((Uint8 *)&infoHeader.biSize);
                swap4Bytes((Uint8 *)&infoHeader.biWidth);
                swap4Bytes((Uint8 *)&infoHeader.biHeight);
                swap2Bytes((Uint8 *)&infoHeader.biPlanes);
                swap2Bytes((Uint8 *)&infoHeader.biBitCount);
                if (palette != NULL)
                    swapBytes((Uint8 *)palette, 256 * 4 /*byteLength*/, 4 /*valWidth*/);
            }
            /* write bitmap file header: do not write the struct because of 32-bit alignment */
            fwrite(&fileHeader.bfType, sizeof(fileHeader.bfType), 1, stream);
            fwrite(&fileHeader.bfSize, sizeof(fileHeader.bfSize), 1, stream);
            fwrite(&fileHeader.bfReserved1, sizeof(fileHeader.bfReserved1), 1, stream);
            fwrite(&fileHeader.bfReserved2, sizeof(fileHeader.bfReserved2), 1, stream);
            fwrite(&fileHeader.bfOffBits, sizeof(fileHeader.bfOffBits), 1, stream);
            /* write bitmap info header: do not write the struct because of 32-bit alignment  */
            fwrite(&infoHeader.biSize, sizeof(infoHeader.biSize), 1, stream);
            fwrite(&infoHeader.biWidth, sizeof(infoHeader.biWidth), 1, stream);
            fwrite(&infoHeader.biHeight, sizeof(infoHeader.biHeight), 1, stream);
            fwrite(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes), 1, stream);
            fwrite(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount), 1, stream);
            fwrite(&infoHeader.biCompression, sizeof(infoHeader.biCompression), 1, stream);
            fwrite(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage), 1, stream);
            fwrite(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter), 1, stream);
            fwrite(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter), 1, stream);
            fwrite(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed), 1, stream);
            fwrite(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant), 1, stream);
            /* write color palette (if applicable) */
            if (palette != NULL)
                fwrite(palette, 4, 256, stream);
            /* write pixel data */
            fwrite(data, 1, (size_t)bytes, stream);
            /* delete color palette */
            delete[] palette;
            result = 1;
        }
        /* delete pixel data */
        delete (char *)data;                 // type cast necessary to avoid compiler warnings using gcc >2.95
    }
    return result;
}
