#include <QvPointSet.h>

QV_NODE_SOURCE(QvPointSet);

QvPointSet::QvPointSet()
{
    QV_NODE_CONSTRUCTOR(QvPointSet);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(startIndex);
    QV_NODE_ADD_FIELD(numPoints);

    startIndex.value = 0;
    numPoints.value = QV_POINT_SET_USE_REST_OF_POINTS;
}

QvPointSet::~QvPointSet()
{
}
