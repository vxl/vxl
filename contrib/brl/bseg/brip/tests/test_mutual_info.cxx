#include <testlib/testlib_test.h>
#include <vcl_cstdlib.h> // for rand()
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_flip.h>
#include <brip/brip_histogram.h>
#include <brip/brip_mutual_info.h>
#include <bsta/bsta_joint_histogram.h>
#include <bsta/bsta_histogram.h>

static void data(vcl_string const& path, vcl_vector<float>& x,
                 vcl_vector<float>& I)
{
  vcl_ifstream is(path.c_str());
  unsigned npts;
  is >> npts;
  float tx, tI;
  for (unsigned i = 0; i<npts; ++i) {
    is >> tx >> tI;
    x.push_back(tx);
    I.push_back(tI/324.0f);
  }
}

static bsta_histogram<float> compute_hist(vcl_vector<float> const& I)
{
  //compute mean intensity and direction
  unsigned npts = I.size();
  float Im=0.0f;
  for (unsigned i = 0; i<npts; ++i)
    Im += I[i];
  Im/= npts;
  bsta_histogram<float> h(1.0f, 25);
  for (unsigned i = 0; i<npts; ++i)
    //    h.upcount(vcl_fabs(I[i]-Im), 1.0f);
    h.upcount(I[i], 1.0f);
  return h;
}

static bsta_joint_histogram<float> compute_jhist(vcl_vector<float> const& x,
                                                 vcl_vector<float> const& I)
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
    float dI = vcl_fabs(Ik-Im);
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
#if 0
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
  TEST_NEAR("Self Joint Entropy", entropy1, entropy3, tol);

  // Test Mutual Information
  //---------------------------------------------------------

  double mi1 = brip_mutual_info(image1, image1, 0, 255, 16);
  double mi2 = brip_mutual_info(image1, image2, 0, 255, 16);
  double mi3 = brip_mutual_info(image2, image1, 0, 255, 16);

  //vcl_cout << "MI1: " << mi1 <<  " MI2: " << mi2 << " MI3: " << mi3 << vcl_endl;

  TEST_NEAR("Mutual Information Commutative", mi2, mi3, 1e-9);
  TEST("Large Self Mutual Info", mi2 < mi1, true);
#endif
  /// temporary hack ///
#if 0
  vil_image_view<vxl_byte> frame = vil_load("C:/images/Calibration/frame_146.png");
  unsigned ni = frame.ni(), nj = frame.nj();
  bsta_joint_histogram<float> h(1000.0f, 20, 1.0f, 20);
  bsta_histogram<float> h1(1000.0f, 20);
  bsta_histogram<float> hI(1.0f, 25);
  float mu = 0.0f;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      mu += frame(i,j)/255.0f;
  mu/=(ni*nj);
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
      hI.upcount(vcl_fabs(frame(i,j)/255.0f-mu), 1.0f);
  hI.print();
  unsigned nsamp = 10000;
  for (unsigned k = 0; k<nsamp; k++) {
    unsigned ik = static_cast<unsigned>((vcl_rand()/(RAND_MAX+1.0))*(ni-1));
    unsigned jk = static_cast<unsigned>((vcl_rand()/(RAND_MAX+1.0))*(nj-1));
    if (ik>=0&&ik<ni&&jk>=0&&jk<nj) {
      float Ik = frame(ik, jk)/(255.0f);
      for (unsigned kk = 0; kk<nsamp; kk++) {
        unsigned ikk=static_cast<unsigned>((vcl_rand()/(RAND_MAX+1.0))*(ni-1));
        unsigned jkk=static_cast<unsigned>((vcl_rand()/(RAND_MAX+1.0))*(nj-1));
        if (ikk>=0&&ikk<ni&&jkk>=0&&jkk<nj) {
          float Ikk = frame(ikk, jkk)/(255.0f);
          float fik = static_cast<float>(ik), fjk = static_cast<float>(jk);
          float fikk = static_cast<float>(ikk), fjkk = static_cast<float>(jkk);
          float d = vcl_sqrt((fik-fikk)*(fik-fikk)+(fjk-fjkk)*(fjk-fjkk));
          float dI = vcl_fabs(Ik-Ikk);
          h.upcount(d, 1.0f, dI, 1.0);
          h1.upcount(d, 1.0f);
        }
      }
    }
  }
#endif

  vcl_vector<float> I_empty, x_empty;
  data("C:/images/Calibration/empty-pointset.txt", x_empty, I_empty);
  bsta_joint_histogram<float> h_empty =
    compute_jhist(x_empty, I_empty);
  bsta_histogram<float> h1_empty = compute_hist(I_empty);
  vcl_cout << "empty hist\n";
  h1_empty.print();
  vcl_vector<float> I_occ, x_occ;
  data("C:/images/Calibration/occ-points.txt", x_occ, I_occ);
  bsta_joint_histogram<float> h_occ = compute_jhist(x_occ, I_occ);
  bsta_histogram<float> h1_occ = compute_hist(I_occ);
  vcl_cout << "occ hist\n";
  h1_occ.print();
#if 0
  vcl_ofstream os("C:/images/Calibration/hist_empty.wrl");
  if (os.is_open()) {
    h_empty.print_to_vrml(os);
    os.close();
  }
  vcl_ofstream oso("C:/images/Calibration/hist_occ.wrl");
  if (oso.is_open()) {
    h_occ.print_to_vrml(oso);
    oso.close();
  }
  vcl_cout << "Empty case\n";
  float p_empty = 0.5f, p_occ = 0.5f;
  float Im_empty = 0.0;
  unsigned n_empty = x_empty.size();
  unsigned n_items_empty = 1;
  for (unsigned ie = 1; ie<n_empty; ++ie) {
    float xe = x_empty[ie], Ie = I_empty[ie];
    float dIe = vcl_fabs(Ie-Im_empty);
    float pe = h1_empty.p(dIe);
    float po = h1_occ.p(dIe);
    if (pe==0.0f) pe = 0.001f;
    if (po==0.0f) po = 0.001f;
    p_empty = p_empty*(pe/(p_empty*pe + (1-p_empty)*po));
    vcl_cout << p_empty << '\n';
    Im_empty = (Im_empty*n_items_empty + Ie)/(n_items_empty+1);
    n_items_empty++;
  }

  vcl_cout << "Occupied case\n";
  p_empty = 0.5f; p_occ = 0.5f;
  float Im_occ = I_occ[0];
  unsigned n_occ = x_occ.size();
  unsigned n_items_occ = 1;
  for (unsigned io= 0; io<n_occ; ++io) {
    float xo = x_occ[io], Io = I_occ[io];
    float dIo = vcl_fabs(Io-Im_occ);
    float pe = h1_empty.p(dIo);
    float po = h1_occ.p(dIo);
    if (pe==0.0f) pe = 0.001f;
    if (po==0.0f) po = 0.001f;
    p_occ = (p_occ*po)/((1.0f-p_occ)*p_empty*pe + p_occ*po);
    vcl_cout << p_occ << '\n';
    Im_occ = (Im_occ*n_items_occ + Io)/(n_items_occ+1);
    n_items_occ++;
  }
#endif
}

TESTMAIN(test_mutual_info);
