#include "rgrl_invariant_single_landmark.h"
#include "rgrl_trans_similarity.h"
#include "rgrl_scale.h"
#include "rgrl_cast.h"

#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_fixed.h>

rgrl_invariant_single_landmark::
rgrl_invariant_single_landmark(vnl_vector<double> location,
                               vnl_vector<double> vessel_dir1,
                               vnl_vector<double> vessel_dir2,
                               vnl_vector<double> vessel_dir3,
                               double width1, double width2, double width3,
                               double angular_std,
                               double width_ratio_std):
  location_(location),
  center_set_(false),
  is_estimate_set_(false)
{
  vcl_vector< vnl_vector<double> > vessel_dirs;
  vessel_dirs.push_back(vessel_dir1);
  vessel_dirs.push_back(vessel_dir2);
  vessel_dirs.push_back(vessel_dir3);

  local_widths_.push_back(width1);
  local_widths_.push_back(width2);
  local_widths_.push_back(width3);

  // Compute the radius of the region that the landmark occupies
  //
  radius_ = vnl_math_max(width1, vnl_math_max(width2, width3));

  // reorder the vessels directions, so that they're ordered in
  // increasing counter-clock-wise angles from the x-axis.
  //
  vcl_vector<double> angles;
  reorder_vessel(vessel_dirs, local_widths_, angles);

  // If the base trace direction is close to 0 degrees (below the threshold)
  // then this landmark is ambiguous
  //
  if ( angles[0] < angular_std*2)
    is_ambiguous_ = true;

  // Set boundary_points_ and trace_normals_ with the new vessel
  // ordering.
  //
  for (unsigned i = 0; i<3; i++ ) {
    vnl_double_2 normal = vnl_double_2(-vessel_dirs[i][1], vessel_dirs[i][0]);
    vnl_double_2 trace_pt = location_+radius_*vessel_dirs[i];

    boundary_points_.push_back( trace_pt-normal*local_widths_[i]/2.0 );
    boundary_points_.push_back( trace_pt+normal*local_widths_[i]/2.0 );
    trace_normals_.push_back(normal);
  }

  // Compute the invariants
  //
  angular_invariants_.set_size(3);
  angular_invariants_[0] = angles[0];
  angular_invariants_[1] = angles[1];
  angular_invariants_[2] = angles[2];

  // local_widths_ should not contain zero entries
  cartesian_invariants_.set_size(2);
  cartesian_invariants_[0] = vcl_atan(local_widths_[0]/local_widths_[1])*angular_std/width_ratio_std;
  cartesian_invariants_[1] = vcl_atan(local_widths_[0]/local_widths_[2])*angular_std/width_ratio_std;
}

rgrl_invariant_single_landmark::
rgrl_invariant_single_landmark(const rgrl_invariant_single_landmark& copy,
                               double angular_std,
                               double width_ratio_std)
  : rgrl_invariant(copy),
    location_(copy.location_),
    radius_(copy.radius_),
    center_set_(false)
{
  // copy the normal, width, and boundary points of each trace in the
  // signature the index of each is shifted by one
  for (unsigned int i = 0; i<3; i++) {
    trace_normals_.push_back(copy.trace_normals_[(i+1)%3]);
    local_widths_.push_back(copy.local_widths_[(i+1)%3]);
    boundary_points_.push_back(copy.boundary_points_[((i+1)%3)*2]);
    boundary_points_.push_back( copy.boundary_points_[((i+1)%3)*2+1]);
  }
  //compute invariants
  angular_invariants_.set_size(3);
  angular_invariants_[0] = copy.angular_invariants_[1];
  angular_invariants_[1] = copy.angular_invariants_[2];
  angular_invariants_[2] = copy.angular_invariants_[0];

  cartesian_invariants_.set_size(2);
  cartesian_invariants_[0] = vcl_atan(local_widths_[0]/local_widths_[1])*angular_std/width_ratio_std;
  cartesian_invariants_[1] = vcl_atan(local_widths_[0]/local_widths_[2])*angular_std/width_ratio_std;
}

const vnl_double_2&
rgrl_invariant_single_landmark::
location() const
{
  return location_;
}

const vnl_double_2&
rgrl_invariant_single_landmark::
boundary_point_location(int i) const
{
  assert(i >= 0 && unsigned(i)<boundary_points_.size());
  return boundary_points_[i];
}

const vnl_double_2&
rgrl_invariant_single_landmark::
boundary_point_normal(int i) const
{
  assert(i >= 0 && unsigned(i)<boundary_points_.size());
  return trace_normals_[i/2];
}

