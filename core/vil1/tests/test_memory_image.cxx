#include <vcl/vcl_iostream.h>
#include <vcl/vcl_unistd.h>

#include <vil/vil_memory_image.h>
#include <vil/vil_save.h>





void save_writable(vil_image const &m, char const *file)
{
  // (try to) remove old file.
  vcl_unlink(file);
  // save.
  vil_save(m, file);
  // Make readable/writeable by all. Else the vil_save() will fail 
  // if the program is run later by another user on the same machine.
  vcl_chmod(file, 0666);
}

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

  save_writable(m, "/tmp/vil_test_memory_image.pgm");

  return 0;
}
