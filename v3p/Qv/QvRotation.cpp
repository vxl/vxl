#include <QvRotation.h>

QV_NODE_SOURCE(QvRotation);

QvRotation::QvRotation()
{
    QV_NODE_CONSTRUCTOR(QvRotation);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(rotation);

    rotation.axis[0] = 0.0;
    rotation.axis[1] = 0.0;
    rotation.axis[2] = 1.0;
    rotation.angle = 0.0;
}

QvRotation::~QvRotation()
{
}
