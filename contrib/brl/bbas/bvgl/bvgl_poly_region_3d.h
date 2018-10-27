// This is brl/bbas/bvgl/bvgl_poly_region_3d.h
#ifndef bvgl_poly_region_3d_h_
#define bvgl_poly_region_3d_h_
//:
// \file
// \brief A 3-d plane surface bounded by a parametric polynomial
// \author January 10, 2016 J.L. Mundy
//

#include <vector>
#include <iosfwd>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_vector.h>

template <class Type>
class bvgl_poly_region_3d
{
 public:
  //: Default constructor (creates empty poly_region_3d)
 bvgl_poly_region_3d():tolerance_(Type(0.5)){}

  //: Construct from parametric polynomial coeficients and the 3-d plane in which the curve lies
  bvgl_poly_region_3d(vnl_vector<Type> const& u_coefs, vnl_vector<Type> const& v_coefs,  vgl_vector_3d<Type> const& normal,
                      vgl_point_3d<Type> const& origin, Type tolerance = 0.5);

  vgl_vector_3d<Type> normal() const {return unit_normal_;}
  Type u(Type t) const;
  Type v(Type t) const;
  vgl_point_3d<Type> p(Type t) const;

  bool in(vgl_point_3d<Type> const& p) const;

  bool signed_distance(vgl_point_3d<Type> const& p, Type& dist) const;

  vgl_point_3d<Type> centroid() const;

  vgl_box_2d<Type> bounding_box_2d() const;

  vgl_box_3d<Type> bounding_box_3d() const;

 vgl_pointset_3d<Type> random_pointset(unsigned n_pts) const;

  void set_point_positive(vgl_point_3d<Type> const& p_pos);

 protected:
  Type tolerance_;
  vgl_point_3d<double> origin_;
  vgl_vector_3d<Type> unit_normal_;
  vgl_plane_3d<Type> plane_;
  vgl_polygon<Type> poly_2d_;  // to test inside

  //cache plane coordinate vectors for convenience
  vgl_vector_3d<Type> u_vec_;
  vgl_vector_3d<Type> v_vec_;

  //: the monomials
  vnl_vector<Type> mono(Type t, unsigned deg) const;
  //: the coefficients
  vnl_vector<Type> u_coefs_;
  vnl_vector<Type> v_coefs_;
};

#define BVGL_POLY_REGION_3D(T) extern "please include bbas/bvgl/bvgl_poly_region_3d.hxx first"

#endif // bvgl_poly_region_3d_h_
