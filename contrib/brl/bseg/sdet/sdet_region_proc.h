#ifndef sdet_region_proc_h_
#define sdet_region_proc_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for extracting expanded resolution regions
//
// edgel_regions uses a flood fill algorithm and is thus
// region labeling is only as localized as one pixel.
//
// \author
//  J.L. Mundy - April 11, 2001
//
// \verbatim
// Modifications
//  8 May 2002 - Peter Vanroose - class name changed from region_proc to sdet_region_proc
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <brip/brip_roi_sptr.h>
#include <sdet/sdet_region_proc_params.h>
class sdet_detector;
class sdet_region_proc : public sdet_region_proc_params
{
 public:
  //Constructors/destructor
  sdet_region_proc(sdet_region_proc_params& rpp);

  ~sdet_region_proc();
  //Accessors
  void set_image(vil1_image const& image,
                 vsol_box_2d_sptr const& box = vsol_box_2d_sptr());

  void set_image_resource(vil_image_resource_sptr const& image,
                          vsol_box_2d_sptr const& box = vsol_box_2d_sptr());


  vcl_vector<vtol_intensity_face_sptr>& get_regions(){return regions_;}

  //Utility Methods
  void extract_regions();
  void clear();

  vil1_image get_residual_image();
  vil_image_view<float> get_residual_image_view();

  //Debug methods
  vil1_image get_edge_image(){return edge_image_;}
  vil_image_view<unsigned char> get_edge_image_view();
  vil_image_resource_sptr get_edge_image_resource();
 protected:
  //protected methods
  bool set_detector_image_vil(sdet_detector& d); 
  bool set_detector_image_vil1(sdet_detector& d);

  //members
  bool regions_valid_;      //process state flag
  bool use_vil_image_;   //flag to determine image type
  vil1_image image_;  //input vil1_image
  vil_image_resource_sptr vimage_; //input vil image
  brip_roi_sptr roi_; //region of interest
  vil1_image clip_; //clipped image region
  vil_image_resource_sptr vclip_; //clipped image resource
  vil1_image edge_image_;//debug
  vcl_vector<vtol_intensity_face_sptr> regions_; //resulting intensity faces
};

#endif // sdet_region_proc_h_
