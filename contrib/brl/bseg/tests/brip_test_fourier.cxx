// This is brl/bseg/tests/brip_test_fourier.cxx
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_math.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_save.h>
#include <vsol/vsol_point_2d.h>
#include <brip/brip_float_ops.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

bool near_eq(double x, double y){return vcl_fabs(x-y)<0.1;}

int main(int argc, char * argv[])
{
   
  int success=0, failures=0;
  vil1_memory_image_of<float> input1, input2, mag, phase, output;
  int w = 128, h = 128;
  input1.resize(w,h);
  // diagonal periodic stripes
  int period = 5;
  for(int y = 0; y< h; y++)
    for(int x = 0; x<w; x++)
      input1(x,y)=0;

  for(int p = 0; p<h; p+=period)
    for(int y = 0; y< h; y++)
      for(int x = 0; x<w; x++)
        if(x==y+p)
          input1(x,y)=50;

  // square at origin
  w = 128, h=128;
  input2.resize(w, h);
  int e = 16;//edge width
  for(int y = 0; y< h; y++)
      for(int x = 0; x<w; x++)
        input2(x,y) = (int)50/(0.002*(x*x + y*y) +1.0f);

  vil1_memory_image_of<unsigned char> char_in, char_mag, char_phase, char_output;
  brip_float_ops::fourier_transform(input1, mag, phase);
  brip_float_ops::inverse_fourier_transform(mag, phase, output);

  char_in = brip_float_ops::convert_to_byte(input1);
  char_mag = brip_float_ops::convert_to_byte(mag);
  char_phase = brip_float_ops::convert_to_byte(phase);
  char_output = brip_float_ops::convert_to_byte(output);

//   vil1_save(char_in, "./input.tif", "tiff");
//   vil1_save(char_mag, "./mag.tif", "tiff");
//   vil1_save(char_phase, "./phase.tif", "tiff");
//   vil1_save(char_output, "./output.tif", "tiff");
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
