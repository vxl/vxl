// Author: Peter@Vanroose.esat.kuleuven.ac.be
// Date: 17 February, 2000
#include <vcl_string.h>
#include <vcl_cstdio.h> // tmpnam()
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vpl/vpl_unistd.h> // vpl_unlink()

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image_of.h>

#define DEBUG 1
#define LEAVE_IMAGES_BEHIND 0

int nr_failures = 0;

//#define ww cout << "reached " __FILE__ ":" << __LINE__ << endl

// -- this function tests to see if all the pixels in two images are equal
bool test_image_equal(char const* test,
                      char const* type_name,
                      vil_image const & image,
                      vil_image const & image2,
                      bool exact = true)
{
//ww;
  int sizex = image.width();
  int sizey = image.height();
  int components = image.components();
  int planes = image.planes();
  int cell_bits = image.bits_per_component();
  int num_bits = sizex * sizey * components * planes * cell_bits;
  int sizex2 = image2.width();
  int sizey2 = image2.height();
  int components2 = image2.components();
  int planes2 = image2.planes();
  int cell_bits2 = image2.bits_per_component();
  int num_bits2 = sizex2 * sizey2 * components2 * planes2 * cell_bits2;
//ww;

  if (sizex != sizex2 || sizey != sizey2)
  {
    vcl_cout << "\nFAILED: test <" << test << "> for " << type_name
         << " -- Image dimensions differ: "
         << sizex2 << " x " << sizey2 << vcl_endl;
    return false;
  }
//ww;

  if (cell_bits != cell_bits2)
  {
    vcl_cout << "\nFAILED: test <" << test << "> for " << type_name
         << " -- Image pixel sizes differ: "
         << cell_bits2 << " instead of " << cell_bits << vcl_endl;
    return false;
  }
//ww;

  if (image.component_format() != image2.component_format())
  {
    vcl_cout << "\nFAILED: test <" << test << "> for " << type_name
         << " -- Image formats differ: "
         << image2.component_format() << " instead of "
         << image.component_format() << vcl_endl;
    return false;
  }
//ww;

  if (image.get_size_bytes() != num_bits/8 ||
      image2.get_size_bytes() != num_bits2/8 )
  {
    vcl_cout << "\nFAILED: test <" << test << "> for " << type_name
         << " -- Image sizes differ: "
         << num_bits2 << "bits, " << image2.get_size_bytes() << vcl_endl;
    return false;
  }
//ww;

  vcl_vector<unsigned char> image_buf(image.get_size_bytes());
  if (!image.get_section(/* xxx */&image_buf[0], 0, 0, sizex, sizey))
  {
    vcl_cout << "\nFAILED: test <" << test << "> for " << type_name
         << " -- image::do_get_section() on first image returned false!" << vcl_endl;
    return false;
  }
//ww;

  vcl_vector<unsigned char> image_buf2(image2.get_size_bytes());
  if (!image2.get_section(/* xxx */&image_buf2[0], 0, 0, sizex2, sizey2))
  {
    vcl_cout << "\nFAILED: test <" << test << "> for " << type_name
         << " -- image::do_get_section() on second image returned false!" << vcl_endl;
    return false;
  }
//ww;
  if (!exact) // no exact pixel match wanted
  {
    vcl_cout << "\nPASSED: test <" << test << "> for " << type_name
         <<  " -- image headers are identical" << vcl_endl;
    return true;
  }

  int bad = 0;
  for (int i=0; i < image.get_size_bytes(); ++i)
  {
    if(image_buf[i] != image_buf2[i])
    {
      if (++bad < 20)
#if DEBUG
        vcl_cout << "\n pixel " << i <<  " differs: " << (int)image_buf[i] << " --> "
             << (int) image_buf2[i];
#else
        vcl_cout << ".";
#endif
    }
  }

  if (bad)
  {
    vcl_cout << "\nFAILED: test <" << test << "> for " << type_name
         << " -- number of unequal pixels: "  << bad << vcl_endl;
    return false;
  }
  else
  {
    vcl_cout << "\nPASSED: test <" << test << "> for " << type_name
         <<  " -- images are identical" << vcl_endl;
    return true;
  }
}

// -- this function tests the read and write for the given image into the
//    image type specified in type.

void vil_test_image_type(char const* type_name, // type for image to read and write
                         vil_image const & image, // test image to save and restore
                         bool exact = true) // require read back image identical
{
  bool passed = true; // let's be optimistic :-)

  // Step 1) Write the image out to disk

  // create a file name
  vcl_string fname(tmpnam(0));
  fname += ".";
  if (type_name) fname += type_name;

  vcl_cout << "vil_test_image_type: Save to [" << fname << "], ";


  // Write image to disk
  if (!vil_save(image, fname./*data() does not null-terminate*/c_str(), type_name))
  {
    ++nr_failures;
    vcl_cout << "\nvil_save() FAILED ***\n";
    return; // fatal error
  }

#if LEAVE_IMAGES_BEHIND
  vpl_chmod(fname.c_str(), 0666); // -rw-rw-rw-
#endif

  // STEP 2) Read the image that was just saved to file
  vcl_cout << "load, ";
  vil_image image2 = vil_load(fname.c_str());
  if (!image2)
  {
    ++nr_failures;
    vcl_cout << "\nvil_load() FAILED ***\n";
    return; // fatal error
  }

  // make sure saved image has the same pixels as the original image
  vcl_cout << "compare, ";
  if(strcmp(type_name,image2.file_format()))
  {
    vcl_cout << "\n***FAILED***: read back image type is " << image2.file_format()
         << " instead of written " << type_name << vcl_endl;
    passed = false; // non-fatal error
  }
  else
    passed = test_image_equal("original equal to saved", type_name, image, image2, exact);

  // if we have made it this far then report save as a success
  if (passed)
    vcl_cout << "PASSED: vil_save() for " << type_name << vcl_endl;
  else
    ++nr_failures;

#if !LEAVE_IMAGES_BEHIND
  vpl_unlink(fname.c_str());
#endif
  vcl_cout << "done\n";
  return;
}


