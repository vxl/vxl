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
 *  Purpose: DicomOverlayPlane (Source) - Multiframe Overlays UNTESTED !
 *
 */


#include "osconfig.h"
#include "ofconsol.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dctagkey.h"
#include "ofbmanip.h"

#include "diovpln.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlayPlane::DiOverlayPlane(const DiDocument *docu,
                               const unsigned int group,
                               Uint16 alloc)
  : NumberOfFrames(0),
    ImageFrameOrigin(0),
    Top(0),
    Left(0),
    Height(0),
    Width(0),
    Rows(0),
    Columns(0),
    BitsAllocated(0),
    BitPosition(0),
    Foreground(1),
    Threshold(1),
    PValue(0),
    Mode(EMO_Graphic),
    DefaultMode(EMO_Graphic),
    Label(),
    Description(),
    GroupNumber(group),
    Valid(0),
    Visible(0),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    if (docu != NULL)
    {
        /* specifiy overlay group number */
        DcmTagKey tag(group, DCM_OverlayRows.getElement() /* dummy */);
        /* get descriptive data */
        tag.setElement(DCM_OverlayLabel.getElement());
        docu->getValue(tag, Label);
        tag.setElement(DCM_OverlayDescription.getElement());
        docu->getValue(tag, Description);
        /* get overlay type */
        tag.setElement(DCM_OverlayType.getElement());
        const char *str;
        if ((docu->getValue(tag, str) > 0) && (strcmp(str, "R") == 0))
            DefaultMode = Mode = EMO_RegionOfInterest;
        Sint32 sl = 0;
        /* multi-frame overlays */
        tag.setElement(DCM_NumberOfFramesInOverlay.getElement());
        docu->getValue(tag, sl);
        NumberOfFrames = (sl < 1) ? 1 : (Uint32)sl;
        tag.setElement(DCM_ImageFrameOrigin.getElement());
        docu->getValue(tag, ImageFrameOrigin);
        if (ImageFrameOrigin > 0)                                   // image frame origin is numbered from 1
            ImageFrameOrigin--;
        tag.setElement(DCM_OverlayOrigin.getElement());
#ifdef REVERSE_OVERLAY_ORIGIN_ORDER
        Valid = (docu->getValue(tag, Left, 0) > 0);
        if (Valid)
        {
            if (docu->getValue(tag, Top, 1) < 2)
            {
                ofConsole.lockCerr() << "WARNING: missing second value for 'OverlayOrigin' ... "
                                     << "assuming 'Top' = " << Top << " !" << endl;
                ofConsole.unlockCerr();
            }
        }
#else
        Valid = (docu->getValue(tag, Top, 0) > 0);
        if (Valid)
        {
            if (docu->getValue(tag, Left, 1) < 2)
            {
                ofConsole.lockCerr() << "WARNING: missing second value for 'OverlayOrigin' ... "
                                     << "assuming 'Left' = " << Left << " !" << endl;
                ofConsole.unlockCerr();
            }
        }
#endif
        /* overlay origin is numbered from 1 */
        Top--;
        Left--;
        /* check overlay resolution */
        tag.setElement(DCM_OverlayRows.getElement());
        Valid &= (docu->getValue(tag, Rows) > 0);
        Height = Rows;
        tag.setElement(DCM_OverlayColumns.getElement());
        Valid &= (docu->getValue(tag, Columns) > 0);
        Width = Columns;
        /* check overlay encoding */
        tag.setElement(DCM_OverlayBitsAllocated.getElement());
        Valid &= (docu->getValue(tag, BitsAllocated) > 0);
        tag.setElement(DCM_OverlayBitPosition.getElement());
        Valid &= (docu->getValue(tag, BitPosition) > 0);
        tag.setElement(DCM_OverlayData.getElement());
        /* final validity checks */
        if (Valid)
        {
            unsigned long length = docu->getValue(tag, Data) * 2 /* bytes */;
            if (length == 0)
            {
                ImageFrameOrigin = 0;                               // see supplement 4
                length = docu->getValue(DCM_PixelData, Data) * 2 /* bytes */;
                EmbeddedData = (Data != NULL);
            } else
                alloc = 1;                                          // separately stored overlay data
            /* check for correct value of BitsAllocated */
            if (BitsAllocated != alloc)                             // see correction proposal 87
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'OverlayBitsAllocated' (" << BitsAllocated
                                         << ") ... assuming " << alloc << " !" << endl;
                    ofConsole.unlockCerr();
                }
                BitsAllocated = alloc;
            }
            /* check for correct value of BitPosition */
            if (BitPosition > BitsAllocated)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'OverlayBitPosition' (" << BitPosition
                                         << ") ... assuming " << (BitsAllocated - 1) << " !" << endl;
                    ofConsole.unlockCerr();
                }
                BitPosition = BitsAllocated - 1;
            }
            /* expected length of overlay data */
            const unsigned long expLen = ((unsigned long)NumberOfFrames * (unsigned long)Rows * (unsigned long)Columns *
                                          (unsigned long)BitsAllocated + 7) / 8;
            if ((length == 0) || (length < expLen))
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                {
                    ofConsole.lockCerr() << "ERROR: overlay data length is too short !" << endl;
                    ofConsole.unlockCerr();
                }
                Valid = 0;
                Data = NULL;
            } else
                Valid = (Data != NULL);
        }
    }
}


