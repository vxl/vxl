// This is brl/bbas/bsta/bsta_spherical_histogram.h
#ifndef bsta_spherical_histogram_h_
#define bsta_spherical_histogram_h_
//:
// \file
// \brief A 2-d histogram with bins defined on a unit sphere
// \author Joseph L. Mundy
// \date May 3, 2011
//
// A histogram on the sphere. There are a number of popular choices for
// spherical coordinates and units that are handled. The units for angle
// can be either degrees or radians. Also there are many choices for
// coordinate systems on the sphere. For example in geographic coordinates
// the North and South poles are at +-90 degrees. Also the azimuthal angle
// can exhibit the range 0 -> 360 or -180 -> 180, with discontinuities in
// the angle coordinate (a branch cut). The latter choice of branch cut is used
// for geographic coordinates where positive azimuths are East of Greenwich,
// England. More typically for physics applications the elevation is 0 at the
// North pole of the sphere and increases to 180 degrees at the South pole, with// the azimuthal range 0 -> 360. Note that azimuth is undefined at the
// North and South poles and a special elevation bin centered at the poles
// is needed. The current implementation does not include such a pole bin.
//
// The histogram is implemented as a map on the linear coordinate derived
// from azimuth and elevation indices. Bins with zero count are not
// present in the map.
//
// Methods are provided for the mean and covariance matrix of the histogram
// distribution. The mean is derived from the mean Cartesian vector
// of points on the unit sphere corresponding to the populated bin centers.
// Covariance is computed on angle differences and so is valid only in the
// tangent plane to the unit sphere at the mean.
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//
//

#include <map>
#include <vector>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
//: A histogram on the unit sphere
template <class T>
class bsta_spherical_histogram
{
 public:
  enum ang_units {RADIANS, DEG};
  enum angle_bounds {B_0_360, B_180_180, B_0_180, B_90_90};
  bsta_spherical_histogram();
  bsta_spherical_histogram(unsigned n_azimuth, unsigned n_elevation,
                           T azimuth_start, T azimuth_range,
                           T elevation_start, T elevation_range,
                           ang_units units = DEG,
                           angle_bounds az_branch_cut = B_180_180,
                           angle_bounds el_poles = B_90_90);

  ~bsta_spherical_histogram() = default;

  //: angle units e.g. RADIANS, DEG
  ang_units units() const {return units_;}
  //: discontinuity azimuthal angle coordinates
  angle_bounds azimuth_branch_cut() const {return az_branch_cut_;}
  //: location of north and south poles on the sphere
  angle_bounds elevation_poles() const {return el_poles_;}

  //: number of bins allowed
  unsigned n_azimuth() const {return n_azimuth_;}
  unsigned n_elevation() const {return n_elevation_;}

  //: indices for individual spherical coordinates
  int azimuth_index(T azimuth) const;
  int elevation_index(T elevation) const;
  //: 1-d linear indices
  int linear_index(int azimuth_index, int elevation_index) const
    {return elevation_index*n_azimuth_ + azimuth_index;}
  //: function name different since T could be int
  int lin_index(T azimuth, T elevation)
    {return linear_index(azimuth_index(azimuth), elevation_index(elevation));}

  //: azimuth bounds
  T azimuth_start() const;
  T azimuth_range() const;

  //: elevation bounds
  T elevation_start() const;
  T elevation_range() const;

  void azimuth_interval(int azimuth_index, T& azimuth_start, T& azimuth_range) const;
  void azimuth_interval(T azimuth, T& azimuth_start, T& azimuth_range) const {
    int az_ind = azimuth_index(azimuth);
    azimuth_interval(az_ind, azimuth_start, azimuth_range);}

  void elevation_interval(int  elevation_index,
                          T& elevation_start, T& elevation_range) const;

  void elevation_interval(T elevation, T& elevation_start, T& elevation_range) const{
  int el_ind = elevation_index(elevation);
  elevation_interval(el_ind, elevation_start, elevation_range);}

