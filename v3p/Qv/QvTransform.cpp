#include <QvTransform.h>

QV_NODE_SOURCE(QvTransform);

QvTransform::QvTransform()
{
    QV_NODE_CONSTRUCTOR(QvTransform);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(translation);
    QV_NODE_ADD_FIELD(rotation);
    QV_NODE_ADD_FIELD(scaleFactor);
    QV_NODE_ADD_FIELD(scaleOrientation);
    QV_NODE_ADD_FIELD(center);

    translation.value[0] = translation.value[1] = translation.value[2] = 0.0;
    rotation.axis[0] = 0.0;
    rotation.axis[1] = 0.0;
    rotation.axis[2] = 1.0;
    rotation.angle = 0.0;
    scaleFactor.value[0] = scaleFactor.value[1] = scaleFactor.value[2] = 1.0;
    scaleOrientation.axis[0] = 0.0;
    scaleOrientation.axis[1] = 0.0;
    scaleOrientation.axis[2] = 1.0;
    scaleOrientation.angle = 0.0;
    center.value[0] = center.value[1] = center.value[2] = 0.0;
}

QvTransform::~QvTransform()
{
}
