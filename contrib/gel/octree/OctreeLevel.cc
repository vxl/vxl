//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "OctreeLevel.h"
#endif
//
// Class: OctreeLevel
// Author: Geoffrey Cross, Oxford RRG
// Created: 11 May 99
// Modifications:
//   990511 Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>

#include "OctreeLevel.h"

// Default ctor
OctreeLevel::OctreeLevel( const int d= 0) :
  depth( d),
  size( int(pow(2,depth))),
  color( size, size, size),
  next( NULL),
  prev( NULL)
{
  cout << "Size = " << size << endl;
}


OctreeLevel::OctreeLevel( OctreeLevel *p) :
  depth( p->GetDepth()+1),
  size( int(pow(2,depth))),
  color( size, size, size),
  next( NULL),
  prev( p)
{
  prev->SetNext( this);
  cout << "Size = " << size << endl;
}

void Octree::InheritFromAbove()
{
  if( !prev)
    {
      
    }
  else
    {

    }


}

