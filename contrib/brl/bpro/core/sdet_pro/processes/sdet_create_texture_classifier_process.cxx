// This is brl/bpro/core/sdet_pro/processes/sdet_create_texture_classifier_process.cxx
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_texture_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <bbas_pro/bbas_1d_array_string.h>
#include <bbas_pro/bbas_1d_array_string_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsl/vsl_binary_io.h>

//: initialize input and output types
bool sdet_create_texture_classifier_process_cons(bprb_func_process& pro)
{
  vsl_add_to_binary_loader(vsol_polygon_2d());
  // process takes 9 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("float"); //lambda 0
  input_types.push_back("float"); //lambda 1
  input_types.push_back("unsigned"); //number of scales
  input_types.push_back("float"); //scale interval
  input_types.push_back("float"); //angle interval
  input_types.push_back("float"); //laplace radius
  input_types.push_back("float"); //gauss radius
  input_types.push_back("unsigned"); // k
  input_types.push_back("unsigned"); // number of samples
  if (!pro.set_input_types(input_types))
    return false;

  // process has 1 output:
  // output[0]: the current state of the texture classifier
  vcl_vector<vcl_string> output_types;
  output_types.push_back("sdet_texture_classifier_sptr");
  return pro.set_output_types(output_types);
}

bool sdet_create_texture_classifier_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process inputs are not valid"<< vcl_endl;
    return false;
  }

  // get inputs
  float    lambda0        = pro.get_input<float>(0);
  float    lambda1        = pro.get_input<float>(1);
  unsigned n_scales       = pro.get_input<unsigned>(2);
  float    scale_interval = pro.get_input<float>(3);
  float    angle_interval = pro.get_input<float>(4);
  float    laplace_radius = pro.get_input<float>(5);
  float    gauss_radius   = pro.get_input<float>(6);
  unsigned k              = pro.get_input<unsigned>(7);
  unsigned n_samples      = pro.get_input<unsigned>(8);

  //set texture classifier params
  sdet_texture_classifier_params tcp;
  tcp.k_              = k;
  tcp.lambda0_        = lambda0;
  tcp.lambda1_        = lambda1;
  tcp.n_scales_       = n_scales;
  tcp.n_samples_      = n_samples;
  tcp.scale_interval_ = scale_interval;
  tcp.angle_interval_ = angle_interval;
  tcp.laplace_radius_ = laplace_radius;
  tcp.gauss_radius_   = gauss_radius;
  tcp.signed_response_ = true;
  tcp.mag_ = false;
  sdet_texture_classifier_sptr tc_ptr = new sdet_texture_classifier(tcp);

  // pass the texture classifier into the database
  // to enable subsequent processing
  pro.set_output_val<sdet_texture_classifier_sptr>(0, tc_ptr);

  return true;
}

//: PROCESS to save the current training data if any (saves the parameter block as well)
bool sdet_save_texture_classifier_process_cons(bprb_func_process& pro)
{
  // process takes 2 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("sdet_texture_classifier_sptr"); //texture classifier
  input_types.push_back("vcl_string"); // output filename

  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  return pro.set_output_types(output_types);
}

bool sdet_save_texture_classifier_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process inputs are not valid"<< vcl_endl;
    return false;
  }

  sdet_texture_classifier_sptr tc_ptr = pro.get_input<sdet_texture_classifier_sptr>(0);
  if (!tc_ptr){
    vcl_cout << "In finishing texture training - null texture_classifier\n";
    return false;
  }
  vcl_string name = pro.get_input<vcl_string>(1);
  if (name == "")
    return false;
  // saves the parameters and the current training data
  tc_ptr->save_data(name);
  return true;
}


//: PROCESS to load the current training data if any (loads the parameter block as well)
bool sdet_load_texture_classifier_process_cons(bprb_func_process& pro)
{
  // process takes 1 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // input filename to read data for the instance of texture classifier
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("sdet_texture_classifier_sptr"); //texture classifier
  return pro.set_output_types(output_types);
}

bool sdet_load_texture_classifier_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process inputs are not valid"<< vcl_endl;
    return false;
  }

  vcl_string input_ins_path = pro.get_input<vcl_string>(0);

  sdet_texture_classifier_params dummy;
  sdet_texture_classifier_sptr tc_ptr = new sdet_texture_classifier(dummy);
  tc_ptr->load_data(input_ins_path);  
  vcl_cout << " loaded classifier with params: " << *tc_ptr << vcl_endl;
  tc_ptr->filter_responses().set_params(tc_ptr->n_scales_,tc_ptr->scale_interval_,tc_ptr->lambda0_,tc_ptr->lambda1_,tc_ptr->angle_interval_,tc_ptr->cutoff_per_);
 
  vcl_cout << " in the loaded classifier max filter radius: " << tc_ptr->max_filter_radius() << vcl_endl;
   
  // pass the texture classifier into the database
  // to enable subsequent processing
  pro.set_output_val<sdet_texture_classifier_sptr>(0, tc_ptr);
  return true;
}


//: PROCESS to load the current dictionary (loads the parameter block as well)
bool sdet_load_texture_dictionary_process_cons(bprb_func_process& pro)
{
  // process takes 1 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // input filename 
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("sdet_texture_classifier_sptr"); //texture classifier
  return pro.set_output_types(output_types);
}

bool sdet_load_texture_dictionary_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process inputs are not valid"<< vcl_endl;
    return false;
  }

  vcl_string input_ins_path = pro.get_input<vcl_string>(0);

  sdet_texture_classifier_params dummy;
  sdet_texture_classifier_sptr tc_ptr = new sdet_texture_classifier(dummy);
  tc_ptr->load_dictionary(input_ins_path);  
  vcl_cout << " loaded classifier with params: " << *tc_ptr << vcl_endl;
  tc_ptr->filter_responses().set_params(tc_ptr->n_scales_,tc_ptr->scale_interval_,tc_ptr->lambda0_,tc_ptr->lambda1_,tc_ptr->angle_interval_,tc_ptr->cutoff_per_);
 
  vcl_cout << " in the loaded classifier max filter radius: " << tc_ptr->max_filter_radius() << vcl_endl;
  // pass the texture classifier into the database
  // to enable subsequent processing
  pro.set_output_val<sdet_texture_classifier_sptr>(0, tc_ptr);
  return true;
}

