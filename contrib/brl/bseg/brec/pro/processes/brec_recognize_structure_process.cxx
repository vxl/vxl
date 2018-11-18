// This is brl/bseg/brec/pro/processes/brec_recognize_structure_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to find instances of various structures, objects (e.g. vehicles) according to a part hierarchy constructed a priori
//
// \author Ozge Can Ozcanli
// \date Oct 28, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 3, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vul/vul_timer.h>

#include <brec/brec_part_hierarchy_builder.h>
#include <brec/brec_part_hierarchy_detector_sptr.h>
#include <brec/brec_part_hierarchy_detector.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_gaussian_sptr.h>
#include <brec/brec_part_gaussian.h>

//: Constructor
bool brec_recognize_structure_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");      // input orig view
  input_types.emplace_back("unsigned");      // detector id for the type of structure to be recognized
  input_types.emplace_back("float");      // angle to rotate detector for the type of structure to be recognized
                                  // should be passed zero if the original orientation of the detector will be used
  if (pro.set_input_types(input_types))
  { //output
    std::vector<std::string> output_types;
    output_types.emplace_back("vil_image_view_base_sptr"); // output prob map
    output_types.emplace_back("vil_image_view_base_sptr"); // output map overlayed on orig view as a byte image
    return pro.set_output_types(output_types);
  }
  else
    return false;
}


bool brec_recognize_structure_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cerr << " brec_recognize_structure_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr orig_view = pro.get_input<vil_image_view_base_sptr>(i++);
  if (orig_view->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;

  vil_image_view<vxl_byte> orig_img(orig_view);
  unsigned ni = orig_img.ni();
  unsigned nj = orig_img.nj();
  vil_image_resource_sptr img = vil_new_image_resource_of_view(orig_img);

  vul_timer t2;
  t2.mark();

  auto d_id = pro.get_input<unsigned>(i++);
  auto angle = pro.get_input<float>(i++);

  brec_part_hierarchy_sptr h;
  switch (d_id) {
    case 0: { h = brec_part_hierarchy_builder::construct_detector_roi1_0(); } break;
    case 1: { h = brec_part_hierarchy_builder::construct_detector_roi1_1(); } break;
    case 2: { h = brec_part_hierarchy_builder::construct_detector_roi1_2(); } break;
    case 3: { h = brec_part_hierarchy_builder::construct_detector_roi1_3(); } break;
    case 4: { h = brec_part_hierarchy_builder::construct_detector_roi1_4(); } break;
    default: { std::cout << "In brec_recognize_structure_process::execute() -- Unrecognized detector type!!\n"; return false; }
  }

#if 0 // before detector class
  // now extract instances of primitive part types in h
  std::vector<bvxm_part_instance_sptr> parts_0;
  std::vector<bvxm_part_instance_sptr>& d_ins = h->get_dummy_primitive_instances();
  unsigned prev_size = parts_0.size();
  for (unsigned i = 0; i < d_ins.size(); i++) {
    if (d_ins[i]->kind_ != bvxm_part_instance_kind::GAUSSIAN)
      return false;

    bvxm_part_gaussian_sptr gp = d_ins[i]->cast_to_gaussian();
    if (!gp)
      return false;

    if (!extract_gaussian_primitives(img, gp->lambda0_, gp->lambda1_, gp->theta_, gp->bright_, gp->cutoff_percentage_, gp->detection_threshold_, gp->type_, parts_0))
      return false;

    std::cout << "extracted " << parts_0.size()-prev_size << " primitive parts of type: " << d_ins[i]->type_ << std::endl;
    prev_size = parts_0.size();
  }

  unsigned highest = h->highest_layer_id();
  std::vector<bvxm_part_instance_sptr> parts_upper_most(parts_0);
  for (unsigned l = 1; l <= highest; l++) {
    std::vector<bvxm_part_instance_sptr> parts_current;
    h->extract_upper_layer(parts_upper_most, ni, nj, parts_current);
    std::cout << "extracted " << parts_current.size() << " parts of layer " << l << '\n';
    parts_upper_most.clear();
    parts_upper_most = parts_current;
  }
#endif

  brec_part_hierarchy_detector hd(h);
  hd.detect(img, angle);
  std::vector<brec_part_instance_sptr>& parts_upper_most = hd.get_parts(h->highest_layer_id());

  vil_image_view<float> output_map_float(ni, nj);
  brec_part_hierarchy::generate_output_map(parts_upper_most, output_map_float);

  vil_image_view<vxl_byte> output_img(ni, nj, 3);
  brec_part_hierarchy::generate_output_img(parts_upper_most, orig_img, output_img, brec_posterior_types::CLASS_FOREGROUND);

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(output_map_float);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_map_sptr);

  vil_image_view_base_sptr out_map_sptr1 = new vil_image_view<vxl_byte>(output_img);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_map_sptr1);

  std::cout << " whole process took: " << t2.real() / 60000.0 << " mins.\n";

  return true;
}

