#include <vcl_iostream.h>

#include <vpl/vpl_unistd.h>

#include <vil/vil_memory_image_of.h>
#include <vil/vil_save.h>
#include <vil/vil_rgb.h>

const int W = 768;
const int H = 256;

void save_writable(vil_image const &m, char const *file) 
{
  // (try to) remove old file.
  vpl_unlink(file);
  // save.
  vil_save(m, file);
  // Make readable/writable by all. Else the vil_save() will fail 
  // if the program is run later by another user on the same machine.
  vpl_chmod(file, 0666);
}

int main()
{
  {
    vcl_cout << "unsigned char" << vcl_endl;
    vil_memory_image_of<unsigned char> image(W,H);
    
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x) {
	image(x,y) = ((x - W/2) * (y - H/2) / 16) % 256;
      }
    
    save_writable(image, "/tmp/vil_test_memory_image_of.pgm");
  }
  
  {
    vcl_cout << "vil_rgb_byte" << vcl_endl;
    vil_memory_image_of<vil_rgb<unsigned char> > image(W,H);
    
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x) {
	vil_rgb<unsigned char>& p = image(x,y);
	p.r = x;
	p.g = ((x - W/2) * (y - H/2) / 16) % 256;
	p.b = y/3;
      }
    
    save_writable(image, "/tmp/vil_test_memory_image_of.ppm");
  }
  
  {
    vcl_cout << "bool" << vcl_endl;
    vil_memory_image_of<bool> map(451, 349);
    for (unsigned x=0; x<map.width(); ++x)
      for (unsigned y=0; y<map.height(); ++y)
	map(x, y) = false;
  }

  return 0;
}
