#include <vcl_exception.h>
#include <vcl_iostream.h>

int test_vcl_exception_main()
{
  const char *ex = "\"const char* exception\"";
  int result = 0;
#if VCL_HAS_EXCEPTIONS
  vcl_try {
    vcl_cout << "throw " << ex << vcl_endl;
    vcl_throw ex;
  }
  vcl_catch (const char* e) {
    vcl_cout << "caught " << e << ".  Good.\n";
    result = 0;
  }
  vcl_catch_all {
    vcl_cout << "caught nothing.  Bad.\n";
    result = 1;
  }
#else
  vcl_cout << "this compiler does not support exception handling\n";
#endif
  return result;
}
