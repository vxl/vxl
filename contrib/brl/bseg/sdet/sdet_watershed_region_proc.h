#ifndef sdet_watershed_region_proc_h_
#define sdet_watershed_region_proc_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for extracting watershed regions
//
// uses a watershed algorithm to define regions
//
// \author
//  J.L. Mundy - July 11, 2004
//
// \verbatim
// Modifications
//  none
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vbl/vbl_array_2d.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_image.h>
#include <brip/brip_roi_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <sdet/sdet_region_sptr.h>
#include <sdet/sdet_watershed_region_proc_params.h>

class sdet_watershed_region_proc : public sdet_watershed_region_proc_params
{
 public:
  enum priority_code{DARK=0, LIGHT, BIG, SMALL};
  //Constructors/destructor
  sdet_watershed_region_proc(sdet_watershed_region_proc_params& wrpp);

  ~sdet_watershed_region_proc();
  //Accessors
  //:set image and potentially clip
  void set_image(vil1_image const& image, 
                 vsol_box_2d_sptr const& box = vsol_box_2d_sptr());
  //:set image roi
  void set_roi(brip_roi_sptr const& roi);
  brip_roi_sptr roi(){return roi_;}

  //:clip image assuming image and roi are set
  void clip_image();

  vil1_memory_image_of<float> image(){return image_;}
  vil1_memory_image_of<float> clip(){return clip_;}

  //clear region array
  void clear();

  vcl_vector<sdet_region_sptr>& regions(){return regions_;}

  //Utility Methods
  bool extract_regions();
  bool merge_regions();
  vil1_image get_residual_image();

  //Debug methods
  void print_region_info();
  vil1_image overlay_image(){return overlay_image_;}
  vil1_image region_image();
  vcl_vector<vsol_polygon_2d_sptr> boundaries();
 protected:
  //protected methods
  void chip_out_roi(vil1_image& image);
  void scan_region_data(vbl_array_2d<unsigned int> const & lab_array);

  bool add_adjacency(sdet_region_sptr const& reg,
                     sdet_region_sptr const& adj_reg);
  bool remove_adjacencies(sdet_region_sptr const& reg);
  bool adjacent_regions(sdet_region_sptr const& reg,
                        vcl_vector<sdet_region_sptr>& adj_regs);
  bool compute_region_image();
  void compute_boundaries();
  //members
  int x0_;
  int y0_;
  int xs_;
  int ys_;
  bool regions_valid_;      //process state flag
  bool region_image_valid_;
  bool boundaries_valid_;
  unsigned int min_label_;
  unsigned int max_label_;
  vil1_memory_image_of<float> image_;  //input image
  vil1_memory_image_of<float> clip_;  //clipped input image
  brip_roi_sptr roi_;
  vil1_image overlay_image_; // for display of original watershed regions
  vil1_memory_image_of<unsigned char> region_image_; //for display of digital regions
  vcl_vector<sdet_region_sptr> regions_; //resulting intensity faces
  vcl_map<sdet_region_sptr, vcl_vector<sdet_region_sptr>* > region_adjacency_;
  vcl_vector<vsol_polygon_2d_sptr> boundaries_;
};

#endif // sdet_watershed_region_proc_h_
