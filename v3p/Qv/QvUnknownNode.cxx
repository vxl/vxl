#include "QvUnknownNode.h"
#include <vcl_cstdlib.h>

QV_NODE_SOURCE(QvUnknownNode);

QvUnknownNode::QvUnknownNode()
{
  QV_NODE_CONSTRUCTOR(QvUnknownNode);

  className = NULL;

  // Set global field data to this instance's
  if (QV_NODE_IS_FIRST_INSTANCE())
    delete fieldData;
  fieldData = instanceFieldData = new QvFieldData;
}

void
QvUnknownNode::setClassName(const char *name)
{
  className = Qv_strdup(name);
}

QvUnknownNode::~QvUnknownNode()
{
  for (int i = 0; i < instanceFieldData->getNumFields(); i++)
    delete instanceFieldData->getField(this, i);

  delete instanceFieldData;

  if (className != NULL)
    vcl_free((void*)className);
}
