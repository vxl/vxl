//This is brl/bpro/core/brad_pro/processes/brad_nitf_read_metadata_process.cxx
//:
// \file
//     Read the metadata for the given satellite image,
//       the metadata files are assumed to be in the same folder as the imagefile
//
//
//

#include <bprb/bprb_func_process.h>

#include <brad/brad_image_metadata.h>
#include <vul/vul_file.h>
//: set input and output types
bool brad_nitf_read_metadata_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); // image name
  input_types.emplace_back("vcl_string"); // meta folder if exists
  input_types.emplace_back("bool");       // option to print out extra information

  if (!pro.set_input_types(input_types))
    return false;

  // in case the 1st input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(1, idx);

  std::vector<std::string> output_types;
  output_types.emplace_back("brad_image_metadata_sptr");
  return pro.set_output_types(output_types);
}

bool brad_nitf_read_metadata_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << " The input number should be " << 1 << std::endl;
    return false;
  }

  //get the inputs
  std::string nitf_img_name = pro.get_input<std::string>(0);
  std::string meta_folder = pro.get_input<std::string>(1);
  bool verbose = pro.get_input<bool>(2);

  brad_image_metadata_sptr md = new brad_image_metadata;
  md->verbose_ = verbose;
  std::string ext = vul_file::extension(nitf_img_name);
  if (ext.compare(".NTF") == 0 || ext.compare(".ntf") == 0) {
    std::cout << "parse from metadata and image header: " << nitf_img_name << std::endl;
    if (!md->parse(nitf_img_name, meta_folder)) {
      std::cout<<"nitf metadata parsing failed\n"<<std::endl;
      return false;
    }
  }
  else {
    if (!md->parse_from_meta_file(nitf_img_name)) {
      std::cout << "nitf metadata parsing failed\n" << std::endl;
      return false;
    }
  }
  pro.set_output_val<brad_image_metadata_sptr>(0, md);
  return true;
}
