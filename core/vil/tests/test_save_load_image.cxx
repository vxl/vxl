// This is core/vil/tests/test_save_load_image.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \author Peter.Vanroose@esat.kuleuven.ac.be
// \date 17 February, 2000
//
// \verbatim
//  Modifications
//   18 Jul 2000 - add vil1_buffer, and mit bug fix.
//   10 Aug 2000 - Peter Vanroose - added non-exact match
//   10 Aug 2000 - Peter Vanroose - added bmp and mit tests
//   10 Aug 2000 - Peter Vanroose - test TIFF and JPEG presence
//   26 Aug 2000 - .bmp loader works (again) on solaris + linux
//   28 Aug 2000 - Peter Vanroose - bmp write of colour images fixed
//    6 Dec 2000 - vil_rgb<vxl_byte> deprecated
//   21 Jan 2001 - deprecated vil1_buffer<> - use vcl_vector<> instead
//    1 May 2001 - Peter Vanroose - now using vil1_test.h
//   7 June 2001 - Peter Vanroose - test added for pbm images
//   14 Apr 2002 - Amitha Perera - switched from vil1_test to testlib
//   26 Aug 2002 - Ian Scott - Started conversion to vil
//    2 Oct 2002 - Peter Vanroose - replaced image24 tests by image3p
//      Jun 2003 - Peter Vanroose - added viff, iris, mit tests for vil
// \endverbatim

#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vxl_config.h> // for vxl_byte

#include <vul/vul_temp_filename.h>
#include <vpl/vpl.h> // vpl_unlink()

#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_print.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>

#ifndef LEAVE_IMAGES_BEHIND
#define LEAVE_IMAGES_BEHIND 0
#endif


//: Test to see if all the pixels in two images are equal
template <class T>
bool test_image_equal(char const* type_name,
                      vil_image_view<T> const & image,
                      vil_image_view_base_sptr const& pimage2,
                      bool exact = true)
{
  vil_image_view<T> image2 = *pimage2;

  int sizex = image.ni();
  int sizey = image.nj();
  int planes = image.nplanes();
  int sizex2 = image2.ni();
  int sizey2 = image2.nj();
  int planes2 = image2.nplanes();

  // make sure saved image has the same pixels as the original image
  TEST("Loaded image can be viewed as same type as saved image", !image2, false);
  if (!image2)
  {
    vcl_cout << "read back image type has pixel type " << pimage2->pixel_format()
             << " instead of (as written) " << image.pixel_format() << '\n' << vcl_flush;
    return false;
  }

  TEST("Image dimensions", sizex == sizex2 && sizey == sizey2, true);
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
  vcl_cout << "istep()=" << image2.istep() << ", jstep()=" << image2.jstep()
           << ", planestep()=" << image2.planestep() << '\n' << vcl_flush;
  TEST("istep is non-zero", image2.istep()==0, false);
  TEST("|istep| is at most 3xheight",  3*sizey2+image2.istep()>=0 && 3*sizey2-image2.istep()>=0, true);
  TEST("jstep is non-zero", image2.jstep()==0, false);
  TEST("|jstep| is at most 3xwidth", 3+3*sizex2+image2.jstep()>=0 && 3+3*sizex2-image2.jstep()>=0, true);
  // The "+3" is there to allow for "row word alignment", e.g. with the BMP format.
  TEST("planestep is non-zero", image2.planestep()==0, false);
  TEST("|planestep| is at most widthxheight",
       (3+sizex2)*sizey2+image2.planestep()>=0 && (3+sizex2)*sizey2-image2.planestep()>=0, true);

  if (100*sizex2+image2.jstep()<0 || 100*sizex2-image2.jstep()<0)
  {
    vcl_cout << "*** Something is terribly wrong with image2's jstep() ***\n";
    return false;
  }
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
  else
    return true;
}

