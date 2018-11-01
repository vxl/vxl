// This is gel/octree/OctreeLevel.h
#ifndef OctreeLevel_h_
#define OctreeLevel_h_
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 11 May 99
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_array_3d.h>

class OctreeLevel
{
 public:
  // Constructors/Destructors--------------------------------------------------

  OctreeLevel( const int = 0);
  OctreeLevel( OctreeLevel *p);

  // Data Access---------------------------------------------------------------

  int GetDepth() const { return depth; }
  void SetNext( OctreeLevel *n) { next= n; }

  // Data Control--------------------------------------------------------------

  void InheritFromAbove();

 protected:
  // Data Members--------------------------------------------------------------

  int depth;
  int size;
  vbl_array_3d<int> color;

  OctreeLevel *next;
  OctreeLevel *prev;
};

#endif // OctreeLevel_h_
