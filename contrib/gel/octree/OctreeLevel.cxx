//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//:
// \file
// \author
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
  size( d<0 ? 0 : (1<<d)),
  color( size, size, size),
  next( NULL),
  prev( NULL)
{
  vcl_cout << "Size = " << size << vcl_endl;
}


OctreeLevel::OctreeLevel( OctreeLevel *p) :
  depth( p->GetDepth()+1),
  size( 1<<depth),
  color( size, size, size),
  next( NULL),
  prev( p)
{
  prev->SetNext( this);
  vcl_cout << "Size = " << size << vcl_endl;
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

