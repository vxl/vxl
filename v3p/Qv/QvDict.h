#ifndef _QV_DICT_
#define _QV_DICT_

#include "QvBasic.h"
#include "QvString.h"
#include "QvPList.h"

class QvDictEntry {
  private:
    u_long		key;
    void *		value;
    QvDictEntry *	next;
    QvDictEntry(u_long k, void *v)	{ key = k; value = v; };

friend class QvDict;
};

class QvDict {
  public:
    QvDict( int entries = 251 );
    ~QvDict();
    void	clear();
    QvBool	enter(u_long key, void *value);
    QvBool	find(u_long key, void *&value) const;
    QvBool	remove(u_long key);

  private:
    int			tableSize;
    QvDictEntry *	*buckets;
    QvDictEntry *&	findEntry(u_long key) const;
};

#endif /* _QV_DICT_ */
