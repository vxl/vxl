// This is brl/bpro/core/vidl_pro/processes/vidl_open_ostream_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vul/vul_file.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vidl/vidl_ostream_sptr.h>
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>

//: Constructor
bool vidl_open_ostream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  input_types.push_back("bool");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vidl_ostream_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
// NOTE! currently only implemented for image list ostream
bool vidl_open_ostream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    vcl_cout << "vidl_open_ostream_process: The input number should be 2" << vcl_endl;
    return false;
  }

  unsigned i=0;
  //Retrieve filename from input
  vcl_string ostream_path = pro.get_input<vcl_string>(i++);

  //Retrieve create directory flag from input
  bool create_directory = pro.get_input<bool>(i++);;

  if (!vul_file::is_directory(ostream_path)) {
    //directory not present
    if (create_directory)
    {
      if (!vul_file::make_directory_path(ostream_path)) 
        {
        vcl_cerr << "In vidl_open_ostream_process::execute() - "
                 << " can't make directory with path " << ostream_path << '\n';
        return false;
        }
    }else{
      vcl_cerr << "In vidl_open_ostream_process::execute() - "
               << " directory doesn't exist with path " << ostream_path << '\n';
      return false;
    }
  }
  vidl_ostream_sptr vostr = new vidl_image_list_ostream(ostream_path);
  if (!vostr->is_open()){
    vcl_cerr << "In vidl_open_ostream_process::execute()"
             << " - could not open" << ostream_path << '\n';
    return false;
  }

  pro.set_output_val<vidl_ostream_sptr>(0, vostr);
  return true;
}

