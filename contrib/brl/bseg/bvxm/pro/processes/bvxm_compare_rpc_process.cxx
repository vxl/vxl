#include "bvxm_compare_rpc_process.h"
//:
// \file
//:
// \file
#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

//: Execute the process
bool
bvxm_compare_rpc_process(bprb_func_process& pro)
{
 // process takes 3 inputs and 0 outputs: 
  //input[0]: please complete
  //input[1]: please complete
  //input[2]: please complete
 
  unsigned n_inputs_ = 3;
  if(pro.n_inputs<n_inputs_)
  {
    vcl_cout << pro.name() <<" : The input number should be "<< n_inputs_ << vcl_endl;
    return false; 
  }
  
  //get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr cam0 =pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam1 =pro.get_input<vpgl_camera_double_sptr>(i++);
  vcl_string path = pro.get_input<vcl_string>(i++);
  
  //check input's validity
  i = 0;
  if (!cam0) {
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  if (!cam1) {
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  //get the rational cameras
  vpgl_rational_camera<double>* rat_cam0 =
    dynamic_cast<vpgl_rational_camera<double>*> (cam0.as_pointer());
  if (!rat_cam0) {
    vcl_cerr << "The camera input 0 is not a rational camera\n";
    return false;
  }

  vpgl_rational_camera<double>* rat_cam1 =
    dynamic_cast<vpgl_rational_camera<double>*> (cam1.as_pointer());
  if (!rat_cam1) {
    vcl_cerr << "The camera input 1 is not a rational camera\n";
    return false;
  }

  rat_cam0->save(path+"_correct.rpc");
  rat_cam1->save(path+"_est.rpc");

  double u, v;
  u = rat_cam1->offset(vpgl_rational_camera<double>::U_INDX) -
    rat_cam0->offset(vpgl_rational_camera<double>::U_INDX);
  v = rat_cam1->offset(vpgl_rational_camera<double>::V_INDX) -
    rat_cam0->offset(vpgl_rational_camera<double>::V_INDX);

  vcl_cout << "u=" << u << " v=" << v << vcl_endl;

  vcl_ofstream file;
  file.open("C:\\test_images\\LIDAR\\Hiafa\\test-reg\\diff.txt", vcl_ofstream::app);
  file << u << ' ' << v <<'\n';
  return true;
}
