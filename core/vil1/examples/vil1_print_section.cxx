// This is core/vil1/examples/vil1_print_section.cxx
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_load.h>
#include <vil1/vil1_image.h>

std::ostream &to_dec(std::ostream &os, unsigned char c)
{
  static char dig[]="0123456789";
  return os << dig[(c/100)%10] << dig[(c/10)%10] << dig[c%10];
}

std::ostream &to_oct(std::ostream &os, unsigned char c)
{
  static char dig[]="01234567";
  return os << dig[(c>>6)&7] << dig[(c>>3)&7] << dig[c&7];
}

std::ostream &to_hex(std::ostream &os, unsigned char c)
{
  static char dig[]="0123456789ABCDEF";
  return os << dig[(c & 0xF0)>>4] << dig[c & 0x0F];
}

// usage : vil1_print_section image x0 y0 w h
int main(int argc, char **argv)
{
  assert(argc == 6);

  vil1_image I = vil1_load(argv[1]); assert(I);
  int x0 = std::atoi(argv[2]);
  int y0 = std::atoi(argv[3]);
  unsigned w = std::atoi(argv[4]);
  unsigned h = std::atoi(argv[5]);

  std::cerr << "image is " << I.width() << 'x' << I.height() << std::endl;
  assert(0<=x0 && 0<=y0 && x0+int(w)<=I.width() && y0+int(h)<=I.height());

  typedef unsigned char byte;
  if (I.planes()==1 && I.components()==3 && I.bits_per_component()==8)
  {
    std::vector<byte> buf(3*w*h);

    bool f = I.get_section(/* xxx */&buf[0], x0, y0, w, h);
    if (f)
    {
      std::cerr << "get_section succeeded.\n";
      std::cout << "red values:\n";
      for (unsigned r=0; r<h; ++r) {
        for (unsigned c=0; c<w; ++c)
          to_dec(std::cout, buf[3*(w*r+c)+0]) << ' ';
        std::cout << std::endl;
      }
      std::cout << "green values:\n";
      for (unsigned r=0; r<h; ++r) {
        for (unsigned c=0; c<w; ++c)
          to_dec(std::cout, buf[3*(w*r+c)+1]) << ' ';
        std::cout << std::endl;
      }
      std::cout << "blue values:\n";
      for (unsigned r=0; r<h; ++r) {
        for (unsigned c=0; c<w; ++c)
          to_dec(std::cout, buf[3*(w*r+c)+2]) << ' ';
        std::cout << std::endl;
      }
    }
    else
      std::cerr << "get_section failed\n";
  }
  else
    std::cerr << "what kind of image is this???\n";

  return 0;
}