// create a 1 bit test image
vil_image CreateTest1bitImage(int wd, int ht)
{
  vil_memory_image image(1, wd, ht, 1, 1, VIL_COMPONENT_FORMAT_UNSIGNED_INT);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y+=8) {
      unsigned char data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image.put_section(&data, x, y, 1, 8);
    }
  return image;
}


// create an 8 bit test image
vil_image CreateTest8bitImage(int wd, int ht)
{
  vil_memory_image_of<unsigned char> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image.put_section(&data, x, y, 1, 1);
    }
  return image;
}

// create a 16 bit test image
vil_image CreateTest16bitImage(int wd, int ht)
{
  vil_memory_image_of<unsigned short> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned short data = ((x-wd/2)*(y-ht/2)/16) % (1<<16);
      image.put_section(&data, x, y, 1, 1);
  }
  return image;
}


// create a 32 bit test image
vil_image CreateTest32bitImage(int wd, int ht)
{
  vil_memory_image_of<int> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++)
      image(x, y) = x + wd*y;
  return image;
}


// create a 24 bit color test image
vil_image CreateTest24bitImage(int wd, int ht)
{
  vil_memory_image_of<vil_rgb<unsigned char> > image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image.put_section(data, x, y, 1, 1);
    }
  return image;
}


// create a 24 bit color test image, with 3 planes
vil_image CreateTest3planeImage(int wd, int ht)
{
  vil_memory_image image(3, wd, ht, 1, 8, VIL_COMPONENT_FORMAT_UNSIGNED_INT);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image.put_section(data, x, y, 1, 1);
    }
  return image;
}


// create a float-pixel test image
vil_image CreateTestfloatImage(int wd, int ht)
{
  vil_memory_image_of<float> image(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      float data = 0.01 * ((x-wd/2)*(y-ht/2)/16);
      image.put_section(&data, x, y, 1, 1);
    }
  return image;
}

int main() {
  // create test images
  int sizex = 253;
  int sizey = 155;
  vil_image image1 = CreateTest1bitImage(sizex, sizey);
  vil_image image8 = CreateTest8bitImage(sizex, sizey);
  vil_image image16 = CreateTest16bitImage(sizex, sizey);
  vil_image image24 = CreateTest24bitImage(sizex, sizey);
  vil_image image32 = CreateTest32bitImage(sizex, sizey);
  vil_image image3p = CreateTest3planeImage(sizex, sizey);
  vil_image imagefloat = CreateTestfloatImage(sizex, sizey);

  // pnm ( = PGM / PPM )
#if 1
  vil_test_image_type("pnm", image8);
  vil_test_image_type("pnm", image16);
  vil_test_image_type("pnm", image24);
#endif

  // lily (Leuven)
#if 1
  //vil_test_image_type("lily", image8);
  //vil_test_image_type("lily", imagefloat);
#endif

  // VIFF image (Khoros)
#if 1
  //vil_test_image_type("viff", image1);
  vil_test_image_type("viff", image8);
  vil_test_image_type("viff", image16);
  //vil_test_image_type("viff", image3p);
  vil_test_image_type("viff", imagefloat);
#endif

  // TIFF
#ifdef HAS_TIFF
  vil_test_image_type("tiff", image8);
  vil_test_image_type("tiff", image24);
#endif

  // JPEG
#ifdef HAS_JPEG
  // lossy format ==> not guaranteed to be identical (hence arg. 3 set to false)
  vil_test_image_type("jpeg", image8, false);
  vil_test_image_type("jpeg", image24, false);
#endif

  // PNG
#ifdef HAS_PNG
  vil_test_image_type("png", image8);
  vil_test_image_type("png", image24);
#endif

  // SGI "iris" rgb
#if 1
  vil_test_image_type("iris", image8);
//vil_test_image_type("iris", image16); // not implemented yet
  vil_test_image_type("iris", image24);
#endif

  // bmp
#if 1
  vil_test_image_type("bmp", image8);
  vil_test_image_type("bmp", image24);
#endif

  // mit
#if 1
  vil_test_image_type("mit", image8);
  vil_test_image_type("mit", image16);
  vil_test_image_type("mit", image24);
#endif

  vcl_cout << "Summary: ";
  if (nr_failures > 1)
    vcl_cout << "*** " << nr_failures << " failures\n";
  else if (nr_failures > 0)
    vcl_cout << "*** " << nr_failures << " failure\n";
  else
    vcl_cout << "all tests passed\n";

  return nr_failures;
}
