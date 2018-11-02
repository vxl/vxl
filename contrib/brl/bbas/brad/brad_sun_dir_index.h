#ifndef __BRAD_SUN_DIR_INDEX_H
#define __BRAD_SUN_DIR_INDEX_H
//-----------------------------------------------------------------------------
//:
// \file
// \brief An index for sun direction vectors.
//
// A sun direction vector is assigned to a bin which is closest in
// terms of the angle between the direction vector and the vector corresponding
// to the center of a given bin. This relationship defines a set of
// vectors forming a cone around the bin center vector.
// Currently there is no limit on the cone angle, but a member,cone_half_angle,
// is provided if future development requires a limit to the extent
// of a bin cone neighborhood.
//
// \author J. L. Mundy
// \date May 10, 2011
//
//----------------------------------------------------------------------------
#include <map>
#include <vector>
#include <iostream>
#include <iosfwd>
#include <bbas/bsta/bsta_spherical_histogram.h>
#include <vnl/vnl_double_3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class brad_sun_dir_index
{
 public:

  //:constructor to provide the sun illumination direction histogram from which the bin structure is automatically derived
  brad_sun_dir_index(double longitude_deg, double latitude_deg,
                int obs_year_start, int obs_hour, int obs_min,
                int obs_time_range_min, int sampling_interval_years,
                int bin_radius);

  //:constructor not enabling the direction histogram, bin centers are previously defined
  brad_sun_dir_index(double longitude_deg, double latitude_deg,
                     std::vector<vnl_double_3> sun_dir_bin_centers)
    : longitude_deg_(longitude_deg), latitude_deg_(latitude_deg),
    cone_axes_(sun_dir_bin_centers), cone_half_angle_(0.0){
    bin_radius_ = ((int)sun_dir_bin_centers.size()-1)/2;}

  ~brad_sun_dir_index()= default;

  //: uses standard definion of sun azimuth and elevation, i.e. elevation zero tangent plane, increasing to nadir and azimuth zero at North and increasing to East, angles in degrees.
  int index(double geo_sun_azimuth, double geo_sun_elevation, double & min_angle) ;
  //: index from sun direction vector (pointing towards the sun).
  int index(double x, double y, double z, double & min_angle ) const;

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
  std::vector<vnl_double_3> major_path();

  //: output a vrml display of the major path and the bin axis positions on sun direction sphere
  void print_to_vrml(std::ostream& os);
 private:
  brad_sun_dir_index() = delete;//no default constructor
  //: the histogram of sun directions for the specified location and collection time
  double longitude_deg_;
  double latitude_deg_;
  //: the total number of bins is 2*bin_radius_ + 1, i.e. a centered neighborhood
  int bin_radius_;
  bsta_spherical_histogram<double> hist_;
  //: cone axes associated with each illumination bin
  std::vector<vnl_double_3> cone_axes_;
  //: cone half angle, to truncate bin membership (not currently used)
  double cone_half_angle_;
};

std::ostream&  operator<<(std::ostream& s, brad_sun_dir_index const& bdi);

#endif
