#include "QvAsciiText.h"

QV_NODE_SOURCE(QvAsciiText);

QvAsciiText::QvAsciiText()
{
  QV_NODE_CONSTRUCTOR(QvAsciiText);
  isBuiltIn = TRUE;

  QV_NODE_ADD_FIELD(string);
  QV_NODE_ADD_FIELD(spacing);
  QV_NODE_ADD_FIELD(justification);
  QV_NODE_ADD_FIELD(width);

  string.values[0] = "";
  spacing.value = 1.0;
  justification.value = LEFT;
  width.values[0] = 0;

  QV_NODE_DEFINE_ENUM_VALUE(Justification, LEFT);
  QV_NODE_DEFINE_ENUM_VALUE(Justification, CENTER);
  QV_NODE_DEFINE_ENUM_VALUE(Justification, RIGHT);

  QV_NODE_SET_SF_ENUM_TYPE(justification, Justification);

  // mpichler, 19960621
  requested_ = 0;
  fontchars_ = 0;  // managed by VRMLScene
  roworigin_ = 0;
  rowmax_ = 0;
  rowhspc_ = 0;
}

QvAsciiText::~QvAsciiText()
{
  delete[] roworigin_;
  delete[] rowmax_;
  delete[] rowhspc_;
}
