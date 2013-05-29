// This is brl/bbas/volm/desc/volm_descriptor_factory.h
#ifndef volm_descriptor_factory_h_
#define volm_descriptor_factory_h_
//:
// \file
// \brief A base factory class for volumetric descriptor.  The descriptor uses a 1D histogram to reprsents
//  the possible scene seen from a hypothesized location.  The objects are defined by their distance to the
//  hypothesized location, absolute height, orientation and land classification.  The object orientation 
//  can be vertical, horizontal and infinite(sky).

//  The children of this class should implement an unique name and a appropriate set_count method 
//  to update the histogram value accordingly.  Current implemented descriptor are "existance" descriptor
//  and "upcount" descriptor.

// \author Yi Dong
// \date May 28, 2013
//
// \verbatim
//  Modifications
//   <none yet>
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


// a structor to hold an object attributes (distance and height are in meters)
class volm_object
{
public:
  volm_object() {}
  volm_object(double const& dist, double const& height, depth_map_region::orientation const& orient, unsigned const& land)
    : dist_(dist), height_(height), orient_(orient), land_(land) {}

  ~volm_object() {}

  void print() const {
    vcl_cout << "[dist: " << dist_ << " height: " << height_
             << " orient: " << orient_ << " land: " << land_ 
             << "(" << volm_label_table::land_string((unsigned char)land_) << ")]\n";
  }
  
  double dist_;
  double height_;
  depth_map_region::orientation orient_;
  unsigned land_;
};

class volm_descriptor_factory;
typedef vbl_smart_ptr<volm_descriptor_factory> volm_descriptor_factory_sptr;

class volm_descriptor_factory : public vbl_ref_count
{
public:
  //: Default constructor (do nothing)
  volm_descriptor_factory() {}

  //: create the histogram
  void create(vcl_vector<double> const& radius,
              vcl_vector<double> const& height,
              unsigned const& n_orient = 3,
              unsigned const& n_land = volm_label_table::land_id.size(),
              unsigned char const& initial_mag = 0);

  //: Destructor
  virtual ~volm_descriptor_factory() {};

  //: return the name of defined volm_descriptor
  vcl_string name() const { return this->desc_name_; }

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

  //: initialize the histrogram with given value
  void initialize_bin(unsigned char const& mag);

  //: set the descriptor name
  void set_name(vcl_string const& name)
  {  desc_name_ = name;  }

  //: get the bin index from object distance, height, orientation type and land type (return idx larger than nbins if invalid)
  unsigned bin_index(double const& dist, double const& height,
                     depth_map_region::orientation const& orient, unsigned const& land) const;

  //: get the bin index from distance index, height index, orientation index and land index (return idx larger than nbins if invalid)
  unsigned bin_index(unsigned const& dist_idx, unsigned const& height_idx,
                     unsigned const& orient_idx, unsigned const& land_idx) const;

  //: get the bin index from an object
  unsigned bin_index(volm_object const& ob) const
  {  return this->bin_index(ob.dist_, ob.height_, ob.orient_, ob.land_); }

  //: method to update the bin value given bin index
  virtual void set_count(unsigned const& bin, unsigned char const& count) = 0;

  //: set the bin value given object distance, height, orientation and land type
  virtual void set_count(double const& dist, double const& height,
                         depth_map_region::orientation const& orient, unsigned const& land,
                         unsigned char const& count) = 0;

  //: set the bin to count value given object dist_id, height_id, orient_id and land_id
  virtual void set_count(unsigned const& dist_idx, unsigned const& height_idx,
                         unsigned const& orient_idx, unsigned const& land_idx,
                         unsigned char const& count) = 0;

  //: set the bin to count value given an object
  virtual void set_count(volm_object const& ob, unsigned char const& count) = 0 ;

  //: Accerror to the value at bin i in the histogram
  unsigned char operator[] (unsigned int i) const { return h_.counts(i);}

  //: print
  void print() const;

  //: visualization
  void visualize(vcl_string fname, unsigned char const& y_max = 10) const;

  //: create desired volm_descriptor to use
  static volm_descriptor_factory_sptr create_descriptor(vcl_string name);

  // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

protected:
  //: histogram strcuture
  unsigned          ndepths_;
  unsigned         nheights_;
  unsigned         norients_;
  unsigned           nlands_;
  unsigned            nbins_;
  vcl_vector<double> radius_;
  vcl_vector<double> height_;
  
  //: name of the descriptor used
  vcl_string desc_name_;

  //: histogram
  bsta_histogram<unsigned char> h_;

};

void vsl_b_write(vsl_b_ostream& os, const volm_descriptor_factory* rptr);

void vsl_b_read(vsl_b_istream& is, volm_descriptor_factory*& rptr);

void vsl_b_write(vsl_b_ostream& os, const volm_descriptor_factory_sptr& sptr);

void vsl_b_read(vsl_b_istream& is, volm_descriptor_factory_sptr& sptr);

#endif // volm_descriptor_factory_h_