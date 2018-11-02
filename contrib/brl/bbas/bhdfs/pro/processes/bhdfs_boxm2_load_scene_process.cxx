// This is brl/bbas/bhdfs/pro/processes/bhdfs_boxm2_load_scene_process.cxx
//:
// \file
// \brief  A process for loading the scene from HDFS file system.
//
// \author Gamze Tunali
// \date   Sep 16, 2012

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_scene_parser.h>

#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>

namespace bhdfs_boxm2_load_scene_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}
bool bhdfs_boxm2_load_scene_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_boxm2_load_scene_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_boxm2_load_scene_process(bprb_func_process& pro)
{
  using namespace bhdfs_boxm2_load_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  std::string filename = pro.get_input<std::string>(i++);


  boxm2_scene_parser parser;

  // load the XML from HDFS
  if (!bhdfs_manager::exists())
    bhdfs_manager::create(std::string("default"), 0);

  bhdfs_manager_sptr mgr = bhdfs_manager::instance();
  bhdfs_fstream_sptr fs = new bhdfs_fstream(filename,"r");
  unsigned long fsize = fs->file_size()-1;
  char* buf = new char[fsize];
  if (fs->read(buf, fsize) == -1)
    std::cerr << "HDFS -- error reading the file!" << std::endl;
  std::string str(buf);

  // clean up the garbage chars after the last bracket '>'
  int pos = str.find_last_of('>');
  str = str.erase(pos+1);
  delete buf;
  const char* newbuf = str.c_str();
  std::cout << fsize << "[" << newbuf << "]" << std::endl;
  if (!parser.parseString(newbuf)) {
    std::cerr << "HDFS--" << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
             << parser.XML_GetCurrentLineNumber() << '\n';
     return false;
  }

  boxm2_scene_sptr scene= new boxm2_scene(newbuf);

  i=0;
  // store scene smart pointer
  pro.set_output_val<boxm2_scene_sptr>(i++, scene);
  return true;
}
