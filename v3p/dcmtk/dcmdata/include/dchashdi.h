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
 *  Purpose: Hash table interface for DICOM data dictionary
 *
 */

#ifndef DCHASHDI_H
#define DCHASHDI_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "ofstream.h"

class DcmDictEntry;
class DcmTagKey;
class DcmHashDict;

/** the default size for a data dictionary hash table */
const int DCMHASHDICT_DEFAULT_HASHSIZE = 2047;

/** iterator class for traversing a DcmDictEntryList
 */
class DcmDictEntryListIterator: public OFListIterator(DcmDictEntry *)
{
public:
    /// default constructor
    DcmDictEntryListIterator() {}

    /** constructor
     *  @param iter reference to an object of the base class
     */
    DcmDictEntryListIterator(const OFListIterator(DcmDictEntry*)& iter)
    : OFListIterator(DcmDictEntry*)(iter) {}

    /// copy assignment operator
    DcmDictEntryListIterator& operator=(const DcmDictEntryListIterator& i)
    {
      OFListIterator(DcmDictEntry*)::operator=(i);
      return *this;
    }
};


/** an ordered list of pointers to DcmDictEntry objects
 */
class DcmDictEntryList : public OFList<DcmDictEntry *>
{
public:
    /// constructor
    DcmDictEntryList() {}

    /// destructor
    ~DcmDictEntryList();

    /// clears list and deletes all entries
    void clear();

    /** inserts an entry into the list and returns any replaced entry
     *  @param e new list entry
     *  @return replaced list entry or NULL
     */
    DcmDictEntry* insertAndReplace(DcmDictEntry* e);

    /* find an entry in the set */
    DcmDictEntry *find(const DcmTagKey& k, const char *privCreator);
};


/** iterator class for traversing a DcmHashDict
 */
class DcmHashDictIterator
{
public:
    /// default constructor
    DcmHashDictIterator()
      : dict(NULL), hindex(0), iterating(OFFalse), iter()
          { init(NULL); }

    /** constructor, creates iterator to existing hash dictionary
     *  @param d pointer to dictionary
     *  @param atEnd if true, iterator points after last element
     *   of hash dictionary, otherwise iterator points to first element.
     */
    DcmHashDictIterator(const DcmHashDict* d, OFBool atEnd = OFFalse)
      : dict(NULL), hindex(0), iterating(OFFalse), iter()
          { init(d, atEnd); }

    /// copy constructor
    DcmHashDictIterator(const DcmHashDictIterator& i)
      : dict(i.dict), hindex(i.hindex), iterating(i.iterating), iter(i.iter)
          { }

    /// copy assignment operator
    DcmHashDictIterator& operator=(const DcmHashDictIterator& i)
        { dict = i.dict; hindex = i.hindex;
        iterating = i.iterating; iter = i.iter; return *this; }

    /// comparison equality
    OFBool operator==(const DcmHashDictIterator& x) const
        { return (hindex == x.hindex) && (iter == x.iter); }

    /// comparison non-equality
    OFBool operator!=(const DcmHashDictIterator& x) const
        { return (hindex != hindex) || (iter != x.iter); }

    /// dereferencing of iterator
    const DcmDictEntry* operator*() const
        { return (*iter); }

    /// pre-increment operator
    DcmHashDictIterator& operator++()
        { stepUp(); return *this; }

    /// post-increment operator
    DcmHashDictIterator operator++(int)
        { DcmHashDictIterator tmp(*this); stepUp(); return tmp; }

private:
    /** initializes the iterator
     *  @param d pointer to hash dictionary, may be NULL
     *  @param atEnd if true, iterator points after last element
     *   of hash dictionary, otherwise iterator points to first element.
     */
    void init(const DcmHashDict *d, OFBool atEnd = OFFalse);

    /** implements increment operator on hash dictionary
     */
    void stepUp();

    /// pointer to the hash dictionary this iterator traverses
    const DcmHashDict* dict;

    /// index of current bucket
    int hindex;

    /// flag indicating if iter is currently valid
    OFBool iterating;

    /// iterator for traversing a bucket in the hash table
    DcmDictEntryListIterator iter;
};


/** a hash table of pointers to DcmDictEntry objects
 */
class DcmHashDict
{

public:
    /** constructor
     *  @param hashTabLen number of buckets in hash table
     */
    DcmHashDict(int hashTabLen = DCMHASHDICT_DEFAULT_HASHSIZE)
     : hashTab(NULL), hashTabLength(0), lowestBucket(0), highestBucket(0), entryCount(0)
        { _init(hashTabLen); }

    /// destructor
    ~DcmHashDict();

    /// counts total number of entries
    int size() const { return entryCount; }

    /// clears the hash table of all entries
    void clear();

    /** inserts an entry into hash table (deletes old entry if present)
     *  @param e pointer to new entry
     */
    void put(DcmDictEntry* e);

    /** hash table lookup for the given tag key and private creator name.
     *  @param key tag key
     *  @param privCreator private creator name, may be NULL
     */
    const DcmDictEntry* get(const DcmTagKey& k, const char *privCreator) const;

    /** deletes the entry for the given tag and private creator
     *  @param k tag key
     *  @param privCreator private creator name, may be NULL
     */
    void del(const DcmTagKey& k, const char *privCreator);

    // iterator over the contents of the hash table
    friend class DcmHashDictIterator;

    /// returns iterator to start of hash table
    DcmHashDictIterator begin() const
        { DcmHashDictIterator iter(this); return iter; }

    /// returns iterator to end of hash table
    DcmHashDictIterator end() const
        { DcmHashDictIterator iter(this, OFTrue); return iter; }

    /// prints some information about hash table bucket utilization
    ostream& loadSummary(ostream& out);

private:

    /// private unimplemented copy constructor
    DcmHashDict(const DcmHashDict &);

    /// private unimplemented copy assignment operator
    DcmHashDict &operator=(const DcmHashDict &);

    /// performs initialization for given hash table size, called from constructor
    void _init(int hashSize);

    /** compute hash value for given tag key
     *  @param k pointer to tag key
     *  @return hash value
     */
    int hash(const DcmTagKey* k) const;

    /** inserts new entry into given list
     *  @param list list to add to
     *  @param e new element to add, will be deleted upon destruction of the hash table
     *  @return pointer to replaced element, if any
     */
    DcmDictEntry* insertInList(DcmDictEntryList& list, DcmDictEntry* e);

    /** removes the entry for the given tag and private creator
     *  @param list list to remove from
     *  @param k tag key
     *  @param privCreator private creator name, may be NULL
     *  @return pointer to removed element, if any
     */
    DcmDictEntry* removeInList(DcmDictEntryList& list, const DcmTagKey& k, const char *privCreator);

    /** searcjes entry for the given tag and private creator
     *  @param list list to search in
     *  @param k tag key
     *  @param privCreator private creator name, may be NULL
     *  @return pointer to found element, NULL if not found
     */
    DcmDictEntry* findInList(DcmDictEntryList& list, const DcmTagKey& k, const char *privCreator) const;

    /** array of (hash table size) pointers to DcmDictEntryList elements
     *  implementing the different buckets of the hash table
     */
    DcmDictEntryList** hashTab;

    /// number of buckets in hash table
    int hashTabLength;

    /// index of lowest bucket for which the DcmDictEntryList has been initialized
    int lowestBucket;

    /// index of highest bucket for which the DcmDictEntryList has been initialized
    int highestBucket;

    /// number of entries in hash table
    int entryCount;

};

#endif /* DCHASHDI_H */
