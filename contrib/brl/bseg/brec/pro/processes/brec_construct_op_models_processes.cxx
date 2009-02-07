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

//: Constructor
bool brec_construct_bg_op_models_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brec_part_hierarchy_sptr"); // h
  input_types.push_back("vcl_string"); // output directory to save the models
  input_types.push_back("vil_image_view_base_sptr");      // bg model mean img to construct response models, float img with values in [0,1] range
  input_types.push_back("vil_image_view_base_sptr");      // bg model std dev img to construct response models, float img with values in [0,1] range
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_construct_bg_op_models_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 4) {
    vcl_cerr << "brec_construct_bg_op_models_process - invalid inputs\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  vcl_string output_dir = pro.get_input<vcl_string>(i++);
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
  vcl_vector<brec_part_instance_sptr> ins = h->get_dummy_primitive_instances();
  for (unsigned i = 0; i < ins.size(); i++) {
    if (ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = ins[i]->cast_to_gaussian();
      if (!p->construct_bg_response_model(mean_img, std_dev_img, lambda_img, k_img)) {
        vcl_cout << "problems in constructing background model for gaussian primitives!!\n";
        return false;
      }
      // write the model parameter images
      vcl_string name = output_dir+p->string_identifier()+"_bg_lambda_img.tiff";
      bool result = vil_save(lambda_img,name.c_str());
      if ( !result ) {
        vcl_cerr << "Failed to save image to" << name << vcl_endl;
        return false;
      }

      name = output_dir+p->string_identifier()+"_bg_k_img.tiff";
      result = vil_save(k_img,name.c_str());
      if ( !result ) {
        vcl_cerr << "Failed to save image to" << name << vcl_endl;
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
  vcl_vector<brec_part_instance_sptr> ins = h->get_dummy_primitive_instances();
  for (unsigned i = 0; i < ins.size(); i++) {
    if (ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = ins[i]->cast_to_gaussian();
      if (!p->construct_bg_response_model_gauss(mean_img, std_dev_img, mu_img, sigma_img)) {
        vcl_cout << "problems in constructing background model for gaussian primitives!!\n";
        return false;
      }
      // write the model parameter images
      vcl_string name = output_dir+p->string_identifier()+"_bg_mu_img.tiff";
      bool result = vil_save(mu_img,name.c_str());
      if ( !result ) {
        vcl_cerr << "Failed to save image to" << name << vcl_endl;
        return false;
      }

      name = output_dir+p->string_identifier()+"_bg_sigma_img.tiff";
      result = vil_save(sigma_img,name.c_str());
      if ( !result ) {
        vcl_cerr << "Failed to save image to" << name << vcl_endl;
        return false;
      }
    }
  }

  return true;
}

//: Constructor
bool brec_construct_fg_op_models_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brec_part_hierarchy_sptr");      // h
  input_types.push_back("vcl_string");                    // output directory to save the response model parameters for the foreground, will save a file with two parameters k and lambda
  input_types.push_back("vil_image_view_base_sptr");      // a training image to construct response models, byte image
  input_types.push_back("vil_image_view_base_sptr");      // a probability img for the input training image, float img with values in [0,1] range
  input_types.push_back("bool");                          // whether to convert the input prob img or not,
                                                          // if a background prob map is passed, it should be converted so pass true,
                                                          // but if a foreground ground-truth mask is passed for instance, it should not be converted so pass false
  input_types.push_back("vil_image_view_base_sptr");      // a mask img for the input training image, float img with values in [0,1] range, stats will be collected from the pixels with mask == true
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

bool brec_construct_fg_op_models_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 6) {
    vcl_cerr << "brec_construct_fg_op_models_process - invalid inputs\n";
    return false;
  }

  // get inputs
  unsigned i = 0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  vcl_string output_dir = pro.get_input<vcl_string>(i++);
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
    vcl_cout << " input prob map was a byte image, after conversion min value: " << min << " max value: " << max << vcl_endl;
  } else if (inp_prob->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
    vcl_cout << "In brec_construct_fg_op_models_process::execute() -- ERROR: input prob image is neither BOOL img nor FLOAT img!!\n";
    return false;
  }

  bool convert_prob_map = pro.get_input<bool>(i++);

  vil_image_view_base_sptr inp_mask = pro.get_input<vil_image_view_base_sptr>(i++);

  vil_image_view<bool> mask_img(img.ni(), img.nj());
  if (!inp_mask) {  // if mask is passed as zero make a bool img with all pixels true, i.e. use the whole input img
    mask_img.fill(true);
  } else {
    if (inp_mask->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
      vcl_cout << "In brec_construct_fg_op_models_process::execute() -- ERROR: input mask image is not of type BOOL!!\n";
      return false;
    }
    mask_img = inp_mask;
  }

  h->set_model_dir(output_dir);

  // learn a model for each primitive
  vcl_vector<brec_part_instance_sptr> ins = h->get_dummy_primitive_instances();
  for (unsigned i = 0; i < ins.size(); i++) {
    if (ins[i]->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = ins[i]->cast_to_gaussian();
      double lambda, k;
      if (!p->construct_fg_response_model(img, prob, mask_img, convert_prob_map, lambda, k)) {
        vcl_cout << "problems in constructing foreground response model parameters for gaussian primitives!!\n";
        return false;
      }
      // write the model parameters into a file in the output directory
      vcl_string name = output_dir+p->string_identifier()+"_fg_params.txt";
      vcl_ofstream of(name.c_str());
      of << k << ' ' << lambda << vcl_endl;
      of.close();
    }
  }

  return true;
}

