// Author: Peter@Vanroose.esat.kuleuven.ac.be
// Date: 17 February, 2000
#include <vcl/vcl_string.h>
#include <vcl/vcl_cstdio.h> // for tmpnam()
#include <vcl/vcl_unlink.h>
#include <vil/vil_rgb_byte.h>

#include <vil/vil_generic_image.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image_of.h>

#include <vil/file_formats/vil_iris.h>
#include <vil/file_formats/vil_jpeg.h>
#include <vil/file_formats/vil_png.h>
#include <vil/file_formats/vil_pnm.h>
#include <vil/file_formats/vil_viff.h>

int all_passed = 0;

// -- this function tests to see if all the pixels in two images are equal
void test_image_equal(const char* test,
		      const char* type_name,
		      vil_generic_image* image,
		      vil_generic_image* image2)
{
  int sizex = image->width();
  int sizey = image->height();
  int components = image->components();
  int planes = image->planes();
  int cell_bits = image->bits_per_component();
  int num_bits = sizex * sizey * components * planes * cell_bits;
  int sizex2 = image2->width();
  int sizey2 = image2->height();
  int components2 = image2->components();
  int planes2 = image2->planes();
  int cell_bits2 = image2->bits_per_component();
  int num_bits2 = sizex2 * sizey2 * components2 * planes2 * cell_bits2;

  if (sizex != sizex2 || sizey != sizey2)
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image dimensions differ: "
         << sizex2 << " x " << sizey2 << endl;
    return;
  }

  if (cell_bits != cell_bits2)
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image pixel sizes differ: "
         << cell_bits2 << " instead of " << cell_bits << endl;
    return;
  }

  if (image->component_format() != image2->component_format())
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image formats differ: "
	 << image2->component_format() << " instead of "
	 << image->component_format() << endl;
    return;
  }

  if (image->get_size_bytes() != num_bits/8 ||
      image2->get_size_bytes() != num_bits2/8 )
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- Image sizes differ: "
	 << num_bits2 << "bits, " << image2->get_size_bytes() << endl;
    return;
  }

  unsigned char *image_buf = new unsigned char [image->get_size_bytes()];
  if (!image->do_get_section(image_buf, 0, 0, sizex, sizey))
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- image::do_get_section() on first image returned false!" << endl;
    delete [] image_buf;
    return;
  }
  unsigned char *image_buf2 = new unsigned char [image2->get_size_bytes()];
  if (!image2->do_get_section(image_buf2, 0, 0, sizex2, sizey2))
  {
    cout << "FAILED: test <" << test << "> for " << type_name
         << " -- image::do_get_section() on second image returned false!" << endl;
    delete [] image_buf2;
    return;
  }

  int bad = 0;
  for (int i=0; i < image->get_size_bytes(); ++i)
  {
    if(image_buf[i] != image_buf2[i])
    {
#ifdef DEBUG
      cout << " pixel " << i <<  " differs: " << (int)image_buf[i] << " --> "
           << (int) image_buf2[i] << endl;
#endif
      bad++;
    }
  }
  delete [] image_buf;
  delete [] image_buf2;
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

void vil_test_image_type(const char* type_name, // type for image to read and write
			 vil_generic_image* image) // test image to save and restore
{
  // Step 1) Write the image out to disk

  // create a file name
  char* basename = tmpnam(0);
  vcl_string fname(basename);
  fname += ".";
  if (type_name) fname += type_name;

  // Write image to disk
  if (!vil_save(image, fname.data(), type_name))
  {
    ++all_passed;
    cout << "vil_save() FAILED\n";
    return;
  }

  // STEP 2) Read the image that was just saved to file
  vil_generic_image* image2 = vil_load(fname.data());
  image2->ref();
  // make sure saved image has the same pixels as the original image
  if(strcmp(type_name,image2->file_format()))
    cout << "WARNING: read back image type is " << image2->file_format()
         << " instead of written " << type_name << endl;
  else
    test_image_equal("original equal to saved", type_name, image, image2);

  // if we have made it this far then report save as a success
  if (all_passed == 0)
    cout << "PASSED: vil_save() for " << type_name << endl;

  image2->unref();
#ifndef DEBUG
  vcl_unlink(fname.data());
#endif
  return;
}


