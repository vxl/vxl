//-*-c++-*-
// This is brl/bseg/sdet/sdet_info_tracker.h
#ifndef sdet_info_tracker_h_
#define sdet_info_tracker_h_
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
#include <vbl/vbl_array_2d.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_intensity_face.h>
#include <sdet/sdet_info_tracker_params.h>
//: A container to support sorting of samples
//  Will result in descending order according to correlation
struct sdet_augmented_face
{
  sdet_augmented_face ();
  sdet_augmented_face (sdet_augmented_face* af);
  ~sdet_augmented_face();
  void set_face(vtol_intensity_face_sptr const& f);
  void set_int_mutual_info(const float mi);
  void set_grad_mutual_info(const float mi);
  float Ix(int i){return Ix_[i];}
  float Iy(int i){return Iy_[i];}
  void set_Ix(int i, const float Ix){Ix_[i] = Ix;}
  void set_Iy(int i, const float Iy){Iy_[i] = Iy;}
  vtol_intensity_face_sptr face() {return f_;}
  float int_mutual_info(){return intensity_mi_;}
  float grad_mutual_info(){return gradient_dir_mi_;}
  float total_info(){return total_info_;}
 private:
  float intensity_mi_;
  float gradient_dir_mi_;
  float total_info_;
  float* Ix_; 
  float* Iy_; 
  vtol_intensity_face_sptr f_;
};
struct sdet_hist
{
  sdet_hist (double range = 255, int nbins = 16);
  ~sdet_hist (){};
  int nbins(){return nbins_;}
  void upcount(const double x);
  double p(const int bin);
  void print();
  private:
  void compute_area();
  bool area_valid_;
  double area_;
  int nbins_;
  double range_;
  double delta_;
  vcl_vector<double> counts_;
};

struct sdet_joint_hist
{
  sdet_joint_hist (int nbins = 16);
  ~sdet_joint_hist (){};
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

struct sdet_double_hist
{
  sdet_double_hist (int nbins = 8);
  ~sdet_double_hist (){};
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
struct sdet_double_joint_hist
{
  sdet_double_joint_hist (int nbins = 8);
  ~sdet_double_joint_hist (){};
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

class sdet_info_tracker : public sdet_info_tracker_params
{
 public:
  //Constructors/destructor
  sdet_info_tracker(sdet_info_tracker_params& tp);

  ~sdet_info_tracker();
  //Accessors
  void set_image_0(vil1_image& image);
  void set_image_i(vil1_image& image);
  void set_initial_model(vtol_face_2d_sptr const& face);
  vtol_face_2d_sptr get_best_sample();
  void get_samples(vcl_vector<vtol_face_2d_sptr> & samples);
  sdet_augmented_face* af(int i){return current_samples_[i];}
  //Utility Methods
  void init();
  void generate_samples();
  void cull_samples();
  void track(); 
  void clear(); 

 protected:
  //protected methods

  //: transform a face, returning a new face
  vtol_intensity_face_sptr 
    transform_face(vtol_intensity_face_sptr const& face,
                   const double tx, const double ty, 
                   const double theta, const double scale);
  
  //: transform an existing augmented face
  void transform_sample_in_place(sdet_augmented_face* sample,
                                 const double tx, const double ty, 
                                 const double theta, const double scale);

  //: fill the pixels in the input face from the input image
  void fill_face(vtol_intensity_face_sptr const& face,
                 vil1_memory_image_of<float> const& image);

  //: set the gradient values in the model face
  void set_gradient(sdet_augmented_face* cf, 
                    vil1_memory_image_of<float> const& Ix,
                    vil1_memory_image_of<float> const& Iy);

  //: Generate a random sample for face translation
  vtol_intensity_face_sptr 
    generate_face(vtol_intensity_face_sptr const& seed);

  //: Generate a more informative face using the previous method
  sdet_augmented_face* 
  generate_augmented_face(sdet_augmented_face* seed);
  

  double compute_intensity_mutual_information(sdet_augmented_face* cf);
  double compute_gradient_mutual_information(sdet_augmented_face* cf);
  void mutual_info_face(sdet_augmented_face* cf);

  //members
  vil1_memory_image_of<float> image_0_;  //frame 0
  vil1_memory_image_of<float> image_i_;  //frame i
  vil1_memory_image_of<float> Ix_0_;  //x derivative of image_0
  vil1_memory_image_of<float> Iy_0_;  //y derivative of image_0
  vil1_memory_image_of<float> Ix_i_;  //x derivative of image_i
  vil1_memory_image_of<float> Iy_i_;  //y derivative of image_i
  vtol_face_2d_sptr initial_model_;//initial model position
  vcl_vector<sdet_augmented_face*> current_samples_;
  vcl_vector<sdet_augmented_face*> hypothesized_samples_;
  sdet_hist* model_intensity_hist_;
  double model_intensity_entropy_;
  sdet_double_hist* model_gradient_dir_hist_;
  double model_gradient_dir_entropy_;

};

#endif // sdet_info_tracker_h_
