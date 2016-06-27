// This is brl/bseg/betr/tests/test_geo_box.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#include <betr/betr_geo_box_3d.h>

void test_geo_box()
{
  betr_geo_box_3d geo_box, geo_boxg;
  //universal date line test (Rabi Island)
  double lat0 = -16.490904, lon0 = 179.996115, elv0 = 99.0;
  double lat1 = -16.486132, lon1 = -179.960005,elv1 = 200.0;
  double lat2 = -16.521430, lon2 = -179.974572, elv2 = 614.0;
  geo_box.add(lon0, lat0, elv0);
  geo_box.add(lon1, lat1, elv1);
  geo_box.add(lon2, lat2, elv2);

  double tlat =-16.502931 , tlon =-179.974746 ,  telv = 266.0;
  bool in = geo_box.contains(tlon, tlat, telv);
  double tolat = -16.523679, tolon = -179.945039, toelv = 249.0;
  bool out = !geo_box.contains(tolon, tolat, toelv);
  //Greenwich England test
  double  latg0 = 51.484443, long0 = -0.002913, elvg0 = 20.0;
  double  latg1 = 51.485020, long1 =  0.014093, elvg1 = 28.0;
  double  latg2 = 51.476992, long2 =  0.009077, elvg2 = 135.0;
  geo_boxg.add(long0, latg0, elvg0);
  geo_boxg.add(long1, latg1, elvg1);
  geo_boxg.add(long2, latg2, elvg2);

  double tlatg =  51.482146, tlong =  0.007451,  telvg = 92.0;
  bool ing = geo_boxg.contains(tlong, tlatg, telvg);

  double tolatg = 51.489497, tolong = 0.001554, toelvg = 11.0;
  bool outg = !geo_boxg.contains(tolatg, tolong, toelvg);
  TEST("geobox add and contains", in&&out&&ing&&outg, true);
}

TESTMAIN(test_geo_box);
