#include <QvSFRotation.h>

QV_SFIELD_SOURCE(QvSFRotation);

QvBool
QvSFRotation::readValue(QvInput *in)
{
    return (in->read(axis[0]) &&
	    in->read(axis[1]) &&
	    in->read(axis[2]) &&
	    in->read(angle));
}
