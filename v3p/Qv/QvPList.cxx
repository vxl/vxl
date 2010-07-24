#include "QvPList.h"

#define DEFAULT_INITIAL_SIZE 4

QvPList::QvPList()
{
    ptrs  = NULL;
    nPtrs = ptrsSize = 0;

    setSize(0);
}

QvPList::~QvPList()
{
    if (ptrs != NULL)
        delete [] ptrs;
}

int
QvPList::find(const void *ptr) const
{
    int i;

    for (i = 0; i < nPtrs; i++)
        if (ptrs[i] == ptr)
            return i;

    return -1;
}

void
QvPList::remove(int which)
{
    int i;

    for (i = which; i < nPtrs - 1; i++)
        ptrs[i] = ptrs[i + 1];

    setSize(nPtrs - 1);
}

// anuss: insert node before

void QvPList::insert (int where, void* ptr)
{
  nPtrs++;
  if (nPtrs > ptrsSize)
    expand (nPtrs);

  for (int i = nPtrs;  i > where;  i--)
    ptrs[i] = ptrs[i-1];

  ptrs[where] = ptr;
}

void
QvPList::expand(int size)
{
    void **newPtrs;
    int i;

    if (ptrsSize == 0)
        ptrsSize = DEFAULT_INITIAL_SIZE;

    while (size > ptrsSize) {
        ptrsSize *= 2;
    }

    newPtrs = new void *[ptrsSize];

    if (ptrs != NULL) {
        for (i = 0; i < nPtrs; i++)
            newPtrs[i] = ptrs[i];
        delete [] ptrs;
    }

    ptrs = newPtrs;
}
