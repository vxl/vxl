#include "QvInfo.h"

QV_NODE_SOURCE(QvInfo);

QvInfo::QvInfo()
{
    QV_NODE_CONSTRUCTOR(QvInfo);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(string);

    string.value = "<Undefined info>";

    handled_ = 0;  // mpichler, 19951116
}

QvInfo::~QvInfo()
{
}
