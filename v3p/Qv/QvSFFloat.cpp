#include <QvSFFloat.h>

QV_SFIELD_SOURCE(QvSFFloat);

QvBool
QvSFFloat::readValue(QvInput *in)
{
    return in->read(value);
}
