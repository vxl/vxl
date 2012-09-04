// This is brl/bpro/core/sdet_pro/processes/sdet_texture_classifier_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_atmospheric_image_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <vil/vil_image_view.h>

//: initialize input and output types
bool sdet_texture_classifier_process_cons(bprb_func_process& pro)
{
  // process takes 4 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("sdet_texture_classifier_sptr"); //classifier
  input_types.push_back("vcl_string"); //texton dictionary
  input_types.push_back("vil_image_view_base_sptr"); //input image path
  input_types.push_back("unsigned");   //texture block size
  if (!pro.set_input_types(input_types))
    return false;

  // process has 1 output:
  // output[0]: output texture color image
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classifier_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process inputs are not valid"<< vcl_endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr =
    pro.get_input<sdet_texture_classifier_sptr>(0);
  sdet_texture_classifier_params* tcp = static_cast<sdet_texture_classifier_params*>(tc_ptr.ptr());
  sdet_atmospheric_image_classifier tc(*tcp);
  vcl_string texton_dict_path = pro.get_input<vcl_string>(1);
  vil_image_view_base_sptr view_ptr =
    pro.get_input<vil_image_view_base_sptr>(2);
  if (!view_ptr)
  {
    vcl_cout << "null image in sdet_texture_classifier_process\n";
    return false;
  }
  //assumes a float image on the range [0, 1];
  vil_image_view<float> fview(view_ptr);

  unsigned block_size = pro.get_input<unsigned>(3); // FIXME -- unused!!
  tc.load_dictionary(texton_dict_path);
  vcl_vector<vcl_string> cats;
  // hard code the names of the atomospheric categories - needs more
  // generality - JLM
  cats.push_back("cld");   cats.push_back("haz");
  tc.set_atmospheric_categories(cats);
  vil_image_view<float> class_img = tc.classify_image_blocks_qual(fview);
  // return the output image
  vil_image_view_base_sptr img_ptr = new vil_image_view<float>(class_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, img_ptr);
  return true;
}
