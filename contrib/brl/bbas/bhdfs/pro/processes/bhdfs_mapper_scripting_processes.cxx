// This is brl/bbas/bhdfs/pro/processes/bhdfs_mapper_scripting_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  Processes to aid scripts that run map-reduce tasks
//
// \author Ozge C. Ozcanli
// \date Dec 14, 2011

#include <bhdfs/bhdfs_manager.h>
#include <bhdfs/bhdfs_fstream.h>
#include <bpro/core/bbas_pro/bbas_1d_array_string.h>
#include <vcl_sstream.h>


//: generate an input file on the output hdfs folder, this file will be streamed to mapper processes
// A vector of strings are input which will be used to form globs to retrieve file names
// the output file will have <name1> <name2> ... <name n> names on each line depending on this vector's size
namespace bhdfs_generate_stdin_file_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 0;
}

bool bhdfs_generate_stdin_file_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_generate_stdin_file_process_globals;

  //process takes 3 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";         // hdfs folder
  input_types_[1] = "bbas_1d_array_string_sptr"; //input images
  input_types_[2] = "vcl_string";         // path relative to current working dir and name of stdin.txt file on hdfs folder


  // process has no outputs:
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_generate_stdin_file_process(bprb_func_process& pro)
{
  using namespace bhdfs_generate_stdin_file_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  vcl_string hdfs_folder = pro.get_input<vcl_string>(i++);
  bbas_1d_array_string_sptr paths =pro.get_input<bbas_1d_array_string_sptr>(i++);
  vcl_string stdin_name = pro.get_input<vcl_string>(i++);

  if (!bhdfs_manager::exists())
    return false;

  bhdfs_manager_sptr mins = bhdfs_manager::instance();

  vcl_vector<vcl_string> dir_list;
  if (!mins->get_dir_list(hdfs_folder, dir_list)) {
    vcl_cout << "In bhdfs_generate_stdin_file_process() - cannot get dirlist!\n";
    return false;
  }
  unsigned glob_cnt = paths->data_array.size();
  if (!glob_cnt) {
    vcl_cout << "In bhdfs_generate_stdin_file_process() - data array size is zero!\n";
    return false;
  }
  vcl_vector<vcl_string> tmp;
  vcl_vector<vcl_vector<vcl_string> > lists(glob_cnt, tmp);
  // check if each file name is from any one of globs
  for (unsigned i = 0; i < dir_list.size(); i++) {
    vcl_string filename = dir_list[i];
    for (unsigned j = 0; j < glob_cnt; j++) {
      vcl_string fileending = paths->data_array[j];
      if (filename.find(fileending) != vcl_string::npos)
        lists[j].push_back(filename);
    }
  }
  unsigned ss = lists[0].size();
  if (!ss) {
    vcl_cout << "In bhdfs_generate_stdin_file_process() - no files with the given glob!\n";
    return false;
  }
  for (unsigned i = 1; i < lists.size(); i++) {
    if (lists[i].size() != ss) {
      vcl_cout << "In bhdfs_generate_stdin_file_process() - glob sizes do not match!\n";
      return false;
    }
  }

  // delete the file if it exists
  vcl_string stdin_fname = mins->get_working_dir() + "/" + stdin_name;
  if (mins->exists(stdin_fname))
    mins->rm(stdin_fname);

  // now create the file on hdfs
  bhdfs_fstream_sptr fs = new bhdfs_fstream(stdin_fname, "w");
  if (!fs->ok()) {
    vcl_cout << "In bhdfs_generate_stdin_file_process() - problems in opening file: " + stdin_fname + "!\n";
    return false;
  }
  for (unsigned j = 0; j < ss; j++) {
    vcl_stringstream stream;
    for (unsigned i = 0; i < lists.size(); i++)
      stream << lists[i][j] << ' ';
    stream << '\n';
    fs->write((stream.str()).c_str(), (stream.str()).length());
  }
  if (!fs->close()) {
    vcl_cout << "In bhdfs_generate_stdin_file_process() - problems closing the file: " + stdin_fname + "!\n";
    return false;
  }

  return true;
}

//: generate a text file with the contents as the given input string
// Write the file to the specified path
// this process is necessary because Python mapper cannot write to any path on hdfs
namespace bhdfs_generate_file_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool bhdfs_generate_file_process_cons(bprb_func_process& pro)
{
  using namespace bhdfs_generate_file_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";   // path relative to current working dir and name of stdin.txt file on hdfs folder
  input_types_[1] = "vcl_string";   // content of the file

  // process has 0 output:
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bhdfs_generate_file_process(bprb_func_process& pro)
{
  using namespace bhdfs_generate_file_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  vcl_string stdin_name = pro.get_input<vcl_string>(i++);
  vcl_string file_content = pro.get_input<vcl_string>(i++);

  if (!bhdfs_manager::exists())
    return false;

  bhdfs_manager_sptr mins = bhdfs_manager::instance();

  // delete the file if it exists
  vcl_string stdin_fname = mins->get_working_dir() + "/" + stdin_name;

  if (mins->exists(stdin_fname))
    mins->rm(stdin_fname);

  // now create the file on hdfs
  bhdfs_fstream_sptr fs = new bhdfs_fstream(stdin_fname, "w");
  if (!fs->ok()) {
    vcl_cout << "In bhdfs_generate_file_process() - problems in opening file: " + stdin_fname + "!\n";
    return false;
  }
  fs->write(file_content.c_str(), file_content.length());

  if (!fs->close()) {
    vcl_cout << "In bhdfs_generate_file_process() - problems closing the file: " + stdin_fname + "!\n";
    return false;
  }

  return true;
}

