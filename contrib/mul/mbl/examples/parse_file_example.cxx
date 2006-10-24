//:
// \file
// \brief Example of reading information from a parameter file using mbl_read_props
// \author Tim Cootes/Sheng Su

#include <vul/vul_arg.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <mbl/mbl_read_props.h>
#include <vcl_sstream.h>
#include <vul/vul_string.h>

void print_usage()
{
  vcl_cout<<"parse_file_example -p params.txt"<<vcl_endl;
  vcl_cout<<"Reads in parameters from named text file."<<vcl_endl;
  vcl_cout<<"Expects format: "<<vcl_endl;
  vcl_cout<<" image_dir: /home/myimages/ "<<vcl_endl;
  vcl_cout<<" image_names: { "<<vcl_endl;
  vcl_cout<<"    fish1.jpg"<<vcl_endl;
  vcl_cout<<"    fish2.jpg"<<vcl_endl;
  vcl_cout<<" }"<<vcl_endl;
  vcl_cout<<" int1: 17"<<vcl_endl;
  vcl_cout<<" double1: 35.24"<<vcl_endl;
}

struct parameter_data 
{
  vcl_string image_dir;
  vcl_vector<vcl_string> image_names; 
  int int1;
  double double1;
};

vcl_ostream& operator<<(vcl_ostream& os,
                        const parameter_data& p)
{
  os<<"image_dir: "<<p.image_dir<<vcl_endl;
  os<<"image_names: {"<<vcl_endl;
  for (unsigned i=0;i<p.image_names.size();++i) 
    os<<"  "<<p.image_names[i]<<vcl_endl;
  os<<"}"<<vcl_endl;
  os<<"int1: "<<p.int1<<vcl_endl;
  os<<"double1: "<<p.double1<<vcl_endl;
  return os;
}

//: Reads in parameters from named file
bool parse_params(const vcl_string &param_path, parameter_data& params)
{
  vcl_ifstream ifs(param_path.c_str());
  if (!ifs)
  {
    vcl_cerr<<"Unable to open "<<param_path<<vcl_endl;
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
    vcl_istringstream in_ss(props["image_names"]);
    vcl_string tag;
    in_ss>>tag;
    params.image_names.resize(0);  // Empty data
    if (tag!="{") 
    {
      vcl_cerr<<"Expected a {, got a "<<tag<<vcl_endl;
      return false;
    }
    while (!in_ss.eof()  && tag!= "}")
    {
      in_ss>>tag;
      if (tag!="}") params.image_names.push_back(tag);
        in_ss>>vcl_ws;
    }
    if (tag!="}")
    {
      vcl_cerr<<"Expected to find a }"<<vcl_endl;
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
  vul_arg<vcl_string> param_path("-p","Parameter path");
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
    vcl_cerr<<"Failed to load in parameters from "<<param_path()<<vcl_endl;
    return 1;
  }

  vcl_cout<<"Read parameters:"<<vcl_endl;
  vcl_cout<<params1<<vcl_endl;

  return 0;
}
