// This is core/vgl/algo/vgl_fit_xy_paraboloid_3d.h
#ifndef vgl_fit_xy_paraboloid_3d_h_
#define vgl_fit_xy_paraboloid_3d_h_
//:
// \file
// \brief Fits a 3-d paraboloid to a set of z(x,y) points
// \author Joseph L. Mundy
// \date August 26, 2021
//
// Useful for refining the solution to a 2-d minimization problem, where z is a cost or error value
// Also can model a depression or mound in a parametric surface z(u,v).
//
// \verbatim
//  Modifications
//   none
// \endverbatim
#include <vector>
#include <iosfwd>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_quadric_3d.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vgl_fit_xy_paraboloid_3d
{
  // Data Members--------------------------------------------------------------
protected:
  std::vector<vgl_homg_point_3d<T>> points_;
  vgl_quadric_3d<T> paraboloid_linear_;
  // coefficient vector, p,  for paraboloid as a 6x1 matrix
  // z = f(x,y) = p_[0][0]x^2 + p_[1][0]xy + p_[2][0]y^2 + p_[3][0]x + p_[4][0]y + p_[5][0]
  vnl_matrix<T> p_;
  vgl_point_2d<T> extremum_;

public:
  // Constructors/Initializers/Destructors-------------------------------------

  vgl_fit_xy_paraboloid_3d() = default;

  vgl_fit_xy_paraboloid_3d(std::vector<vgl_point_3d<T>> points);

  ~vgl_fit_xy_paraboloid_3d() = default;

  // Operations---------------------------------------------------------------

  //: add a point to point set
  void
  add_point(const vgl_point_3d<T> & p);
  void
  add_point(const T x, const T y, const T z);

  //: clear internal data
  void
  clear();

  //: fit the paraboloid using a linear regression algorithm
  // returns the average distance from the points to the paraboloid
  // error conditions are reported on outstream, if defined.
  T
  fit_linear(std::ostream * errstream);

  // Data Access---------------------------------------------------------------

  std::vector<vgl_point_3d<T>>
  points() const;

  //: The linear fit
  vgl_quadric_3d<T>
  paraboloid_linear() const
  {
    return paraboloid_linear_;
  }

  //: z values for fitted paraboloid
  T
  z(T x, T y) const;

  //: The extremum x-y location
  vgl_point_2d<T>
  extremum_point() const;

  //: the string type name  should be "elliptic_paraboloid"
  std::string
  quadric_type() const
  {
    return paraboloid_linear_.type_by_number(paraboloid_linear_.type());
  }
};

#define VGL_FIT_XY_PARABOLOID_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_xy_paraboloid_3d.hxx first"

#endif // vgl_fit_xy_paraboloid_3d_h_
