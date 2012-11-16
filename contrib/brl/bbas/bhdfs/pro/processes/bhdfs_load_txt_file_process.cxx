// This is brl/bbas/bhdfs/pro/processes/bhdfs_load_txt_file_process.cxx
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


#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>

//: Init function
bool bhdfs_load_txt_file_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vcl_string");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool bhdfs_load_txt_file_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "bhdfs_load_txt_file_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string filename = pro.get_input<vcl_string>(0);

  // read projection matrix from the file.
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(vcl_string("default"),0);
  bhdfs_manager_sptr mgr = bhdfs_manager::instance();
  vcl_cerr << "The working directory is: [" << mgr->get_working_dir() << "]" << vcl_endl;
  bhdfs_fstream_sptr fs = new bhdfs_fstream(filename, "r");
  unsigned long fsize =  fs->file_size();
  char* buf = new char[fsize];
  fs->read(buf, fsize);
  fs->close();
  vcl_cerr << "[" << buf << "]" << vcl_endl;
  vcl_string str(buf);
  
  pro.set_output_val<vcl_string>(0, str);

  return true;
}
