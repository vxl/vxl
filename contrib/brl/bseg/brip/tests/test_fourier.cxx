// This is brl/bseg/brip/tests/test_fourier.cxx
#include <vil1/vil1_memory_image_of.h>
#ifdef DEBUG
#include <vil1/vil1_save.h>
#endif
#include <brip/brip_vil1_float_ops.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

static void test_fourier()
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
  brip_vil1_float_ops::fourier_transform(input1, mag, phase);
  brip_vil1_float_ops::inverse_fourier_transform(mag, phase, output);

  // square at origin
  vil1_memory_image_of<float> input2(w, h);
  for (int y = 0; y< h; y++)
      for (int x = 0; x<w; x++)
        input2(x,y) = 50.f/(0.002f*(x*x + y*y) +1.0f);

  vil1_memory_image_of<float> mag2, phase2, output2;
  brip_vil1_float_ops::fourier_transform(input2, mag2, phase2);
  brip_vil1_float_ops::inverse_fourier_transform(mag2, phase2, output2);

  vil1_memory_image_of<unsigned char> char_in = brip_vil1_float_ops::convert_to_byte(input1);
  vil1_memory_image_of<unsigned char> char_mag = brip_vil1_float_ops::convert_to_byte(mag);
  vil1_memory_image_of<unsigned char> char_phase = brip_vil1_float_ops::convert_to_byte(phase);
  vil1_memory_image_of<unsigned char> char_output = brip_vil1_float_ops::convert_to_byte(output);
  vcl_cout << "fourier transform mag(55,55) = " << mag(55,55) << " = " << (int)char_mag(55,55) << '\n'
           << "fourier transform phase(26,13) = " << phase(26,13) << " = " << (int)char_phase(26,13) << '\n'
           << "fourier transform mag(64,64) = " << mag(64,64) << " = " << (int)char_mag(64,64) << '\n';
  TEST("fourier transform mag", char_mag(55,55), 0);
  TEST("fourier transform phase", char_phase(26,13), 0);
  int val = (char_mag(64,64) > 254) ? 255 : 254;
  TEST("fourier transform peak", char_mag(64,64), val); // either 254 or 255
  TEST("inverse fourier transform", char_in(1,2), char_output(1,2));

  vil1_memory_image_of<unsigned char> char_square = brip_vil1_float_ops::convert_to_byte(input2);
  vil1_memory_image_of<unsigned char> char_mag2 = brip_vil1_float_ops::convert_to_byte(mag2);
  vil1_memory_image_of<unsigned char> char_phase2 = brip_vil1_float_ops::convert_to_byte(phase2);
  vil1_memory_image_of<unsigned char> char_output2 = brip_vil1_float_ops::convert_to_byte(output2);
  vcl_cout << "fourier transform mag(55,55) = " << mag2(55,55) << " = " << (int)char_mag2(55,55) << '\n'
           << "fourier transform phase(59,59) = " << phase2(59,59) << " = " << (int)char_phase2(59,59) << '\n'
           << "fourier transform mag(64,64) = " << mag2(64,64) << " = " << (int)char_mag2(64,64) << '\n';
  TEST("fourier transform mag", char_mag2(55,55), 0);
  TEST("fourier transform phase", char_phase2(59,59), val); // either 254 or 255
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

TESTMAIN(test_fourier);
