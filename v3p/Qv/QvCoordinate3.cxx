#include "QvCoordinate3.h"

QV_NODE_SOURCE(QvCoordinate3);

QvCoordinate3::QvCoordinate3()
{
    QV_NODE_CONSTRUCTOR(QvCoordinate3);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(point);

    point.values[0] = point.values[1] = point.values[2] = 0.0;

    minmaxbuilt_ = 0;  // mpichler, 19960614
}

QvCoordinate3::~QvCoordinate3()
{
}
