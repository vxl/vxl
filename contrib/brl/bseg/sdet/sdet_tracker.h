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
#include <vbl/vbl_array_2d.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <sdet/sdet_tracker_params.h>

//: A container to support sorting of samples
//  Will result in descending order according to correlation
struct sdet_correlated_face
{
  sdet_correlated_face ();
  ~sdet_correlated_face();
  void set_face(vtol_intensity_face_sptr const& f);
  void set_correlation(const float c) {c_ = c;}
  void set_avg_intensity(const float a) {a_ = a;}
  void set_int_mutual_info(const float mi);
  void set_grad_mutual_info(const float mi);
  float Ix(int i){return Ix_[i];}
  float Iy(int i){return Iy_[i];}
  void set_Ix(int i, const float Ix){Ix_[i] = Ix;}
  void set_Iy(int i, const float Iy){Iy_[i] = Iy;}
  vtol_intensity_face_sptr face() {return f_;}
  float correlation() const {return c_;}
  float avg_intensity() const {return a_;}
  float int_mutual_info(){return intensity_mi_;}
  float grad_mutual_info(){return gradient_dir_mi_;}
  float total_info(){return total_info_;}
 private:
  float a_;
  float c_;
  float intensity_mi_;
  float gradient_dir_mi_;
  float total_info_;
  float* Ix_;
  float* Iy_;
  vtol_intensity_face_sptr f_;
};

struct sdet_byte_hist
{
  sdet_byte_hist (int nbins = 16);
  ~sdet_byte_hist (){};
  int nbins(){return nbins_;}
  void upcount(const double x);
  double p(const int bin);
  void print();
 private:
  void compute_area();
  bool area_valid_;
  double area_;
  int nbins_;
  double delta_;
  vcl_vector<double> counts_;
};

struct sdet_byte_joint_hist
{
  sdet_byte_joint_hist (int nbins = 16);
  ~sdet_byte_joint_hist (){};
  int nbins(){return nbins_;}
  void upcount(const double a, const double b);
  double p(const int a, const int b);
  void print();
 private:
  void compute_volume();
  bool volume_valid_;
  int nbins_;
  double delta_;
  double volume_;
  vbl_array_2d<double> counts_;
};

struct sdet_gradient_dir_hist
{
  sdet_gradient_dir_hist (int nbins = 8);
  ~sdet_gradient_dir_hist (){};
  int nbins(){return nbins_;}
  void upcount(const double dir, const double mag);
  double p(const int bin);
  void print();
 private:
  void compute_area();
  bool area_valid_;
  double area_;
  int nbins_;
  double delta_;
  vcl_vector<double> counts_;
};

struct sdet_gradient_dir_joint_hist
{
  sdet_gradient_dir_joint_hist (int nbins = 8);
  ~sdet_gradient_dir_joint_hist (){};
  int nbins(){return nbins_;}
  void upcount(const double a, const double mag_a,
               const double b, const double mag_b);
  double p(const int a, const int b);
  void print();
 private:
  void compute_volume();
  bool volume_valid_;
  int nbins_;
  double delta_;
  double volume_;
  vbl_array_2d<double> counts_;
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
                   double tx, double ty, double theta, double scale);

  //: fill the pixels in the input face from the input image
  void fill_face(vtol_intensity_face_sptr const& face,
                 vil1_memory_image_of<float> const& image);

  //: set the gradient values in the model face
  void set_gradient(sdet_correlated_face* cf,
                    vil1_memory_image_of<float> const& Ix,
                    vil1_memory_image_of<float> const& Iy);

  //: compute a distance between a face and the underlying image
  void correlate_face(sdet_correlated_face* cf);

  //: Generate a random sample for face translation
  vtol_intensity_face_sptr
    generate_sample(vtol_intensity_face_sptr const& seed);

  //: Generate a more informative face using the previous method
  sdet_correlated_face*
  generate_cf_sample(sdet_correlated_face* seed);

  //: Form a new sample from the current image
  sdet_correlated_face*
    regenerate_cf_sample(sdet_correlated_face* sample);

  //: Form a new sample from the current image
  void transform_sample_in_place(sdet_correlated_face* sample,
                                 double tx, double ty, double theta);

  //: Compute the optical flow motion
  bool compute_motion(sdet_correlated_face* cf,
                      double& tx, double& ty, double& theta);

  double compute_correlation(sdet_correlated_face* cf);

  double compute_intensity_mutual_information(sdet_correlated_face* cf);
  double compute_gradient_mutual_information(sdet_correlated_face* cf);

  double compute_gradient_angle(sdet_correlated_face* cf);

  void compute_gradient_angle_hist(sdet_correlated_face* cf,
                                   vcl_vector<double>& ang_hist);

  double compute_angle_motion(sdet_correlated_face* cf);
  bool compute_scale_motion(sdet_correlated_face* cf, double& sx, double& sy);
  void mutual_info_face(sdet_correlated_face* cf);
  //members
  vil1_memory_image_of<float> image_0_;  //frame 0
  vil1_memory_image_of<float> image_i_;  //frame i
  vil1_memory_image_of<float> Ix_0_;  //x derivative of image_0
  vil1_memory_image_of<float> Iy_0_;  //y derivative of image_0
  vil1_memory_image_of<float> Ix_i_;  //x derivative of image_i
  vil1_memory_image_of<float> Iy_i_;  //y derivative of image_i
  vtol_face_2d_sptr initial_model_;//initial model position
  vcl_vector<sdet_correlated_face*> current_samples_;
  vcl_vector<sdet_correlated_face*> hypothesized_samples_;
  vcl_vector<double> sample_scores_;
  sdet_byte_hist* model_intensity_hist_;
  double model_intensity_entropy_;
  sdet_gradient_dir_hist* model_gradient_dir_hist_;
  double model_gradient_dir_entropy_;
};

#endif // sdet_tracker_h_
