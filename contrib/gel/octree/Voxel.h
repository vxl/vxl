// This is gel/octree/Voxel.h
#ifndef Voxel_h_
#define Voxel_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 20 May 99
//
//-----------------------------------------------------------------------------


#include <vcl_iosfwd.h>

struct Voxel
{
  Voxel( int d= 0, int xx= 0, int yx= 0, int zx= 0) : depth(d), x(xx), y(yx), z(zx) {}

  unsigned char depth;
  unsigned char x;
  unsigned char y;
  unsigned char z;

  bool operator==( const Voxel &a) const { return a.depth==depth && a.x==x && a.y==y && a.z==z; }
};

//bool operator==( const Voxel &a, const Voxel &b);
vcl_ostream &operator<<( vcl_ostream &os, const Voxel &a);

bool operator<( Voxel &v1, Voxel &v2);

#endif // Voxel_h_
