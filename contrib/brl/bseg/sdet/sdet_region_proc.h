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
#include <vil/vil_image.h>
#include <vdgl/vdgl_intensity_face.h>
#include <sdet/sdet_region_proc_params.h>

class sdet_region_proc : public sdet_region_proc_params
{
public:
  //Constructors/destructor
  sdet_region_proc(sdet_region_proc_params& rpp);

  ~sdet_region_proc();
  //Accessors
  void set_image(vil_image& image);
  vcl_vector<vdgl_intensity_face_sptr>& get_regions(){return regions_;}

  //Utility Methods
  void extract_regions();
  void clear();
  vil_image get_residual_image();
  //Debug methods
  vil_image get_edge_image(){return edge_image_;}
protected:
  //protected methods

  //members
  bool regions_valid_;      //process state flag
  vil_image image_;  //input image
  vil_image edge_image_;//debug
  vcl_vector<vdgl_intensity_face_sptr> regions_; //resulting intensity faces
};
#endif // sdet_region_proc_h_
