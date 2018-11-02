// This is brl/bbas/bgui/tests/test_histogram.cxx
#include <iostream>
#include <limits>
#include <testlib/testlib_test.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
    std::cout << "Loading image " << argv[1] << " ... ";
    img = vil_load(argv[1]);
    if (!img) std::cout << "failed!!\n";
    else      std::cout << "done.\n";
  }
  if (!img)
    img = CreateTest16bitImage(1000,1000);

  std::cout << img.ni() << 'x' << img.nj() << '\n'
           << img(455,544) << '\n';
  std::vector<double> histogram;
  int nbins = 1<< (8*sizeof(unsigned short));
  vil_histogram(img,histogram, std::numeric_limits<unsigned short>::min(),
                std::numeric_limits<unsigned short>::max(), nbins);

  for (int i =0; i<nbins; i+=200)
    std::cout << "H[" << i << "]=" << histogram[i] << '\n';

    std::cout << "short [" << std::numeric_limits<short>::min()
             << ' ' << std::numeric_limits<short>::max()  << "]\n"

             << "unsigned short[" << std::numeric_limits<unsigned short>::min()
             << ' ' << std::numeric_limits<unsigned short>::max()  << "]\n"

             << "int [" << std::numeric_limits<int>::min()
             << ' ' << std::numeric_limits<int>::max()  << "]\n"

             << "unsigned int [" << std::numeric_limits<unsigned int>::min()
             << ' ' << std::numeric_limits<unsigned int>::max()  << "]\n"

             << "long [" << std::numeric_limits< long>::min()
             << ' ' << std::numeric_limits< long>::max()  << "]\n"

             << "unsigned  long [" << std::numeric_limits<unsigned  long>::min()
             << ' ' << std::numeric_limits<unsigned  long>::max()  << "]\n"

             << "float [" << -std::numeric_limits< float>::max()
             << ' ' << std::numeric_limits< float>::max()  << "]\n"

             << "double [" << -std::numeric_limits< double>::max()
             << ' ' << std::numeric_limits< double>::max()  << "]\n"
#ifdef INCLUDE_LONG_DOUBLE_TESTS
             << "long double [" << -std::numeric_limits< long double>::max()
             << ' ' << std::numeric_limits< long double>::max()  << "]\n"
#endif
             << "\n";
}

TESTMAIN_ARGS(test_histogram);
