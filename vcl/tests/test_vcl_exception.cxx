#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

int test_vcl_exception_main()
{
  int result = 0;
#if VCL_HAS_EXCEPTIONS
  vcl_try {
    vcl_throw "\"const char* exception\"";
  }
  vcl_catch (const char* e) {
    vcl_cout << "caught " << e << ". Good.\n";
  }
  vcl_catch_all {
    vcl_cout << "caught nuffink. Bad.\n";
    result = 1;
  }
#else
  vcl_cout << "this compiler does not support exception handling\n";
#endif
  return result;
}
