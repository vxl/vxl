#include <QvCube.h>

QV_NODE_SOURCE(QvCube);

QvCube::QvCube()
{
    QV_NODE_CONSTRUCTOR(QvCube);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(width);
    QV_NODE_ADD_FIELD(height);
    QV_NODE_ADD_FIELD(depth);

    width.value = 2.0;
    height.value = 2.0;
    depth.value = 2.0;
}

QvCube::~QvCube()
{
}
