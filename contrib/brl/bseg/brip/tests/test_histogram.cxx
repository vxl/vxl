#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vil/vil_image_view.h>
#include <brip/brip_histogram.h>

template <class T>
void test_hist(const vil_image_view<T>& image,
               double min, double max, unsigned n_bins)
{
  vcl_vector<double> hist;

  double sum = brip_histogram(image, hist, min, max, n_bins);
  double test_sum = 0.0;

  //vcl_cout << "Histogram: ";
  for (unsigned int i=0; i<hist.size(); ++i){
    //vcl_cout << hist[i] << ' ';
    test_sum += hist[i];
  }
  //vcl_cout << vcl_endl;
  //vcl_cout << "Sum: " << sum << vcl_endl;

  vcl_stringstream test_name;
  test_name << "Validate Sums (" << n_bins <<" bins)";
  testlib_test_begin(test_name.str().c_str());
  testlib_test_perform(test_sum == sum && sum == image.ni()*image.nj()*image.nplanes());
}

template <class T>
void test_hist_weight(const vil_image_view<T>& image,
                      const vil_image_view<double>& weights,
                      double min, double max, unsigned n_bins)
{
  double test_sum = 0.0;
  for (unsigned int i=0; i<weights.ni(); ++i)
    for (unsigned int j=0; j<weights.nj(); ++j)
      for (unsigned int p=0; p<weights.nplanes(); ++p)
        test_sum += weights(i,j,p);

  vcl_vector<double> hist;
  double sum = brip_weighted_histogram(image, weights, hist, min, max, n_bins);

  double verify_sum = 0.0;
  for (unsigned int i=0; i<hist.size(); ++i)
    verify_sum += hist[i];

  vcl_stringstream test_name;
  test_name<<"Validate Weights ("<<n_bins<<" bins, range "<<min<<"--"<<max<<')';
  testlib_test_begin(test_name.str().c_str());
  testlib_test_assert_near("sum", sum, test_sum, 1e-9);

  vcl_stringstream t;
  t<<"Validate histogram sum ("<<n_bins<<" bins, range "<<min<<"--"<<max<<')';
  testlib_test_begin(test_name.str().c_str());
  testlib_test_assert_near("sum", verify_sum, test_sum, 1e-9);
}

template <class T>
void test_hist_joint(const vil_image_view<T>& image1,
                     const vil_image_view<T>& image2,
                     double min, double max, unsigned n_bins)
{
  vcl_vector<vcl_vector<double> > hist;

  double sum = brip_joint_histogram(image1, image2, hist, min, max, n_bins);
  double test_sum = 0.0;

  for (unsigned int i=0; i<n_bins; ++i) {
    for (unsigned int j=0; j<n_bins; ++j) {
      test_sum += hist[i][j];
      //vcl_cout << hist[i][j] << ' ';
    }
    //vcl_cout << vcl_endl;
  }

  //vcl_cout << "Sum: " << sum << vcl_endl;

  vcl_stringstream test_name;
  test_name << "Validate Joint Sums (" << n_bins <<"^2 bins)";
  testlib_test_begin(test_name.str().c_str());
  testlib_test_perform(test_sum == sum);
}

static void test_histogram()
{
  unsigned int ni=256, nj=256;
  vil_image_view<vxl_byte> image1(ni,nj), image2(ni,nj);
  vil_image_view<double> image3(ni,nj);
  for (unsigned j=0;j<nj;++j){
    for (unsigned i=0;i<ni;++i){
      image1(i,j) = vxl_byte((i+j-1)/2);
      image2(i,j) = vxl_byte(i+3);
      image3(i,j) = (i+j+7)/555.0; // so values are between 0 and 1 (= weights)
    }
  }

  ni = 1000; nj = 100;
  vil_image_view<double> image4(ni,nj);
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
      image4(i,j) = 10.0/(i+j+10);

  test_hist(image1, 0, 255, 16);
  test_hist(image1, 0, 255, 256);
  test_hist(image2, 0, 255, 16);
  test_hist(image2, 0, 255, 256);
  test_hist(image3, 0, 1, 5);
  test_hist(image3, 0, 1, 100);
  test_hist(image4, 0, 1, 5);
  test_hist(image4, 0, 1, 100);

  test_hist_weight(image1, image3, 0, 255, 20);
  test_hist_weight(image1, image3, 0, 255, 200);
  test_hist_weight(image3, image3, 0, 1, 20);
  test_hist_weight(image3, image3, 0, 1, 200);

  // extremal cases
  test_hist_weight(image1, image3, 0, 255, 1);
  test_hist_weight(image3, image3, 0, 1, 1);
  test_hist_weight(image1, image3, 0, 255, 1000000);
  test_hist_weight(image3, image3, 0, 1, 1000000);
  // add empty bins
  test_hist_weight(image1, image3, -100, 1000, 200);
  test_hist_weight(image3, image3, -1, 2, 200);

  // include negative weights
  image3(1,2) = -1; image3(5,3) = -0.5;
  test_hist_weight(image1, image3, 0, 255, 20);
  test_hist_weight(image1, image3, 0, 255, 200);
  test_hist_weight(image3, image3, 0, 255, 20);
  test_hist_weight(image3, image3, 0, 255, 200);
  test_hist_weight(image1, image3, -100, 1000, 200);
  test_hist_weight(image3, image3, -1, 2, 200);

  test_hist_joint(image1, image2, 0, 255, 16);
  test_hist_joint(image1, image1, 0, 255, 16);
}

TESTMAIN(test_histogram);
