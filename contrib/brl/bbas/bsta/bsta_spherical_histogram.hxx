#ifndef bsta_spherical_histogram_hxx_
#define bsta_spherical_histogram_hxx_
//:
// \file
#include <iostream>
#include <cmath>
#include "bsta_spherical_histogram.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
//
// determine if an angle lies inside an interval that possibly contains
// the branch cut at 0/360
//
template <class T>
static bool in_interval(T start, T range, T angle)
{
  T end = start + range;
  bool no_cut = (end <= T(360));
  if (no_cut) {
    return angle >= start && angle < end;
  }
  // the interval contains the cut
  if (angle >= start && angle<=T(360))
    return true;
  T branch_end = end - T(360);
  if (angle >= 0 && angle < branch_end)
    return true;
  return false;
}

template <class T>
static T start_90_180(T start, T range)
{
  return -(start+range) + T(90);
}

// find the smallest distance between two azimuth angles, a and b
// if it is assumed that x0 < x1 then compare (x1-x0) with pi - (x1-x0)
// the smaller value is the difference. The sign of the difference is
// determined by whether it is necessary to switch a and b.
template <class T>
static T azimuth_diff(T a, T b, T two_pi)
{
  T x0 = a, x1 = b, sign = T(1);
  if (a>b) {x0 = b; x1 = a; sign = -sign;}
  T a1 = x1-x0;
  T a2 = two_pi - a1;
  if (a1>a2) return -sign*a2;
  return sign*a1;
}

template <class T>
bsta_spherical_histogram<T>::bsta_spherical_histogram()
  : n_azimuth_(0), n_elevation_(0),
    azimuth_start_(0), azimuth_range_(0),
    elevation_start_(0), elevation_range_(0),
    az_branch_cut_(B_180_180), el_poles_(B_90_90),
    units_(DEG), total_counts_valid_(false), total_counts_(0)
{
}

template <class T>
bsta_spherical_histogram<T>::
bsta_spherical_histogram(unsigned n_azimuth, unsigned n_elevation,
                         T azimuth_start, T azimuth_range, T elevation_start,
                         T elevation_range, ang_units units,
                         angle_bounds az_branch_cut, angle_bounds el_poles)
  : n_azimuth_(n_azimuth), n_elevation_(n_elevation),
    azimuth_start_(azimuth_start), azimuth_range_(azimuth_range),
    elevation_start_(elevation_start), elevation_range_(elevation_range),
    az_branch_cut_(az_branch_cut), el_poles_(el_poles),
    units_(units), total_counts_valid_(false), total_counts_(0)
{
  assert(n_azimuth>0&&n_elevation>0);
  // convert to degrees if necessary
  if (units_ == RADIANS) {
    azimuth_start_   = rad_to_deg(azimuth_start_);
    azimuth_range_   = rad_to_deg(azimuth_range_);
    elevation_start_ = rad_to_deg(elevation_start_);
    elevation_range_ = rad_to_deg(elevation_range_);
  }
  //convert azimuth branch_cut to 0-360 if necessary
  if (az_branch_cut == B_180_180)
    if (azimuth_start_<T(0))
      azimuth_start_ = azimuth_start_+ static_cast<T>(360);

  //convert elevation range to 0->180
  if (el_poles_ == B_90_90)
    elevation_start_ =
      start_90_180(elevation_start_,elevation_range_);

  delta_az_ = azimuth_range_/n_azimuth_;
  delta_el_ = elevation_range_/n_elevation_;
}

//: azimuth bounds
template <class T>
T bsta_spherical_histogram<T>::azimuth_start() const
{
  T az_start = azimuth_start_;
  if (az_branch_cut_==B_180_180)
    if (az_start >=T(180))
      az_start -= T(360);
  if (units_ == RADIANS)
    az_start = deg_to_rad(az_start);
  return az_start;
}

template <class T>
T bsta_spherical_histogram<T>::azimuth_range() const
{
  if (units_ == RADIANS)
    return deg_to_rad(azimuth_range_);
  return azimuth_range_;
}

