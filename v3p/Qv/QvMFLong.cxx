#include "QvMFLong.h"

#ifdef __osf__
QV_MFIELD_SOURCE(QvMFLong, int, 1, FALSE)  /* mpichler, 19950622 */
#else
QV_MFIELD_SOURCE(QvMFLong, long, 1, FALSE);
#endif

QvBool
QvMFLong::read1Value(QvInput *in, int index)
{
    return in->read(values[index]);
}
