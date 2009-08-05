#ifndef boxm_opt_rt_sample_h_
#define boxm_opt_rt_sample_h_

#include <vcl_iostream.h>
#include <vil/vil_rgb.h>
#include <vsl/vsl_binary_io.h>
#include <boxm/opt/boxm_aux_traits.h>

template<class OBS_T>
class boxm_opt_rt_sample
{
public:
  boxm_opt_rt_sample() : obs_(0), pre_(0.0f), vis_(0.0f), PI_(0.0f), seg_len_(0.0f), Beta_(0.0f) {}
  ~boxm_opt_rt_sample(){}
  short version_no() const { return 1; }
  void print(vcl_ostream& os) const;

  OBS_T obs_;
  float pre_;
  float vis_;
  float PI_;
  float seg_len_;
  float Beta_;
};

//: traits for a grey optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>
{
public:
  typedef boxm_opt_rt_sample<float> sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_grey_work"; }

};

//: traits for an rgb optimization sample
template<>
class boxm_aux_traits<BOXM_AUX_OPT_RT_RGB>
{
public:
  typedef boxm_opt_rt_sample<vil_rgb<float> > sample_datatype;

  static vcl_string storage_subdir() { return "opt_rt_rgb_work"; }

};

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_opt_rt_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_opt_rt_sample<T> const * &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_opt_rt_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_opt_rt_sample<T> *&sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, const boxm_opt_rt_sample<T>& sample);

#endif

