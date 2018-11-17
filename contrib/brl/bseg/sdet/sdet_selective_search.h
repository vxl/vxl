#ifndef sdet_selective_search_h_
#define sdet_selective_search_h_
//---------------------------------------------------------------------
//:
// \file
// \brief An implementation of the selective search algorithm
//
//
// \author
//  J.L. Mundy - December 13, 2016
//
// \verbatim
// Modifications
//  None
// \endverbatim
//
//-------------------------------------------------------------------------
// The hierarchy of regions is constructed by merging an intial set of regions
// produced by the algorithm of Felzenszwalb and Huttenlocher
// International Journal of Computer Vision, Vol. 59, No. 2, September 2004
// For details see J.R.R. Uijlings et al, Selective Search for Object Recognition,
// International Journal of Computer Vision, 2013
#include <iostream>
#include <map>
#include <queue>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include "sdet_region.h"
#include "sdet_selective_search_params.h"
#include <bsta/bsta_histogram.h>
#include <vgl/vgl_box_2d.h>

class sdet_selective_search : public sdet_selective_search_params
{
 public:
  //Constructors/destructor
  //:use default parameters
 sdet_selective_search():  initial_regions_valid_(false), image_area_(0),diverse_max_label_(0){}
  //:specify different parameters
 sdet_selective_search(sdet_selective_search_params& rpp):sdet_selective_search_params(rpp),  initial_regions_valid_(false), image_area_(0),diverse_max_label_(0){}

  ~sdet_selective_search() override= default;;
  //Accessors
  void set_byte_image_view(vil_image_view<vxl_byte> const& view){
    byte_view_ = view;
    clear_regions();
    image_area_ = static_cast<float>(view.ni()*view.nj());
  }
  //:returns the current region segmentation including merged regions
  const std::map<unsigned, sdet_region_sptr >& regions(){return regions_;}
  const std::map<unsigned, bsta_histogram<float> >&  hists() {return hists_;}
  //: returns all the regions processed and merged so far
  const std::map<unsigned, sdet_region_sptr >& diverse_regions(){return diverse_regions_;}
  const std::map<unsigned, bsta_histogram<float> >&  diverse_hists() {return diverse_hists_;}
  vil_image_view<vil_rgb<vxl_byte> >& color_region_image(){return color_region_view_;}
  //Utility Methods
  void clear_regions(){
    hists_.clear();
    regions_.clear();
    while(!sim_.empty())
      sim_.pop();
    initial_regions_valid_=false;
  }
  void clear_all(){
    clear_regions();
    diverse_regions_.clear();
    diverse_hists_.clear();
  }
  void extract_initial_regions();
  void update_region_labels();
  void initialize_hists();
  void compute_initial_similarity();
  void merge_regions();
  void find_max_label();
  void create_color_region_view(unsigned min_region_area, unsigned max_region_area);
  //========================================
  // =======the main process function ======
  // typically executed a number of times with different parameters to create diversity
  // in the segmentation of intial and merged regions as per the example:
  //  proc.weight_thresh_ = 30.0f;
  //  proc.process();
  //  proc.four_or_eight_conn_ = 8;
  //  proc.process();
  //  proc.weight_thresh_ = 20.0f;
  //  proc.four_or_eight_conn_ = 4;
  //  proc.use_vd_edges_ = false;
  //  proc.process();
  //
  void process(){
    clear_regions();
    extract_initial_regions();
    find_max_label();
    initialize_hists();
    compute_initial_similarity();
    merge_regions();
    update_region_labels();
  }
 //: IO functions
  void save_bboxes(std::ostream& ostr);
 private:
  void insert_similarities(const sdet_region_sptr& r,  std::set<unsigned>& removed_labels);
  bool initial_regions_valid_;
  float image_area_;
  unsigned max_label_;
  unsigned diverse_max_label_;
  vil_image_view<vxl_byte> byte_view_;
  std::map<unsigned, bsta_histogram<float> >  hists_;
  std::map<unsigned, bsta_histogram<float> >  diverse_hists_;
  std::map<unsigned, sdet_region_sptr>  regions_;
  std::map<unsigned, sdet_region_sptr> diverse_regions_;
  vil_image_view<vil_rgb<vxl_byte> >  color_region_view_;
  std::priority_queue<region_sim> sim_;
};
#endif // sdet_selective_search_h_
