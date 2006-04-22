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
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: class DcmTag
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctag.h"
#include "dcerror.h"     /* for dcmdata error constants */
#include "dcdict.h"
#include "dcdicent.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


DcmTag::DcmTag()
  : vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
}

DcmTag::DcmTag(const DcmTagKey& akey)
  : DcmTagKey(akey),
    vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
    lookupVRinDictionary();
}

DcmTag::DcmTag(Uint16 g, Uint16 e)
  : DcmTagKey(g, e),
    vr(EVR_UNKNOWN),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_InvalidTag)
{
    lookupVRinDictionary();
}

DcmTag::DcmTag(Uint16 g, Uint16 e, const DcmVR& avr)
  : DcmTagKey(g, e),
    vr(avr),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_Normal)
{
}

DcmTag::DcmTag(const DcmTagKey& akey, const DcmVR& avr)
  : DcmTagKey(akey),
    vr(avr),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(EC_Normal)
{
}

DcmTag::DcmTag(const DcmTag& tag)
  : DcmTagKey(tag),
    vr(tag.vr),
    tagName(NULL),
    privateCreator(NULL),
    errorFlag(tag.errorFlag)
{
  updateTagName(tag.tagName);
  updatePrivateCreator(tag.privateCreator);
}


// ********************************


DcmTag::~DcmTag()
{
  delete[] tagName;
  delete[] privateCreator;
}


// ********************************


DcmTag& DcmTag::operator= ( const DcmTag& tag )
{
    if (this != &tag)
    {
      updateTagName(tag.tagName);
      updatePrivateCreator(tag.privateCreator);
      DcmTagKey::set(tag);
      vr = tag.vr;
      errorFlag = tag.errorFlag;
    }
    return *this;
}

// ********************************

void DcmTag::lookupVRinDictionary()
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(*this, privateCreator);
    if (dictRef)
    {
        vr = dictRef->getVR();
        errorFlag = EC_Normal;
    }
    dcmDataDict.unlock();
}

// ********************************


DcmVR DcmTag::setVR( const DcmVR& avr )    // resolve ambiguous VR
{
    vr = avr;

    if ( vr.getEVR() == EVR_UNKNOWN ) {
        errorFlag = EC_InvalidVR;
    } else {
        errorFlag = EC_Normal;
    }
    return vr;
}


const char *DcmTag::getTagName()
{
  if (tagName) return tagName;

  const char *newTagName = NULL;
  const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
  const DcmDictEntry *dictRef = globalDataDict.findEntry(*this, privateCreator);
  if (dictRef) newTagName=dictRef->getTagName();
  if (newTagName==NULL) newTagName = DcmTag_ERROR_TagName;
  updateTagName(newTagName);
  dcmDataDict.unlock();

  if (tagName) return tagName;
  return DcmTag_ERROR_TagName;
}

OFBool DcmTag::isSignable() const
{
  OFBool result = isSignableTag();
  if (result) result = (! isUnknownVR());
  return result;
}

OFBool DcmTag::isUnknownVR() const
{
  OFBool result = OFFalse;
  switch (vr.getValidEVR()) // this is the VR we're going to write in explicit VR
  {
    case EVR_UNKNOWN:
    case EVR_UNKNOWN2B:
    case EVR_UN:
      result = OFTrue;
      break;
    default:
      /* nothing */
      break;
  }
  return result;
}


OFCondition DcmTag::findTagFromName(const char *name,
                                    DcmTag &value)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((name != NULL) && (strlen(name) > 0))
    {
        result = EC_Normal;
        unsigned int grp = 0xffff;
        unsigned int elm = 0xffff;
        /* check whether tag name has format 'xxxx,xxxx' */
        if (sscanf(name, "%x,%x", &grp, &elm) == 2)
        {
            /* store resulting tag value */
            value.set(grp, elm);
        } else {
            /* it is a name: look up in the dictionary */
            const DcmDataDictionary &globalDataDict = dcmDataDict.rdlock();
            const DcmDictEntry *dicent = globalDataDict.findEntry(name);
            /* store resulting tag value */
            if (dicent != NULL)
                value.set(dicent->getKey());
            else
                result = EC_TagNotFound;
            dcmDataDict.unlock();
        }
    }
    return result;
}


const char* DcmTag::getPrivateCreator() const
{
  return privateCreator;
}

void DcmTag::setPrivateCreator(const char *privCreator)
{
    // a new private creator code probably changes the name
    // of the tag. Enforce new dictionary lookup the next time
    // getTagName() is called.
    updateTagName(NULL);
    updatePrivateCreator(privCreator);
}

void DcmTag::updateTagName(const char *c)
{
    delete[] tagName;
    if (c)
    {
      tagName = new char[strlen(c)+1];
      if (tagName) strcpy(tagName,c);
    } else tagName = NULL;
}

void DcmTag::updatePrivateCreator(const char *c)
{
    delete[] privateCreator;
    if (c)
    {
      privateCreator = new char[strlen(c)+1];
      if (privateCreator) strcpy(privateCreator,c);
    } else privateCreator = NULL;
}
