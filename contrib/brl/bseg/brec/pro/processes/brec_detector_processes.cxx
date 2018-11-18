// This is brl/bseg/brec/pro/processes/brec_detector_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to detect instances of compositions given by part hierarchies
//
// \author Ozge Can Ozcanli
// \date March 03, 2009
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vul/vul_timer.h>

#include <brec/brec_part_hierarchy_builder.h>
#include <brec/brec_part_hierarchy_detector_sptr.h>
#include <brec/brec_part_hierarchy_detector.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_gaussian_sptr.h>
#include <brec/brec_part_gaussian.h>

//: Constructor
bool brec_initialize_detector_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_sptr"); // detector hierarchy for the type of structure to be recognized (needs to be loaded a priori)
  if (pro.set_input_types(input_types)) {
    std::vector<std::string> output_types;
    output_types.emplace_back("brec_part_hierarchy_detector_sptr");
    return pro.set_output_types(output_types);
  }
  else
    return false;
}

bool brec_initialize_detector_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    std::cerr << " brec_initialize_detector_process - invalid inputs\n";
    return false;
  }
  // get input
  unsigned i = 0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  brec_part_hierarchy_detector_sptr hd = new brec_part_hierarchy_detector(h);
  pro.set_output_val<brec_part_hierarchy_detector_sptr>(0, hd);
  return true;
}

//: Constructor
bool brec_add_hierarchy_to_detector_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_detector_sptr"); // detector hierarchy for the type of structure to be recognized (needs to be loaded a priori)
  input_types.emplace_back("brec_part_hierarchy_sptr"); // detector hierarchy for the type of structure to be recognized (needs to be loaded a priori)
  if (pro.set_input_types(input_types)) {
    std::vector<std::string> output_types;
    return pro.set_output_types(output_types);
  }
  else
    return false;
}

bool brec_add_hierarchy_to_detector_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << " brec_add_hierarchy_to_detector_process - invalid inputs\n";
    return false;
  }
  // get input
  unsigned i = 0;
  brec_part_hierarchy_detector_sptr hd = pro.get_input<brec_part_hierarchy_detector_sptr>(i++);
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  hd->add_to_class_hierarchies(h);
  return true;
}

//: Constructor
// To be used when detector is needed to be passed as an input
bool brec_detect_hierarchy_process_cons(bprb_func_process& pro)
{
  //inputs
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");      // input orig view
  input_types.emplace_back("vil_image_view_base_sptr");      // input view's "foreground" probability map, float img with values in [0,1] range,
                                                          // CAUTION: Convert it before passing to this process if necessary, e.g. if only the background map is available
  input_types.emplace_back("brec_part_hierarchy_detector_sptr"); // detector hierarchy for the type of structure to be recognized (needs to be loaded a priori)
  input_types.emplace_back("unsigned"); // layer_id (the id of the layer to be recognized
  input_types.emplace_back("float");      // angle to rotate detector for the type of structure to be recognized
                                  // should be passed zero if the original orientation of the detector will be used
  input_types.emplace_back("double");  // detection radius of primitives (doubled as we go up in the hierarchy) - used for denominator calculations of posterior probs
  input_types.emplace_back("float");  // class prior

  if (pro.set_input_types(input_types)) {
    //output
    std::vector<std::string> output_types;
    output_types.emplace_back("vil_image_view_base_sptr");      // output prob map: class foreground posterior
    output_types.emplace_back("vil_image_view_base_sptr");      // output map class foreground posterior overlayed on orig view as a byte image
    output_types.emplace_back("vil_image_view_base_sptr");      // output map class background posterior overlayed on orig view as a byte image
    output_types.emplace_back("vil_image_view_base_sptr");      // output map non-class foreground posterior overlayed on orig view as a byte image
    output_types.emplace_back("vil_image_view_base_sptr");      // output map non-class background posterior overlayed on orig view as a byte image
    return pro.set_output_types(output_types);
  }
  else
    return false;
}