// create a colour gif image
static bool create_colour_gif(const char* filename)
{
#ifdef VCL_VC
#pragma warning ( push )
#pragma warning ( disable : 4305 4309)
#endif
  unsigned char a[] = { 253, 0, 155, 0, 247, 0, 0 };
  unsigned char b[] = {
    44, 0, 0, 0, 0, 253, 0, 155, 0, 0, 8, 254, 0, 1, 164, 74, 150, 45, 87, 162, 76, 121, 14, 80, 168, 224, 226, 5, 140,
    24, 98, 198, 144, 41, 99, 230, 12, 154, 52, 202, 150, 49, 107, 230, 236, 25, 180, 104, 210, 166, 81, 171, 102, 237, 26, 182,
    108, 1, 16, 36, 176, 112, 1, 67, 6, 25, 51, 104, 212, 176, 113, 3, 71, 14, 53, 107, 216, 180, 113, 243, 6, 78, 28, 57, 115,
    232, 212, 177, 115, 7, 79, 30, 1, 10, 22, 104, 216, 192, 161, 131, 142, 29, 60, 122, 248, 248, 1, 36, 136, 158, 61, 124, 250,
    248, 249, 3, 40, 144, 160, 65, 132, 10, 25, 58, 132, 40, 17, 1, 7, 15, 68, 140, 32, 81, 66, 201, 18, 38, 77, 156, 60, 129, 18,
    69, 211, 38, 78, 157, 60, 125, 2, 21, 74, 212, 40, 82, 165, 76, 157, 66, 149, 106, 0, 131, 6, 30, 62, 128, 8, 33, 100, 8, 145,
    34, 70, 142, 32, 73, 162, 104, 17, 163, 70, 142, 30, 65, 138, 36, 105, 18, 165, 74, 150, 46, 97, 202, 84, 0, 66, 4, 19, 39,
    80, 164, 144, 50, 133, 74, 21, 43, 87, 176, 100, 81, 181, 138, 85, 43, 87, 175, 96, 197, 146, 53, 139, 86, 45, 91, 183, 112,
    229, 50, 32, 97, 130, 138, 21, 44, 90, 104, 217, 194, 165, 139, 151, 47, 96, 194, 232, 218, 197, 171, 151, 175, 95, 192, 130,
    9, 27, 70, 172, 152, 177, 99, 200, 146, 254, 1, 24, 79, 160, 60, 129, 241, 2, 0, 8, 88, 63, 62, 64, 0, 0, 238, 199, 27, 0, 96,
    160, 126, 251, 241, 248, 243, 235, 223, 207, 191, 191, 255, 255, 0, 6, 40, 224, 128, 4, 22, 104, 224, 129, 8, 38, 168, 224,
    130, 12, 54, 232, 224, 131, 16, 70, 40, 225, 132, 20, 86, 104, 225, 133, 24, 102, 168, 225, 134, 28, 118, 232, 225, 135, 32,
    134, 40, 226, 136, 36, 150, 104, 226, 137, 40, 166, 168, 226, 138, 44, 182, 232, 226, 139, 48, 198, 40, 227, 140, 52, 214,
    104, 227, 141, 56, 230, 168, 227, 142, 60, 246, 232, 227, 143, 64, 6, 41, 228, 144, 68, 22, 105, 228, 145, 72, 38, 169, 228,
    146, 76, 54, 233, 228, 147, 80, 70, 41, 229, 148, 84, 86, 105, 229, 149, 88, 102, 169, 229, 150, 92, 118, 233, 229, 151, 96,
    134, 41, 230, 152, 100, 150, 105, 230, 153, 104, 166, 169, 230, 154, 108, 182, 233, 230, 155, 112, 198, 41, 231, 156, 116,
    214, 105, 231, 157, 120, 230, 169, 231, 158, 124, 246, 233, 231, 159, 128, 6, 42, 232, 160, 132, 22, 106, 232, 161, 136, 38,
    170, 232, 162, 140, 54, 234, 232, 163, 144, 70, 42, 233, 164, 148, 86, 106, 233, 165, 152, 102, 170, 233, 166, 156, 118, 234,
    233, 167, 160, 134, 42, 234, 168, 164, 150, 106, 234, 169, 168, 166, 170, 234, 170, 172, 182, 234, 234, 171, 118, 176, 198,
    42, 235, 172, 180, 214, 106, 235, 173, 184, 230, 170, 235, 174, 188, 246, 234, 235, 175, 192, 6, 43, 236, 176, 196, 22, 107,
    236, 177, 200, 38, 171, 236, 178, 204, 54, 235, 236, 179, 208, 70, 43, 237, 180, 212, 86, 107, 237, 181, 216, 102, 171, 237,
    182, 220, 118, 235, 237, 183, 224, 134, 43, 238, 184, 228, 150, 107, 238, 185, 232, 166, 171, 238, 186, 236, 182, 235, 238,
    187, 240, 198, 43, 239, 188, 244, 214, 107, 239, 189, 248, 230, 171, 239, 190, 252, 246, 235, 239, 191, 0, 7, 44, 240, 192, 4,
    23, 108, 240, 193, 8, 39, 172, 240, 194, 168, 6, 4, 0, 59 };
#ifdef VCL_VC
#pragma warning ( pop )
#endif
  vcl_ofstream f(filename, vcl_ios_binary);
  if (!f) return false;
  f << "GIF87a";
  for (int i=0; i<7; ++i) f << a[i];
  for (int i=0; i<256; ++i) f << (unsigned char)i << (unsigned char)i << (unsigned char)0;
  for (int i=0; i<642; ++i) f << b[i];
  f.close();
  return true;
}

