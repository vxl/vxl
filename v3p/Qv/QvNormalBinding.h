#ifndef  _QV_NORMAL_BINDING_
#define  _QV_NORMAL_BINDING_

#include "QvSFEnum.h"
#include "QvSubNode.h"

class QvNormalBinding : public QvNode
{
  QV_NODE_HEADER(QvNormalBinding);

public:
  enum Binding {
    DEFAULT,
    OVERALL,
    PER_PART,
    PER_PART_INDEXED,
    PER_FACE,
    PER_FACE_INDEXED,
    PER_VERTEX,
    PER_VERTEX_INDEXED
  };

  // Fields
  QvSFEnum value; // Normal binding value
};

#endif /* _QV_NORMAL_BINDING_ */
