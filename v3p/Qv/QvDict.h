#ifndef _QV_DICT_
#define _QV_DICT_

#include "QvBasic.h"
#include "QvString.h"
#include "QvPList.h"

class QvDictEntry {
  private:
    const char*         key;
    void *              value;
    QvDictEntry *       next;
    QvDictEntry(const char* k, void *v)      { key = k; value = v; };

friend class QvDict;
};

class QvDict {
  public:
    QvDict( int entries = 251 );
    ~QvDict();
    void        clear();
    QvBool      enter(const char* key, void *value);
    QvBool      find(const char* key, void *&value) const;
    QvBool      remove(const char* key);

  private:
    int                 tableSize;
    QvDictEntry *       *buckets;
    QvDictEntry *&      findEntry(const char* key) const;
};

#endif /* _QV_DICT_ */
