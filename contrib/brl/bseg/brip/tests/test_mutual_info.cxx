#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_flip.h>
#include <brip/brip_histogram.h>
#include <brip/brip_mutual_info.h>


static void test_mutual_info()
{
  START ("mutual info");

  // Generate some images
  //--------------------------------------------
  unsigned ni=256;
  unsigned nj=256;
  vil_image_view<vxl_byte> image1(ni,nj), image2(ni,nj);
  for (unsigned j=0;j<nj;++j){
    for (unsigned i=0;i<ni;++i){
      image1(i,j) = vxl_byte((i+j)/2);
      image2(i,j) = vxl_byte(i);
    }
  }


  // Test Entropy functions
  //----------------------------------------------------
  vcl_vector<double> hist1, hist2;
  vcl_vector<vcl_vector<double> > hist3;

  double sum1 = brip_histogram(image1, hist1, 0, 255, 16);
  double entropy1 = brip_hist_entropy(hist1, sum1);

  double sum2 = brip_histogram(vil_flip_lr(image1), hist2, 0, 255, 16);
  double entropy2 = brip_hist_entropy(hist2, sum2);

  double sum3 = brip_joint_histogram(image1, image1, hist3, 0, 255, 16);
  double entropy3 = brip_hist_entropy(hist3, sum3);

  const double tol = 1e-12; // tolerance

  TEST("Mirror Entropy", entropy1, entropy2);

  testlib_test_begin("Self Joint Entropy");
  testlib_test_perform(vcl_abs(entropy1 - entropy3) <= tol);

  // Test Mutual Information
  //---------------------------------------------------------

  double mi1 = brip_mutual_info(image1, image1, 0, 255, 16);
  double mi2 = brip_mutual_info(image1, image2, 0, 255, 16);
  double mi3 = brip_mutual_info(image2, image1, 0, 255, 16);

  //vcl_cout << "MI1: " << mi1 <<  " MI2: " << mi2 << " MI3: " << mi3 << vcl_endl;

  TEST_NEAR("Mutual Information Commutative", mi2, mi3, 1e-9);
  testlib_test_begin("Large Self Mutual Info");
  testlib_test_perform(mi2 < mi1);
}

TESTMAIN(test_mutual_info);