// create a 1 bit test image
vil_generic_image* CreateTest1bitImage(int wd, int ht)
{
  vil_memory_image* image = new vil_memory_image(1, wd, ht, 1, 1, VIL_COMPONENT_FORMAT_UNSIGNED_INT);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y+=8) {
      unsigned char data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image->do_put_section(&data, x, y, 1, 8);
    }
  return image;
}


// create an 8 bit test image
vil_generic_image* CreateTest8bitImage(int wd, int ht)
{
  vil_memory_image_of<unsigned char>* image = new vil_memory_image_of<unsigned char>(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data = ((x-wd/2)*(y-ht/2)/16) % (1<<8);
      image->do_put_section(&data, x, y, 1, 1);
    }
  return image;
}


// create a 16 bit test image
vil_generic_image* CreateTest16bitImage(int wd, int ht)
{
  vil_memory_image_of<unsigned short>* image = new vil_memory_image_of<unsigned short>(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned short data = ((x-wd/2)*(y-ht/2)/16) % (1<<16);
      image->do_put_section(&data, x, y, 1, 1);
  }
  return image;
}


// create a 24 bit color test image
vil_generic_image* CreateTest24bitImage(int wd, int ht)
{
  vil_memory_image_of<vil_rgb_byte>* image = new vil_memory_image_of<vil_rgb_byte>(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image->do_put_section(data, x, y, 1, 1);
    }
  return image;
}


// create a 24 bit color test image, with 3 planes
vil_generic_image* CreateTest3planeImage(int wd, int ht)
{
  vil_memory_image* image = new vil_memory_image(3, wd, ht, 1, 8, VIL_COMPONENT_FORMAT_UNSIGNED_INT);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      unsigned char data[3] = { x%(1<<8), ((x-wd/2)*(y-ht/2)/16) % (1<<8), ((y/3)%(1<<8)) };
      image->do_put_section(data, x, y, 1, 1);
    }
  return image;
}


// create a float-pixel test image
vil_generic_image* CreateTestfloatImage(int wd, int ht)
{
  vil_memory_image_of<float>* image = new vil_memory_image_of<float>(wd, ht);
  for(int x = 0; x < wd; x++)
    for(int y = 0; y < ht; y++) {
      float data = 0.01 * ((x-wd/2)*(y-ht/2)/16);
      image->do_put_section(&data, x, y, 1, 1);
    }
  return image;
}

int main() {
  // create a test image
  int sizex = 256, sizey = 155;
  vil_generic_image* image1 = CreateTest1bitImage(sizex, sizey); image1->ref();
  vil_generic_image* image8 = CreateTest8bitImage(sizex, sizey); image8->ref();
  vil_generic_image* image16 = CreateTest16bitImage(sizex, sizey); image16->ref();
  vil_generic_image* image24 = CreateTest24bitImage(sizex, sizey); image24->ref();
  vil_generic_image* image3p = CreateTest3planeImage(sizex, sizey); image3p->ref();
  vil_generic_image* imagefloat = CreateTestfloatImage(sizex, sizey); imagefloat->ref();

  // test the pnm read write
  vil_test_image_type("pnm", image8);
  vil_test_image_type("pnm", image16);
  vil_test_image_type("pnm", image24);
  // test the lily image read write
  //vil_test_image_type("lily", image8);
  //vil_test_image_type("lily", imagefloat);
  // test the VIFF image read write
  //vil_test_image_type("viff", image1);
  vil_test_image_type("viff", image8);
  vil_test_image_type("viff", image16);
  //vil_test_image_type("viff", image3p);
  vil_test_image_type("viff", imagefloat);
  // test the TIFF read write
  vil_test_image_type("tiff", image8);
  vil_test_image_type("tiff", image24);
  // test the JPEG read write
  vil_test_image_type("jpeg", image8);
  vil_test_image_type("jpeg", image24);
  // test the PNG read write
  vil_test_image_type("png", image8);
  vil_test_image_type("png", image24);
  // test the sgi read write
  vil_test_image_type("iris", image8);
  vil_test_image_type("iris", image16);
  vil_test_image_type("iris", image24);

  image1->unref();
  image8->unref();
  image16->unref();
  image24->unref();
  imagefloat->unref();
  return 0; // return all_passed;
}
