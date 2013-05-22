// This is brl/bbas/volm/desc/volm_descriptor.h
#ifndef volm_descriptor_h_
#define volm_descriptor_h_
//:
// \file
// \brief A simple descriptor to description the existance of object types at various distance using bsta_histogram for representation
//        Note current orientation typs only considers horizontal, front_parallel and others which includes porous, infinite
// \author Yi Dong
// \date May 03, 2013
//
//
// \verbatim
//  <none yet>
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <bsta_histogram.h>
#include <bsta_histogram.txx>
#include <volm_io.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_io.h>
#include <bpgl/depth_map/depth_map_region.h>
#include <bpgl/depth_map/depth_map_region_sptr.h>

// a struct to hold an object attributes (distance and height are in meter)
class volm_object
{
public:
  volm_object() {}
  volm_object(double const& dist, double const& height, depth_map_region::orientation const& orient, unsigned const& land)
    : dist_(dist), height_(height), orient_(orient), land_(land) {}

  ~volm_object() {}

  void print() const{
    vcl_cout << "[dist: " << dist_ << " height: " << height_
             << " orient: " << orient_ << " land: " << land_ 
             << "(" << volm_label_table::land_string((unsigned char)land_) << ")]\n";}
  
  double dist_;
  double height_;
  depth_map_region::orientation orient_;
  unsigned land_;
};


class volm_descriptor : public vbl_ref_count
{
public:
  //: Default constructor
  volm_descriptor() {}
  
  //: Constuctor
  volm_descriptor(vcl_vector<double> const& radius,
                  vcl_vector<double> const& height,
                  unsigned const& n_orient = 3,
                  unsigned const& n_land = volm_label_table::land_id.size(),
                  unsigned char const& initial_mag = 0);

  //: Destructor
  ~volm_descriptor() {}

  //: the number of depth bins
  unsigned ndepths() const { return this->ndepths_; }

  //: the number of height bins
  unsigned nheights() const { return this->nheights_; }

  //: the number of orient bins
  unsigned norients() const { return this->norients_; }

  //: the number of land bins
  unsigned nlands() const { return this->nlands_; }

  //: the number of bins in the histogram
  unsigned int nbins() const { return nbins_; }

  //: the radius that defines the distance interval
  vcl_vector<double>& radius() { return radius_; }

  //: the height that defines the height interval
  vcl_vector<double>& height() { return height_; }

  //: the histrogram
  bsta_histogram<unsigned char> const h() { return this->h_; }

  //: initialize the histrogram with given value
  void initialize_bin(unsigned char const& mag);

  //: get the bin index from object distance, height, orientation type and land type (return idx larger than nbins if invalid)
  unsigned bin_index(double const& dist, double const& height,
                     depth_map_region::orientation const& orient, unsigned const& land);

  //: get the bin index from distance index, height index, orientation index and land index (return idx larger than nbins if invalid)
  unsigned bin_index(unsigned const& dist_idx, unsigned const& height_idx,
                     unsigned const& orient_idx, unsigned const& land_idx);

  //: get the bin index from an object
  unsigned bin_index(volm_object const& ob)
  {  return this->bin_index(ob.dist_, ob.height_, ob.orient_, ob.land_); }

  //: set the bin to count value given bin index
  void set_count(unsigned const& bin, unsigned char const& count)
  {  if (bin<nbins_) h_.set_count(bin,count);}

  //: set the bin to count value given object distance, height, orientation and land type (assign nothing if invalid)
  void set_count(double const& dist, double const& height,
                 depth_map_region::orientation const& orient, unsigned const& land,
                 unsigned char const& count);

  //: set the bin to count value given object dist_id, height_id, orient_id and land_id
  void set_count(unsigned const& dist_idx, unsigned const& height_idx,
                 unsigned const& orient_idx, unsigned const& land_idx,
                 unsigned char const& count);

  //: set the bin to count value given an object
  void set_count(volm_object const& ob, unsigned char const& count)
  {  this->set_count(ob.dist_, ob.height_, ob.orient_, ob.land_, count); }

  //: Increaset the count of the bin corresponding to object attributes by magnitude
  void upcount(unsigned const bin, unsigned char const& mag)
  { 
    unsigned char temp = h_.counts(bin);
    h_.set_count(bin, temp+mag);
  }

  //: Increase the count of the bin corresponding to object attributes by magnitude (assign nothing if invalid)
  void upcount(double const& dist, double const& height,
               depth_map_region::orientation const& orient, unsigned const& land,
               unsigned char const& mag);

  //: Increase the count of the bin corresponding to dist_idx, height_idx, orient_idx and land_idx
  void upcount(unsigned const& dist_idx, unsigned const& height_idx,
               unsigned const& orient_idx, unsigned const& land_idx,
               unsigned char const& mag);

  //: Increase the count of the bin corresponding to an object
  void upcount(volm_object const& ob, unsigned char const& mag)
  { this->upcount(ob.dist_, ob.height_, ob.orient_, ob.land_, mag); }

  //: Accerror to the value at bin i in the histogram
  unsigned char operator[] (unsigned int i) const { return h_.counts(i);}


  //: print
  void print() const;

  //: visualize
  void visualize(vcl_string fname, unsigned char const& y_max = 10) const;
  
  // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

private:
  unsigned    ndepths_;
  unsigned   nheights_;
  unsigned   norients_;
  unsigned     nlands_;
  unsigned      nbins_;

  vcl_vector<double> radius_;
  vcl_vector<double> height_;
  bsta_histogram<unsigned char> h_;
};

#include "volm_descriptor_sptr.h"
void vsl_b_write(vsl_b_ostream& os, const volm_descriptor* rptr);

void vsl_b_read(vsl_b_istream &is, volm_descriptor*& rptr);

void vsl_b_write(vsl_b_ostream& os, const volm_descriptor_sptr& sptr);
//
void vsl_b_read(vsl_b_istream &is, volm_descriptor_sptr& sptr);

#endif // volm_descriptor_h_
