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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Basis class for dicom tags.
 *
 */

#ifndef DCMTAGKEY_H
#define DCMTAGKEY_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofstream.h"
#include "dctypes.h"
#include "ofstring.h"

/*
** Defines
*/

#define DCM_UndefinedTagKey     DcmTagKey(0xffff, 0xffff);


/*
 * Unique key generation for DICOM tags
 */

class DcmTagKey {
private:
    Uint16 group;
    Uint16 element;

protected:
    int groupLT(const DcmTagKey& key) const;
    int groupGT(const DcmTagKey& key) const;
    int groupEQ(const DcmTagKey& key) const;
    int elementLT(const DcmTagKey& key) const;
    int elementGT(const DcmTagKey& key) const;
    int elementEQ(const DcmTagKey& key) const;

public:
    DcmTagKey();
    DcmTagKey(const DcmTagKey& key);
    DcmTagKey(Uint16 g, Uint16 e);

    void set(const DcmTagKey& key);
    void set(Uint16 g, Uint16 e);
    void setGroup(Uint16 g);
    void setElement(Uint16 e);
    Uint16 getGroup() const;
    Uint16 getElement() const;

    Uint32 hash() const; // generate simple hash code

    DcmTagKey& operator = (const DcmTagKey& key);
    int operator == (const DcmTagKey& key) const;
    int operator != (const DcmTagKey& key) const;
    int operator < (const DcmTagKey& key) const;
    int operator > (const DcmTagKey& key) const;
    int operator <= (const DcmTagKey& key) const;
    int operator >= (const DcmTagKey& key) const;

    friend ostream&   operator<<(ostream& s, const DcmTagKey& k);

    OFString toString() const;

    /** returns true if a data element with the given tag key can
     *  be digitally signed, false otherwise
     *  @return true if signable, false otherwise
     */
    OFBool isSignableTag() const;
};


/*
** inline versions of functions
*/

/* Constructors */

inline
DcmTagKey::DcmTagKey()
  : group(0xffff),
    element(0xffff)
{
}

inline
DcmTagKey::DcmTagKey(const DcmTagKey& key)
  : group(key.group),
    element(key.element)
{
}

inline
DcmTagKey::DcmTagKey(Uint16 g, Uint16 e)
  : group(g),
    element(e)
{
}

/* access methods */

inline void
DcmTagKey::set(const DcmTagKey& key)
{
    group = key.group;
    element = key.element;
}

inline void
DcmTagKey::set(Uint16 g, Uint16 e)
{
    group = g;
    element = e;
}

inline void
DcmTagKey::setGroup(Uint16 g)
{
    group = g;
}

inline void
DcmTagKey::setElement(Uint16 e)
{
    element = e;
}

inline Uint16
DcmTagKey::getGroup() const
{
    return group;
}

inline Uint16
DcmTagKey::getElement() const
{
    return element;
}

inline DcmTagKey&
DcmTagKey::operator=(const DcmTagKey& key)
{
    set(key);
    return *this;
}

/* Simple Hash Function */

inline Uint32
DcmTagKey::hash() const
{
    // generate simple hash code
    return (((getGroup() << 16) & 0xffff0000) | (getElement() & 0xffff));
}

/* Comparisons */

inline int
DcmTagKey::groupLT(const DcmTagKey& key) const
{
    return (getGroup() < key.getGroup());
}

inline int
DcmTagKey::groupGT(const DcmTagKey& key) const
{
    return (getGroup() > key.getGroup());
}

inline int
DcmTagKey::groupEQ(const DcmTagKey& key) const
{
    return getGroup() == key.getGroup();
}

inline int
DcmTagKey::elementLT(const DcmTagKey& key) const
{
    return (getElement() < key.getElement());
}

inline int
DcmTagKey::elementGT(const DcmTagKey& key) const
{
    return (getElement() > key.getElement());
}

inline int
DcmTagKey::elementEQ(const DcmTagKey& key) const
{
    return getElement() == key.getElement();
}

inline int
DcmTagKey::operator == (const DcmTagKey& key) const
{
    return ( groupEQ(key) && elementEQ(key) );
}

inline int
DcmTagKey::operator != (const DcmTagKey& key) const
{
    return !(*this == key);
}

inline int
DcmTagKey::operator < (const DcmTagKey& key) const
{
    return (groupLT(key) || (groupEQ(key) && elementLT(key)));
}

inline int
DcmTagKey::operator > (const DcmTagKey& key) const
{
    return (groupGT(key) || (groupEQ(key) && elementGT(key)));
}

inline int
DcmTagKey::operator <= (const DcmTagKey& key) const
{
    return (*this < key) || (*this == key);
}

inline int
DcmTagKey::operator >= (const DcmTagKey& key) const
{
    return (*this > key) || (*this == key);
}

#endif
