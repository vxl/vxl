#include <QvMFFloat.h>

QV_MFIELD_SOURCE(QvMFFloat, float, 1, FALSE);

QvBool
QvMFFloat::read1Value(QvInput *in, int index)
{
    return in->read(values[index]);
}
