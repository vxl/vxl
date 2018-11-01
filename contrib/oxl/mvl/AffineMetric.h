// This is oxl/mvl/AffineMetric.h
#ifndef AffineMetric_h_
#define AffineMetric_h_
//:
// \file
// \brief AffineMetric is an ImageMetric that is an affine transformation.
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 24 Feb 97
//
// \verbatim
//  Modifications
//   22 Jun 2003 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_fwd.h>
#include <mvl/ImageMetric.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class AffineMetric : public ImageMetric
{
  vnl_double_3x3 A_;
  vnl_double_3x3 A_inverse_;
 public:

  AffineMetric();
  AffineMetric(vnl_double_3x3 const& A);

  vgl_point_2d<double> homg_to_image(vgl_homg_point_2d<double> const&) const override;
  vnl_double_2 homg_to_image(const HomgPoint2D& p) const override;

  vgl_homg_point_2d<double> image_to_homg(vgl_point_2d<double> const&) const override;
  HomgPoint2D image_to_homg(const vnl_double_2&) const override;
  HomgPoint2D image_to_homg(double x, double y) const override;

  vgl_homg_point_2d<double> homg_to_imagehomg(vgl_homg_point_2d<double> const&) const override;
  vgl_homg_point_2d<double> imagehomg_to_homg(vgl_homg_point_2d<double> const&) const override;

  HomgPoint2D homg_to_imagehomg(const HomgPoint2D& p) const override;
  HomgPoint2D imagehomg_to_homg(const HomgPoint2D& p) const override;

  void set(vnl_double_3x3 const& A);
  void set(vnl_matrix<double> const& A);

  void set(double a11, double a13, double a22, double a23, double a33);

  //: Return forward transformation matrix
  vnl_double_3x3 get_C() const override { return A_; }

  //: Return inverse transformation matrix
  vnl_double_3x3 get_C_inverse() const override { return A_inverse_; }

  //: Declare that this is a linear transformation
  bool is_linear() const override { return true; }

  //: Declare that this is not an isometry
  bool can_invert_distance() const override { return false; }

  //: Send a human-readable representation to ostream
  std::ostream& print(std::ostream& s) const override;
};

#endif // AffineMetric_h_
