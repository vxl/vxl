// This is brl/bseg/tests/brip_test_homography.cxx
#define DEBUG
#include <vil1/vil1_memory_image_of.h>
#ifdef DEBUG
#include <vil1/vil1_save.h>
#endif
#include <brip/brip_vil1_float_ops.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
static void brip_test_homography()
{
  const int w = 100, h = 100;
  vil1_memory_image_of<float> input(w,h), out;
  // diagonal periodic stripes
  int period = 5;
  for (int y = 0; y< h; y++)
    for (int x = 0; x<w; x++)
      input(x,y)=50;

  for (int p = 0; p<h; p+=period)
    for (int y = 0; y< h; y++)
      for (int x = 0; x<w; x++)
        if (x==y+p)
          input(x,y)=150;
  //first try simple rotation
  vnl_matrix_fixed<double,3, 3> M;
  M[0][0]= 0.707;   M[0][1]= -0.707;   M[0][2]= 70.7; 
  M[1][0]= 0.707;   M[1][1]= 0.707;   M[1][2]= 0;
  M[2][0]= 0;   M[2][1]= 0; M[2][2]= 1;
  vgl_h_matrix_2d<double> H(M);
  vcl_cout << "H\n"<< H << "\n";
  if(!brip_vil1_float_ops::homography(input, H, out))
    {
      vcl_cout << "homography failed \n";
      return;
    }

  vil1_memory_image_of<unsigned char> char_in = brip_vil1_float_ops::convert_to_byte(input);
  vil1_memory_image_of<unsigned char> char_out = brip_vil1_float_ops::convert_to_byte(out, 0, 255);
#ifdef DEBUG
  vil1_save(char_in, "./homg_input.tif");
  vil1_save(char_out, "./homg_out.tif");
#endif
}

TESTMAIN(brip_test_homography);
