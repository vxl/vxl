#include "carmen_interface.h"
#include <vcl_string.h>
#include <vcl_iostream.h>

int main(int argc, char ** argv)
{
  vcl_string path = "./egemen1.cm";
  if (argc > 1) path = vcl_string(argv[1]);
  vcl_cout << "loading correspondence file " << path << vcl_endl;

  carmen_interface ci;
  ci.set_carmen_camera(1);
  ci.load_correspondence_file(path);
  ci.solve();
  ci.print_results();
  path += ".out1";
  ci.save_camera(path.c_str());

  return 0;
}
