#include <QvSFVec3f.h>

QV_SFIELD_SOURCE(QvSFVec3f);

QvBool
QvSFVec3f::readValue(QvInput *in)
{
    return (in->read(value[0]) &&
	    in->read(value[1]) &&
	    in->read(value[2]));
}
