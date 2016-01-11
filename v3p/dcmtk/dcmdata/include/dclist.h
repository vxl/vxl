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
 *  Purpose: generic list class
 *
 */

#ifndef DCLIST_H
#define DCLIST_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcerror.h"
#include "dctypes.h"

#define INCLUDE_CSTDDEF
#define INCLUDE_CSTDLIB
#include "ofstdinc.h"


const unsigned long DCM_EndOfListIndex = (unsigned long)(-1L);


class DcmObject;    // forward declaration


class DcmListNode {
    friend class DcmList;
    DcmListNode *nextNode;
    DcmListNode *prevNode;
    DcmObject *objNodeValue;

 // --- declarations to avoid compiler warnings

    DcmListNode(const DcmListNode &);
    DcmListNode &operator=(const DcmListNode &);

public:
    DcmListNode( DcmObject *obj );
    ~DcmListNode();
    DcmObject *value();
};


typedef enum
{
    ELP_atpos,
    ELP_first,
    ELP_last,
    ELP_prev,
    ELP_next
} E_ListPos;

/* this class only manages pointers to elements.
 * remove() does not delete the element pointed to.
 * Upon destruction of the list, all elements pointed to are also deleted.
 */

class DcmList {
    DcmListNode *firstNode;
    DcmListNode *lastNode;
    DcmListNode *actualNode;
    unsigned long cardinality;

 // --- declarations to avoid compiler warnings

    DcmList &operator=(const DcmList &);
    DcmList(const DcmList &newList);

public:
    DcmList();
    ~DcmList();

    DcmObject *append(  DcmObject *obj );
    DcmObject *prepend( DcmObject *obj );
    DcmObject *insert(  DcmObject *obj,
                        E_ListPos pos = ELP_next );
    DcmObject *remove();
    DcmObject *get(     E_ListPos pos = ELP_atpos );
    DcmObject *seek(    E_ListPos pos = ELP_next );
    DcmObject *seek_to(unsigned long absolute_position);
    inline unsigned long card() const { return cardinality; }
    inline OFBool empty(void) const { return firstNode == NULL; }
    inline OFBool valid(void) const { return actualNode != NULL; }
};

#endif  // DCLIST_H
