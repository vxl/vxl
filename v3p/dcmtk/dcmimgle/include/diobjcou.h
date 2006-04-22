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
 *  Purpose: DicomObjectCounter (Header)
 *
 */


#ifndef __DIOBJCOU_H
#define __DIOBJCOU_H

#include "osconfig.h"

#include "ofthread.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to count number of instances (objects created from a certain class).
 *  used to manage more than one reference to an object in a secure way.
 */
class DiObjectCounter
{
 public:

    /** add a reference.
     *  Increase the internal counter by 1.
     */
    inline void addReference()
    {
#ifdef _REENTRANT
        theMutex.lock();
#endif
        Counter++;
#ifdef _REENTRANT
        theMutex.unlock();
#endif
    }

    /** remove a reference.
     *  Decrease the internal counter by 1 and delete the object only if the counter is zero.
     */
    inline void removeReference()
    {
#ifdef _REENTRANT
        theMutex.lock();
#endif
        if (--Counter == 0)
        {
#ifdef _REENTRANT
            theMutex.unlock();
#endif
            delete this;
#ifdef _REENTRANT
        } else {
            theMutex.unlock();
#endif
        }
    }


 protected:

    /** constructor.
     *  Internal counter is initialized with 1.
     */
    DiObjectCounter()
      : Counter(1)
#ifdef _REENTRANT
       ,theMutex()
#endif
    {
    }

    /** destructor
     */
    virtual ~DiObjectCounter()
    {
    }


 private:

    /// internal counter
    unsigned long Counter;

#ifdef _REENTRANT
    /** if compiled for multi-thread operation, the Mutex protecting
     *  access to the value of this object.
     */
    OFMutex theMutex;
#endif
};


#endif
