// This is brl/bbas/bgui/tests/test_histogram.cxx
#include <testlib/testlib_test.h>

#include <vcl_limits.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_histogram.h>

// create a 16 bit test image
static vil_image_view<unsigned short> CreateTest16bitImage(int wd, int ht)
{
  vil_image_view<unsigned short> image(wd, ht);
  for (int j = 0; j < ht; j++)
    for (int i = 0; i < wd; i++)
      image(i,j) = (unsigned short)(((i-wd/2)*(j-ht/2)/16)&0xffff);
  return image;
}

static void test_histogram(int argc, char* argv[])
{
  vil_image_view<unsigned short> img;
  if (argc > 1) {
    vcl_cout << "Loading image " << argv[1] << " ... ";
    img = vil_load(argv[1]);
    if (!img) vcl_cout << "failed!!\n";
    else      vcl_cout << "done.\n";
  }
  if (!img)
    img = CreateTest16bitImage(1000,1000);

  vcl_cout << img.ni() << 'x' << img.nj() << '\n'
           << img(455,544) << '\n';
  vcl_vector<double> histogram;
  int nbins = 1<< (8*sizeof(unsigned short));
  vil_histogram(img,histogram, vcl_numeric_limits<unsigned short>::min(),
                vcl_numeric_limits<unsigned short>::max(), nbins);

  for (int i =0; i<nbins; i+=200)
    vcl_cout << "H[" << i << "]=" << histogram[i] << '\n';

    vcl_cout << "short [" << vcl_numeric_limits<short>::min()
             << ' ' << vcl_numeric_limits<short>::max()  << "]\n"

             << "unsigned short[" << vcl_numeric_limits<unsigned short>::min()
             << ' ' << vcl_numeric_limits<unsigned short>::max()  << "]\n"

             << "int [" << vcl_numeric_limits<int>::min()
             << ' ' << vcl_numeric_limits<int>::max()  << "]\n"

             << "unsigned int [" << vcl_numeric_limits<unsigned int>::min()
             << ' ' << vcl_numeric_limits<unsigned int>::max()  << "]\n"

             << "long [" << vcl_numeric_limits< long>::min()
             << ' ' << vcl_numeric_limits< long>::max()  << "]\n"

             << "unsigned  long [" << vcl_numeric_limits<unsigned  long>::min()
             << ' ' << vcl_numeric_limits<unsigned  long>::max()  << "]\n"

             << "float [" << -vcl_numeric_limits< float>::max()
             << ' ' << vcl_numeric_limits< float>::max()  << "]\n"

             << "double [" << -vcl_numeric_limits< double>::max()
             << ' ' << vcl_numeric_limits< double>::max()  << "]\n"

             << "long double [" << -vcl_numeric_limits< long double>::max()
             << ' ' << vcl_numeric_limits< long double>::max()  << "]\n";
}

TESTMAIN_ARGS(test_histogram);
