#include <vcl/vcl_iostream.h>

#include <vil/vil_memory_image.h>
#include <vil/vil_save.h>

void p(vil_image& m)
{
  for(int y = 0; y < m.height(); ++y) {
    for(int x = 0; x < m.width(); ++x) {
      unsigned char val = 0;
      m.get_section(&val, x, y, 1, 1);
      cout << "\t" << (int)val;
    }
    cout << endl;
  }
}

int main()
{
  vil_memory_image m(3, 2, VIL_BYTE);
  
  unsigned char data[] = {
    1, 2, 3,
    4, 5, 6
  };
  m.put_section(data, 0, 0, 3, 2);
  p(m);

  vil_save(m, "/tmp/vil_test_memory_image.pgm");

  return 0;
}