//: use the init method if want to pass a foreground probability map which is 1.0 for all the image pixels (all the image is foreground)
bool brec_detect_hierarchy_process_init(bprb_func_process& pro)
{
  // initialize a dummy img
  vil_image_view<float> dummy(10, 10);
  vil_image_view_base_sptr m_ptr = new vil_image_view<float>(dummy);
  pro.set_input(1, new brdb_value_t<vil_image_view_base_sptr>(m_ptr));
  return true;
}

bool brec_detect_hierarchy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 7) {
    std::cerr << " brec_detect_hierarchy_process - invalid inputs\n";
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
  //std::cout << "CAUTION: Input prob map is assumed to be a \"foreground\" probability map, convert it before passing to this process if necessary! (e.g. if only the background map is available)\n";

  if (inp_prob_map->ni() != inp_img->ni() || inp_prob_map->nj() != inp_img->nj()) {
    std::cout << "No fg prob map is passed! Will assume all image pixels are foreground!\n";
    fg_prob_map.set_size(inp_img->ni(), inp_img->nj());
    fg_prob_map.fill(1.0f);
  }

  if (inp_prob_map->pixel_format() == VIL_PIXEL_FORMAT_BOOL) {
    float min, max;
    vil_math_value_range(fg_prob_map, min, max);
    std::cout << "checking proper conversion of bool to float: min val in the image: " << min << " max: " << max << std::endl;
  }

  brec_part_hierarchy_detector_sptr d = pro.get_input<brec_part_hierarchy_detector_sptr>(i++);
  auto layer_id = pro.get_input<unsigned>(i++);
  auto angle = pro.get_input<float>(i++);
  auto detection_radius = pro.get_input<double>(i++);
  auto class_prior = pro.get_input<float>(i++);

  vul_timer t2;
  t2.mark();

  std::cout << "Hierarchy Detector will use the response models at: " << d->get_hierarchy()->model_dir() << std::endl;

  d->detect(img, fg_prob_map, angle, brec_detector_methods::POSTERIOR, detection_radius, class_prior, layer_id);

  //std::vector<brec_part_instance_sptr>& parts_upper_most = d->get_parts(d->get_hierarchy()->highest_layer_id());
  std::vector<brec_part_instance_sptr>& parts_upper_most = d->get_parts(layer_id);

  vil_image_view<float> output_map_float(ni, nj);
  brec_part_hierarchy::generate_output_map_posterior(parts_upper_most, output_map_float, brec_posterior_types::CLASS_FOREGROUND);
  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(output_map_float);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_map_sptr);

  vil_image_view<vxl_byte> output_img1(ni, nj, 3);
  brec_part_hierarchy::generate_output_img(parts_upper_most, orig_img, output_img1, brec_posterior_types::CLASS_FOREGROUND);
  vil_image_view_base_sptr out_map_sptr1 = new vil_image_view<vxl_byte>(output_img1);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_map_sptr1);

  vil_image_view<vxl_byte> output_img2(ni, nj, 3);
  brec_part_hierarchy::generate_output_img(parts_upper_most, orig_img, output_img2, brec_posterior_types::CLASS_BACKGROUND);
  vil_image_view_base_sptr out_map_sptr2 = new vil_image_view<vxl_byte>(output_img2);
  pro.set_output_val<vil_image_view_base_sptr>(2, out_map_sptr2);

  vil_image_view<vxl_byte> output_img3(ni, nj, 3);
  brec_part_hierarchy::generate_output_img(parts_upper_most, orig_img, output_img3, brec_posterior_types::NON_CLASS_FOREGROUND);
  vil_image_view_base_sptr out_map_sptr3 = new vil_image_view<vxl_byte>(output_img3);
  pro.set_output_val<vil_image_view_base_sptr>(3, out_map_sptr3);

  vil_image_view<vxl_byte> output_img4(ni, nj, 3);
  brec_part_hierarchy::generate_output_img(parts_upper_most, orig_img, output_img4, brec_posterior_types::NON_CLASS_BACKGROUND);
  vil_image_view_base_sptr out_map_sptr4 = new vil_image_view<vxl_byte>(output_img4);
  pro.set_output_val<vil_image_view_base_sptr>(4, out_map_sptr4);


  std::cout << " whole process took: " << t2.real() / 60000.0 << " mins.\n";

  return true;
}
