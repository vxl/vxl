#ifndef  _QV_PERSPECTIVE_CAMERA_
#define  _QV_PERSPECTIVE_CAMERA_

#include "QvSFFloat.h"
#include "QvSFRotation.h"
#include "QvSFVec3f.h"
#include "QvSubNode.h"

class QvSwitch;
#include "vectors.h"

class QvPerspectiveCamera : public QvNode
{
  QV_NODE_HEADER(QvPerspectiveCamera);

public:
  void switchto ();

public:
  QvSFVec3f             position;       // Location of viewpoint
  QvSFRotation          orientation;    // Orientation (rotation with respect to (0,0,-1) vector)
  QvSFFloat             focalDistance;  // Distance from viewpoint to point of focus.
  QvSFFloat             heightAngle;    // Angle (in radians) of field of view, in height direction

  // non-standard VRML fields (ignored); mpichler, 19950713
  QvSFFloat             nearDistance;   // near clipping plane
  QvSFFloat             farDistance;    // far clipping plane

  // mpichler, 19950503
  const point3D* pos_;                  // position
  float rotangle_;                      // rotation angle (radians)
  const vector3D* rotaxis_;             //   and axis
  float yangle_;                        // vertical viewing angle (radians)
  // mpichler, 19951011
  matrix4D mat_, invmat_;               // modelview transform and inverse
  // mpichler, 19951024
  int registered_;                      // put in viewpoint list
  QvSwitch* camswitch_;                 // camera switch node
  int camswindex_;                      // according index
};

#endif // _QV_PERSPECTIVE_CAMERA_
