#include <QvScale.h>

QV_NODE_SOURCE(QvScale);

QvScale::QvScale()
{
    QV_NODE_CONSTRUCTOR(QvScale);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(scaleFactor);

    scaleFactor.value[0] = scaleFactor.value[1] = scaleFactor.value[2] =1.0;
}

QvScale::~QvScale()
{
}
