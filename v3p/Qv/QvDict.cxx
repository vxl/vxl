#include "QvDict.h"

#ifdef _MSC_VER
# if _MSC_VER <= 1200
   typedef unsigned long intptr_t;
# else
#  include <stddef.h>
#  include <stdlib.h> /* for malloc and friends */
# endif
#else
# if defined(__alpha) /* there is no inttypes.h here */
   typedef unsigned long intptr_t;
# elif defined(__CYGWIN__)
#  include <sys/types.h> /* for intptr_t on Cygwin */
# elif defined(__BORLANDC__)
#  if __BORLANDC__ < 0x0560
    typedef unsigned long intptr_t;
#  else
#   include <stdint.h> /* for intptr_t on Borland 5.6. */
#  endif
# else
#  include <inttypes.h> /* for intptr_t on e.g. SGI, Linux, Solaris */
# endif
#endif

struct QvDictListThing {
    QvPList *keyList;
    QvPList *valueList;
};

QvDict::QvDict( int entries )
{
    tableSize=entries;
    buckets=new QvDictEntry *[tableSize];
    for (int i = 0; i < tableSize; i++)
        buckets[i] = NULL;
}

QvDict::~QvDict()
{
    clear();
    delete [] buckets;
}

void
QvDict::clear()
{
    int         i;
    QvDictEntry *entry, *nextEntry;

    for (i = 0; i < tableSize; i++) {

        for (entry = buckets[i]; entry != NULL; entry = nextEntry) {
            nextEntry = entry->next;
            delete entry;
        }
        buckets[i] = NULL;
    }
}

QvBool
QvDict::enter(const char* key, void *value)
{
    QvDictEntry         *&entry = findEntry(key);

    if (entry == NULL) {
        entry = new QvDictEntry(key, value);
        entry->next = NULL;
        return TRUE;
    }
    else {
        entry->value = value;
        return FALSE;
    }
}

QvBool
QvDict::find(const char* key, void *&value) const
{
    QvDictEntry         *&entry = findEntry(key);

    if (entry == NULL) {
        value = NULL;
        return FALSE;
    }
    else {
        value = entry->value;
        return TRUE;
    }
}

QvDictEntry *&
QvDict::findEntry(const char* key) const
{
    QvDictEntry         **entry;

    entry = &buckets[ (intptr_t)key % tableSize];

    while (*entry != NULL) {
        if ((*entry)->key == key)
            break;
        entry = &(*entry)->next;
    }
    return *entry;
}

QvBool
QvDict::remove(const char* key)
{
    QvDictEntry         *&entry = findEntry(key);
    QvDictEntry         *tmp;

    if (entry == NULL)
        return FALSE;
    else {
        tmp = entry;
        entry = entry->next;
        delete tmp;
        return TRUE;
    }
}
