// This is core/vgl/vgl_quadric.h
#ifndef vgl_quadric_h_
#define vgl_quadric_h_
//:
// \file
// \brief A 2nd order algebraic surface in 3-d
// \author J.L. Mundy
// \date June 4, 2017
// \verbatim
// Modifications
// none
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <list>
#include <string>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
// the implicit equation for a quadric surface is:
// 1) ax^2 + by^2 + cz^2 + dxy + exz + fyz + gxw +hyw +izw +jw^2 = 0
// w is the homogeneous scale factor.
// there are 17 classes of quadrics depending on the signs of curvature
// and degrees of degeneracy
//
template <class T>
class vgl_quadric_3d
{
 public:
   enum vgl_quadric_type {
    no_type=0,
    real_ellipsoid,
    imaginary_ellipsoid,
    hyperboloid_of_one_sheet,
    hyperboloid_of_two_sheets,
    real_elliptic_cone,
    imaginary_elliptic_cone,
    elliptic_paraboloid,
    hyperbolic_paraboloid,
    real_elliptic_cylinder,
    imaginary_elliptic_cylinder,
    hyperbolic_cylinder,
    real_intersecting_planes,
    imaginary_intersecting_planes,
    parabolic_cylinder,
    real_parallel_planes,
    imaginary_parallel_planes,
    coincident_planes,
    num_quadric_types // is here to enable iterating through this list
  };

 private:
  // DATA MEMBERS
  bool det_zero_;
  vgl_quadric_type type_;
  T a_; //!< coefficient of \a x^2
  T b_; //!< coefficient of \a y^2
  T c_; //!< coefficient of \a z^2
  T d_; //!< coefficient of \a xy
  T e_; //!< coefficient of \a xz
  T f_; //!< coefficient of \a yz
  T g_; //!< coefficient of \a xw
  T h_; //!< coefficient of \a yw
  T i_; //!< coefficient of \a zw
  T j_; //!< coefficient of \a w^2

 public:
  inline vgl_quadric_type type() const { return type_; }

  //: Returns the internal enum value corresponding to the string argument.
  // Useful for comparison purposes, or for use in "case" statements.
  static vgl_quadric_type type_by_name(std::string const& name);

  //: Converts the quadric type from enum (internal representation) to string.
  static std::string type_by_number(vgl_quadric_type const& type) ;

  //: Returns the coefficient of \f$X^2\f$
  inline T a() const { return  a_; }

  //: Returns the coefficient of \f$Y^2\f$
  inline T b() const { return  b_; }

  //: Returns the coefficient of \f$Z^2\f$
  inline T c() const { return  c_; }

  //: Returns the coefficient of \f$XY\f$
  inline T d() const { return  d_; }

  //: Returns the coefficient of \f$XZ\f$
  inline T e() const { return  e_; }

  //: Returns the coefficient of \f$YZ\f$
  inline T f() const { return  f_; }

  //: Returns the coefficient of \f$XW\f$
  inline T g() const { return  g_; }

  //: Returns the coefficient of \f$YW\f$
  inline T h() const { return  h_; }

  //: Returns the coefficient of \f$ZW\f$
  inline T i() const { return  i_; }

  //: Returns the coefficient of \f$W^2\f$
  inline T j() const { return  j_; }

  // CONSTRUCTORS AND RELATED STUFF
  // default constructor
  vgl_quadric_3d() : type_(no_type) {}

  //: constructor using polynomial coefficients.
  vgl_quadric_3d(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j);

  //: constructor from a linear array of polynomial coefficients, given as a C array.
  vgl_quadric_3d(T const coeff[]);

  //: constructor from a matix of polynomial coefficients (see below)
  vgl_quadric_3d(std::vector<std::vector<T> > const& Q);

  //: return a matrix of quadric coefficients of the form:
  //       _                  _
  //      |  a   d/2  e/2  g/2 |
  //      | d/2   b   f/2  h/2 |
  // Q =  | e/2  f/2   c   i/2 |
  //      | g/2  h/2  i/2   j  |
  //       -                  -
  //  Note that X^t Q X = 0 , where X^t =[x y z w] is the same as implicit equation 1) above.
  //
  std::vector<std::vector<T> > coef_matrix() const;

