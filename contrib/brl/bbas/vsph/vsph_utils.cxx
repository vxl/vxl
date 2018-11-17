#include "vsph_utils.h"
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// compute b-a on the circle
double vsph_utils::azimuth_diff(double a, double b,
                                bool in_radians)
{
  double pye = in_radians?vnl_math::pi:180.0;
  double two_pye = 2.0*pye;
  double diff = b-a;
  diff = (diff>pye)?(diff-two_pye):diff;
  diff = (diff<-pye)?(diff+two_pye):diff;
  return diff;
}
double vsph_utils::azimuth_sum(double a, double b, bool in_radians){
  double pye = in_radians?vnl_math::pi:180.0;
  double two_pye = 2.0*pye;
  double sum = a + b;
  sum = (sum>pye)?(sum-two_pye):sum;
  sum = (sum<-pye)?(sum+two_pye):sum;
  return sum;
}
void vsph_utils::half_angle(double phi_a, double phi_b, double& ang_1,
                            double& ang_2, bool in_radians)
{
  double pye = in_radians?vnl_math::pi:180.0;
  double two_pye = 2.0*pye;
  double hd = 0.5*(phi_b - phi_a);
  ang_1 = phi_a + hd;
  if (ang_1>pye) ang_1 -=two_pye;
  else if (ang_1< -pye) ang_1 += two_pye;
  ang_2 = ang_1 + pye;
  if (ang_2>pye) ang_2 -=two_pye;
  else if (ang_2< -pye) ang_2 += two_pye;
}

bool vsph_utils::azimuth_in_interval(double phi, double a_phi, double b_phi,
                                     double c_phi, bool in_radians)
{
  double pye = in_radians?vnl_math::pi:180.0;
  // small interval contains 180
  if (a_phi >0 && b_phi<0) {
    // is the interval the short interval?
    if ((c_phi> a_phi && c_phi<=pye) ||
        (c_phi >= -pye && c_phi < b_phi)) {
      //yes
      bool in = (phi>= a_phi && phi<=pye) ||
                (phi >= -pye && phi <= b_phi);
      return in;
    }
    // is the interval the long interval
    if ((c_phi< a_phi && c_phi>=0.0) ||
        (c_phi < 0 && c_phi > b_phi)) {
      //yes
      bool in = (phi<= a_phi && phi>=0.0) ||
                (phi <= 0 && phi >= b_phi);
      return in;
    }
    // can't happen
    assert(false);
    return false;
  }
  // small interval doesn't contain 180
  if (c_phi > a_phi && c_phi < b_phi)  // small interval
    return phi >= a_phi && phi <= b_phi;
  else //long interval
    return (phi<=a_phi && phi>=-pye ) ||
      (phi<=pye && phi>=b_phi);
}

double vsph_utils::arc_len(double min_ph, double max_ph, double c_ph)
{
  double dif = std::fabs(vsph_utils::azimuth_diff(min_ph, c_ph, true));
  dif += std::fabs(vsph_utils::azimuth_diff(c_ph, max_ph, true));
  return dif;
}

