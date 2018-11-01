// This is gel/octree/VoxmapImagePoints.h
#ifndef VoxmapImagePoints_h_
#define VoxmapImagePoints_h_
//:
// \file
// \author  Geoffrey Cross, Oxford RRG, 20 May 99
//-----------------------------------------------------------------------------

#include <vbl/vbl_sparse_array_3d.h>
#include <vnl/vnl_double_2.h>
#include <mvl/PMatrix.h>

#include "VoxmapPoints.h"

class VoxmapImagePoints : public VoxmapPoints
{
 public:
  // Constructors/Destructors--------------------------------------------------

  VoxmapImagePoints( int, vnl_double_3, double, int);

  // Operations----------------------------------------------------------------

  void SetPMatrix( PMatrix &P, int im);

  // Computations--------------------------------------------------------------

  vnl_double_2 GetCentreImage( int x, int y, int z, int d, int im) const;
  vnl_double_2 GetCornerImage( int x, int y, int z, int dx, int dy, int dz, int d, int im) const;

 protected:
  // Data Members--------------------------------------------------------------

  PMatrix                  **pmatrices;
  vbl_sparse_array_3d<vnl_double_2> **imagecorners;
  vbl_sparse_array_3d<vnl_double_2> **imagecentres;
};

#endif // VoxmapImagePoints_h_
