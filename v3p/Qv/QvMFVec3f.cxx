#include <QvMFVec3f.h>

QV_MFIELD_SOURCE(QvMFVec3f, float, 3, FALSE);

QvBool
QvMFVec3f::read1Value(QvInput *in, int index)
{
    float *valuePtr = values + index * 3;

    return (in->read(valuePtr[0]) &&
	    in->read(valuePtr[1]) &&
	    in->read(valuePtr[2]));
}
