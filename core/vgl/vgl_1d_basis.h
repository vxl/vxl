// This is core/vgl/vgl_1d_basis.h
#ifndef vgl_1d_basis_h_
#define vgl_1d_basis_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief storage for 3 collinear points to serve as 1-D projective basis
//
// vgl_1d_basis<T> is a class templated on the point type that
// will be stored.  Typically, T will be vgl_point_2d<double> or
// vgl_homg_point_2d<int> or even vgl_line_2d<float> or vgl_point_3d<T>.
//
// This class stores three unequal collinear points and will use these to
// define a projection from e.g. 2d points on a line to a 1-dimensional
// projective space: the first point receives coordinates (0,1), the second
// one (1,0), and the third one (the unit point) coordinates (1,1).
//
// Note that this class can also be used for ``projecting'' from e.g.
// vgl_homg_point_1d<float> or vgl_point_1d<int> to vgl_homg_point_1d<double>,
// i.e., a kind of type casting operation, by passing it the homogeneous
// points (0,1), (1,0) and (1,1).  Note that the destination space is
// always vgl_homg_point_1d<double>.
//
// The only conditions on template type T are the following:
// - It must know the concept of collinearity, viz. there must be a function
//   ``bool collinear(T,T,T)''.
//   (This is not necessary when using the constructor with two arguments.)
// - There must be a function ``double cross_ratio(T,T,T,T)'', or alternatively
//   (when using the constructor with two arguments) there must be a function
//   ``double ratio(T,T,T)''.  These functions should return 0 if the last
//   argument equals the 2-but-last and 1 if it equals the 1-but-last.
//
// These conditions are satisfied for the vgl_point_[23]d<Type> and
// vgl_homg_point_[123]d<Type> classes.  For concurrent vgl_line_2d's
// the method project() has to be specialised (which is easily done by
// taking its intersection with the line at infinity!)
//
// \author Peter Vanroose
// \date   7 July, 2001
//
// \verbatim
// Modifications
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vgl/vgl_fwd.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

//----------------------------------------------------------------------

//: Storage for 3 collinear points to serve as 1-D projective basis.
// This class is templated on the point type that will be stored.
// Typically, T will be vgl_point_2d<double> or
// vgl_homg_point_2d<int> or even vgl_line_2d<float> or vgl_point_3d<T>.
//
// This class stores three unequal collinear points and will use these to
// define a projection from e.g. 2d points on a line to a 1-dimensional
// projective space: the first point receives coordinates (0,1), the second
// one (the unit point) (1,1), and the third one (point at infinity) (1,0).
//
// Note that this class can also be used for ``projecting'' from e.g.
// vgl_homg_point_1d<float> or vgl_point_1d<int> to vgl_homg_point_1d<double>,
// i.e., a kind of type casting operation, by passing it the homogeneous
// points (0,1), (1,1) and (1,0).  Note that the destination space is
// always vgl_homg_point_1d<double>.
//
// The only conditions on template type T are the following:
// - It must know the concept of collinearity, viz. there must be a function
//   ``bool collinear(T,T,T)''.
//   (This is not necessary when using the constructor with two arguments.)
// - There must be a function ``double cross_ratio(T,T,T,T)'', or alternatively
//   (when using the constructor with two arguments) there must be a function
//   ``double ratio(T,T,T)''.  These functions should return 0 if the last
//   argument equals the 2-but-last and 1 if it equals the 1-but-last.
//
// These conditions are satisfied for the vgl_point_[23]d<Type> and
// vgl_homg_point_[123]d<Type> classes.  For concurrent vgl_line_2d's
// the method project() has to be specialised (which is easily done by
// taking its intersection with the line at infinity!)
//
template <class T>
class vgl_1d_basis
{
  // Data members are private:
  T origin_;    //!< The point to be mapped to homogeneous (0,1)
  T unity_;     //!< The point to be mapped to homogeneous (1,1)
  T inf_pt_;    //!< The point to be mapped to homogeneous (1,0)
  bool affine_; //!< normally false; if true, inf_pt_ is not used: affine basis
  // No usable default constructor:
  inline vgl_1d_basis() {}

 public:
  inline T origin() const { return origin_; }
  inline T unity() const { return unity_; }
  inline T inf_pt() const { assert(!affine_); return inf_pt_; }
  inline bool affine() const { return affine_; }
  inline bool projective() const { return !affine_; }

  //: Construct from three collinear points (projective basis).
  // It will serve as origin (0,1), unity (1,1) and point at infinity (1,0).
  // The points must be collinear, and different from each other.
  //
  // Note that there is no valid default constructor, since any sensible default
  // heavily depends on the structure of the point class T, the template type.
  //
  // Note that there is no way to overwrite an existing vgl_basis_1d;
  // just create a new one if you need a different one.
  // Hence it is not possible to read a vgl_basis_1d from stream with >>.
  //
  inline vgl_1d_basis(T const& o, T const& u, T const& i)
    : origin_(o), unity_(u), inf_pt_(i), affine_(false)
  { assert(collinear(o,i,u) && o!=i && o!=u && i!=u); }

  //: Construct from two points (affine basis).
  // It will serve as origin (0,1) and unity point (1,1).
  // The points must be different from each other, and not at infinity.
  // This creates an affine basis, i.e., the point at infinity of the basis
  // will be the point at infinity of the line o-u in the source space.
  vgl_1d_basis(T const& o, T const& u);

  //: Projection from a point in the source space to a 1-D homogeneous point
  vgl_homg_point_1d<double> project(T const& p);
};

//  +-+-+ 1d_basis simple I/O +-+-+

//: Write "<vgl_1d_basis o u i> " to stream
// \relates vgl_1d_basis
template <class T> vcl_ostream& operator<<(vcl_ostream& s, vgl_1d_basis<T> const&);

#define VGL_1D_BASIS_INSTANTIATE(T) extern "please include vgl/vgl_1d_basis.txx first"

#endif // vgl_1d_basis_h_
