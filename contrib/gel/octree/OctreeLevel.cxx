// This is gel/octree/OctreeLevel.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
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

#include "OctreeLevel.h"
#include <vcl_iostream.h>

// Default ctor
OctreeLevel::OctreeLevel( const int d) :
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

void OctreeLevel::InheritFromAbove()
{
  if (!prev)
    {
      // TODO
    }
  else
    {
      // TODO
    }
}

