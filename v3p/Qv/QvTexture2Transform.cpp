#include <QvTexture2Transform.h>

QV_NODE_SOURCE(QvTexture2Transform);

QvTexture2Transform::QvTexture2Transform()
{
    QV_NODE_CONSTRUCTOR(QvTexture2Transform);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(translation);
    QV_NODE_ADD_FIELD(rotation);
    QV_NODE_ADD_FIELD(scaleFactor);
    QV_NODE_ADD_FIELD(center);

    translation.value[0] = translation.value[1] = 0.0;
    rotation.value = 0.0;
    scaleFactor.value[0] = scaleFactor.value[1] = 1.0;
    center.value[0] = center.value[1] = 0.0;
}

QvTexture2Transform::~QvTexture2Transform()
{
}
