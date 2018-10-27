#include <string>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_ray_3d.h>
#include "boxm2_vecf_composite_face_params.h"
void boxm2_vecf_composite_face_params::compute_auxillary_pts(){
  // midpoint of line joining the left and right canthus
  vgl_vector_3d<double> r_to_l = left_lateral_canthus_-right_lateral_canthus_;
  vgl_point_3d<double> midp = right_lateral_canthus_+(0.5*r_to_l);
  canth_mid_vect_.set(midp.x(), midp.y(), midp.z());
  // affine-invariant point on forehead plane
  // construct a ray from the canthi midpoint parallel to the forehead
  // normal and then find the intersection with the forehead plane
  vgl_ray_3d<double> ray(midp, mid_forehead_normal_);
  vgl_plane_3d<double> pl(mid_forehead_normal_, mid_forehead_);
  bool success = vgl_intersection(ray, pl, forehead_intersection_);
  if(!success){
    std::cout << "Intersection of ray with forehead plane - failed\n";
  }
}
std::ostream&  operator << (std::ostream& s, boxm2_vecf_composite_face_params const& prc){
  if(!s){
    std::cout << "Bad parameter file stream\n";
    return s;
  }
  // bad practice --- but easy
  auto & pr = const_cast<boxm2_vecf_composite_face_params &>(prc);
  s << "affine_transform: " << pr.trans_ << '\n';
  return s;
}
std::istream&  operator >> (std::istream& s, boxm2_vecf_composite_face_params& pr){
  if(!s){
    std::cout << "Bad parameter file stream\n";
    return s;
  }
  std::string st;
  s >> st >> pr.trans_;
  return s;
}
