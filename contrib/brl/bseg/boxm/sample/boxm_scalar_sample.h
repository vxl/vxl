#ifndef boxm_scalar_sample_h_
#define boxm_scalar_sample_h_
//:
// \file

#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <boxm/boxm_apm_traits.h>


template <class T>
class boxm_scalar_sample
{
 public:
  boxm_scalar_sample() : scalar_sum_(0), seg_len_(0.0f) { }

  ~boxm_scalar_sample() {}
  T basic_val() { if (seg_len_>0) return scalar_sum_/seg_len_; else return (T)0; }
  static short version_no() { return 1; }
  void print(vcl_ostream& os) const;

  T scalar_sum_;
  float seg_len_;
};

//: traits for a scalar sample
template <>
class boxm_apm_traits<BOXM_SCALAR_FLOAT>
{
 public:
  typedef boxm_scalar_sample<float> sample_datatype;

  static vcl_string storage_subdir() { return "scalar_subdir"; }
};


template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scalar_sample<T> const &sample);

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scalar_sample<T> const* &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scalar_sample<T> &sample);

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scalar_sample<T>* &sample);

template <class T>
vcl_ostream& operator << (vcl_ostream& os, const boxm_scalar_sample<T>& sample);

#endif // boxm_scalar_sample_h_
