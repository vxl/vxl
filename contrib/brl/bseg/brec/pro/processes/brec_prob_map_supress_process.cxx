// This is brl/bseg/brec/pro/processes/brec_prob_map_supress_process.cxx

//:
// \file
// \brief A process to suppress a given density/prob map with respect to another map, e.g. to remove vehicle areas from a change map
//
// Input map is P(X in B), suppressor is e.g. P(X in V) (x is a vehicles pixel)
// Output of this process is: P(X in B and X not in V)
//
// \author Ozge Can Ozcanli
// \date October 28, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_save.h>

//: Constructor
bool brec_prob_map_supress_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");      // input map (density or prob map (i.e. values in [0,1]) to be corrected with respect to suppressor map
  input_types.emplace_back("vil_image_view_base_sptr");      // suppressor map (another prob map)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");      // output map as suppressed input map
  output_types.emplace_back("vil_image_view_base_sptr");      // output byte image of suppressed map
  ok = pro.set_output_types(output_types);
  return ok;
}


bool brec_prob_map_supress_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << " brec_prob_map_roc_compute_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  vil_image_view_base_sptr inp = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view_base_sptr sup = pro.get_input<vil_image_view_base_sptr>(i++);

  if (sup->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view<float> input_map(inp);
  vil_image_view<float> sup_map(sup);

  unsigned ni = input_map.ni();
  unsigned nj = input_map.nj();

  vil_image_view<vxl_byte> dummy_byte(ni, nj);

  float min, max;
  vil_math_value_range(input_map, min, max);
  //std::cout << "\t input map float value range, min: " << min << " max: " << max << std::endl;
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
  //std::cout << "\t output map float value range after multiplication, min: " << min << " max: " << max << std::endl;

  //vil_convert_stretch_range_limited(output_map, dummy_byte, 0.0f, max);
  //vil_save(dummy_byte, "./output_map_mult.png");

  dummy.fill(max);
  vil_math_image_difference(dummy, output_map, dummy2);
  vil_convert_stretch_range_limited(dummy2, dummy_byte, 0.0f, max);
  //vil_save(dummy_byte, "./output_map_mult_inverse.png");

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(dummy2);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_map_sptr);

  vil_image_view_base_sptr out_map_sptr1 = new vil_image_view<vxl_byte>(dummy_byte);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_map_sptr1);

  return true;
}
