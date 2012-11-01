// This is brl/bbas/bhdfs/pro/processes/bhdfs_load_perspective_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to load a perspective camera via libdhfs to access hadoop's file system (HDFS)
//
//     
// \author Gamze Tunali
// \date Sep15, 2012
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>

//: Init function
bool bhdfs_load_perspective_camera_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool bhdfs_load_perspective_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "bhdfs_load_perspective_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string camera_filename = pro.get_input<vcl_string>(0);
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;

  // read projection matrix from the file.
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(vcl_string("default"),0);
  bhdfs_manager_sptr mgr = bhdfs_manager::instance();
  vcl_cerr << "The working directory is: [" << mgr->get_working_dir() << "]" << vcl_endl;
  bhdfs_fstream_sptr fs = new bhdfs_fstream(camera_filename, "r");
  unsigned long fsize =  fs->file_size();
  char* buf = new char[fsize];
  fs->read(buf, fsize);
  fs->close();
  vcl_cerr << "[" << buf << "]" << vcl_endl;
  vcl_string str(buf);
  vcl_stringstream iss(str);
  iss >> *pcam;
  vcl_cerr << "CAMERA=" << *pcam;
  
  pro.set_output_val<vpgl_camera_double_sptr>(0, pcam);

  return true;
}
