

#include <vil/vil_memory_image_of.h>
#include <vil/vil_save.h>
#include <vil/vil_rgb_byte.h>

const int W = 768;
const int H = 256;

int main()
{
  {
    cout << "unsigned char" << endl;
    vil_memory_image_of<unsigned char> image(W,H);
    
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x) {
	image(x,y) = ((x - W/2) * (y - H/2) / 16) % 256;
      }
    
    vil_save(&image, "/tmp/vil_test_memory_image_of.pgm", "pnm");
  }
  
  {
    cout << "vil_rgb_byte" << endl;
    vil_memory_image_of<vil_rgb_byte> image(W,H);
    
    for(int y = 0; y < image.height(); ++y)
      for(int x = 0; x < image.width(); ++x) {
	vil_rgb_byte& p = image(x,y);
	p.r = x;
	p.g = ((x - W/2) * (y - H/2) / 16) % 256;
	p.b = y/3;
      }
    
    vil_save(&image, "/tmp/vil_test_memory_image_of.ppm", "pnm");
  }
  
  {
    cout << "bool" << endl;
    vil_memory_image_of<bool> map(451, 349);
    for (unsigned x=0; x<map.width(); ++x)
      for (unsigned y=0; y<map.height(); ++y)
	map(x, y) = false;
  }

  return 0;
}
