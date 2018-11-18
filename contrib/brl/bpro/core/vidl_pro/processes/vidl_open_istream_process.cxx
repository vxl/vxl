// This is brl/bpro/core/vidl_pro/processes/vidl_open_istream_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <vul/vul_file.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_config.h>
#if VIDL_HAS_DSHOW
#include <vidl/vidl_dshow_file_istream.h>
#endif
#include <vidl/vidl_istream_sptr.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
bool vidl_open_istream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vidl_istream_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool vidl_open_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!=1) {
    std::cout << "vidl_open_ostream_process: The input number should be 1" << std::endl;
    return false;
  }
  //Retrieve filename from input
  std::string istream_filename = pro.get_input<std::string>(0);
  bool glob = false;
  if(vul_file::is_directory(istream_filename)) glob = true;
  else
    for(std::string::const_iterator cit = istream_filename.begin();
      cit != istream_filename.end()&&!glob; ++cit)
      if(*cit=='*')
        glob = true;
  vidl_istream_sptr vistr;
  //if glob open an image list istream
  if(glob)
    vistr = new vidl_image_list_istream(istream_filename);
  //otherwise if DSHOW is present, open a dshow stream
#if VIDL_HAS_DSHOW
  else{  vistr = new vidl_dshow_file_istream(istream_filename);}
#endif
  if (!vistr || !vistr->is_open()){
    std::cerr << "In vidl_open_istream_process::execute()"
             << " - could not open" << istream_filename << std::endl;
    return false;
  }

  pro.set_output_val<vidl_istream_sptr>(0, vistr);
  return true;
}
