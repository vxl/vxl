#include <vcl_exception.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

int test_exception_main(int /*argc*/,char* /*argv*/[])
{
  const char *ex = "\"const char* exception\"";
  int result = 0;
#if VCL_HAS_EXCEPTIONS
  vcl_try {
    vcl_cout << "throw " << ex << vcl_endl;
    vcl_throw ex;
  }
  vcl_catch (const char* e) {
    vcl_cout << "caught " << e << ". Good." << vcl_endl;
  }
  vcl_catch_all {
    vcl_cout << "caught nuffink. Bad." << vcl_endl;
    result = 1;
  }
#else
  vcl_cout << "this compiler does not support exception handling\n";
#endif
  return result;
}
