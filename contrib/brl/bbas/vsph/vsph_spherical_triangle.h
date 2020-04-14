#ifndef vsph_spherical_triangle_h_
#define vsph_spherical_triangle_h_
//:
// \file
// \brief  A triangle on the sphere - useful as a spherical index cell
// \author J. L. Mundy
// \date February 21, 2020
//
// \verbatim
//  Modifications
//   None
// \endverbatim
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <iosfwd>
#include "vsph_sph_point_3d.h"
#include "vsph_spherical_coord.h"
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_triangle_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// the containment predicate is approximated by the planar chord triangle. Typically the distance from the plane
// and the spherical surface is negligble, so the full spherical test isn't worth the extra computation - lots of trig
class vsph_spherical_triangle
{
 public:
  //: default constructor
 vsph_spherical_triangle():cell_index_(unique_id_++){}

  //: constructor,
 vsph_spherical_triangle(vsph_sph_point_3d const& v0, vsph_sph_point_3d const& v1,vsph_sph_point_3d const& v2,
                         vsph_spherical_coord& sph_cs){
   set(v0, v1, v2, sph_cs);
 }
  void set(vsph_sph_point_3d const& v0, vsph_sph_point_3d const& v1,vsph_sph_point_3d const& v2, vsph_spherical_coord& sph_cs){
    v0_ = v0; v1_ = v1; v2_=v2;
    v0_cart_ = sph_cs.cart_coord(v0_);    v1_cart_ = sph_cs.cart_coord(v1_);    v2_cart_ = sph_cs.cart_coord(v2_);
    pl_ = vgl_plane_3d<double>(v0_cart_, v1_cart_, v2_cart_);
    pl_.normalize();
    cell_index_ = unique_id_++;
  }
  //: destructor
  ~vsph_spherical_triangle() = default;

  // center of the planar chord triangle
  vsph_sph_point_3d centroid(vsph_spherical_coord & scs) const{
    double avg_x = (v0_cart_.x() + v1_cart_.x() + v2_cart_.x())/3.0;
    double avg_y = (v0_cart_.y() + v1_cart_.y() + v2_cart_.y())/3.0;
    double avg_z = (v0_cart_.z() + v1_cart_.z() + v2_cart_.z())/3.0;
    vsph_sph_point_3d sp;
    scs.spherical_coord(vgl_point_3d<double>(avg_x, avg_y, avg_z), sp);
    scs.move_point(sp);
    return sp;
  }

  // is the point inside the spherical triangle - approximated by
  // the planar chord triangle
  bool contains(vsph_sph_point_3d const& pt, vsph_spherical_coord& scs) const{
    vsph_sph_point_3d moved_pt = pt;
    scs.move_point(moved_pt);
    // multiple of the chord length
    double tol = 5.0*vgl_distance(v0_cart_, v1_cart_);
    // project point onto planar triangle
    vgl_point_3d<double> pt_cart = scs.cart_coord(moved_pt);
    vgl_point_3d<double> cp = vgl_closest_point(pt_cart, pl_);
    double d = vgl_distance(pt_cart, cp);
    if (d > tol)
      return false;
    // the planar containment test
    return vgl_triangle_3d_test_inside(cp, v0_cart_, v1_cart_,v2_cart_);
  }
  // A unique identifier for a triangle instance
  size_t cell_index() const {return cell_index_;}

  // spherical coordinates of the vertices
  vsph_sph_point_3d v0() const { return v0_; }
  vsph_sph_point_3d v1() const { return v1_; }
  vsph_sph_point_3d v2() const { return v2_; }

  // Cartesian coordinates of the vertices
  vgl_point_3d<double> v0_cart() const {return v0_cart_;}
  vgl_point_3d<double> v1_cart() const {return v1_cart_;}
  vgl_point_3d<double> v2_cart() const {return v2_cart_;}
 private:
  static size_t unique_id_;
  size_t cell_index_;
  vsph_sph_point_3d v0_;
  vsph_sph_point_3d v1_;
  vsph_sph_point_3d v2_;
  vgl_point_3d<double> v0_cart_;
  vgl_point_3d<double> v1_cart_;
  vgl_point_3d<double> v2_cart_;
  vgl_plane_3d<double> pl_;
};
#endif //spherical triangle
