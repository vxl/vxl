#ifndef vsrl_region_disparity_h_
#define vsrl_region_disparity_h_
//:
// \file
//
// \brief Routines to calculate disparity based on regions rather than pixels.
//
// The routine takes a form of region segmentation as input.
// The segmentation can be vtol_intensity_faces or vdgl_digital_regions.
// A disparity map for the image is also taken as input.
// The algorithm takes the average of the disparity in each region and
// assigns it to each pixel in the region.
//
// \author G.W. Brooksby
// \date 09/13/03, 9/17/03
// Written at ORD on two LONG layovers...

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_soview2D.h>
#include <vsrl/vsrl_parameters.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_intensity_face_sptr.h>

class vsrl_region_disparity
{
  enum region_type{UNDEFINED=0, INTENSITY_FACE, DIGITAL_REGION};
 public:
  vsrl_region_disparity();
  vsrl_region_disparity(vil1_image* li, vil1_image* ri);
  ~vsrl_region_disparity();

  bool Execute();
  void SetLeftImage(vil1_image* li){l_img_ = li;}
  void SetRightImage(vil1_image* ri) {r_img_ = ri;}
  void SetDisparityImage(vil1_memory_image_of<unsigned char>* di) {d_img_ = di;}
  vcl_vector<vtol_intensity_face_sptr>* GetIFRegions() {return if_regions_;}
  vcl_vector<vdgl_digital_region*>* GetDigitalRegions() {return digi_regions_;}
  void SetRegions(vcl_vector<vtol_intensity_face_sptr>* regs);
  void SetRegions(vcl_vector<vdgl_digital_region*>* regs);
  vil1_memory_image_of<double>* GetRegionDisparities() {return reg_disp_img_;}

 private:
  void init();
  vil1_image* l_img_; // Left image
  vil1_image* r_img_; // Right image
  vil1_memory_image_of<unsigned char>* d_img_; // Disparity image
  vil1_memory_image_of<double>* reg_disp_img_; // Newly calculated disparity image
  vcl_vector<vtol_intensity_face_sptr>* if_regions_; // Intensity Face Regions
  vcl_vector<vdgl_digital_region*>* digi_regions_; // Digital Regions
  bool run_digital_regions();
  bool run_intensity_faces();
  int region_type_;
};

#endif // vsrl_region_disparity_h_
