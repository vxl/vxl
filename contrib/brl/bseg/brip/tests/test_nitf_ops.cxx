#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <brip/brip_vil_nitf_ops.h>
#include <vcl_iostream.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>

static void test_nitf_ops( int argc, char* argv[] )
{
  // load a nitf sample images from file
  vcl_string image_file;

  if( argc >= 2 )
    {
    image_file = argv[1];
    }
  else
    {
    vcl_string root = testlib_root_dir();
    image_file = root + "/core/vil/tests/file_read_data/ff_nitf_16bit.nitf";
    }
  if( !vul_file::exists(image_file) )
    {
    vcl_string error_msg = "Can not find test input image: " + image_file;
    TEST(error_msg.c_str(), false, true);
    vcl_cout << error_msg << '\n';
    return;
    }

  vil_image_resource_sptr ir = vil_load_image_resource(image_file.c_str() );
  vil_nitf2_image*        nitf = static_cast<vil_nitf2_image *>(ir.ptr() );

  unsigned ni = nitf->ni();
  unsigned nj = nitf->nj();

  vil_image_view<vxl_uint_16> nitf_view = nitf->get_view();
  vcl_cout << "Input nitf image pixel values: \n";
  for( unsigned i = 0; i < ni; i++ )
    {
    for( unsigned j = 0; j < nj; j++ )
      {
      vcl_cout << nitf_view(i, j) << ' ';
      }
    vcl_cout << '\n';
    }

  TEST("Input input image format", nitf->pixel_format(), VIL_PIXEL_FORMAT_UINT_16);

  vil_image_view<vxl_byte> out_byte(ni, nj);
  out_byte.fill(0);

  TEST("Truncate nitf image to byte image", brip_vil_nitf_ops::truncate_nitf_bits(nitf_view, out_byte), true);

  vcl_string out_byte_fname = "./out_byte.tif";
  vil_save(out_byte, out_byte_fname.c_str() );

  vil_image_view<vxl_uint_16> out_short(ni, nj);
  out_short.fill(0);

  TEST("Truncate nitf image to unsigned short image", brip_vil_nitf_ops::truncate_nitf_bits(nitf_view, out_short),
       true);

  TEST("output image format (byte)", out_byte.pixel_format(), VIL_PIXEL_FORMAT_BYTE);
  TEST("output image format (short)", out_short.pixel_format(), VIL_PIXEL_FORMAT_UINT_16);
  vcl_string out_short_fname = "./out_short.tif";
  vil_save(out_short, out_short_fname.c_str() );

  vcl_cout << "Truncated byte image (ignore most significant 5 bits and less 3 bits): \n";
  for( unsigned i = 0; i < ni; i++ )
    {
    for( unsigned j = 0; j < nj; j++ )
      {
      vcl_cout << (int)(out_byte)(i, j) << ' ';
      }
    vcl_cout << '\n';
    }

  vcl_cout << "Truncated short image (ignore most significant 5 bits): \n";
  for( unsigned i = 0; i < ni; i++ )
    {
    for( unsigned j = 0; j < nj; j++ )
      {
      vcl_cout << out_short(i, j) << ' ';
      }
    vcl_cout << '\n';
    }

}

TESTMAIN_ARGS(test_nitf_ops);
