#include "QvOrthographicCamera.h"
#include "QvSwitch.h"

QV_NODE_SOURCE(QvOrthographicCamera);

QvOrthographicCamera::QvOrthographicCamera()
{
  QV_NODE_CONSTRUCTOR(QvOrthographicCamera);
  isBuiltIn = TRUE;

  QV_NODE_ADD_FIELD(position);
  QV_NODE_ADD_FIELD(orientation);
  QV_NODE_ADD_FIELD(focalDistance);
  QV_NODE_ADD_FIELD(height);

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
  height.value = 2.0;

  // mpichler, 19951024
  registered_ = 0;
  camswitch_ = 0;
}

QvOrthographicCamera::~QvOrthographicCamera()
{
}


// mpichler, 19951024
void QvOrthographicCamera::switchto ()
{
  if (camswitch_)
    camswitch_->whichChild.value = camswindex_;
}