// create a grey gif image
static bool create_grey_gif(const char* filename)
{
#ifdef VCL_VC
#pragma warning ( push )
#pragma warning ( disable : 4305 4309)
#endif
  unsigned char a[] = { 253, 0, 155, 0, 247, 0, 0 };
  unsigned char b[] = {
    44, 0, 0, 0, 0, 253, 0, 155, 0, 0, 8, 254, 0, 1, 8, 28, 72, 176, 160, 193, 131, 8, 19, 42, 92, 200, 176, 161, 195,
    135, 16, 35, 74, 156, 72, 177, 162, 197, 139, 24, 51, 106, 220, 200, 177, 163, 199, 143, 32, 67, 138, 28, 73, 178, 164, 201,
    147, 40, 83, 170, 92, 201, 178, 165, 203, 151, 48, 99, 202, 156, 73, 179, 166, 205, 155, 56, 115, 234, 220, 201, 179, 167,
    207, 159, 64, 131, 10, 29, 74, 180, 168, 209, 163, 72, 147, 42, 93, 202, 180, 169, 211, 167, 80, 163, 74, 157, 74, 181, 170,
    213, 171, 88, 179, 106, 221, 202, 181, 171, 215, 175, 96, 195, 138, 29, 75, 182, 172, 217, 179, 104, 211, 170, 93, 203, 182,
    173, 219, 183, 112, 227, 202, 157, 75, 183, 174, 221, 187, 120, 243, 234, 221, 203, 183, 175, 223, 191, 128, 3, 11, 30, 76,
    184, 176, 225, 195, 136, 19, 43, 94, 204, 184, 177, 227, 199, 144, 35, 75, 158, 76, 185, 178, 229, 203, 152, 51, 107, 222,
    204, 185, 179, 231, 207, 160, 67, 139, 30, 77, 186, 180, 233, 211, 168, 83, 171, 94, 205, 186, 181, 235, 215, 176, 99, 203,
    158, 77, 187, 182, 237, 219, 184, 115, 235, 222, 205, 187, 183, 239, 223, 192, 131, 11, 31, 78, 188, 184, 241, 227, 200, 147,
    43, 95, 206, 188, 185, 243, 231, 208, 163, 75, 159, 78, 189, 186, 245, 235, 216, 179, 107, 223, 206, 189, 187, 247, 239, 224,
    195, 67, 139, 31, 79, 190, 188, 249, 243, 232, 211, 171, 95, 207, 190, 189, 251, 247, 240, 227, 203, 159, 79, 191, 190, 253,
    251, 248, 243, 235, 223, 207, 191, 191, 255, 255, 0, 6, 40, 224, 128, 4, 22, 104, 224, 129, 8, 38, 168, 224, 130, 12, 54, 232,
    224, 131, 16, 70, 40, 225, 132, 20, 86, 104, 225, 133, 155, 5, 4, 0, 59 };
#ifdef VCL_VC
#pragma warning ( pop )
#endif
  vcl_ofstream f(filename, vcl_ios_binary);
  if (!f) return false;
  f << "GIF87a";
  for (int i=0; i<7; ++i) f << a[i];
  for (int i=0; i<256; ++i) f << (unsigned char)i << (unsigned char)i << (unsigned char)i;
  for (int i=0; i<336; ++i) f << b[i];
  f.close();
  return true;
}


