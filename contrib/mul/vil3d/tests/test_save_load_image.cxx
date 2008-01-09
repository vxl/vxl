// This is mul/vil3d/tests/test_save_load_image.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Peter Vanroose, Ian Scott
// \date 18 Dec 2007
//

#include <vcl_string.h>
#include <vcl_iostream.h>

#include <vxl_config.h> // for vxl_byte

#include <vul/vul_temp_filename.h>
#include <vpl/vpl.h> // vpl_unlink()

#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil/vil_print.h>
#include <vil3d/vil3d_property.h>

#ifndef LEAVE_IMAGES_BEHIND
#define LEAVE_IMAGES_BEHIND 0
#endif


//: Test to see if all the pixels in two images are equal
template <class T>
bool test_image_equal(char const* type_name,
                      vil3d_image_view<T> const & image,
                      float voxel_size[3],
                      vil3d_image_resource_sptr const& pimage2,
                      bool exact = true)
{
  int sizex = image.ni();
  int sizey = image.nj();
  int planes = image.nplanes();
  int sizex2 = pimage2->ni();
  int sizey2 = pimage2->nj();
  int planes2 = pimage2->nplanes();

  // make sure saved image has the same pixels as the original image
  TEST("Loaded image can be viewed as same type as saved image", !pimage2, false);
  if (!pimage2)
  {
    vcl_cout << "read back image type has pixel type " << pimage2->pixel_format()
             << " instead of (as written) " << image.pixel_format() << '\n' << vcl_flush;
    return false;
  }

  TEST("Image dimensions", sizex == sizex2 && sizey == sizey2 && sizey == sizey2, true);
  if (sizex != sizex2 || sizey != sizey2)
  {
    vcl_cout << type_name << ": sizes are " << sizex2 << " x " << sizey2
             << " instead of " << sizex << " x " << sizey << '\n' << vcl_flush;
    return false;
  }

  TEST("Number of planes", planes, planes2);
  if (planes != planes2)
  {
    vcl_cout << type_name << ": nplanes are " << planes2
             << " instead of " << planes << '\n' << vcl_flush;
    return false;
  }

  if (!exact) // no exact pixel match wanted
  {
    TEST("image headers are identical", true, true);
    return true;
  }

  vil3d_image_view<T> image2 = pimage2->get_view();
  TEST("retrieved pixels", !image2, false);
  if (!image2) return false;


  int bad = 0;
  for (int p=0; p < planes; ++p)
  {
    for (int j=0; j < sizey; ++j)
    {
      for (int i=0; i < sizex; ++i)
      {
        if ( !(image(i,j,p) == image2(i,j,p)) )
        {
    #ifndef NDEBUG
          if (++bad < 20)
          {
            vcl_cout << "pixel (" << i << ',' << j << ',' << p <<  ") differs:\t";
            vil_print_value(vcl_cout, image(i,j,p));
            vcl_cout << "---> ";
            vil_print_value(vcl_cout,image2(i,j,p));
            vcl_cout << '\n';
          }
    #else
          ++bad; vcl_cout << '.' << vcl_flush;
    #endif
        }
      }
    }
  }

  TEST("pixelwise comparison", bad, 0);
  if (bad)
  {
    vcl_cout << type_name << ": number of unequal pixels: "  << bad
             << " out of " << planes *sizex * sizey << '\n' << vcl_flush;
    return false;
  }

  if (voxel_size != 0)
  {
    float vs2[3];
    TEST("Voxel size provided", pimage2->get_property(vil3d_property_voxel_size, vs2), true);
    TEST_NEAR("Voxel size i correct", vs2[0], 0.001*voxel_size[0], 1e-4);
    TEST_NEAR("Voxel size j correct", vs2[1], 0.001*voxel_size[1], 1e-4);
    TEST_NEAR("Voxel size k correct", vs2[2], 0.001*voxel_size[2], 1e-4);
  }
  return true;
}


template<class T>
void vil3d_test_image_type(char const* type_name, // type for image to read and write
                           vil3d_image_view<T> const & image, // test image to save and restore
                           float voxel_size[3],
                           bool exact = true,  // require read back image identical
                           bool fail_save = false) // expect fail on save if true
{
  vcl_cout << "=== Start testing " << type_name << " ===\n" << vcl_flush;

  // Step 1) Write the image out to disk
  //
  // create a file name
  vcl_string fname = vul_temp_filename();
  fname += ".";
  if (type_name) fname += type_name;

  vcl_cout << "vil3d_test_image_type: Save " << image.is_a() <<
              " to [" << fname << "]\n" << vcl_flush;
  {
    // Write image to disk
    bool success = false;
    if (!voxel_size)
      success = vil3d_save(image, fname.c_str());
    else
      success = vil3d_save(image, voxel_size[0],
        voxel_size[1], voxel_size[2], fname.c_str());
    TEST("save image", success, true);
  }

  // STEP 2) Read the image that was just saved to file
  {
    vil3d_image_resource_sptr ir2 = vil3d_load_image_resource(fname.c_str());
    TEST("load image", !ir2, false);
    if (!ir2)
    {
      return; // fatal error
    }

    test_image_equal(type_name, image, voxel_size, ir2, exact);
  }

#if !LEAVE_IMAGES_BEHIND
  vpl_unlink(fname.c_str());
#endif
}


