//-*-c++-*-
// This is brl/bseg/strk/strk_info_model_tracker.h
#ifndef strk_info_model_tracker_h_
#define strk_info_model_tracker_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for tracking an articulated model
// 
//  The info_model_tracker operates by randomly generating a 
//  set of hypotheses in the vicinity of the previous best n matches. 
//  These new hypotheses are tested,using mutual information on intensity 
//  and optionally gradient direction. Each sample represents a different
//  configuration of the strk_art_art_info_model.
//  
// \author
//  J.L. Mundy - November 05, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_matrix.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_intensity_face.h>
#include <strk/strk_tracking_face_2d_sptr.h>
#include <strk/strk_art_info_model_sptr.h>
#include <strk/strk_info_model_tracker_params.h>

class strk_info_model_tracker : public strk_info_model_tracker_params
{
 public:
  //Constructors/destructor
  strk_info_model_tracker(strk_info_model_tracker_params& tp);

  ~strk_info_model_tracker();
  //Accessors
  void set_image_0(vil1_image& image);
  void set_image_i(vil1_image& image);
  void set_initial_model(vcl_vector<vtol_face_2d_sptr> const & faces)
  {initial_model_ = faces;}
  strk_art_info_model_sptr get_best_sample();
  void get_samples(vcl_vector<strk_art_info_model_sptr> & samples);
  strk_art_info_model_sptr tf(int i){return current_samples_[i];}
  //Utility Methods
  void init();
  void generate_samples();
  void cull_samples();
  void track(); 
  void clear(); 

 protected:
  //protected methods

  //: fill the pixels in the input face from the input image
  void fill_face(vtol_intensity_face_sptr const& face,
                 vil1_memory_image_of<float> const& image);

  //: set the gradient values in the model face
  void set_gradient(strk_art_info_model_sptr tf, 
                    vil1_memory_image_of<float> const& Ix,
                    vil1_memory_image_of<float> const& Iy);

  //: Generate a random sample
  strk_art_info_model_sptr 
  generate_model(strk_art_info_model_sptr const& seed);

  double compute_intensity_mutual_information(strk_art_info_model_sptr tf);
  double compute_gradient_mutual_information(strk_art_info_model_sptr tf);
  
  
  //members
  vil1_memory_image_of<float> image_0_;  //frame 0
  vil1_memory_image_of<float> image_i_;  //frame i
  vil1_memory_image_of<float> Ix_0_;  //x derivative of image_0
  vil1_memory_image_of<float> Iy_0_;  //y derivative of image_0
  vil1_memory_image_of<float> Ix_i_;  //x derivative of image_i
  vil1_memory_image_of<float> Iy_i_;  //y derivative of image_i

  vcl_vector<vtol_face_2d_sptr> initial_model_;
  vcl_vector<strk_art_info_model_sptr> current_samples_;
  vcl_vector<strk_art_info_model_sptr> hypothesized_samples_;
  double model_intensity_entropy_;
  double model_gradient_dir_entropy_;
};

#endif // strk_info_model_tracker_h_
