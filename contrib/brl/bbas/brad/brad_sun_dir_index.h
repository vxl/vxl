#ifndef __BRAD_SUN_DIR_INDEX_H
#define __BRAD_SUN_DIR_INDEX_H
//-----------------------------------------------------------------------------
//:
// \file
// \brief An index for sun direction vectors.
// \author J. L. Mundy
// \date May 10, 2011
//
//----------------------------------------------------------------------------
#include <vcl_map.h>
#include <bbas/bsta/bsta_spherical_histogram.h>
#include <vnl/vnl_double_3.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>

class brad_sun_dir_index
{
 public:
  brad_sun_dir_index(double longitude_deg, double latitude_deg,
                     int obs_year_start, int obs_hour, int obs_min,
                     int obs_time_range_min, int sampling_interval_years,
                     int bin_radius);
  ~brad_sun_dir_index() {}

  //: uses standard definion of sun azimuth and elevation, i.e. elevation zero tangent plane, increasing to nadir and azimuth zero at North and increasing to East, angles in degrees.
  int index(double geo_sun_azimuth, double geo_sun_elevation) const;
  //: index from sun direction vector (pointing towards the sun).
  int index(double x, double y, double z) const;

  //: number of illumination bins
  unsigned n_sun_dir_bins() const {return static_cast<unsigned>(2*bin_radius_+1);}

  //: cone that defines the range of illuminations in a given bin
  double cone_half_angle() const {return cone_half_angle_;}
  vnl_double_3 cone_axis(unsigned i) const {return cone_axes_[i];}

  //: location on the Earth being modeled
  double longitude_deg() const {return longitude_deg_;}
  double latitude_deg() const {return latitude_deg_;}

  //: path along the major axis of the illumination distribution, +- 2std_dev.
  // useful for debugging
  vcl_vector<vnl_double_3> major_path();

  //: output a vrml display of the major path and the bin axis positions on sun direction sphere
  void print_to_vrml(vcl_ostream& os);
 private:
  brad_sun_dir_index();//no default constructor
  //: the histogram of sun directions for the specified location and collection time
  double longitude_deg_;
  double latitude_deg_;
  //: the total number of bins is 2*bin_radius_ + 1, i.e. a centered neighborhood
  int bin_radius_;
  bsta_spherical_histogram<double> hist_;
  //: cone axes associated with each illumination bin
  vcl_vector<vnl_double_3> cone_axes_;
  //: cone half angle
  double cone_half_angle_;
};

vcl_ostream&  operator<<(vcl_ostream& s, brad_sun_dir_index const& bdi);

#endif
