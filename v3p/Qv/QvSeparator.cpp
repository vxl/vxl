#include <QvSeparator.h>

QV_NODE_SOURCE(QvSeparator);

QvSeparator::QvSeparator()
{
    QV_NODE_CONSTRUCTOR(QvSeparator);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(renderCulling);

    renderCulling.value = AUTO;

    QV_NODE_DEFINE_ENUM_VALUE(CullEnabled, ON);
    QV_NODE_DEFINE_ENUM_VALUE(CullEnabled, OFF);
    QV_NODE_DEFINE_ENUM_VALUE(CullEnabled, AUTO);

    QV_NODE_SET_SF_ENUM_TYPE(renderCulling, CullEnabled);
}

QvSeparator::~QvSeparator()
{
}
