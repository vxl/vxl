#ifndef  _QV_SHAPE_HINTS_
#define  _QV_SHAPE_HINTS_

#include "QvSFEnum.h"
#include "QvSFFloat.h"
#include "QvSubNode.h"

class QvShapeHints : public QvNode
{
  QV_NODE_HEADER(QvShapeHints);

public:
  enum VertexOrdering {
    UNKNOWN_ORDERING,
    CLOCKWISE,
    COUNTERCLOCKWISE
  };

  enum ShapeType {
    UNKNOWN_SHAPE_TYPE,
    SOLID
  };

  enum FaceType {
    UNKNOWN_FACE_TYPE,
    CONVEX
  };

  // Fields
  QvSFEnum   vertexOrdering; // Ordering of face vertices
  QvSFEnum   shapeType;      // Info about shape geometry
  QvSFEnum   faceType;       // Info about face geometry
  QvSFFloat  creaseAngle;    // Smallest angle for sharp edge

  // mpichler, 19950706
  int backfaceculling_;  // for solid objects with CCW ordered vertices
};

#endif /* _QV_SHAPE_HINTS_ */
