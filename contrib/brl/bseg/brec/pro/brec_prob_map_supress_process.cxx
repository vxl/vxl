#include "brec_prob_map_supress_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_save.h>

brec_prob_map_supress_process::brec_prob_map_supress_process()
{
  //inputs
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0] = "vil_image_view_base_sptr";      // input map (density or prob map (i.e. values in [0,1]) to be corrected with respect to suppressor map
  input_types_[1] = "vil_image_view_base_sptr";      // suppressor map (another prob map)

  //output
  output_data_.resize(2,brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0]= "vil_image_view_base_sptr";      // output map as suppressed input map
  output_types_[1]= "vil_image_view_base_sptr";      // output byte image of suppressed map
}


bool brec_prob_map_supress_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  brdb_value_t<vil_image_view_base_sptr>* input0 = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr inp = input0->value();

  if (inp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  brdb_value_t<vil_image_view_base_sptr>* input1 = static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  vil_image_view_base_sptr sup = input1->value();

  if (sup->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view<float> input_map(inp);
  vil_image_view<float> sup_map(sup);

  unsigned ni = input_map.ni();
  unsigned nj = input_map.nj();

  vil_image_view<vxl_byte> dummy_byte(ni, nj);

  float min, max;
  vil_math_value_range(input_map, min, max);
  //vcl_cout << "\t input map float value range, min: " << min << " max: " << max << vcl_endl;
  //vil_convert_stretch_range_limited(input_map, dummy_byte, 0.0f, max);
  //vil_save(dummy_byte, "./input_map.png");

  vil_image_view<float> dummy(ni, nj), dummy2(ni, nj), dummy4(ni, nj), dummy3(ni, nj);
  dummy.fill(max);
  vil_math_image_difference(dummy, input_map, dummy2);
  //vil_convert_stretch_range_limited(dummy2, dummy_byte, 0.0f, max);
  //vil_save(dummy_byte, "./input_map_inverse.png");

  vil_math_value_range(sup_map, min, max);

  float min2, max2;
  vil_image_view<float> prod(ni, nj), prod_scaled(ni, nj);
  vil_math_image_product(dummy2, sup_map, prod);
  vil_math_value_range(prod, min2, max2);
  vil_convert_stretch_range_limited(prod, prod_scaled, 0.0f, max2, 0.0f, 1.0f);
  //vil_convert_stretch_range_limited(prod_scaled, dummy_byte, 0.0f, 1.0f);
  //vil_save(dummy_byte, "./inputs_prod_scaled.png");

  dummy3.fill(1.0f);
  //vil_math_image_difference(dummy3, sup_map, dummy4);
  vil_math_image_difference(dummy3, prod_scaled, dummy4);
  //vil_convert_stretch_range_limited(dummy4, dummy_byte, 0.0f, 1.0f);
  //vil_save(dummy_byte, "./sup_map_inverse.png");

  vil_image_view<float> output_map(ni, nj);
  vil_math_image_product(dummy2, dummy4, output_map);
  //vil_math_value_range(output_map, min, max);
  //vcl_cout << "\t output map float value range after multiplication, min: " << min << " max: " << max << vcl_endl;

  //vil_convert_stretch_range_limited(output_map, dummy_byte, 0.0f, max);
  //vil_save(dummy_byte, "./output_map_mult.png");

  dummy.fill(max);
  vil_math_image_difference(dummy, output_map, dummy2);
  vil_convert_stretch_range_limited(dummy2, dummy_byte, 0.0f, max);
  //vil_save(dummy_byte, "./output_map_mult_inverse.png");

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(dummy2);
  brdb_value_sptr output = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr);
  output_data_[0] = output;

  vil_image_view_base_sptr out_map_sptr1 = new vil_image_view<vxl_byte>(dummy_byte);
  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(out_map_sptr1);
  output_data_[1] = output1;

  return true;
}

