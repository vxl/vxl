#include <vcl_new.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_cstddef.h> // for vcl_size_t
#include <vcl_iostream.h>

#include <vnl/vnl_test.h>

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_linear_operators_3.h>

bool verbose_malloc = false;
int malloc_count = 0;

// FIXME: Win32 will have different operator new in vnl dll from
// the one generated here, so this test fails - RWMC.
#ifndef WIN32
# define reset_count malloc_count = 0
# define check_count vnl_test_assert("mallocs", malloc_count <= 1)
#else
# define reset_count malloc_count = 0
# define check_count /* */
#endif

void test_matrix_fixed()
{
  verbose_malloc = true;
  double datablock[9] = {
    11, 12, 13,
    21, 22, 23,
    31, 32, 33,
  };

  vcl_cout << "Calling ctor -- should be no mallocs\n";
  //Refvnl_double_3x3 X(datablock);
  reset_count;
  vnl_double_3x3 X(datablock);
  check_count;
  vcl_cout << "X = [" << X << "]\n";

  reset_count;
  vnl_double_3 v(10,11,12);
  check_count;
  vcl_cout << "v = [ " << v << "]\n";
  
  reset_count;
  vnl_double_3 splork = X * (v + v);
  check_count;
  vcl_cout << "splork = " << splork << vcl_endl;
  
  // This shouldn't compile...
  // vnl_matrix<double>* base = new vnl_double_3x3(datablock);

  vcl_cout << "Now watch them mallocs\n";
  vnl_matrix<double>& CX = X;
  vnl_vector<double>& cv = v;
  vcl_cout << "X v = " << CX * (cv + cv) << vcl_endl;

  verbose_malloc = false;
}

// with gcc 3.0, formatted stream output uses operator
// new so printing to cout here causes stack overflow.

void* operator new(vcl_size_t s)
  // [18.4.1] lib.new.delete
#if defined(VCL_SUNPRO_CC_50) || defined(GNU_LIBSTDCXX_V3) || defined(VCL_KAI)
  throw (std::bad_alloc)
#endif
{
  void *r = vcl_malloc(s);

  ++malloc_count;

  if (verbose_malloc)
    vcl_printf("malloc: %08X for %d\n", unsigned(r), int(s));

  return r;
}

void operator delete(void* s)
#if defined(GNU_LIBSTDCXX_V3) || defined(VCL_SUNPRO_CC_50)
  throw ()
#endif
{
  if (verbose_malloc)
    vcl_printf("delete: %08X\n", unsigned(s));

  vcl_free(s);
}

TESTMAIN(test_matrix_fixed);
