#include <QvDirectionalLight.h>

QV_NODE_SOURCE(QvDirectionalLight);

QvDirectionalLight::QvDirectionalLight()
{
    QV_NODE_CONSTRUCTOR(QvDirectionalLight);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(on);
    QV_NODE_ADD_FIELD(intensity);
    QV_NODE_ADD_FIELD(color);
    QV_NODE_ADD_FIELD(direction);

    on.value = TRUE;
    intensity.value = 1.0;
    color.value[0] = color.value[1] = color.value[2] = 1.0;
    direction.value[0] = 0.0;
    direction.value[1] = 0.0;
    direction.value[2] = -1.0;
}

QvDirectionalLight::~QvDirectionalLight()
{
}
