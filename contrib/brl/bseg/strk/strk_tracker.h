// This is brl/bseg/strk/strk_tracker.h
#ifndef strk_tracker_h_
#define strk_tracker_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for tracking a face_2d based on intensity matching
//
//  The tracker operates by randomly generating a set of hypotheses in the
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
#include <vtol/vtol_intensity_face_sptr.h>
#include <strk/strk_tracker_params.h>

//: A container to support sorting of samples
//  Will result in descending order according to correlation
struct strk_correlated_face
{
  strk_correlated_face ();
  ~strk_correlated_face();
  void set_face(vtol_intensity_face_sptr const& f);
  void set_correlation(const float c) {c_ = c;}
  vtol_intensity_face_sptr face() {return f_;}
  float correlation() const {return c_;}
  float Ix(int i){return Ix_[i];}
  float Iy(int i){return Iy_[i];}
  void set_Ix(int i, const float Ix){Ix_[i] = Ix;}
  void set_Iy(int i, const float Iy){Iy_[i] = Iy;}
 private:
  float c_;
  vtol_intensity_face_sptr f_;
  float* Ix_;
  float* Iy_;
};

class strk_tracker : public strk_tracker_params
{
 public:
  //Constructors/destructor
  strk_tracker(strk_tracker_params& tp);

  ~strk_tracker();
  //Accessors
  void set_image_0(vil1_image& image);
  void set_image_i(vil1_image& image);
  void set_initial_model(vtol_face_2d_sptr const& face);
  vtol_face_2d_sptr get_best_sample();
  void get_samples(vcl_vector<vtol_face_2d_sptr> & samples);
  //Utility Methods
  void init();
  void generate_samples();
  void cull_samples();
  void track();
  void clear();

 protected:
  //protected methods
  //: translate the input face by tx and ty
  vtol_intensity_face_sptr
    transform_face(vtol_intensity_face_sptr const& face,
                   double tx, double ty, double theta, double scale);

  //: fill the pixels in the input face from the input image
  void fill_face(vtol_intensity_face_sptr const& face,
                 vil1_memory_image_of<float> const& image);

  void set_gradient(strk_correlated_face* cf,
                    vil1_memory_image_of<float> const& Ix,
                    vil1_memory_image_of<float> const& Iy);
  //: compute a distance between a face and the underlying image
  void correlate_face(strk_correlated_face* cf);

  //: Generate a random sample for face translation
  vtol_intensity_face_sptr
    generate_sample(vtol_intensity_face_sptr const& seed);

  //: Generate a more informative face using the previous method
  strk_correlated_face*
  generate_cf_sample(strk_correlated_face* seed);

  //: Form a new sample from the current image
  strk_correlated_face*
    regenerate_cf_sample(strk_correlated_face* sample);

  //: Form a new sample from the current image
  void transform_sample_in_place(strk_correlated_face* sample,
                                 double tx, double ty,
                                 double theta, double scale);

  //: Compute the optical flow motion
  bool compute_motion(strk_correlated_face* cf,
                      double& tx, double& ty,
                      double& theta, double& scale);

  double compute_correlation(strk_correlated_face* cf);

  double compute_gradient_angle(strk_correlated_face* cf);

  double compute_angle_motion(strk_correlated_face* cf);
  bool compute_scale_motion(strk_correlated_face* cf, double& sx, double& sy);
  //members
  vil1_memory_image_of<float> image_0_;  //frame 0
  vil1_memory_image_of<float> image_i_;  //frame i
  vil1_memory_image_of<float> Ix_0_;  //x derivative of image_0
  vil1_memory_image_of<float> Iy_0_;  //y derivative of image_0
  vil1_memory_image_of<float> Ix_i_;  //x derivative of image_i
  vil1_memory_image_of<float> Iy_i_;  //y derivative of image_i
  vtol_face_2d_sptr initial_model_;//initial model position
  vcl_vector<strk_correlated_face*> current_samples_;
  vcl_vector<strk_correlated_face*> hypothesized_samples_;
  vcl_vector<double> sample_scores_;
};

#endif // strk_tracker_h_