// create a 1 bit test image
static vil3d_image_view<bool> CreateTest1bitImage(int wd, int ht, int dt)
{
  vil3d_image_view<bool> image(wd, ht, dt);
  for (int k = 0; k < dt; k++)
    for (int j = 0; j < ht; j++)
      for (int i = 0; i < wd; i++)
        image(i, j, k) = (i*j*k)%2 ==1 ? true : false;
  return image;
}


// create an 8 bit test image
static vil3d_image_view<vxl_byte> CreateTest8bitImage(int wd, int ht, int dt)
{
  vil3d_image_view<vxl_byte> image(wd, ht, dt);
  for (int k = 0; k < dt; k++)
    for (int j = 0; j < ht; j++)
      for (int i = 0; i < wd; i++)
        image(i, j, k) = ((i-wd/2)*(j-ht/2)*(k-dt/2)) & 0xff;
  return image;
}

// create a 16 bit test image
static vil3d_image_view<vxl_uint_16> CreateTestU16bitImage(int wd, int ht, int dt)
{
  vil3d_image_view<vxl_uint_16> image(wd, ht, dt);
  for (int k = 0; k < dt; k++)
    for (int j = 0; j < ht; j++)
      for (int i = 0; i < wd; i++)
        image(i, j, k) = ((i-wd/2)*(j-ht/2)/8*(k-dt/2)/64) & 0xffff;
  return image;
}

// create a 16 bit test image
static vil3d_image_view<vxl_int_16> CreateTest16bitImage(int wd, int ht, int dt)
{
  vil3d_image_view<vxl_int_16> image(wd, ht, dt);
  for (int k = 0; k < dt; k++)
    for (int j = 0; j < ht; j++)
      for (int i = 0; i < wd; i++)
        image(i, j, k) = ((i-wd/2)*(j-ht/2)/8*(k-dt/2)/64) & 0xffff;
  return image;
}

// create a 32 bit test image
static vil3d_image_view<vxl_uint_32> CreateTestU32bitImage(int wd, int ht, int dt)
{
  vil3d_image_view<vxl_uint_32> image(wd, ht, dt);
  for (int k = 0; k < dt; k++)
    for (int j = 0; j < ht; j++)
      for (int i = 0; i < wd; i++)
        image(i, j, k) = i + wd*j + dt*k;
  return image;
}


// create a 32 bit test image
static vil3d_image_view<vxl_int_32> CreateTest32bitImage(int wd, int ht, int dt)
{
  vil3d_image_view<vxl_int_32> image(wd, ht, dt);
  for (int k = 0; k < dt; k++)
    for (int j = 0; j < ht; j++)
      for (int i = 0; i < wd; i++)
        image(i, j, k) = i + wd*j + dt*k;
  return image;
}


// create a float-pixel test image
static vil3d_image_view<float> CreateTestfloatImage(int wd, int ht, int dt)
{
  vil3d_image_view<float> image(wd, ht, dt);
  for (int k = 0; k < dt; k++)
    for (int j = 0; j < ht; j++)
      for (int i = 0; i < wd; i++)
        image(i, j, k) = 0.01f * ((i-wd/2)*(j-ht/2)/16);
  return image;
}

static void test_save_load_image()
{
  // create test images
  int sizex = 101;
  int sizey = 32;
  int sizez = 37;
  vil3d_image_view<bool>                image1 = CreateTest1bitImage(sizex, sizey, sizez);
  vil3d_image_view<vxl_byte>            image8 = CreateTest8bitImage(sizex, sizey, sizez);
  vil3d_image_view<vxl_uint_16>         imageu16 = CreateTestU16bitImage(sizex, sizey, sizez);
  vil3d_image_view<vxl_int_16>          image16 = CreateTest16bitImage(sizex, sizey, sizez);
  vil3d_image_view<vxl_uint_32>         imageu32 = CreateTestU32bitImage(sizex, sizey, sizez);
  vil3d_image_view<vxl_int_32>          image32 = CreateTest32bitImage(sizex, sizey, sizez);
  vil3d_image_view<float>               imagefloat = CreateTestfloatImage(sizex, sizey, sizez);

  float voxel_size[3] = { 0.5, 2, 5 };

  vil3d_test_image_type("gipl", image8, voxel_size);
  vil3d_test_image_type("gipl", image16, voxel_size);
  vil3d_test_image_type("gipl", imageu16, voxel_size);
  vil3d_test_image_type("gipl", image32, voxel_size);
  vil3d_test_image_type("gipl", imageu32, voxel_size);
  vil3d_test_image_type("gipl", imagefloat, voxel_size);


  vil3d_test_image_type("hdr", image8, voxel_size);
  vil3d_test_image_type("hdr", image16, voxel_size);
  vil3d_test_image_type("hdr", image32, voxel_size);
  vil3d_test_image_type("hdr", imagefloat, voxel_size);

#if 0 // v3i is only defined in vimt3d
  vil3d_test_image_type("v3i", image1, voxel_size);
  vil3d_test_image_type("v3i", image8, voxel_size);
  vil3d_test_image_type("v3i", image16, voxel_size);
  vil3d_test_image_type("v3i", image32, voxel_size);
  vil3d_test_image_type("v3i", imagefloat, voxel_size);
#endif
}

TESTMAIN(test_save_load_image);