//: elevation bounds
template <class T>
T bsta_spherical_histogram<T>::elevation_start() const
{
  T el_start = elevation_start_;
  if (el_poles_==B_90_90)
    el_start = start_90_180(el_start, elevation_range_);
  if (units_ == RADIANS)
    el_start = deg_to_rad(el_start);
  return el_start;
}

template <class T>
T bsta_spherical_histogram<T>::elevation_range() const
{
  if (units_ == RADIANS)
    return deg_to_rad(elevation_range_);
  return elevation_range_;
}

template <class T>
T bsta_spherical_histogram<T>::azimuth_center(int azimuth_index) const
{
  double start = azimuth_index*delta_az_ + azimuth_start_;
  double end = start + delta_az_;
  double mid = 0.5*(start + end);
  if (mid>=360.0)
    mid -= 360.0;
  if (az_branch_cut_==B_180_180 && mid > 180.0)
    mid -= 360.0;
  T middle = static_cast<T>(mid);
  if (units_ == RADIANS)
    return deg_to_rad(middle);
  return middle;
}

template <class T>
T bsta_spherical_histogram<T>::elevation_center(int elevation_index) const
{
  double start = elevation_index*delta_el_;
  double end = start + delta_el_;
  double mid = 0.5*(start + end);
  if (el_poles_ == B_90_90)
    mid = 90.0-mid;
  T middle = static_cast<T>(mid);
  if (units_ == RADIANS)
    return deg_to_rad(middle);
  return middle;
}

template <class T>
void bsta_spherical_histogram<T>::
center(int linear_index, T& az_center, T& el_center)
{
  //convert linear_index to azimuth and elevation index
  int el_index = linear_index/n_azimuth_;
  int az_index = linear_index%n_azimuth_;
  az_center = azimuth_center(az_index);
  el_center = elevation_center(el_index);
}

template <class T>
void bsta_spherical_histogram<T>::
convert_to_cartesian(T azimuth, T elevation, T& x, T& y, T& z) const
{
  T az = azimuth, el=elevation;
  if (units_==DEG) {
    az = deg_to_rad(az);
    el = deg_to_rad(el);
  }
  //convert to standard spherical coordinates if necessary
  if (az_branch_cut_ == B_0_360)
    if (az>static_cast<T>(vnl_math::pi))
      az -= static_cast<T>(vnl_math::twopi);
  if (el_poles_ == B_90_90)
    el = -el + static_cast<T>(vnl_math::pi/2.0);
  //convert to cartesian
  T s = std::sin(el);
  x = s*std::cos(az);
  y = s*std::sin(az);
  z = std::cos(el);
}

template <class T>
void bsta_spherical_histogram<T>::
convert_to_spherical(T x, T y, T z, T& azimuth, T& elevation) const
{
  //internal representation is the standard spherical coordinate system
  //azimuth 0-360, elevation 0-180 with North Pole elevation = 0

  if (std::fabs(z-T(1))<1e-8) {//essentially at the North Pole
    azimuth = T(0);//units don't matter since azimuth is ambiguous
    if (el_poles_ == B_90_90)
      elevation = T(90);
    else
      elevation = T(0);
    if (units_ == RADIANS)
      elevation = deg_to_rad(elevation);
    return;
  }
  if (std::fabs(z+T(1))<1e-8) {//essentially at the South Pole
    azimuth = T(0);//units don't matter since azimuth is ambiguous
    if (el_poles_ == B_90_90)
      elevation = -T(90);
    else
      elevation = T(180);
    if (units_ == RADIANS)
      elevation = deg_to_rad(elevation);
    return;
  }
  elevation = std::acos(z);
  T s = std::sin(elevation);
  T xa = x/s;
  T ya = y/s;
  azimuth = std::atan2(ya, xa);//returns angles with +-180 branch cut
  if (az_branch_cut_ == B_0_360)
    if (azimuth<0) azimuth += static_cast<T>(vnl_math::twopi);
  if (el_poles_ == B_90_90)
    elevation = -elevation + static_cast<T>(0.5*vnl_math::pi);
  if (units_ == DEG) {
    elevation = rad_to_deg(elevation);
    azimuth = rad_to_deg(azimuth);
  }
  return;
}

