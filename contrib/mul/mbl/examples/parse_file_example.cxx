//:
// \file
// \brief Example of reading information from a parameter file using mbl_read_props
// \author Tim Cootes/Sheng Su

#include <iostream>
#include <fstream>
#include <sstream>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_read_props.h>
#include <vul/vul_string.h>

void print_usage()
{
  std::cout<<"parse_file_example -p params.txt"<<std::endl;
  std::cout<<"Reads in parameters from named text file."<<std::endl;
  std::cout<<"Expects format: "<<std::endl;
  std::cout<<" image_dir: /home/myimages/ "<<std::endl;
  std::cout<<" image_names: { "<<std::endl;
  std::cout<<"    fish1.jpg"<<std::endl;
  std::cout<<"    fish2.jpg"<<std::endl;
  std::cout<<" }"<<std::endl;
  std::cout<<" int1: 17"<<std::endl;
  std::cout<<" double1: 35.24"<<std::endl;
}

struct parameter_data
{
  std::string image_dir;
  std::vector<std::string> image_names;
  int int1;
  double double1;
};

std::ostream& operator<<(std::ostream& os,
                        const parameter_data& p)
{
  os<<"image_dir: "<<p.image_dir<<std::endl;
  os<<"image_names: {"<<std::endl;
  for (const auto & image_name : p.image_names)
    os<<"  "<<image_name<<std::endl;
  os<<"}"<<std::endl;
  os<<"int1: "<<p.int1<<std::endl;
  os<<"double1: "<<p.double1<<std::endl;
  return os;
}

//: Reads in parameters from named file
bool parse_params(const std::string &param_path, parameter_data& params)
{
  std::ifstream ifs(param_path.c_str());
  if (!ifs)
  {
    std::cerr<<"Unable to open "<<param_path<<std::endl;
    return false;
  }

  // Cycle through string and produce a map of properties
  mbl_read_props_type props = mbl_read_props_ws(ifs);

  // Extract the properties
  if (props.find("image_dir")!=props.end())
  {
    params.image_dir = props["image_dir"];  props.erase("image_dir");
  }
  if (props.find("int1")!=props.end())
  {
    params.int1 = vul_string_atoi(props["int1"]);  props.erase("int1");
  }
  if (props.find("double1")!=props.end())
  {
    params.double1 = vul_string_atoi(props["double1"]);  props.erase("double1");
  }

  if (props.find("image_names")!=props.end())
  {
    std::istringstream in_ss(props["image_names"]);
    std::string tag;
    in_ss>>tag;
    params.image_names.resize(0);  // Empty data
    if (tag!="{")
    {
      std::cerr<<"Expected a {, got a "<<tag<<std::endl;
      return false;
    }
    while (!in_ss.eof()  && tag!= "}")
    {
      in_ss>>tag;
      if (tag!="}") params.image_names.push_back(tag);
        in_ss>>std::ws;
    }
    if (tag!="}")
    {
      std::cerr<<"Expected to find a }"<<std::endl;
      return false;
    }
    props.erase("image_names");
  }

   // Check for unused props
  mbl_read_props_look_for_unused_props("parse_file_example", props, mbl_read_props_type());
  return true;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter path");
  vul_arg_parse(argc, argv);

  if (param_path()=="")
  {
    print_usage();
    return 1;
  }

  // Object to store the parameters
  parameter_data params1;

  // Could make use of exceptions to catch parsing problems
  if (!parse_params(param_path(), params1))
  {
    std::cerr<<"Failed to load in parameters from "<<param_path()<<std::endl;
    return 1;
  }

  std::cout<<"Read parameters:"<<std::endl;
  std::cout<<params1<<std::endl;

  return 0;
}
