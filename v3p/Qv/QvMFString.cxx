#include "QvMFString.h"
#include <vcl_cstdlib.h>

QV_MFIELD_SOURCE(QvMFString, QvString, 1, TRUE);

QvBool
QvMFString::read1Value(QvInput *in, int index)
{
    return in->read(values[index]);
}