template <class T>
int bsta_spherical_histogram<T>::azimuth_index(T azimuth) const
{
  if (n_azimuth_==0) return -1;
  //convert to degrees if necessary
  T conv_az = azimuth;
  if (units_==RADIANS)
    conv_az = rad_to_deg(conv_az);
  //convert angle to 0/360 if necessary
  if (az_branch_cut_ == B_180_180)
    if (conv_az < 0)
      conv_az += T(360);
  for (unsigned i = 0; i<n_azimuth_; ++i) {
    T start = static_cast<T>(i*delta_az_)+ azimuth_start_;
    if (in_interval(start, delta_az_, conv_az))
    {
      return i;
    }
  }
  return -1;
}

template <class T>
int bsta_spherical_histogram<T>::elevation_index(T elevation) const
{
  if (n_elevation_==0) return -1;
  //convert to degrees if necessary
  T conv_el = elevation;
  if (units_==RADIANS)
    conv_el = rad_to_deg(conv_el);
  //standardized elevation coordinates range from
  // 0 (at North pole) to 180 at South pole
  if (el_poles_ == B_90_90) {
    conv_el = -conv_el + T(90);
  }
  for (unsigned i = 0; i<n_elevation_; ++i)
    if (static_cast<T>((i+1)*delta_el_) + elevation_start_ >= conv_el)
    {
      return i;
    }
  return -1;
}

template <class T>
void bsta_spherical_histogram<T>::
azimuth_interval(int azimuth_index, T& azimuth_start, T& azimuth_range) const
{
  if (n_azimuth_==0) {
    azimuth_start = T(0);
    azimuth_range = T(0);
    return;
  }
  T az_start = static_cast<T>(azimuth_index*delta_az_)+azimuth_start_;
  //if branch _180 need to shift min, max
  if (az_branch_cut_ == B_180_180) {
    if (az_start >= T(180))
      az_start -= T(360);
  }
  else
    if (az_start>=T(360)) az_start -=T(360);

  // if radians
  if (units_ == RADIANS) {
    azimuth_start = deg_to_rad(az_start);
    azimuth_range = deg_to_rad(delta_az_);
    return;
  }
  azimuth_start = az_start;
  azimuth_range = delta_az_;
}

template <class T>
void bsta_spherical_histogram<T>::
elevation_interval(int elevation_index, T& elevation_start, T& elevation_range) const
{
  if (n_elevation_==0) {
    elevation_start = T(0);
    elevation_range = T(0);
    return;
  }

  T el_start = static_cast<T>(elevation_index*delta_el_);
  if (el_poles_ == B_90_90)
    el_start = start_90_180(el_start, delta_el_);

  if (units_ == RADIANS) {
    elevation_start  = deg_to_rad(el_start);
    elevation_range  = deg_to_rad(delta_el_);
    return;
  }
  elevation_start = el_start;
  elevation_range = delta_el_;
}

template <class T>
void bsta_spherical_histogram<T>::
upcount(T azimuth, T elevation, T mag)
{
  int az_indx = azimuth_index(azimuth);
  int el_indx = elevation_index(elevation);
  int lidx = linear_index(az_indx, el_indx);
  if (!counts_[lidx])
    counts_[lidx]=T(0);
  counts_[lidx]+=mag;
  total_counts_valid_ = false;
}

template <class T>
void bsta_spherical_histogram<T>::
upcount_weighted_by_area(T azimuth, T elevation, T mag)
{
  int el_indx = elevation_index(elevation);
  double ecnt = elevation_center(el_indx);
  T s = static_cast<T>(std::fabs(std::sin(ecnt)));
  if (s>0)
    mag /= s;
  upcount(azimuth, elevation, mag);
}

