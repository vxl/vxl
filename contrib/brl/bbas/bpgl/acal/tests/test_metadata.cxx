#include <iostream>
#include <testlib/testlib_test.h>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <vul/vul_file.h>
#include <vgl/vgl_vector_2d.h>
#include <bjson/bjson.h>

#include <bpgl/acal/acal_metadata.h>


static std::string test_json_string("[ {\"id\": \"image_abcd_105\", \"meta\": { \"view_angles\": [ 100, 50   ], \"acquisition_time\": [ 2015, 10, 3, 14, 4, 52 ], \"gsd\": 0.202,  \"image_type\": \"XXX\", \"sun_angles\": [ 23,  30 ], \"cloud_percentage\": 0.0 } } ]");


static void test_metadata()
{
  Json::Value root;
  Json::Reader reader;
  std::stringstream ss;
  ss << test_json_string;
  bool good = reader.parse(ss, root);
  if (!good) {
    std::string error_msg = reader.getFormattedErrorMessages();
    std::cout << "error in parsing file\n" << error_msg << std::endl;
    root = Json::nullValue;
    return;
  }
  acal_metadata meta, file_meta, geo_meta;
  meta.deserialize_image_meta(root);
  size_t n = meta.img_meta_.size();
  if (n == 0) {
    TEST("read from string", false, true);
  }
  else {
    TEST("read from string", meta.img_meta_[0].image_name_, "image_abcd_105");
  }
  std::string wd = vul_file::get_cwd();
  std::cout << "working dir " << wd << std::endl;
  std::string path = "./images.json";
  std::ofstream ostr(path);
  if(!ostr){
    std::cout << "Can't open " << path << " to write image metadata" << std::endl;
    TEST("write metadata to file",false, true);
  }
  ostr << ss.str();
  ostr.close();
  Json::Value file_root;
  std::ifstream istr(path.c_str());
  if(!istr){
    TEST("read from file", false, true);
  }
  good = reader.parse(istr, file_root);
  file_meta.deserialize_image_meta(file_root);
   good = good && file_meta.img_meta_.size() == 1;
  good = good && file_meta.img_meta_[0].image_name_ == "image_abcd_105";
  TEST("read from file", good, true);
  geo_corr_metadata gc0, gc1;
  gc0.image_name_ = "geoimage1";
  gc0.image_id_ = 0;
  gc0.translation_.set(10.0, -5.0);
  gc0.rms_proj_err_ = 0.3;
  gc1.image_name_ = "geoimage2";
  gc1.image_id_ = 1;
  gc1.translation_.set(-5.0, 15.0);
  gc1.rms_proj_err_ = 0.3;
  gc1.seed_camera_ = true;
  geo_meta.geo_corr_meta_.push_back(gc0);
  geo_meta.geo_corr_meta_.push_back(gc1);
  Json::Value geo_root;
  geo_meta.serialize_geo_corr_meta(geo_root);
  good = !geo_root.isNull();
  Json::StyledWriter writer;
  std::string geo_test = writer.write(geo_root);
  std::cout << geo_test << std::endl;
  size_t ngeo = geo_test.size();
  good = good && ngeo == 377;
  TEST("serialize geo corr", good, true);
}

TESTMAIN(test_metadata);
