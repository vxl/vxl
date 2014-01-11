//This is brl/bpro/core/brad_pro/processes/brad_estimate_reflectance_process.cxx
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
bool brad_estimate_reflectance_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: The normalized image (pixel values are band-averaged radiance with units W m^-2 sr^-1 um-1
  //1: image metadata
  //2: estimated atmospheric parameters

  vcl_vector<vcl_string> input_types_(3);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "brad_image_metadata_sptr";
  input_types_[2] = "brad_atmospheric_parameters_sptr";

  if (!pro.set_input_types(input_types_))
    return false;

  //output: predicted reflectance values:
  vcl_vector<vcl_string> output_types_(1);
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
    vcl_cout << pro.name() << " Invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr radiance_img_base = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(1);
  brad_atmospheric_parameters_sptr atm_params = pro.get_input<brad_atmospheric_parameters_sptr>(2);

  //check inputs validity
  if (!radiance_img_base) {
    vcl_cout << pro.name() <<" :--  image  is null!\n";
    return false;
  }

  if (radiance_img_base->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
     vcl_cerr << "ERROR: brad_estimate_reflectance: expecting floating point image\n";
     return false;
  }
  vil_image_view<float>* radiance_img = dynamic_cast<vil_image_view<float>*>(radiance_img_base.ptr());
  if (!radiance_img) {
     vcl_cerr << "ERROR: brad_estimate_reflectance: error casting to float image\n";
     return false;
  }

  unsigned int ni = radiance_img->ni();
  unsigned int nj = radiance_img->nj();

  if (radiance_img->nplanes() == 1) {
    vil_image_view<float> *reflectance_img = new vil_image_view<float>(ni,nj);
    brad_estimate_reflectance_image(*radiance_img, *mdata, *atm_params, *reflectance_img);
    vil_image_view_base_sptr output_img(reflectance_img);
    pro.set_output_val<vil_image_view_base_sptr>(0, output_img);
  } else {
    vil_image_view<float> *reflectance_img = new vil_image_view<float>(ni,nj,radiance_img->nplanes());
    brad_estimate_reflectance_image_multi(*radiance_img, *mdata, *atm_params, *reflectance_img);
    vil_image_view_base_sptr output_img(reflectance_img);
    pro.set_output_val<vil_image_view_base_sptr>(0, output_img);
  }

  

  return true;
}

