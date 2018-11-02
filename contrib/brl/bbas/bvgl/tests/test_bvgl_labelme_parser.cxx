//This is brl/bbas/bvgl/tests/test_bvgl_labelme_parser.cxx
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
//:
// \file

#include <bvgl/bvgl_labelme_parser.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

static std::string test_xml_file()
{
  return  "\
<annotation>\
<filename>andy_eze.jpg</filename>\
<folder>test</folder>\
<source>\
<sourceImage>The MIT-CSAIL database of objects and scenes</sourceImage>\
<sourceAnnotation>LabelMe Webtool</sourceAnnotation>\
</source>\
<object>\
<name>mouth</name>\
<deleted>0</deleted>\
<verified>0</verified>\
<date>05-Mar-2012 22:14:49</date>\
<id>0</id>\
<polygon>\
<username>Andy</username>\
<pt>\
<x>335</x>\
<y>183</y>\
</pt>\
<pt>\
<x>333</x>\
<y>220</y>\
</pt>\
<pt>\
<x>332</x>\
<y>234</y>\
</pt>\
</polygon>\
</object>\
<object>\
<name>Sunglasses</name>\
<deleted>0</deleted>\
<verified>0</verified>\
<date>06-Mar-2012 02:48:58</date>\
<id>1</id>\
<polygon>\
<username>anonymous</username>\
<pt>\
<x>544</x>\
<y>200</y>\
</pt>\
<pt>\
<x>501</x>\
<y>211</y>\
</pt>\
<pt>\
<x>497</x>\
<y>224</y>\
</pt>\
</polygon>\
</object>\
</annotation>";
}


//: Test changes
static void test_bvgl_labelme_parser()
{
  std::string filename = "labelme_test.xml";
  std::cout<<"Testing label me parser on synthetic data"<<std::endl;
  std::ofstream file;
  file.open (filename.c_str());
  file << test_xml_file() << std::endl;
  file.close();

  //Use parser
  bvgl_labelme_parser parser(filename);
  std::vector<vgl_polygon<double> > polys = parser.polygons();
  TEST("Number of polygons returned", 2, polys.size());

  //ground truth
  std::vector<vgl_point_2d<double> > poly0;
  poly0.emplace_back(335, 183);
  poly0.emplace_back(333, 220);
  poly0.emplace_back(332, 234);

  std::vector<vgl_point_2d<double> > poly1;
  poly1.emplace_back(544, 200);
  poly1.emplace_back(501, 211);
  poly1.emplace_back(497, 224);

  for (unsigned int i=0; i<poly0.size(); ++i) {
    double tx = poly0[i].x();
    double ty = poly0[i].y();
    TEST_NEAR("Polygon point equal", tx, polys[0][0][i].x(), 1e-5);
    TEST_NEAR("Polygon point equal", ty, polys[0][0][i].y(), 1e-5);
  }
  for (unsigned int i=0; i<poly1.size(); ++i) {
    double tx = poly1[i].x();
    double ty = poly1[i].y();
    TEST_NEAR("Polygon point equal", tx, polys[1][0][i].x(), 1e-5);
    TEST_NEAR("Polygon point equal", ty, polys[1][0][i].y(), 1e-5);
  }

  //test filename
  TEST("Testing filename tag", "andy_eze.jpg", parser.image_name());

  //test each object name
  std::vector<std::string> names = parser.obj_names();
  TEST("Testing object name", names[0], "mouth");
  TEST("Testing object name", names[1], "Sunglasses");

  //Remove file from directory
  vul_file::delete_file_glob("labelme_test.xml");
}

TESTMAIN( test_bvgl_labelme_parser );
