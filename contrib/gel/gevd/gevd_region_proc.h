#ifndef gevd_region_proc_h_
#define gevd_region_proc_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for extracting expanded resolution regions
//
// edgel_regions uses a flood fill algorithm and is thus
// region labeling is only as localized as one pixel.
// In the CAD application, we need sub-pixel details. So
// In this algorithm the original image resolution is
// expanded using Gaussian interpolation, typically by
// a factor of two. Then the resulting regions are
// transformed back to original image coordinates.
//
// \author
//  J.L. Mundy - April 11, 2001
//
// \verbatim
// Modifications
//  8 May 2002 - Peter Vanroose - class name changed from region_proc to gevd_region_proc
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <gevd/gevd_bufferxy.h>
#include <vil1/vil1_image.h>
#include <vdgl/vdgl_digital_region.h>
#include <gevd/gevd_region_proc_params.h>

class gevd_region_proc : public gevd_region_proc_params
{
public:
  //Constructors/destructor
  gevd_region_proc();

  ~gevd_region_proc() override;
  //Accessors
  void set_image(vil1_image& image);
//  void set_roi_proc(lung_roi_proc_ref& roi_proc){roi_proc_ = roi_proc;}

  // vector<gevd_poly_intensity_face_sptr>& get_regions(){return regions_;}
  std::vector<vdgl_digital_region *>& get_regions(){return regions_;}

  //Utility Methods
  void extract_regions();
  void clear();

  void set_expand_scale(float scale){expand_scale_=scale;}

  //Debug methods
  void set_debug(){debug_ = true;}
  void clear_debug(){debug_ = false;}

protected:
  //protected methods
  gevd_bufferxy* get_image_buffer(vil1_image& image);
  gevd_bufferxy* get_float_buffer(gevd_bufferxy* b);
  gevd_bufferxy* put_float_buffer(gevd_bufferxy* fbuf);
  vil1_image buffer_to_image(gevd_bufferxy* buf);
  void restore_image_rois();
  //members
  bool debug_;//debug flag
  bool regions_valid_;      //process state flag
  float expand_scale_;
  float burt_adelson_factor_;
  vil1_image image_;  //input image
  gevd_bufferxy* buf_;
  //lung_roi_proc_ref roi_proc_; //Lung roi processor
  //vector<gevd_poly_intensity_face_ref> regions_; //resulting intensity faces
  std::vector<vdgl_digital_region *> regions_; //resulting digital regions
};
#endif // gevd_region_proc_h_
