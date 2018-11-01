// This is oxl/osl/examples/run_osl_canny.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vil1/vil1_load.h>

#include <osl/osl_easy_canny.h>
#include <osl/osl_save_topology.h>

// runs canny on the given input image and outputs
// the segmentation to the given output file.

int main(int argc, char **argv)
{
  vul_arg<int>        canny("-canny", "which canny? (0:oxford, 1:rothwell1, 2:rothwell2)", 0);
  vul_arg<std::string> in   ("-in", "input image", "");
  vul_arg<std::string> out  ("-out", "output file (default is stdout)", "");
  vul_arg_parse(argc, argv);

  auto* in_file = new std::string(in());
  if (*in_file == "") {
    std::cout << "input image file: ";
    char tmp[1024];
    std::cin >> tmp;
    delete in_file;
    in_file = new std::string(tmp);
  }
  assert(*in_file != "");

  vil1_image image = vil1_load(in_file->c_str());
  if (!image)
    return 1;
  std::cerr << in_file << " : " << image << std::endl;

  std::list<osl_edge*> edges;
  osl_easy_canny(canny(), image, &edges);

  if (out() == "")
    osl_save_topology(std::cout, edges, std::list<osl_Vertex*>());
  else
    osl_save_topology(out().c_str(), edges, std::list<osl_Vertex*>());

  return 0;
}
