// This is core/vgl/vgl_lineseg_test.h
#ifndef vgl_lineseg_test_h_
#define vgl_lineseg_test_h_
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//   Nov.2003 - Peter Vanroose - made all functions templated
// \endverbatim

#include <vgl/vgl_line_segment_2d.h>

// The old signature vgl_lineseg_test() was incorrectly documented. Its
// meaning was the same as the new vgl_lineseg_test_line(). Only you can
// decide which one you want.

//: true if the line joining [1], [2] meets the linesegment joining [3], [4].
export template <class T>
bool vgl_lineseg_test_line(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4);

//: true if the linesegment joining [1], [2] meets the linesegment joining [3], [4].
export template <class T>
bool vgl_lineseg_test_lineseg(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4);

//: return true if the two linesegments meet
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
