#include "carmen_interface.h"
#include <string>
#include <vcl_compiler.h>
#include <iostream>
#include <iostream>

int main(int argc, char ** argv)
{
  std::string path = "./egemen1.cm";
  if (argc > 1) path = std::string(argv[1]);
  std::cout << "loading correspondence file " << path << std::endl;

  carmen_interface ci;
  ci.set_carmen_camera(1);
  ci.load_correspondence_file(path);
  ci.solve();
  ci.print_results();
  path += ".out1";
  ci.save_camera(path.c_str());

  return 0;
}
