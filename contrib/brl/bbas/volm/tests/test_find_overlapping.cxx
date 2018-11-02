#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>

#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>
#include <volm/volm_geo_index2.h>

#include <bkml/bkml_write.h>
#include <vul/vul_file.h>
#include <vcl_where_root_dir.h>

static volm_satellite_resources_sptr create_test_sat_res()
{
  vgl_box_2d<double> bbox(35.0, 37.0, 32.0, 33.0);
  volm_satellite_resources_sptr res_sptr = new volm_satellite_resources(bbox, 0.25, false);
  volm_satellite_resource res;
  std::string name1 = "/path_to_img1/img_1.tif";
  res.full_path_ = name1;
  res.name_ = vul_file::strip_directory(name1);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(36.10513, 32.13166, 0.0);   res.meta_->upper_right_.set(36.40004, 32.51381, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  std::vector<vgl_point_2d<double> > footprint_corners;
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name2 = "/path_to_img2/img_2.tif";
  res.full_path_ = name2;
  res.name_ = vul_file::strip_directory(name2);
  res.name_ = vul_file::strip_extension(res.name_);  res.meta_->satellite_name_ = "GeoEye-1";
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(36.03285, 32.14575, 0.0);   res.meta_->upper_right_.set(36.46879, 32.49186, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name3 = "/path_to_img3/img_3.tif";
  res.full_path_ = name3;
  res.name_ = vul_file::strip_directory(name3);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(36.08056, 32.14817, 0.0);   res.meta_->upper_right_.set(36.43067, 32.61172, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name4 = "/path_to_img4/img_4.tif";
  res.full_path_ = name4;
  res.name_ = vul_file::strip_directory(name4);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(36.10970, 32.05984, 0.0);   res.meta_->upper_right_.set(36.39290, 32.50900, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name5 = "/path_to_img5/img_5.tif";
  res.full_path_ = name5;
  res.name_ = vul_file::strip_directory(name5);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(35.98952, 32.19027, 0.0);   res.meta_->upper_right_.set(36.54515, 32.53180, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name6 = "/path_to_img6/img_6.tif";
  res.full_path_ = name6;
  res.name_ = vul_file::strip_directory(name6);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(35.94401, 32.16550, 0.0);   res.meta_->upper_right_.set(36.56926, 32.46338, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name7 = "/path_to_img7/img_7.tif";
  res.full_path_ = name7;
  res.name_ = vul_file::strip_directory(name7);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(36.09320, 32.12822, 0.0);   res.meta_->upper_right_.set(36.39953, 32.58570, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name8 = "/path_to_img8/img_8.tif";
  res.full_path_ = name8;
  res.name_ = vul_file::strip_directory(name8);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(35.98967, 32.14767, 0.0);   res.meta_->upper_right_.set(36.53007, 32.51360, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name9 = "/path_to_img9/img_9.tif";
  res.full_path_ = name9;
  res.name_ = vul_file::strip_directory(name9);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(36.03028, 32.15126, 0.0);   res.meta_->upper_right_.set(36.48835, 32.52314, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  std::string name10 = "/path_to_img10/img_10.tif";
  res.full_path_ = name10;
  res.name_ = vul_file::strip_directory(name10);
  res.name_ = vul_file::strip_extension(res.name_);
  res.meta_ = new brad_image_metadata();
  res.meta_->lower_left_.set(36.00632, 32.13226, 0.0);   res.meta_->upper_right_.set(36.51066, 32.52135, 0.0);
  res.meta_->gsd_ = 1.0f;  res.meta_->band_ = "PAN";  res.meta_->satellite_name_ = "GeoEye-1";
  footprint_corners.clear();
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->lower_left_.y());
  footprint_corners.emplace_back(res.meta_->upper_right_.x(), res.meta_->upper_right_.y());
  footprint_corners.emplace_back(res.meta_->lower_left_.x(),  res.meta_->upper_right_.y());
  res.meta_->footprint_ = vgl_polygon<double>(footprint_corners);
  res_sptr->resources_.push_back(res);

  // add resources into tree
  for (unsigned i = 0; i < res_sptr->resources_.size(); i++) {
    std::vector<volm_geo_index2_node_sptr> leaves;
    vgl_box_2d<double> satellite_footprint;
    satellite_footprint.add(vgl_point_2d<double>(res_sptr->resources_[i].meta_->lower_left_.x(),  res_sptr->resources_[i].meta_->lower_left_.y()));
    satellite_footprint.add(vgl_point_2d<double>(res_sptr->resources_[i].meta_->upper_right_.x(), res_sptr->resources_[i].meta_->upper_right_.y()));
    volm_geo_index2::get_leaves(res_sptr->root_, leaves, satellite_footprint);
    for (auto & leave : leaves) {
      auto* leaf_ptr = dynamic_cast<volm_geo_index2_node<std::vector<unsigned> >* >(leave.ptr());
      leaf_ptr->contents_.push_back(i);  // push this satellite image to this leave that intersects its footprint
    }
  }

  return res_sptr;
}

static void create_a_test_kml(std::string const& kml_file)
{
  vgl_polygon<double> poly;
  std::vector<vgl_point_2d<double> > points;
  points.emplace_back(36.18825968469867,32.32497866643852);
  points.emplace_back(36.22929773807768,32.34162986956272);
  points.emplace_back(36.22927331990476,32.34222250106324);
  points.emplace_back(36.19077105901775,32.34223021056664);
  poly.push_back(points);
  std::ofstream ofs(kml_file.c_str());
  bkml_write::open_document(ofs);
  bkml_write::write_polygon(ofs, poly, "footprint", "");
  bkml_write::close_document(ofs);
  return;
}

static void test_overlapping_resources()
{
  // resource file created by create_satellite_resources.py, which calls
  std::string kml_file = "./test_overlapping.kml";
  create_a_test_kml(kml_file);
  volm_satellite_resources_sptr res = create_test_sat_res();
  std::cout << "there are " << res->resources_size() << " resources in the file!\n";

  std::vector<std::string> overlapping_res;
  res->highly_overlapping_resources(overlapping_res, res, kml_file, 100.0f);

  for (const auto & overlapping_re : overlapping_res)
    {
    std::cout << overlapping_re << std::endl;
    }
  std::cout << std::endl;

}

static void test_intersecting_resources()
{
  // resource file created by create_satellite_resources.py, which calls
  std::string kml_file = "./test_overlapping.kml";
  create_a_test_kml(kml_file);
  volm_satellite_resources_sptr res = create_test_sat_res();
  std::cout << "there are " << res->resources_size() << " resources in the file!\n";
#if 0 // FIX BUGS AS NUMBER OF RESOURCE IS SMALL
  std::vector<std::string> overlapping_res;
  res->highly_intersecting_resources(overlapping_res, res, kml_file);

  for(int i=0; i < overlapping_res.size(); ++i) {
    std::cout << overlapping_res[i] << std::endl;
  }
  std::cout << std::endl;
#endif
}

static void test_compute_intersection()
{
  // resource file created by create_satellite_resources.py, which calls
  volm_satellite_resources_sptr res = create_test_sat_res();
  std::cout << "there are " << res->resources_size() << " resources in the file!\n";

  std::vector<vgl_polygon<double> > footprints;
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(  6.96578484, -14.32022812));
    footprint.push_back(vgl_point_2d<double>(-0.56180803, -0.40059161));
    footprint.push_back(vgl_point_2d<double>(-27.60466641,  -6.32010066));
    footprint.push_back(vgl_point_2d<double>(-20.07707354, -20.23973717));
    footprint.push_back(vgl_point_2d<double>(  6.96578484, -14.32022812));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(  9.86031084,  17.30602255));
    footprint.push_back(vgl_point_2d<double>(-16.65467243,  23.37292325));
    footprint.push_back(vgl_point_2d<double>(-38.62524118,  10.88856858));
    footprint.push_back(vgl_point_2d<double>(-12.11025791,   4.82166788));
    footprint.push_back(vgl_point_2d<double>(  9.86031084,  17.30602255));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-24.47583861,   0.79007923));
    footprint.push_back(vgl_point_2d<double>( -4.52928017, -18.30042274));
    footprint.push_back(vgl_point_2d<double>( 12.73037319,   0.69530667));
    footprint.push_back(vgl_point_2d<double>( -7.21618526,  19.78580864));
    footprint.push_back(vgl_point_2d<double>(-24.47583861,   0.79007923));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-10.4571369 , -18.17991572));
    footprint.push_back(vgl_point_2d<double>(-14.54043578, -11.46874204));
    footprint.push_back(vgl_point_2d<double>( -2.01418419,  10.16162632));
    footprint.push_back(vgl_point_2d<double>( 2.06911469,  3.45045264));
    footprint.push_back(vgl_point_2d<double>(-10.4571369 , -18.17991572));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>( 12.64561638,  -4.16966793));
    footprint.push_back(vgl_point_2d<double>( 10.00981543, -22.189241 ));
    footprint.push_back(vgl_point_2d<double>( -7.10074225, -23.04330731));
    footprint.push_back(vgl_point_2d<double>(-4.4649413 , -5.02373424));
    footprint.push_back(vgl_point_2d<double>( 12.64561638,  -4.16966793));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(  0.85833915,  35.76693877));
    footprint.push_back(vgl_point_2d<double>( 7.31103026,  9.67003435));
    footprint.push_back(vgl_point_2d<double>( 27.07958284, -17.26309788));
    footprint.push_back(vgl_point_2d<double>( 20.62689172,   8.83380654));
    footprint.push_back(vgl_point_2d<double>(  0.85833915,  35.76693877));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-17.54710261,  -7.46710868));
    footprint.push_back(vgl_point_2d<double>(-29.27027646,   3.58687291));
    footprint.push_back(vgl_point_2d<double>(-5.1306033 ,  7.17772329));
    footprint.push_back(vgl_point_2d<double>( 6.59257055, -3.8762583));
    footprint.push_back(vgl_point_2d<double>(-17.54710261,  -7.46710868));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>( 20.59928871,  12.71430972));
    footprint.push_back(vgl_point_2d<double>(-7.33740002, -1.75889139));
    footprint.push_back(vgl_point_2d<double>(-31.89662415, -11.51022845));
    footprint.push_back(vgl_point_2d<double>(-3.95993541,  2.96297266));
    footprint.push_back(vgl_point_2d<double>( 20.59928871,  12.71430972));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>( 2.32929353,  6.01126638));
    footprint.push_back(vgl_point_2d<double>(  5.13191029,  34.18634405));
    footprint.push_back(vgl_point_2d<double>( 24.04064296,  17.67837464));
    footprint.push_back(vgl_point_2d<double>( 21.2380262 , -10.49670302));
    footprint.push_back(vgl_point_2d<double>( 2.32929353,  6.01126638));
    footprints.push_back(footprint);
  }
  {
    vgl_polygon<double> footprint(1);
    footprint.push_back(vgl_point_2d<double>(-3.92395365, -2.01537927));
    footprint.push_back(vgl_point_2d<double>(  1.95004508, -26.70582914));
    footprint.push_back(vgl_point_2d<double>(-23.36240902,  -8.46480089));
    footprint.push_back(vgl_point_2d<double>(-29.23640775,  16.22564899));
    footprint.push_back(vgl_point_2d<double>(-3.92395365, -2.01537927));
    footprints.push_back(footprint);
  }
#if 0// FIX BUGS AS NUMBER OF RESOURCE IS SMALL
  std::vector<unsigned> overlapping_ids;
  res->highly_intersecting_resources(overlapping_ids, footprints, 2, 5);

  for(int i=0; i < overlapping_ids.size(); ++i) {
    std::cout << overlapping_ids[i] << ",";
  }
  std::cout << std::endl;
#endif
}

TESTMAIN(test_intersecting_resources);

TESTMAIN(test_overlapping_resources);

TESTMAIN(test_compute_intersection);
