// This is brl/bseg/brec/pro/processes/brec_construct_op_models_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//
// brec_construct_bg_op_models_process:
// A process to construct operator response models for the primitive parts of a given hierarchy
// For all the primitive instances in the input hierarchy, the background response models
// are constructed and saved into the input directory depending on the type of the primitive instance
// A mixture of gaussian (MOG) image needs to be inputted as the background model of the scene
//
// brec_construct_fg_op_models_process:
// A class to construct operator response models for the primitive parts of a given hierarchy
// For all the primitive instances in the input hierarchy, the foreground response models
// are constructed and saved into the input directory depending on the type of the primitive instance
// An image with a prob img to designate foreground regions is required
//
// \author Ozge Can Ozcanli
// \date Dec 26, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_gaussian_sptr.h>
#include <brec/brec_part_gaussian.h>

#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_threshold.h>

//: Constructor
bool brec_construct_bg_op_models_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_sptr"); // h
  input_types.emplace_back("vcl_string"); // output directory to save the models
  input_types.emplace_back("vil_image_view_base_sptr");      // bg model mean img to construct response models, float img with values in [0,1] range
  input_types.emplace_back("vil_image_view_base_sptr");      // bg model std dev img to construct response models, float img with values in [0,1] range
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_construct_bg_op_models_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 4) {
    std::cerr << "brec_construct_bg_op_models_process - invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  std::string output_dir = pro.get_input<std::string>(i++);
  vil_image_view_base_sptr inp_mean = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_mean->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view_base_sptr inp_std_dev = pro.get_input<vil_image_view_base_sptr>(i++);

  if (inp_std_dev->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  vil_image_view<float> mean_img(inp_mean);
  vil_image_view<float> std_dev_img(inp_std_dev);

#if 0  // using weibull model, i.e. saving params lambda, k
  vil_image_view<float> lambda_img(mean_img.ni(), mean_img.nj());
  vil_image_view<float> k_img(mean_img.ni(), mean_img.nj());

  h->set_model_dir(output_dir);

  // learn a model for each primitive
  std::vector<brec_part_instance_sptr> ins = h->get_dummy_primitive_instances();
  for (unsigned i = 0; i < ins.size(); i++) {
    if (ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = ins[i]->cast_to_gaussian();
      if (!p->construct_bg_response_model(mean_img, std_dev_img, lambda_img, k_img)) {
        std::cout << "problems in constructing background model for gaussian primitives!!\n";
        return false;
      }
      // write the model parameter images
      std::string name = output_dir+p->string_identifier()+"_bg_lambda_img.tiff";
      bool result = vil_save(lambda_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return false;
      }

      name = output_dir+p->string_identifier()+"_bg_k_img.tiff";
      result = vil_save(k_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return false;
      }
    }
  }
#endif
  //: use the gaussian response model at every pixel
  vil_image_view<float> mu_img(mean_img.ni(), mean_img.nj());
  vil_image_view<float> sigma_img(mean_img.ni(), mean_img.nj());

  h->set_model_dir(output_dir);

  // learn a model for each primitive
  std::vector<brec_part_instance_sptr> ins = h->get_dummy_primitive_instances();
  for (auto & in : ins) {
    if (in->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = in->cast_to_gaussian();
      if (!p->construct_bg_response_model_gauss(mean_img, std_dev_img, mu_img, sigma_img)) {
        std::cout << "problems in constructing background model for gaussian primitives!!\n";
        return false;
      }
      // write the model parameter images
      std::string name = output_dir+p->string_identifier()+"_bg_mu_img.tiff";
      bool result = vil_save(mu_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return false;
      }

      name = output_dir+p->string_identifier()+"_bg_sigma_img.tiff";
      result = vil_save(sigma_img,name.c_str());
      if ( !result ) {
        std::cerr << "Failed to save image to" << name << std::endl;
        return false;
      }
    }
  }

  return true;
}

//: Constructor
bool brec_construct_class_op_models_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_sptr");      // h
  input_types.emplace_back("vcl_string");                    // output directory to save the response model parameters for the foreground, will save a file with two parameters k and lambda
  input_types.emplace_back("vil_image_view_base_sptr");      // a training image to construct response models,
  input_types.emplace_back("vil_image_view_base_sptr");      // a class probability img for the input training image, float img with values in [0,1] range, it could be a byte image, class regions are assumed to be true
  input_types.emplace_back("vil_image_view_base_sptr");      // a mask img for the input training image, stats will be collected from the pixels with mask == true
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: use the init method if want to pass a mask image which is true for all the image pixels (all the image is used)
bool brec_construct_class_op_models_process_init(bprb_func_process& pro)
{
  // initialize an empty pointer
  vil_image_view_base_sptr m_ptr;
  pro.set_input(4, new brdb_value_t<vil_image_view_base_sptr>(m_ptr));
  return true;
}

bool brec_construct_class_op_models_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 5) {
    std::cerr << "brec_construct_fg_op_models_process - invalid inputs\n";
    return false;
  }

  // get inputs
  unsigned i = 0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  std::string output_dir = pro.get_input<std::string>(i++);
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
    std::cout << "In brec_construct_fg_op_models_process::execute() -- ERROR: input prob image is neither BOOL img nor FLOAT img!!\n";
    return false;
  }

  vil_image_view_base_sptr inp_mask = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<bool> mask_img(img.ni(), img.nj());
  if (!inp_mask) {  // if mask is passed as zero make a bool img with all pixels true, i.e. use the whole input img
    std::cout << "Made a mask image which is true everywhere!\n";
    mask_img.fill(true);
  } else if (inp_mask->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {  // assume white pixels are true
    std::cout << "converting white pixels to true to create a BOOL mask image\n";
    vil_image_view<float> m_prob = *vil_convert_cast(float(), inp_mask);
    vil_threshold_above<float>(m_prob, mask_img, 128);
    vil_save(mask_img, "./thresholded_mask.tiff");
  } else {
    if (inp_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
      std::cout << "In brec_construct_fg_op_models_process::execute() -- ERROR: input mask image is not of type BOOL!!\n";
      return false;
    }
    mask_img = inp_mask;
  }

  h->set_model_dir(output_dir);

  // learn a model for each primitive
  std::vector<brec_part_instance_sptr> ins = h->get_dummy_primitive_instances();
  for (auto & in : ins) {
    if (in->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = in->cast_to_gaussian();
      double lambda, k, lambda_non_class, k_non_class;
      if (!p->construct_class_response_models(img, prob, mask_img, lambda, k, lambda_non_class, k_non_class)) {
        std::cout << "problems in constructing foreground response model parameters for gaussian primitives!!\n";
        return false;
      }
      // write the model parameters into a file in the output directory
      std::string name = output_dir+p->string_identifier()+"_fg_params.txt";
      std::ofstream of(name.c_str());
      of << k << ' ' << lambda << std::endl;
      of << k_non_class << ' ' << lambda_non_class << std::endl;
      of.close();
    }
  }

  return true;
}
