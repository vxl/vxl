#include "QvWWWAnchor.h"

QV_NODE_SOURCE(QvWWWAnchor);

QvWWWAnchor::QvWWWAnchor()
{
    QV_NODE_CONSTRUCTOR(QvWWWAnchor);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(name);
    QV_NODE_ADD_FIELD(description);
    QV_NODE_ADD_FIELD(map);
    QV_NODE_ADD_FIELD(target);

    name.value = "";
    description.value = "";
    map.value = NONE;
    target.value = "";

    QV_NODE_DEFINE_ENUM_VALUE(Map, NONE);
    QV_NODE_DEFINE_ENUM_VALUE(Map, POINT);

    QV_NODE_SET_SF_ENUM_TYPE(map, Map);

    // if anchor should ever be activated on other ways than clicking
    init3D (hitpoint_, 0, 0, 0);  // mpichler, 19951122
    hganchorid_ = 0;  // non-0 if Hyper-G anchor; mpichler, 19960604
}

QvWWWAnchor::~QvWWWAnchor()
{
}
