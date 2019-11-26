// This is core/vgl/vgl_triangle_scan_iterator.hxx
#ifndef vgl_triangle_scan_iterator_hxx_
#define vgl_triangle_scan_iterator_hxx_
//:
// \file
// \author fsm

#include <cmath>
#include <iostream>
#include "vgl_triangle_scan_iterator.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
static inline
void min_n_max(T a, T b, T c, T *min, T *max)
{
  if (a < b) {
    if (a < c) {
      *min = a;
      if (b < c)  // a b c
  *max = c;
      else        // a c b
  *max = b;
    }
    else {        // c a b
      *min = c;
      *max = b;
    }
  }
  else {
    if (b < c) {
      *min = b;
      if (a < c) // b a c
  *max = c;
      else       // b c a
  *max = a;
    }
    else {       // c b a
      *min = c;
      *max = a;
    }
  }
  // that was fun. now do some work.
}

#if use_polygon_scan_iterator
#include "vgl_polygon_scan_iterator.h"
template <class T>
struct vgl_triangle_scan_iterator<T>::data_t : public vgl_polygon_scan_iterator<T>
{
  typedef vgl_polygon_scan_iterator<T> base;
  data_t(vgl_polygon<T> const &p) : base(p) {}
};

template <class T>
vgl_triangle_scan_iterator<T>::~vgl_triangle_scan_iterator()
{
  if (data)
    delete data;
  data = 0;
}
#else
// w(a, b, c) = det [ a b c ]
//                  [ 1 1 1 ]
//# define w(a, b, c) ( b.x*c.y - b.x*a.y - a.x*c.y - c.x*b.y + c.x*a.y + a.x*b.y )
#endif

template <class T>
void vgl_triangle_scan_iterator<T>::reset()
{
#if use_polygon_scan_iterator
  if (data)
    delete data;
  T x[3] = { a.x, b.x, c.x };
  T y[3] = { a.y, b.y, c.y };
  vgl_polygon<T> p(x, y, 3);
  data = new data_t(p);
  data->reset();
#else
  T min, max;

  min_n_max(a.x, b.x, c.x, &min, &max);
  x0 = (int) std::ceil (min);
  x1 = (int) std::floor(max);
#ifdef DEBUG
  std::cerr << "x0 x1 = " << x0 << ' ' << x1 << '\n';
#endif

  min_n_max(a.y, b.y, c.y, &min, &max);
  y0 = (int) std::ceil (min);
  y1 = (int) std::floor(max);
#ifdef DEBUG
  std::cerr << "y0 y1 = " << y0 << ' ' << y1 << '\n';
#endif

  scany_ = y0 - 1;

  // compute centroid
  g.x = std::floor((a.x + b.x + c.x)/3);
  g.y = std::floor((a.y + b.y + c.y)/3);
#ifdef DEBUG
  std::cerr << "g = " << g.x << ' ' << g.y << '\n';
#endif

  //
  pt ag = { a.x - g.x, a.y - g.y };
  pt bg = { b.x - g.x, b.y - g.y };
  pt cg = { c.x - g.x, c.y - g.y };

  data[0][0] = bg.y - cg.y; data[0][1] = cg.x - bg.x; data[0][2] = bg.x * cg.y - bg.y * cg.x;
  data[1][0] = cg.y - ag.y; data[1][1] = ag.x - cg.x; data[1][2] = cg.x * ag.y - cg.y * ag.x;
  data[2][0] = ag.y - bg.y; data[2][1] = bg.x - ag.x; data[2][2] = ag.x * bg.y - ag.y * bg.x;

  T tmp = ( bg.x*cg.y - bg.x*ag.y - ag.x*cg.y - cg.x*bg.y + cg.x*ag.y + ag.x*bg.y );
  if (tmp < 0)
    tmp = -1;
  else
    tmp = +1;
  for (auto & i : data) {
    T f = tmp; // / sqrt(data[i][0]*data[i][0] + data[i][1]*data[i][1]);
    for (int j=0; j<3; ++j)
      i[j] *= f;
  }
#if 0
  std::cerr << "data:\n";
  for (int i=0; i<3; ++i) {
    for (int j=0; j<3; ++j)
      std::cerr << ' ' << data[i][j];
    std::cerr << std::endl;
  }
  std::cerr << std::endl;
#endif
#endif // use_polygon_scan_iterator
}

template <class T>
bool vgl_triangle_scan_iterator<T>::next()
{
#if use_polygon_scan_iterator
  if (data->next()) {
    scany_ = data->scany();
    startx_ = data->startx();
    endx_ = data->endx();
    return true;
  }
  else
    return false;
#else
  if (++scany_ > y1)
    return false;

  T minx = x0 - g.x;
  T maxx = x1 - g.x;

#ifdef DEBUG
  std::cerr << "minx maxx = " << minx << ' ' << maxx << '\n';
#endif
  for (auto & i : data) {
    T a_ = i[0];
    T b_ = i[1] * (scany_ - g.y) + i[2];
    // ax + b >= 0
    if (a_ == 0) {
      // bif bif
    }
    else {
      T x = -b_/a_;
      if (a_ > 0) {
        if (x > minx)
          minx = x;
      }
      else {
        if (x < maxx)
          maxx = x;
      }
    }
#ifdef DEBUG
    std::cerr << "minx maxx = " << minx << ' ' << maxx << '\n';
#endif
  }

  startx_ = (int) std::ceil (minx + g.x);
  endx_   = (int) std::floor(maxx + g.x);

  // can not use (scany_ == y0) || (startx_ <= endx_)
  // for early scan termination because some triangles may have
  // (startx_ > endx_) for more than the first scan line


  return true;
#endif
}

#undef VGL_TRIANGLE_SCAN_ITERATOR_INSTANTIATE
#define VGL_TRIANGLE_SCAN_ITERATOR_INSTANTIATE(T) \
template class vgl_triangle_scan_iterator<T >

#endif // vgl_triangle_scan_iterator_hxx_
