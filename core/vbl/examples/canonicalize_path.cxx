/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_iostream.h>
#include <vbl/vbl_canonical_path.h>

int main(int argc, char **argv)
{
  for (int i=1; i<argc; ++i)
    cout << '\'' << argv[i] << "\' => \'" << vbl_canonical_path(argv[i]) << '\'' << endl;

  return 0;
}
