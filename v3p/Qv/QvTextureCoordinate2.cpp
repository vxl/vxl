#include <QvTextureCoordinate2.h>

QV_NODE_SOURCE(QvTextureCoordinate2);

QvTextureCoordinate2::QvTextureCoordinate2()
{
    QV_NODE_CONSTRUCTOR(QvTextureCoordinate2);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(point);

    point.values[0] = point.values[1] = 0.0;
}

QvTextureCoordinate2::~QvTextureCoordinate2()
{
}
