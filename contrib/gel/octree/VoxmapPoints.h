//-*- c++ -*-------------------------------------------------------------------
#ifndef VoxmapPoints_h_
#define VoxmapPoints_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 17 May 99
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_sparse_array_3d.h>
#include <vnl/vnl_double_3.h>
#include <vcl_vector.h>

#include "BigSparseArray3D.h"

class VoxmapPoints {
public:
  // Constructors/Destructors--------------------------------------------------

  VoxmapPoints( int, vnl_double_3, double);

  // Data Access---------------------------------------------------------------

  vnl_double_3 GetVoxmapCentre() { return centre; }
  double  GetVoxmapSize() { return size; }

  void SetVoxmapCentre( vnl_double_3 c) { centre= c; }
  void SetVoxmapSize( double s) { size= s; }

  // Data Control--------------------------------------------------------------

  vnl_double_3 *GetCentre( int x, int y, int z, int d);
  vnl_double_3 *GetCorner( int x, int y, int z, int dx, int dy, int dz, int d);
  int GetCornerIndex( int x, int y, int z, int dx, int dy, int dz, int d);

protected:
  // Data Members--------------------------------------------------------------

  int depth;
  int nocorners;
  int nocentres;

  vnl_double_3 centre;
  double  size;

private:

  vcl_vector<vnl_double_3 *> cornerpoints;
  vbl_sparse_array_3d<int> corners;
  vbl_sparse_array_3d<vnl_double_3 *> centres;
};

#endif // VoxmapPoints_h_
