#include <QvNormalBinding.h>

QV_NODE_SOURCE(QvNormalBinding);

QvNormalBinding::QvNormalBinding()
{
    QV_NODE_CONSTRUCTOR(QvNormalBinding);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(value);

    value.value = DEFAULT;

    QV_NODE_DEFINE_ENUM_VALUE(Binding, DEFAULT);
    QV_NODE_DEFINE_ENUM_VALUE(Binding, OVERALL);
    QV_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART);
    QV_NODE_DEFINE_ENUM_VALUE(Binding, PER_PART_INDEXED);
    QV_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE);
    QV_NODE_DEFINE_ENUM_VALUE(Binding, PER_FACE_INDEXED);
    QV_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX);
    QV_NODE_DEFINE_ENUM_VALUE(Binding, PER_VERTEX_INDEXED);

    QV_NODE_SET_SF_ENUM_TYPE(value, Binding);
}

QvNormalBinding::~QvNormalBinding()
{
}
