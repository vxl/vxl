#include <QvSFColor.h>

QV_SFIELD_SOURCE(QvSFColor);

QvBool
QvSFColor::readValue(QvInput *in)
{
    return (in->read(value[0]) &&
	    in->read(value[1]) &&
	    in->read(value[2]));
}
