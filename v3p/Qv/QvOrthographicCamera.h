#ifndef  _QV_ORTHOGRAPHIC_CAMERA_
#define  _QV_ORTHOGRAPHIC_CAMERA_

#include "QvSFFloat.h"
#include "QvSFRotation.h"
#include "QvSFVec3f.h"
#include "QvSubNode.h"

class QvSwitch;
#include "vectors.h"

class QvOrthographicCamera : public QvNode
{
  QV_NODE_HEADER(QvOrthographicCamera);

public:
  void switchto ();

public:
  QvSFVec3f     position;       // Location of viewpoint
  QvSFRotation  orientation;    // Orientation (rotation with respect to (0,0,-1) vector)
  QvSFFloat     focalDistance;  // Distance from viewpoint to point of focus.
  QvSFFloat     height;         // Height of view volume

  // non-standard VRML fields (ignored); mpichler, 19950713
  QvSFFloat     nearDistance;   // near clipping plane
  QvSFFloat     farDistance;    // far clipping plane

  // mpichler, 19950504
  const point3D* pos_;          // position
  float rotangle_;              // rotation angle (degrees)
  const vector3D* rotaxis_;     //   and axis
  float height_;                // vertical window size
  // mpichler, 19951012
  matrix4D mat_, invmat_;       // modelview transform and inverse
  // mpichler, 19951024
  int registered_;              // put in viewpoint list
  QvSwitch* camswitch_;         // camera switch node
  int camswindex_;              // according index
};

#endif /* _QV_ORTHOGRAPHIC_CAMERA_ */