template <class T>
T bsta_spherical_histogram<T>::total_counts()
{
  if (!total_counts_valid_) {
    total_counts_ = T(0);
    typename std::map<int, T>::iterator cit = counts_.begin();
    for (; cit != counts_.end(); ++cit)
      total_counts_ += (*cit).second;
    total_counts_valid_ = true;
  }
  return total_counts_;
}

template <class T>
T bsta_spherical_histogram<T>::p(int azimuth_index, int elevation_index)
{
  T tcnts = this->total_counts();
  if (tcnts == T(0)) return T(0);
  if (elevation_index<0||elevation_index>=static_cast<int>(n_elevation())||
     azimuth_index<0||azimuth_index>=static_cast<int>(n_azimuth()))
    return T(0);
  int lidx = linear_index(azimuth_index, elevation_index);
  if (!counts_[lidx])
    return 0;
  return counts_[lidx]/tcnts;
}

template <class T>
void bsta_spherical_histogram<T>::mean(T& mean_az, T& mean_el)
{
  //define mean as the average Cartesian unit vector for the
  //unit vectors corresponding to the centers of the occupied bins
  double mean_x = 0.0, mean_y = 0.0, mean_z = 0.0;
  for (int el = 0; el<static_cast<int>(n_elevation_); ++el) {
    T ec = elevation_center(el);
    for (int az = 0; az<static_cast<int>(n_azimuth_); ++az) {
      T ac = azimuth_center(az);
      T x, y, z;
      convert_to_cartesian(ac, ec, x, y, z);
      T pc = p(az, el);
      mean_x  += pc*x;      mean_y  += pc*y;       mean_z  += pc*z;
    }
  }
  double length = std::sqrt(mean_x*mean_x + mean_y*mean_y + mean_z*mean_z);
  if (length<1.0e-8) {
    mean_az = 0;
    mean_el = 0;
    return;
  }
  mean_x /= length;   mean_y /= length;   mean_z /= length;
  convert_to_spherical(static_cast<T>(mean_x), static_cast<T>(mean_y),
                       static_cast<T>(mean_z), mean_az, mean_el);
}

template <class T>
vnl_matrix_fixed<T, 2, 2> bsta_spherical_histogram<T>::covariance_matrix()
{
  double mean_x = T(0), mean_y = T(0) , mean_z = T(0);
  T two_pi = T(360);
  if (units_ == RADIANS)
    two_pi = static_cast<T>(vnl_math::twopi);
  for (int el = 0; el<static_cast<int>(n_elevation_); ++el) {
    T ec = elevation_center(el);
    for (int az = 0; az<static_cast<int>(n_azimuth_); ++az) {
      T pc = p(az, el);
      if (pc==T(0)) continue;
      T ac = azimuth_center(az);
      T x, y, z;
      convert_to_cartesian(ac, ec, x, y, z);
      mean_x  += pc*x;      mean_y  += pc*y;  mean_z  += pc*z;
    }
  }
  double length = std::sqrt(mean_x*mean_x + mean_y*mean_y + mean_z*mean_z);
  if (length<1.0e-8)
    return vnl_matrix_fixed<T, 2 ,2> ();
  mean_x /= length;   mean_y /= length;   mean_z /= length;
  T mean_az, mean_el;
  convert_to_spherical(static_cast<T>(mean_x), static_cast<T>(mean_y),
                       static_cast<T>(mean_z), mean_az, mean_el);
  T caz=T(0), caz_el=T(0), cel=T(0);
  for (int el = 0; el<static_cast<int>(n_elevation_); ++el) {
    T ec = elevation_center(el);
    for (int az = 0; az<static_cast<int>(n_azimuth_); ++az)
    {
      T pc = p(az, el);
      if (pc==T(0)) continue;
      T ac = azimuth_center(az);
      T delta_el = (ec-mean_el);
      cel += pc*delta_el*delta_el;
      T delta_az = azimuth_diff(mean_az, ac, two_pi);
      caz_el += pc*(ec-mean_el)*delta_az;
      caz += pc*delta_az*delta_az;
    }
  }
  vnl_matrix_fixed<T, 2, 2> ret;
  ret[0][0] = caz; ret[0][1]=caz_el; ret[1][0]=ret[0][1]; ret[1][1]=cel;
  return ret;
}

