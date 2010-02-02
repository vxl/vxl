#include "boxm_apm_traits.h"

const char* boxm_apm_types::app_model_strings[]  = { "apm_mog_grey",
                                                      "apm_mog_rgb", 
                                                      "apm_simple_grey",
                                                      "apm_simple_rgb", 
                                                      "apm_mob_grey",
                                                      "float",
                                                      "bsta_mog_f1",
                                                      "bsta_gauss_f1",
                                                      "bvpl_sample_float",
                                                      "bvpl_sample_gauss_f1",
                                                      "edge_float",
                                                      "apm_na"};


boxm_apm_type boxm_apm_types::str_to_enum(const char* s)
{
  for (int i=0; i < int(BOXM_APM_UNKNOWN); i++)
  {
    if (vcl_strcmp(s, boxm_apm_types::app_model_strings[i]) == 0)
      return (boxm_apm_type) i;
  }
  return BOXM_APM_UNKNOWN;
}

void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const &sample)
{
  vsl_b_write(os, sample.version_no());
  vsl_b_write(os, sample.color_);
  vsl_b_write(os, sample.one_over_sigma_);
  //vsl_b_write(os, sample.gauss_weight_);
}

void vsl_b_write(vsl_b_ostream & os, boxm_simple_grey const * &sample)
{
  if (sample) {
    vsl_b_write(os, *sample);
  }
}

void vsl_b_read(vsl_b_istream & is, boxm_simple_grey &sample)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      vsl_b_read(is, sample.color_);
      vsl_b_read(is, sample.one_over_sigma_);
      //vsl_b_read(is, sample.gauss_weight_);
      
      break;
    default:
      vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_sample<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      break;
  }
}

void vsl_b_read(vsl_b_istream & is, boxm_simple_grey *&sample)
{
  vsl_b_read(is, *sample);
}

vcl_ostream& operator<<(vcl_ostream &os, boxm_simple_grey const& apm) 
{ 
  os << "color: " << apm.color() << ", one_over_sigma: " << apm.one_over_sigma() << vcl_endl;
  return os;
}
