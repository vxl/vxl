#include <vcl_fstream.h>

#include <vpl/vpl_unistd.h>

#include <vil/vil_load.h>
#include <vil/vil_image_impl.h>

#define AssertEq(fred, x) {\
vcl_cout << "TEST [" << fred << "] == [" << x << "] : "; bool b = (fred) == (x); vcl_cout << (b?"PASSED":"FAILED") << vcl_endl; }

void test(char const* magic, int comps, int bits)
{
  char const* FNAME = "/tmp/t.pgm";
  {
    ofstream f(FNAME);
    // reference to rvalue not allowed.
    f << magic << "\n2\n3\n255\nABCDEF";
  }

  vil_image i = vil_load(FNAME);
  
  vcl_cout <<
    "vil_image_impl: size " << i.width() << "x" << i.height() <<
    ", " << i.components() << " component" <<
    ", " << i.bits_per_component() << " bit" << 
    endl;

  AssertEq(i.components(), comps);
  AssertEq(i.bits_per_component(), bits);

  vpl_unlink(FNAME);
  
}  

int main(int , char **)
{
  test("P1", 1, 1);
  test("P2", 1, 8);
  test("P3", 3, 8);
  test("P4", 1, 1);
  test("P5", 1, 8);
  test("P6", 3, 8);
    
  return 0;
}