template <class T>
void bsta_spherical_histogram<T>::std_dev(T& std_dev_az, T& std_dev_el)
{
  vnl_matrix_fixed<T, 2, 2> covar = covariance_matrix();
  double var_az = covar[0][0], var_el = covar[1][1];
  std_dev_az = static_cast<T>(std::sqrt(var_az));
  std_dev_el = static_cast<T>(std::sqrt(var_el));
}

template <class T>
std::vector<int> bsta_spherical_histogram<T>::
bins_intersecting_cone(T center_az, T center_el, T cone_half_angle)
{
  std::vector<int> ret;
  T xc, yc ,zc;
  convert_to_cartesian(center_az, center_el, xc, yc, zc);
  vnl_vector<double> cone_axis(3);
  cone_axis[0]=xc; cone_axis[1]=yc; cone_axis[2]=zc;
  for (int az = 0; az<int(n_azimuth_); ++az)
    for (int el = 0; el<int(n_elevation_); ++el) {
      T azc = azimuth_center(az), elc = elevation_center(el);
      T xb, yb, zb;
      convert_to_cartesian(azc, elc, xb, yb, zb);
      vnl_vector<double> bin_vector(3);
      bin_vector[0]=xb; bin_vector[1]=yb; bin_vector[2]=zb;
      double ang = angle(cone_axis, bin_vector);
      if (units_ == DEG)
        ang = rad_to_deg(static_cast<T>(ang));
      if (std::fabs(ang)<=cone_half_angle)
        ret.push_back(linear_index(az, el));
    }
  return ret;
}

template <class T>
void bsta_spherical_histogram<T>::
write_counts_with_interval(std::ostream& os, int azimuth_index,
                           int elevation_index) {
  T az_start, az_range, el_start, el_range;
  int lidx = linear_index(azimuth_index,elevation_index);
  T cnts = counts_[lidx];
  if (!(cnts>T(0))) return;
  azimuth_interval(azimuth_index, az_start, az_range);
  elevation_interval(elevation_index, el_start, el_range);
  os << "az(s:"<< az_start << " int:" << az_range << "):el(s:"
     << el_start << " int:" << el_range << ") "
     << cnts <<'\n';
}

template <class T>
void bsta_spherical_histogram<T>::
write_counts_with_center(std::ostream& os, int azimuth_index,
                         int elevation_index) {
  int lidx = linear_index(azimuth_index,elevation_index);
  T cnts = counts_[lidx];
  if (!(cnts>T(0))) return;
  T az_center = azimuth_center(azimuth_index);
  T el_center = elevation_center(elevation_index);
  os << az_center << ' ' << el_center << ' ' << cnts << '\n';
}

template <class T>
void bsta_spherical_histogram<T>::print_to_text(std::ostream& os)
{
  for (unsigned el = 0; el<n_elevation(); ++el)
    for (unsigned az = 0; az<n_azimuth(); ++az)
      this->write_counts_with_center(os, az, el);
}

