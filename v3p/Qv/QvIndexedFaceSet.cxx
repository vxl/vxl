#include "QvIndexedFaceSet.h"

QV_NODE_SOURCE(QvIndexedFaceSet)

QvIndexedFaceSet::QvIndexedFaceSet()
{
  QV_NODE_CONSTRUCTOR(QvIndexedFaceSet);
  isBuiltIn = TRUE;

  // field declaration for parser
  QV_NODE_ADD_FIELD(coordIndex);
  QV_NODE_ADD_FIELD(materialIndex);
  QV_NODE_ADD_FIELD(normalIndex);
  QV_NODE_ADD_FIELD(textureCoordIndex);

  coordIndex.values[0] = 0;
  materialIndex.values[0] = QV_END_FACE_INDEX;
  normalIndex.values[0] = QV_END_FACE_INDEX;
  textureCoordIndex.values[0] = QV_END_FACE_INDEX;

  // each QvMF* is created with 1 element (see QvSubField.h)
  convexCoordIndex.values[0] = QV_END_FACE_INDEX;  // rpersche, 19960410
  convexFaceNormals_.values[0] = 0;                // rpersche, 19960603
  normalList_.values[0] = 0;                       // pzemljic, 19960519

  numconvexinds_ = 0;
  facenormals_ = 0;  // mpichler, 19950524
  numnormalinds_ = 0;  // mpichler, 19960617
}

QvIndexedFaceSet::~QvIndexedFaceSet()
{
  delete[] facenormals_;  // mpichler, 19950524
  facenormals_ = 0;  // mpichler, 19950524
}
