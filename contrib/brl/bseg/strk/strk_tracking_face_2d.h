// This is brl/bseg/strk/strk_tracking_face_2d.h
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
template <class T> class strk_histf
{
 public:
  strk_histf(T range = 255, unsigned int nbins = 16);
 ~strk_histf() {}
  unsigned int nbins() const { return nbins_; }
  void upcount(T x);
  T p(unsigned int bin) const;
  void print() const;
 private:
  void compute_area() const; // mutable const
  mutable bool area_valid_;
  mutable T area_;
  unsigned int nbins_;
  T range_;
  T delta_;
  vcl_vector<T> counts_;
};

template <class T> class strk_joint_histf
{
 public:
  strk_joint_histf(unsigned int nbins = 16);
 ~strk_joint_histf() {}
  unsigned int nbins() const { return nbins_; }
  void upcount(T a, T b);
  T p(unsigned int a, unsigned int b) const;
  void print() const;
 private:
  void compute_volume() const; // mutable const
  mutable bool volume_valid_;
  mutable T volume_;
  unsigned int nbins_;
  T delta_;
  vbl_array_2d<T> counts_;
};

template <class T> class strk_double_histf
{
 public:
  strk_double_histf(unsigned int nbins = 8);
 ~strk_double_histf() {}
  unsigned int nbins() const { return nbins_; }
  void upcount(T dir, T mag);
  T p(unsigned int bin) const;
  void print() const;
 private:
  void compute_area() const; // mutable const
  mutable bool area_valid_;
  mutable T area_;
  unsigned int nbins_;
  T delta_;
  vcl_vector<T> counts_;
};

template <class T> class strk_double_joint_histf
{
 public:
  strk_double_joint_histf(unsigned int nbins = 8);
 ~strk_double_joint_histf() {}
  unsigned int nbins() const { return nbins_; }
  void upcount(T a, T mag_a,
               T b, T mag_b);
  T p(unsigned int a, unsigned int b) const;
  void print() const;
 private:
  void compute_volume() const; // mutable const
  mutable bool volume_valid_;
  mutable T volume_;
  unsigned int nbins_;
  T delta_;
  vbl_array_2d<T> counts_;
};

//
//========================TRACKING_FACE_2D==================================
//
class strk_tracking_face_2d : public vbl_ref_count
{
 public:
  strk_tracking_face_2d(vtol_face_2d_sptr const& face,
                        vil1_memory_image_of<float> const& image,
                        vil1_memory_image_of<float> const& Ix,
                        vil1_memory_image_of<float> const& Iy,
                        vil1_memory_image_of<float> const& hue,
                        vil1_memory_image_of<float> const& sat,
                        const float min_gradient);

  strk_tracking_face_2d(vtol_intensity_face_sptr const& intf);
  strk_tracking_face_2d(strk_tracking_face_2d_sptr const& tf);
  ~strk_tracking_face_2d();
  //:accessors
  void set_min_gradient(const float min_gradient){min_gradient_=min_gradient;}
  vtol_intensity_face_sptr face() const { return intf_; }
  bool gradient_needed() const { return gradient_info_; }
  bool color_needed() const { return color_info_; }
  unsigned int intensity_hist_bins() const { return intensity_hist_bins_; }
  unsigned int gradient_dir_hist_bins() const { return gradient_dir_hist_bins_; }
  unsigned int color_hist_bins() const { return color_hist_bins_; }
  void set_int_mutual_info(float mi);
  void set_grad_mutual_info(float mi);
  void set_color_mutual_info(float mi);
  float Ix(int i) const { return Ix_[i]; }
  float Iy(int i) const { return Iy_[i]; }
  void set_Ix(int i, float Ix) { Ix_[i] = Ix; }
  void set_Iy(int i, float Iy) { Iy_[i] = Iy; }
  float hue(int i) const { return hue_[i]; }
  float sat(int i) const { return sat_[i]; }
  void set_hue(int i, float Ix) { hue_[i] = Ix; }
  void set_sat(int i, float Iy) { sat_[i] = Iy; }
  float int_mutual_info() const { return intensity_mi_; }
  float grad_mutual_info() const { return gradient_dir_mi_; }
  float color_mutual_info() const { return color_mi_; }
  float total_info() const { return total_info_; }
  //: internal entropies for debugging purposes
  float model_intensity_entropy(){return model_intensity_entropy_;}
  float intensity_entropy(){return intensity_entropy_;}
  float intensity_joint_entropy(){return intensity_joint_entropy_;}

