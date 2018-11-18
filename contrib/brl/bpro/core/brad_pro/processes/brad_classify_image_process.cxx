// This is brl/bpro/core/brad_pro/processes/brad_classify_image_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
#include <brad/brad_eigenspace.h>
#include <vil/vil_load.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
namespace bbas_core_brad_classify_image
{
}

//: Constructor
bool brad_classify_image_process_cons(bprb_func_process& pro)
{
  using namespace bbas_core_brad_classify_image;
#if 0
  std::vector<std::string> input_types(7);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bsta_joint_histogram_3d_base_sptr"; //no atmospherics
  input_types[2]="bsta_joint_histogram_3d_base_sptr"; //with atmospherics
  input_types[3]="vcl_string"; //input image path
  input_types[4]="vcl_string"; //output image path
  input_types[5]="unsigned"; //tile ni
  input_types[6]="unsigned"; //tile nj
  bool ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no outputs
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
#endif
  std::vector<std::string> input_types(6);
  input_types[0]="brad_eigenspace_sptr"; //eigenspace
  input_types[1]="bsta_joint_histogram_3d_base_sptr"; //no atmospherics
  input_types[2]="bsta_joint_histogram_3d_base_sptr"; //with atmospherics
  input_types[3]="vcl_string"; //input image path
  input_types[4]="unsigned"; //tile ni
  input_types[5]="unsigned"; //tile nj

  //no outputs
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  output_types.emplace_back("vil_image_view_base_sptr");
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool brad_classify_image_process(bprb_func_process& pro)
{
  using namespace bbas_core_brad_classify_image;
  // Sanity check
  if (pro.n_inputs()!= 6) {
    std::cout << "brad_classify_image_process: The number of inputs should be 6" << std::endl;
    return false;
  }
  brad_eigenspace_sptr es_ptr = pro.get_input<brad_eigenspace_sptr>(0);
  if (!es_ptr) {
    std::cout << "in classify_image_process, null eigenspace pointer\n";
    return false;
  }
  bsta_joint_histogram_3d_base_sptr hno_ptr =
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(1);

  auto* hist_no = dynamic_cast<bsta_joint_histogram_3d<float>*>(hno_ptr.ptr());

  if (!hist_no) {
    std::cout << "in classify_image_process, hist can't be cast\n";
    return false;
  }

  bsta_joint_histogram_3d_base_sptr h_atmos_ptr =
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(2);

  auto* hist_atmos = dynamic_cast<bsta_joint_histogram_3d<float>*>(h_atmos_ptr.ptr());

  if (!hist_atmos) {
    std::cout << "in classify_image_process, hist can't be cast\n";
    return false;
  }
  std::string input_path = pro.get_input<std::string>(3);
  vil_image_resource_sptr input = vil_load_image_resource(input_path.c_str());
  if (!input) {
    std::cout << "in classify_image_process, input resource can't be loaded\n";
    return false;
  }
  //std::string output_path = pro.get_input<std::string>(4);

  auto nit = pro.get_input<unsigned>(4);
  auto njt = pro.get_input<unsigned>(5);

  //CAST_CALL_EIGENSPACE(es_ptr, ep->classify_image(input, *hist_no, *hist_atmos, nit, njt, output_path), "in classify_image_process - classify function failed\n")
  vil_image_resource_sptr out_r, out_r_orig_size;
  CAST_CALL_EIGENSPACE(es_ptr, ep->classify_image(input, *hist_no, *hist_atmos, nit, njt, out_r, out_r_orig_size), "in classify_image_process - classify function failed\n")

  pro.set_output_val<vil_image_view_base_sptr>(0, out_r->get_view());
  pro.set_output_val<vil_image_view_base_sptr>(1, out_r_orig_size->get_view());

  return true;
}
