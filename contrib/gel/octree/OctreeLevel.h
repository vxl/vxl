//-*- c++ -*-------------------------------------------------------------------
#ifndef OctreeLevel_h_
#define OctreeLevel_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : OctreeLevel
//
// .SECTION Description
//    OctreeLevel is a class that Geoff hasn't documented properly. FIXME
//
// .NAME        OctreeLevel - Undocumented class FIXME
// .LIBRARY     new
// .HEADER	octree Package
// .INCLUDE     new/OctreeLevel.h
// .FILE        OctreeLevel.h
// .FILE        OctreeLevel.C
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 11 May 99
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_array_3d.h>

class OctreeLevel {
public:
  // Constructors/Destructors--------------------------------------------------
  
  OctreeLevel( const int = 0);
  OctreeLevel( OctreeLevel *p);

  // Operations----------------------------------------------------------------
  
  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  int GetDepth() const { return depth; }
  void SetNext( OctreeLevel *n) { next= n; }

  // Data Control--------------------------------------------------------------

  void Octree::InheritFromAbove();

protected:
  // Data Members--------------------------------------------------------------
  
  int depth;
  int size;
  vbl_array_3d<int> color;

  OctreeLevel *next;
  OctreeLevel *prev;

  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS OctreeLevel.

