#include <QvSFLong.h>

QV_SFIELD_SOURCE(QvSFLong);

QvBool
QvSFLong::readValue(QvInput *in)
{
    return in->read(value);
}
