#include <QvPointLight.h>

QV_NODE_SOURCE(QvPointLight);

QvPointLight::QvPointLight()
{
    QV_NODE_CONSTRUCTOR(QvPointLight);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(on);
    QV_NODE_ADD_FIELD(intensity);
    QV_NODE_ADD_FIELD(color);
    QV_NODE_ADD_FIELD(location);

    on.value = TRUE;
    intensity.value = 1.0;
    color.value[0] = color.value[1] = color.value[2] = 1.0;
    location.value[0] = 0.0;
    location.value[1] = 0.0;
    location.value[2] = 1.0;
}

QvPointLight::~QvPointLight()
{
}
