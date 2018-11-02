// This is brl/bbas/bhdfs/pro/processes/bhdfs_load_perspective_camera_process.cxx
#include <iostream>
#include <fstream>
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

#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::vector<std::string> input_types;
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool bhdfs_load_perspective_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "bhdfs_load_perspective_camera_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  std::string camera_filename = pro.get_input<std::string>(0);
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;

  // read projection matrix from the file.
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(std::string("default"),0);
  bhdfs_manager_sptr mgr = bhdfs_manager::instance();
  std::cerr << "The working directory is: [" << mgr->get_working_dir() << "]" << std::endl;
  bhdfs_fstream_sptr fs = new bhdfs_fstream(camera_filename, "r");
  unsigned long fsize =  fs->file_size();
  char* buf = new char[fsize];
  fs->read(buf, fsize);
  fs->close();
  std::cerr << "[" << buf << "]" << std::endl;
  std::string str(buf);
  std::stringstream iss(str);
  iss >> *pcam;
  std::cerr << "CAMERA=" << *pcam;

  pro.set_output_val<vpgl_camera_double_sptr>(0, pcam);

  return true;
}
