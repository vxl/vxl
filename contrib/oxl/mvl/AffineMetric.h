// This is oxl/mvl/AffineMetric.h
#ifndef AffineMetric_h_
#define AffineMetric_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief AffineMetric is an ImageMetric that is an affine transformation.
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 24 Feb 97
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>
#include <mvl/ImageMetric.h>

class AffineMetric : public ImageMetric
{
  vnl_double_3x3 A_;
  vnl_double_3x3 A_inverse_;
 public:

  AffineMetric();
  AffineMetric(vnl_double_3x3 const& A);

  virtual vnl_double_2 homg_to_image(const HomgPoint2D& p) const;
  virtual HomgPoint2D image_to_homg(const vnl_double_2&) const;
  virtual HomgPoint2D image_to_homg(double x, double y) const;

  virtual HomgPoint2D homg_to_imagehomg(const HomgPoint2D& p) const;
  virtual HomgPoint2D imagehomg_to_homg(const HomgPoint2D& p) const;

  void set(vnl_double_3x3 const& A);
  void set(vnl_matrix<double> const& A);

  void set(double a11, double a13, double a22, double a23, double a33);

  //: Return forward transformation matrix
  virtual vnl_double_3x3 get_C() const { return A_; }

  //: Return inverse transformation matrix
  virtual vnl_double_3x3 get_C_inverse() const { return A_inverse_; }

  //: Declare that this is a linear transformation
  virtual bool is_linear() const { return true; }

  //: Declare that this is not an isometry
  virtual bool can_invert_distance() const { return false; }

  //: Send a human-readable representation to ostream
  vcl_ostream& print(vcl_ostream& s) const;
};

#endif // AffineMetric_h_
