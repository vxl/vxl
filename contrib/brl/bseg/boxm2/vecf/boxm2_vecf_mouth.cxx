#include <cstdlib>
#include <cmath>
#include "boxm2_vecf_mouth.h"

boxm2_vecf_mouth::boxm2_vecf_mouth(std::vector<vgl_point_3d<double> > const& knots){
  sup_ = bvgl_spline_region_3d<double>(knots, 1.0);
  inf_ = bvgl_spline_region_3d<double>(knots, 1.0);
  // set the sense of the normals so that points inside the mouth
  // have positive signed distances from both region planes
  vgl_point_3d<double> cent_sup = sup_.centroid();
  vgl_point_3d<double> cent_inf = cent_sup;
  cent_sup.set(cent_sup.x(), cent_sup.y()-1.0, cent_sup.z());
  cent_inf.set(cent_inf.x(), cent_inf.y()+1.0, cent_inf.z());
  sup_.set_point_positive(cent_sup);
  inf_.set_point_positive(cent_inf);
}

boxm2_vecf_mouth::boxm2_vecf_mouth(vgl_pointset_3d<double>  const& ptset){
  *this = boxm2_vecf_mouth(ptset.points());
}
//
// rotate the plane of the lower lip accoding to the
// angle of the mandible. Defines the extreme lower bound
// on the mouth region
//
void boxm2_vecf_mouth::rotate_inf(){
  std::vector<vgl_point_3d<double> > knots = sup_.knots();
  // rotate the knots of the sup rather than current inf knots
  // thus an invariant starting point before rotation
  std::vector<vgl_point_3d<double> > rot_knots;
  for(auto & knot : knots){
    vgl_point_3d<double> rp = rot_*knot;
    rot_knots.push_back(rp);
  }
  inf_ = bvgl_spline_region_3d<double>(rot_knots, 1.0);
  vgl_point_3d<double> cent_sup = sup_.centroid();
  // add to y to avoid degenerate offset above inf
  cent_sup.set(cent_sup.x(), cent_sup.y()+0.1, cent_sup.z());
  inf_.set_point_positive(cent_sup);
}

void boxm2_vecf_mouth::set_mandible_params(boxm2_vecf_mandible_params const& mand_params){
  mand_params_ = mand_params;
  vnl_vector_fixed<double,3> X(1.0, 0.0, 0.0);
  vnl_quaternion<double> Q(X,mand_params_.jaw_opening_angle_rad_);
  rot_ = vgl_rotation_3d<double>(Q);
  this->rotate_inf();
  vgl_box_3d<double> bb = this->bounding_box();
  params_.t_max_= params_.t(0.0, bb.min_y());
}

vgl_box_3d<double> boxm2_vecf_mouth::bounding_box() const{
  std::vector<vgl_point_3d<double> > sup_knots = sup_.knots();
  std::vector<vgl_point_3d<double> > inf_knots = inf_.knots();
  vgl_box_3d<double> ret;
  for(auto & sup_knot : sup_knots)
    ret.add(sup_knot);
  for(auto & inf_knot : inf_knots)
    ret.add(inf_knot);
  return ret;
}
//
// the shape of the mouth opening is controlled mainly by the
// orbicularis oris muscle. The shape of the opening is defined
// by a linear blend of two polynomials by the parameter t
// this function tests to see if the x,y position of a point
// lies inside the region defined by the two polynomials. The
// lower lip t = t_max is determined from the rotation angle of the mandible
// the upper lip t is defined to be zero. The x,y coordinates determine
// a t value so that the point lies on the blended polynomial. If
// t lies inside the bounds then the point is in the mouth opening.
//
bool boxm2_vecf_mouth::in_oris(vgl_point_3d<double> const& pt) const{
  double xp = 0.95*pt.x(), y = pt.y();
 if(xp<params_.x_min_ || xp>params_.x_max_)
    return false;
  double t = params_.t(xp, y);
  bool valid = valid_t(t);
  return valid;
}
// the idea here is to map the point, pt, backwards using a linear
// approximation to the rotational vector field of the mandible,
// that is,
//                     [ 1    0    0   ]
//      R_inv(theta) = [ 0    1  theta ]
//                     [ 0 -theta  1   ]
//
// the value of theta needed to map the point back to sup,
// (the mouth plane when theta == 0) can be found by solving the
// linear equation:
//
//       { a  b  c  d][         ][ x ]
//                    [ R(theta)][ y ] = 0
//                    [         ][ z ]
//                    [         ][ 1 ]
// The solution is,
//
//   theta  = -( ax + by +cz +d)/(bz -cy)
//
//  The plane is normalized below to insure good numerical conditioning
//  for the linear solution. Note that b ~ 1.0 and |c| << |b| so the
//  singular condition (bz - cy) == 0 occurs in plane, -cy + bz = 0,
//  approximately the z=0 plane passing through the x axis.
//
bool boxm2_vecf_mouth::in(vgl_point_3d<double> const& pt) const{
  double x = pt.x(), y = pt.y(), z = pt.z();
  const vgl_plane_3d<double>& plane = sup_.plane();
  double a = plane.a(), b = plane.b(), c = plane.c(), d = plane.d();
  double norm = std::sqrt(a*a + b*b + c*c);
  a/=norm; b/=norm; c/=norm; d/=norm;
  double theta = -(a*x + b*y + c*z + d)/(b*z - c*y);
  double theta_max = mand_params_.jaw_opening_angle_rad_;
  if(theta<=0.0 || theta>theta_max)
    return false;
  vgl_point_3d<double> inv_p(x, (y+theta*z), (z-theta*y));
  bool in = sup_.in(inv_p);
  if(in){
    bool in_o = this->in_oris(pt);
    return in_o;
  }
  return false;
}
//
// A randomly generated set of 3-d points that lie inside the mouth for debugging purposes
//
vgl_pointset_3d<double> boxm2_vecf_mouth::random_pointset(unsigned n_pts) const{
  // add sup and inf random pointsets
  vgl_pointset_3d<double> pts = sup_.random_pointset(n_pts);
  vgl_pointset_3d<double> inf_pts = inf_.random_pointset(n_pts);
  pts.append_pointset(inf_pts);
  vgl_box_3d<double> bb = this->bounding_box();
  // generate interior points
  unsigned n_req = 100*n_pts, niter =0;
  double xmin = bb.min_x(), xmax = bb.max_x();
  double ymin = bb.min_y(), ymax = bb.max_y();
  double zmin = bb.min_z(), zmax = bb.max_z();
  while(n_req>0 && niter < 10000*n_pts){
    double x = (xmax-xmin)*(static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)) + xmin;
    double y = (ymax-ymin)*(static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)) + ymin;
    double z = (zmax-zmin)*(static_cast<double>(std::rand())/static_cast<double>(RAND_MAX)) + zmin;
    vgl_point_3d<double> p(x, y, z);
    if(this->in(p)){
      if(p.x()>0.0)
        pts.add_point_with_normal(p, vgl_vector_3d<double>(-1.0, 0.0, 0.0));
      else
        pts.add_point_with_normal(p, vgl_vector_3d<double>(-1.0, 0.0, 0.0));
      n_req--;
    }else niter++;
  }
  if(n_req !=0)
    std::cout << "Warning! Insufficient number of points " << pts.npts() << " instead of " << n_pts << '\n';

  return pts;
}
