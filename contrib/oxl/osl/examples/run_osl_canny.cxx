/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vul_arg.h"//otherwise "unresolved typeinfo vul_arg_base"
#endif

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>

#include <osl/osl_easy_canny.h>
#include <osl/osl_save_topology.h>

// runs canny on the given input image and outputs
// the segmentation to the given output file.

int main(int argc, char **argv)
{
  vul_arg<int>        canny("-canny", "which canny? (0:oxford, 1:rothwell1, 2:rothwell2)", 0);
  vul_arg<vcl_string> in   ("-in", "input image", "");
  vul_arg<vcl_string> out  ("-out", "output file (default is stdout)", "");
  vul_arg_parse(argc, argv);

  vcl_string* in_file = new vcl_string(in());
  if (*in_file == "") {
    vcl_cout << "input image file: ";
    char tmp[1024];
    vcl_cin >> tmp;
    delete in_file;
    in_file = new vcl_string(tmp);
  }
  assert(*in_file != "");

  vil_image image = vil_load(in_file->c_str());
  if (!image)
    return 1;
  vcl_cerr << in_file << " : " << image << vcl_endl;

  vcl_list<osl_edge*> edges;
  osl_easy_canny(canny(), image, &edges);

  if (out() == "")
    osl_save_topology(vcl_cout, edges, vcl_list<osl_Vertex*>());
  else
    osl_save_topology(out().c_str(), edges, vcl_list<osl_Vertex*>());

  return 0;
}