const vnl_vector<double>&
rgrl_invariant_single_landmark::
cartesian_invariants() const
{
  return cartesian_invariants_;
}

const vnl_vector<double>&
rgrl_invariant_single_landmark::
angular_invariants() const
{
  return angular_invariants_;
}

rgrl_mask_box
rgrl_invariant_single_landmark::
region() const
{
  double ratio = 5;
  vnl_double_2 x0(location_[0] - ratio*radius_, location_[1] - ratio*radius_);
  vnl_double_2 x1(location_[0] + ratio*radius_, location_[1] + ratio*radius_);

  return rgrl_mask_box(x0, x1);
}

bool
rgrl_invariant_single_landmark::
estimate(rgrl_invariant_sptr         from_inv,
         rgrl_transformation_sptr&   xform,
         rgrl_scale_sptr&            scale )
{
  rgrl_invariant_single_landmark* from = rgrl_cast<rgrl_invariant_single_landmark*>(from_inv);

  // Form the lhs and rhs matrices from the centered points.
  // Only compute the upper triangular part of the lhs matrix, then
  //  copy the results to the lower part to form the symmetrical matrix
  vnl_matrix_fixed<double,4,4> sum_prod(0.0);
  vnl_matrix_fixed<double,4,1> sum_rhs(0.0);

  unsigned int i=0;
  unsigned int num_boundary_points = 6;
  while (i < num_boundary_points) {
    double px, py, nx, ny, qx, qy;
    nx = this->boundary_point_normal(i)[0];
    ny = this->boundary_point_normal(i)[1];

    px = from->boundary_point_location(i)[0] - from->center()[0];
    py = from->boundary_point_location(i)[1] - from->center()[1];

    qx = this->boundary_point_location(i)[0] - this->center()[0];
    qy = this->boundary_point_location(i)[1] - this->center()[1];

    ++i;

    double elt0 = nx;
    double elt1 = ny;
    double elt2 = px*nx + py*ny;
    double elt3 = -py*nx + px*ny;
    sum_prod(0,0)+=elt0*elt0;     sum_prod(0,1)+=elt0*elt1;
    sum_prod(0,2)+=elt0*elt2;     sum_prod(0,3)+=elt0*elt3;
    sum_prod(1,1)+=elt1*elt1;     sum_prod(1,2)+=elt1*elt2;
    sum_prod(1,3)+=elt1*elt3;     sum_prod(2,2)+=elt2*elt2;
    sum_prod(2,3)+=elt2*elt3;     sum_prod(3,3)+=elt3*elt3;

    double u = nx*qx + ny*qy;
    sum_rhs(0,0)+=u*elt0;
    sum_rhs(1,0)+=u*elt1;
    sum_rhs(2,0)+=u*elt2;
    sum_rhs(3,0)+=u*elt3;
  }

  //for landmark center location(s)
  double px = from->location()[0] - from->center()[0];
  double py = from->location()[1] - from->center()[1];

  sum_prod(0,0)+=1;               sum_prod(0,1)+=0;
  sum_prod(0,2)+=px;              sum_prod(0,3)+=-py;
  sum_prod(1,1)+=1;               sum_prod(1,2)+=py;
  sum_prod(1,3)+=px;              sum_prod(2,2)+=px*px+py*py;
  sum_prod(2,3)+=0;               sum_prod(3,3)+=px*px+py*py;

  double qx = this->location()[0] - this->center()[0];
  double qy = this->location()[1] - this->center()[1];

  sum_rhs(0,0)+=qx;
  sum_rhs(1,0)+=qy;
  sum_rhs(2,0)+=px*qx+py*qy;
  sum_rhs(3,0)+=-py*qx+px*qy;

  // Fill in the lhs below the main diagonal.
  for ( int i=1; i<4; ++i )
    for ( int j=0; j<i; ++j )
      sum_prod(i,j) = sum_prod(j,i);


  // Find the scales and scale the matrices appropriate to normalize
  // them and increase the numerical stability.
  double factor0 = vnl_math_max(sum_prod(0,0),sum_prod(1,1));
  double factor1 = vnl_math_max(sum_prod(2,2),sum_prod(3,3));
  double norm_scale = vcl_sqrt(factor1 / factor0);   // neither should be 0

  vnl_double_4 s;
  s(2) = s(3) = 1; s(0) = s(1) = norm_scale;
  for ( int i=0; i<4; i++ ) {
    sum_rhs(i,0) *= s(i);
    for ( int j=0; j<4; j++ )
      sum_prod(i,j) *= s(i) * s(j);
  }

  // Estimate the transform
  vnl_matrix_fixed<double,4,4> norm_covar = vnl_inverse(sum_prod);
  vnl_matrix_fixed<double,4,1> c_params = norm_covar * sum_rhs;

  // Eliminate the scale of the sum_prod
  //
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      norm_covar(i,j) *= s(i) * s(j);
  }
  for ( int i=0; i<4; i++ ) {
    c_params(i,0) *= s(i);
  }

  // Eliminate the center
  //
  double tx = c_params(0,0)+this->center()[0]
              -c_params(2,0)*from->center()[0]
              +c_params(3,0)*from->center()[1];
  double ty = c_params(1,0)+this->center()[1]
              -c_params(3,0)*from->center()[0]
              -c_params(2,0)*from->center()[1];

  // Initial estimate of the transform (without proper scaling on the
  // covar matrix)
  //
  vnl_double_2 trans(tx, ty);
  vnl_matrix<double> A(2,2);
  A(0,0) = A(1,1) = c_params(2,0);
  A(0,1) = -c_params(3,0);
  A(1,0) = -A(0,1);
  rgrl_trans_similarity est_xform(A, trans, norm_covar);

  // Compute the scale
  //
  double obj = 0;
  double geometric_scale = 0;
  vnl_vector<double> mapped;
  for (unsigned int i = 0; i< num_boundary_points; i++) {
    est_xform.map_location(from->boundary_point_location(i), mapped);
    obj += vnl_math_sqr( dot_product((this->boundary_point_location(i) - mapped),
                                     this->boundary_point_normal(i)) );
  }
  est_xform.map_location(from->location(), mapped);
  obj += (location_ - mapped).squared_magnitude();
  //n = 8 for number of constraints
  //k = 4 for dof
  //geometric_scale = vcl_sqrt(obj/(n-k));
  geometric_scale = vcl_sqrt(obj/4);

  // Set the return parameters
  //
  xform = new rgrl_trans_similarity( est_xform.A(), est_xform.t(),
                                     est_xform.covar()*vnl_math_sqr(geometric_scale) );
  scale = new rgrl_scale;
  scale->set_geometric_scale( geometric_scale );

  return is_estimate_set_ = true;
}

