//-*- c++ -*-------------------------------------------------------------------
#ifndef Voxel_h_
#define Voxel_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : Voxel
//
// .SECTION Description
//    Voxel is a class that Geoff hasn't documented properly. FIXME
//
// .NAME        Voxel - Undocumented class FIXME
// .LIBRARY     new
// .HEADER	octree Package
// .INCLUDE     new/Voxel.h
// .FILE        Voxel.h
// .FILE        Voxel.C
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 20 May 99
//
//-----------------------------------------------------------------------------


#include <vcl/vcl_iostream.h>

struct Voxel {

  Voxel( int d= 0, int xx= 0, int yx= 0, int zx= 0) : depth(d), x(xx), y(yx), z(zx) {} 

  unsigned char depth;
  unsigned char x;
  unsigned char y;
  unsigned char z;

  bool operator==( const Voxel &a) const { return ((a.depth==depth)&&(a.x==x)&&(a.y==y)&&(a.z==z)); }

};

//bool operator==( const Voxel &a, const Voxel &b);
ostream &operator<<( ostream &os, const Voxel &a);

bool operator<( Voxel &v1, Voxel &v2);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS Voxel.

