#ifndef  _QV_SPOT_LIGHT_
#define  _QV_SPOT_LIGHT_

#include "QvSFBool.h"
#include "QvSFColor.h"
#include "QvSFFloat.h"
#include "QvSFVec3f.h"
#include "QvSubNode.h"

#include "vectors.h"
#include "color.h"

class QvSpotLight : public QvNode
{
  QV_NODE_HEADER(QvSpotLight);

public:
  // Fields:
  QvSFBool      on;             // Whether light is on
  QvSFFloat     intensity;      // Source intensity (0 to 1)
  QvSFColor     color;          // RGB source color
  QvSFVec3f     location;       // Source location
  QvSFVec3f     direction;      // Primary direction of illumination
  QvSFFloat     dropOffRate;    // Rate of intensity drop-off from primary direction:
                                // 0 = constant intensity, 1 = sharp drop-off
  QvSFFloat     cutOffAngle;    // Angle (in radians) outside of which intensity is zero,
                                // measured from edge of cone to other edge

  // mpichler, 19950523
  colorRGB      color_;         // color, multiplied by intensity
  const point3D* position_;     // position of light source
  const vector3D* direction_;   // spot direction (from light away)
  float         cutangle_;      // cut off angle in degrees
};

#endif /* _QV_SPOT_LIGHT_ */
