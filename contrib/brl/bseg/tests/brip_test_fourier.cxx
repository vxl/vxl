// This is brl/bseg/tests/brip_test_fourier.cxx
#include <vil1/vil1_memory_image_of.h>
#ifdef DEBUG
#include <vil1/vil1_save.h>
#endif
#include <brip/brip_float_ops.h>
#include <testlib/testlib_test.h>

static void brip_test_fourier()
{
  const int w = 128, h = 128;
  vil1_memory_image_of<float> input1(w,h);
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

  vil1_memory_image_of<float> mag, phase, output;
  brip_float_ops::fourier_transform(input1, mag, phase);
  brip_float_ops::inverse_fourier_transform(mag, phase, output);

  // square at origin
  vil1_memory_image_of<float> input2(w, h);
  for (int y = 0; y< h; y++)
      for (int x = 0; x<w; x++)
        input2(x,y) = 50.f/(0.002f*(x*x + y*y) +1.0f);

  vil1_memory_image_of<float> mag2, phase2, output2;
  brip_float_ops::fourier_transform(input2, mag2, phase2);
  brip_float_ops::inverse_fourier_transform(mag2, phase2, output2);

  vil1_memory_image_of<unsigned char> char_in = brip_float_ops::convert_to_byte(input1);
  vil1_memory_image_of<unsigned char> char_mag = brip_float_ops::convert_to_byte(mag);
  vil1_memory_image_of<unsigned char> char_phase = brip_float_ops::convert_to_byte(phase);
  vil1_memory_image_of<unsigned char> char_output = brip_float_ops::convert_to_byte(output);
  TEST("fourier transform mag", char_mag(55,55), 0);
  TEST("fourier transform phase", char_phase(26,13), 0);
  TEST("fourier transform peak", char_mag(64,64), 255);
  TEST("inverse fourier transform", char_in(1,2), char_output(1,2));

  vil1_memory_image_of<unsigned char> char_square = brip_float_ops::convert_to_byte(input2);
  vil1_memory_image_of<unsigned char> char_mag2 = brip_float_ops::convert_to_byte(mag2);
  vil1_memory_image_of<unsigned char> char_phase2 = brip_float_ops::convert_to_byte(phase2);
  vil1_memory_image_of<unsigned char> char_output2 = brip_float_ops::convert_to_byte(output2);
  TEST("fourier transform mag", char_mag2(55,55), 0);
  TEST("fourier transform phase", char_phase2(59,59), 255);
  TEST("fourier transform peak", char_mag2(64,64), 255);
  TEST("inverse fourier transform", char_square(0,0), char_output2(0,0));

#ifdef DEBUG
  vil1_save(char_in, "./input.tif");
  vil1_save(char_mag, "./mag.tif");
  vil1_save(char_phase, "./phase.tif");
  vil1_save(char_output, "./output.tif");
  vil1_save(char_square, "./input2.tif");
  vil1_save(char_mag2, "./mag2.tif");
  vil1_save(char_phase2, "./phase2.tif");
  vil1_save(char_output2, "./output2.tif");
#endif
}

TESTMAIN(brip_test_fourier);
