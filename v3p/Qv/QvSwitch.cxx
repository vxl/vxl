#include <QvSwitch.h>

QV_NODE_SOURCE(QvSwitch);

QvSwitch::QvSwitch()
{
    QV_NODE_CONSTRUCTOR(QvSwitch);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(whichChild);

    whichChild.value = QV_SWITCH_NONE;
}

QvSwitch::~QvSwitch()
{
}
