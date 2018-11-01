// This is gel/octree/Voxel.h
#ifndef Voxel_h_
#define Voxel_h_
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 20 May 99
//
//-----------------------------------------------------------------------------


#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct Voxel
{
  Voxel( unsigned char d= 0, unsigned char xx= 0, unsigned char yx= 0, unsigned char zx= 0) : depth(d), x(xx), y(yx), z(zx) {}

  unsigned char depth;
  unsigned char x;
  unsigned char y;
  unsigned char z;

  bool operator==( const Voxel &a) const { return a.depth==depth && a.x==x && a.y==y && a.z==z; }
};

#if 0
bool operator==( const Voxel &a, const Voxel &b) { return a.depth==b.depth && a.x==b.x && a.y==b.y && a.z==b.z; }
#endif // 0

std::ostream &operator<<( std::ostream &os, const Voxel &a);

bool operator<( Voxel &v1, Voxel &v2);

#endif // Voxel_h_
