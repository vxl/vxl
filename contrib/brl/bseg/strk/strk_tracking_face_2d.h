// This is brl/bseg/sdet/strk_tracking_face_2d.h
#ifndef strk_tracking_face_2d_h_
#define strk_tracking_face_2d_h_
//:
// \file
// \brief a face for tracking with mutual information
//
// \verbatim
// The shape and intensity data for this class are maintained by the 
// vtol_intensity_face member.  Additional gradient information is maintained
// in order to support the formation of a gradient direction histogram.
// Local histogram structs are defined to collect the intensity and gradient
// statistics of the face.
// 
//
// \author
//    Joseph L. Mundy - October 29, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include<vbl/vbl_ref_count.h>
#include<vbl/vbl_array_2d.h>
#include<vil1/vil1_memory_image_of.h>
#include<vsol/vsol_line_2d_sptr.h>
#include<vtol/vtol_intensity_face.h>
#include<strk/strk_tracking_face_2d_sptr.h>

//===================== HISTOGRAM DEFS ===================================
struct strk_histf
{
  strk_histf (double range = 255, int nbins = 16);
  ~strk_histf (){};
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

struct strk_joint_histf
{
  strk_joint_histf (int nbins = 16);
  ~strk_joint_histf (){};
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

struct strk_double_histf
{
  strk_double_histf (int nbins = 8);
  ~strk_double_histf (){};
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
struct strk_double_joint_histf
{
  strk_double_joint_histf (int nbins = 8);
  ~strk_double_joint_histf (){};
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
//
//========================TRACKING_FACE_2D==================================
//
class strk_tracking_face_2d : public vbl_ref_count
{
 public:  
  strk_tracking_face_2d(vtol_face_2d_sptr const& face,
                        vil1_memory_image_of<float> const& image);

  strk_tracking_face_2d(vtol_face_2d_sptr const& face,
                        vil1_memory_image_of<float> const& image,
                        vil1_memory_image_of<float> const& Ix,
                        vil1_memory_image_of<float> const& Iy);

  strk_tracking_face_2d(vtol_intensity_face_sptr const& intf);
  strk_tracking_face_2d(strk_tracking_face_2d_sptr const& tf);
  ~strk_tracking_face_2d();
  //:accessors
  vtol_intensity_face_sptr face(){return intf_;}
  bool gradient_needed(){return gradient_info_;}
  int intensity_hist_bins(){return intensity_hist_bins_;}
  int gradient_dir_hist_bins(){return gradient_dir_hist_bins_;}
  void set_int_mutual_info(const float mi);
  void set_grad_mutual_info(const float mi);
  float Ix(int i){return Ix_[i];}
  float Iy(int i){return Iy_[i];}
  void set_Ix(int i, const float Ix){Ix_[i] = Ix;}
  void set_Iy(int i, const float Iy){Iy_[i] = Iy;}
  float int_mutual_info(){return intensity_mi_;}
  float grad_mutual_info(){return gradient_dir_mi_;}
  float total_info(){return total_info_;}
  //: utilities
  bool compute_mutual_information(vil1_memory_image_of<float> const& image,
                                  vil1_memory_image_of<float> const& Ix,
                                  vil1_memory_image_of<float> const& Iy);
  //:from the face vertices
  void centroid(double& x, double& y);

  //:copy of intf interface
  void reset() const {intf_->reset();}
  bool next() const {return intf_->next();}
  float Xo()const { return intf_->Xo(); }
  float Yo()const { return intf_->Yo(); }
  void set_X(float x){intf_->set_X(x); }
  void set_Y(float y){intf_->set_Y(y); }
  int Npix()const {return intf_->Npix(); }
  float X() const { return intf_->X(); }
  float Y() const { return intf_->Y(); }
  unsigned short I() const {return intf_->I();}
  
  //:transformation
  void transform(const double tx, const double ty, 
                 const double theta, const double scale);


 private:
  //:local functions
  void init_face_info(vil1_memory_image_of<float> const& image,
                      vil1_memory_image_of<float> const& Ix,
                      vil1_memory_image_of<float> const& Iy);

  void set_gradient(vil1_memory_image_of<float> const& Ix,
                    vil1_memory_image_of<float> const& Iy);

  void init(vtol_face_2d_sptr const& face, vil1_memory_image_of<float> const& image,
          vil1_memory_image_of<float> const& Ix,
          vil1_memory_image_of<float> const& Iy);
  double 
    compute_intensity_mutual_information(vil1_memory_image_of<float> const& image);

  double 
    compute_gradient_mutual_information(vil1_memory_image_of<float> const& Ix,
                                        vil1_memory_image_of<float> const& Iy);
  
  //:members
  vtol_intensity_face_sptr intf_;
  bool gradient_info_;
  float intensity_mi_;
  float gradient_dir_mi_;
  float total_info_;
  float* Ix_; 
  float* Iy_; 
  int intensity_hist_bins_;
  int gradient_dir_hist_bins_;
  double model_intensity_entropy_;
  double model_gradient_dir_entropy_;
};

#endif // strk_tracking_face_2d_h_
