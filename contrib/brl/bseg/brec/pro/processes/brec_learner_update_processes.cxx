// This is brl/bseg/brec/pro/processes/brec_learner_update_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to update stats of learner instances from training imgs
//
// \author Ozge Can Ozcanli
// \date Jan 22, 2009
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <brec/brec_part_hierarchy_learner_sptr.h>
#include <brec/brec_part_hierarchy_learner.h>

#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//: Constructor
bool brec_learner_layer0_update_stats_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_learner_sptr");      // learner instance
  input_types.emplace_back("vil_image_view_base_sptr");      // a training image, byte image
  input_types.emplace_back("vil_image_view_base_sptr");      // a foreground probability img for the input training image, float img with values in [0,1] range
                                                     // this is the foreground ground-truth mask if available
  input_types.emplace_back("vil_image_view_base_sptr");      // a bool mask img to designate valid regions in the input image, stats will be collected from the pixels with mask == true
                                                     // if this pointer is passed as zero, a mask filled with "true" is used (all image is used)
  input_types.emplace_back("vcl_string");      // output file to save histograms in matlab .m file format
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

//: don't set the third input if it needs to be initialized as a mask filled with "true", just call the init method from Python before running the process
bool brec_learner_layer0_update_stats_process_init(bprb_func_process& pro)
{
  // initialize a dummy mask
  vil_image_view<bool> dummy(10, 10);
  dummy.fill(true);
  vil_image_view_base_sptr m_ptr = new vil_image_view<bool>(dummy);
  pro.set_input(3, new brdb_value_t<vil_image_view_base_sptr>(m_ptr));
  return true;
}

bool brec_learner_layer0_update_stats_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 5) {
    std::cerr << "  brec_learner_layer0_update_stats_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_learner_sptr hl = pro.get_input<brec_part_hierarchy_learner_sptr>(i++);

  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  if (inp_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  vil_image_view_base_sptr inp_prob = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> prob = *vil_convert_cast(float(), inp_prob);
  if (inp_prob->pixel_format() == VIL_PIXEL_FORMAT_BOOL) {
    // just a check to make sure conversion went well
    float min, max;
    vil_math_value_range(prob, min, max);
    std::cout << " input prob map was a byte image, after conversion min value: " << min << " max value: " << max << std::endl;
  } else if (inp_prob->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    std::cout << "In brec_learner_update_stats_process::execute() -- ERROR: input prob image is neither BOOL img nor FLOAT img!!\n";
    return false;
  }

  vil_image_view_base_sptr inp_mask = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<bool> mask_img(img.ni(), img.nj());
  if (!inp_mask || inp_mask->ni() != img.ni()) {  // if mask is passed as zero make a bool img with all pixels true, i.e. use the whole input img
    mask_img.fill(true);
  } else {
    if (inp_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
      std::cout << "In brec_learner_update_stats_process::execute() -- ERROR: input mask image is not of type BOOL!!\n";
      return false;
    }
    mask_img = inp_mask;
  }

  std::string output_file = pro.get_input<std::string>(i++);

  hl->layer0_collect_stats(img, prob, mask_img);
  hl->print_to_m_file_layer0(output_file);

  return true;
}

//: Constructor
bool brec_learner_layer0_update_posterior_stats_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_learner_sptr");  // learner instance, assumes the foreground response distribution's parameters have already been fit
  input_types.emplace_back("vil_image_view_base_sptr");      // a training image, byte image
  input_types.emplace_back("vil_image_view_base_sptr");      // a foreground probability img for the input training image, float img with values in [0,1] range
                                                     // this is the foreground ground-truth mask if available
  input_types.emplace_back("vil_image_view_base_sptr");      // a bool mask img to designate valid regions in the input image, stats will be collected from the pixels with mask == true
                                                     // if this pointer is passed as zero, a mask filled with "true" is used (all image is used)

  input_types.emplace_back("vil_image_view_base_sptr");      // bg model mean img to construct response models, float img with values in [0,1] range
  input_types.emplace_back("vil_image_view_base_sptr");      // bg model std dev img to construct response models, float img with values in [0,1] range
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_learner_layer0_update_posterior_stats_process_init(bprb_func_process& pro)
{
  // initialize a dummy mask
  vil_image_view<bool> dummy(10, 10);
  dummy.fill(true);
  vil_image_view_base_sptr m_ptr = new vil_image_view<bool>(dummy);
  pro.set_input(3, new brdb_value_t<vil_image_view_base_sptr>(m_ptr));
  return true;
}

bool brec_learner_layer0_update_posterior_stats_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 6) {
    std::cerr << "  brec_learner_layer0_update_stats_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_learner_sptr hl = pro.get_input<brec_part_hierarchy_learner_sptr>(i++);

  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  if (inp_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  vil_image_view_base_sptr inp_prob = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<float> prob = *vil_convert_cast(float(), inp_prob);
  if (inp_prob->pixel_format() == VIL_PIXEL_FORMAT_BOOL) {
    // just a check to make sure conversion went well
    float min, max;
    vil_math_value_range(prob, min, max);
    std::cout << " input prob map was a byte image, after conversion min value: " << min << " max value: " << max << std::endl;
  } else if (inp_prob->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    std::cout << "In brec_learner_update_posterior_stats_process::execute() -- ERROR: input prob image is neither BOOL img nor FLOAT img!!\n";
    return false;
  }

  vil_image_view_base_sptr inp_mask = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<bool> mask_img(img.ni(), img.nj());
  if (!inp_mask || inp_mask->ni() != img.ni()) {  // if mask is passed as zero make a bool img with all pixels true, i.e. use the whole input img
    mask_img.fill(true);
  } else {
    if (inp_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
      std::cout << "In brec_learner_update_posterior_stats_process::execute() -- ERROR: input mask image is not of type BOOL!!\n";
      return false;
    }
    mask_img = inp_mask;
  }

  vil_image_view_base_sptr inp_mean = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_mean->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view_base_sptr inp_std_dev = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_std_dev->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view<float> mean_img(inp_mean);
  vil_image_view<float> std_dev_img(inp_std_dev);

  hl->layer0_collect_posterior_stats(img, prob, mask_img, mean_img, std_dev_img);

  return true;
}

//: Constructor
bool brec_learner_layer_n_update_stats_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_learner_sptr");      // learner instance
  input_types.emplace_back("brec_part_hierarchy_detector_sptr");      // a detector instance
  input_types.emplace_back("unsigned");      // n: layer id
  input_types.emplace_back("unsigned");      // k: class id
  input_types.emplace_back("vcl_string");      // output file to save histograms in matlab .m file format
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_learner_layer_n_update_stats_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4){
    std::cerr << "  brec_learner_layer_n_update_stats_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  brec_part_hierarchy_learner_sptr hl = pro.get_input<brec_part_hierarchy_learner_sptr>(i++);
  brec_part_hierarchy_detector_sptr hd = pro.get_input<brec_part_hierarchy_detector_sptr>(i++);
  auto n = pro.get_input<unsigned>(i++);
  auto k = pro.get_input<unsigned>(i++);
  std::string output_file = pro.get_input<std::string>(i++);

  hl->layer_n_collect_stats(hd, n, k);
  hl->print_to_m_file_layer_n(output_file, k, false);

  return true;
}
