#include "QvWWWInline.h"

QV_NODE_SOURCE(QvWWWInline)

QvWWWInline::QvWWWInline()
{
    QV_NODE_CONSTRUCTOR(QvWWWInline);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(name);
    QV_NODE_ADD_FIELD(bboxSize);
    QV_NODE_ADD_FIELD(bboxCenter);

    name.value = "";
    // mpichler, 19950713: bug in initialisation fixed
    bboxSize.value[0] = bboxSize.value[1] = bboxSize.value[2] = 0.0;
    bboxCenter.value[0] = bboxCenter.value[1] = bboxCenter.value[2] = 0.0;

  // mpichler, 19950608
  state_ = s_virgin;
}

QvWWWInline::~QvWWWInline()
{
}
