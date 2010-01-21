#ifndef boxm_sample_h_
#define boxm_sample_h_
//:
// \file
#include "boxm_apm_traits.h"
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <bsta/bsta_attributes.h>

template <boxm_apm_type APM_MODEL>
class boxm_sample
{
 public:
  static const boxm_apm_type apm_type = APM_MODEL;
  typedef typename boxm_apm_traits<APM_MODEL>::apm_datatype apm_datatype;
  typedef typename boxm_apm_traits<APM_MODEL>::obs_datatype obs_datatype;
  typedef typename boxm_apm_traits<APM_MODEL>::obs_mathtype obs_mathtype;
  typedef typename boxm_apm_traits<APM_MODEL>::apm_processor apm_processor;

  //: default constructor: alpha is set to 0.0015.
  boxm_sample() : alpha(0.0015f), appearance_() {}

  boxm_sample(float alpha_val): alpha(alpha_val), appearance_() {}
  //: constructor
  boxm_sample(float alpha_val, apm_datatype apm_val): alpha(alpha_val), appearance_(apm_val) {}

  virtual ~boxm_sample() {}

  static short version_no() { return 1; }

  //: bin is to imitate the bin number of multiple_bin case
  apm_datatype & appearance(int /*bin*/=-1) {return appearance_;}

  void set_appearance(apm_datatype  app,int /*bin*/=-1) { appearance_=app; }

  //: returns a basic value that represents this sample (mostly for raw file creation purposes)
  float basic_val() { return alpha; }

  //: the occlusion density at the sample point
  float alpha;

  //: the appearance model at the sample point
  typename boxm_apm_traits<APM_MODEL>::apm_datatype appearance_;

  void  print(vcl_ostream& os) const;
};

template <boxm_apm_type APM_MODEL>
void vsl_b_write(vsl_b_ostream & os, boxm_sample<APM_MODEL> const &sample);

template <boxm_apm_type APM_MODEL>
void vsl_b_write(vsl_b_ostream & os, boxm_sample<APM_MODEL> const * &sample);

template <boxm_apm_type APM_MODEL>
void vsl_b_read(vsl_b_istream & is, boxm_sample<APM_MODEL> &sample);

template <boxm_apm_type APM_MODEL>
void vsl_b_read(vsl_b_istream & is, boxm_sample<APM_MODEL> *&sample);

template <boxm_apm_type APM_MODEL>
vcl_ostream& operator << (vcl_ostream& os, const boxm_sample<APM_MODEL>& sample);

#endif
