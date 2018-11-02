/*
  fsm
*/
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_expand_path.h>

int main(int argc, char **argv)
{
  for (int i=1; i<argc; ++i)
    std::cout << '\'' << argv[i] << "\' => \'" << vul_expand_path(argv[i]) << '\'' << std::endl;

  return 0;
}
