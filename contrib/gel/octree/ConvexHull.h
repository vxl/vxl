// This is gel/octree/ConvexHull.h
#ifndef ConvexHull_h_
#define ConvexHull_h_
//:
// \file
// \author
//     Geoffrey Cross, Oxford RRG, 24 May 99
//
//-----------------------------------------------------------------------------

class ConvexHull
{
 public:
  // Constructors/Destructors--------------------------------------------------

  ConvexHull( int npoints);

  // Computations--------------------------------------------------------------

  void compute();

  // Data Access---------------------------------------------------------------

  void set_point( int n, double x, double y);
  double get_pointx( int n) const { return P[n][0]; }
  double get_pointy( int n) const { return P[n][1]; }
  inline int get_npoints() const { return hullsize; }

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

#endif // ConvexHull_h_
