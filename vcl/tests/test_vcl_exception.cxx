
#define abort my_abort
extern "C" void my_abort();

#include <vcl_exception.h>

#include <vcl_iostream.h>

extern "C" void my_abort()
{
  vcl_cout << "abort()\n";
}

int main()
{
  vcl_try {
    vcl_throw "Bad something....\n";
  }
  vcl_catch (char* e) {
    vcl_cout << "caught " << e << vcl_endl;
  }
  vcl_catch_all {
    vcl_cout << "caught nuffink " << vcl_endl;
  }
}
