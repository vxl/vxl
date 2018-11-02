#include <iostream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <bvxm/bvxm_illum_util.h>
#include <bsta/bsta_histogram.h>
#include <bsta/io/bsta_io_histogram.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_illum()
{
  START("bvxm illum utilities test");
#if 0
  std::vector<std::vector<vnl_matrix<float> > > nhds;
  bool good = bvxm_illum_util::load_surface_nhbds("E:/BaghdadIlum/white-side.txt",
                                                  nhds);
  // assume that each surface point has the same number of neighborhoods
  unsigned n_images = nhds[0].size();
  unsigned n_surf_pts = nhds.size();
  std::vector<std::vector<float> > mean_intensities(n_images);
  for (unsigned i = 0; i<n_images; ++i) {
    std::vector<float> ints(n_surf_pts);
    for (unsigned s = 0; s<n_surf_pts; ++s) {
      vnl_matrix<float> mat = nhds[s][i];
      float mean = mat.mean();
      ints[s]=mean;
    }
    mean_intensities[i]=ints;
  }
  for (unsigned i = 0; i<n_images; ++i) {
    for (unsigned s = 0; s<n_surf_pts; ++s)
      std::cout << mean_intensities[i][s] << ' ';
    std::cout << '\n';
  }
#endif
#if 0
  vsl_b_ifstream is("E:/BaghdadIlum/bag_hists.vsl");
  if (!is)
    return;
  unsigned n = 0;
  vsl_b_read(is, n);
  if (!n)
    return;
  std::vector<bsta_histogram<float> > hists;
  for (unsigned i = 0; i<n; ++i) {
    bsta_histogram<float> hist;
    vsl_b_read(is, hist);
    hists.push_back(hist);
  }
  float frac = 0.005f;
  for (unsigned i = 0; i<n; ++i)
  {
    float vmin = hists[i].avg_bin_value(hists[i].low_bin());
    float vlow = hists[i].value_with_area_below(frac);
    float vhigh = hists[i].value_with_area_above(3*frac);
    std::cout << vmin << ' ' << vlow << ' ' << vhigh << ' ' << '\n';
  }
#endif
  // test model fitting
  double inten[] ={0.596473568,0.632584074,0.583595386,0.923219624,
                   0.507035684,0.288599204,0.957025019,0.651967156,
                   0.850692165,0.668889453,0.630201033,0.510793676,
                   0.72214914,0.796109,0.754603,0.858133993};
  std::vector<double> intensities(inten, inten+16);

  vnl_double_3 ill_dirs[]={
    vnl_double_3(0.34476,-0.30717,0.8870),
    vnl_double_3(0.358640131,-0.377927323,0.853550347),
    vnl_double_3(0.359314323,-0.71441535,0.60041979),
    vnl_double_3(0.403196666,-0.308266776,0.861628715),
    vnl_double_3(0.344537352,-0.784875912,0.515037683),
    vnl_double_3(0.385479801,-0.753291859,0.532875874),
    vnl_double_3(0.334663709,-0.168318145,0.927183479),
    vnl_double_3(0.410189725,-0.638934973,0.650773763),
    vnl_double_3(0.272629314,-0.25691254,0.927183479),
    vnl_double_3(0.332982112,-0.549817743,0.766043969),
    vnl_double_3(0.203003209,-0.235181137,0.950515402),
    vnl_double_3(0.293558722,-0.764744293,0.573576014),
    vnl_double_3(0.200950347,-0.171021944,0.964557128),
    vnl_double_3(0.242156,-0.72794,0.641449),
    vnl_double_3(0.271353,-0.17689,0.946085),
    vnl_double_3(0.150315903,-0.803200724,0.576431892)
  };
  std::vector<vnl_double_3> illumination_dirs(ill_dirs, ill_dirs+16);
#if 0
  bvxm_illum_util::load_illumination_dirs("illum_dirs.txt", illumination_dirs);
#endif
  vnl_double_4 model_params;
  double fitting_error;
  bvxm_illum_util::solve_lambertian_model(illumination_dirs,
                                          intensities,
                                          model_params,
                                          fitting_error);
  std::cout << "Model Params: " << model_params << '\n'
           << "Fitting Error: " << fitting_error << '\n';
  TEST_NEAR("test lambertian model", fitting_error , 0.137807, 1.0e-6);
}

TESTMAIN( test_illum );
