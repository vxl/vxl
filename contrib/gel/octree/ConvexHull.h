//-*- c++ -*-------------------------------------------------------------------
#ifndef ConvexHull_h_
#define ConvexHull_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : ConvexHull
//
// .SECTION Description
//    ConvexHull is a class that Geoff hasn't documented properly. FIXME
//
// .NAME        ConvexHull - Undocumented class FIXME
// .LIBRARY     new
// .HEADER	octree Package
// .INCLUDE     new/ConvexHull.h
// .FILE        ConvexHull.h
// .FILE        ConvexHull.C
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 24 May 99
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>

class ConvexHull {
public:
  // Constructors/Destructors--------------------------------------------------
  
  ConvexHull( int npoints);

  // Operations----------------------------------------------------------------
  
  // Computations--------------------------------------------------------------

  void compute();

  // Data Access---------------------------------------------------------------

  void set_point( int n, double x, double y);
  double get_pointx( int n) { return P[n][0]; }
  double get_pointy( int n) { return P[n][1]; }
  inline int get_npoints() { return hullsize; }

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  
  int numberofpoints;
  int hullsize;
  double points[10][2], *P[10+1];

  // Helpers-------------------------------------------------------------------

  static int cmpl(const void *a, const void *b);
  static int cmph(const void *a, const void *b);
  int ccw(double **P, int i, int j, int k);
  int ch2d();
  int make_chain(double** V, int n, int (*cmp)(const void*, const void*));
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS ConvexHull.

