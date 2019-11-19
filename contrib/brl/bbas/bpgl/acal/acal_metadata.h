// This is//external/acal/acal_metadata.h
#ifndef acal_metadata_h
#define acal_metadata_h

//:
// \file
// \brief A class to represent metadata for a regoin of interest
// \author J.L. Mundy
// \date Dec 16, 2018
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vcl_compiler.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <bjson/bjson.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_2d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vul/vul_file.h>

struct image_metadata
{
  std::string image_name_;
  vgl_vector_2d<double> sph_view_dir_;
  size_t year_;
  size_t month_;
  size_t day_;
  size_t hour_;
  size_t min_;
  size_t sec_;
  double gsd_;
  std::string img_type_;
  vgl_vector_2d<double> sph_sun_dir_;
  std::string sensor_platform_;
  double cloud_per_;
};

struct tile_metadata
{
  size_t tile_id_;
  double dsm_resolution_;
  double aster_mean_;
  vgl_point_3d<double> global_lower_left_;
  vgl_point_3d<double> global_upper_right_;
  vgl_box_3d<double> global_bb_;
  vgl_point_3d<double> local_lower_left_;
  vgl_point_3d<double> local_upper_right_;
  vgl_box_3d<double> local_bb_;
};

struct tile_image_metadata
{
  size_t image_id_;
  std::string image_name_;
};

struct geo_corr_metadata
{
  size_t image_id_;
  std::string image_name_;
  vgl_vector_2d<double> translation_;
  double rms_proj_err_;
  bool seed_camera_;
};

struct pair_selection_metadata
{
  size_t pair_index_;
  size_t image1_id_;
  size_t image2_id_;
  double gsd_cost_;
  double sun_angle_cost_;
  double view_angle_cost_;
  double cost_;
};


class acal_metadata
{
 public:
 acal_metadata(){}

 void deserialize_image_meta( Json::Value& root)
 {
   const Json::Value img_list = root;
   for(Json::Value::const_iterator lit = img_list.begin();
       lit != img_list.end(); ++lit)
   {
     image_metadata im;
     im.image_name_ = (*lit).get("id", "").asString();
     const Json::Value meta = (*lit)["meta"];
     if(meta != Json::nullValue){
       const Json::Value vdir = meta["view_angles"];
       if(vdir != Json::nullValue){
         Json::Value::const_iterator vit = vdir.begin();
         double az = (*vit++).asDouble(), el = (*vit).asDouble();
         im.sph_view_dir_.set(az, el);
       }
       const Json::Value dt = meta["acquisition_time"];
       if(dt != Json::nullValue){
         Json::Value::const_iterator dit = dt.begin();
         im.year_ = (*dit++).asUInt();
         im.month_ = (*dit++).asUInt();
         im.day_ = (*dit++).asUInt();
         im.hour_ = (*dit++).asUInt();
         im.min_ = (*dit++).asUInt();
         im.sec_ = (*dit).asUInt();
       }
       im.gsd_ = meta.get("gsd", 0.0).asDouble();
       im.sensor_platform_ = meta.get("platform_name", "").asString();
       const Json::Value sdir = meta["sun_angles"];
       if(dt != Json::nullValue){
         Json::Value::const_iterator sit = sdir.begin();
         double az = (*sit++).asDouble();  double el = (*sit).asDouble();
         im.sph_sun_dir_.set(az, el);
       }
       im.img_type_ = meta.get("image_type", "").asString();
       im.cloud_per_ = meta.get("cloud_percentage", 0.0).asDouble();
     }
     img_meta_.push_back(im);
   }
 }

