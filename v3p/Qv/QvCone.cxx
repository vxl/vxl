#include <QvCone.h>

QV_NODE_SOURCE(QvCone);

QvCone::QvCone()
{
    QV_NODE_CONSTRUCTOR(QvCone);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(parts);
    QV_NODE_ADD_FIELD(bottomRadius);
    QV_NODE_ADD_FIELD(height);

    parts.value = ALL;
    bottomRadius.value = 1.0;
    height.value = 2.0;

    QV_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
    QV_NODE_DEFINE_ENUM_VALUE(Part, BOTTOM);
    QV_NODE_DEFINE_ENUM_VALUE(Part, ALL);

    QV_NODE_SET_SF_ENUM_TYPE(parts, Part);
}

QvCone::~QvCone()
{
}
