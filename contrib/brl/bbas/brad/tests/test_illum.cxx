#include <iostream>
#include <cstdlib>
#include <fstream>
#include <testlib/testlib_test.h>
#include <brad/brad_illum_util.h>
#include <bsta/bsta_histogram.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_illum()
{
  START("brad illum utilities test");
  vnl_double_3 ill_dirs[]={
    vnl_double_3(0.344759944,-0.307169525,0.887010408),
    vnl_double_3(0.358640131,-0.377927323,0.853550347),
    vnl_double_3(0.359314323,-0.71441535,0.60041979),
    vnl_double_3(0.294631455,-0.153375276,0.943222312),
    vnl_double_3(0.324632254,-0.176995073,0.929132199),
    vnl_double_3(0.271352601,-0.176892346,0.946085019),
    vnl_double_3(0.242155829,-0.727944712,0.64144918),
    vnl_double_3(0.309160992,-0.444823599,0.840566147),
    vnl_double_3(0.31978229,-0.451643953,0.832920781),
    vnl_double_3(0.403196666,-0.308266776,0.861628715),
    vnl_double_3(0.344537352,-0.784875912,0.515037683),
    vnl_double_3(0.334663709,-0.168318145,0.927183479),
    vnl_double_3(0.309220889,-0.328136784,0.892585398),
    vnl_double_3(0.410189725,-0.638934973,0.650773763),
    vnl_double_3(0.321029856,-0.16997639,0.93169086),
    vnl_double_3(0.288541577,-0.24643749,0.92520934),
    vnl_double_3(0.292178258,-0.43317131,0.852639714),
    vnl_double_3(0.272629314,-0.25691254,0.927183479),
    vnl_double_3(0.252426572,-0.283316967,0.92520934),
    vnl_double_3(0.226285422,-0.312600525,0.922537707),
    vnl_double_3(0.245479874,-0.16186398,0.955792699),
    vnl_double_3(0.332982112,-0.549817743,0.766043969),
    vnl_double_3(0.150315903,-0.803200724,0.576431892),
    vnl_double_3(0.252962587,-0.658988064,0.708339368),
    vnl_double_3(0.208629911,-0.185230074,0.960293382),
    vnl_double_3(0.203003209,-0.235181137,0.950515402),
    vnl_double_3(0.129604627,-0.766263041,0.629319944),
    vnl_double_3(0.293558722,-0.764744293,0.573576014),
    vnl_double_3(0.217739267,-0.769471044,0.60041979),
    vnl_double_3(0.200950347,-0.171021944,0.964557128),
    vnl_double_3(0.181560261,-0.207394314,0.961261395)
  };
  std::vector<vnl_double_3> illumination_dirs(ill_dirs, ill_dirs+31);
  double intens [] ={
    0.681509147,
    0.729145981,
    0.723445466,
    0.769725486,
    0.762458528,
    0.754602953,
    0.796108572,
    0.800420385,
    0.869547508,
    0.885219607,
    0.671781687,
    0.944756979,
    0.843227158,
    0.755964269,
    0.853543398,
    0.823241748,
    0.831943459,
    0.906163005,
    0.881455635,
    0.973166642,
    0.942790689,
    0.754428148,
    0.858133993,
    0.778665719,
    0.829114052,
    0.776806151,
    0.710474508,
    0.578924588,
    0.874122302,
    0.802154069,
    0.826317575};
  std::vector<double> intensities(intens, intens+31);
  std::vector<double> fitting_error;
  vnl_double_4 model_params;
  brad_solve_lambertian_model(illumination_dirs,
                              intensities,
                              model_params,
                              fitting_error);

  double er = 0.0;
  unsigned m = fitting_error.size();
  for (unsigned i = 0; i<m; ++i)
    er += fitting_error[i];
  er /= m;
  er = std::sqrt(er);
  std::cout << "Model Params: " << model_params << '\n'
           << "Fitting Error: " << er << std::endl;
  TEST_NEAR("test lambertian model", er , 0.072795, 1.0e-6);
#if 0
  for (unsigned i = 0; i<m ; ++i)
    std::cout << std::sqrt(fitting_error[i]) << std::endl;

  std::cout << "Predicted Intensity\n";
  for (unsigned i = 0; i<m; ++i)
    std::cout << brad_expected_intensity(illumination_dirs[i], model_params) << std::endl;
  std::vector<double> fitting_errors;
  brad_solution_error(illumination_dirs, intensities, fitting_errors);
  std::cout << "Interpolated intensity\n";
  for (unsigned i = 0; i<m; ++i)
    std::cout << fitting_errors[i] << std::endl;
#endif
}

TESTMAIN( test_illum );
