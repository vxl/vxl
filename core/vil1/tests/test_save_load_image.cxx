// Author: Peter@Vanroose.esat.kuleuven.ac.be
// Date: 17 February, 2000
#include <vcl/vcl_cstdio.h> // tmpnam()
#include <vcl/vcl_unistd.h> // vcl_unlink()
#include <vcl/vcl_string.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vil/vil_rgb_byte.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image_of.h>

#include <vil/file_formats/vil_iris.h>
//#include <vil/file_formats/vil_jpeg.h>
#include <vil/file_formats/vil_png.h>
#include <vil/file_formats/vil_pnm.h>
#include <vil/file_formats/vil_viff.h>

#define DEBUG 1
#define LEAVE_IMAGES_BEHIND 0

int all_passed = 0;

static bool blather = false;
#define ww (blather ? cout << "reached " __FILE__ ":" << __LINE__ << endl : cout)

// -- this function tests to see if all the pixels in two images are equal
void test_image_equal(char const* test,
		      char const* type_name,
		      vil_image const & image,
		      vil_image const & image2)
{
  ww;
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
  ww;

  if (sizex != sizex2 || sizey != sizey2)
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image dimensions differ: "
         << sizex2 << " x " << sizey2 << endl;
    return;
  }
  ww;

  if (cell_bits != cell_bits2)
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image pixel sizes differ: "
         << cell_bits2 << " instead of " << cell_bits << endl;
    return;
  }
  ww;

  if (image.component_format() != image2.component_format())
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image formats differ: "
	 << image2.component_format() << " instead of "
	 << image.component_format() << endl;
    return;
  }
  ww;

  if (image.get_size_bytes() != num_bits/8 ||
      image2.get_size_bytes() != num_bits2/8 )
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image sizes differ: "
	 << num_bits2 << "bits, " << image2.get_size_bytes() << endl;
    return;
  }
  ww;

  vcl_vector<unsigned char> image_buf(image.get_size_bytes());
  if (!image.get_section(image_buf.begin(), 0, 0, sizex, sizey))
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- image::do_get_section() on first image returned false!" << endl;
    return;
  }
  ww;

  vcl_vector<unsigned char> image_buf2(image2.get_size_bytes());
  if (!image2.get_section(image_buf2.begin(), 0, 0, sizex2, sizey2))
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- image::do_get_section() on second image returned false!" << endl;
    return;
  }
  ww;

  int bad = 0;
  for (int i=0; i < image.get_size_bytes(); ++i)
  {
    if(image_buf[i] != image_buf2[i])
    {
#if DEBUG
      cerr << " pixel " << i <<  " differs: " << (int)image_buf[i] << " --> "
           << (int) image_buf2[i] << endl;
#endif
      bad++;
    }
    if (bad > 100) {
      cerr << "100 or more pixels differ - ignoring rest" << endl;
      break;
    }
  }

  if (bad)
    {
      cout << "FAILED: test <" << test << "> for " << type_name
           << " -- number of unequal pixels: "  << bad << endl;
      ++all_passed;
    }
  else
    cout << "PASSED: test <" << test << "> for " << type_name
         <<  " -- images are the same" << endl;
}

// -- this function tests the read and write for the given image into the
//    image type specified in type.

void vil_test_image_type(char const* type_name, // type for image to read and write
			 vil_image const & image) // test image to save and restore
{
  // Step 1) Write the image out to disk

  // create a file name
  char* basename = tmpnam(0);
  vcl_string fname(basename);
  fname += ".";
  if (type_name) fname += type_name;

  cout << "vil_test_image_type: Save to [" << fname << "], ";

  
  // Write image to disk
  if (!vil_save(image, fname.data(), type_name))
  {
    ++all_passed;
    cout << "vil_save() FAILED\n";
    return;
  }

  // STEP 2) Read the image that was just saved to file
  cout << "load, ";
  vil_image image2 = vil_load(fname.data());

  // make sure saved image has the same pixels as the original image
  cout << "compare, ";
  if(strcmp(type_name,image2.file_format()))
    cout << "WARNING: read back image type is " << image2.file_format()
         << " instead of written " << type_name << endl;
  else
    test_image_equal("original equal to saved", type_name, image, image2);

  // if we have made it this far then report save as a success
  if (all_passed == 0)
    cout << "PASSED: vil_save() for " << type_name << endl;

#if !LEAVE_IMAGES_BEHIND
  vcl_unlink(fname.data());
#endif
  cout << "done\n";
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


// create a 24 bit color test image
vil_image CreateTest24bitImage(int wd, int ht)
{
  vil_memory_image_of<vil_rgb_byte> image(wd, ht);
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
  vil_image image3p = CreateTest3planeImage(sizex, sizey);
  vil_image imagefloat = CreateTestfloatImage(sizex, sizey);

#if 0
  // pnm
  vil_test_image_type("pnm", image8);
  vil_test_image_type("pnm", image16);
  vil_test_image_type("pnm", image24);

  // lily image
  //vil_test_image_type("lily", image8);
  //vil_test_image_type("lily", imagefloat);

  // VIFF image
  //vil_test_image_type("viff", image1);
  vil_test_image_type("viff", image8);
  vil_test_image_type("viff", image16);
  //vil_test_image_type("viff", image3p);
  vil_test_image_type("viff", imagefloat);

  // TIFF
  vil_test_image_type("tiff", image8);
  vil_test_image_type("tiff", image24);

#if 0 // lossy format
  // JPEG
  vil_test_image_type("jpeg", image8);
  vil_test_image_type("jpeg", image24);
#endif

  // PNG
  vil_test_image_type("png", image8);
  vil_test_image_type("png", image24);
#endif

  // sgi
  blather = true;
  vil_test_image_type("iris", image8);
  vil_test_image_type("iris", image16);
  vil_test_image_type("iris", image24);

  return 0;
}