  T azimuth_center(int azimuth_index) const;
  T elevation_center(int elevation_index) const;
  void center(int linear_index, T& az_center, T& el_center);

  //: increment the count in a given bin by mag
  void upcount(T azimuth, T elevation, T mag = T(1.0));

  //: weight the increment by spherical area of the bin, i.e. mag/sin(el)
  void upcount_weighted_by_area(T azimuth, T elevation, T mag = T(1.0));

  //: counts in a specified bin
  T counts (int azimuth_index, int elevation_index) {
    if (azimuth_index>=0&&azimuth_index<=static_cast<int>(n_azimuth())&&
       elevation_index>=0&&elevation_index<=static_cast<int>(n_elevation())) {
      int lidx = linear_index(azimuth_index,elevation_index);
      if (counts_[lidx])
        return counts_[lidx];
    }
 return T(0);
  }
  T counts(T azimuth, T elevation)  {
    int az_ind = azimuth_index(azimuth);
    int el_ind = elevation_index(elevation);
    return counts(az_ind, el_ind);
  }

  T counts(int linear_index) {return counts_[linear_index];}

  //:total number of counts, cached so non-const
  T total_counts();

  //: probability of a bin
  T p(int azimuth_index, int elevation_index) ;
  T p(T azimuth, T elevation)
  {
    int az_ind = azimuth_index(azimuth),el_ind = elevation_index(elevation);
    return p(az_ind, el_ind);
  }

  //: mean of histogram distribution
  void mean(T& mean_az, T& mean_el);

  //: covariance matrix of histogram cvar = [(az-mu_az)][(az-mu_az) (el-mu_el)]
  //                                        [(el-mu_el)]
  vnl_matrix_fixed<T, 2, 2> covariance_matrix();

  //: marginal standard deviations
  void std_dev(T& std_dev_az, T& std_dev_el);

  //: linear index of bins which have centers lying inside the cone
  std::vector<int> bins_intersecting_cone(T center_az, T center_el,
                                         T cone_half_angle);


  //:unit conversions
  static T deg_to_rad(T ang) {return static_cast<T>(vnl_math::pi_over_180*ang);}
  static T rad_to_deg(T ang) {return static_cast<T>(vnl_math::deg_per_rad*ang);}

  //:output of bin ranges and counts (zero counts not output)
  void write_counts_with_interval(std::ostream& os, int azimuth_index,
                                  int elevation_index);
  //:output of bin center and counts (zero counts not output)
  void write_counts_with_center(std::ostream& os, int azimuth_index,
                                int elevation_index);
  //: write bin centers (azimuth elevation) and non_zero counts to stream
  void print_to_text(std::ostream& os);

  //: display counts in vrml with reference sphere
  void print_to_vrml(std::ostream& os, T transparency = T(0));

  //: convenient utilities
  void convert_to_cartesian(T azimuth, T elevation, T& x, T& y, T& z) const;
  void convert_to_spherical(T x, T y, T z, T& azimuth, T& elevation) const;
 private:
  //: members
  unsigned n_azimuth_;
  unsigned n_elevation_;
  T azimuth_start_;
  T azimuth_range_;
  T elevation_start_;
  T elevation_range_;
  T delta_az_;
  T delta_el_;
  angle_bounds az_branch_cut_;
  angle_bounds el_poles_;
  ang_units units_;
  bool total_counts_valid_;
  T total_counts_;
  std::map<int, T> counts_;
};

//: Write histogram to stream
// \relatesalso bsta_spherical_histogram
template <class T>
std::ostream&  operator<<(std::ostream& s, bsta_spherical_histogram<T> const& h);

//: Read histogram from stream
// \relatesalso bsta_spherical_histogram
template <class T>
std::istream&  operator>>(std::istream& is,  bsta_spherical_histogram<T>& h);


#endif // bsta_spherical_histogram_h_