template<class T>
void vil_test_image_type(char const* type_name, // type for image to read and write
                         vil_image_view<T> const & image, // test image to save and restore
                         bool exact = true,  // require read back image identical
                         bool fail_save = false) // expect fail on save if true
{
  vcl_cout << "=== Start testing " << type_name << " (" << sizeof(T)
           << " bpp) ===\n" << vcl_flush;

  // Step 1) Write the image out to disk
  //
  // create a file name
  vcl_string fname = vul_temp_filename();
  fname += ".";
  if (type_name) fname += type_name;

  vcl_cout << "vil_test_image_type: Save " << image.is_a() <<
              " to [" << fname << "]\n" << vcl_flush;

  // Write image to disk
  if (vcl_strcmp(type_name, "gif") == 0 &&
      vil_pixel_format_num_components( image.pixel_format() ) == 3)
  {
    if (!create_colour_gif(fname.c_str()))
      return; // fatal error
  }
  else if (vcl_strcmp(type_name, "gif") == 0)
  {
    if (!create_grey_gif(fname.c_str()))
      return; // fatal error
  }
  else
  {
    bool tst = vil_save(image, fname.c_str(), type_name);
    TEST("write image to disk", tst, !fail_save);
    if (!tst) return;
  }

  // STEP 2) Read the image that was just saved to file
  {
    vil_image_view_base_sptr image2 = vil_load(fname.c_str());
    TEST("load image", !image2, false);
    if (!image2)
    {
      return; // fatal error
    }

    test_image_equal(type_name, image, image2, exact);
  }

#if !LEAVE_IMAGES_BEHIND
  vpl_unlink(fname.c_str());
#endif
}


// create a 1 bit test image
vil_image_view<bool> CreateTest1bitImage(int wd, int ht)
{
  vil_image_view<bool> image(wd, ht, 1);
  for (int j = 0; j < ht; ++j) {
    for (int i = 0; i < wd; i++)
      image(i,j) = (i*j)%2 ==1 ? true : false;
  }
  return image;
}


// create an 8 bit test image
vil_image_view<vxl_byte> CreateTest8bitImage(int wd, int ht)
{
  vil_image_view<vxl_byte> image(wd, ht);
  for (int j = 0; j < ht; j++)
    for (int i = 0; i < wd; i++) {
      image(i,j) = ((i-wd/2)*(j-ht/2)) & 0xff;
    }
  return image;
}

// create a 16 bit test image
vil_image_view<vxl_uint_16> CreateTest16bitImage(int wd, int ht)
{
  vil_image_view<vxl_uint_16> image(wd, ht);
  for (int j = 0; j < ht; j++)
    for (int i = 0; i < wd; i++) {
      image(i,j) = ((i-wd/2)*(j-ht/2)/16) & 0xffff;
  }
  return image;
}


