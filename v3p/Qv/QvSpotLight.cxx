#include "QvSpotLight.h"
#include "Qv_pi.h" // for QV_PI_4

QV_NODE_SOURCE(QvSpotLight);

QvSpotLight::QvSpotLight()
{
    QV_NODE_CONSTRUCTOR(QvSpotLight);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(on);
    QV_NODE_ADD_FIELD(intensity);
    QV_NODE_ADD_FIELD(color);
    QV_NODE_ADD_FIELD(location);
    QV_NODE_ADD_FIELD(direction);
    QV_NODE_ADD_FIELD(dropOffRate);
    QV_NODE_ADD_FIELD(cutOffAngle);

    on.value = TRUE;
    intensity.value = 1.0;
    color.value[0] = color.value[1] = color.value[2] = 1.0;
    location.value[0] = 0.0;
    location.value[1] = 0.0;
    location.value[2] = 1.0;
    direction.value[0] = 0.0;
    direction.value[1] = 0.0;
    direction.value[2] = -1.0;
    dropOffRate.value = 0.0;
    cutOffAngle.value = (float)QV_PI_4;
}

QvSpotLight::~QvSpotLight()
{
}