DiOverlayPlane::DiOverlayPlane(const unsigned int group,
                               const Sint16 left_pos,
                               const Sint16 top_pos,
                               const Uint16 columns,
                               const Uint16 rows,
                               const DcmOverlayData &data,
                               const DcmLongString &label,
                               const DcmLongString &description,
                               const EM_Overlay mode)
  : NumberOfFrames(1),
    ImageFrameOrigin(0),
    Top(top_pos),
    Left(left_pos),
    Height(rows),
    Width(columns),
    Rows(rows),
    Columns(columns),
    BitsAllocated(1),
    BitPosition(0),
    Foreground(1),
    Threshold(1),
    PValue(0),
    Mode(mode),
    DefaultMode(mode),
    Label(),
    Description(),
    GroupNumber(group),
    Valid(0),
    Visible((mode == EMO_BitmapShutter) ? 1 : 0),
    BitPos(0),
    StartBitPos(0),
    StartLeft(0),
    StartTop(0),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    DiDocument::getElemValue((const DcmElement *)&label, Label);
    DiDocument::getElemValue((const DcmElement *)&description, Description);
    if ((Columns > 0) && (Rows > 0))
    {
        const unsigned long length = DiDocument::getElemValue((const DcmElement *)&data, Data) * 2 /* Bytes */;
        /* expected length of overlay data */
        const unsigned long expLen = ((unsigned long)Rows * (unsigned long)Columns + 7) / 8;
        if ((length == 0) || (length < expLen))
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: overlay data length is too short !" << endl;
                ofConsole.unlockCerr();
            }
            /* Valid = 0;  =>  This is the default. */
            Data = NULL;
        } else
            Valid = (Data != NULL);
    }
    Top--;                                                      // overlay origin is numbered from 1
    Left--;
}


