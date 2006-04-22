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
 *  Purpose: DicomLookupTable (Source)
 *
 */


#include "osconfig.h"
#include "dcdeftag.h"
#include "dcsequen.h"
#include "dcitem.h"
#include "ofbmanip.h"

#include "diluptab.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiLookupTable::DiLookupTable(const DiDocument *docu,
                             const DcmTagKey &descriptor,
                             const DcmTagKey &data,
                             const DcmTagKey &explanation,
                             EI_Status *status)
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
{
    if (docu != NULL)
        Init(docu, NULL, descriptor, data, explanation, status);
}


DiLookupTable::DiLookupTable(const DiDocument *docu,
                             const DcmTagKey &sequence,
                             const DcmTagKey &descriptor,
                             const DcmTagKey &data,
                             const DcmTagKey &explanation,
                             const unsigned long pos,
                             unsigned long *card)
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
{
    if (docu != NULL)
    {
        DcmSequenceOfItems *seq = NULL;
        const unsigned long count = docu->getSequence(sequence, seq);
        /* store number of items in the option return variable */
        if (card != NULL)
            *card = count;
        if ((seq != NULL) && (pos < count))
        {
            DcmItem *item = seq->getItem(pos);
            Init(docu, item, descriptor, data, explanation);
        }
    }
}


DiLookupTable::DiLookupTable(const DcmUnsignedShort &data,
                             const DcmUnsignedShort &descriptor,
                             const DcmLongString *explanation,
                             const signed long first,
                             EI_Status *status)
  : DiBaseLUT(),
    OriginalBitsAllocated(16),
    OriginalData(NULL)
{
    Uint16 us = 0;
    if (DiDocument::getElemValue((const DcmElement *)&descriptor, us, 0) >= 3)           // number of LUT entries
    {
        Count = (us == 0) ? MAX_TABLE_ENTRY_COUNT : us;                                  // see DICOM supplement 5: "0" => 65536
        DiDocument::getElemValue((const DcmElement *)&descriptor, FirstEntry, 1);        // can be SS or US (will be type casted later)
        if ((first >= 0) && (FirstEntry != (Uint16)first))
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: invalid value for 'First input value mapped' (" << FirstEntry
                                     << ") ... assuming " << first << " !" << endl;
                ofConsole.unlockCerr();
            }
            FirstEntry = (Uint16)first;
        }
        DiDocument::getElemValue((const DcmElement *)&descriptor, us, 2);                // bits per entry (only informational)
        unsigned long count = DiDocument::getElemValue((const DcmElement *)&data, Data);
        OriginalData = (void *)Data;                                                     // store pointer to original data
        if (explanation != NULL)
            DiDocument::getElemValue((const DcmElement *)explanation, Explanation);      // explanation (free form text)
        checkTable(count, us, status);
     } else {
        if (status != NULL)
        {
            *status = EIS_MissingAttribute;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: incomplete or missing 'LookupTableDescriptor' !" << endl;
                ofConsole.unlockCerr();
            }
        } else {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: incomplete or missing  'LookupTableDescriptor' ... ignoring LUT !" << endl;
                ofConsole.unlockCerr();
            }
        }
     }
}


DiLookupTable::DiLookupTable(Uint16 *buffer,
                             const Uint32 count,
                             const Uint16 bits)
  : DiBaseLUT(buffer, count, bits),
    OriginalBitsAllocated(16),
    OriginalData(buffer)
{
    checkTable(count, bits);
}


/*--------------*
 *  destructor  *
 *--------------*/

DiLookupTable::~DiLookupTable()
{
}


/********************************************************************/