 void deserialize_tile_meta( Json::Value& root)
 {
   const Json::Value img_list = root;
   Json::Value::Members tile_ids = root.getMemberNames();
   size_t n = tile_ids.size();
   for(size_t i  = 0; i<n; ++i)
     {
       tile_metadata tm;
       std::string key = tile_ids[i];
       const Json::Value tile_info = root.get(key, "");
       tm.tile_id_ = std::stoi(key);
       tm.aster_mean_ = tile_info["aster_mean"].asDouble();
       tm.dsm_resolution_ = tile_info["dsm_resolution"].asDouble();
       const Json::Value local = tile_info["local"];
       const Json::Value ll = local["lower_left"];
       if(ll != Json::nullValue){
        Json::Value::const_iterator vit = ll.begin();
        double lon = (*vit++).asDouble(), lat = (*vit++).asDouble(), elev = (*vit++).asDouble();
        tm.local_lower_left_.set(lon, lat, elev);
       }
       const Json::Value ur = local["upper_right"];
       if (ur != Json::nullValue) {
          Json::Value::const_iterator vit = ur.begin();
          double lon = (*vit++).asDouble(), lat = (*vit++).asDouble(), elev = (*vit++).asDouble();
          tm.local_upper_right_.set(lon, lat, elev);
       }
       tm.local_bb_ = vgl_box_3d<double>(tm.local_lower_left_, tm.local_upper_right_);
       const Json::Value global = tile_info["global"];
       const Json::Value gll = global["lower_left"];
       if (gll != Json::nullValue) {
           Json::Value::const_iterator vit = gll.begin();
           double lon = (*vit++).asDouble(), lat = (*vit++).asDouble(), elev = (*vit++).asDouble();
           tm.global_lower_left_.set(lon, lat, elev);
       }
       const Json::Value gur = global["upper_right"];
       if (gur != Json::nullValue) {
           Json::Value::const_iterator vit = gur.begin();
           double lon = (*vit++).asDouble(), lat = (*vit++).asDouble(), elev = (*vit++).asDouble();
           tm.global_upper_right_.set(lon, lat, elev);
       }
       tm.global_bb_ = vgl_box_3d<double>(tm.global_lower_left_, tm.global_upper_right_);

     tile_meta_.push_back(tm);
   }
 }
void deserialize_tile_image_meta( Json::Value& root)
 {
   const Json::Value img_list = root;
   Json::Value::Members image_ids = root.getMemberNames();
   size_t n = image_ids.size();
   for(size_t i  = 0; i<n; ++i)
     {
       std::string key = image_ids[i];
       std::string image_path = root.get(key, "").asString();
       std::string base_name = vul_file::strip_directory(image_path);
       base_name = vul_file::strip_extension(base_name);
       tile_image_metadata tim;
       std::stringstream ss;
       ss << key;
       size_t id = -1;
       ss >> id;
       tim.image_id_ = id;
       tim.image_name_ = base_name;
       tile_image_meta_.push_back(tim);
     }
 }
 void serialize_geo_corr_meta( Json::Value& root)
 {
   Json::Value geo_corr_list;
   size_t n = geo_corr_meta_.size();
   if(n == 0){
     std::cout << "no geo-correction to serialize" << std::endl;
     return;
   }
   for(size_t i = 0; i<n; ++i)
   {
     Json::Value geo_corr;
     geo_corr["iname"] = geo_corr_meta_[i].image_name_;
     geo_corr["image_id"] = static_cast<Json::LargestUInt>(geo_corr_meta_[i].image_id_);
     geo_corr["samp_trans"] = geo_corr_meta_[i].translation_.x();
     geo_corr["line_trans"] = geo_corr_meta_[i].translation_.y();
     geo_corr["rms_proj_err"] = geo_corr_meta_[i].rms_proj_err_;
     geo_corr["seed_camera"] = geo_corr_meta_[i].seed_camera_;
     Json::Value::ArrayIndex ai = static_cast<Json::Value::ArrayIndex>(i);
     geo_corr_list[ai] = geo_corr;
   }
   root = geo_corr_list;
 }

 void deserialize_geo_corr_meta( Json::Value& root)
 {
   geo_corr_meta_.clear();
   const Json::Value geo_corr_list = root;
   for(Json::Value::const_iterator glit = geo_corr_list.begin();
       glit != geo_corr_list.end(); ++glit)
   {
     geo_corr_metadata gm;
     gm.image_id_     = (*glit).get("image_id", -1).asUInt();
     gm.image_name_   = (*glit).get("iname", "").asString();
     gm.rms_proj_err_ = (*glit).get("rms_proj_err", 0.0).asDouble();
     gm.seed_camera_  = (*glit).get("seed_camera", false).asBool();
     double tu = 0.0, tv = 0.0;
     tu = (*glit).get("samp_trans", 0.0).asDouble();
     tv = (*glit).get("line_trans", 0.0).asDouble();
     gm.translation_.set(tu, tv);
     geo_corr_meta_.push_back(gm);
   }
 }

 void serialize_pair_selection_meta( Json::Value& root)
 {
   Json::Value pair_list;
   size_t n = pair_meta_.size();
   if(n == 0){
     std::cout << "no pair selection to serialize" << std::endl;
     return;
   }
   for(size_t i = 0; i<n; ++i)
   {
     Json::Value pair;
     pair["pair_index"] = static_cast<Json::LargestUInt>(pair_meta_[i].pair_index_);
     pair["image1_id"] = static_cast<Json::LargestUInt>(pair_meta_[i].image1_id_);
     pair["image2_id"] = static_cast<Json::LargestUInt>(pair_meta_[i].image2_id_);
     pair["gsd_cost"] = pair_meta_[i].gsd_cost_;
     pair["sun_angle_cost"] = pair_meta_[i].sun_angle_cost_;
     pair["view_angle_cost"] = pair_meta_[i].view_angle_cost_;
     pair["total_cost"] = pair_meta_[i].cost_;
     Json::Value::ArrayIndex ai = static_cast<Json::Value::ArrayIndex>(i);
     pair_list[ai] = pair;
   }
   root = pair_list;
 }
  void deserialize_pair_selection_meta( Json::Value& root)
 {
   pair_meta_.clear();
   const Json::Value pair_list = root;
   for(Json::Value::const_iterator prit = pair_list.begin();
       prit != pair_list.end(); ++prit)
   {
     pair_selection_metadata pm;
     pm.pair_index_       = (*prit).get("pair_index", -1).asUInt();
     pm.image1_id_       = (*prit).get("image1_id", -1).asUInt();
     pm.image2_id_       = (*prit).get("image2_id", -1).asUInt();
     pm.gsd_cost_         = (*prit).get("gsd_cost", 0.0).asDouble();
     pm.sun_angle_cost_   = (*prit).get("sun_angle_cost", 0.0).asDouble();
     pm.view_angle_cost_  = (*prit).get("view_angle_cost", 0.0).asDouble();
     pm.cost_             = (*prit).get("total_cost", 0.0).asDouble();
     pair_meta_.push_back(pm);
   }
 }

 // metadata elements
 std::vector<image_metadata> img_meta_;
 std::vector<tile_metadata> tile_meta_;
 std::vector<tile_image_metadata> tile_image_meta_;
 std::vector<geo_corr_metadata> geo_corr_meta_;
 std::vector<pair_selection_metadata> pair_meta_;
};

#endif
