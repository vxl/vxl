#include "QvGroup.h"
#include "QvInput.h"
#include "QvReadError.h"
#include "QvFieldData.h"
#include "QvChildList.h"

QV_NODE_SOURCE(QvGroup);

QvGroup::QvGroup()
{
    children = new QvChildList();
    QV_NODE_CONSTRUCTOR(QvGroup);
    isBuiltIn = TRUE;
}

QvGroup::~QvGroup()
{
    delete children;
}

QvNode *
QvGroup::getChild(int index) const
{
    return(*children)[index];
}

int
QvGroup::getNumChildren() const
{
    return children->getLength();
}

QvChildList *
QvGroup::getChildren() const
{
    return children;
}

QvBool
QvGroup::readInstance(QvInput *in)
{
    QvName      typeString;
    QvFieldData *fieldData_auto = getFieldData();

    if (! isBuiltIn) {
        if (in->read(typeString, TRUE)) {
            if (typeString == "fields") {
                if (! fieldData_auto->readFieldTypes(in, this)) {
                    QvReadError::post(in, "Bad field specifications for node");
                    return FALSE;
                }
            }
            else
                in->putBack(typeString.getString());
        }
    }

    return fieldData_auto->read(in, this, FALSE) && readChildren(in);
}

QvBool
QvGroup::readChildren(QvInput *in)
{
    QvNode      *child;
    QvBool      ret = TRUE;

    while (TRUE) {
        if (read(in, child)) {
            if (child != NULL)
                children->append(child);
            else
                break;
        }
        else {
// mpichler, 19950711: continue on bad children
// should work because QvNode::readNode cleans up after wrong nodes
//          ret = FALSE;
//          break;
        }
    }

    return ret;
}
