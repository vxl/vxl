#pragma once
// This is brl/bbas/volm/desc/volm_desc_ex_land_only.h
#ifndef volm_desc_ex_land_only_h_
#define volm_desc_ex_land_only_h_
//:
// \file
// \brief  A class to represent a location with a descriptor as an existence histogram of surfaces 
//         with various land type at certain distance (no orientation)
//
// \author Yi Dong
// \date June 11, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include "volm_desc.h"
#include <vcl_iostream.h>
#include <volm_io.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <bpgl/depth_map/depth_map_region.h>
#include <bpgl/depth_map/depth_map_region_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>

class volm_desc_ex_land_only : public volm_desc
{
public:

  //: Default constructor
  volm_desc_ex_land_only() {}

  //: simple constructor that only sets the number of bins
  volm_desc_ex_land_only(unsigned ndists, unsigned nlands, vcl_vector<double>& radius) : 
    ndists_(ndists), nlands_(nlands), radius_(radius) { nbins_ = ndists_ * nlands_; h_.resize(nbins_); initialize_bin(0); name_ = "existence land only descriptor";}


  //: Constructor from depth map scene
  volm_desc_ex_land_only(depth_map_scene_sptr const& dms,
                         vcl_vector<double> const& radius,
                         unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                         unsigned char const& initial_mag = 0);

  //: Constructor from created index
  volm_desc_ex_land_only(vcl_vector<unsigned char> const& index_dst,
                         vcl_vector<unsigned char> const& index_combined,
                         vcl_vector<double> depth_interval,
                         vcl_vector<double> const& radius,
                         unsigned const& nlands = volm_label_table::compute_number_of_labels(),
                         unsigned char const& initial_mag = 0);

  //: Destructor
  ~volm_desc_ex_land_only() {}

  //: number of depth bins
  unsigned ndepths() const { return this->ndists_; }

  //: the number of land bins
  unsigned nlands() const { return this->nlands_; }

  //: the radius that defines the distance interval
  vcl_vector<double>& radius() { return radius_; }

  //: initialize the histogram with given value
  void initialize_bin(unsigned char const& mag);

  //: get the bin index from object distance, height, orientation type and land type (return idx larger than nbins if invalid)
  unsigned bin_index(double const& dist, unsigned const& land) const;

  //: get the bin index from distance index, height index, orientation index and land index (return idx larger than nbins if invalid)
  unsigned bin_index(unsigned const& dist_idx, unsigned const& land_idx) const;

  //: set the bin value given bin index (do nothing if bin is bigger than number of bins)
  void set_count(unsigned const& bin, unsigned char const& count);

  //: set the bin value given object distance, orientation and land type
  void set_count(double const& dist, unsigned const& land, unsigned char const& count);

  //: set the bin to count value given object dist_id, orient_id and land_id
  void set_count(unsigned const& dist_idx, unsigned const& land_idx, unsigned char const& count);

  //: screen print
  void print() const;

  //: similarity method -- calculate the intersection of two histogram, normalized by current histogram
  float similarity(volm_desc_sptr other);

  // ===========  binary I/O ================

  //: version
  unsigned version() const { return 1; }

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

private:
  //: histogram structure
  unsigned ndists_;
  unsigned nlands_;
  vcl_vector<double> radius_;

  unsigned locate_idx(double const& target, vcl_vector<double> const& arr) const;
};

#endif // volm_desc_ex_land_only_h_