// Author: Peter@Vanroose.esat.kuleuven.ac.be
// Date: 17 February, 2000
#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_cstdio.h> // tmpnam()
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vpl/vpl_unistd.h> // vpl_unlink()

#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image_of.h>

#include <vil/vil_test.h>

#define DEBUG 0
#define LEAVE_IMAGES_BEHIND 0

#if 0
#define ww cout << "reached " __FILE__ ":" << __LINE__ << endl
#endif

//: Test to see if all the pixels in two images are equal
bool test_image_equal(char const* type_name,
                      vil_image const & image,
                      vil_image const & image2,
                      bool exact = true)
{
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

  TEST ("Image dimensions", sizex == sizex2 && sizey == sizey2, true);
  if (sizex != sizex2 || sizey != sizey2)
  {
    vcl_cout << type_name << ": sizes are " << sizex2 << " x " << sizey2 << vcl_endl;
    return false;
  }

  TEST ("Image pixel sizes", cell_bits, cell_bits2);
  if (cell_bits != cell_bits2)
  {
    vcl_cout << type_name << ": pixel size is " << cell_bits2 << vcl_endl;
    return false;
  }

  TEST ("Image format", image.component_format(), image2.component_format());
  if (image.component_format() != image2.component_format())
  {
    vcl_cout << type_name << ": format is " << image2.component_format() << vcl_endl;
    return false;
  }

  TEST ("Image byte count",
        image.get_size_bytes() == num_bits/8 &&
        image2.get_size_bytes() == num_bits2/8,
        true);
  if (image.get_size_bytes() != num_bits/8 ||
      image2.get_size_bytes() != num_bits2/8 )
  {
    vcl_cout << type_name << num_bits2 << "bits, " << image2.get_size_bytes() << " bytes\n";
    return false;
  }

  vcl_vector<unsigned char> image_buf(image.get_size_bytes());
  TEST ("get_section() on first image", ! image.get_section(&image_buf[0], 0, 0, sizex, sizey), false);

  vcl_vector<unsigned char> image_buf2(image2.get_size_bytes());
  TEST ("get_section() on second image", ! image2.get_section(&image_buf2[0], 0, 0, sizex2, sizey2), false);

  if (!exact) // no exact pixel match wanted
  {
    TEST ("image headers are identical", true, true);
    return true;
  }

  int bad = 0;
  for (int i=0; i < image.get_size_bytes(); ++i)
  {
    if(image_buf[i] != image_buf2[i])
    {
#if DEBUG
      if (++bad < 20)
        vcl_cout << "\n pixel " << i <<  " differs: " << (int)image_buf[i] << " --> "
             << (int) image_buf2[i];
#else
      ++bad; vcl_cout << ".";
#endif
    }
  }

  TEST ("pixelwise comparison", bad, 0);
  if (bad)
  {
    vcl_cout << type_name << ": number of unequal pixels: "  << bad << vcl_endl;
    return false;
  }
  else
    return true;
}

//: Test the read and write for the given image into the image type specified in type.

void vil_test_image_type(char const* type_name, // type for image to read and write
                         vil_image const & image, // test image to save and restore
                         bool exact = true) // require read back image identical
{
  // Step 1) Write the image out to disk
  //
  // create a file name
  vcl_string fname(tmpnam(0));
  fname += ".";
  if (type_name) fname += type_name;

  vcl_cout << "vil_test_image_type: Save to [" << fname << "]\n";

  // Write image to disk
  bool tst = vil_save(image, fname./*data() does not null-terminate*/c_str(), type_name);
  TEST ("write image to disk", tst, true);
  if (!tst) return; // fatal error

#if LEAVE_IMAGES_BEHIND
  vpl_chmod(fname.c_str(), 0666); // -rw-rw-rw-
#endif

  // STEP 2) Read the image that was just saved to file
  vil_image image2 = vil_load(fname.c_str());
  TEST ("load image", !image2, false);
  if (!image2)
  {
    return; // fatal error
  }

  // make sure saved image has the same pixels as the original image
  tst = !(vcl_strcmp(type_name,image2.file_format()));
  TEST ("compare image file formats", tst, true);
  if (!tst)
    vcl_cout << "read back image type is " << image2.file_format()
             << " instead of written " << type_name << vcl_endl;
  else
    test_image_equal(type_name, image, image2, exact);

#if !LEAVE_IMAGES_BEHIND
  vpl_unlink(fname.c_str());
#endif
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

void test_save_load_image() {
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

  // GIF
#if 0
  vil_test_image_type("gif", image8);
  vil_test_image_type("gif", image24);
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
}

TESTMAIN(test_save_load_image);