// create a 32 bit test image
vil_image_view<vxl_uint_32> CreateTest32bitImage(int wd, int ht)
{
  vil_image_view<vxl_uint_32> image(wd, ht);
  for (int j = 0; j < ht; j++)
    for (int i = 0; i < wd; i++)
      image(i, j) = i + wd*j;
  return image;
}

// create a 24 bit color test image
vil_image_view<vil_rgb<vxl_byte> > CreateTest24bitImage(int wd, int ht)
{
  vil_image_view<vil_rgb<vxl_byte> > image(wd, ht);
  for (int i = 0; i < wd; i++)
    for (int j = 0; j < ht; j++)
      image(i,j) = vil_rgb<vxl_byte>(i%(1<<8), ((i-wd/2)*(j-ht/2)/16) % (1<<8), ((j/3)%(1<<8)));
  return image;
}

// create a 24 bit color test image, with 3 planes
vil_image_view<vxl_byte> CreateTest3planeImage(int wd, int ht)
{
  vil_image_view<vxl_byte> image( wd, ht, 3);
  for (int i = 0; i < wd; i++)
    for (int j = 0; j < ht; j++) {
      image(i,j,0) = i%(1<<8);
      image(i,j,1) = ((i-wd/2)*(j-ht/2)/16) % (1<<8);
      image(i,j,2) = ((j/3)%(1<<8));
    }
  return image;
}

// create a float-pixel test image
vil_image_view<float> CreateTestfloatImage(int wd, int ht)
{
  vil_image_view<float> image(wd, ht);
  for (int i = 0; i < wd; i++)
    for (int j = 0; j < ht; j++)
      image(i,j) = 0.01f * ((i-wd/2)*(j-ht/2)/16);
  return image;
}

// create a double-pixel test image
vil_image_view<double> CreateTestdoubleImage(int wd, int ht)
{
  vil_image_view<double> image(wd, ht);
  for (int i = 0; i < wd; i++)
    for (int j = 0; j < ht; j++)
      image(i,j) = 0.01 * ((i-wd/2)*(j-ht/2)/16);
  return image;
}


static void test_vil_save_image_resource()
{
  vil_image_view<vxl_byte> view = CreateTest8bitImage(251, 153);
  vil_image_resource_sptr mem = vil_new_image_resource_of_view(view);
  const char* out_path ="test_save_image_resource.pgm";
  TEST("Saving image resource",vil_save_image_resource(mem, out_path), true);
  vil_image_view<vxl_byte> loaded_view = vil_load(out_path);
  TEST("Loaded correct image", vil_image_view_deep_equality(view, loaded_view),true);
#if !LEAVE_IMAGES_BEHIND
    vpl_unlink(out_path);
#endif
}


