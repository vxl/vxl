// This is core/vil/examples/vil_print_section.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>    // atoi()

#include <vil/vil_load.h>
#include <vil/vil_image.h>

vcl_ostream &to_dec(vcl_ostream &os, unsigned char c)
{
  static char dig[]="0123456789";
  return os << dig[(c/100)%10] << dig[(c/10)%10] << dig[c%10];
}

vcl_ostream &to_oct(vcl_ostream &os, unsigned char c)
{
  static char dig[]="01234567";
  return os << dig[(c>>6)&7] << dig[(c>>3)&7] << dig[c&7];
}

vcl_ostream &to_hex(vcl_ostream &os, unsigned char c)
{
  static char dig[]="0123456789ABCDEF";
  return os << dig[(c & 0xF0)>>4] << dig[c & 0x0F];
}

// usage : vil_print_section image x0 y0 w h
int main(int argc, char **argv)
{
  assert(argc == 6);

  vil_image I = vil_load(argv[1]); assert(I);
  int x0 = vcl_atoi(argv[2]);
  int y0 = vcl_atoi(argv[3]);
  unsigned w = vcl_atoi(argv[4]);
  unsigned h = vcl_atoi(argv[5]);

  vcl_cerr << "image is " << I.width() << 'x' << I.height() << vcl_endl;
  assert(0<=x0 && 0<=y0 && x0+int(w)<=I.width() && y0+int(h)<=I.height());

  typedef unsigned char byte;
  if (I.planes()==1 && I.components()==3 && I.bits_per_component()==8)
  {
    vcl_vector<byte> buf(3*w*h);

    bool f = I.get_section(/* xxx */&buf[0], x0, y0, w, h);
    if (f)
    {
      vcl_cerr << "get_section succeeded.\n";
      vcl_cout << "red values:\n";
      for (unsigned r=0; r<h; ++r) {
        for (unsigned c=0; c<w; ++c)
          to_dec(vcl_cout, buf[3*(w*r+c)+0]) << ' ';
        vcl_cout << vcl_endl;
      }
      vcl_cout << "green values:\n";
      for (unsigned r=0; r<h; ++r) {
        for (unsigned c=0; c<w; ++c)
          to_dec(vcl_cout, buf[3*(w*r+c)+1]) << ' ';
        vcl_cout << vcl_endl;
      }
      vcl_cout << "blue values:\n";
      for (unsigned r=0; r<h; ++r) {
        for (unsigned c=0; c<w; ++c)
          to_dec(vcl_cout, buf[3*(w*r+c)+2]) << ' ';
        vcl_cout << vcl_endl;
      }
    }
    else
      vcl_cerr << "get_section failed\n";
  }
  else
    vcl_cerr << "what kind of image is this???\n";

  return 0;
}
