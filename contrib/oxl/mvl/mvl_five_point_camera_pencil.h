// This is oxl/mvl/mvl_five_point_camera_pencil.h
#ifndef mvl_five_point_camera_pencil_h_
#define mvl_five_point_camera_pencil_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4.h>

//:
// Given 5 image points, compute the pencil of
// cameras which map the canonical projective
// basis onto those points.
//
// The computed generators [A, B] are invariant
// to similarity transformations of the image
// coordinates. So there is no need to condition
// the given image points.
//
// For full details consult
//   "A Six Point Solution for Structure and Motion", by
//    Schaffalitzky, Zisserman, Hartley and Torr in ECCV 2000.

bool mvl_five_point_camera_pencil(double const xs[5],
                                  double const ys[5],
                                  vnl_double_3x4 *A,
                                  vnl_double_3x4 *B);

//:
// Return the pair (s, t) such that the reprojection of X under
// P = s A + t B is closest to (u, v) in image coordinates. The
// image residuals are returned in res[].
bool mvl_five_point_camera_pencil_parameters(vnl_double_3x4 const &A,
                                             vnl_double_3x4 const &B,
                                             vnl_double_4 const &X,
                                             double u, double v,
                                             double st[2],
                                             double res[2]);

#endif // mvl_five_point_camera_pencil_h_