static void test_save_load_image()
{
  // create test images
  int sizex = 253;
  int sizey = 155;
  vil_image_view<bool>                image1 = CreateTest1bitImage(sizex, sizey);
  vil_image_view<vxl_byte>            image8 = CreateTest8bitImage(sizex, sizey);
  vil_image_view<vxl_uint_16>         image16 = CreateTest16bitImage(sizex, sizey);
#if 0 // read back pixel type will not match: ppm is always read in as 3-plane image
  vil_image_view<vil_rgb<vxl_byte> >  image24 = CreateTest24bitImage(sizex, sizey);
#endif
  vil_image_view<vxl_uint_32>         image32 = CreateTest32bitImage(sizex, sizey);
  vil_image_view<vxl_byte>            image3p = CreateTest3planeImage(sizex, sizey);
  vil_image_view<float>               imagefloat = CreateTestfloatImage(sizex, sizey);
  vil_image_view<double>              imagedouble = CreateTestdoubleImage(sizex, sizey);


  // PNG
#if 1
  vil_test_image_type("png", image8);
  vil_test_image_type("png", image16);
  vil_test_image_type("png", image3p);
#endif


  // JPEG
#if HAS_JPEG
  // lossy format ==> not guaranteed to be identical (hence arg. 3 set to false)
  vil_test_image_type("jpeg", image8, false);
#if 0
  vil_test_image_type("jpeg", image16, false);
  vil_test_image_type("jpeg", image3p, false);
#endif

  // Test small image
  {
    unsigned ni =30, nj=29;
    vil_image_view<vxl_byte> small_greyscale_image(ni,nj);
    for (unsigned j=0;j<nj;++j)
      for (unsigned i=0;i<ni;++i) small_greyscale_image(i,j)=(i+j)*4;
//    vil_print_all(vcl_cout, small_greyscale_image);
    vil_test_image_type("jpeg", small_greyscale_image, false);
    vcl_string out_path("test_save_load_jpeg.jpg");
    TEST("Saving JPEG",vil_save(small_greyscale_image, out_path.c_str()),true);

    vil_image_view<vxl_byte> new_image = vil_load(out_path.c_str());
    TEST("JPEG Size correct",new_image.ni()==ni && new_image.nj()==nj, true);
    double sum2 = 0;
    for (unsigned i=0;i<ni;++i)
    { double d=double(small_greyscale_image(i,17))-new_image(i,17); sum2+=d*d; }
    TEST_NEAR("Loaded image close to original",sum2,0.0,2*ni);
#if !LEAVE_IMAGES_BEHIND
    vpl_unlink(out_path.c_str());
#endif
  }
#endif


  // pnm ( = PBM / PGM / PPM )
#if 1
  vil_test_image_type("pnm", image1);
  vil_test_image_type("pbm", image1);
  vil_test_image_type("pnm", image8);
  vil_test_image_type("pgm", image8);
  vil_test_image_type("pnm", image16);
  vil_test_image_type("pnm", image32);
  vil_test_image_type("pnm", image3p);
  vil_test_image_type("ppm", image3p);
#endif


  // bmp
#if 1
  vil_test_image_type("bmp", image8);
  vil_test_image_type("bmp", image3p);
#endif


  // VIFF image (Khoros 1.0)
#if 1
//vil_test_image_type("viff", image1);
  vil_test_image_type("viff", image8);
  vil_test_image_type("viff", image16);
  vil_test_image_type("viff", image32);
  vil_test_image_type("viff", image3p);
  vil_test_image_type("viff", imagefloat);
  vil_test_image_type("viff", imagedouble);
#endif


  // TIFF
  vil_test_image_type("tiff", image8);
  vil_test_image_type("tiff", image3p);
#if 0
  vil_test_image_type("tiff", image1, true, true); // Test that boolean doesn't work
  // Test that >1 byte pixels don't work yet:
  vil_test_image_type("tiff", image16, true, true);
  vil_test_image_type("tiff", image32, true, true);
  vil_test_image_type("tiff", imagefloat, true, true);
  vil_test_image_type("tiff", imagedouble, true, true);
#endif


  // GIF (read-only)
#if 0
  // lossy format ==> not guaranteed to be identical (hence arg. 3 set to false)
  vil_test_image_type("gif", image8, false);
  vil_test_image_type("gif", image3p, false);
#endif


  // SGI "iris" rgb
#if 1
  vil_test_image_type("iris", image8);
  vil_test_image_type("iris", image16);
  vil_test_image_type("iris", image3p);
#endif


  // mit
#if 1
//vil_test_image_type("mit", image1); // Test that boolean doesn't work
  vil_test_image_type("mit", image8);
  vil_test_image_type("mit", image16);
  vil_test_image_type("mit", image32);
  vil_test_image_type("mit", image3p);
#endif


  // requires rnm support
#if 1
  test_vil_save_image_resource();
#endif
}

TESTMAIN(test_save_load_image);