DiOverlayPlane::DiOverlayPlane(DiOverlayPlane *plane,
                               const unsigned int bit,
                               Uint16 *data,
                               Uint16 *temp,
                               const Uint16 width,
                               const Uint16 height,
                               const Uint16 columns,
                               const Uint16 rows)
  : NumberOfFrames(plane->NumberOfFrames),
    ImageFrameOrigin(plane->ImageFrameOrigin),
    Top(plane->Top),
    Left(plane->Left),
    Height(plane->Height),
    Width(plane->Width),
    Rows(rows),
    Columns(columns),
    BitsAllocated(16),
    BitPosition(bit),
    Foreground(plane->Foreground),
    Threshold(plane->Threshold),
    PValue(0),
    Mode(plane->Mode),
    DefaultMode(plane->DefaultMode),
    Label(plane->Label),
    Description(plane->Description),
    GroupNumber(plane->GroupNumber),
    Valid(0),
    Visible(plane->Visible),
    BitPos(0),
    StartBitPos(0),
    StartLeft(plane->StartLeft),
    StartTop(plane->StartTop),
    EmbeddedData(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(data)
{
    if (temp != NULL)
    {
        register Uint16 x;
        register Uint16 y;
        register Uint16 *q = temp;
        register const Uint16 mask = 1 << bit;
        const Uint16 skip_x = width - plane->Columns;
        const unsigned long skip_f = (unsigned long)(height - plane->Rows) * (unsigned long)width;
        for (unsigned long f = 0; f < NumberOfFrames; f++)
        {
            if (plane->reset(f + ImageFrameOrigin))
            {
                for (y = 0; y < plane->Rows; y++)
                {
                    for (x = 0; x < plane->Columns; x++, q++)
                    {
                        if (plane->getNextBit())
                            *q |= mask;                         // set corresponding bit
                        else
                            *q &= ~mask;                        // unset ... bit
                    }
                    q += skip_x;                                // skip to next line start
                }
                q += skip_f;                                    // skip to next frame start
            }
        }
    }
    Valid = (Data != NULL);
}


/*--------------*
 *  destructor  *
 *--------------*/

DiOverlayPlane::~DiOverlayPlane()
{
}


/********************************************************************/


void *DiOverlayPlane::getData(const unsigned long frame,
                              const Uint16 xmin,
                              const Uint16 ymin,
                              const Uint16 xmax,
                              const Uint16 ymax,
                              const int bits,
                              const Uint16 fore,
                              const Uint16 back)
{
    const unsigned long count = (unsigned long)(xmax - xmin) * (unsigned long)(ymax - ymin);
    if (Valid && (count > 0))
    {
        const Uint16 mask = (Uint16)DicomImageClass::maxval(bits);
        if (bits == 1)
        {
            const unsigned long count8 = (count + 7) / 8;           // round value: 8 bit padding
            Uint8 *data = new Uint8[count8];
            if (data != NULL)
            {
                if ((fore & mask) != (back & mask))
                {
                    OFBitmanipTemplate<Uint8>::setMem(data, 0x0, count8);
                    register Uint16 x;
                    register Uint16 y;
                    register Uint8 value = 0;
                    register Uint8 *q = data;
                    register int bit = 0;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; y++)
                        {
                            setStart(xmin, y);
                            for (x = xmin; x < xmax; x++)
                            {
                                if (getNextBit())
                                {
                                    if (fore)
                                        value |= (1 << bit);
                                } else if (back)
                                    value |= (1 << bit);
                                if (bit == 7)
                                {
                                    *(q++) = value;
                                    value = 0;
                                    bit = 0;
                                } else {
                                    bit++;
                                }
                            }
                        }
                        if (bit != 0)
                            *(q++) = value;
                    }
                } else {
                    OFBitmanipTemplate<Uint8>::setMem(data, (fore) ? 0xff : 0x0, count8);
                }
            }
            return (void *)data;
        }
        else if ((bits > 1) && (bits <= 8))
        {
            Uint8 *data = new Uint8[count];
            if (data != NULL)
            {
                const Uint8 fore8 = (Uint8)(fore & mask);
                const Uint8 back8 = (Uint8)(back & mask);
                OFBitmanipTemplate<Uint8>::setMem(data, back8, count);
                if (fore8 != back8)                                     // optimization
                {
                    register Uint16 x;
                    register Uint16 y;
                    register Uint8 *q = data;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; y++)
                        {
                            setStart(xmin, y);
                            for (x = xmin; x < xmax; x++, q++)
                            {
                                if (getNextBit())
                                    *q = fore8;                         // set pixel value (default: 0xff)
                            }
                        }
                    }
                }
            }
            return (void *)data;
        }
        else if ((bits > 8) && (bits <= 16))
        {
            Uint16 *data = new Uint16[count];
            if (data != NULL)
            {
                const Uint16 fore16 = fore & mask;
                const Uint16 back16 = back & mask;
                OFBitmanipTemplate<Uint16>::setMem(data, back16, count);
                if (fore16 != back16)                                   // optimization
                {
                    register Uint16 x;
                    register Uint16 y;
                    register Uint16 *q = data;
                    if (reset(frame + ImageFrameOrigin))
                    {
                        for (y = ymin; y < ymax; y++)
                        {
                            setStart(xmin, y);
                            for (x = xmin; x < xmax; x++, q++)
                            {
                                if (getNextBit())
                                    *q = fore16;                        // set pixel value (default: 0xff)
                            }
                        }
                    }
                }
            }
            return (void *)data;
        }
    }
    return NULL;
}