double vsph_utils::sph_inter_area(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2)
{
  bool in_radians = true;
  double theta_min =
    b1.min_theta(in_radians) < b2.min_theta(in_radians) ?
    b2.min_theta(in_radians) : b1.min_theta(in_radians);

  double theta_max =
    b1.max_theta(in_radians) < b2.max_theta(in_radians) ?
    b1.max_theta(in_radians) : b2.max_theta(in_radians);
  // empty box.
  if (theta_max <= theta_min)
    return 0.0;
  double a = std::fabs(std::cos(theta_min)-std::cos(theta_max));

  double b1_a_phi = b1.a_phi(in_radians), b1_b_phi = b1.b_phi(in_radians),
    b1_c_phi = b1.c_phi(in_radians);
  double b2_a_phi = b2.a_phi(in_radians), b2_b_phi = b2.b_phi(in_radians),
    b2_c_phi = b2.c_phi(in_radians);

  double b2_min_ph = b2.min_phi(in_radians), b1_min_ph = b1.min_phi(in_radians);
  double b2_max_ph = b2.max_phi(in_radians), b1_max_ph = b1.max_phi(in_radians);

  bool b2min_in_b1 = vsph_utils::azimuth_in_interval(b2_min_ph, b1_a_phi, b1_b_phi, b1_c_phi,in_radians);
  bool b1min_in_b2 = vsph_utils::azimuth_in_interval(b1_min_ph, b2_a_phi, b2_b_phi, b2_c_phi,in_radians);
  bool b2max_in_b1 = vsph_utils::azimuth_in_interval(b2_max_ph, b1_a_phi, b1_b_phi, b1_c_phi,in_radians);
  bool b1max_in_b2 = vsph_utils::azimuth_in_interval(b1_max_ph, b2_a_phi, b2_b_phi, b2_c_phi, in_radians);

  unsigned short flags = 0;
  if (b2max_in_b1) flags = flags | 1;
  if (b2min_in_b1) flags = flags | 2;
  if (b1max_in_b2) flags = flags | 4;
  if (b1min_in_b2) flags = flags | 8;
  double ha1, ha2, dph =0.0;
  switch (flags)
  {
    // no intersection
    case 0:{
      return 0.0;
    }
      // b2 contained in b1
    case 3:{
      dph = vsph_utils::arc_len(b2_min_ph, b2_max_ph, b2_c_phi);
      return dph*a;
    }
      //  b2_min => b1_max
    case 6:{
      vsph_utils::half_angle(b2_min_ph, b1_max_ph, ha1, ha2, in_radians);
      //either ha1 or  ha2 has to be in both box intervals
      if (vsph_utils::azimuth_in_interval(ha1,b1_a_phi, b1_b_phi, b1_c_phi,in_radians)&& vsph_utils::azimuth_in_interval(ha1,b2_a_phi, b2_b_phi, b2_c_phi,in_radians))
        dph = vsph_utils::arc_len(b2_min_ph, b1_max_ph,ha1);
      else
        dph =  vsph_utils::arc_len(b2_min_ph, b1_max_ph, ha2);
      return dph*a;
    }
      //  b1_min => b2_max
    case 9:{
      vsph_utils::half_angle(b1_min_ph, b2_max_ph, ha1, ha2, in_radians);
      //eithr ha1 or ha2 has to be in both intervals
      if (vsph_utils::azimuth_in_interval(ha1, b1_a_phi, b1_b_phi, b1_c_phi,in_radians)&& vsph_utils::azimuth_in_interval(ha1, b2_a_phi, b2_b_phi, b2_c_phi,in_radians))
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph, ha1);
      else
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph, ha2);
      return dph*a;
    }
      // b1 contained in b2
    case 12:{
      dph = vsph_utils::arc_len(b1_min_ph, b1_max_ph, b1_c_phi);
      return dph*a;
    }

    // This condition produces two boxes (b1min=>b2max  b2min=>b1max)
    case 15:{
      double dph2 =0.0;
      vsph_utils::half_angle(b1_min_ph, b2_max_ph, ha1, ha2, in_radians);
      if (vsph_utils::azimuth_in_interval(ha1, b2_a_phi, b2_b_phi, b2_c_phi, in_radians)&& vsph_utils::azimuth_in_interval(ha1,b1_a_phi, b1_b_phi, b1_c_phi,in_radians))
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph,ha1);
      else
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph,ha2);

      vsph_utils::half_angle(b2_min_ph, b1_max_ph, ha1, ha2, in_radians);
      if (vsph_utils::azimuth_in_interval(ha1,b2_a_phi, b2_b_phi, b2_c_phi,in_radians) &&
          vsph_utils::azimuth_in_interval(ha1,b1_a_phi, b1_b_phi, b1_c_phi,in_radians))
        dph2 = vsph_utils::arc_len(b2_min_ph, b1_max_ph, ha1);
      else
        dph2 = vsph_utils::arc_len(b2_min_ph, b1_max_ph, ha2);
      return a*(dph+dph2);
    }
    default:
      std::cout << "IMPOSSIBLE INTERSECTION CONDITION NOT HANDLED!!\n";
      assert(false); //shouldn't happen
      return 0.0;
  }
}

