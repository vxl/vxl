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
#include <vsol/vsol_box_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <brip/brip_roi_sptr.h>
#include <sdet/sdet_region_proc_params.h>

class sdet_region_proc : public sdet_region_proc_params
{
 public:
  //Constructors/destructor
  sdet_region_proc(sdet_region_proc_params& rpp);

  ~sdet_region_proc();
  //Accessors
  void set_image(vil1_image& image,
                 vsol_box_2d_sptr const& box = vsol_box_2d_sptr());
  vcl_vector<vtol_intensity_face_sptr>& get_regions(){return regions_;}

  //Utility Methods
  void extract_regions();
  void clear();
  vil1_image get_residual_image();
  //Debug methods
  vil1_image get_edge_image(){return edge_image_;}
 protected:
  //protected methods

  //members
  bool regions_valid_;      //process state flag
  vil1_image image_;  //input image
  brip_roi_sptr roi_; //region of interest
  vil1_image clip_; //clipped image region
  vil1_image edge_image_;//debug
  vcl_vector<vtol_intensity_face_sptr> regions_; //resulting intensity faces
};

#endif // sdet_region_proc_h_
