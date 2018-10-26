// This is vcl/tests/test_exception.cxx
#include <vcl_compiler.h>
#include <iostream>
#include <exception>
#include <iostream>

int test_exception_main(int /*argc*/,char* /*argv*/[])
{
#if VCL_HAS_EXCEPTIONS
  const char *ex = "\"const char* exception\"";
  int result;
  try {
    std::cout << "throw " << ex << std::endl;
    throw ex;
  }
  catch (const char* e) {
    std::cout << "caught " << e << ".  Good." << std::endl;
    result = 0;
  }
  catch(...) {
    std::cout << "caught nothing.  Bad." << std::endl;
    result = 1;
  }
  return result;
#else
  std::cout << "this compiler does not support exception handling\n";
  return 0;
#endif
}
