#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_flip.h>
#include <brip/brip_histogram.h>
#include <brip/brip_mutual_info.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/bsta_histogram.h>

static void data(std::string const& path, std::vector<float>& x,
                 std::vector<float>& I)
{
  std::ifstream is(path.c_str());
  unsigned npts;
  is >> npts;
  float tx, tI;
  for (unsigned i = 0; i<npts; ++i) {
    is >> tx >> tI;
    x.push_back(tx);
    I.push_back(tI/324.0f);
  }
}

static bsta_histogram<float> compute_hist(std::vector<float> const& I)
{
  //compute mean intensity and direction
  unsigned npts = I.size();
  float Im=0.0f;
  for (unsigned i = 0; i<npts; ++i)
    Im += I[i];
  bsta_histogram<float> h(1.0f, 25);
  for (unsigned i = 0; i<npts; ++i)
    //    h.upcount(std::fabs(I[i]-Im), 1.0f);
    h.upcount(I[i], 1.0f);
  return h;
}

static bsta_joint_histogram<float> compute_jhist(std::vector<float> const& x,
                                                 std::vector<float> const& I)
{
  //compute mean intensity and direction
  unsigned npts = x.size();
  float Im=0.0f;
  for (unsigned i = 0; i<npts; ++i)
    Im += I[i];
  Im/= npts;

  bsta_joint_histogram<float> h(420.0f, 20, 1.0f, 20);
  bsta_histogram<float> h1(420.0f, 20);
  for (unsigned k = 0; k<npts; ++k) {
    float xk = x[k], Ik = I[k];
    float dI = std::fabs(Ik-Im);
    h.upcount(xk, 1.0f, dI, 1.0);
    //    h1.upcount(dx, 1.0f);
  }
#if 0
  for (unsigned r = 0; r<20; r++)
  for (unsigned c = 0; c<20; c++) {
    float p = h.get_count(r, c);
    float p1 = h1.counts(r);
    if (p1) {
      p /= p1;
      h.set_count(r,c,p);
    }
    else h.set_count(r,c,0.0f);
  }
#endif
  return h;
}

static void test_mutual_info()
{

  //generate some images
  //--------------------------------------------
  unsigned ni=256;
  unsigned nj=256;
  vil_image_view<vxl_byte> image1(ni,nj), image2(ni,nj);
  for (unsigned j=0;j<nj;++j) {
    for (unsigned i=0;i<ni;++i) {
      image1(i,j) = vxl_byte((i+j)/2);
      image2(i,j) = vxl_byte(i);
    }
  }


  // Test Entropy functions
  //----------------------------------------------------
  std::vector<double> hist1, hist2;
  std::vector<std::vector<double> > hist3;

  double sum1 = brip_histogram(image1, hist1, 0, 255, 16);
  double entropy1 = brip_hist_entropy(hist1, sum1);

  double sum2 = brip_histogram(vil_flip_lr(image1), hist2, 0, 255, 16);
  double entropy2 = brip_hist_entropy(hist2, sum2);

  double sum3 = brip_joint_histogram(image1, image1, hist3, 0, 255, 16);
  double entropy3 = brip_hist_entropy(hist3, sum3);

  const double tol = 1e-12; // tolerance

  TEST("Mirror Entropy", entropy1, entropy2);
  TEST_NEAR("Self Joint Entropy", entropy1, entropy3, tol);

  // Test Mutual Information
  //---------------------------------------------------------

  double mi1 = brip_mutual_info(image1, image1, 0, 255, 16);
  double mi2 = brip_mutual_info(image1, image2, 0, 255, 16);
  double mi3 = brip_mutual_info(image2, image1, 0, 255, 16);

  //std::cout << "MI1: " << mi1 <<  " MI2: " << mi2 << " MI3: " << mi3 << std::endl;

  TEST_NEAR("Mutual Information Commutative", mi2, mi3, 1e-9);
  TEST("Large Self Mutual Info", mi2 < mi1, true);
}

TESTMAIN(test_mutual_info);