void DiLookupTable::Init(const DiDocument *docu,
                         DcmObject *obj,
                         const DcmTagKey &descriptor,
                         const DcmTagKey &data,
                         const DcmTagKey &explanation,
                         EI_Status *status)
{
    Uint16 us = 0;
    if (docu->getValue(descriptor, us, 0, obj) >= 3)                         // number of LUT entries
    {
        Count = (us == 0) ? MAX_TABLE_ENTRY_COUNT : us;                      // see DICOM supplement 5: "0" => 65536
        docu->getValue(descriptor, FirstEntry, 1, obj);                      // can be SS or US (will be type casted later)
        docu->getValue(descriptor, us, 2, obj);                              // bits per entry (only informational)
        unsigned long count = docu->getValue(data, Data, obj);
        OriginalData = (void *)Data;                                         // store pointer to original data
        if (explanation != DcmTagKey(0, 0))
            docu->getValue(explanation, Explanation, 0 /*vm pos*/, obj);     // explanation (free form text)
        checkTable(count, us, status);
    } else {
        if (status != NULL)
        {
            *status = EIS_MissingAttribute;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: incomplete or missing 'LookupTableDescriptor' !" << endl;
                ofConsole.unlockCerr();
            }
        } else {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: incomplete or missing  'LookupTableDescriptor' ... ignoring LUT !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
}


void DiLookupTable::checkTable(unsigned long count,
                               Uint16 bits,
                               EI_Status *status)
{
    if (count > 0)                                                            // valid LUT
    {
        register unsigned long i;
        if (count > MAX_TABLE_ENTRY_COUNT)                                    // cut LUT length to maximum
            count = MAX_TABLE_ENTRY_COUNT;
        if (count != Count)                                                   // length of LUT differs from number of LUT entries
        {
            if (count == ((Count + 1) >> 1))                                  // bits allocated 8, ignore padding
            {
                OriginalBitsAllocated = 8;
#ifdef DEBUG
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
                {
                    ofConsole.lockCerr() << "INFO: lookup table uses 8 bits allocated ... converting to 16 bits." << endl;
                    ofConsole.unlockCerr();
                }
#endif
                DataBuffer = new Uint16[Count];                               // create new LUT
                if ((DataBuffer != NULL) && (Data != NULL))
                {
                    register const Uint8 *p = (const Uint8 *)Data;
                    register Uint16 *q = DataBuffer;
                    if (gLocalByteOrder == EBO_BigEndian)                     // local machine has big endian byte ordering
                    {
#ifdef DEBUG
                        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
                        {
                            ofConsole.lockCerr() << "INFO: local machine has big endian byte ordering"
                                                 << " ... swapping 8 bit LUT entries." << endl;
                            ofConsole.unlockCerr();
                        }
#endif
                        for (i = count; i != 0; i--)                          // copy 8 bit entries to new 16 bit LUT (swap hi/lo byte)
                        {
                            *(q++) = *(p + 1);                                // copy low byte ...
                            *(q++) = *p;                                      // ... and then high byte
                            p += 2;                                           // jump to next hi/lo byte pair
                        }
                    } else {                                                  // local machine has little endian byte ordering (or unknown)
                        for (i = Count; i != 0; i--)
                            *(q++) = *(p++);                                  // copy 8 bit entries to new 16 bit LUT
                    }
                }
                Data = DataBuffer;
            } else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid value for 'NumberOfTableEntries' (" << Count << ") "
                                         << "... assuming " << count << " !" << endl;
                    ofConsole.unlockCerr();
                }
                Count = count;
            }
        }
        MinValue = (Uint16)DicomImageClass::maxval(MAX_TABLE_ENTRY_SIZE);     // set minimum to maximum value
        register const Uint16 *p = Data;
        register Uint16 value;
        if (DataBuffer != NULL)                                               // LUT entries have been copied 8 -> 16 bits
        {
            for (i = Count; i != 0; i--)
            {
                value = *(p++);
                if (value < MinValue)                                         // get global minimum
                    MinValue = value;
                if (value > MaxValue)                                         // get global maximum
                    MaxValue = value;
            }
            checkBits(bits, 8);                                               // set 'Bits'
        } else {
            int cmp = 0;
            for (i = Count; i != 0; i--)
            {
                value = *(p++);
                if (((value >> 8) != 0) && (value & 0xff) != (value >> 8))    // lo-byte not equal to hi-byte and ...
                    cmp = 1;
                if (value < MinValue)                                         // get global minimum
                    MinValue = value;
                if (value > MaxValue)                                         // get global maximum
                    MaxValue = value;
            }
            if (cmp == 0)                                                     // lo-byte is always equal to hi-byte
                checkBits(bits, MIN_TABLE_ENTRY_SIZE, MAX_TABLE_ENTRY_SIZE);  // set 'Bits'
            else
                checkBits(bits, MAX_TABLE_ENTRY_SIZE, MIN_TABLE_ENTRY_SIZE);
        }
        Uint16 mask = (Uint16)DicomImageClass::maxval(Bits);                  // mask lo-byte (8) or full word (16)
        if (((MinValue & mask) != MinValue) || ((MaxValue & mask) != MaxValue))
        {                                                                     // mask table entries and copy them to new LUT
            MinValue &= mask;
            MaxValue &= mask;
            if (DataBuffer == NULL)
                DataBuffer = new Uint16[Count];                               // create new LUT
            if (DataBuffer != NULL)
            {
                p = Data;
                register Uint16 *q = DataBuffer;
                for (i = Count; i != 0; i--)
                    *(q++) = *(p++) & mask;
            }
            Data = DataBuffer;
        }
        Valid = (Data != NULL);                                               // lookup table is valid
    } else {
        if (status != NULL)
        {
            *status = EIS_InvalidValue;
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: empty 'LookupTableData' attribute !" << endl;
                ofConsole.unlockCerr();
            }
        } else {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: empty 'LookupTableData' attribute ... ignoring LUT !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
}