template <class T>
void bsta_spherical_histogram<T>::print_to_vrml(std::ostream& os,
                                                T transparency)
{
  os << "#VRML V2.0 utf8\n"
     << "Background {\n"
     << "  skyColor [ 0 0 0 ]\n"
     << "  groundColor [ 0 0 0 ]\n"
     << "}\n"
     << "PointLight {\n"
     << "  on FALSE\n"
     << "  intensity 1\n"
     << "ambientIntensity 0\n"
     << "color 1 1 1\n"
     << "location 0 0 0\n"
     << "attenuation 1 0 0\n"
     << "radius 100\n"
     << "}\n";
    for (int az = 0; az<int(n_azimuth_); ++az)
      for (int el = 0; el<int(n_elevation_); ++el) {
        T cnts = counts(az, el);
        if (cnts>0) {
          T azc = azimuth_center(az), elc = elevation_center(el);
          T x, y, z;
          convert_to_cartesian(azc, elc, x, y, z);
          os<< "Transform {\n"
            << "translation " << x << ' ' << y << ' '
            << ' ' << z << '\n'
            << "children [\n"
            << "Shape {\n"
            << " appearance Appearance{\n"
            << "   material Material\n"
            << "    {\n"
            << "      diffuseColor " << 1 << ' ' << 1 << ' ' << 0 << '\n'
            << "      transparency " << transparency << '\n'
            << "    }\n"
            << "  }\n"
            << " geometry Sphere\n"
            <<   "{\n"
            << "  radius " << p(az, el) << '\n'
            <<  "   }\n"
            <<  "  }\n"
            <<  " ]\n"
            << "}\n";
        }
      }
  os << "Transform {\n"
     << "translation " << 0 << ' ' << 0 << ' '
     << ' ' << 0 << '\n'
     << "children [\n"
     << "Shape {\n"
     << " appearance Appearance{\n"
     << "   material Material\n"
     << "    {\n"
     << "      diffuseColor " << 0 << ' ' << 1 << ' ' << 0 << '\n'
     << "      transparency " << 0 << '\n'
     << "    }\n"
     << "  }\n"
     << " geometry Sphere\n"
     <<   "{\n"
     << "  radius " << 0.95 << '\n'
     <<  "   }\n"
     <<  "  }\n"
     <<  " ]\n"
     << "}\n";
}

//: Write to stream
template <class T>
std::ostream& operator<<(std::ostream& s, bsta_spherical_histogram<T> const& h)
{
  bsta_spherical_histogram<T>& nch = const_cast<bsta_spherical_histogram<T>&>(h);
  s << "bsta_spherical_histogram<T> ==>\n";
  if (h.units() == bsta_spherical_histogram<T>::RADIANS)
    s << "units: radians\n";
  else s << "units: degrees\n";
  s << "n_bins_azimuth: " << h.n_azimuth() << " n_bins_elevation: "
    << h.n_elevation() << '\n'
    << "azimuth(start: " << h.azimuth_start() <<" interval: " << h.azimuth_range()<< " )\n"
    << "elevation(start: "<< h.elevation_start()<<" interval: " << h.elevation_range()<<" )\n";
  if (h.azimuth_branch_cut() == bsta_spherical_histogram<T>::B_0_360) {
    if (h.units() == bsta_spherical_histogram<T>::RADIANS)
      s << "azimuth branch cut: 0/2*PI\n";
    else
      s << "azimuth branch cut: 0/360\n";
  }
  else {
    if (h.units() == bsta_spherical_histogram<T>::RADIANS)
      s << "azimuth branch cut: -PI/PI\n";
    else
      s << "azimuth branch cut: -180/180\n";
  }

  if (h.elevation_poles() == bsta_spherical_histogram<T>::B_90_90) {
    if (h.units()==bsta_spherical_histogram<T>::RADIANS)
      s << "south pole: -PI/2  north_pole: PI\n";
    else
      s << "south pole: -90  north_pole: 90\n";
  }
  else {
    if (h.units()==bsta_spherical_histogram<T>::RADIANS)
      s << "south pole: PI  north_pole: 0\n";
    else
      s << "south pole: 180  north_pole: 0\n";
  }
  s << "-- counts --\n";
  for (unsigned r = 0; r<h.n_elevation(); ++r)
    for (unsigned c = 0; c<h.n_azimuth(); ++c)
      nch.write_counts_with_interval(s, c, r);
  return s;
}

//: Read from stream
template <class T>
std::istream& operator>>(std::istream& is, bsta_spherical_histogram<T>& h)
{
  return is;
}

#undef BSTA_SPHERICAL_HISTOGRAM_INSTANTIATE
#define BSTA_SPHERICAL_HISTOGRAM_INSTANTIATE(T) \
template class bsta_spherical_histogram<T >;\
template std::istream& operator>>(std::istream&, bsta_spherical_histogram<T >&);\
template std::ostream& operator<<(std::ostream&, bsta_spherical_histogram<T > const&)

#endif // bsta_spherical_histogram_hxx_
