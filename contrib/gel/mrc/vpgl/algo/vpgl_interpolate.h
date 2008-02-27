// This is gel/mrc/vpgl/algo/vpgl_interpolate.h
#ifndef vpgl_interpolate_h_
#define vpgl_interpolate_h_
//:
// \file
// \brief Methods for interpolating between cameras
// \author J. L. Mundy
// \date Dec 29, 2007

#include <vcl_vector.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vpgl/vpgl_perspective_camera.h>

class vpgl_interpolate
{
 public:
  ~vpgl_interpolate();

  //: the log of a rotation matrix
  //
  //  \verbatim
  //               phi
  // log(R) =   --------- (R - transpose(R))
  //           2*sin(phi)
  //  \endverbatim
  //
  // where 1 + 2*cos(phi) = Trace(R)
  //
  //
  static vnl_double_3x3 logR(vnl_double_3x3 const& R);

  //: the exponential form of a rotation Lie algebra element
  //
  //  \verbatim
  //                  sin(|r|)     1-cos(|r|)
  // exp(r) = I  +  --------- r   ----------- r*r
  //                    |r|         |r|*|r|
  //  \endverbatim
  //
  // where r is a skew-symmetric matric matrix formed from a vector, v as,
  //  \verbatim
  //        _             _
  //       |  0   -vz   vy |
  //   r = |  vz   0   -vx | ,and |r| = |v|. Note this norm is different from
  //       | -vy   vx   0  |   the Frobenius norm of r as a 3x3 matrix.
  //        -             -
  //  \endverbatim
  //
  static vnl_double_3x3 expr(vnl_double_3x3 const& r);

  //: the "A" matrix in the exp operator for Special Euclidean 3-d (SE3)
  //  Let r be as above.
  //  Then
  //  \verbatim
  //                    1 - cos(|r|)         |r| - sin(|r|)
  //            A = I + ------------ r   +   -------------- r*r
  //                      |r|*|r|             |r|*|r|*|r|
  //  \endverbatim
  //
  static vnl_double_3x3 A(vnl_double_3x3 const& r);

  //: the inverse A  matrix for log operator on Special Euclidean 3-d (SE3)
  //  Let r be as above.
  //  Then
  //  \verbatim
  //                       1       2*sin(|r|)-|r|*(1+cos(|r|)
  //            Ainv = I + - r  +  -------------------------- r*r
  //                       2            2 |r|*|r|*sin(|r|)
  //  \endverbatim
  //
  static vnl_double_3x3 Ainv(vnl_double_3x3 const& r);

  //:Interpolate between two rotation matrices, R0 and R1
  // Lie group theory can be used to find a two-point interpolation of rotation
  // with respect to a parameter s, where 0<= s <=1
  // The solution is:
  //
  //  R(s) = R0*exp(s r)
  //  where r = log(transpose(R0).R1)
  //
  // for more details, see
  // F. C. Park, B. Ravani,"Smooth invariant interpolation of rotations,"
  // ACM Transactions on Graphics, Vol. 16, No. 3, July 1997, pp. 277-295.
  //
  static vcl_vector<vnl_double_3x3> interpolateR(vnl_double_3x3 R0,
                                                 vnl_double_3x3 R1,
                                                 unsigned n_between);

  //:Interpolate both R and t at the specified intervals
  static void interpolateRt(vnl_double_3x3 R0,
                            vnl_double_3 t0,
                            vnl_double_3x3 R1,
                            vnl_double_3 t1,
                            unsigned n_between,
                            vcl_vector<vnl_double_3x3>& Rintrp,
                            vcl_vector<vnl_double_3>& tintrp);

  //:Interpolate between two perpspective cameras with the same K given that cam0 = K[R0|t0], cam1 = K[R1|t1]
  // The interpolation produces cameras on uniform intervals in Lie distance
  static bool interpolate(vpgl_perspective_camera<double> const& cam0,
                          vpgl_perspective_camera<double> const& cam1,
                          unsigned n_between,
                          vcl_vector<vpgl_perspective_camera<double> >& cams);

  //:Linearly interpolate (or extrapolate if abs(alpha) > 1) a rotation in Lie space.
  // In Lie space, the interpolated rotation = R0 + alpha*(R1 - R0)
  static vnl_double_3x3 interpolateR(vnl_double_3x3 R0,
                                     vnl_double_3x3 R1,
                                     double alpha);

 private:
  //: constructor private - static methods only
  vpgl_interpolate();
};

#endif // vpgl_interpolate_h_
