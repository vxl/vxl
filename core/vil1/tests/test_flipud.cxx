#include <vcl_iostream.h>

#include <vil/vil_memory_image.h>
#include <vil/vil_flipud.h>

#include <testlib/testlib_test.h>

static
void p(vil_image const& m)
{
  for(int y = 0; y < m.height(); ++y) {
    for(int x = 0; x < m.width(); ++x) {
      unsigned char val = 0;
      m.get_section(&val, x, y, 1, 1);
      vcl_cout << "\t" << (int)val;
    }
    vcl_cout << vcl_endl << vcl_endl;
  }
}

void test_flipud()
{
  vil_memory_image m(3, 2, VIL_BYTE);

  unsigned char data[] = {
    1, 2, 3,
    4, 5, 6
  };
  unsigned char flipped_data[] = {
    4, 5, 6,
    1, 2, 3
  };

  m.put_section(data, 0, 0, 3, 2);
  p(m);

  vil_image flip = vil_flipud(m);
  p(flip);

  int i;
  for (i =0; i < 6; i++)
  {
    unsigned char val=0;
    flip.get_section(&val, i%3, i / 3, 1, 1);
    if (flipped_data[i] != val)  break;
  }

  TEST ("small get_section shows flipped data", i, 6);
  unsigned char data2[6];
  flip.get_section(data2, 0, 0, 3, 2);

  for (i =0; i < 6; i++)
    if (flipped_data[i] != data2[i])  break;

  TEST ("full get_section shows flipped data", i, 6);
}

MAIN( test_flipud )
{
  RUN_TEST_FUNC(test_flipud);
}
