#include <QvUnknownNode.h>

QV_NODE_SOURCE(QvUnknownNode);

QvUnknownNode::QvUnknownNode()
{
    QV_NODE_CONSTRUCTOR(QvUnknownNode);

    className = NULL;

    // Set global field data to this instance's
    if (QV_NODE_IS_FIRST_INSTANCE())
	delete fieldData;
    instanceFieldData = new QvFieldData;
    fieldData = instanceFieldData;
}

void
QvUnknownNode::setClassName(const char *name)
{
    className = strdup(name);
}

QvUnknownNode::~QvUnknownNode()
{
    for (int i = 0; i < instanceFieldData->getNumFields(); i++)
	delete instanceFieldData->getField(this, i);

    delete instanceFieldData;

    if (className != NULL)
	free((void *) className);
}
