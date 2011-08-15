// This is brl/bpro/core/bbas_pro/processes/brad_classify_image_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_hist_prob_feature_vector.h>
#include <brad/brad_grad_hist_feature_vector.h>
#include <brad/brad_grad_int_feature_vector.h>
#include <brad/brad_eigenspace.h>
#include <vil/vil_load.h>
//:
// \file

#include <vcl_fstream.h>
namespace bbas_core_brad_classify_image
{
}

//: Constructor
bool brad_classify_image_process_cons(bprb_func_process& pro)
{
using namespace bbas_core_brad_classify_image;
  bool ok=false;
  vcl_vector<vcl_string> input_types(7);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bsta_joint_histogram_3d_base_sptr"; //no atmospherics
  input_types[2]="bsta_joint_histogram_3d_base_sptr"; //with atmospherics
  input_types[3]="vcl_string"; //input image path
  input_types[4]="vcl_string"; //output image path
  input_types[5]="unsigned"; //tile ni
  input_types[6]="unsigned"; //tile nj
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no outputs
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_classify_image_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_classify_image;
  // Sanity check
  if (pro.n_inputs()!= 7) {
    vcl_cout << "brad_classify_image_process: The input number should be 5" << vcl_endl;
    return false;
  }
  brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if(!es_ptr){
    vcl_cout << "in classify_image_process, null eigenspace pointer\n";
    return false;
  }
  bsta_joint_histogram_3d_base_sptr hno_ptr = 
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(1);

  bsta_joint_histogram_3d<float>* hist_no = dynamic_cast<bsta_joint_histogram_3d<float>*>(hno_ptr.ptr());

  if(!hist_no){
    vcl_cout << "in classify_image_process, hist can't be cast\n";
    return false;
  }

  bsta_joint_histogram_3d_base_sptr h_atmos_ptr = 
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(2);

  bsta_joint_histogram_3d<float>* hist_atmos = dynamic_cast<bsta_joint_histogram_3d<float>*>(h_atmos_ptr.ptr());

  if(!hist_atmos){
    vcl_cout << "in classify_image_process, hist can't be cast\n";
    return false;
  }
  vcl_string input_path = pro.get_input<vcl_string>(3);
  vil_image_resource_sptr input = vil_load_image_resource(input_path.c_str());
  if(!input){
    vcl_cout << "in classify_image_process, input resource can't be loaded\n";
    return false;
  } 
  vcl_string output_path = pro.get_input<vcl_string>(4);

  unsigned nit = pro.get_input<unsigned>(5);
  unsigned njt = pro.get_input<unsigned>(6);

  if(es_ptr->feature_vector_type() == "brad_hist_prob_feature_vector"){
    brad_eigenspace<brad_hist_prob_feature_vector>* hp =
      dynamic_cast<brad_eigenspace<brad_hist_prob_feature_vector>* >(es_ptr.ptr());
    if(!hp->classify_image(input, *hist_no, *hist_atmos, nit, njt, output_path)){
      vcl_cout << "in classify_image_process -classify function failed\n";
      return false;
    }
  }else if(es_ptr->feature_vector_type() == "brad_grad_hist_feature_vector"){
    brad_eigenspace<brad_grad_hist_feature_vector>* hp =
      dynamic_cast<brad_eigenspace<brad_grad_hist_feature_vector>* >(es_ptr.ptr());
    if(!hp->classify_image(input, *hist_no, *hist_atmos, nit, njt, output_path)){
      vcl_cout << "in classify_image_process - classify function failed\n";
      return false;
    }
  }else if(es_ptr->feature_vector_type() == "brad_grad_int_feature_vector"){
     brad_eigenspace<brad_grad_int_feature_vector>* hp =
      dynamic_cast<brad_eigenspace<brad_grad_int_feature_vector>* >(es_ptr.ptr());
    if(!hp->classify_image(input, *hist_no, *hist_atmos, nit, njt, output_path)){
      vcl_cout << "in classify_image_process - classify function failed\n";
      return false;
    }
  }else{
      vcl_cout << "in classify_image_process-unknown feature vector type\n";
      return false;
  }
  return true;
}

