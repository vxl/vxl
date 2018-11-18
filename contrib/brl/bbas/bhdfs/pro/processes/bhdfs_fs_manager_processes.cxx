// This is brl/bbas/bhdfs/pro/processes/bhdfs_fs_manager_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Processes for creating HDFS manager and run various FS operations
//
// \author Ozge C. Ozcanli
// \date Dec 14, 2011

#include <bhdfs/bhdfs_manager.h>


namespace bhdfs_create_fs_manager_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}

bool bhdfs_create_fs_manager_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_create_fs_manager_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // host_name
  input_types_[1] = "int";         // port

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "bhdfs_manager_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_create_fs_manager_process(bprb_func_process& pro)
{
  using namespace bhdfs_create_fs_manager_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  std::string host_name= pro.get_input<std::string>(i++);
  int port = pro.get_input<int>(i++);

  if (!bhdfs_manager::exists())
    bhdfs_manager::create(host_name, port);

  if (!bhdfs_manager::exists())  // if still doesn't exist, there is a problem
    return false;

  i=0;
  // store smart pointer
  pro.set_output_val<bhdfs_manager_sptr>(i++, bhdfs_manager::instance());
  return true;
}

namespace bhdfs_fs_create_dir_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 0;
}

bool bhdfs_fs_create_dir_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_fs_create_dir_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // path on current dir

  // process has no output
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_fs_create_dir_process(bprb_func_process& pro)
{
  using namespace bhdfs_fs_create_dir_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  std::string path= pro.get_input<std::string>(i++);

  if (!bhdfs_manager::exists())
    return false;

  bhdfs_manager_sptr mins = bhdfs_manager::instance();
  std::string fpath = mins->get_working_dir() + "/" + path;
  if (mins->exists(fpath))
    return true;
  else
    return mins->create_dir(fpath);
}

namespace bhdfs_fs_copy_file_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool bhdfs_fs_copy_file_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_fs_copy_file_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // full path of file on local dir
  input_types_[1] = "vcl_string";  // path on hdfs

  // process has no output
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_fs_copy_file_process(bprb_func_process& pro)
{
  using namespace bhdfs_fs_copy_file_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  if (!bhdfs_manager::exists())
    return false;

  //get the inputs
  unsigned i = 0;
  std::string local_file= pro.get_input<std::string>(i++);

  bhdfs_manager_sptr mins = bhdfs_manager::instance();
  std::string hdfs_path= mins->get_working_dir() + "/" + pro.get_input<std::string>(i++);
  return mins->copy_to_hdfs(local_file, hdfs_path);
}

namespace bhdfs_fs_copy_files_to_local_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}

bool bhdfs_fs_copy_files_to_local_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_fs_copy_files_to_local_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // path on hdfs
  input_types_[1] = "vcl_string";  // name ending
  input_types_[2] = "vcl_string";  // local path

  // process has no output
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_fs_copy_files_to_local_process(bprb_func_process& pro)
{
  using namespace bhdfs_fs_copy_files_to_local_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  if (!bhdfs_manager::exists())
    return false;
  bhdfs_manager_sptr mins = bhdfs_manager::instance();

  //get the inputs
  unsigned i = 0;
  std::string hdfs_path= mins->get_working_dir() + "/" + pro.get_input<std::string>(i++);
  std::string name_ending= pro.get_input<std::string>(i++);
  std::string local_folder= pro.get_input<std::string>(i++);

  std::vector<std::string> dir_list;
  if (!mins->get_dir_list(hdfs_path, dir_list)) {
    std::cout << "In bhdfs_fs_copy_files_to_local_process() - cannot get dirlist!\n";
    return false;
  }
  bool ok = true;
  for (unsigned i = 0; i < dir_list.size(); i++) {
    std::string filename = dir_list[i];
    if (filename.find(name_ending) != std::string::npos)
      ok = ok && mins->copy_from_hdfs(filename, local_folder);
  }
  if (!ok) {
    std::cerr << "In bhdfs_fs_copy_files_to_local_process() - there were problems copying files!\n";
  }
  return true;
}


namespace bhdfs_fs_get_working_dir_process_globals
{
  constexpr unsigned n_inputs_ = 0;
  constexpr unsigned n_outputs_ = 1;
}

bool bhdfs_fs_get_working_dir_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_fs_get_working_dir_process_globals;

  //process takes no input
  std::vector<std::string> input_types_(n_inputs_);

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vcl_string";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_fs_get_working_dir_process(bprb_func_process& pro)
{
  using namespace bhdfs_fs_get_working_dir_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  if (!bhdfs_manager::exists())
    return false;

  bhdfs_manager_sptr mins = bhdfs_manager::instance();
  std::string hdfs_path= mins->get_working_dir();
  pro.set_output_val<std::string>(0, hdfs_path); //  fix the path names in stdin.txt file in the script s4
  return true;
}
