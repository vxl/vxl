#include <vgui/vgui_image_tableau.h>

#include <testlib/testlib_test.h>

#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>

static void test_image_tableau(int argc, char* argv[])
{
  // Make sure that constructing a vgui_image_tableau with a filename
  // results in the vil1_image being loaded, since this is expected by
  // older code.
  //
  const char* input_file = argc>1 ? argv[1] : "Please give it as command line parameter";
  vil1_image img = vil1_load( input_file );
  if ( !img ) {
    std::cout << "Couldn't load test image \"" << input_file << "\"\n";
  } else {
    vgui_image_tableau_new img_tab( input_file );
    vil1_image img2 = img_tab->get_image();
    TEST( "Construct with filename", !img2, false);
    TEST( "Size is correct",
          img.width()      == img2.width() &&
          img.height()     == img2.height() &&
          img.components() == img2.components() &&
          img.planes()     == img2.planes() &&
          img.bits_per_component() == img2.bits_per_component(), true);

    unsigned buf_size = img.width() * img.height() *
                        img.planes() * img.components() *
                        ( (img.bits_per_component()+7) / 8 );
    char* img1_buf = new char[buf_size];
    char* img2_buf = new char[buf_size];

    TEST( "Get section from img1",
          !img.get_section( img1_buf, 0, 0, img.width(), img.height() ), false);
    TEST( "Get section from img2",
          !img2.get_section( img2_buf, 0, 0, img2.width(), img2.height() ), false);

    bool okay = true;
    for ( unsigned i = 0; i < buf_size; ++i ) {
      if ( img1_buf[i] != img2_buf[i] ) {
        okay = false;
        break;
      }
    }
    TEST( "Contents are correct", okay, true);

    delete [] img1_buf;
    delete [] img2_buf;
  }
}

// Supply a test image as the first argument

TESTMAIN_ARGS(test_image_tableau);
