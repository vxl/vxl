// This is brl/bpro/core/vidl_pro/processes/vidl_open_ostream_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vul/vul_file.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vidl/vidl_ostream_sptr.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool vidl_open_ostream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("bool");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vidl_ostream_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
// NOTE! currently only implemented for image list ostream
bool vidl_open_ostream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    std::cout << "vidl_open_ostream_process: The input number should be 2" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve filename from input
  std::string ostream_path = pro.get_input<std::string>(i++);

  //Retrieve create directory flag from input
  bool create_directory = pro.get_input<bool>(i++);;

  if (!vul_file::is_directory(ostream_path)) {
    //directory not present
    if (create_directory)
    {
      if (!vul_file::make_directory_path(ostream_path))
        {
        std::cerr << "In vidl_open_ostream_process::execute() - "
                 << " can't make directory with path " << ostream_path << '\n';
        return false;
        }
    }else{
      std::cerr << "In vidl_open_ostream_process::execute() - "
               << " directory doesn't exist with path " << ostream_path << '\n';
      return false;
    }
  }
  vidl_ostream_sptr vostr = new vidl_image_list_ostream(ostream_path);
  if (!vostr->is_open()){
    std::cerr << "In vidl_open_ostream_process::execute()"
             << " - could not open" << ostream_path << '\n';
    return false;
  }

  pro.set_output_val<vidl_ostream_sptr>(0, vostr);
  return true;
}
