/*
  fsm
*/
#include <vcl_iostream.h>
#include <vul/vul_expand_path.h>

int main(int argc, char **argv)
{
  for (int i=1; i<argc; ++i)
    vcl_cout << '\'' << argv[i] << "\' => \'" << vul_expand_path(argv[i]) << '\'' << vcl_endl;

  return 0;
}
