// This is brl/bbas/vgui/tests/vgui_test_histogram.cxx
#include <testlib/testlib_test.h>
#include <vxl_config.h>
#include <vcl_climits.h> // for CHAR_BIT
#include <vil/vil_pixel_traits.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_histogram.h>
static void test_histogram()
{
  vil_image_view<unsigned short> img;
  img = vil_load("c:/images/CAD/Mouse/18_al05_1770_0365.tif");
  vcl_cout << img(500,500) << '\n';
  vcl_vector<double> histogram;
  int nbins = 1<< vil_pixel_traits<unsigned short>::num_bits();
  vil_histogram(img,histogram, vil_pixel_traits<unsigned short>::minval(),
                vil_pixel_traits<unsigned short>::maxval(), nbins);
               
  for(int i =0; i<nbins; i+=200)
	vcl_cout << "H[" << i << "]=" << histogram[i] << '\n';




	vcl_cout << "short [" << vil_pixel_traits<short>::minval() 
           << ' ' << vil_pixel_traits<short>::maxval()  << "]\n";

	vcl_cout << "unsigned short[" << vil_pixel_traits<unsigned short>::minval() 
           << ' ' << vil_pixel_traits<unsigned short>::maxval()  << "]\n";

	vcl_cout << "int [" << vil_pixel_traits<int>::minval() 
           << ' ' << vil_pixel_traits<int>::maxval()  << "]\n";

	vcl_cout << "unsigned int [" << vil_pixel_traits<unsigned int>::minval() 
           << ' ' << vil_pixel_traits<unsigned int>::maxval()  << "]\n";

	vcl_cout << "long [" << vil_pixel_traits< long>::minval() 
           << ' ' << vil_pixel_traits< long>::maxval()  << "]\n";

	vcl_cout << "unsigned  long [" << vil_pixel_traits<unsigned  long>::minval() 
           << ' ' << vil_pixel_traits<unsigned  long>::maxval()  << "]\n";

	vcl_cout << "float [" << vil_pixel_traits< float>::minval() 
           << ' ' << vil_pixel_traits< float>::maxval()  << "]\n";

	vcl_cout << "double [" << vil_pixel_traits< double>::minval() 
           << ' ' << vil_pixel_traits< double>::maxval()  << "]\n";

	vcl_cout << "long double [" << vil_pixel_traits< long double>::minval() 
           << ' ' << vil_pixel_traits< long double>::maxval()  << "]\n";
}
TESTMAIN(test_histogram);
