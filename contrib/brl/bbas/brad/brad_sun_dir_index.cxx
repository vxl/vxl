#include "brad_sun_dir_index.h"
//
#include <brad/brad_sun_pos.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>

brad_sun_dir_index::brad_sun_dir_index(double longitude_deg, double latitude_deg,
                                       int obs_year_start, int obs_hour,
                                       int obs_min, int obs_time_range_min,
                                       int sampling_interval_years,
                                       int bin_radius)
  : longitude_deg_(longitude_deg), latitude_deg_(latitude_deg),
    bin_radius_(bin_radius)
{
  // form histogram from randomly sampled sundirections over
  brad_sun_direction_hist(obs_year_start, obs_hour, obs_min, obs_time_range_min,
                          sampling_interval_years, 0,
                          longitude_deg_, latitude_deg_, hist_);
  // get bounds on distribution
  double mean_az, mean_el;
  hist_.mean(mean_az, mean_el);
  vnl_matrix_fixed<double, 2, 2> covar = hist_.covariance_matrix();
  // the eigensystem of the covariance matrix
  vnl_symmetric_eigensystem<double> es(covar);
  vnl_vector<double> major_v = es.get_eigenvector(1);
  double major_sd = std::sqrt(es.get_eigenvalue(1));
  //the extent of each bin along the max eigenvector direction
  double dl_major = 1.75*major_sd/(bin_radius+0.5);
  cone_half_angle_ = dl_major/2.0;
  int map_index = 0;
  cone_axes_.resize(2*bin_radius + 1);
  for (int i = -bin_radius; i<=bin_radius; ++i, map_index++) {
    //compute cone centers
    double dl = i*dl_major;
    double az = mean_az + dl*major_v[0];
    double el = mean_el + dl*major_v[1];
    double x, y, z;
    hist_.convert_to_cartesian(az, el, x, y, z);
    vnl_double_3 axis(x, y, z);
    cone_axes_[map_index]=axis;
  }
}

int brad_sun_dir_index::index(double geo_sun_azimuth,
                              double geo_sun_elevation,double & min_angle)
{
  //convert to standard spherical coordinates
  double sphere_az = 90.0 - geo_sun_azimuth + 360.0;
  if (sphere_az>=360.0) sphere_az -= 360.0;
  double sphere_el = 90.0 - geo_sun_elevation;
  double az_rad = vnl_math::pi_over_180*sphere_az;
  double el_rad = vnl_math::pi_over_180*sphere_el;
#if 0 // unused
  double half_ang = vnl_math::pi_over_180*cone_half_angle_;
#endif
  // convert to Cartesian
  double s = std::sin(el_rad);
  double x = s*std::cos(az_rad);
  double y = s*std::sin(az_rad);
  double z = std::cos(el_rad);
  vnl_double_3 dir(x, y, z);
  min_angle = std::fabs(angle(cone_axes_[0], dir));
  int min_i = 0;
  for (unsigned i=1; i< n_sun_dir_bins(); ++i) {
    double ang  = std::fabs(angle(cone_axes_[i], dir));
    if (ang<min_angle) {
      min_angle = ang;
      min_i = i;
    }
  }
#ifdef DEBUG
  std::cout << "min dir angle["<< min_i << "]: " << min_ang*vnl_math::deg_per_rad
           << " (" << x << ' ' << y << ' ' << z << ")\n";
#endif
  return min_i;
}

int brad_sun_dir_index::index(double x, double y, double z,double &  /*min_angle*/) const
{
  vnl_double_3 dir(x, y, z);
  double min_ang = std::fabs(angle(cone_axes_[0], dir));
  int min_i = 0;
  for (unsigned i=1; i< n_sun_dir_bins(); ++i) {
    double ang  = std::fabs(angle(cone_axes_[i], dir));
    if (ang<min_ang) {
      min_ang = ang;
      min_i = i;
    }
  }
#ifdef DEBUG
  std::cout << "min dir angle["<< min_i << "]: " << min_ang*vnl_math::deg_per_rad << '\n';
#endif
  return min_i;
}

std::vector<vnl_double_3> brad_sun_dir_index::major_path()
{
  std::vector<vnl_double_3> ret;
  double mean_az, mean_el;
  hist_.mean(mean_az, mean_el);
  vnl_matrix_fixed<double, 2, 2> covar = hist_.covariance_matrix();
  // the eigensystem of the covariance matrix
  vnl_symmetric_eigensystem<double> es(covar);
  vnl_vector<double> major_v = es.get_eigenvector(1);
  double major_sd = std::sqrt(es.get_eigenvalue(1));
  //the extent of each bin along the max eigenvector direction
  double dl_major = 2.0*major_sd/(bin_radius_+0.5);
  for (double r = -bin_radius_; r<=bin_radius_; r+=2.0/30.0) {
    //compute cone centers
    double dl = r*dl_major;
    double az = mean_az + dl*major_v[0];
    double el = mean_el + dl*major_v[1];
    double x, y, z;
    hist_.convert_to_cartesian(az, el, x, y, z);
    ret.emplace_back(x, y, z);
  }
  return ret;
}

void brad_sun_dir_index::print_to_vrml(std::ostream& os)
{
  //output the sun dir histogram, also encodes the vrml header
  hist_.print_to_vrml(os, 0.25);
  std::vector<vnl_double_3> path = this->major_path();
  for (auto & vit : path) {
    double x = vit[0], y = vit[1], z = vit[2];
    os<< "Transform {\n"
      << "translation " << x << ' ' << y << ' '
      << ' ' << z << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << 1 << ' ' << 0 << ' ' << 0 << '\n'
      << "      transparency " << 0 << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Sphere\n"
      <<   "{\n"
      << "  radius " << 0.02 << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
  }
  for (unsigned i = 0; i<n_sun_dir_bins(); ++i) {
    vnl_double_3 a = this->cone_axis(i);
    double x = a[0], y = a[1], z = a[2];
    os<< "Transform {\n"
      << "translation " << x << ' ' << y << ' '
      << ' ' << z << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << 0 << ' ' << 0 << ' ' << 1 << '\n'
      << "      transparency " << 0 << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Sphere\n"
      <<   "{\n"
      << "  radius " << 0.03 << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
  }
}

std::ostream&  operator<<(std::ostream& s, brad_sun_dir_index const& bdi)
{
  s << "Earth position ( longitude:" << bdi.longitude_deg()<< " latitude:"
    << bdi.latitude_deg() << " )\n"
    << " n bins " << bdi.n_sun_dir_bins() << " bin centers ==>\n";
  for (unsigned i = 0; i<bdi.n_sun_dir_bins(); ++i)
    s << "axis[" << i << "]( " << bdi.cone_axis(i) << " )\n";
  return s;
}
