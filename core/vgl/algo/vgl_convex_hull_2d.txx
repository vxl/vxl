#ifndef vgl_convex_hull_2d_txx_
#define vgl_convex_hull_2d_txx_
#include "vgl_convex_hull_2d.h"
//:
// \file
// \brief two-dimensional convex hull
// read points from stdin,
//      one point per line, as two numbers separated by whitespace
// on stdout, points on convex hull in order around hull, given
//      by their numbers in input order
// the results should be "robust", and not return a wildly wrong hull,
//      despite using floating point
// works in O(n log n); I think a bit faster than Graham scan;
//      somewhat like Procedure 8.2 in Edelsbrunner's "Algorithms in Combinatorial
//      Geometry", and very close to:
//        A.M. Andrew, "Another Efficient Algorithm for Convex Hulls in Two Dimensions",
//        Info. Proc. Letters 9, 216-219 (1979)
//      (See also http://geometryalgorithms.com/Archive/algorithm_0110/algorithm_0110.htm)
//
// Ken Clarkson wrote this.  Copyright (c) 1996 by AT&T..
// Permission to use, copy, modify, and distribute this software for any
// purpose without fee is hereby granted, provided that this entire notice
// is included in all copies of any software which is or includes a copy
// or modification of this software and in all copies of the supporting
// documentation for such software.
// THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
// REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
// OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.

#if 0
static void print_hull(double **P, int m)
{
  for (int i=0; i<m; i++)
    vcl_cout << (P[i]-points[0])/2) << ' ';
  vcl_cout << vcl_endl;
}
#endif // 0

static int ccw(double **P, int i, int j, int k)
{
  double a = P[i][0] - P[j][0],
         b = P[i][1] - P[j][1],
         c = P[k][0] - P[j][0],
         d = P[k][1] - P[j][1];
  return a*d - b*c <= 0;   // true if points i, j, k counterclockwise
}


#define CMPM(c,A,B) \
  v = (*(double*const*)A)[c] - (*(double*const*)B)[c];\
  if (v>0) return 1;\
  if (v<0) return -1;

static int cmpl(const void *a, const void *b)
{
  double v;
  CMPM(0,a,b);
  CMPM(1,b,a);
  return 0;
}

static int cmph(const void *a, const void *b) {return cmpl(b,a);}


static int make_chain(double** V, int n, int (*cmp)(const void*, const void*))
{
  qsort(V, n, sizeof(double*), cmp);
  int s = 1;
  for (int i=2; i<n; i++) {
    while (s>=1 && ccw(V, i, s, s-1)) --s;
    ++s;
    double* t = V[s]; V[s] = V[i]; V[i] = t;
  }
  return s;
}

static int ch2d(double **P, int n)
{
  int u = make_chain(P, n, cmpl);         // make lower hull
  if (!n) return 0;
  P[n] = P[0];
  return u+make_chain(P+u, n-u+1, cmph);  // make upper hull
}

template <class T>
vgl_convex_hull_2d<T>::
vgl_convex_hull_2d (vcl_vector<vgl_point_2d<T> > const& points)
{
  hull_valid_ = false;
  points_ = points;
}

template <class T>
void vgl_convex_hull_2d<T>::compute_hull()
{
  //convert points to internal data structure
  int N = points_.size();
  double * array = new double[2*N];
  double** points = new double*[N];
  double** P = new double*[N+1];
  for (int i = 0; i<N; i++)
    points[i]=&array[2*i];

  for (int n = 0; n<N; n++)
  {
    points[n][0]=(double)points_[n].x();
    points[n][1]=(double)points_[n].y();
    P[n] = &points[n][0];
  }
  //the main hull routine
  int n_hull = ch2d(P, N);

  //convert back to vgl_points
  vcl_vector<vgl_point_2d<double> > temp;
  for (int i = 0; i<=n_hull; i++)
  {
    vgl_point_2d<T> p((T)P[i][0], (T)P[i][1]);
    temp.push_back(p);
  }

  //construct the hull polygon
  hull_ = vgl_polygon<T>(temp);
  //clean up memory
  delete [] array;
  delete [] points;
  delete [] P;
  hull_valid_ = true;
}

template <class T>
vgl_polygon<T> vgl_convex_hull_2d<T>::hull()
{
  if (!hull_valid_)
    this->compute_hull();
  return hull_;
}

//----------------------------------------------------------------------------
#undef VGL_CONVEX_HULL_2D_INSTANTIATE
#define VGL_CONVEX_HULL_2D_INSTANTIATE(T) \
/* template vcl_ostream& operator<<(vcl_ostream& s, vgl_convex_hull_2d<T >const& h); \
   template vcl_istream& operator>>(vcl_istream& s, vgl_convex_hull_2d<T >& h); */ \
template class vgl_convex_hull_2d<T >

#endif // vgl_convex_hull_2d_txx_