double vsph_utils::distance_on_usphere(vsph_sph_point_2d const& a,
                                       vsph_sph_point_2d const& b)
{
  double dist = std::fabs(vsph_utils::azimuth_diff(a.phi_, b.phi_));
  dist += std::fabs(a.theta_-b.theta_);
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

void vsph_utils::
ray_spherical_coordinates(vpgl_perspective_camera<double> const& cam,
                          double u, double v,
                          double& elevation, double& azimuth,
                          const std::string& units)
{
  vgl_ray_3d<double> ray = cam.backproject_ray(u, v);
  vgl_vector_3d<double> dir = ray.direction();
  vgl_vector_3d<double> dirn = normalized(dir);
  double x = dirn.x(), y = dirn.y(), z = dirn.z();
  if (std::fabs(z-1.0)<1e-8) {//essentially at the North Pole
    azimuth = 0.0;
    elevation = 0.0;
    return;
  }
  if (std::fabs(z + 1.0)<1e-8) {//essentially at the South Pole
    azimuth = 0.0;
    elevation = 180.0;
    if (units == "radians")
      elevation = vnl_math::pi;
    return;
  }
  elevation = std::acos(z);
  // azimuth is zero pointing along x with positive angle rotating towards y
  azimuth = std::atan2(y, x);//returns angles with +-180 branch cut
  if (units == "degrees") {
    elevation = vnl_math::deg_per_rad*elevation;
    azimuth = vnl_math::deg_per_rad*azimuth;
  }
}

vgl_polygon<double> vsph_utils::
project_poly_onto_unit_sphere(vpgl_perspective_camera<double> const& cam,
                              vgl_polygon<double> const& image_poly,
                              const std::string& units)
{
  std::vector<std::vector<vgl_point_2d<double> > > sph_sheets;
  unsigned n_sh = image_poly.num_sheets();
  for (unsigned sh_idx = 0; sh_idx<n_sh; ++sh_idx) {
    std::vector<vgl_point_2d<double> > sheet = image_poly[sh_idx];
    std::vector<vgl_point_2d<double> > sph_sheet;
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

bool vsph_utils::read_ray_index_data(const std::string& path, std::vector<unsigned char>& data)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  int nrays;
  is >> nrays;
  if (nrays <= 0)
    return false;
  data.resize(nrays);
  for (int i = 0; i< nrays; ++i) {
    int temp;
    is >> temp ;
    data[i] = static_cast<unsigned char>(temp);
  }
  return true;
}

vsph_sph_box_2d vsph_utils::box_from_camera(vpgl_perspective_camera<double> const& cam, const std::string& units)
{
  bool in_radians = true;
  if (units == "degrees")
    in_radians = false;
  // extract the image bounds
  const vpgl_calibration_matrix<double>& K = cam.get_calibration();
  vgl_point_2d<double> pp = K.principal_point();
  double ni = 2.0*pp.x(), nj = 2.0*pp.y();
  double elevation, azimuth;
  vsph_utils::ray_spherical_coordinates(cam,0.0,0.0, elevation, azimuth, units);
  vsph_sph_point_2d p0(elevation, azimuth, in_radians);
  vsph_utils::ray_spherical_coordinates(cam, ni,0.0, elevation, azimuth, units);
  vsph_sph_point_2d p1(elevation, azimuth, in_radians);
  vsph_utils::ray_spherical_coordinates(cam, ni, nj, elevation, azimuth, units);
  vsph_sph_point_2d p2(elevation, azimuth, in_radians);
  vsph_utils::ray_spherical_coordinates(cam, 0.0, nj,elevation, azimuth, units);
  vsph_sph_point_2d p3(elevation, azimuth, in_radians);
  vsph_utils::ray_spherical_coordinates(cam, 0.5*ni, 0.5*nj, elevation, azimuth, units);
  // need center of image to define azimuthal interval (see vsph_sph_box_2d)
  vsph_sph_point_2d p4(elevation, azimuth, in_radians);
  vsph_sph_box_2d box(p1, p0, p4);
  box.add(p2); box.add(p3);
  return box;
}
