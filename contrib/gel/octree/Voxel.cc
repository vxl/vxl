//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "Voxel.h"
#endif
//
// Class: Voxel
// Author: Geoffrey Cross, Oxford RRG
// Created: 20 May 99
// Modifications:
//   990520 Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include <assert.h>
#include "Voxel.h"


// bool operator==( const Voxel &a, const Voxel &b) 
// { 
//   return a==b; 
// }


ostream &operator<<( ostream &os, const Voxel &a)
{
  return os << a.depth << " " << a.x << " " << a.y << " " << a.z;
}

bool operator<( Voxel &v1, Voxel &v2) 
{ 
  assert(0); 
  return false; 
}
