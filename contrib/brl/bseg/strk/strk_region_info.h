// This is brl/bseg/strk/strk_region_info.h
#ifndef strk_region_info_h_
#define strk_region_info_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for computing the mutual information between image regions
//
// \author
//  J.L. Mundy - March 14, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <strk/strk_tracking_face_2d_sptr.h>
#include <strk/strk_region_info_params.h>

class strk_region_info : public strk_region_info_params
{
 public:

  //Constructors/destructor

  strk_region_info(strk_region_info_params& tp);
 ~strk_region_info();

  //Accessors

  //unsigned char images for display purposes
  vil1_memory_image_of<unsigned char> image_0();
  vil1_memory_image_of<unsigned char> image_i();

  //Mutators

  void set_image_0(vil1_image& image);
  void set_image_i(vil1_image& image);
  void set_face_0(vtol_face_2d_sptr const& face);
  void set_face_i(vtol_face_2d_sptr const& face);
  void set_background_face(vtol_face_2d_sptr const& face);
  //Utility Methods

  void init();

  //Debug Methods

  //: Evalutate the information at the initial region
  void evaluate_info();
  
  //: Evalutate the information for the background region
  void evaluate_background_info();
  
  //Protected methods

 protected:
  bool map_i_to_0(vgl_h_matrix_2d<double>& H);

  //Members

  vil1_memory_image_of<float> image_0_;  //!< frame 0 intensity
  vil1_memory_image_of<float> image_i_;  //!< frame i intensity
  vil1_memory_image_of<float> hue_0_;  //!< hue of image_0
  vil1_memory_image_of<float> sat_0_;  //!< saturation of image_0
  vil1_memory_image_of<float> hue_i_;  //!< hue of image i
  vil1_memory_image_of<float> sat_i_;  //!< saturation of image_i
  vil1_memory_image_of<float> Ix_0_;  //!< x derivative of image_0 intensity
  vil1_memory_image_of<float> Iy_0_;  //!< y derivative of image_0 intensity
  vil1_memory_image_of<float> Ix_i_;  //!< x derivative of image_i intensity
  vil1_memory_image_of<float> Iy_i_;  //!< y derivative of image_i intensity
  vtol_face_2d_sptr face_0_;//!< initial model
  vtol_face_2d_sptr face_i_;//!< observation model
  vtol_face_2d_sptr background_face_;
  strk_tracking_face_2d_sptr info_face_;
};

#endif // strk_region_info_h_