/********************************************************************/


void DiLookupTable::checkBits(const Uint16 bits,
                              const Uint16 rightBits,
                              const Uint16 wrongBits)
{
    if ((bits < MIN_TABLE_ENTRY_SIZE) || (bits > MAX_TABLE_ENTRY_SIZE))
    {
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
        {
            ofConsole.lockCerr() << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << bits << ") ... should be between "
                                 << MIN_TABLE_ENTRY_SIZE << " and " << MAX_TABLE_ENTRY_SIZE << " inclusive !" << endl;
            ofConsole.unlockCerr();
        }
        if (bits < MIN_TABLE_ENTRY_SIZE)
            Bits = MIN_TABLE_ENTRY_SIZE;
        else
            Bits = MAX_TABLE_ENTRY_SIZE;
    }
    else if (bits == wrongBits)
    {
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
        {
            ofConsole.lockCerr() << "WARNING: unsuitable value for 'BitsPerTableEntry' (" << bits << ") "
                                 << "... assuming " << rightBits << " !" << endl;
            ofConsole.unlockCerr();
        }
        Bits = rightBits;
    } else {

        /* do something heuristically in the future, e.g. create a 'Mask'? */

        Bits = bits;                                // assuming that descriptor value is correct !
    }
}


int DiLookupTable::invertTable(const int flag)
{
    int result = 0;
    if ((Data != NULL) && (Count > 0) && (flag & 0x3))
    {
        register Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        register const Uint8 *p = (const Uint8 *)OriginalData;
                        register Uint8 *q = (Uint8 *)OriginalData;
                        const Uint8 max = (Uint8)DicomImageClass::maxval(Bits);
                        for (i = Count; i != 0; i--)
                            *(q++) = max - *(p++);
                        result |= 0x2;
                    }
                } else {
                    register const Uint16 *p = (const Uint16 *)OriginalData;
                    register Uint16 *q = (Uint16 *)OriginalData;
                    const Uint16 max = (Uint16)DicomImageClass::maxval(Bits);
                    for (i = Count; i != 0; i--)
                        *(q++) = max - *(p++);
                    result |= 0x2;
                }
            }
        }
        if (flag & 0x1)
        {
            if (DataBuffer != NULL)
            {
                register const Uint16 *p = (const Uint16 *)DataBuffer;
                register Uint16 *q = DataBuffer;
                const Uint16 max = (Uint16)DicomImageClass::maxval(Bits);
                for (i = Count; i != 0; i--)
                    *(q++) = max - *(p++);
                result |= 0x1;
            }
            else if (!(flag & 0x2))
            {
                DataBuffer = new Uint16[Count];
                if (DataBuffer != NULL)
                {
                    register const Uint16 *p = Data;
                    register Uint16 *q = DataBuffer;
                    const Uint16 max = (Uint16)DicomImageClass::maxval(Bits);
                    for (i = Count; i != 0; i--)
                        *(q++) = max - *(p++);
                    Data = DataBuffer;
                    result |= 0x1;
                }
            }
        }
    }
    return result;
}


