// This is brl/bseg/bvxm/pro/processes/bvxm_compare_rpc_process.cxx
#include <string>
#include <iostream>
#include <fstream>
#include "bvxm_compare_rpc_process.h"
//:
// \file
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>

//:sets input and output types for  bvxm_compare_rpc_process
bool bvxm_compare_rpc_process_cons(bprb_func_process& pro)
{
  //set input types
  using namespace bvxm_compare_rpc_process_globals;
  std::vector<std::string> input_types_(n_inputs_);

  int i=0;
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  //no outputs
  return true;
}

//: Execute the process
bool bvxm_compare_rpc_process(bprb_func_process& pro)
{
  using namespace bvxm_compare_rpc_process_globals;

  //check number of inputs.
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() <<" : The input number should be "<< n_inputs_ << std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr cam0 =pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam1 =pro.get_input<vpgl_camera_double_sptr>(i++);
  std::string path = pro.get_input<std::string>(i++);

  //check input's validity
  i = 0;
  if (!cam0) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!cam1) {
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  //get the rational cameras
  auto* rat_cam0 =
    dynamic_cast<vpgl_rational_camera<double>*> (cam0.as_pointer());
  if (!rat_cam0) {
    std::cerr << "The camera input 0 is not a rational camera\n";
    return false;
  }

  auto* rat_cam1 =
    dynamic_cast<vpgl_rational_camera<double>*> (cam1.as_pointer());
  if (!rat_cam1) {
    std::cerr << "The camera input 1 is not a rational camera\n";
    return false;
  }

  rat_cam0->save(path+"_correct.rpc");
  rat_cam1->save(path+"_est.rpc");

  double u, v;
  u = rat_cam1->offset(vpgl_rational_camera<double>::U_INDX) -
    rat_cam0->offset(vpgl_rational_camera<double>::U_INDX);
  v = rat_cam1->offset(vpgl_rational_camera<double>::V_INDX) -
    rat_cam0->offset(vpgl_rational_camera<double>::V_INDX);

  std::cout << "u=" << u << " v=" << v << std::endl;

  std::ofstream file;
  file.open("C:\\test_images\\LIDAR\\Hiafa\\test-reg\\diff.txt", std::ofstream::app);
  file << u << ' ' << v <<'\n';
  return true;
}
