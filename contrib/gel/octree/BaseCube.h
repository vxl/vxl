//-*- c++ -*-------------------------------------------------------------------
#ifndef BaseCube_h_
#define BaseCube_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : BaseCube
//
// .SECTION Description
//    BaseCube is a class that Geoff hasn't documented properly. FIXME
//
// .NAME        BaseCube - Undocumented class FIXME
// .LIBRARY     new
// .HEADER	octree Package
// .INCLUDE     new/BaseCube.h
// .FILE        BaseCube.h
// .FILE        BaseCube.C
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 11 May 99
//
//-----------------------------------------------------------------------------

class BaseCube {
public:
  // Constructors/Destructors--------------------------------------------------
  
  BaseCube( double, double, double, double);

  // Operations----------------------------------------------------------------
  
  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  
  double bases;
  double basex;
  double basey;
  double basez;

  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS BaseCube.

