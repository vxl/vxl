
// First define testmain

#include <vil/vil_test.h>
#undef TESTMAIN
#define TESTMAIN(x)


#include <vil/io/tests/test_memory_image_format_io.cxx>
#include <vil/io/tests/test_rgb_io.cxx>
#include <vil/io/tests/test_rgba_io.cxx>
#include <vil/io/tests/test_memory_image_impl_io.cxx>
#include <vil/io/tests/test_memory_image_of_io.cxx>
#include <vil/io/tests/golden_test_vil_io.cxx>

#undef TESTMAIN
#define TESTMAIN(x) \
int main() {vil_test_start(#x);x();return vil_test_summary();}

void run_test_vil_io()
{
  test_memory_image_format_double_io();  
  test_rgb_double_io();  
  test_rgba_double_io(); 
  test_memory_image_impl_io();  
  test_memory_image_of_io();
  golden_test_vil_io(false);
}


TESTMAIN(run_test_vil_io);
