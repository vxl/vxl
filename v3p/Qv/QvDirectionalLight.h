#ifndef  _QV_DIRECTIONAL_LIGHT_
#define  _QV_DIRECTIONAL_LIGHT_

#include "QvSFBool.h"
#include "QvSFColor.h"
#include "QvSFFloat.h"
#include "QvSFVec3f.h"
#include "QvSubNode.h"

#include "vectors.h"
#include "color.h"

class QvDirectionalLight : public QvNode
{
  QV_NODE_HEADER(QvDirectionalLight);

public:
  // Fields
  QvSFBool     on;         // Whether light is on
  QvSFFloat    intensity;  // Source intensity (0 to 1)
  QvSFColor    color;      // RGB source color
  QvSFVec3f    direction;  // Illumination direction vector

  // mpichler, 19950523
  colorRGB color_;         // color, multiplied by intensity
  point3D direction_;      // direction vector (*towards* light source)
};

#endif /* _QV_DIRECTIONAL_LIGHT_ */
