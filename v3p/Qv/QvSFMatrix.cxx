#include <QvSFMatrix.h>

QV_SFIELD_SOURCE(QvSFMatrix);

QvBool
QvSFMatrix::readValue(QvInput *in)
{
    return  (in->read(value[0][0]) && in->read(value[0][1]) &&
	     in->read(value[0][2]) && in->read(value[0][3]) &&
	     in->read(value[1][0]) && in->read(value[1][1]) &&
	     in->read(value[1][2]) && in->read(value[1][3]) &&
	     in->read(value[2][0]) && in->read(value[2][1]) &&
	     in->read(value[2][2]) && in->read(value[2][3]) &&
	     in->read(value[3][0]) && in->read(value[3][1]) &&
	     in->read(value[3][2]) && in->read(value[3][3]));
}
