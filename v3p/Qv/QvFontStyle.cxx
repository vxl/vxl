#include <QvFontStyle.h>

QV_NODE_SOURCE(QvFontStyle);

QvFontStyle::QvFontStyle()
{
    QV_NODE_CONSTRUCTOR(QvFontStyle);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(size);
    QV_NODE_ADD_FIELD(family);
    QV_NODE_ADD_FIELD(style);

    size.value = 10.0;
    family.value = SERIF;
    style.value = NONE;

    QV_NODE_DEFINE_ENUM_VALUE(Family, SERIF);
    QV_NODE_DEFINE_ENUM_VALUE(Family, SANS);
    QV_NODE_DEFINE_ENUM_VALUE(Family, TYPEWRITER);

    QV_NODE_DEFINE_ENUM_VALUE(Style, NONE);
    QV_NODE_DEFINE_ENUM_VALUE(Style, BOLD);
    QV_NODE_DEFINE_ENUM_VALUE(Style, ITALIC);

    QV_NODE_SET_SF_ENUM_TYPE(family, Family);
    QV_NODE_SET_SF_ENUM_TYPE(style,  Style);
}

QvFontStyle::~QvFontStyle()
{
}
