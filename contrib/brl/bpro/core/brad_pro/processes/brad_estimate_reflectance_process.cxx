//This is brl/bpro/core/brad_pro/processes/brad_estimate_reflectance_process.cxx
//:
// \file
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>

//:sets input and output types
bool brad_estimate_reflectance_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: The normalized image (pixel values are band-averaged radiance with units W m^-2 sr^-1 um-1
  //1: image metadata
  //2: mean scene reflectance
  //3: (optional) compute average airlight from all visible bands (boollean).
  //4: (optional) normalize the returned reflectance by average radiance.

  std::vector<std::string> input_types_(5);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "brad_image_metadata_sptr";
  input_types_[2] = "float";
  input_types_[3] = "bool";
  input_types_[4] = "bool";

  if (!pro.set_input_types(input_types_))
    return false;

  //output: predicted reflectance values:
  std::vector<std::string> output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool brad_estimate_reflectance_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs" << std::endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr radiance_img_base = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(1);
  auto mean_reflectance = pro.get_input<float>(2);
  bool average_airlight = pro.get_input<bool>(3);
  bool is_normalize = pro.get_input<bool>(4);

  //check inputs validity
  if (!radiance_img_base) {
    std::cout << pro.name() <<" :--  image  is null!\n";
    return false;
  }

  if (radiance_img_base->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
     std::cerr << "ERROR: brad_estimate_reflectance: expecting floating point image\n";
     return false;
  }
  auto* radiance_img = dynamic_cast<vil_image_view<float>*>(radiance_img_base.ptr());
  if (!radiance_img) {
     std::cerr << "ERROR: brad_estimate_reflectance: error casting to float image\n";
     return false;
  }

  unsigned int ni = radiance_img->ni();
  unsigned int nj = radiance_img->nj();
  unsigned int np = radiance_img->nplanes();
  if (mean_reflectance <= 0.0)
    is_normalize = false;

  auto *reflectance_img = new vil_image_view<float>(ni, nj, np);
  bool success = brad_estimate_reflectance_image(*radiance_img, *mdata, mean_reflectance, *reflectance_img, average_airlight, is_normalize);

  if (!success)
    return false;
  vil_image_view_base_sptr output_img(reflectance_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, output_img);
  return true;
}
