#include <vcl_iostream.h>

#include <vpl/vpl_unistd.h>
#include <vcl_cstdio.h> /* for tempnam() */

#include <vil/vil_memory_image.h>
#include <vil/vil_save.h>

void p(vil_image const& m)
{
  for(int y = 0; y < m.height(); ++y) {
    for(int x = 0; x < m.width(); ++x) {
      unsigned char val = 0;
      m.get_section(&val, x, y, 1, 1);
      vcl_cout << "\t" << (int)val;
    }
    vcl_cout << vcl_endl;
  }
}

int main()
{
  char const* TMPNAM1 = tempnam(0,"vil_test_memory_image_1.pgm");
  char const* PGMFILE1 = TMPNAM1 ? TMPNAM1 : "/tmp/vil_test_memory_image_1.pgm";
  char const* TMPNAM2 = tempnam(0,"vil_test_memory_image_2.pgm");
  char const* PGMFILE2 = TMPNAM2 ? TMPNAM2 : "/tmp/vil_test_memory_image_2.pgm";

  {
    vil_memory_image m(3, 2, VIL_BYTE);

    unsigned char data[] = {
      1, 2, 3,
      4, 5, 6
    };
    m.put_section(data, 0, 0, 3, 2);
    p(m);

    vil_save(m, PGMFILE1);

    vil_memory_image n(data, 3, 2, VIL_BYTE);
    p(n);

    vil_save(n, PGMFILE2);
  }

  // don't leave garbage behind by default.
  vpl_unlink(PGMFILE1);
  vpl_unlink(PGMFILE2);

  return 0;
}
