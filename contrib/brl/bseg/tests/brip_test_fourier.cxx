// This is brl/bseg/tests/brip_test_fourier.cxx
#include <vil1/vil1_memory_image_of.h>
#ifdef DEBUG
#include <vil1/vil1_save.h>
#endif
#include <brip/brip_float_ops.h>
#include <testlib/testlib_test.h>

static void brip_test_fourier()
{
  vil1_memory_image_of<float> input1, input2, mag, phase, output;
  int w = 128, h = 128;
  input1.resize(w,h);
  // diagonal periodic stripes
  int period = 5;
  for (int y = 0; y< h; y++)
    for (int x = 0; x<w; x++)
      input1(x,y)=0;

  for (int p = 0; p<h; p+=period)
    for (int y = 0; y< h; y++)
      for (int x = 0; x<w; x++)
        if (x==y+p)
          input1(x,y)=50;

  // square at origin
  w = 128, h=128;
  input2.resize(w, h);
  for (int y = 0; y< h; y++)
      for (int x = 0; x<w; x++)
        input2(x,y) = (int)50/(0.002*(x*x + y*y) +1.0f);

  vil1_memory_image_of<unsigned char> char_in, char_mag, char_phase, char_output;
  brip_float_ops::fourier_transform(input1, mag, phase);
  brip_float_ops::inverse_fourier_transform(mag, phase, output);

  char_in = brip_float_ops::convert_to_byte(input1);
  char_mag = brip_float_ops::convert_to_byte(mag);
  char_phase = brip_float_ops::convert_to_byte(phase);
  char_output = brip_float_ops::convert_to_byte(output);

#ifdef DEBUG
  vil1_save(char_in, "./input.tif");
  vil1_save(char_mag, "./mag.tif");
  vil1_save(char_phase, "./phase.tif");
  vil1_save(char_output, "./output.tif");
#endif
}

TESTMAIN(brip_test_fourier);
