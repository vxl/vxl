// This is mul/vimt/algo/vimt_dummy.cxx
//:
// \file
// \author Ian Scott
// This file merely creates some code so that MSVC and other compilers will actually produce a library.

#ifdef _MSC_VER
char * vimt_algo_dummy = "dummy";
#endif

namespace vimt_algo_dummy
{

void __dummy()
{
  /*
   * At leaset 1 symbol is needed in every compilation unit
   * to avoid linking error that occurs in empty archive
   * files for some compilers
   */
   return;
}

}