#include "QvLists.h"
#include "QvNode.h"

QvNodeList::QvNodeList() : QvPList()
{
}

void
QvNodeList::append(QvNode *node)
{
    QvPList::append(node);

    node->ref();
}

void
QvNodeList::remove(int which)
{
    if ((*this)[which] != NULL)
        (*this)[which]->unref();

    QvPList::remove(which);
}

// anuss: insert node before

void QvNodeList::insert (int where, QvNode* node)
{
    QvPList::insert (where, node);

    node->ref ();
}


void
QvNodeList::truncate(int start)
{
    for (int i = start; i < getLength(); i++)
        if ((*this)[i] != NULL)
            (*this)[i]->unref();

    QvPList::truncate(start);
}
