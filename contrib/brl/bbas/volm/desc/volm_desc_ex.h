#pragma once
// This is brl/bbas/volm/desc/volm_desc_ex.h
#ifndef volm_desc_ex_h_
#define volm_desc_ex_h_
//:
// \file
// \brief  A class to represent a location with a descriptor as an existence histogram of surfaces 
//         with various land types and/or orientations etc at certain distance
//
// \author Yi Dong
// \date May 29, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include "volm_desc.h"
#include <vcl_iostream.h>
#include <volm_io.h>
#include <volm_category_io.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <bpgl/depth_map/depth_map_region.h>
#include <bpgl/depth_map/depth_map_region_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>

// a structor to hold an object attributes (distance and height are in meters)
class volm_object
{
public:
  volm_object() {}
  volm_object(double const& dist, depth_map_region::orientation const& orient, unsigned const& land, double const& height = 0)
    : dist_(dist), height_(height), orient_(orient), land_(land) {}
  ~volm_object() {}

  void print() const {
    vcl_cout << "[dist: " << dist_ << " height: " << height_
             << " orient: " << orient_ << " land: " << land_
             << "(" << volm_osm_category_io::volm_category_name_table[land_] << ")]\n";
  }

  double dist_;
  double height_;
  depth_map_region::orientation orient_;
  unsigned land_;
};


class volm_desc_ex : public volm_desc
{
public:
  //: Default constructor
  volm_desc_ex() {}
  //: constructor from depth_map_scene
  volm_desc_ex(depth_map_scene_sptr const& dms,
               vcl_vector<double> const& radius,
               unsigned const& norients = 3,
               unsigned const& nlands = volm_osm_category_io::volm_land_table.size(),
               unsigned char const& initial_mag = 0);
  
  //: constrcutor from created index
  volm_desc_ex(vcl_vector<unsigned char> const& index_dst,
               vcl_vector<unsigned char> const& index_combined,
               vcl_vector<double> depth_interval,
               vcl_vector<double> const& radius,
               unsigned const& norients = 3,
               unsigned const& nlands = volm_osm_category_io::volm_land_table.size(),
               unsigned char const& initial_mag = 0);

  //: simple constructor that only sets the number of bins
  volm_desc_ex(unsigned ndists, unsigned norients, unsigned nlands, vcl_vector<double>& radius) : 
    ndists_(ndists), norients_(norients), nlands_(nlands), radius_(radius) { nbins_ = ndists_ * nlands_; h_.resize(nbins_); initialize_bin(0); }

  //: destructor
  ~volm_desc_ex() {}

  //: number of depth bins
  unsigned ndepths() const { return this->ndists_; }

  //: the number of orient bins
  unsigned norients() const { return this->norients_; }

  //: the number of land bins
  unsigned nlands() const { return this->nlands_; }

  //: the radius that defines the distance interval
  vcl_vector<double>& radius() { return radius_; }

  //: initialize the histrogram with given value
  void initialize_bin(unsigned char const& mag);

  //: get the bin index from object distance, height, orientation type and land type (return idx larger than nbins if invalid)
  unsigned bin_index(double const& dist, depth_map_region::orientation const& orient, unsigned const& land) const;
  unsigned bin_index(double const& dist, unsigned const& orient, unsigned const& land) const;

  //: get the bin index from distance index, height index, orientation index and land index (return idx larger than nbins if invalid)
  unsigned bin_index(unsigned const& dist_idx, unsigned const& orient_idx, unsigned const& land_idx) const;

  //: get the bin index from an object
  unsigned bin_index(volm_object const& ob) const
  {  return this->bin_index(ob.dist_, ob.orient_, ob.land_); }

  //: set the bin value given bin index (do nothing if bin is bigger than number of bins)
  void set_count(unsigned const& bin, unsigned char const& count);

  //: set the bin value given object distance, orientation and land type
  void set_count(double const& dist, depth_map_region::orientation const& orient, unsigned const& land,
                 unsigned char const& count);

  void set_count(double const& dist, unsigned const& orient, unsigned const& land,
                 unsigned char const& count);

  //: set the bin to count value given object dist_id, orient_id and land_id
  void set_count(unsigned const& dist_idx, unsigned const& orient_idx, unsigned const& land_idx,
                 unsigned char const& count);

  //: set the bin to count value given an object
  void set_count(volm_object const& ob, unsigned char const& count)
  {  this->set_count(ob.dist_, ob.orient_, ob.land_, count);  }

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
  unsigned norients_;
  unsigned nlands_;
  vcl_vector<double> radius_;
  
};

#endif  // volm_desc_ex_h_
