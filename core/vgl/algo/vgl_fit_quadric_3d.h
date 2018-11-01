// This is core/vgl/algo/vgl_fit_quadric_3d.h
#ifndef vgl_fit_quadric_3d_h_
#define vgl_fit_quadric_3d_h_
//:
// \file
// \brief Fits a quadric surface to a set of 3D points
// \author Joseph L. Mundy
// \date July 7, 2017
//
//  Variousl algorithms for quadric fitting
//  1)  Taubin method - first order approximation to Euclidean distance from point to quadric
//      Normalization by the gradient of the algebraic distance.Formulation as generalized Eigensystem.
//      (G. Taubin, Estimation of Planar Curves, Surfaces,
//      and Nonplanar Space Curves,Defined by Implicit Equations with Applications
//      to Edge and Range Image Segmentation IEEE PAMI, v13, No. 11, 1991.)
//
//
//  2) Alliare method - the normalizing constraint is a quadratic form that restricts the quadric class
//     in terms of the invariants of the upper 3x3 matrix of the quadratic form.
//     Formulation as generalized Eigensystem.
//     (Type-Constrained Robust Fitting of Quadrics with Application to the 3D Morphological
//     Characterization of Saddle-Shaped Articular Surfaces, S. Allaire et al,
//      Proc.International Conference on Computer Vision , 2007.)
//
// \verbatim
//  Modifications
//   none
// \endverbatim
#include <vector>
#include <iosfwd>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_quadric_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vgl_fit_quadric_3d
{
  // Data Members--------------------------------------------------------------
 protected:
  //: fit an ellipsoid  using the linear Allaire method
  // returns the average distance from the points to the quadric
  std::vector<vgl_homg_point_3d<T> > points_;
  vgl_quadric_3d<T> quadric_Taubin_;
  vgl_quadric_3d<T> quadric_Allaire_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

   vgl_fit_quadric_3d() = default;

   vgl_fit_quadric_3d(std::vector<vgl_point_3d<T> > points);

  ~vgl_fit_quadric_3d() = default;

  // Operations---------------------------------------------------------------

  //: add a point to point set
  void add_point(vgl_point_3d<T> const &p);
  void add_point(const T x, const T y, const T z);

  //: clear internal data
  void clear();

  //: fit a general quadric to the stored points using the linear Taubin method.
  // No restriction on type.
  // returns the average distance from the points to the quadric.
  // error conditions are reported on outstream, if defined.
  T fit_linear_Taubin(std::ostream* outstream=nullptr);

  //: fit an ellipsoid using the linear Allaire method
  // returns the average distance from the points to the ellipsoid
  // error conditions are reported on outstream, if defined.
  T fit_ellipsoid_linear_Allaire(std::ostream* outstream=nullptr);

  //: fit a quadric class from the set{ hyperboloid_of_one_sheets,
  //  hyperboloid_of_two_sheets, real_elliptic_cone,
  //  hyperbolic_paraboloid, hyperbolic_cylinder}
  // returns the average distance from the points to the ellipsoid
  // error conditions are reported on outstream, if defined.
  T fit_saddle_shaped_quadric_linear_Allaire(std::ostream* outstream=nullptr);

  // Data Access---------------------------------------------------------------

  std::vector<vgl_point_3d<T> > get_points() const;

  //: The fit computed by the Taubin method
  vgl_quadric_3d<T>& quadric_Taubin_fit() {return quadric_Taubin_;}
  //: The fit computed by the Allaire method
  vgl_quadric_3d<T>& quadric_Allaire_fit() {return quadric_Allaire_;}
};

#define VGL_FIT_QUADRIC_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_quadric_3d.hxx first"

#endif // vgl_fit_quadric_3d_h_
