#include "QvMFLong.h"

QV_MFIELD_SOURCE(QvMFLong, vxl_int_32, 1, FALSE)  /* mpichler, 19950622 */

QvBool
QvMFLong::read1Value(QvInput *in, int index)
{
    return in->read(values[index]);
}
