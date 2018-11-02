#include <iostream>
#include <cstdlib>
#include <fstream>
#include <testlib/testlib_test.h>
#include <brad/brad_sun_dir_index.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>

#if 0 // currently unused
// illumination directions for longitude = 33.331465, latitude =44.376970 deg
// for images taken over a 7 year period at roughly 07:30Z
static std::vector<vnl_double_3> illum_dirs()
{
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
  return illumination_dirs;
}
#endif // 0

// azimuth and elevation angles taken directly from IMD files
static std::vector<vnl_double_2> illum_angles()
{
  vnl_double_2 ill_angs[]={
  vnl_double_2(62.5, 131.7),
  vnl_double_2(58.6, 136.5),
  vnl_double_2(36.9, 153.3),
  vnl_double_2(70.6, 117.5),
  vnl_double_2(68.3, 118.6),
  vnl_double_2(71.1, 123.1),
  vnl_double_2(39.9, 161.6),
  vnl_double_2(57.2, 145.2),
  vnl_double_2(56.4, 144.7),
  vnl_double_2(59.5, 127.4),
  vnl_double_2(31,   156.3),
  vnl_double_2(32.2, 152.9),
  vnl_double_2(68,   116.7),
  vnl_double_2(63.2, 136.7),
  vnl_double_2(40.6, 147.3),
  vnl_double_2(68.7, 117.9),
  vnl_double_2(67.7, 130.5),
  vnl_double_2(58.5, 146),
  vnl_double_2(68,   133.3),
  vnl_double_2(67.7, 138.3),
  vnl_double_2(67.3, 144.1),
  vnl_double_2(72.9, 123.4),
  vnl_double_2(50,   148.8),
  vnl_double_2(35.2, 169.4),
  vnl_double_2(45.1, 159),
  vnl_double_2(73.8, 131.6),
  vnl_double_2(71.9, 139.2),
  vnl_double_2(39,   170.4),
  vnl_double_2(35,   159),
  vnl_double_2(36.9, 164.2),
  vnl_double_2(74.7, 130.4),
  vnl_double_2(74,   138.8)};

  std::vector<vnl_double_2> illumination_angs(ill_angs, ill_angs+32);
  return illumination_angs;
}

static void test_sun_dir_index()
{
  START("sun direction index test");
  int oyear = 2002, ohour = 7, omin = 43, orange = 20, inter_years = 5;
  double longitude = 44.56780378, latitude = 33.34870538;
  bsta_spherical_histogram<double> h;
  brad_sun_dir_index diridx(longitude, latitude,oyear, ohour, omin,
                            orange, inter_years, 1);

  std::cout << diridx << '\n';
  double x0 = diridx.cone_axis(0)[0], y0 = diridx.cone_axis(0)[1];
  double er = std::fabs(x0-0.302867)+ std::fabs(y0 + 0.236176);
  TEST_NEAR("test constructor", er, 0.0, 0.001);
  std::vector<vnl_double_2> ill_angs = illum_angles();
  double min_angle;
  int index = diridx.index(ill_angs[2][1], ill_angs[2][0], min_angle);
  TEST("bin index", index, 2);
#if 0
  std::ofstream os("c:/images/BaghdadBoxm2/sun_index.wrl");
  diridx.print_to_vrml(os);
  os.close();

  std::cout << "Dirs from metadata\n";
  std::vector<vnl_double_2> ill_angs = illum_angles();
  for (unsigned int i=0; i<ill_angs.size(); ++i)
    std::cout << '(' << ill_angs[i][1] << ' ' << ill_angs[i][0] << ")->"
             << diridx.index(ill_angs[i][1], ill_angs[i][0])<< '\n';
#endif
}

TESTMAIN( test_sun_dir_index );
