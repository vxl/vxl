/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Purpose: DicomMonochromeModality (Source)
 *
 */


#include "osconfig.h"
#include "dcdeftag.h"
#include "dcuid.h"

#include "dimomod.h"
#include "didocu.h"
#include "diinpx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(0),
    AbsMinimum(0),
    AbsMaximum(0),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        if (!(docu->getFlags() & CIF_UsePresentationState))             // ignore modality LUT and rescaling
        {
            const char *sopClassUID = NULL;                             // check for XA and XRF image (ignore MLUT)
            if ((docu->getValue(DCM_SOPClassUID, sopClassUID) == 0) || (sopClassUID == NULL) ||
               ((strcmp(sopClassUID, UID_XRayAngiographicImageStorage) != 0) &&
                (strcmp(sopClassUID, UID_XRayFluoroscopyImageStorage) != 0)))
            {
                TableData = new DiLookupTable(docu, DCM_ModalityLUTSequence, DCM_LUTDescriptor, DCM_LUTData, DCM_LUTExplanation);
                checkTable();
                Rescaling = (docu->getValue(DCM_RescaleIntercept, RescaleIntercept) > 0);
                Rescaling &= (docu->getValue(DCM_RescaleSlope, RescaleSlope) > 0);
                checkRescaling(pixel);
            } else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
                {
                    ofConsole.lockCerr() << "INFO: processing XA or XRF image ... ignoring possible modality transform !" << endl;
                    ofConsole.unlockCerr();
                }
            }
        }
        Representation = DicomImageClass::determineRepresentation(MinValue, MaxValue);
    }
}


DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel,
                               const double slope,
                               const double intercept)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(0),
    AbsMinimum(0),
    AbsMaximum(0),
    RescaleIntercept(intercept),
    RescaleSlope(slope),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        Rescaling = 1;
        checkRescaling(pixel);
        Representation = DicomImageClass::determineRepresentation(MinValue, MaxValue);
    }
}


DiMonoModality::DiMonoModality(const DiDocument *docu,
                               DiInputPixel *pixel,
                               const DcmUnsignedShort &data,
                               const DcmUnsignedShort &descriptor,
                               const DcmLongString *explanation)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(0),
    AbsMinimum(0),
    AbsMaximum(0),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
    if (Init(docu, pixel))
    {
        TableData = new DiLookupTable(data, descriptor, explanation);
        checkTable();
        Representation = DicomImageClass::determineRepresentation(MinValue, MaxValue);
    }
}


DiMonoModality::DiMonoModality(const int bits)
  : Representation(EPR_MaxSigned),
    MinValue(0),
    MaxValue(0),
    Bits(bits),
    AbsMinimum(0),
    AbsMaximum(DicomImageClass::maxval(bits)),
    RescaleIntercept(0),
    RescaleSlope(0),
    LookupTable(0),
    Rescaling(0),
    TableData(NULL)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoModality::~DiMonoModality()
{
    delete TableData;
}


/*********************************************************************/


int DiMonoModality::Init(const DiDocument *docu,
                         DiInputPixel *pixel)
{
    if ((docu != NULL) && (pixel != NULL))
    {
        pixel->determineMinMax();
        MinValue = pixel->getMinValue(1 /* selected range of pixels only */);
        MaxValue = pixel->getMaxValue(1 /* selected range of pixels only */);
        Bits = pixel->getBits();
        AbsMinimum = pixel->getAbsMinimum();
        AbsMaximum = pixel->getAbsMaximum();
        Uint16 us;
        if (docu->getValue(DCM_SamplesPerPixel, us) && (us != 1))
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: invalid value for 'SamplesPerPixel' (" << us << ") ... assuming 1 !" << endl;
                ofConsole.unlockCerr();
            }
        }
        return 1;
    }
    return 0;
}


void DiMonoModality::checkTable()
{
    if (TableData != NULL)
    {
        LookupTable = TableData->isValid();
        if (LookupTable)
        {
            MinValue = TableData->getMinValue();
            MaxValue = TableData->getMaxValue();
            Bits = TableData->getBits();
            AbsMinimum = 0;
            AbsMaximum = DicomImageClass::maxval(Bits);
        }
    }
}


void DiMonoModality::checkRescaling(const DiInputPixel *pixel)
{
    if (Rescaling)
    {
        if (LookupTable) {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: redundant values for 'RescaleSlope/Intercept'"
                                     << " ... using modality LUT transformation !" << endl;
                ofConsole.unlockCerr();
            }
            Rescaling = 0;
        } else {
            if (RescaleSlope == 0)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'RescaleSlope' (" << RescaleSlope
                                         << ") ... ignoring modality transformation !" << endl;
                    ofConsole.unlockCerr();
                }
                Rescaling = 0;
            }
            else
            {
                if (RescaleSlope < 0)                                       // negative slope value
                {
                    const double temp = MinValue;
                    MinValue = MaxValue * RescaleSlope + RescaleIntercept;
                    MaxValue = temp * RescaleSlope + RescaleIntercept;
                    AbsMinimum = pixel->getAbsMaximum() * RescaleSlope + RescaleIntercept;
                    AbsMaximum = pixel->getAbsMinimum() * RescaleSlope + RescaleIntercept;
                } else {                                                    // positive slope value
                    MinValue = MinValue * RescaleSlope + RescaleIntercept;
                    MaxValue = MaxValue * RescaleSlope + RescaleIntercept;
                    AbsMinimum = pixel->getAbsMinimum() * RescaleSlope + RescaleIntercept;
                    AbsMaximum = pixel->getAbsMaximum() * RescaleSlope + RescaleIntercept;
                }
                Bits = DicomImageClass::tobits((unsigned long)(AbsMaximum - AbsMinimum), 0);
            }
        }
    }
}
