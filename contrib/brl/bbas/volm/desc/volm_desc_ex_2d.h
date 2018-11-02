// This is brl/bbas/volm/desc/volm_desc_ex_2d.h
#ifndef volm_desc_ex_2d_h_
#define volm_desc_ex_2d_h_
//:
// \file
// \brief  A class to represent a location with a descriptor as a 2-d existence histogram of surfaces
//         with difference heading direction interval, various land type at certain distance
//         Note that the descriptor is arranged as heading[distance[land]]
// \author Yi Dong
// \date July 15, 2014
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include <iostream>
#include "volm_desc.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <volm_io.h>
#include <cassert>
#include <vsl/vsl_binary_io.h>
#include <bpgl/depth_map/depth_map_region.h>
#include <bpgl/depth_map/depth_map_region_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>

class volm_desc_ex_2d : public volm_desc
{
public:
  //: default constructor
  volm_desc_ex_2d() = default;

  //: constructor to create an empty descriptor
  volm_desc_ex_2d(std::vector<double> const& radius,
                  double const& h_width = 360.0, double const& h_inc = 360.0,
                  unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                  unsigned char const& initial_mag = 0);

  //: destructor
  ~volm_desc_ex_2d() override = default;

  //: initialize the histogram with given value
  void initialize_bin(unsigned char const& mag);

  //: number of depth bins
  unsigned ndepths() const {return ndists_; }
  //: number of land bins
  unsigned nlands() const { return nlands_; }
  //: number of heading bins
  unsigned nheadings() const { return nheadings_; }
  //: heading width
  double h_width() const { return h_width_; }
  //: heading incremental
  double h_inc() const { return h_inc_; }
  //: the radius that defines the distance interval
  std::vector<double>& radius(){ return radius_; }
  //: the heading intervals
  std::vector<std::pair<double, double> >& heading_intervals() { return heading_intervals_; }

  //: get the bin index from object distance, land type and heading direction (return index larger than nbins if invalid)
  //: multiple values exist due to heading bins overlap
  std::vector<unsigned> bin_index(double const& dist, unsigned const& land, double const& heading) const;

  //: get the bin index from distance bin index, land bin index and heading index (return index larger than nbins if invalid)
  unsigned bin_index(unsigned const& dist_idx, unsigned const& land_idx, unsigned const& heading_idx) const;

  //: set the bin value given bin index (do nothing if bin is bigger than number of bins)
  void set_count(unsigned const& bin, unsigned char const& count);

  //: set the bin value given object distance id, land id and heading id
  void set_count(unsigned const& dist_idx, unsigned const& land_idx, unsigned const& heading_idx, unsigned char const& count);

  //: set the bin value given object distance, land type and heading direction
  //: note a given heading direction can have multiple bins
  void set_count(double const& dist, unsigned const& land, double const& heading, unsigned char const& count);

  //: similarity method -- calculate the intersection of two histogram, normalized by current histogram
  float similarity(volm_desc_sptr other) override;

  //: screen print
  void print() const override;

  // ================  binary I/O ===================

  //: version
  unsigned version() const override { return 1; }

  //: binary IO write
  void b_write(vsl_b_ostream& os) override;

  //: binary IO read
  void b_read(vsl_b_istream& is) override;


private:
  //: histogram structure
  unsigned ndists_;
  unsigned nlands_;
  unsigned nheadings_;
  double h_width_;
  double h_inc_;
  std::vector<double> radius_;
  std::vector<std::pair<double, double> > heading_intervals_;

  unsigned locate_idx(double const& target, std::vector<double> const& arr) const;
};


#endif // volm_desc_ex_2d_h_
