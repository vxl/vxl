// This is gel/octree/VoxmapPoints.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author
// Author: Geoffrey Cross, Oxford RRG
// Created: 17 May 99
// Modifications:
//   990517 Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include "VoxmapPoints.h"

// Default ctor
VoxmapPoints::VoxmapPoints( int d, vnl_double_3 c, double s)
  : depth(d),
    nocorners( int(1<<depth)+1),
    nocentres( int(1<<depth)),
    centre(c),
    size(s)
    //    corners( nocorners, nocorners, nocorners),
    //    centres( nocentres, nocentres, nocentres)
{
}


vnl_double_3 VoxmapPoints::GetCentre( int x, int y, int z, int d) const
{
  if (d!= depth)
    return GetCorner( x*2,y*2,z*2,1,1,1,d+1);

  if (centres.fullp(x,y,z))
    return centres(x,y,z);

  vnl_double_3 np( centre[0]-size/2+size/nocentres*(x+0.5),
                   centre[1]-size/2+size/nocentres*(y+0.5),
                   centre[2]-size/2+size/nocentres*(z+0.5));

  centres.put(x,y,z,np);

  return np;
}


vnl_double_3 VoxmapPoints::GetCorner( int x, int y, int z, int dx, int dy, int dz, int d) const
{
  int dd= 1<<(depth-d);

  int ix= (x+dx)*dd;
  int iy= (y+dy)*dd;
  int iz= (z+dz)*dd;

  if (corners.fullp(ix,iy,iz))
    return cornerpoints[corners(ix,iy,iz)];

  vnl_double_3 np( centre[0]-size/2+(ix)*size/(nocorners-1),
                   centre[1]-size/2+(iy)*size/(nocorners-1),
                   centre[2]-size/2+(iz)*size/(nocorners-1));

  corners.put(ix,iy,iz,cornerpoints.size());
  cornerpoints.push_back( np);

  return np;
}


int VoxmapPoints::GetCornerIndex( int x, int y, int z, int dx, int dy, int dz, int d) const
{
  int dd= 1<<(depth-d);

  int ix= (x+dx)*dd;
  int iy= (y+dy)*dd;
  int iz= (z+dz)*dd;

  if (corners.fullp(ix,iy,iz))
    return corners(ix,iy,iz);

  vnl_double_3 np( centre[0]-size/2+(ix)*size/(nocorners-1),
                   centre[1]-size/2+(iy)*size/(nocorners-1),
                   centre[2]-size/2+(iz)*size/(nocorners-1));

  corners.put(ix,iy,iz,cornerpoints.size());
  cornerpoints.push_back( np);

  return cornerpoints.size()-1;
}

#include <vbl/vbl_sparse_array_3d.txx>
VBL_SPARSE_ARRAY_3D_INSTANTIATE(vnl_double_3);
