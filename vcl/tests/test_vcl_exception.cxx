#include <vcl_exception.h>
#include <vcl_iostream.h>

extern "C" void abort() { vcl_cout << "abort()\n"; }

int main()
{
  int result;
  vcl_try {
    vcl_throw "\"const char* exception\"";
  }
  vcl_catch (const char* e) {
    vcl_cout << "caught " << e << ". Good." << vcl_endl;
    result = 0;
  }
  vcl_catch_all {
    vcl_cout << "caught nuffink. Bad. " << vcl_endl;
    result = 1;
  }
  return result;
}
