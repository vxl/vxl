// This is vxl/vgl/vgl_lineseg_test.h
#ifndef vgl_lineseg_test_h_
#define vgl_lineseg_test_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vgl/vgl_line_segment_2d.h>

//: return true if the two linesegments meet
template <class T>
bool vgl_lineseg_test(T x1, T y1, T x2, T y2,
        T x3, T y3, T x4, T y4);

//: return true if the two linesegments meet
// \relates vgl_line_segment_2d
template <class T>
inline bool vgl_lineseg_test(vgl_line_segment_2d<T> const& l1,
                             vgl_line_segment_2d<T> const& l2)
{
  return vgl_lineseg_test(l1.point1().x(),l1.point1().y(),
                          l1.point2().x(),l1.point2().y(),
                          l2.point1().x(),l2.point1().y(),
                          l2.point2().x(),l2.point2().y());
}

#endif // vgl_lineseg_test_h_
