// This is core/vgl/vgl_lineseg_test.h
#ifndef vgl_lineseg_test_h_
#define vgl_lineseg_test_h_
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//   Nov.2003 - Peter Vanroose - made all functions templated
//   Sep.2005 - Peter Vanroose - bug fix: collinear line segments always "true"
//   Mar.2008 - Ibrahim Eden - bug fix: bool vgl_lineseg_test_line(vgl_line_2d<T> const& l1,vgl_line_segment_2d<T> const& l2)
//   Mar.2009 - Dirk Steckhan - bug fix in vgl_lineseg_test_point (missing sqrt)
// \endverbatim

#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_tolerance.h>
#include <vcl_cmath.h> // for std::sqrt()

// The old signature vgl_lineseg_test() was incorrectly documented. Its
// meaning was the same as the new vgl_lineseg_test_line(). Only you can
// decide which one you want.

//: true if the line joining [1], [2] meets the linesegment joining [3], [4].
// End points are considered to belong to a line segment.
export template <class T>
bool vgl_lineseg_test_line(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4);

//: true if the linesegment joining [1], [2] meets the linesegment joining [3], [4].
// End points are considered to belong to a line segment.
export template <class T>
bool vgl_lineseg_test_lineseg(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4);

//: true if the line meets the linesegment.
// End points are considered to belong to a line segment.
// \relates vgl_line_2d
// \relates vgl_line_segment_2d
template <class T>
inline bool vgl_lineseg_test_line(vgl_line_2d<T> const& l1,
                                  vgl_line_segment_2d<T> const& l2)
{
  vgl_point_2d<T> l1_p1,l1_p2;
  l1.get_two_points(l1_p1,l1_p2);
  return vgl_lineseg_test_line(l1_p1.x(),l1_p1.y(),
                               l1_p2.x(),l1_p2.y(),
                               l2.point1().x(),l2.point1().y(),
                               l2.point2().x(),l2.point2().y());
}

//: true if the point lies on the line segment and is between the endpoints
template <class T>
inline bool vgl_lineseg_test_point(vgl_point_2d<T> const& p,
                                   vgl_line_segment_2d<T> const& lseg)
{
  vgl_point_2d<T> p1 = lseg.point1(), p2 = lseg.point2();
  T x1 = p1.x(), y1 = p1.y(),
    x2 = p2.x(), y2 = p2.y(),
    xp = p.x(),  yp = p.y();
  // compute squared distances
  T d1p = (xp-x1)*(xp-x1) + (yp-y1)*(yp-y1);
  T d2p = (xp-x2)*(xp-x2) + (yp-y2)*(yp-y2);
  T d12 = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  double diff = vcl_sqrt(d1p) + vcl_sqrt(d2p) - vcl_sqrt(d12);
  // diff is always >= 0 (triangle inequality)
  return diff <= vgl_tolerance<double>::position;
}

//: return true if the two linesegments meet.
// End points are considered to belong to a line segment.
// \relates vgl_line_segment_2d
template <class T>
inline bool vgl_lineseg_test_lineseg(vgl_line_segment_2d<T> const& l1,
                                     vgl_line_segment_2d<T> const& l2)
{
  return vgl_lineseg_test_lineseg(l1.point1().x(),l1.point1().y(),
                                  l1.point2().x(),l1.point2().y(),
                                  l2.point1().x(),l2.point1().y(),
                                  l2.point2().x(),l2.point2().y());
}

#define VGL_LINESEG_TEST_INSTANTIATE(T) extern "please include vgl/vgl_lineseg_test.txx instead"

#endif // vgl_lineseg_test_h_