const vnl_double_2&
rgrl_invariant_single_landmark::
center()
{
  if ( center_set_ ) return center_;

  //update center_
  double xc = location_[0];
  double yc = location_[1];

  for (unsigned int i = 0; i< boundary_points_.size(); i++) {
    xc += boundary_points_[i][0];
    yc += boundary_points_[i][1];
  }

  int size = int(boundary_points_.size() + 1);
  xc /= size; yc /= size;
  center_[0] = xc;
  center_[1] = yc;
  center_set_ = true;

  return center_;
}

//------------ Non-member Functions ----------------------------
void
rgrl_invariant_single_landmark::
reorder_vessel(vcl_vector<vnl_vector<double> >& directions,
               vcl_vector<double>& local_widths,
               vcl_vector<double>& angles)
{
  // create a basis for the x-axis
  vnl_double_2 x_axis(1, 0);

  // make a copy of the old stuff
  vcl_vector<vnl_vector<double> > old_dirs = directions;
  vcl_vector<double> old_widths = local_widths;
  directions.clear();
  local_widths.clear();

  // calculate the counterclockwise angle between the basis and each
  // trace direction.
  angles.clear();
  for (int i = 0; i<3; ++i)
    angles.push_back( ccw_angle_between(x_axis, old_dirs[i]) );

  // make a copy of the angles before sorting, and sort the angles
  vcl_vector<double> old_angles(angles);
  vcl_sort(angles.begin(), angles.end());

  // re-assign the directions
  for (int i=0; i<3; ++i){
    for (int j=0; j<3; ++j){
      if (angles[i]==old_angles[j]){
        directions.push_back(old_dirs[j]);
        local_widths.push_back(old_widths[j]);
        break;
      }
    }
  }
}

double
rgrl_invariant_single_landmark::
ccw_angle_between(vnl_double_2 from, vnl_double_2 to)
{
  double cosine = (from[0]*to[0] + from[1]*to[1])/
                  (from.magnitude()*to.magnitude()); //normalize
  double angle = vcl_acos(cosine);
  double z = from[0]*to[1] - from[1]*to[0];
  if (z >= 0)
    return angle;
  else
    return vnl_math::pi*2 - angle;
}
