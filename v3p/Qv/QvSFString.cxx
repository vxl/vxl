#include <QvSFString.h>

QV_SFIELD_SOURCE(QvSFString);

QvBool
QvSFString::readValue(QvInput *in)
{
    return in->read(value);
}
