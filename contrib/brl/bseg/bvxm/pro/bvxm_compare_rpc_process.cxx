#include "bvxm_compare_rpc_process.h"
//:
// \file

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

//: Constructor
bvxm_compare_rpc_process::bvxm_compare_rpc_process()
{
  //this process takes no input
  input_data_.resize(3, brdb_value_sptr(0));
  input_types_.resize(3);

  int i=0;
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "vpgl_camera_double_sptr";   // rational camera
  input_types_[i++] = "vcl_string";
  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
  //output_types_[0]= "bvxm_compare_rpc_sptr";
}


//: Execute the process
bool
bvxm_compare_rpc_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  brdb_value_t<vpgl_camera_double_sptr >* input0 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr >* >(input_data_[0].ptr());
  vpgl_camera_double_sptr cam0 = input0->value();

  brdb_value_t<vpgl_camera_double_sptr >* input1 =
    static_cast<brdb_value_t<vpgl_camera_double_sptr >* >(input_data_[1].ptr());
  vpgl_camera_double_sptr cam1 = input1->value();

  brdb_value_t<vcl_string >* input2 =
    static_cast<brdb_value_t<vcl_string >* >(input_data_[2].ptr());
  vcl_string path = input2->value();

  vpgl_rational_camera<double>* rat_cam0 =
    dynamic_cast<vpgl_rational_camera<double>*> (cam0.as_pointer());
  if (!rat_cam0) {
    vcl_cerr << "The camera input is not a rational camera\n";
    return false;
  }

  vpgl_rational_camera<double>* rat_cam1 =
    dynamic_cast<vpgl_rational_camera<double>*> (cam1.as_pointer());
  if (!rat_cam1) {
    vcl_cerr << "The camera input is not a rational camera\n";
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
