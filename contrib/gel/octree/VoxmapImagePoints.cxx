//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "VoxmapImagePoints.h"
#endif
//
// Class: VoxmapImagePoints
// Author: Geoffrey Cross, Oxford RRG
// Created: 20 May 99
// Modifications:
//   990520 Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>

#include "VoxmapImagePoints.h"

// Default ctor
VoxmapImagePoints::VoxmapImagePoints( int d, vnl_double_3 c, double s, int ims)
  : VoxmapPoints( d, c, s),
    pmatrices( new (PMatrix *)[ims]),
    imagecorners( new (BigSparseArray3D<vnl_double_2 *> *)[ims]),
    imagecentres( new (BigSparseArray3D<vnl_double_2 *> *)[ims])
{
}

void VoxmapImagePoints::SetPMatrix( PMatrix &P, int im)
{
  pmatrices[im]   = new PMatrix(P);
  imagecorners[im]= new BigSparseArray3D<vnl_double_2 *>(nocorners,nocorners,nocorners);
  imagecentres[im]= new BigSparseArray3D<vnl_double_2 *>(nocentres,nocentres,nocentres);
}

vnl_double_2 *VoxmapImagePoints::GetCentreImage( int x, int y, int z, int d, int im)
{
  if( d!= depth)
    return GetCornerImage( x*2,y*2,z*2,1,1,1,d+1,im);

  if( imagecentres[im]->fullp(x,y,z))
    return (*imagecentres[im])(x,y,z);

  vnl_double_3     *p= GetCentre( x,y,z,d);
  HomgPoint2D ip( pmatrices[im]->project( HomgPoint3D( (*p)[0], (*p)[1], (*p)[2])));
  vnl_double_2    *np= new vnl_double_2( ip.x()/ip.w(), ip.y()/ip.w());

  imagecentres[im]->put(x,y,z,np);

  return np;
}

vnl_double_2 *VoxmapImagePoints::GetCornerImage( int x, int y, int z, int dx, int dy, int dz, int d, int im)
{
  //  cout << x << " " << y << " " << z << endl;

  //  if( d!= depth)
  //    return GetCornerImage( (x+dx)*2, (y+dy)*2, (z+dz)*2, 0, 0, 0, d+1, im);

  int dd= 1<<(depth-d);

  int ix= (x+dx)*dd;
  int iy= (y+dy)*dd;
  int iz= (z+dz)*dd;

  if( imagecorners[im]->fullp(ix,iy,iz))
    return (*imagecorners[im])(ix,iy,iz);
  
  vnl_double_3     *p= GetCorner( x,y,z,dx,dy,dz,d);
  HomgPoint2D ip( pmatrices[im]->project( HomgPoint3D( (*p)[0], (*p)[1], (*p)[2])));
  vnl_double_2    *np= new vnl_double_2( ip.x()/ip.w(), ip.y()/ip.w());

  imagecorners[im]->put(ix,iy,iz,np);

  return np;
}
