// This is brl/bseg/sdet/sdet_tracker.h
#ifndef sdet_tracker_h_
#define sdet_tracker_h_
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
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <sdet/sdet_tracker_params.h>
//: A container to support sorting of samples
//  Will result in descending order according to correlation
struct sdet_correlated_face
{
  sdet_correlated_face () {};

  void set_face(vtol_intensity_face_sptr const& f) {f_ = f;}
  void set_correlation(const float c) {c_ = c;}
  void set_avg_intensity(const float a) {a_ = a;}
  vtol_intensity_face_sptr face() {return f_;}
  float correlation() const {return c_;}
  float avg_intensity() const {return a_;}

 private:
  float a_;
  float c_;
  vtol_intensity_face_sptr f_;
};

class sdet_tracker : public sdet_tracker_params
{
 public:
  //Constructors/destructor
  sdet_tracker(sdet_tracker_params& tp);

  ~sdet_tracker();
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
                   double tx, double ty);
  
  //: fill the pixels in the input face from the input image
  void fill_face(vtol_intensity_face_sptr const& face,
                 vil1_image image);

  //: perform a normalized cross-correlation between the face and image
  void correlate_face(vtol_intensity_face_sptr const& face1,
                      vil1_image image, double& corr, double& avg_int);
  
  //: Generate a random sample for face translation
  vtol_intensity_face_sptr 
    generate_sample(vtol_intensity_face_sptr const& seed);
  
  //members
  vil1_image image_0_;  //frame 0
  vil1_image image_i_;  //frame i
  vtol_face_2d_sptr initial_model_;//initial model position
  vcl_vector<vtol_intensity_face_sptr> current_samples_;
  vcl_vector<sdet_correlated_face*> hypothesized_samples_;
  vcl_vector<double> sample_scores_;

};

#endif // sdet_tracker_h_