unsigned long DiOverlayPlane::create6xxx3000Data(Uint8 *&buffer,
                                                 unsigned int &width,
                                                 unsigned int &height,
                                                 unsigned long &frames)
{
    buffer = NULL;
    width = Width;
    height = Height;
    frames = NumberOfFrames;
    const unsigned long count = (unsigned long)Width * (unsigned long)Height * NumberOfFrames;
    if (Valid && (count > 0))
    {
        const unsigned long count8 = ((count + 15) / 16) * 2;           // round value: 16 bit padding
        buffer = new Uint8[count8];
        if (buffer != NULL)
        {
            OFBitmanipTemplate<Uint8>::setMem(buffer, 0x0, count8);
            register Uint16 x;
            register Uint16 y;
            register Uint8 value = 0;
            register Uint8 *q = buffer;
            register int bit = 0;
            for (unsigned long f = 0; f < NumberOfFrames; f++)
            {
                if (reset(f + ImageFrameOrigin))
                {
                    for (y = 0; y < Height; y++)
                    {
                        for (x = 0; x < Width; x++)
                        {
                            if (getNextBit())
                                value |= (1 << bit);
                            if (bit == 7)
                            {
                                *(q++) = value;
                                value = 0;
                                bit = 0;
                            } else {
                                bit++;
                            }
                        }
                    }
                }
                if (bit != 0)
                    *(q++) = value;
            }
            return count8;      // number of bytes
        }
    }
    return 0;
}


void DiOverlayPlane::show(const double fore,
                          const double thresh,
                          const EM_Overlay mode)
{
    Foreground = (fore < 0) ? 0 : (fore > 1) ? 1 : fore;
    Threshold = (thresh < 0) ? 0 : (thresh > 1) ? 1 : thresh;
    Mode = (mode == EMO_Default) ? DefaultMode : mode;
    Visible = 1;
}


int DiOverlayPlane::show(const Uint16 pvalue)
{
    if (Mode == EMO_BitmapShutter)
    {
        PValue = pvalue;
        Visible = 1;
        return 1;
    }
    return 0;
}


void DiOverlayPlane::setScaling(const double xfactor,
                                const double yfactor)
{
    Left = (Sint16)(xfactor * Left);
    Top = (Sint16)(yfactor * Top);
    Width = (Uint16)(xfactor * Width);
    Height = (Uint16)(yfactor * Height);
}


void DiOverlayPlane::setFlipping(const int horz,
                                 const int vert,
                                 const signed long columns,
                                 const signed long rows)
{
    if (horz)
    {
        Left = (Sint16)(columns - Width - Left);
        StartLeft = (Uint16)((signed long)Columns - Width - StartLeft);
    }
    if (vert)
    {
        Top = (Sint16)(rows - Height - Top);
        StartTop = (Uint16)((signed long)Rows - Height - StartTop);
    }
}


void DiOverlayPlane::setRotation(const int degree,
                                 const signed long left_pos,
                                 const signed long top_pos,
                                 const Uint16 columns,
                                 const Uint16 rows)
{
    if (degree == 180)                          // equal to v/h flip
        setFlipping(1, 1, left_pos + columns, top_pos + rows);
    else if ((degree == 90) || (degree == 270))
    {
        Uint16 us = Height;                     // swap visible width/height
        Height = Width;
        Width = us;
/*
        us = Rows;                              // swap stored width/height -> already done in the constructor !
        Rows = Columns;
        Columns = us;
*/
        if (degree == 90)                       // rotate right
        {
            Sint16 ss = Left;
            us = StartLeft;
            Left = (Sint16)((signed long)columns - Width - Top + top_pos);
            StartLeft = (Uint16)((signed long)Columns - Width - StartTop);
            Top = (Sint16)(ss - left_pos);
            StartTop = us;
        } else {                                // rotate left
            Sint16 ss = Left;
            us = StartLeft;
            Left = (Sint16)(Top - top_pos);
            StartLeft = StartTop;
            Top = (Sint16)((signed long)rows - Height - ss + left_pos);
            StartTop = (Uint16)((signed long)Rows - Height - us);
        }
    }
}