  float model_gradient_entropy(){return model_gradient_dir_entropy_;}
  float gradient_entropy(){return gradient_dir_entropy_;}
  float gradient_joint_entropy(){return gradient_joint_entropy_;}

  float model_color_entropy(){return model_color_entropy_;}
  float color_entropy(){return color_entropy_;}
  float color_joint_entropy(){return color_joint_entropy_;}
  
  //: utilities
  bool compute_mutual_information(vil1_memory_image_of<float> const& image,
                                  vil1_memory_image_of<float> const& Ix,
                                  vil1_memory_image_of<float> const& Iy,
                                  vil1_memory_image_of<float> const& hue,
                                  vil1_memory_image_of<float> const& sat
                                  );
  //: from the face vertices
  void centroid(double& x, double& y) const;

  //: copy of intf interface
  void reset() const { intf_->reset(); }
  bool next() const { return intf_->next(); }
  float Xo() const { return intf_->Xo(); }
  float Yo() const { return intf_->Yo(); }
  void set_X(float x) { intf_->set_X(x); }
  void set_Y(float y) { intf_->set_Y(y); }
  int Npix()const { return intf_->Npix(); }
  float X() const { return intf_->X(); }
  float Y() const { return intf_->Y(); }
  unsigned short I() const { return intf_->I(); }

  //: transformation: translate by (tx,ty), rotate by theta, zoom by scale
  void transform(double tx, double ty,
                 double theta, double scale);

 private:
  // local functions
  void set_gradient(vil1_memory_image_of<float> const& Ix,
                    vil1_memory_image_of<float> const& Iy);
  void set_color(vil1_memory_image_of<float> const& hue,
                 vil1_memory_image_of<float> const& sat);

  void init_intensity_info(vtol_face_2d_sptr const& face,
                           vil1_memory_image_of<float> const& image);

  void init_gradient_info(vil1_memory_image_of<float> const& Ix,
                          vil1_memory_image_of<float> const& Iy);
  
  void init_color_info(vil1_memory_image_of<float> const& hue,
                       vil1_memory_image_of<float> const& sat);
  
  float
    compute_intensity_mutual_information(vil1_memory_image_of<float> const& image);

  float
    compute_gradient_mutual_information(vil1_memory_image_of<float> const& Ix,
                                        vil1_memory_image_of<float> const& Iy);


  float
    compute_color_mutual_information(vil1_memory_image_of<float> const& hue,
                                     vil1_memory_image_of<float> const& sat);

  // members
  float min_gradient_;
  vtol_intensity_face_sptr intf_;
  bool gradient_info_;
  bool color_info_;
  float intensity_mi_;
  float gradient_dir_mi_;
  float color_mi_;
  float total_info_;
  float* Ix_;
  float* Iy_;
  float* hue_;
  float* sat_;
  unsigned int intensity_hist_bins_;
  unsigned int gradient_dir_hist_bins_;
  unsigned int color_hist_bins_;
  float model_intensity_entropy_;
  float model_gradient_dir_entropy_;
  float model_color_entropy_;
  float intensity_entropy_;
  float gradient_dir_entropy_;
  float color_entropy_;
  float intensity_joint_entropy_;
  float gradient_joint_entropy_;
  float color_joint_entropy_;
};

#endif // strk_tracking_face_2d_h_
