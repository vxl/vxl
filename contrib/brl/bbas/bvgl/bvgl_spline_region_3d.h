// This is brl/bbas/bvgl/bvgl_spline_region_3d.h
#ifndef bvgl_spline_region_3d_h_
#define bvgl_spline_region_3d_h_
//:
// \file
// \brief A 3-d plane surface bounded by a cubic spline lying in the plane
// \author December 20, 2015 J.L. Mundy
//

#include <vector>
#include <iostream>
#include <vgl/vgl_cubic_spline_3d.h>
#include <vgl/vgl_cubic_spline_2d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>

template <class Type>
class bvgl_spline_region_3d
{
 public:

  //: Default constructor (creates empty spline_region_3d)
 bvgl_spline_region_3d():tolerance_(Type(0.5)){};

  //: Construct using spline knots (must be closed curve)
 bvgl_spline_region_3d(std::vector<vgl_point_3d<Type> > const& knots, Type tolerance);

  //: Construct using spline knots as a pointset
 bvgl_spline_region_3d(vgl_pointset_3d<Type> const& ptset, Type tolerance);

 //: construct from a 2-d spline a 3-d origin and plane normal
 // the spline in 3-d is a planar curve defined by the 2-d spline and the plane
 bvgl_spline_region_3d(std::vector<vgl_point_2d<Type> > const& knots, vgl_vector_3d<Type> const& normal,
                       vgl_point_3d<Type> const& origin, Type tolerance);

 //: set point positve, the plane is oriented so the specified point has a positive distance.
  void set_point_positive(vgl_point_3d<Type> const& p_pos);

  //: mapping from/to the spline plane coordinates
  void plane_to_world(Type u, Type v, vgl_point_3d<Type>& p3d) const;
  bool world_to_plane(vgl_point_3d<Type>, Type& u, Type& v, Type tolerance) const;

  //: plane coordinate vectors
  void plane_coordinate_vectors(vgl_vector_3d<Type>& uvec, vgl_vector_3d<Type>& vvec) const{
    uvec = u_vec_; vvec = v_vec_;}

  //: is a point inside the region (on the plane and within or on the spline boundary
  bool in(vgl_point_3d<Type> const& p3d) const;

  //: signed distance. If the closest planar is ::in return true, otherwise false
  bool signed_distance(vgl_point_3d<Type> const& p, Type& dist) const;

  //: closest point in the region to p, including on the boundary
  vgl_point_3d<Type> closest_point(vgl_point_3d<Type> const& p) const;

  Type max_t() const {return spline_2d_.max_t();}
  //: a point on the spline curve at parameter t
  vgl_point_3d<Type> operator () (Type t) const;

  //: centroid of the region
  vgl_point_3d<Type> compute_centroid() const;
  vgl_point_2d<Type> compute_centroid_2d() const;

  vgl_point_3d<Type> centroid() const{return centroid_3d_;}
  vgl_point_2d<Type> centroid_2d() const{return centroid_2d_;}

  //: area of the region
  Type area() const;

  //: plane normal
  vgl_vector_3d<Type> normal() const{ return unit_normal_;}

  //: plane
  vgl_plane_3d<Type> plane() const {return plane_;}

  //: bounding box methods
  vgl_box_2d<Type> bounding_box_2d() const;
  vgl_box_3d<Type> bounding_box() const;

  //: accessors
  std::vector<vgl_point_3d<Type> > knots() const {return spline_3d_.knots();}
  std::vector<vgl_point_2d<Type> > knots_2d() const {return spline_2d_.knots();}
  vgl_point_3d<Type> origin() const {return origin_;}

  //: set parameters for scaling for efficiency. apply these calls before computing vector field values
  // if |L1| == 0 then the principal axis is u.
  void set_principal_eigenvector(vgl_vector_3d<Type> const& L1);
  void set_principal_offset(Type principal_offset){principal_offset_ = principal_offset;}
  void set_deformation_eigenvalues(Type su, Type sv){su_ = su; sv_ = sv;}
  void set_offset_vector(vgl_vector_3d<Type> const& tv){tv_ = tv;}
  //: inverse vector  field for the scale transformation
  bool inverse_vector_field(vgl_point_3d<Type> const& p, vgl_vector_3d<Type>& inv, Type tolerance = Type(-1)) const;

  //: forward vector field, used to couple the deformation field to adjacent structures
  bool vector_field(vgl_point_3d<Type> const& p, vgl_vector_3d<Type>& vf, vgl_vector_3d<Type> const& tv) const;

  //: scale the boundary isotropically around the centroid and translate by vector tv
  // useful for generalized cylinder applications
  bvgl_spline_region_3d<Type> scale(Type s,vgl_vector_3d<Type> const& tv) const;

  //: anisotropically scale the boundary about the centroid with principal axis L1 and translate by vector tv
  // if |L1| == 0 then the principal axis is u.
  bvgl_spline_region_3d<Type> scale(Type su, Type sv, vgl_vector_3d<Type> const& tv, vgl_vector_3d<Type> const& L1, bool verbose = false)const;

  // for debug purposes
  // generate a random poinset drawn from the region
  vgl_pointset_3d<Type> random_pointset(unsigned n_pts) const;
 private:
  Type tolerance_;
  vgl_polygon<Type> poly_2d_;  // to test inside
  vgl_point_2d<Type> centroid_2d_;
  vgl_point_3d<Type> centroid_3d_;
  vgl_cubic_spline_2d<Type> spline_2d_;
  vgl_cubic_spline_3d<Type> spline_3d_;
  vgl_plane_3d<Type> plane_;
  // a point on the plane to support some forms of construction
  vgl_point_3d<Type> origin_;
  // contained in plane but for efficiency cache unit vector
  vgl_vector_3d<Type> unit_normal_;
  //cache plane coordinate vectors for convenience
  vgl_vector_3d<Type> u_vec_;
  vgl_vector_3d<Type> v_vec_;
  Type principal_offset_;
  // parameters for deformation
  Type su_; //first principal eigenvalue
  Type sv_; //second principal eigenvalue
  vgl_vector_3d<Type> tv_;//translation vector
  Type sang_; //principal_axis_sine
  Type cang_; // principal_axis_cosine;
};

#define BVGL_SPLINE_REGION_3D(T) extern "please include bbas/bvgl/bvgl_spline_region_3d.hxx first"

#endif // bvgl_spline_region_3d_h_
