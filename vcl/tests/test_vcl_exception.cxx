
#define abort my_abort
void my_abort();

#include <vcl/vcl_exception.h>

#include <vcl/vcl_iostream.h>

void my_abort()
{
  cerr << "abort()\n";
}

main()
{
  vcl_try {
    vcl_throw "Bad something....\n";
  }
  vcl_catch (char* e) {
    cerr << "caught " << e << endl;
  }
  vcl_catch_all {
    cerr << "caught nuffink " << endl;
  }
}
