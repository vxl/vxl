// This is brl/bpro/core/sdet_pro/processes/sdet_texture_training_process.cxx
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_texture_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <vil/vil_image_view.h>
#include <bbas_pro/bbas_1d_array_string.h>
#include <bbas_pro/bbas_1d_array_string_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsl/vsl_binary_io.h>

//: initialize input and output types
bool sdet_texture_training_process_cons(bprb_func_process& pro)
{
  vsl_add_to_binary_loader(vsol_polygon_2d());
  // process takes 6 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //texture classifier
  input_types.emplace_back("bool"); //if true, compute textons for category,
  //otherwise append filter responses from the image to the training data
  input_types.emplace_back("vil_image_view_base_sptr"); //input image
  input_types.emplace_back("vcl_string"); //polygon file
  input_types.emplace_back("vcl_string"); // category name
  input_types.emplace_back("vcl_string"); // texton dictionary path

  if (!pro.set_input_types(input_types))
    return false;

  // process has 1 output:
  // output[0]: the current state of the texture classifier
  std::vector<std::string> output_types;
  output_types.emplace_back("sdet_texture_classifier_sptr");
  return pro.set_output_types(output_types);
}

bool sdet_texture_training_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process inputs are not valid"<< std::endl;
    return false;
  }

  // get inputs
  //== the texture classifier
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);

  //== compute textons (true) or just append samples to training data
  //for the category (false)
  bool compute_textons = pro.get_input<bool>(1);

  //== the input image
  vil_image_view_base_sptr view_ptr =
    pro.get_input<vil_image_view_base_sptr>(2);
  if (!view_ptr)
  {
    std::cout << "null image in sdet_texture_classifier_process\n";
    return false;
  }
  //assumes a float image on the range [0, 1] so cast without checking
  vil_image_view<float> fview(view_ptr);

  //go ahead and compute the filter bank as well as laplace and Gauss filters
  if (!tc_ptr->compute_filter_bank(fview)){
    std::cout << "failed to compute filter bank\n";
    return false;
  }

  //remaining arguments
  //== polygon region delineating samples of the texture
  std::string poly_path = pro.get_input<std::string>(3);

  //== the string name of the texture category
  std::string category = pro.get_input<std::string>(4);

  // extract the training data from the current image filter responses
  if (!tc_ptr->compute_training_data(category, poly_path))
  {
    std::cout << "failed to compute training data\n";
    return false;
  }
  // if the last training image for the category then complete the
  // texton dictionary entry
  if (compute_textons)
    if (!tc_ptr->compute_textons(category))
      return false;

  // pass the texture classifier into the database for additional training
  // data or additional categories
  pro.set_output_val<sdet_texture_classifier_sptr>(0, tc_ptr);

  return true;
}

// this method is called if all training is complete
// the texton dictionary is finalized and written out to the specified
// file
bool sdet_texture_training_process_finish(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  if (!tc_ptr){
    std::cout << "In finishing texture training - null texture_classifier\n";
    return false;
  }
  std::string dict_path = pro.get_input<std::string>(5);
  if (dict_path == "")
    return false;
  tc_ptr->compute_category_histograms();
  std::cout << "Saving dictionary\n";
  if (!tc_ptr->save_dictionary(dict_path))
    return false;
  return true;
}


//: this process assumes that the input classifier already has the filter bank of the training image (e.g. run sdetExtractFilterBankProcess first)
bool sdet_texture_training_process2_cons(bprb_func_process& pro)
{
  vsl_add_to_binary_loader(vsol_polygon_2d());
  // process takes 5 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //texture classifier
  input_types.emplace_back("bool"); //if true, compute textons for category,
  //otherwise append filter responses from the image to the training data
  input_types.emplace_back("vcl_string"); //polygon file
  input_types.emplace_back("vcl_string"); // category name
  input_types.emplace_back("vcl_string"); // texton dictionary path

  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  output_types.emplace_back("sdet_texture_classifier_sptr");
  return pro.set_output_types(output_types);
}

bool sdet_texture_training_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process2 inputs are not valid"<< std::endl;
    return false;
  }

  // get inputs
  //== the texture classifier
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);

  //== compute textons (true) or just append samples to training data
  //for the category (false)
  bool compute_textons = pro.get_input<bool>(1);

  //remaining arguments
  //== polygon region delineating samples of the texture
  std::string poly_path = pro.get_input<std::string>(2);

  //== the string name of the texture category
  std::string category = pro.get_input<std::string>(3);

  // extract the training data from the current image filter responses
  if (!tc_ptr->compute_training_data(category, poly_path))
  {
    std::cout << "failed to compute training data\n";
    return false;
  }
  // if the last training image for the category then complete the
  // texton dictionary entry
  if (compute_textons)
    if (!tc_ptr->compute_textons(category))
      return false;

  // pass the texture classifier into the database for additional training
  // data or additional categories
  pro.set_output_val<sdet_texture_classifier_sptr>(0, tc_ptr);

  return true;
}

// this method is called if all training is complete
// the texton dictionary is finalized and written out to the specified
// file
bool sdet_texture_training_process2_finish(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  if (!tc_ptr){
    std::cout << "In finishing texture training - null texture_classifier\n";
    return false;
  }
  std::string dict_path = pro.get_input<std::string>(4);
  if (dict_path == "")
    return false;
  tc_ptr->compute_category_histograms();
  std::cout << "Saving dictionary\n";
  if (!tc_ptr->save_dictionary(dict_path))
    return false;
  return true;
}

//: this process converts vsol objects saved as .bin files to .txt files
bool sdet_dump_vsol_binary_data_process_cons(bprb_func_process& pro)
{
  vsl_add_to_binary_loader(vsol_polygon_2d());
  // process takes 5 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); //polygon file
  input_types.emplace_back("vcl_string"); //output file name
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_dump_vsol_binary_data_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process2 inputs are not valid"<< std::endl;
    return false;
  }

  //== polygon region delineating samples of the texture
  std::string poly_path = pro.get_input<std::string>(0);

  //== the string name of the texture category
  std::string output_file = pro.get_input<std::string>(1);
  std::ofstream ofs(output_file.c_str());
  if (!ofs) {
    std::cout << " In sdet_dump_vsol_binary_data_process() -- cannot open file: " << output_file << std::endl;
    return false;
  }

  // dummy classifier
  sdet_texture_classifier_params param;
  sdet_texture_classifier_sptr tc_ptr = new sdet_texture_classifier(param);
  std::vector<vgl_polygon<double> > polys;
  polys = tc_ptr->load_polys(poly_path);

  for (auto & poly : polys) {
    for (unsigned j = 0; j < poly.num_sheets(); j++)
      for (unsigned k = 0; k < poly[j].size(); k++)
        ofs << poly[j][k].x() << " " << poly[j][k].y() << " ";
    ofs << '\n';
  }
  ofs.close();
  return true;
}
