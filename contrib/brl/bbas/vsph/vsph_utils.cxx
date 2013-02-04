#include "vsph_utils.h"
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_math.h>

// compute b-a on the circle
double vsph_utils::azimuth_diff(double a, double b,
                                bool in_radians)
{
  double diff = b-a;
  if (!in_radians) {
    if (diff > 180)  diff -= 360;
    if (diff < -180) diff += 360;
  }
  else {
    if (diff >  vnl_math::pi) diff -= vnl_math::twopi;
    if (diff < -vnl_math::pi) diff += vnl_math::twopi;
  }
  return diff;
}

double vsph_utils::distance_on_usphere(vsph_sph_point_2d const& a,
                                       vsph_sph_point_2d const& b)
{
  double dist = vcl_fabs(vsph_utils::azimuth_diff(a.phi_, b.phi_));
  dist += vcl_fabs(a.theta_-b.theta_);
  return dist;
}

bool vsph_utils::a_eq_b(double a, double b, bool in_radians)
{
  if (!in_radians) {
    if (( a == -180 || a == 180 ) && ( b == 180 || b == -180 ))
      return true;
  }
  else {
    const double pi = vnl_math::pi;
    if (( a == -pi || a == pi ) && ( b == pi || b == -pi ))
      return true;
  }
  return a == b;
}

bool vsph_utils::a_lt_b(double phi_a, double phi_b, bool in_radians)
{
  double diff = vsph_utils::azimuth_diff(phi_a, phi_b, in_radians);
  return diff > 0; // a < b if (b-a) > 0
}

vsph_sph_box_2d vsph_utils::intersection(vsph_sph_box_2d const& b1,
                                         vsph_sph_box_2d const& b2)
{
  vsph_sph_box_2d rbox;

  double phi_min =
    vsph_utils::a_lt_b(b1.min_phi(),b2.min_phi()) ? b2.min_phi() : b1.min_phi();

  double phi_max =
    vsph_utils::a_lt_b(b1.max_phi(),b2.max_phi()) ? b1.max_phi() : b2.max_phi();

  double theta_min =
    b1.min_theta() < b2.min_theta() ? b2.min_theta() : b1.min_theta();

  double theta_max =
    b1.max_theta() < b2.max_theta() ? b1.max_theta() : b2.max_theta();

  //arbitrarily set units according to b1
  rbox.add(theta_min, phi_min, b1.in_radians());
  rbox.add(theta_max, phi_max, b1.in_radians());

  return rbox;
}
void vsph_utils::
ray_spherical_coordinates(vpgl_perspective_camera<double> const& cam,
                          double u, double v,
                          double& elevation, double& azimuth,
                          vcl_string units)
{
  vgl_ray_3d<double> ray = cam.backproject_ray(u, v);
  vgl_vector_3d<double> dir = ray.direction();
  vgl_vector_3d<double> dirn = normalized(dir);
  double x = dirn.x(), y = dirn.y(), z = dirn.z();
  if (vcl_fabs(z-1.0)<1e-8) {//essentially at the North Pole
    azimuth = 0.0;
    elevation = 0.0;
    return;
  }
  if (vcl_fabs(z + 1.0)<1e-8) {//essentially at the South Pole
    azimuth = 0.0;
    elevation = 180.0;
    if (units == "radians")
      elevation = vnl_math::pi;
    return;
  }
  elevation = vcl_acos(z);
  // azimuth is zero pointing along x with positive angle rotating towards y
  azimuth = vcl_atan2(y, x);//returns angles with +-180 branch cut
  if (units == "degrees") {
    elevation = vnl_math::deg_per_rad*elevation;
    azimuth = vnl_math::deg_per_rad*azimuth;
  }
}

vgl_polygon<double> vsph_utils::
project_poly_onto_unit_sphere(vpgl_perspective_camera<double> const& cam,
                              vgl_polygon<double> const& image_poly,
                              vcl_string units)
{
  vcl_vector<vcl_vector<vgl_point_2d<double> > > sph_sheets;
  unsigned n_sh = image_poly.num_sheets();
  for (unsigned sh_idx = 0; sh_idx<n_sh; ++sh_idx) {
    vcl_vector<vgl_point_2d<double> > sheet = image_poly[sh_idx];
    vcl_vector<vgl_point_2d<double> > sph_sheet;
    unsigned n_vert = sheet.size();
    for (unsigned vidx = 0; vidx<n_vert; ++vidx) {
      vgl_point_2d<double> vert = sheet[vidx];
      double elevation, azimuth;
      vsph_utils::ray_spherical_coordinates(cam, vert.x(), vert.y(),
                                                   elevation, azimuth, units);
      //maintain consistent coordinate order with the vsph library
      // elevation == x, azmiuth == y
      vgl_point_2d<double> sph_vert(elevation,azimuth);
      sph_sheet.push_back(sph_vert);
    }
    sph_sheets.push_back(sph_sheet);
  }
  return vgl_polygon<double>(sph_sheets);
}
bool vsph_utils::read_ray_index_data(vcl_string path, vcl_vector<unsigned char>& data){
  vcl_ifstream is(path.c_str());
  if(!is.is_open())
    return false;
  int nrays;
  is >> nrays;
  if(nrays <= 0)
    return false;
  data.resize(nrays);
  for(int i = 0; i< nrays; ++i)
    is >> data[i];
  return true;
}
