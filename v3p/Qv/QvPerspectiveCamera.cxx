#include "QvPerspectiveCamera.h"
#include "QvSwitch.h"
#include "Qv_pi.h" /* for QV_PI_4 */
//#include <vcl_cmath.h>  /* mpichler, 19950616 */

QV_NODE_SOURCE(QvPerspectiveCamera);

QvPerspectiveCamera::QvPerspectiveCamera()
{
    QV_NODE_CONSTRUCTOR(QvPerspectiveCamera);
    isBuiltIn = TRUE;

    QV_NODE_ADD_FIELD(position);
    QV_NODE_ADD_FIELD(orientation);
    QV_NODE_ADD_FIELD(focalDistance);
    QV_NODE_ADD_FIELD(heightAngle);

    // mpichler, 19950713
    QV_NODE_ADD_FIELD(nearDistance);
    QV_NODE_ADD_FIELD(farDistance);
    nearDistance.value = -1;
    farDistance.value = -1;

    position.value[0] = 0.0;
    position.value[1] = 0.0;
    position.value[2] = 1.0;
    orientation.axis[0] = 0.0;
    orientation.axis[1] = 0.0;
    orientation.axis[2] = 1.0;
    orientation.angle = 0.0;
    focalDistance.value = 5.0;
    heightAngle.value = (float)QV_PI_4; // 45 degrees

    // mpichler, 19951024
    registered_ = 0;
    camswitch_ = 0;
}

QvPerspectiveCamera::~QvPerspectiveCamera()
{
}

// mpichler, 19951024
void QvPerspectiveCamera::switchto ()
{
  if (camswitch_)
    camswitch_->whichChild.value = camswindex_;
}
