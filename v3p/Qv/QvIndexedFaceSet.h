#ifndef  _QV_INDEXED_FACE_SET_
#define  _QV_INDEXED_FACE_SET_

#include "QvMFLong.h"
#include "QvSubNode.h"
#include "QvMFVec3f.h"  /* rpersche 19960519 */

#include "vectors.h"
#include "mtl.h"

#define QV_END_FACE_INDEX (-1)

class QvIndexedFaceSet : public QvNode
{
  QV_NODE_HEADER(QvIndexedFaceSet);

public:
  // Fields:
  QvMFLong  coordIndex;               // Coordinate indices
  QvMFLong  materialIndex;            // Material indices
  QvMFLong  normalIndex;              // Surface normal indices
  QvMFLong  textureCoordIndex;        // Texture Coordinate indices

  // mpichler, 19950508, 19950630
  const point3D* vertexlist_;         // vertex data
  int numvertinds_;                   // no. of vertex indices
  const int* vertindices_;            // vertex index list

  // Georg Meszaros, 19960310
  int numvertices_;                   // number of vertices in vertexlist

  QvMFLong convexCoordIndex;          // Coordinate indices of convex version
  int numconvexinds_;                 // no. of vertex indices after triangulation

  // rpersche, 19960603
  QvMFVec3f convexFaceNormals_;       // face normals after the triangulation

  // pzemljic, 19960519
  QvMFVec3f normalList_;              // automatically generated vertex normals

  const materialsGE3D* materials_;    // material data
  int matbinding_;                    // material binding (as defined in ge3d/mtl.h)
  int nummatinds_;                    // no. of material indices
  const int* matindices_;             // material index list (for _INDEXED binding)

  const vector3D* normallist_;        // normal vector data
//int normalbinding_;                 // TODO: normal binding
  int numnormalinds_;                 // no. of normal indices
  const int* normalindices_;          // normal index list

  point3D* facenormals_;              // automatically generated face normals
                                      // for flat shading; allocated

  const point2D* texvertlist_;        // texture vertices
  // no texture binding
  int numtextureinds_;                // no. of texture indices
  const int* textureindices_;         // texture index list
};

#endif /* _QV_INDEXED_FACE_SET_ */
