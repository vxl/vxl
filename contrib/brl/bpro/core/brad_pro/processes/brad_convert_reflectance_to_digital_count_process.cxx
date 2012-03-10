//This is brl/bpro/core/brad_pro/processes/brad_convert_reflectance_to_digital_count_process.cxx
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_image_atmospherics_est.h>

//:sets input and output types
bool brad_convert_reflectance_to_digital_count_process_cons(bprb_func_process& pro)
{
  //inputs
  //0: image with pixel values corresponding to reflectance values
  //1: image metadata
  //2: estimated atmospheric parameters
  //3: normalize values? If TRUE, output image will floating point with input[4] mapped to 1.0
  //4: maximum digital count value (default 2047)

  vcl_vector<vcl_string> input_types_(5);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "brad_image_metadata_sptr";
  input_types_[2] = "brad_atmospheric_parameters_sptr";
  input_types_[3] = "bool";
  input_types_[4] = "unsigned";

  if (!pro.set_input_types(input_types_))
    return false;

  //output: digital count of original image - normalized to (0,1) if input 3 is set to TRUE
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "vil_image_view_base_sptr";

  if (!pro.set_output_types(output_types_))
     return false;

  // set default normalization constant
  pro.set_input(4, new brdb_value_t<unsigned>(2047));

  return true;
}

bool brad_convert_reflectance_to_digital_count_process(bprb_func_process& pro)
{
  //check number of inputs
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << " Invalid inputs" << vcl_endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr reflectance_img_base = pro.get_input<vil_image_view_base_sptr>(0);
  brad_image_metadata_sptr mdata = pro.get_input<brad_image_metadata_sptr>(1);
  brad_atmospheric_parameters_sptr atm_params = pro.get_input<brad_atmospheric_parameters_sptr>(2);
  bool do_normalization = pro.get_input<bool>(3);
  unsigned int max_digital_count = pro.get_input<unsigned>(4);

  //check inputs validity
  if (!reflectance_img_base) {
    vcl_cout << pro.name() <<" :--  image  is null!\n";
    return false;
  }

  if (reflectance_img_base->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
     vcl_cerr << "ERROR: brad_convert_reflectance_to_digital_count: expecting floating point image\n";
     return false;
  }
  vil_image_view<float>* reflectance_img = dynamic_cast<vil_image_view<float>*>(reflectance_img_base.ptr());
  if (!reflectance_img) {
     vcl_cerr << "ERROR: brad_convert_reflectance_to_digital_count: error casting to float image\n";
     return false;
  }

  unsigned int ni = reflectance_img->ni();
  unsigned int nj = reflectance_img->nj();

  vil_image_view<float> toa_radiance_img(ni,nj);

  brad_undo_reflectance_estimate(*reflectance_img, *mdata, *atm_params, toa_radiance_img);

  vil_math_scale_and_offset_values(toa_radiance_img, (1.0/mdata->gain_), -(mdata->offset_/mdata->gain_) );

  vil_image_view_base_sptr output_img = 0;
  if (do_normalization) {
    vil_image_view<float> *output_img_float = new vil_image_view<float>(ni,nj);
    vil_convert_stretch_range_limited(toa_radiance_img,*output_img_float,0.0f,(float)max_digital_count,0.0f,1.0f);
    output_img = output_img_float;
  }
  else {
    vil_image_view<vxl_uint_16> *output_img_uint16 = new vil_image_view<vxl_uint_16>(ni,nj);
    vil_convert_stretch_range_limited(toa_radiance_img,*output_img_uint16, 0.0f, (float)max_digital_count,(vxl_uint_16)0, (vxl_uint_16)max_digital_count);
    output_img = output_img_uint16;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, output_img);

  return true;
}

