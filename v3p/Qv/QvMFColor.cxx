#include <QvMFColor.h>

QV_MFIELD_SOURCE(QvMFColor, float, 3, FALSE);

QvBool
QvMFColor::read1Value(QvInput *in, int index)
{
    float *valuePtr = values + index * 3;

    return (in->read(valuePtr[0]) &&
	    in->read(valuePtr[1]) &&
	    in->read(valuePtr[2]));
}
