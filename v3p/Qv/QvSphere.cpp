#include <QvSphere.h>

QV_NODE_SOURCE(QvSphere);

QvSphere::QvSphere()
{
    QV_NODE_CONSTRUCTOR(QvSphere);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(radius);

    radius.value = 1.0;
}

QvSphere::~QvSphere()
{
}
