#ifndef vgl_lineseg_test_h_
#define vgl_lineseg_test_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vgl/vgl_lineseg_test.h

//:
// \file
// \author fsm@robots.ox.ac.uk

//: return true if the two linesegments meet
template <class T>
bool vgl_lineseg_test(T x1, T y1, T x2, T y2,
        T x3, T y3, T x4, T y4);

#endif // vgl_lineseg_test_h_
