#include <testlib/testlib_test.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_utils.h>
#include <vsph/vsph_grid_index_2d.h>


static void test_grid_index()
{
  // test box spanning 360 degrees in azimuth
  vsph_sph_point_2d p9(0.0, 80.0, false);
  vsph_sph_point_2d p10(80.0, 0.0, false);
  vsph_sph_point_2d p11(110.0,10.0, false);
  vsph_sph_point_2d p12(80.0, 60.0, false);
  vsph_sph_point_2d p13(110.0, 110.0, false);
  vsph_sph_point_2d p14(80.0, 165.0, false);
  vsph_sph_point_2d p14a(80.0, 166.0, false);
  vsph_sph_point_2d p15(110.0, -175.0, false);
  vsph_sph_point_2d p15a(110.0, -180.0, false);
  vsph_sph_point_2d p16(80.0, -130.0, false);
  vsph_sph_point_2d p17(110.0, -100.0, false);
  vsph_sph_point_2d p18(80.0,  45.0, false);
  vsph_sph_point_2d p19(80.0, -10.0, false);
  vsph_sph_point_2d p20(180.0, -180.0, false);
  vsph_sph_point_2d p21(109.9999, -179.9999, false);
  vsph_sph_point_2d p22(0.0001, 79.9999, false);
  vsph_sph_point_2d p23(179.9999, -179.9999, false);
  vsph_sph_point_2d p24(109.9999, -100.0001, false);

  // check th_min, ph_min bound neighbors
  // to insert
  vsph_sph_point_2d p25(49.9999, 41.0, false);
  vsph_sph_point_2d p26(51.0, 39.9999, false);
  vsph_sph_point_2d p27(49.9999, 39.9999, false);
  // to test
  vsph_sph_point_2d p28(50.0, 40.0, false);
  vsph_sph_point_2d p29(50.0, 41.0, false);
  vsph_sph_point_2d p30(51.0, 40.0, false);

  // check th_min, ph_max bound neighbors
  // to insert
  vsph_sph_point_2d p31(49.9999, 41.0, false);
  vsph_sph_point_2d p32(51.0, 50.0, false);
  vsph_sph_point_2d p33(49.9999, 50.0, false);
  // to test
  vsph_sph_point_2d p34(50.0, 41.0, false);
  vsph_sph_point_2d p35(51.0, 49.9999, false);
  vsph_sph_point_2d p36(50.0, 49.9999, false);

  // check th_max, ph_min bound neighbors
  // to insert
  vsph_sph_point_2d p37(60.0, 41.0, false);
  vsph_sph_point_2d p38(51.0, 39.9999, false);
  vsph_sph_point_2d p39(60.0, 39.9999, false);
  // to test
  vsph_sph_point_2d p40(59.9999, 41.0, false);
  vsph_sph_point_2d p41(51.0, 40.0, false);
  vsph_sph_point_2d p42(60.0, 40.0, false);

  // check th_max, ph_max bound neighbors
  // to insert
  vsph_sph_point_2d p43(60.0, 41.0, false);
  vsph_sph_point_2d p44(51.0, 50.0, false);
  vsph_sph_point_2d p45(60.0, 50.0, false);
  // to test
  vsph_sph_point_2d p46(59.9999, 41.0, false);
  vsph_sph_point_2d p47(51.0, 49.9999, false);
  vsph_sph_point_2d p48(60.0, 49.9999, false);

  unsigned n_bins_theta = 18, n_bins_phi = 36;
  vsph_grid_index_2d gidx(n_bins_theta, n_bins_phi, false);
  bool good = gidx.insert(p9, 9);
  good = good && gidx.insert(p10, 10);
  good = good && gidx.insert(p15a, 151);
  good = good && !gidx.insert(p10, 10);
  good = good && gidx.insert(p20, 20);
  good = good && gidx.insert(p17, 17);
  TEST("grid insert", good, true);
  unsigned th_idx = 0, ph_idx = 0;
  int id = 0;
  bool f16 = gidx.find(p16, th_idx, ph_idx, id);//false
  bool f20 = gidx.find(p15a, th_idx, ph_idx, id);//true
  bool f21 = gidx.find(p21, th_idx, ph_idx, id);//true
  bool f22 = gidx.find(p22, th_idx, ph_idx, id);//true
  bool f23 = gidx.find(p23, th_idx, ph_idx, id);//true
  bool f24 = gidx.find(p24, th_idx, ph_idx, id);//true
  bool find1 = !f16&&f20&&f21&&f22&f23&&f24;
  TEST("find with poles and cut", find1, true);
  gidx.clear();
  gidx.insert(p25, 25);
  gidx.insert(p26, 26);
  gidx.insert(p27, 27);
  int id28, id29, id30;
  bool f28 = gidx.find(p28, th_idx, ph_idx, id28);//true id == 27
  bool f29 = gidx.find(p29, th_idx, ph_idx, id29);//true id == 25
  bool f30 = gidx.find(p30, th_idx, ph_idx, id30);//true id == 26
  bool find2 = f28&&f29&&f30&&id28==27&&id29==25&&id30==26;
  gidx.clear();
  gidx.insert(p31, 31);
  gidx.insert(p32, 32);
  gidx.insert(p33, 33);
  int id34, id35, id36;
  bool f34 = gidx.find(p34, th_idx, ph_idx, id34);//true id == 31
  bool f35 = gidx.find(p35, th_idx, ph_idx, id35);//true id == 32
  bool f36 = gidx.find(p36, th_idx, ph_idx, id36);//true id == 33
  bool find3 = f34&&f35&&f36&&id34==31&&id35==32&&id36==33;
  gidx.clear();
  gidx.insert(p37, 37);
  gidx.insert(p38, 38);
  gidx.insert(p39, 39);
  int id40, id41, id42;
  bool f40 = gidx.find(p40, th_idx, ph_idx, id40);//true id == 37
  bool f41 = gidx.find(p41, th_idx, ph_idx, id41);//true id == 38
  bool f42 = gidx.find(p42, th_idx, ph_idx, id42);//true id == 39
  bool find4 = f40&&f41&&f42&&id40==37&&id41==38&&id42==39;
  gidx.clear();
  gidx.insert(p43, 43);
  gidx.insert(p44, 44);
  gidx.insert(p45, 45);
  int id46, id47, id48;
  bool f46 = gidx.find(p46, th_idx, ph_idx, id46);//true id == 43
  bool f47 = gidx.find(p47, th_idx, ph_idx, id47);//true id == 44
  bool f48 = gidx.find(p48, th_idx, ph_idx, id48);//true id == 45
  bool find5 = f46&&f47&&f48&&id46==43&&id47==44&&id48==45;
  TEST("bin bounds", find2&&find3&&find4&&find5, true);
 }

TESTMAIN(test_grid_index);
