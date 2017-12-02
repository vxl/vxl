#include "boxm_apm_traits.h"

const char* boxm_apm_types::app_model_strings[]  = { "apm_mog_grey",
                                                     "apm_mog_rgb",
                                                     "apm_simple_grey",
                                                     "apm_simple_rgb",
                                                     "apm_mob_grey",
                                                     "float",
                                                     "int",
                                                     "short",
                                                     "char",
                                                     "bool",
                                                     "vnl_double_10",
                                                     "vnl_float_10",
                                                     "vnl_float_3",
                                                     "bsta_mog_f1",
                                                     "bsta_gauss_f1",
                                                     "bvpl_sample_float",
                                                     "bvpl_sample_gauss_f1",
                                                     "bvpl_pca_basis_sample_10",
                                                     "edge_float",
                                                     "edge_line",
                                                     "edge_tangent_line",
                                                     "scalar_float",
                                                     "dbrec3d_part_instance",
                                                     "dbrec3d_pair_composite_instance",
                                                     "apm_na",
                                                     "apm_unknown" };


boxm_apm_type boxm_apm_types::str_to_enum(const char* s)
{
  for (int i=0; i < int(BOXM_APM_UNKNOWN); i++)
  {
    if (std::strcmp(s, boxm_apm_types::app_model_strings[i]) == 0)
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
    std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_sample<T>&)\n"
             << "           Unknown version number "<< version << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    break;
  }
}

void vsl_b_read(vsl_b_istream & is, boxm_simple_grey *&sample)
{
  vsl_b_read(is, *sample);
}

std::ostream& operator<<(std::ostream &os, boxm_simple_grey const& apm)
{
  os << "color: " << apm.color() << ", one_over_sigma: " << apm.one_over_sigma() << std::endl;
  return os;
}
