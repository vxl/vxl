// This is brl/bpro/core/vidl_pro/processes/vidl_open_istream_process.cxx
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
#include <vcl_iostream.h>

//: Constructor
bool vidl_open_istream_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vidl_istream_sptr");  
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool vidl_open_istream_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!=1) {
    vcl_cout << "vidl_open_ostream_process: The input number should be 1" << vcl_endl;
    return false;
  }
  //Retrieve filename from input
  vcl_string istream_filename = pro.get_input<vcl_string>(0);
  bool glob = false;
  if(vul_file::is_directory(istream_filename)) glob = true;
  else
    for(vcl_string::const_iterator cit = istream_filename.begin(); 
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
    vcl_cerr << "In vidl_open_istream_process::execute()"
             << " - could not open" << istream_filename << vcl_endl;
    return false;
  }

  pro.set_output_val<vidl_istream_sptr>(0, vistr);
  return true;
}