//: Constructor
// To be used for training mainly, or when detector is not needed to be passed as an input
bool brec_recognize_structure2_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");      // input orig view
  input_types.emplace_back("vil_image_view_base_sptr");      // input view's "foreground" probability map, float img with values in [0,1] range,
                                                          // CAUTION: Convert it before passing to this process if necessary, e.g. if only the background map is available
  input_types.emplace_back("brec_part_hierarchy_sptr"); // detector hierarchy for the type of structure to be recognized (needs to be loaded a priori)
  input_types.emplace_back("float");      // angle to rotate detector for the type of structure to be recognized
                                  // should be passed zero if the original orientation of the detector will be used
  input_types.emplace_back("bool");  // set to true if detecting instances during training (sets rho value of the primitives differently during training)
  input_types.emplace_back("double");

  if (pro.set_input_types(input_types)) {
    //output
    std::vector<std::string> output_types;
    output_types.emplace_back("vil_image_view_base_sptr");      // output prob map
    output_types.emplace_back("vil_image_view_base_sptr");      // output map overlayed on orig view as a byte image
    output_types.emplace_back("brec_part_hierarchy_detector_sptr");      // output map overlayed on orig view as a byte image
    return pro.set_output_types(output_types);
  }
  else
    return false;
}


bool brec_recognize_structure2_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 6) {
    std::cerr << " brec_recognize_structure2_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  vil_image_view_base_sptr inp_img = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> orig_img(inp_img);

  vil_image_view<float> img = *vil_convert_cast(float(), inp_img);
  if (inp_img->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned ni = img.ni();
  unsigned nj = img.nj();

  vil_image_view_base_sptr inp_prob_map = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> fg_prob_map = *vil_convert_cast(float(), inp_prob_map);
  std::cout << "CAUTION: Input prob map is assumed to be a \"foreground\" probability map, convert it before passing to this process if necessary! (e.g. if only the background map is available)\n";

  if (inp_prob_map->pixel_format() == VIL_PIXEL_FORMAT_BOOL) {
    float min, max;
    vil_math_value_range(fg_prob_map, min, max);
    std::cout << "checking proper conversion of bool to float: min val in the image: " << min << " max: " << max << std::endl;
  }

  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  auto angle = pro.get_input<float>(i++);
  bool training = pro.get_input<bool>(i++);
  auto detection_radius = pro.get_input<double>(i++);

  vul_timer t2;
  t2.mark();

  std::cout << "Hierarchy Detector will use the response models at: " << h->model_dir() << std::endl;

  brec_part_hierarchy_detector_sptr hd = new brec_part_hierarchy_detector(h);

  if (training) {
    std::cout << "detect instances for training\n";
    hd->detect(img, fg_prob_map, angle, brec_detector_methods::DENSITY_FOR_TRAINING);
  }
  else {
    std::cout << "detect instances for testing\n";
    hd->detect(img, fg_prob_map, angle, brec_detector_methods::POSTERIOR, detection_radius);
  }

  std::vector<brec_part_instance_sptr>& parts_upper_most = hd->get_parts(h->highest_layer_id());

  vil_image_view<float> output_map_float(ni, nj);
  brec_part_hierarchy::generate_output_map(parts_upper_most, output_map_float);

  vil_image_view<vxl_byte> output_img(ni, nj, 3);
  brec_part_hierarchy::generate_output_img(parts_upper_most, orig_img, output_img, brec_posterior_types::CLASS_FOREGROUND);

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(output_map_float);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_map_sptr);

  vil_image_view_base_sptr out_map_sptr1 = new vil_image_view<vxl_byte>(output_img);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_map_sptr1);

  pro.set_output_val<brec_part_hierarchy_detector_sptr>(2, hd);

  std::cout << " whole process took: " << t2.real() / 60000.0 << " mins.\n";

  return true;
}
