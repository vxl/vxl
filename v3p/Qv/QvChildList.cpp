#include <QvChildList.h>

QvChildList::QvChildList() : QvNodeList()
{
}

QvChildList::~QvChildList()
{
    truncate(0);
}
