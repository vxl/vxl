// This is gel/octree/VoxmapPoints.h
#ifndef VoxmapPoints_h_
#define VoxmapPoints_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
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

class VoxmapPoints
{
 public:
  // Constructors/Destructors--------------------------------------------------

  VoxmapPoints( int, vnl_double_3, double);

  // Data Access---------------------------------------------------------------

  vnl_double_3 GetVoxmapCentre() const { return centre; }
  double  GetVoxmapSize() const { return size; }

  void SetVoxmapCentre( vnl_double_3 c) { centre= c; }
  void SetVoxmapSize( double s) { size= s; }

  // Data Control--------------------------------------------------------------

  vnl_double_3 GetCentre( int x, int y, int z, int d) const;
  vnl_double_3 GetCorner( int x, int y, int z, int dx, int dy, int dz, int d) const;
  int GetCornerIndex( int x, int y, int z, int dx, int dy, int dz, int d) const;

 protected:
  // Data Members--------------------------------------------------------------

  int depth;
  int nocorners;
  int nocentres;

  vnl_double_3 centre;
  double  size;

 private:

  mutable vcl_vector<vnl_double_3> cornerpoints;
  mutable vbl_sparse_array_3d<int> corners;
  mutable vbl_sparse_array_3d<vnl_double_3> centres;
};

#endif // VoxmapPoints_h_
