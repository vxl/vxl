#ifndef boxm_edge_sample_h_
#define boxm_edge_sample_h_
//:
// \file

#include <vcl_iostream.h>
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <boxm/opt/boxm_aux_traits.h>

template<class OBS_T>
class boxm_edge_sample
{
 public:
  boxm_edge_sample(): edge_prob_(0), num_obs_(0.0f){  }
  ~boxm_edge_sample() {}
  static short version_no() { return 1; }
  void print(vcl_ostream& os) const;

  //: returns a basic value that represents this sample (mostly for raw file creation purposes)
  float basic_val() { return edge_prob_; }
  OBS_T edge_prob_;
  float num_obs_;
};

template<class OBS_T>
class boxm_aux_edge_sample
{
 public:
  boxm_aux_edge_sample(): obs_(0), seg_len_(0.0f) { }
  ~boxm_aux_edge_sample() {}
  static short version_no() { return 1; }
  void print(vcl_ostream& os) const;

  OBS_T obs_;
  float seg_len_;
};

#if 0 // two classes commented out

//: traits for a grey optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>
{
 public:
  typedef boxm_edge_sample<float> sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_grey_work"; }
};

//: traits for an rgb optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_RGB>
{
 public:
  typedef boxm_edge_sample<vil_rgb<float> > sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_rgb_work"; }
};

#endif // 0

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_edge_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_edge_sample<T> *&sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, const boxm_edge_sample<T>& sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_aux_edge_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_aux_edge_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_aux_edge_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_aux_edge_sample<T> *&sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, const boxm_aux_edge_sample<T>& sample);


#endif // boxm_edge_sample_h_
