//:
// \file
//  Test of the vipl_erode_disk templated IP classes
//  vipl_erode_disk<vil_image,vil_image,T,T>
//  for T in { unsigned char, unsigned short, float, RGB }.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   12 sept. 2000
//
#include <vil/vil_memory_image_of.h>
//#include <vil/vil_rgb_byte.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_erode_disk.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
typedef unsigned char ubyte;

// create an 8 bit test image
vil_image CreateTest8bitImage(int wd, int ht)
{
  vil_memory_image_of<unsigned char> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image.put_section(&data, x, y, 1, 1);
    }
  return image;
}

// create a 16 bit test image
vil_image CreateTest16bitImage(int wd, int ht)
{
  vil_memory_image_of<unsigned short> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned short data = ((x-wd/2)*(y-ht/2)/16) % (1<<16);
      image.put_section(&data, x, y, 1, 1);
  }
  return image;
}

#if 0 // no colour erosion for the moment
// create a 24 bit color test image
vil_image CreateTest24bitImage(int wd, int ht)
{
  vil_memory_image_of<vil_rgb_byte> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image.put_section(data, x, y, 1, 1);
    }
  return image;
}
#endif

// create a 24 bit color test image, with 3 planes
vil_image CreateTest3planeImage(int wd, int ht)
{
  vil_memory_image image(3, wd, ht, 1, 8, VIL_COMPONENT_FORMAT_UNSIGNED_INT);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image.put_section(data, x, y, 1, 1);
    }
  return image;
}


// create a float-pixel test image
vil_image CreateTestfloatImage(int wd, int ht)
{
  vil_memory_image_of<float> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      float data = 0.01 * ((x-wd/2)*(y-ht/2)/16);
      image.put_section(&data, x, y, 1, 1);
    }
  return image;
}

static bool difference(vil_image const& a, vil_image const& b, int v, const char* m=0, const char* m2=0)
{
  int sx = a.width(),  sy = a.height();
  if (sx != b.width() || sy != b.height())
    {if (m2) vcl_cout<<m<<m2<<" test FAILED: images are different size\n"; return true;}
  if (a.planes() != b.planes() || a.components() != b.components())
    {if (m2) vcl_cout<<m<<m2<<" test FAILED: images have different # planes/components\n"; return true;}
  if (a.component_format()   != b.component_format() ||
      a.bits_per_component() != b.bits_per_component())
    {if (m2) vcl_cout<<m<<m2<<" test FAILED: images are different format\n";return true;}

  int ret = 0;
  // run over all pixels except for an outer border of 1 pixel:
  int siz = (sx-2)*(sy-2)*a.planes()*a.components()*(a.bits_per_component()/8);
  char* v1 = new char[siz]; a.get_section(v1,1,1,sx-2,sy-2);
  char* v2 = new char[siz]; b.get_section(v2,1,1,sx-2,sy-2);
  for (int i=0; i<siz; ++i) {
    int d;
#define DIFF(T) d=((int)(((T*)(v1))[i]))-((int)(((T*)(v2))[i])); if (d<0) d = -d
    if (a.component_format() == VIL_COMPONENT_FORMAT_IEEE_FLOAT)
      switch (a.bits_per_component()) {
        case sizeof(float): DIFF(float); break;
        case sizeof(double): DIFF(double); break;
        default: d = 0;
      }
    else
      switch (a.bits_per_component()) {
        case 1: DIFF(unsigned char); break;
        case 2: DIFF(unsigned short); break;
        case 3: DIFF(unsigned char); break;
        case 4: DIFF(int); break;
        default: d = 0;
      }
    ret += d;
  }
  delete[] v1; delete[] v2;
  ret /= a.planes()*a.components();
  if (vcl_abs(ret - v) > 0.01*vcl_abs(v))
  { if (m2) vcl_cout<<m<<m2<<" test FAILED: "<<ret<<" instead of "<<v<<vcl_endl;return true;}
  else { if (m2) vcl_cout<<m<<m2<<" test PASSED\n"; return false; }
}

#define TEST(i,r,d,T,v,m,m2) { \
  vipl_erode_disk<vil_image,vil_image,T,T> op(5); \
  op.put_in_data_ptr(&r); op.put_out_data_ptr(&i); op.filter(); \
  difference(i,r,v,m,m2); \
  if (difference(i,r,0)) vcl_cout<<m<<m2<<" test FAILED: input image changed!\n"; }
#define ALL_TESTS(d,v,m) \
  TEST(byte_img,byte_ori,d,unsigned char,v,m,"_byte"); \
  TEST(shrt_img,shrt_ori,d,unsigned short,v,m,"_short"); \
/*TEST(flot_img,flot_ori,d,float,v,m,"_float"); */ \
/*TEST(colr_img,colr_ori,d,vil_rgb_byte,v,m,"_colour") */

int main() {
  vil_image byte_img = CreateTest8bitImage(32,32),  byte_ori = CreateTest8bitImage(32,32);
  vil_image shrt_img = CreateTest16bitImage(32,32), shrt_ori = CreateTest16bitImage(32,32);
  vil_image flot_img = CreateTestfloatImage(32,32), flot_ori = CreateTestfloatImage(32,32);
//vil_image colr_img = CreateTest24bitImage(32,32), colr_ori = CreateTest24bitImage(32,32);
  vil_image colr_img = CreateTest3planeImage(32,32),colr_ori = CreateTest3planeImage(32,32);
  vil_memory_image_of<unsigned char> byte_out(32, 32);
  vil_memory_image_of<unsigned short> shrt_out(32, 32);
  vil_memory_image_of<float> flot_out(32, 32);
//vil_memory_image_of<vil_rgb_byte> colr_out(32, 32);

  ALL_TESTS(0,0,"vipl_erode_disk");
  return 0;
}
