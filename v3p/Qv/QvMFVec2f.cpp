#include <QvMFVec2f.h>

QV_MFIELD_SOURCE(QvMFVec2f, float, 2, FALSE);

QvBool
QvMFVec2f::read1Value(QvInput *in, int index)
{
    float *valuePtr = values + index * 2;

    return (in->read(valuePtr[0]) &&
	    in->read(valuePtr[1]));
}
