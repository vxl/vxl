//This is brl/bpro/core/brad_pro/processes/brad_estimate_atmospheric_parameters_process.cxx
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>

//:sets input and output types
bool brad_estimate_atmospheric_parameters_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: The normalized image (pixel values are band-averaged radiance with units W m^-2 sr^-1 um-1
  //1: image metadata
  //2: (optional) mean scene reflectance
  //3: (optional) constrain atmospheric parameters to physically meaningful values? (boolean)

  vcl_vector<vcl_string> input_types_(4);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "brad_image_metadata_sptr";
  input_types_[2] = "float";
  input_types_[3] = "bool";

  if (!pro.set_input_types(input_types_))
    return false;

  //output: estimated atmospheric parameters
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "brad_atmospheric_parameters_sptr";

  if (!pro.set_output_types(output_types_))
     return false;

  // set default value for mean radiance
  pro.set_input(2, new brdb_value_t<float>(0.0f));
  // set default value for meaningful atmospheric parameters option
  pro.set_input(3, new brdb_value_t<bool>(true));

  return true;
}

bool brad_estimate_atmospheric_parameters_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " Invalid inputs" << vcl_endl;
    return false;
  }
  //get the inputs
  vil_image_view_base_sptr radiance_img_base = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(1);
  float mean_reflectance = pro.get_input<float>(2);
  bool constrain_atmospheric_params = pro.get_input<bool>(3);

  //check inputs validity
  if (!radiance_img_base) {
    vcl_cout << pro.name() <<" :--  image  is null!\n";
    return false;
  }

  if (radiance_img_base->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
     vcl_cerr << "ERROR: brad_estimate_atmospheric_parameters: expecting floating point image\n";
     return false;
  }
  vil_image_view<float>* radiance_img = dynamic_cast<vil_image_view<float>*>(radiance_img_base.ptr());
  if (!radiance_img) {
     vcl_cerr << "ERROR: brad_estimate_atmospheric_parameters: error casting to float image\n";
     return false;
  }
  brad_atmospheric_parameters_sptr atm_params = new brad_atmospheric_parameters();

  bool succeed;
  if (mean_reflectance > 0) {
    if (radiance_img->nplanes() == 1)
      succeed = brad_estimate_atmospheric_parameters(*radiance_img, *mdata, mean_reflectance, *atm_params, constrain_atmospheric_params);
    else
      succeed = brad_estimate_atmospheric_parameters_multi(*radiance_img, *mdata, mean_reflectance, *atm_params, constrain_atmospheric_params);
  }
  else {
    if (radiance_img->nplanes() == 1)
     succeed = brad_estimate_atmospheric_parameters(*radiance_img, *mdata, *atm_params);
    else
     succeed = brad_estimate_atmospheric_parameters_multi(*radiance_img, *mdata, *atm_params);
  }
  if (succeed) {
    pro.set_output_val<brad_atmospheric_parameters_sptr>(0, atm_params);
    return true;
  }
  else
    return false;
  
}

