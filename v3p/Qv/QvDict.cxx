#include <QvDict.h>

struct QvDictListThing {
    QvPList	*keyList;
    QvPList	*valueList;
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
    int		i;
    QvDictEntry	*entry, *nextEntry;

    for (i = 0; i < tableSize; i++) {

	for (entry = buckets[i]; entry != NULL; entry = nextEntry) {
	    nextEntry = entry->next;
	    delete entry;
	}
	buckets[i] = NULL;
    }
}

QvBool
QvDict::enter(u_long key, void *value)
{
    QvDictEntry		*&entry = findEntry(key);

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
QvDict::find(u_long key, void *&value) const
{
    QvDictEntry		*&entry = findEntry(key);

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
QvDict::findEntry(u_long key) const
{
    QvDictEntry		**entry;

    entry = &buckets[key % tableSize];

    while (*entry != NULL) {
	if ((*entry)->key == key)
	    break;
	entry = &(*entry)->next;
    }
    return *entry;
}

QvBool
QvDict::remove(u_long key)
{
    QvDictEntry		*&entry = findEntry(key);
    QvDictEntry		*tmp;

    if (entry == NULL)
	return FALSE;
    else {
	tmp = entry;
	entry = entry->next;
	delete tmp;
	return TRUE;
    }
}
