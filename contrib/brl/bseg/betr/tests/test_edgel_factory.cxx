// This is brl/bseg/betr/tests/test_edgel_factory.cxx
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <betr/betr_edgel_factory.h>
#include <vil/vil_load.h>
#include <bsta/bsta_histogram.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
void test_edgel_factory()
{
  vsol_point_2d_sptr p0 = new vsol_point_2d(94.9259, 347.247);
  vsol_point_2d_sptr p1 = new vsol_point_2d(134.333, 354.062);
  vsol_point_2d_sptr p2 = new vsol_point_2d(110.037, 379.247);
  vsol_point_2d_sptr p3 = new vsol_point_2d(89, 376.284);
  std::vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);  verts.push_back(p1);  verts.push_back(p2);   verts.push_back(p2);
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(verts);
  // D:/tests/chiletest/crop_dir/09DEC06145803-P1BS-052869858050_01_P002_bin_2.tif
  std::string img_dir = "D:/tests/chiletest/crop_dir/";
  std::string image_name = "09DEC06145803-P1BS-052869858050_01_P002_bin_2";
  std::string img_path = img_dir + image_name + ".tif";
  betr_edgel_factory efact;
  vil_image_resource_sptr imgr = vil_load_image_resource(img_path.c_str());
  bool good = efact.add_image(image_name, imgr);
  unsigned i0 = 290, j0 = 330;
  unsigned ni = 80, nj = 80;
  std::string region_name = "test_region";
  //  good = good && efact.add_region_from_origin_and_size(image_name, region_name,ni0, j0, ni, nj);
  good = good && efact.add_region(image_name, region_name, poly);
  good = good && efact.process(image_name, region_name);
  const bsta_histogram<double>& h = efact.hist(image_name, region_name);
  std::cout << "edgel strength\n";
  h.print();
  std::cout << '\n';
}

TESTMAIN(test_edgel_factory);
