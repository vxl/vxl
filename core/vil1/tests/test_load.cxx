#include <vcl/vcl_fstream.h>
#include <vcl/vcl_unlink.h>

#include <vil/vil_load.h>
#include <vil/vil_generic_image.h>

#define AssertEq(fred, x) {\
cout << "TEST [" << fred << "] == [" << x << "] : "; bool b = (fred) == (x); cout << (b?"PASSED":"FAILED") << endl; }

void test(char const* magic, int comps, int bits)
{
  char const* FNAME = "/tmp/t.pgm";
  {
    ofstream f(FNAME);
    f << magic << "\n2\n3\n255\nABCDEF";
  }
  vil_generic_image* i = vil_load(FNAME);

  cout <<
    "vil_generic_image: size " << i->width() << "x" << i->height() <<
    ", " << i->components() << " component" <<
    ", " << i->bits_per_component() << " bit" << 
    endl;

  AssertEq(i->components(), comps);
  AssertEq(i->bits_per_component(), bits);

  vcl_unlink(FNAME);
  
}  

int main(int argc, char **argv)
{
  test("P1", 1, 1);
  test("P2", 1, 8);
  test("P3", 3, 8);
  test("P4", 1, 1);
  test("P5", 1, 8);
  test("P6", 3, 8);
    
  return 0;
}
