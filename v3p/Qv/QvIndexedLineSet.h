#ifndef  _QV_INDEXED_LINE_SET_
#define  _QV_INDEXED_LINE_SET_

#include "QvMFLong.h"
#include "QvSubNode.h"

#include "vectors.h"

#define QV_END_LINE_INDEX (-1)

class QvIndexedLineSet : public QvNode
{
  QV_NODE_HEADER(QvIndexedLineSet);

public:
  // Fields:
  QvMFLong     coordIndex;             // Coordinate indices
  QvMFLong     materialIndex;          // Material indices
  QvMFLong     normalIndex;            // Surline normal indices
  QvMFLong     textureCoordIndex;      // Texture Coordinate indices

  // mpichler, 19950502
  const point3D* vertexlist_;          // vertex data
  int numvertinds_;                    // no. of vertex indices
  const int* vertindices_;             // vertex index list

  // mpichler, 19951019
  float epsilon_;                      // picking tolerance
};

#endif /* _QV_INDEXED_LINE_SET_ */
