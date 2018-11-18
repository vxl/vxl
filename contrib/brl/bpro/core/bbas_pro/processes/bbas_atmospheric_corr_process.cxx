//This is brl/bpro/core/bbas_pro/processes/bbas_atmospheric_corr_process.cxx
//:
// \file
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bsta/bsta_histogram.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#ifdef DEBUG
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#endif

#include <brdb/brdb_value.h>
#include <brip/brip_vil_float_ops.h>

//:sets input and output types
bool bbas_atmospheric_corr_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: The unnormalized image
  //1: sz the z component of the illumination direction

  std::vector<std::string> input_types_(2);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "float";

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(1);
  output_types_[0]= "vil_image_view_base_sptr"; // corrected image
  return pro.set_output_types(output_types_);
}

bool bbas_atmospheric_corr_process(bprb_func_process& pro)
{

 //check number of inputs
  if(!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs " << std::endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr input_img =
    pro.get_input<vil_image_view_base_sptr>(0);

  auto sz = pro.get_input<float>(1);

  //check inputs validity
  if (!input_img) {
    std::cout << pro.name() <<" :--  image  is null!\n";
    return false;
  }
  unsigned ni_ = input_img->ni();
  unsigned nj_ = input_img->nj();
  unsigned nplanes_ = input_img->nplanes();
  if(nplanes_!=1){
    std::cout << pro.name() <<" :--  image  is not grey scale!\n";
    return false;
  }
  if(sz<0.1) {
    std::cout << pro.name() <<" :--  sun illumination angle too low\n";
    return false;
  }
  vil_image_view<vxl_byte> byte_img = *vil_convert_cast(vxl_byte(), input_img);

  // compute histogram for image
  bsta_histogram<float> h(0.0f, 255.0f, 256);
  for(unsigned j = 0; j<nj_; ++j)
    for(unsigned i = 0; i<ni_; ++i)
      h.upcount(byte_img(i, j), 1.0f);

  // compute airlight and scene irradiance
  float frac = 0.0001f;
  float airlight = h.value_with_area_below(frac);
  float mean = h.mean_vals(airlight, h.max());
//#def BBAS_NORM_USE_SUN_ANGLE
#ifdef BBAS_NORM_USE_SUN_ANGLE
  float irrad = (mean-airlight)/sz;
  //
  // The corrected image should be explained by a Lambertian model,
  // that is, Icorr = alpha_i (n_i . s_j)
  //
#else
  float irrad = (mean-airlight);
#endif
  std::cout << "airlight " << airlight << "  irradiance " << irrad << '\n';

  vil_image_view<float> float_img(ni_, nj_);
  float min = 1.0e8f, max = -1.0e8f;
  for(unsigned j = 0; j<nj_; ++j)
    for(unsigned i = 0; i<ni_; ++i){
      auto v = static_cast<float>((byte_img(i,j)-airlight)/irrad);
      if(v<min) min = v;
      if(v>max) max = v;
      float_img(i,j) = v;
    }

#ifdef BBAS_NORM_USE_SUN_ANGLE
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(float_img));
#else
  // 0 and 3 come from the relative refectances
  vil_image_view<float> float_stretch(ni_, nj_);
  vil_convert_stretch_range_limited<float>(float_img, float_stretch,
                                           0.0f, 3.0f, 0.0f, 255.0f);
  auto* byte_output_img =
    new vil_image_view<vxl_byte>(ni_, nj_, nplanes_);
  vil_convert_cast(float_stretch, *byte_output_img);

  pro.set_output_val<vil_image_view_base_sptr>(0, byte_output_img);
#endif
#if 0
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(float_img));
#endif
  return true;
}
