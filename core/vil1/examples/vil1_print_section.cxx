#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstdlib.h>    // atoi()

#include <vil/vil_load.h>
#include <vil/vil_image.h>
#include <vil/vil_buffer.h>

ostream &dec(ostream &os, unsigned char c) {
  static char dig[]="0123456789";
  return os << dig[(c/100)%10] << dig[(c/10)%10] << dig[c%10];
}

ostream &hex(ostream &os, unsigned char c) {
  static char dig[]="0123456789ABCDEF";
  return os << dig[(c & 0xF0)>>4] << dig[c & 0x0F];
}

// usage : vil_print_section image x0 y0 w h
int main(int argc, char **argv) {
  assert(argc == 6);

  vil_image I = vil_load(argv[1]); assert(I);
  int x0 = atoi(argv[2]);
  int y0 = atoi(argv[3]);
  unsigned w = atoi(argv[4]);
  unsigned h = atoi(argv[5]);
  
  cerr << "image is " << I.width() << 'x' << I.height() << endl;
  assert(0<=x0 && 0<=y0 && x0+w<=I.width() && y0+h<=I.height());
  
  typedef unsigned char byte;
  if (I.planes()==1 && I.components()==3 && I.bits_per_component()==8) {
    vil_buffer<byte> buf(3*w*h);
    
    bool f = I.get_section(buf.data(), x0, y0, w, h);
    if (f) {
      cerr << "get_section succeeded." << endl;
      cout << "red values:" << endl;
      for (unsigned r=0; r<h; ++r) {
	for (unsigned c=0; c<w; ++c)
	  dec(cout, buf[3*(w*r+c)+0]) << ' ';
	cout << endl;
      }
      cout << "green values:" << endl;
      for (unsigned r=0; r<h; ++r) {
	for (unsigned c=0; c<w; ++c)
	  dec(cout, buf[3*(w*r+c)+1]) << ' ';
	cout << endl;
      }
      cout << "blue values:" << endl;
      for (unsigned r=0; r<h; ++r) {
	for (unsigned c=0; c<w; ++c)
	  dec(cout, buf[3*(w*r+c)+2]) << ' ';
	cout << endl;
      }
    }
    else {
      cerr << "get_section failed" << endl;
    }
  }
  else
    cerr << "what kind of image is this???" << endl;
  
  return 0;
}
