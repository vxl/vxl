#ifndef  _QV_WWW_INLINE_
#define  _QV_WWW_INLINE_

#include "QvSFEnum.h"
#include "QvSFVec3f.h"
#include "QvGroup.h"
#include "QvSFString.h"

class QvWWWInline : public QvGroup
{
  QV_NODE_HEADER(QvWWWInline);

  QvString            parentURL_;     // edgrossman, 19950720

public:
  // Fields
  QvSFString            name;           // URL name
  QvSFVec3f             bboxSize;       // Size of 3D bounding box
  QvSFVec3f             bboxCenter;     // Center of 3D bounding box

  // point3D min_, max_;  // mpichler, 19950607
  // mpichler, 19951006: object coordinate bounding box now in QvNode

  enum
  { s_virgin, s_requested, s_completed, s_failed
  } state_;                           // state of inline anchor request
};

#endif /* _QV_WWW_INLINE_ */
