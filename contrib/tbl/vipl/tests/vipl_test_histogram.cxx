//:
// \file
//  Test of the vipl_histogram templated IP classes
//  vipl_histogram<vil_image_view<TW,vnl_vector<T>,T,unsigned int>
//  for T in { vxl_byte, vxl_uint_16, float }.
//
// \author Ian Scott, ISBE, Manchester.
// \date   22 May 2002
//
// \verbatim
// Modifications:
//   Peter Vanroose, Feb.2004 - replaced vil1_image by vil2_image_view<T>
// \endverbatim
//

#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vipl/vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_vector.h>
#include <vipl/vipl_histogram.h>
#include <vnl/vnl_vector.h>
#include <vcl_iostream.h>
#include "test_driver.h"
#include <vxl_config.h> // for vxl_byte

//create and image containing no 0s, one 1, two 2s, three 3s, etc.
vil_image_view<vxl_byte> vipl_test_histogram_CreateTest8bitImage(int wd, int ht)
{
  vxl_byte i=0, j=1;
  vil_image_view<vxl_byte> image(wd, ht);
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; ++y, ++i)
    {
      if (i >= j) {++j;i=0;}
      image(x,y) = j;
    }
  return image;
}


int vipl_test_histogram()
{
  vil_image_view<vxl_byte> byte_img = vipl_test_histogram_CreateTest8bitImage(32, 32);

  vcl_cout << "Starting vipl_histogram test\n";

  {
    const unsigned int expected_n = 45;
    const int scale = 2;
    vnl_vector<unsigned int> byte_out(expected_n / scale + 1);
    vipl_histogram<vil_image_view<vxl_byte>,vnl_vector<unsigned int>, vxl_byte,unsigned int> op(scale);
    op.put_in_data_ptr(&byte_img); op.put_out_data_ptr(&byte_out); op.filter();

    // check that final bin has correct number.
    bool test_pass = byte_out(byte_out.size()-1) + ((expected_n-2)*(expected_n-1))/2
                     == (unsigned int)(byte_img.ni() * byte_img.nj());

    // check that rest of bins are correct
    for (unsigned int i=0; i+2<byte_out.size(); ++i)
      if (byte_out(i)  != i*2 + i*2+1) test_pass = false;
    TEST("vnl_vector histogram counted correctly", test_pass, true);
    vcl_cout << byte_out << vcl_endl;
  }
  {
    vcl_vector<unsigned int> byte_out(46);
    vipl_histogram<vil_image_view<vxl_byte>,vcl_vector<unsigned int>, vxl_byte,unsigned int> op;
    op.put_in_data_ptr(&byte_img); op.put_out_data_ptr(&byte_out); op.filter();

    // check that final bin has correct number.
    bool test_pass = byte_out.back() + ((byte_out.size()-2)*(byte_out.size()-1))/2
                     == (unsigned int)(byte_img.ni() * byte_img.nj());

    // check that rest of bins are correct
    for (unsigned int i=0; i+1<byte_out.size(); ++i)
      if (byte_out[i] != i) test_pass = false;
    TEST("vcl_vector histogram counted correctly", test_pass, true);
  }

  return 0;
}

TESTMAIN(vipl_test_histogram);
