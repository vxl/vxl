// This is brl/bseg/strk/strk_info_tracker.h
#ifndef strk_info_tracker_h_
#define strk_info_tracker_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for tracking a face_2d based on intensity matching
//
//  The info_tracker operates by randomly generating a set of hypotheses in the
//  vicinity of the previous best n matches. These new hypotheses are tested,
//  (for now by normalized cross-correlation) and ranked to select the best
//  matches for the next iteration.  The current algorithm only adjusts the
//  translation each tracked face.
//
// \author
//  J.L. Mundy - August 20, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <strk/strk_tracking_face_2d_sptr.h>
#include <strk/strk_info_tracker_params.h>

class strk_info_tracker : public strk_info_tracker_params
{
 public:
  //Constructors/destructor
  strk_info_tracker(strk_info_tracker_params& tp);

  ~strk_info_tracker();
  //Accessors
  void set_image_0(vil1_image& image);
  void set_image_i(vil1_image& image);
  void set_initial_model(vtol_face_2d_sptr const& face);
  vtol_face_2d_sptr get_best_sample();
  void get_samples(vcl_vector<vtol_face_2d_sptr> & samples);
  strk_tracking_face_2d_sptr tf(int i){return current_samples_[i];}
  //Utility Methods
  void init();
  void generate_samples();
  void cull_samples();
  void track();
  void clear();

 protected:
  //protected methods
  //:random choice to refresh the intensity data of a sample
  bool refresh_sample();

  //: Generate a new tracking face
  strk_tracking_face_2d_sptr
  generate_randomly_positioned_sample(strk_tracking_face_2d_sptr const& seed);
  //: Generate a new tracking face with refreshed data
  strk_tracking_face_2d_sptr
  clone_and_refresh_data(strk_tracking_face_2d_sptr const& sample);


  //  void refine_best_sample();
  //members
  vil1_memory_image_of<float> image_0_;  //frame 0 intensity
  vil1_memory_image_of<float> image_i_;  //frame i intensity
  vil1_memory_image_of<float> hue_0_;  //hue of image_0
  vil1_memory_image_of<float> sat_0_;  //saturation of image_0
  vil1_memory_image_of<float> hue_i_;  //hue of image i
  vil1_memory_image_of<float> sat_i_;  //saturation of image_i
  vil1_memory_image_of<float> Ix_0_;  //x derivative of image_0 intensity
  vil1_memory_image_of<float> Iy_0_;  //y derivative of image_0 intensity
  vil1_memory_image_of<float> Ix_i_;  //x derivative of image_i intensity
  vil1_memory_image_of<float> Iy_i_;  //y derivative of image_i intensity
  vtol_face_2d_sptr initial_model_;//initial model position
  vcl_vector<strk_tracking_face_2d_sptr> current_samples_;
  vcl_vector<strk_tracking_face_2d_sptr> hypothesized_samples_;
  vcl_vector<strk_tracking_face_2d_sptr> track_history_;
  double model_intensity_entropy_;
  double model_gradient_dir_entropy_;
  double model_color_entropy_;
};

#endif // strk_info_tracker_h_
