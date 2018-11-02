#ifndef vgl_convex_hull_2d_hxx_
#define vgl_convex_hull_2d_hxx_
#include <cstdlib>
#include <limits>
#include "vgl_convex_hull_2d.h"
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_area.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_math.h>

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
    std::cout << (P[i]-points[0])/2) << ' ';
  std::cout << std::endl;
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
  v = (*(double*const*)(A))[c] - (*(double*const*)(B))[c];\
  if (v>0) return 1;\
  if (v<0) return -1

static int cmpl(const void *a, const void *b)
{
  double v;
  CMPM(0,a,b);
  CMPM(1,b,a);
  return 0;
}
#undef CMPM

static int cmph(const void *a, const void *b) {return cmpl(b,a);}


static int make_chain(double** V, int n, int (*cmp)(const void*, const void*))
{
  std::qsort(V, n, sizeof(double*), cmp);
  int s = 1;
  for (int i=2; i<n; i++) {
    while (s>=1 && ccw(V, i, s, s-1)) --s;
    ++s;
    double* t = V[s]; V[s] = V[i]; V[i] = t;
  }
  return s;
}

static inline int ch2d(double **P, int n)
{
  int u = make_chain(P, n, cmpl);         // make lower hull
  if (!n) return 0;
  P[n] = P[0];
  return u+make_chain(P+u, n-u+1, cmph);  // make upper hull
}

template <class T>
vgl_convex_hull_2d<T>::
vgl_convex_hull_2d (std::vector<vgl_point_2d<T> > const& points)
{
  hull_valid_ = false;
  points_ = points;
}

template <class T>
void vgl_convex_hull_2d<T>::compute_hull()
{
  //convert points to internal data structure
  int N = points_.size();
  if (N < 1)
    return;

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
  std::vector<vgl_point_2d<T> > temp;
  for (int i = 0; i<n_hull; i++)
  {
    vgl_point_2d<T> p((T)P[i][0], (T)P[i][1]);
    temp.push_back(p);
  }
  // Do not add last point if it is identical to the first one - PVr
  if (P[0][0] != P[n_hull][0] || P[0][1] != P[n_hull][1])
    temp.push_back(vgl_point_2d<T>((T)P[n_hull][0], (T)P[n_hull][1]));

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
template <class T>
vgl_oriented_box_2d<T>  vgl_convex_hull_2d<T>::min_area_enclosing_rectangle(){
  if (!hull_valid_)
    this->compute_hull();
  // the algorithm uses the fact that the smallest enclosing rectangle
  // has a side in common with and edge of the convex hull
  std::vector<vgl_point_2d<T> > verts = hull_[0];
  size_t n = verts.size();
  vgl_box_2d<T> min_box;
  T min_angle = T(0);
  T min_area = std::numeric_limits<T>::max();
  vgl_vector_2d<T> min_offset(T(0),T(0));
  for(size_t i = 1; i<=n; ++i){
    vgl_point_2d<T>& vm1 = verts[i-1];
    vgl_vector_2d<T> dir = verts[i%n]-vm1;
    T theta = atan2(dir.y(), dir.x());
    T c = cos(-theta), s=sin(-theta);
    vgl_box_2d<T> rbox;
      // rotate the vertices about v_i-1
      // the resulting box needs to be shifted by v_i-1
    for(size_t j = 0; j<n; ++j){
      vgl_vector_2d<T> vp = verts[j]-vm1;
      vgl_point_2d<T> rpp((c*vp.x()-s*vp.y()),(s*vp.x() + c*vp.y()));
      rbox.add(rpp);
    }
    T area = vgl_area(rbox);
    if(area<min_area){
      min_offset.set(vm1.x(), vm1.y());
      min_area = vgl_area(rbox);
      min_box = rbox;
      min_angle = theta;
    }
  }
  T w = min_box.width(), h = min_box.height();
  vgl_point_2d<T> c = min_box.centroid();
  //select major axis such that width > height
  T width = w, height = h;
  vgl_point_2d<T> pmaj0(c.x()-width/T(2), c.y()), pmaj1(c.x()+width/T(2), c.y());
  if(w<h){
    width = h;
    height = w;
    pmaj0.set(c.x(), c.y()-width/T(2)); pmaj1.set(c.x(), c.y()+width/T(2));
  }

  // rotate major axis about v_i-1
  T cs = cos(min_angle), s = sin(min_angle);
  T pmaj0x = pmaj0.x(), pmaj0y = pmaj0.y();
  T pmaj1x = pmaj1.x(), pmaj1y = pmaj1.y();
  vgl_point_2d<T> pmaj0r(cs*pmaj0x - s*pmaj0y, s*pmaj0x + cs*pmaj0y);
  vgl_point_2d<T> pmaj1r(cs*pmaj1x - s*pmaj1y, s*pmaj1x + cs*pmaj1y);

  // add back rotation center
  pmaj0r += min_offset; pmaj1r += min_offset;
  return vgl_oriented_box_2d<T>(pmaj0r, pmaj1r, height);
}
//----------------------------------------------------------------------------
#undef VGL_CONVEX_HULL_2D_INSTANTIATE
#define VGL_CONVEX_HULL_2D_INSTANTIATE(T) \
/* template std::ostream& operator<<(std::ostream& s, vgl_convex_hull_2d<T >const& h); */ \
/* template std::istream& operator>>(std::istream& s, vgl_convex_hull_2d<T >& h); */ \
template class vgl_convex_hull_2d<T >

#endif // vgl_convex_hull_2d_hxx_
