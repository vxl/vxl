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
// \author
//    Joseph L. Mundy - October 29, 2003
//    Brown University
//
// \verbatim
//  Modifications
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
//   15-june-2005 Ozge Can Ozcanli Added methods to calculate mutual information 
//                                 with known pixel correspondences of two faces 
//   13-july-2005 Ozge Can Ozcanli Added max_intensity_ variable to support images 
//                                 with larger range than 8 bits
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vil1/vil1_memory_image_of.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vtol/vtol_intensity_face.h>
#include <strk/strk_tracking_face_2d_sptr.h>
#include <bsta/bsta_histogram.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_utility.h>
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
                        const float min_gradient,
                        const float parzen_sigma,
                        const unsigned int intensity_hist_bins,
                        const unsigned int gradient_dir_hist_bins,
                        const unsigned int color_hist_bins,
                        const float max_intensity = 255.0f
                        );

  strk_tracking_face_2d(vtol_intensity_face_sptr const& intf,
                        const unsigned int intensity_hist_bins,
                        const unsigned int gradient_dir_hist_bins,
                        const unsigned int color_hist_bins,
                        const float max_intensity = 255.0f
                        );
  strk_tracking_face_2d(strk_tracking_face_2d_sptr const& tf);
  strk_tracking_face_2d(strk_tracking_face_2d const& tf);
  ~strk_tracking_face_2d();
  //:accessors
  vtol_intensity_face_sptr face() const { return intf_; }
  bool gradient_needed() const { return gradient_info_; }
  bool color_needed() const { return color_info_; }
  float Ix(int i) const { return Ix_[i]; }
  float Iy(int i) const { return Iy_[i]; }
  float hue(int i) const { return hue_[i]; }
  float sat(int i) const { return sat_[i]; }
  float int_mutual_info() const { return intensity_mi_; }
  float grad_mutual_info() const { return gradient_dir_mi_; }
  float color_mutual_info() const { return color_mi_; }
  float total_info() const { return total_info_; }
  vnl_matrix_fixed<double, 3, 3>& trans(){return trans_;}
  //:mutators
  void set_min_gradient(const float min_gradient){min_gradient_=min_gradient;}
  void set_parzen_sigma(const float parzen_sigma){parzen_sigma_=parzen_sigma;}
  void set_Ix(int i, float Ix) { Ix_[i] = Ix; }
  void set_Iy(int i, float Iy) { Iy_[i] = Iy; }
  void set_hue(int i, float Ix) { hue_[i] = Ix; }
  void set_sat(int i, float Iy) { sat_[i] = Iy; }
  void set_int_mutual_info(float mi);
  void set_grad_mutual_info(float mi);
  void set_color_mutual_info(float mi);
  void set_max_intensity(const float max_intensity){max_intensity_=max_intensity;}
  //:controls
  void set_renyi_joint_entropy(){renyi_joint_entropy_=true;}
  void unset_renyi_joint_entropy(){renyi_joint_entropy_=false;}
  //:histogram properties
  void set_intensity_hist_bins(const unsigned int n_bins)
    {intensity_hist_bins_=n_bins;}
  void set_gradient_dir_bins(const unsigned int n_bins)
    {gradient_dir_hist_bins_ = n_bins;}
  void set_color_dir_bins(const unsigned int n_bins)
    {color_hist_bins_ = n_bins;}
  unsigned int intensity_hist_bins() const { return intensity_hist_bins_;}
  unsigned int gradient_dir_hist_bins() const {return gradient_dir_hist_bins_;}
  unsigned int color_hist_bins() const { return color_hist_bins_; }

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

  float intensity_info_diff(){return intensity_info_diff_;}
  float color_info_diff(){return color_info_diff_;}
  float total_info_diff();
  //: utilities
  bool compute_mutual_information(vil1_memory_image_of<float> const& image,
                                  vil1_memory_image_of<float> const& Ix,
                                  vil1_memory_image_of<float> const& Iy,
                                  vil1_memory_image_of<float> const& hue,
                                  vil1_memory_image_of<float> const& sat);
  
  bool compute_mutual_information(vcl_vector <vcl_vector< vgl_point_2d<int> > > region_map,
                                  int base_x, int base_y,
                                  vil1_memory_image_of<float> const& image,
                                  vil1_memory_image_of<float> const& Ix,
                                  vil1_memory_image_of<float> const& Iy,
                                  vil1_memory_image_of<float> const& hue,
                                  vil1_memory_image_of<float> const& sat);

  bool compute_only_gradient_mi(vil1_memory_image_of<float> const& Ix,
                                vil1_memory_image_of<float> const& Iy);
                                
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
  void transform(double tx, double ty, double theta, double scale);

  void transform(vnl_matrix_fixed<double,3,3> const& T);

  //: utilities involving another tracking face (background)

  //: select a random set of intensities from the interior
  vcl_vector<float> random_intensities(int& n_pix);

  //: select a random set of colors from the interior
  bool random_colors(int& n_pix,
                     vcl_vector<float>& hue, vcl_vector<float>& sat);

  //:an alternative model for explaining current intensity values inside *this
  float intensity_mutual_info_diff(strk_tracking_face_2d_sptr const& other,
                                   vil1_memory_image_of<float> const& image,
                                   bool verbose = false);


  float color_mutual_info_diff(strk_tracking_face_2d_sptr const& other,
                               vil1_memory_image_of<float> const& hue,
                               vil1_memory_image_of<float> const& sat,
                               bool verbose = false);

  //: a background model consisting of cloned faces surrounding *this face.
  float intensity_mutual_info_diff(vcl_vector<strk_tracking_face_2d_sptr> const& others,
                                   vil1_memory_image_of<float> const& image,
                                   bool verbose = false);

  float color_mutual_info_diff(vcl_vector<strk_tracking_face_2d_sptr> const& others,
                               vil1_memory_image_of<float> const& hue,
                               vil1_memory_image_of<float> const& sat,
                               bool verbose = false);


  //: for debugging
  void print_pixels(vil1_memory_image_of<float> const& image);

  void face_points(vcl_vector<vtol_topology_object_sptr>& points);

  void print_intensity_histograms(vil1_memory_image_of<float> const& image);
  void print_gradient_histograms(vil1_memory_image_of<float> const& Ix,
                                  vil1_memory_image_of<float> const& Iy);
  void print_color_histograms(vil1_memory_image_of<float> const& hue,
                              vil1_memory_image_of<float> const& sat);
  bsta_histogram<float> 
    intensity_histogram(vil1_memory_image_of<float> const& image);  
  bsta_histogram<float> 
    gradient_histogram(vil1_memory_image_of<float> const& Ix,
                       vil1_memory_image_of<float> const& Iy);  
  bsta_histogram<float> 
    color_histogram(vil1_memory_image_of<float> const& hue,
                    vil1_memory_image_of<float> const& sat);  

 private:
  // local functions
  void init_bins();

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
    compute_intensity_mutual_information(vcl_vector <vcl_vector< vgl_point_2d<int> > > region_map,
                                         int base_x, int base_y,
                                         vil1_memory_image_of<float> const& image);

  float
    compute_gradient_mutual_information(vil1_memory_image_of<float> const& Ix,
                                        vil1_memory_image_of<float> const& Iy);

  float
    compute_gradient_mutual_information(vcl_vector <vcl_vector< vgl_point_2d<int> > > region_map,
                                        int base_x, int base_y,
                                        vil1_memory_image_of<float> const& Ix,
                                        vil1_memory_image_of<float> const& Iy);

  float
    compute_color_mutual_information(vil1_memory_image_of<float> const& hue,
                                     vil1_memory_image_of<float> const& sat);

  float
    compute_color_mutual_information(vcl_vector <vcl_vector< vgl_point_2d<int> > > region_map,
                                     int base_x, int base_y,
                                     vil1_memory_image_of<float> const& hue,
                                     vil1_memory_image_of<float> const& sat);

  float
    compute_intensity_joint_entropy(strk_tracking_face_2d_sptr const& other,
                                    vil1_memory_image_of<float> const& image);
float
  compute_model_intensity_joint_entropy(strk_tracking_face_2d_sptr const& other);

 float compute_color_joint_entropy(strk_tracking_face_2d_sptr const& other,
                                    vil1_memory_image_of<float> const& hue,
                                    vil1_memory_image_of<float> const& sat);

  //: helper function for transform(): transformation: rotate gradients by theta
  void transform_gradients(double theta);

  // members
  float min_gradient_;
  float parzen_sigma_;
  vtol_intensity_face_sptr intf_;
  bool gradient_info_;
  bool color_info_;
  bool renyi_joint_entropy_;
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
  float model_intensity_joint_entropy_;
  float gradient_joint_entropy_;
  float color_joint_entropy_;
  float intensity_info_diff_;
  float color_info_diff_;
  vnl_matrix_fixed<double, 3, 3> trans_;
  float max_intensity_;
};

#endif // strk_tracking_face_2d_h_
