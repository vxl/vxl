#ifndef vsph_segment_sphere_h_
#define vsph_segment_sphere_h_
#include <iostream>
#include <map>
#include "vsph_unit_sphere.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_disjoint_sets.h>

class vsph_segment_sphere
{
 public:
  vsph_segment_sphere(vsph_unit_sphere const& usph, double c, int min_size,double sigma, bool dosmoothing  )
  : seg_valid_(false),usph_(usph),sigma_(sigma), c_(c), min_size_(min_size),dosmoothing_(dosmoothing),num_ccs_(0) {}
  // === accessors ==-
  double sigma() const {return sigma_;}
  double c() const {return c_;}
  int min_size() const {return min_size_;}
  int num_ccs() const {return num_ccs_;}
  const std::map<int,  std::vector<int> >& regions() const {return regions_;}
  // === process methods ===
  void set_data(std::vector<double> data){data_ = data;}
  void segment();
  bool extract_region_bounding_boxes();
  const std::map<int, vsph_sph_box_2d> region_boxes() const {return bboxes_;}
  std::vector<double> region_data() const;
  std::vector<std::vector<float> > region_color() const;

  //: function to compute mean of the pixels in a region using the oringal values of the spherical segment
  double region_mean(int id);
  //: function to compute median of the pixels in a region using the oringal values of the spherical segment
  double region_median(int id);

  vsph_unit_sphere & unit_sphere(){return usph_;}
    std::vector<double> data_;
 private:
  void smooth_data();

  bool seg_valid_;
  vsph_unit_sphere usph_;
  double sigma_;
  double c_;
  int min_size_;
  bool dosmoothing_;
  //: spherical "image" data

  std::vector<double> smooth_data_;
  int num_ccs_;
  //: segmented regions
  vbl_disjoint_sets ds_;
  //: region id  sph vert ids
  std::map<int,  std::vector<int> > regions_;
  //: region id  box
  std::map<int, vsph_sph_box_2d> bboxes_;
};

#endif