int DiLookupTable::mirrorTable(const int flag)
{
    int result = 0;
    if ((Data != NULL) && (Count > 0) && (flag & 0x3))
    {
        register Uint32 i;
        if (flag & 0x2)
        {
            if (OriginalData != NULL)
            {
                if (OriginalBitsAllocated == 8)
                {
                    if (Bits <= 8)
                    {
                        register Uint8 *p = (Uint8 *)OriginalData + (Count - 1);
                        register Uint8 *q = (Uint8 *)OriginalData;
                        register Uint8 val;
                        const unsigned long mid = Count / 2;
                        for (i = mid; i != 0; i--)
                        {
                            val = *q;
                            *(q++) = *p;
                            *(p--) = val;
                        }
                        result |= 0x2;
                    }
                } else {
                    register Uint16 *p = (Uint16 *)OriginalData + (Count - 1);
                    register Uint16 *q = (Uint16 *)OriginalData;
                    register Uint16 val;
                    const unsigned long mid = Count / 2;
                    for (i = mid; i != 0; i--)
                    {
                        val = *q;
                        *(q++) = *p;
                        *(p--) = val;
                    }
                    result |= 0x2;
                }
            }
        }
        if (flag & 0x1)
        {
            if (DataBuffer != NULL)
            {
                register Uint16 *p = DataBuffer + (Count - 1);
                register Uint16 *q = DataBuffer;
                register Uint16 val;
                const unsigned long mid = Count / 2;
                for (i = mid; i != 0; i--)
                {
                    val = *q;
                    *(q++) = *p;
                    *(p--) = val;
                }
                result |= 0x1;
            }
            else if (!(flag & 0x2))
            {
                DataBuffer = new Uint16[Count];
                if (DataBuffer != NULL)
                {
                    register Uint16 *p = (Uint16 *)Data + (Count - 1);
                    register Uint16 *q = DataBuffer;
                    register Uint16 val;
                    const unsigned long mid = Count / 2;
                    for (i = mid; i != 0; i--)
                    {
                        val = *q;
                        *(q++) = *p;
                        *(p--) = val;
                    }
                    Data = DataBuffer;
                    result |= 0x1;
                }
            }
        }
    }
    return result;
}


DiLookupTable *DiLookupTable::createInverseLUT() const
{
    DiLookupTable *lut = NULL;
    if (Valid)
    {
        const Uint32 count = DicomImageClass::maxval(Bits, 0);
        const Uint16 bits = DicomImageClass::tobits(Count + FirstEntry);
        Uint16 *data = new Uint16[count];
        Uint8 *valid = new Uint8[count];
        if ((data != NULL) && (valid != NULL))
        {
            OFBitmanipTemplate<Uint8>::zeroMem(valid, count);   // initialize array
            register Uint32 i;
            for (i = 0; i < Count; i++)                         // 'copy' values to new array
            {
                if (!valid[Data[i]])
                    data[Data[i]] = (Uint16)(i + FirstEntry);
                valid[Data[i]] = 1;
            }
            Uint32 last = 0;
            i = 0;
            while (i < count)                                   // fill gaps with valid values
            {
                if (valid[i])                                   // skip valid values
                    last = i;
                else
                {
                    register Uint32 j = i + 1;
                    while ((j < count) && !valid[j])            // find next valid value
                        j++;
                    if (valid[last])                            // check for starting conditions
                    {
                        const Uint32 mid = (j < count) ? (i + j) / 2 : count;
                        while (i < mid)
                        {                                       // fill first half with 'left' value
                            data[i] = data[last];
                            i++;
                        }
                    }
                    if ((j < count) && valid[j])
                    {
                        while (i < j)                           // fill second half with 'right' value
                        {
                            data[i] = data[j];
                            i++;
                        }
                        last = j;
                    }
                }
                i++;
            }
            lut = new DiLookupTable(data, count, bits);         // create new LUT
        }
        delete[] valid;
    }
    return lut;
}


int DiLookupTable::compareLUT(const DcmUnsignedShort &data,
                              const DcmUnsignedShort &descriptor)
{
    int result = 1;
    DiBaseLUT *lut = new DiLookupTable(data, descriptor);
    if (lut != NULL)
        result = compare(lut);
    delete lut;
    return result;
}


OFBool DiLookupTable::operator==(const DiBaseLUT &lut)
{
    return (compare(&lut) == 0);
}


OFBool DiLookupTable::operator==(const DiLookupTable &lut)
{
    return (compare((const DiBaseLUT *)&lut) == 0);
}
