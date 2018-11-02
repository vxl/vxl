// This is brl/bbas/bhdfs/pro/processes/bhdfs_save_txt_file_process.cxx
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

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>

//: Init function
bool bhdfs_save_txt_file_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.push_back("vcl_string"); // string to save
  input_types.push_back("vcl_string"); // filename
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool bhdfs_save_txt_file_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "bhdfs_save_txt_file_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  std::string data = pro.get_input<std::string>(0);
  std::string filename = pro.get_input<std::string>(1);
  // read projection matrix from the file.
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(std::string("default"),0);
  bhdfs_manager_sptr mgr = bhdfs_manager::instance();
  std::cerr << "The working directory is: [" << mgr->get_working_dir() << "]" << std::endl;
  bhdfs_fstream_sptr fs = new bhdfs_fstream(filename, "a");
  fs->write(data.c_str(),data.size());
  fs->close();

  return true;
}
