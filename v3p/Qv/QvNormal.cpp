#include <QvNormal.h>

QV_NODE_SOURCE(QvNormal);

QvNormal::QvNormal()
{
    QV_NODE_CONSTRUCTOR(QvNormal);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(vector);

    vector.values[0] = 0.0;
    vector.values[1] = 0.0;
    vector.values[2] = 1.0;
}

QvNormal::~QvNormal()
{
}
