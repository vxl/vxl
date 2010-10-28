// This is algo/bapl/bapl_affine_transform.h
#ifndef bapl_affine_transform_h_
#define bapl_affine_transform_h_
//:
// \file
// \brief A class to represent affine transformations.
// \author Matt Leotta
// \date 10/14/03
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_2.h>

//:A class to represent 2D affine transformations.
// This class represents a 2D affine transformation for use with
// affine patches.  This class is used to map affine image patches
// into a common (retangular) representation for comparison.

class bapl_affine_transform
{
public:
  //: Default Constructor
  bapl_affine_transform();

  //: Constructor
  bapl_affine_transform(const vnl_double_2x2& A, const vnl_double_2& t);

  //: Constructor
  bapl_affine_transform(double a11, double a12,
                        double a21, double a22,
                        double tx,  double ty );

  //: Apply the transformation
  inline vnl_double_2 map(const vnl_double_2& p) const { return A_*p + t_; }

  //: Apply the transformation
  inline void map(double from_x, double from_y, double& to_x, double& to_y) const
  { 
    vnl_double_2 to = this->map(vnl_double_2(from_x,from_y)); 
    to_x = to[0];  to_y = to[1];
  }

  //: Apply the transformation
  inline vnl_double_2 operator* (const vnl_double_2& p) const { return this->map(p); }

  //: Compute and return the inverse matrix
  bapl_affine_transform inverse() const;

  //: Return the skew/scale/rotation matrix
  vnl_double_2x2 A() const { return A_; }

  //: Return the translation vector
  vnl_double_2 t() const { return t_; }


private:
  //: skew/scale/rotation components
  vnl_double_2x2 A_;
  //: translation components
  vnl_double_2 t_;
};

#endif // bapl_affine_transformation_h_
