#ifndef  _QV_POINT_LIGHT_
#define  _QV_POINT_LIGHT_

#include "QvSFBool.h"
#include "QvSFColor.h"
#include "QvSFFloat.h"
#include "QvSFVec3f.h"
#include "QvSubNode.h"

#include "vectors.h"
#include "color.h"

class QvPointLight : public QvNode
{
  QV_NODE_HEADER(QvPointLight);

public:
  // Fields
  QvSFBool       on;        // Whether light is on
  QvSFFloat      intensity; // Source intensity (0 to 1)
  QvSFColor      color;     // RGB source color
  QvSFVec3f      location;  // Source location

  // mpichler, 19950523
  colorRGB color_;          // color, multiplied by intensity
  const point3D* position_; // position of light source
};

#endif /* _QV_POINT_LIGHT_ */