  //: constructor from a canonical 4x4 quadric coefficient matrix and a 4x4 homogeneous matrix, H
  // representing the Euclidean transformation from the canonical frame to the global frame
  //                  _    _
  //                 |R    t|  where t is a 3x1 translation vector and R is a rotation matrix
  //             H = |      |  0^t is a 1x3 zero vector.
  //                 |0^t  1|
  //                  -    -
  vgl_quadric_3d(std::vector<std::vector<T> > const& canonical_quadric, std::vector<std::vector<T> > const& H);

  //: constructor for central quadrics e.g. ellipsoid, ax^2+ by^2+ cz*2 + j = 0, where diag = [a,b,c,j]
  // are the diagonal elements of the 4x4 quadric coefficient matrix and a 4x4 homogeneous matrix, and
  //  H represents the Euclidean transformation from the canonical frame to the global frame (see above)
  vgl_quadric_3d(std::vector<T> const& diag, std::vector<std::vector<T> > const& H);

  //: set or reset the quadric using polynomial coefficients.
  void set(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j);

  void set(std::vector<std::vector<T> > const& Q);

  //: comparison operator.
  //  Comparison is on the quadric, not the equation coefficients.  Hence two
  //  quadrics are identical if their coefficient vectors are multiples of
  //  each other.
  bool operator==(vgl_quadric_3d<T> const& c) const;

  // UTILITY FUNCTIONS

  //: Returns true if this quadric is degenerate
  bool is_degenerate() const{return det_zero_;}

  //: Sampson distance d_sampson(p) = ||p^t Q p||^2/||grad(p*t Q p)||^2
  // a first order approximation to Euclidean distance
  T sampson_dist(vgl_homg_point_3d<T> const& pt) const;

  //: Returns true if the point pt belongs to the quadric surface.
  //  I.e., if it  satisfies the quadric equation within algebraic distance, i.e. pt^t Q pt < tol;
  bool on(vgl_homg_point_3d<T> const& pt, T tol = T(0)) const;

  //: if the upper 3x3 submatrix of Q is full rank then the center of the quadric can be defined
  // otherwise the center is not defined for degenerate quadrics
  bool center(vgl_point_3d<T>& center) const;

  //:: eigenvalues and eigenvectors of the upper 3x3 quadric matrix
  void upper_3x3_eigensystem(std::vector<T>& eigenvalues, std::vector<std::vector<T> >& eigenvectors) const;

  //:: The quadric in its canonical frame if the center is defined, i.e. the upper 3x3 quadric matrix is full rank
  // In this case the quadric coefficient matrix in the canonical frame is
  // a 4x4 diagonal matrix, e.g. ax^2 + by^2 + cz^2 + j = 0. Note that the canonical frame is not unique as
  // alignment of quadric axes with the orthogonal frame has numerous possible arrangements.
  // H is a homogenous(4x4)transformation from canonical coordinate space back to the original space.
  bool canonical_central_quadric(std::vector<T>& diag, std::vector<std::vector<T> >& H) const;

  //: The quadric coefficient matrix in the canonical frame, whether or not the quadric is central
  // H is a homogenous(4x4)transformation from canonical coordinate space back to the original space.
  std::vector<std::vector<T> > canonical_quadric(std::vector<std::vector<T> >& H) const;
 private:
  //--------------------------------------------------------------------------
  //: set quadric type from polynomial coefficients and store in member type_
  void compute_type();

};

// \relatesalso vgl_quadric_3d
template <class T>
std::ostream&  operator<<(std::ostream& s, vgl_quadric_3d<T> const& c);

//: Read a b c d e f g h i j from stream
// \relatesalso vgl_quadric_3d
template <class T>
std::istream&  operator>>(std::istream& s, vgl_quadric_3d<T>& c);

//: Computes the eigensystem for real symmetric matrices. Used to classify quadrics.
template <class T, size_t n>
void eigen(T m[n][n], T l[n], T vc[n][n]);

//: Auxillary function used by eigen
template <class T, size_t n>
void twst(T m[n][n], T c, T s, int i, int j);

#define VGL_QUADRIC_3D_INSTANTIATE(T) extern "please include vgl/vgl_quadric_3d.hxx first"

#endif // vgl_quadric_3d_h_
