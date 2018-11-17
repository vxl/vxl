#include <iostream>
#include <algorithm>
#include "volm_query.h"
//:
// \file
#include <bpgl/bpgl_camera_utils.h>
#include <volm/volm_spherical_container.h>
#include <bsol/bsol_algs.h>
#include <vil/vil_image_view.h>
#include <vil/vil_rgb.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <volm/volm_spherical_layers.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <volm/volm_camera_space.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_set_io.h>

#define TOL -1E-8

volm_query::volm_query(const volm_camera_space_sptr& cam_space,
                       std::string const& label_xml_file,
                       std::string const& category_file,
                       volm_spherical_container_sptr const& sph,
                       volm_spherical_shell_container_sptr const& sph_shell)
: cam_space_(cam_space), invalid_((unsigned char)255), sph_depth_(sph), sph_(sph_shell)
{
  //the discrete rays defined on the sphere as x, y, z
  query_points_ = sph_->cart_points();
  query_size_ = (unsigned)query_points_.size();

  // load the labelme xml to define depth_map_scene and associated default value of camera parameters
  dm_ = new depth_map_scene;
  std::string img_category;
  volm_io::read_labelme(label_xml_file, category_file, dm_, img_category);
  // the dimensions of the depth image
  ni_ = dm_->ni();
  nj_ = dm_->nj();
  // the image may be downsampled to provide more efficient matching
  log_downsample_ratio_ = 0;//initial scale = 1
  // the larger dimension of the image
  unsigned bigger = ni_ > nj_ ? ni_ : nj_;
  // find the scale that keeps the largest image dimension > 500
  while ((bigger>>log_downsample_ratio_) > 500)
    ++log_downsample_ratio_;
  std::cout << "log_downsample_ratio_: " << log_downsample_ratio_ << std::endl; // need flush

#if NO_CAM_SPACE_CLASS
  //
  // set the default camera hypothesis parameters
  // based on img_category ( "desert" and "coast")
  //
  if (img_category == "desert") {
    head_ = 0.0;      head_d_ = 180.0; head_inc_ = 2.0;
    tilt_ = 90.0;     tilt_d_ = 20.0; tilt_inc_ = 2.0;
    roll_ = 0.0;      roll_d_ = 3.0; roll_inc_ = 2.0; // try +2 -2
    tfov_ = 5.0;      tfov_d_ = 5.0; tfov_inc_ = 2.0;
  }
  else if (img_category == "coast") {
    // temporary use desert default
    head_ = 0.0;      head_d_ = 180.0; head_inc_ = 2.0;
    tilt_ = 90.0;     tilt_d_ = 0.0; tilt_inc_ = 2.0;
    roll_ = 0.0;      roll_d_ = 1.0; roll_inc_ = 1.0;
    tfov_ = 5.0;      tfov_d_ = 30.0; tfov_inc_ = 2.0;
  }
  else {
    // undefined img category, use desert default
    head_ = 0.0;      head_d_ = 180.0; head_inc_ = 2.0;
    tilt_ = 90.0;     tilt_d_ = 20.0; tilt_inc_ = 2.0;
    roll_ = 0.0;      roll_d_ = 1.0; roll_inc_ = 1.0;
    tfov_ = 5.0;      tfov_d_ = 30.0;  tfov_inc_ = 2.0;
  }
  // read the camera file but don't use the values
  // use the default values instead
  if (use_default_) {
    double lat, lon, head, head_d, tilt, tilt_d, roll, roll_d, tfov, tfov_d, altitude;
    // load the camera kml, fetch the camera value and deviation
    // for now we don't trust angle params read from the camera file
    volm_io::read_camera(cam_kml_file, ni_, nj_, head, head_d, tilt, tilt_d, roll, roll_d, tfov, tfov_d, altitude, lat, lon);
    // overwrite roll deviation for now
    altitude_ = 3.0;
    roll_d_ = 3.0;
    std::cout << "In volm_query() - default values are used:\nheading: "
             << head_ << " dev: " << head_d_
             << "\ntilt: " << tilt_ << " dev: " << tilt_d_
             << "\nroll: " << roll_ << " dev: " << roll_d_
             << " (hard-coded to 3 till .kml passes meaningful values!!)\n"
             << "top_fov: " << tfov_ << " dev: " << tfov_d_
             << " alt: " << altitude_ << std::endl;
  }
  else {//overwrite the camera parameters with those in the camera file
    double lat, lon;
    volm_io::read_camera(cam_kml_file, ni_, nj_, head_, head_d_, tilt_, tilt_d_, roll_, roll_d_, tfov_, tfov_d_, altitude_, lat, lon);
    std::cout << "In volm_query() - values are read:\nheading: "
             << head_ << " dev: " << head_d_
             << "\ntilt: " << tilt_ << " dev: " << tilt_d_
             << "\nroll: " << roll_ << " dev: " << roll_d_
             << "\ntop_fov: " << tfov_ << " dev: " << tfov_d_
             << " alt: " << altitude_ << std::endl;
  }
#endif

  altitude_ = cam_space_->altitude();
  // create camera hypotheses
  this->create_cameras();
  std::cout << " volm_query has " << this->get_cam_num() << " cameras" << std::endl;
  // generate polygon vector based on defined order
  this->generate_regions();
  ground_orient_ = volm_orient_table::ori_id["horizontal"];
  sky_orient_ = volm_orient_table::ori_id["infinite"];
  std::cout << "volm_query generated " << this->depth_regions().size() << " regions! ingesting...\n"; std::cout.flush();
  // start to ingest query, with min_dist and order implemented
  this->query_ingest();
  // create offset vector
  this->offset_ingest();
  // start to ingest order, store in order_index_
  this->order_ingest();
  // implement the weight parameters for all objects
}

// build a query from an existing depth map scene
volm_query::volm_query(const volm_camera_space_sptr& cam_space,
                       std::string const& depth_map_scene_file,
                       volm_spherical_shell_container_sptr const& sph_shell,
                       volm_spherical_container_sptr const& sph)
: cam_space_(cam_space), invalid_((unsigned char)255), sph_depth_(sph), sph_(sph_shell)
{
  //the discrete rays defined on the sphere as x, y, z
  query_points_ = sph_->cart_points();
  query_size_ = (unsigned)query_points_.size();

  // load the depth_map_scene from depth_map_scene binary
  dm_ = new depth_map_scene;
  vsl_b_ifstream dis(depth_map_scene_file.c_str());
  dm_->b_read(dis);
  dis.close();
  // the dimensions of the depth image
  ni_ = dm_->ni();
  nj_ = dm_->nj();
  // the image may be downsampled to provide more efficient matching
  log_downsample_ratio_ = 0;//initial scale = 1
  // the larger dimension of the image
  unsigned bigger = ni_ > nj_ ? ni_ : nj_;
  // find the scale that keeps the largest image dimension > 500
  while ((bigger>>log_downsample_ratio_) > 500)
    ++log_downsample_ratio_;
  std::cout << "log_downsample_ratio_: " << log_downsample_ratio_ << std::endl; // need flush

  altitude_ = cam_space_->altitude();
  this->create_cameras();
  std::cout << " volm_query has " << this->get_cam_num() << " cameras" << std::endl;
  // generate polygon vector based on defined order
  this->generate_regions();
  ground_orient_ = volm_orient_table::ori_id["horizontal"];
  sky_orient_ = volm_orient_table::ori_id["infinite"];
  std::cout << "volm_query generated " << this->depth_regions().size() << " regions! ingesting...\n"; std::cout.flush();
  // start to ingest query, with min_dist and order implemented
  this->query_ingest();
  // create offset vector
  this->offset_ingest();
  // start to ingest order, store in order_index_
  this->order_ingest();
  // implement the weight parameters for all objects
  //this->weight_ingest();
}

volm_query::volm_query(std::string const& query_file, const volm_camera_space_sptr& cam_space,
                       std::string const& depth_map_scene_file,
                       volm_spherical_shell_container_sptr const& sph_shell,
                       volm_spherical_container_sptr const& sph)
: cam_space_(cam_space), invalid_((unsigned char)255), sph_depth_(sph), sph_(sph_shell)
{

  //the discrete rays defined on the sphere as x, y, z
  std::cout << "depth_map_scene_file = " << depth_map_scene_file << std::endl;

  query_points_ = sph_->cart_points();
  query_size_ = (unsigned)query_points_.size();

  // load the depth_map_scene from depth_map_scene binary
  dm_ = new depth_map_scene;
  vsl_b_ifstream dis(depth_map_scene_file.c_str());

  dm_->b_read(dis);
  dis.close();
  // the dimensions of the depth image
  ni_ = dm_->ni();
  nj_ = dm_->nj();
  // the image may be downsampled to provide more efficient matching
  log_downsample_ratio_ = 0;//initial scale = 1
  // the larger dimension of the image
  unsigned bigger = ni_ > nj_ ? ni_ : nj_;
  // find the scale that keeps the largest image dimension > 500
  while ((bigger>>log_downsample_ratio_) > 500)
    ++log_downsample_ratio_;
  //std::cout << "log_downsample_ratio_: " << log_downsample_ratio_ << std::endl; // need flush

  depth_regions_ = dm_->scene_regions();
  // sort the regions on depth order
  std::sort(depth_regions_.begin(), depth_regions_.end(), compare_order());

  altitude_ = cam_space_->altitude();

  // read the rest from the binary file
  vsl_b_ifstream dis_self(query_file.c_str());
  this->read_data(dis_self);
  dis_self.close();
  std::cout << " volm_query has " << this->get_cam_num() << " cameras" << std::endl;
}


#if NO_CAM_SPACE_CLASS
// generate the set of camera hypotheses
// the camera space includes multiple
// choices for focal length, heading, tilt and roll
void volm_query::create_cameras()
{
  // set up the camera parameter arrays and construct vector of cameras
  if (use_default_)//define focal length search space
  {
    if (img_category == "desert") {
      //field of view angle choices (degrees)
      double stock[] = {18.0, 19.0,
                        20.0, 24.0, 26.0,
                        28.0, 30.0, 32.0};
      //store in vector
      if (ni_ >= nj_) {  // landscape
        top_fov_.insert(top_fov_.end(), stock, stock + 8);
      }
      else { // protrait, so take into account aspect ratio
        double dtor = vnl_math::pi_over_180;
        for (unsigned i = 0; i < 8; ++i) {
          double tr = std::tan(stock[i]*dtor);
          double top = std::atan(nj_*tr/ni_)/dtor;
          top_fov_.push_back(top);
        }
      }
      std::cout << " NOTE: default top field of view is used:\n\t[ ";
      for (unsigned i = 0; i < 8; ++i)
        std::cout << top_fov_[i] << ' ';
      std::cout << ']' << std::endl;
    }
    else if (img_category == "coast") {
      double stock[] = {3.0, 4.0, 5.0, 9.0,
                        15.0, 16.0,
                        20.0, 21.0, 22.0, 23.0, 24.0};
      if (ni_ >= nj_) {   // landscape
        top_fov_.insert(top_fov_.end(), stock, stock + 11);
      }
      else {             // protrait
        double dtor = vnl_math::pi_over_180;
        for (unsigned i = 0; i < 11; ++i) {
          double tr = std::tan(stock[i]*dtor);
          double top = std::atan(nj_*tr/ni_)/dtor;
          top_fov_.push_back(top);
        }
      }
      std::cout << " NOTE: default top field of view is used:\n\t[ ";
      for (unsigned i = 0; i < 11; ++i)
        std::cout << top_fov_[i] << ' ';
      std::cout << ']' << std::endl;
    }
    else {
      double stock[] = {3.0,  4.0, 5.0,
                        12.0, 17.0, 18.0,19.0,
                       20.0, 24.0};
      top_fov_.insert(top_fov_.end(), stock, stock + 9);
      std::cout << " NOTE: default top field of view is used:\n\t[ ";
      for (unsigned i = 0; i < 9; ++i)
        std::cout << top_fov_[i] << ' ';
      std::cout << ']' << std::endl;
    }
  }// end use_default == true
  else {
    tfov_inc_ = 2.0;
#if 0
    if (tfov_ < 10)      tfov_inc_ = 1.0;
    else if (tfov_ > 20) tfov_inc_ = 4.0;
    else                 tfov_inc_ = 2.0;
#endif
    top_fov_.push_back(tfov_);    // top viewing ranges from 1 to 89
    std::cout << "\t top_fov:\n\t " << tfov_ << ' ';
    for (double i = tfov_inc_; i <= tfov_d_; i+=tfov_inc_) {
      double right_fov = tfov_ + i, left_fov = tfov_ - i;
      if (right_fov > 89)  right_fov = 89;
      if (left_fov  < 1)   left_fov = 1;
      top_fov_.push_back(right_fov);
      if (left_fov != right_fov) {
        top_fov_.push_back(left_fov);
        std::cout << right_fov << ' ' << left_fov << ' ';
      }
      else
        std::cout << right_fov << ' ';
    }
  }
  // store heading hypotheses
  headings_.push_back(head_);
  std::cout << "\n\t headings:\n\t " << head_ << ' ';
  for (double i = head_inc_; i <= head_d_; i+= head_inc_) {   // heading ranges from 0 to 360
    double right_hd = head_ + i, left_hd = head_ - i;
    if (right_hd > 360) right_hd = right_hd - 360;
    if (left_hd < 0)    left_hd = left_hd + 360;
    headings_.push_back(right_hd);
    if (left_hd != right_hd) {
      headings_.push_back(left_hd); std::cout << right_hd << ' ' << left_hd << ' ';
    }
    else
      std::cout << right_hd << ' ';
  }
  // store tilt hypotheses
  std::cout << "\n\t tilts:\n\t " << tilt_ << ' ';
  tilts_.push_back(tilt_);   // tilt ranges from 0 to 180
  for (double i =  tilt_inc_; i <= tilt_d_; i+= tilt_inc_) {
    double right_tlt = tilt_ + i, left_tlt = tilt_ - i;
    if (right_tlt > 180) right_tlt = 180;
    if (left_tlt < 0)    left_tlt = 0;
    tilts_.push_back(right_tlt);  tilts_.push_back(left_tlt);
    std::cout << right_tlt << ' ' << left_tlt << ' ';
  }
  // store roll hypotheses
  std::cout << "\n\t rolls:\n\t " << roll_ << ' ';
  rolls_.push_back(roll_);    // roll ranges from -180 to 180 in kml, how about in camera_from_kml ?? (need to check...)
  for (double i = roll_inc_; i <= roll_d_; i+=roll_inc_) {
    double right_rol = roll_ + i , left_rol = roll_ - i;
    if (right_rol > 180) right_rol = right_rol - 360;
    if (left_rol < -180) left_rol = left_rol + 360;
    rolls_.push_back(roll_ + i);  rolls_.push_back(roll_ - i);
    std::cout << right_rol << ' ' << left_rol << ' ';
  }
  std::cout << '\n';
  std::cout.flush();

  // construct the camera hypotheses
  for (unsigned i = 0; i < tilts_.size(); ++i)
    for (unsigned j = 0; j < rolls_.size(); ++j)
      for (unsigned k = 0; k < top_fov_.size(); ++k)
        for (unsigned h = 0; h < headings_.size(); ++h)
        {
          double tilt = tilts_[i], roll = rolls_[j], top_f = top_fov_[k], head = headings_[h];
          double dtor = vnl_math::pi_over_180;
          double ttr = std::tan(top_f*dtor);
          double right_f = std::atan(ni_*ttr/nj_)/dtor;
          vpgl_perspective_camera<double> cam = bpgl_camera_utils::camera_from_kml((double)ni_, (double)nj_, right_f, top_f, altitude_, head, tilt, roll);
          // check whether current camera is consistent with defined ground plane
          // that is, if the 2-d ground plane region actually projects onto the 3-d ground plane
          bool success = true;
          if (dm_->ground_plane().size()) {
            for (unsigned i = 0; (success && i < dm_->ground_plane().size()); ++i) {
              success = dm_->ground_plane()[i]->region_ground_2d_to_3d(cam);
#if 0
              std::cout << "checking ground plane consistency for: " << dm_->ground_plane()[i]->name() << " min depth is: " << dm_->ground_plane()[i]->min_depth()
                       << ' ' << (success ? "" : "not_") << "consistent!\n"
#endif
            }
            if (success) {
              cameras_.push_back(cam);
              camera_strings_.push_back(bpgl_camera_utils::get_string((double)ni_, (double)nj_, right_f, top_f, altitude_, head, tilt, roll));
            }
            else
            {
#if 0
              std::cout << "WARNING: following camera hypothesis is NOT consistent with defined ground plane in the query image and ignored\n"
                       << " \t heading = " << head << ", tilt = " << tilt << ", roll = " << roll << ", top_fov = " << top_f << ", right_fov = " << right_f << std::endl;
#endif
            }
          }
          else
          {
            cameras_.push_back(cam);
            camera_strings_.push_back(bpgl_camera_utils::get_string((double)ni_, (double)nj_, right_f, top_f, altitude_, head, tilt, roll));
          }
        }
}
#endif

void volm_query::create_cameras()
{
  // iterate over valid cameras in the camera space
  std::vector<unsigned int> const& valid_cams = cam_space_->valid_indices();
  for (unsigned int valid_cam : valid_cams) {
    vpgl_perspective_camera<double> cam = cam_space_->camera(valid_cam);
    cameras_.push_back(cam);
    std::string cam_str = cam_space_->get_string(valid_cam);
    camera_strings_.push_back(cam_str);
  }
}

// convert min and max distances from scene depth regions to voxel indices
// insure consistent order indices
// obtain the maximum distance bound (meters)
void volm_query::generate_regions()
{
  // generate the map of the depth_map_region based on their order
  // a vector of regions not including sky or ground plane
  depth_regions_ = dm_->scene_regions();
  auto size = (unsigned)depth_regions_.size();
  // sort the regions on depth order
  std::sort(depth_regions_.begin(), depth_regions_.end(), compare_order());
  // obtain the min and max dist for different non-ground, non-sky objects
  // distances are in terms of voxel indices
  for (unsigned i = 0; i < size; ++i) {
    min_obj_dist_.push_back(sph_depth_->get_depth_interval(depth_regions_[i]->min_depth()));
    max_obj_dist_.push_back(sph_depth_->get_depth_interval(depth_regions_[i]->max_depth()));
    order_obj_.push_back((unsigned char)depth_regions_[i]->order());
    obj_orient_.push_back((unsigned char)depth_regions_[i]->orient_type());

    auto land_id = (unsigned char)depth_regions_[i]->land_id();
    std::vector<unsigned char> land_fallback_id = volm_fallback_label::fallback_id[land_id];
    obj_land_id_.push_back(land_fallback_id);
    std::vector<float> land_fallback_wgt = volm_fallback_label::fallback_weight[land_id];
    obj_land_wgt_.push_back(land_fallback_wgt);
  }
  d_threshold_ = 20000.0; //upper depth bound
  for (unsigned i = 0; i < size; ++i) {
  //depth_regions_[(dm_->scene_regions())[i]->order()] = (dm_->scene_regions())[i];
    if (d_threshold_ < (dm_->scene_regions())[i]->max_depth())
      d_threshold_ = (dm_->scene_regions())[i]->max_depth();
  }
  if (dm_->ground_plane().size()) {
    for (unsigned i = 0; i < dm_->ground_plane().size(); ++i) {
      if (d_threshold_ < dm_->ground_plane()[i]->max_depth())
        d_threshold_ = dm_->ground_plane()[i]->max_depth();
    }
  }
  // set a constant value for sky objects, which is equal to minimum order for all sky objects
  // actually, order for sky shouldn't matter - JLM
  if (dm_->sky().size()) {
    order_sky_ = dm_->sky()[0]->order();
    for (unsigned i = 1; i < dm_->sky().size(); ++i) {
      if ( order_sky_ > dm_->sky()[i]->order() )
        order_sky_ = dm_->sky()[i]->order();
    }
  }
  // create the order_set for all non-ground region
  // std::set<unsigned>
 if (depth_regions_.size())
    for (auto & depth_region : depth_regions_)
      order_set_.insert(depth_region->order());
  if (dm_->sky().size())
    order_set_.insert(order_sky_);
}

bool volm_query::order_ingest()
{
  // loop over camera hypotheses
  auto cam_num = (unsigned)cameras_.size();
  for (unsigned cam_id = 0; cam_id < cam_num; ++cam_id) {
    // create a vector to store all objects and fetch the order for current layer
    std::vector<std::vector<unsigned> > order_cam(order_set_.size());
    std::vector<unsigned char> order_layer = order_[cam_id];
#if 0
    std::cout << " --------------------- camera " << cam_id << " --------------------" << std::endl;
#endif
    // loop over rays on query sphere
    for (unsigned idx = 0; idx < query_size_; ++idx) {
      unsigned count = 0;
      //std::cout << " cam " << cam_id << ", order_layer[" << idx << "] = " << (int)order_layer[idx] << std::endl;
      auto iter = this->order_set_.begin();
      for ( ; iter != order_set_.end(); ++iter) {
        if ( (int)order_layer[idx] == *iter) {
          order_cam[count].push_back(idx);
          break;
        }
        else {
          ++count;
        }
      }
    }
    order_index_.push_back(order_cam);
  } // loop over camera
  return true;
}

// computes a set of spherical shell layers for each camera hypothesis
// the layer contents are indexed by the camera ray, p_idx
bool volm_query::query_ingest()
{
  // loop over the set of camera hypotheses
  for (const auto& cam : cameras_)
  {
    // the layers for camera hypothesis i
    // the minimum distance for each ray
    std::vector<unsigned char> min_dist_layer;
    // the maximum distance for each ray
    std::vector<unsigned char> max_dist_layer;
    // the region order for each ray
    std::vector<unsigned char> order_layer;
    // the set of rays intersecting the ground plane
    std::vector<unsigned> ground_id_layer;
    // the ground plane distance for the ray
    std::vector<unsigned char> ground_dist_layer;
    // the ground plane land fallback class for the ray
    std::vector<std::vector<unsigned char> > ground_land_layer;
    // the ground pland land fallback class weight
    std::vector<std::vector<float> > ground_land_wgt_layer;
    // the set of rays intersecting sky
    std::vector<unsigned> sky_id_layer;
    // the set of rays for each non-gp, non-sky region
    std::vector<std::vector<unsigned> > dist_id_layer(depth_regions_.size());
    // put current camera into depth_map_scene
    dm_->set_camera(cam);
    // create an depth image for current camera if there is ground plane
    vil_image_view<float> depth_img;
    if (dm_->ground_plane().size()) {
      depth_img = dm_->depth_map("ground_plane", log_downsample_ratio_, d_threshold_);
    }

    // loop over rays on sphere
    unsigned count = 0;
    for (unsigned p_idx = 0; p_idx < query_size_; ++p_idx)
    {
      vgl_point_3d<double> qp(query_points_[p_idx].x(), query_points_[p_idx].y(), query_points_[p_idx].z()+altitude_);
      unsigned char min_dist, order, max_dist;
      // check whether the point is behind the camera
      if (cam.is_behind_camera(vgl_homg_point_3d<double>(qp))) {
        min_dist_layer.push_back((unsigned char)255);
        max_dist_layer.push_back((unsigned char)255);
        order_layer.push_back((unsigned char)255);
      }
      else {
        double u, v;
        cam.project(qp.x(), qp.y(), qp.z(), u, v);
        // compare (u, v) with depth_map_scene, return min_dist
        if ( u > (double)ni_ || v > (double)nj_ || u < TOL || v < TOL) {   // container point qp is outside camera viewing volume
          min_dist_layer.push_back((unsigned char)255);
          max_dist_layer.push_back((unsigned char)255);
          order_layer.push_back((unsigned char)255);
        }
        else {
          bool is_ground = false, is_sky = false, is_object = false;
          unsigned obj_id;
          std::vector<unsigned char> grd_fallback_id;
          std::vector<float> grd_fallback_wgt;
          min_dist = this->fetch_depth(u, v, order, max_dist, obj_id, grd_fallback_id, grd_fallback_wgt, is_ground, is_sky, is_object, depth_img);
          min_dist_layer.push_back(min_dist);
          max_dist_layer.push_back(max_dist);
          order_layer.push_back(order);
          if (is_ground) {
            ground_id_layer.push_back(p_idx);
            ground_dist_layer.push_back(min_dist);
            ground_land_layer.push_back(grd_fallback_id);
            ground_land_wgt_layer.push_back(grd_fallback_wgt);
          }
          else if (is_sky) {
            sky_id_layer.push_back(p_idx);
          }
          else if (is_object) {
            if (obj_id < depth_regions_.size()) {
              dist_id_layer[obj_id].push_back(p_idx);
            }
            else {
              std::cerr << "ERROR in query creation: object id exceeds the size of non-ground, non-sky objects\n";
              return false;
            }
          }
          if ((unsigned)min_dist != 255)
            ++count;
        }
      }
    } // loop over rays for current camera
    min_dist_.push_back(min_dist_layer);
    max_dist_.push_back(max_dist_layer);
    order_.push_back(order_layer);
    ground_id_.push_back(ground_id_layer);
    ground_dist_.push_back(ground_dist_layer);
    ground_land_id_.push_back(ground_land_layer);
    ground_land_wgt_.push_back(ground_land_wgt_layer);
    sky_id_.push_back(sky_id_layer);
    dist_id_.push_back(dist_id_layer);
  } // loop over cameras
  return true;
}

// u, v defines the pixel location in a depth image
// depth image is the ground plane depth map
// returns min distance to depth region that contains
// u, v
bool volm_query::offset_ingest()
{
  auto n_cam = (unsigned)cameras_.size();
  // create ground offset
  unsigned count = 0;
  ground_offset_.push_back(count);
  for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
    count += (unsigned)ground_id_[cam_id].size();
    ground_offset_.push_back(count);
  }

  // create sky offset
  count = 0;
  sky_offset_.push_back(count);
  for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id) {
    count += (unsigned)sky_id_[cam_id].size();
    sky_offset_.push_back(count);
  }

  // create object offset
  count = 0;
  auto n_obj = (unsigned)depth_regions_.size();
  dist_offset_.push_back(count);
  for (unsigned cam_id = 0; cam_id < n_cam; ++cam_id)
    for (unsigned obj_id = 0; obj_id < n_obj; ++obj_id) {
      count += (unsigned)dist_id_[cam_id][obj_id].size();
      dist_offset_.push_back(count);
    }

  return true;
}

unsigned char volm_query::fetch_depth(double const& u,
                                      double const& v,
                                      unsigned char& order,
                                      unsigned char& max_dist,
                                      unsigned& object_id,
                                      std::vector<unsigned char>& grd_fallback_id,
                                      std::vector<float>& grd_fallback_wgt,
                                      bool& is_ground,
                                      bool& is_sky,
                                      bool& is_object,
                                      vil_image_view<float> const& depth_img)
{
  unsigned char min_dist;
  // check other objects before ground,
  // e.g.,  for overlap region of a building and ground, building is on the ground and it is must closer than the ground
  // find if (u, v) is contained in non-ground, non-sky region
  if (depth_regions_.size()) {
    for (unsigned i = 0; i < depth_regions_.size(); ++i) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly(depth_regions_[i]->region_2d());
      if (poly.contains(u,v)) {
        is_object = true;
        object_id = i;
        double min_depth = depth_regions_[i]->min_depth();
        if (min_depth < 0)
          min_dist = (unsigned char)255;
        else
          min_dist = sph_depth_->get_depth_interval(min_depth);
        double max_depth = depth_regions_[i]->max_depth();
        if (max_depth < sph_depth_->min_voxel_res())
          max_dist = (unsigned char)255;
        else
          max_dist = sph_depth_->get_depth_interval(max_depth);
        order = (unsigned char)depth_regions_[i]->order();
        return min_dist;
      }
    }
  }
  // not contained in a non-ground non_sky region, check ground_plane
  if (dm_->ground_plane().size()) {
    for (unsigned i = 0; i < dm_->ground_plane().size(); ++i) {
      vgl_polygon<double> vgl_ground = bsol_algs::vgl_from_poly((dm_->ground_plane()[i])->region_2d());
      if (vgl_ground.contains(u,v)) {
        is_ground = true;
        // get the depth of the pixel
        // maybe better to do bilinear interpolation instead of casting to nearest pixel
        int uu = (int)std::floor(u/(1<<log_downsample_ratio_)+0.5);
        uu = uu < 0 ? 0 : uu;
        uu = uu >= (int)depth_img.ni() ? depth_img.ni()-1 : uu;
        int vv = (int)std::floor(v/(1<<log_downsample_ratio_)+0.5);
        vv = vv < 0 ? 0 : vv;
        vv = vv >= (int)depth_img.nj() ? depth_img.nj()-1 : vv;
        float depth_uv = depth_img(uu,vv);
        // handle the case where the voxel/ray is too close to ground_plane boundary
        if (depth_uv < 0) {
#ifdef DEBUG
          std::cout << " WARNING: point (" << (int)u << ',' << (int)v
                   << ") is too close to the ground boundary, disregarded." << std::endl;
#endif
          is_ground = false;
          max_dist = (unsigned char)255;
          order = (unsigned char)255;
          return (unsigned char)253; // invalid depth value
        }
        min_dist = sph_depth_->get_depth_interval(depth_uv);
        max_dist = (unsigned char)255;
        order = (unsigned char)(dm_->ground_plane()[i])->order();
        unsigned char grd_land = dm_->ground_plane()[i]->land_id();
        grd_fallback_id = volm_fallback_label::fallback_id[grd_land];
        grd_fallback_wgt = volm_fallback_label::fallback_weight[grd_land];
        return min_dist;
      }
    }
  }
  // check if (u, v) is contained in sky
  // considered last since all objects should be closer than sky
  if (dm_->sky().size()) {
    for (unsigned i = 0; i < dm_->sky().size(); ++i) {
      vgl_polygon<double> vgl_sky = bsol_algs::vgl_from_poly((dm_->sky()[i])->region_2d());
      if (vgl_sky.contains(u,v)) {
        is_sky = true;
        max_dist = (unsigned char)254;
        order = order_sky_;
        return (unsigned char)254;
      }
    }
  }
  // the image points (u,v) is not inside any defined objectes
  max_dist = (unsigned char)255;
  order = (unsigned char)255;
  return (unsigned char)255;
}

void volm_query::draw_template(std::string const& vrml_fname)
{
  // write the header and shell container first
  sph_->draw_template(vrml_fname);
  // write rays
  //this->draw_rays(vrml_fname);
  // write the camera
  unsigned cam_num = this->get_cam_num();
  for (unsigned i = 0; i < cam_num; ++i) {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    if (i%2 == 0)
      g = 1.0f;
    else if (i%2 == 1) {
      b = 1.0f; g = 0.0f;
    }
    else {
      r = 0.0f; g = 1.0f;
    }
    this->draw_viewing_volume(vrml_fname, cameras_[i], r, g, b);
  }
}

void volm_query::draw_rays(std::string const& fname)
{
  std::ofstream ofs(fname.c_str(), std::ios::app);
  double len = 800.0;
  vgl_point_3d<double> ori(0.0,0.0,0.0);
  for (unsigned i=0; i<query_size_; ++i) {
    vgl_ray_3d<double> ray(ori, query_points_[i]);
    bvrml_write::write_vrml_cylinder(ofs, ori, ray.direction(), (float)3.0, (float)len, 0.0f, 0.0f, 0.0f, 1);
  }
}

void volm_query::draw_viewing_volume(std::string const& fname, vpgl_perspective_camera<double> cam, float r, float g, float b)
{
  std::ofstream ofs(fname.c_str(), std::ios::app);
  // reset the center back to zero
  cam.set_camera_center(vgl_point_3d<double>(0.0,0.0,0.0) );
  //bvrml_write::write_vrml_cylinder(ofs, cam.get_camera_center(), cam.principal_axis(),0.2f, (double)(conf_focal_ + init_focal_),r,g,b);

  // create the viewing volume by rays
  vgl_ray_3d<double> rtl = cam.backproject_ray(vgl_point_2d<double>(0.0, 0.0));
  vgl_ray_3d<double> rtr = cam.backproject_ray(vgl_point_2d<double>((double)ni_, 0.0));
  vgl_ray_3d<double> rll = cam.backproject_ray(vgl_point_2d<double>(0.0, (double)nj_));
  vgl_ray_3d<double> rlr = cam.backproject_ray(vgl_point_2d<double>((double)ni_, (double)nj_));
  // calculate a scaling factor
  double scale = 0.5;
  double focal = (cam.get_calibration()).focal_length();
  double depth = focal * scale;
  double dist = depth / focal * 0.5 * std::sqrt(4*focal*focal + ni_*ni_ + nj_*nj_);
  // get image corner point
  vgl_point_3d<double> ptl = cam.get_camera_center() + dist*rtl.direction();
  vgl_point_3d<double> ptr = cam.get_camera_center() + dist*rtr.direction();
  vgl_point_3d<double> pll = cam.get_camera_center() + dist*rll.direction();
  vgl_point_3d<double> plr = cam.get_camera_center() + dist*rlr.direction();
  // draw the boundary face
  ofs << "Shape {\n"
      << " appearance Appearance {\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << 0 << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry IndexedFaceSet {\n"
      << "   coord Coordinate {\n"
      << "      point [\n"
      << "        " << cam.get_camera_center().x() << ' '
      << "        " << cam.get_camera_center().y() << ' '
      << "        " << cam.get_camera_center().z() << ",\n"
      << "        " << ptl.x() << ' ' << ptl.y() << ' ' << ptl.z() << ",\n"
      << "        " << ptr.x() << ' ' << ptr.y() << ' ' << ptr.z() << ",\n"
      << "        " << pll.x() << ' ' << pll.y() << ' ' << pll.z() << ",\n"
      << "        " << plr.x() << ' ' << plr.y() << ' ' << plr.z() << '\n'
      << "      ]\n"
      << "    }\n"
      << "    coordIndex [\n"
      << "  0, 1, 2, -1,\n"
      << "  0, 2, 4, -1,\n"
      << "  0, 3, 4, -1,\n"
      << "  0, 1, 3, -1,\n"
      << "    ]\n"
      << "  }\n"
      << "}\n";
  ofs.close();
}

void volm_query::draw_polygon(vil_image_view<vil_rgb<vxl_byte> >& img, vgl_polygon<double> const& poly, unsigned char const& depth)
{
  for (unsigned pi = 0; pi < poly.num_sheets(); ++pi)
  {
    for (unsigned vi = 0; vi < poly[pi].size(); ++vi)
    {
      vgl_point_2d<double> s = poly[pi][vi];
      vgl_point_2d<double> e;
      if (vi < poly[pi].size()-1)  e = poly[pi][vi+1];
      else  e = poly[pi][0];
      double k;
      if (e.x() == s.x()) k = 10000;
      else k = (e.y()-s.y())/(e.x()-s.x());
      double b = s.y() - k * s.x();
      if (std::sqrt(k*k) < 1) {// loop x
        if (s.x() <= e.x()) {
          for (auto xi = (unsigned)s.x(); xi <= (unsigned)e.x(); ++xi) {
            auto xj = (unsigned)(k*xi+b);
            if (xi < img.ni() && xj < img.nj()) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
        else {
          for (auto xi = (unsigned)e.x(); xi <= (unsigned)s.x(); ++xi) {
            auto xj = (unsigned)(k*xi+b);
            if (xi < img.ni() && xj < img.nj()) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
      }
      else {
        if (s.y() <= e.y()) {
          for (auto xj = (unsigned)s.y(); xj <= (unsigned)e.y(); ++xj) {
            auto xi = (unsigned)((xj-b)/k);
            if (xi < img.ni() && xj < img.nj()) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
        else {
          for (auto xj = (unsigned)e.y(); xj <= (unsigned)s.y(); ++xj) {
            auto xi = (unsigned)((xj-b)/k);
            if (xi < img.ni() && xj < img.nj()) {
              img(xi,xj).r = bvrml_color::heatmap_classic[(int)depth][0];
              img(xi,xj).g = bvrml_color::heatmap_classic[(int)depth][1];
              img(xi,xj).b = bvrml_color::heatmap_classic[(int)depth][2];
            }
          }
        }
      }
    }
  }
}

void volm_query::draw_dot(vil_image_view<vil_rgb<vxl_byte> >& img,
                          vgl_point_3d<double> const& world_point,
                          vil_rgb<vxl_byte> color,
                          vpgl_perspective_camera<double> const& cam)
{
  int dot_size = ( img.ni() < img.nj() ) ? (int)(0.01*ni_) : (int)(0.01*nj_);
  double u, v;
  vgl_homg_point_3d<double> current_p(world_point.x(), world_point.y(), world_point.z()+altitude_);
  if (!(cam.is_behind_camera(current_p))) {
    cam.project(world_point.x(), world_point.y(), world_point.z()+altitude_, u, v);
    int cx = (int)u;
    int cy = (int)v;
    for (int i = -dot_size; i < dot_size; ++i)
      for (int j = -dot_size; j < dot_size; ++j) {
        int x = cx + i ; int y = cy + j;
        if ( !(x < 0 || y < 0 || x >= (int)img.ni() || y >= (int)img.nj()) ) {
          img((unsigned)x, (unsigned)y) = color;
        }
      }
  }
}

void volm_query::draw_depth_map_regions(vil_image_view<vil_rgb<vxl_byte> >& out_img)
{
  // draw depth_map polygons on the depth images
  if (dm_->sky().size()) {
    for (unsigned i = 0; i < dm_->sky().size(); ++i) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->sky()[i])->region_2d());
      this->draw_polygon(out_img, poly, (unsigned char)254);
    }
  }
  if (dm_->ground_plane().size()) {
    unsigned ground_size = (unsigned)dm_->ground_plane().size();
    for (unsigned i = 0; i < ground_size; ++i) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->ground_plane()[i])->region_2d());
      this->draw_polygon(out_img, poly, (unsigned char)1);
    }
  }
  if (dm_->scene_regions().size()) {
    unsigned region_size = (unsigned)dm_->scene_regions().size();
    for ( unsigned i = 0; i < region_size; ++i) {
      vgl_polygon<double> poly = bsol_algs::vgl_from_poly((dm_->scene_regions())[i]->region_2d());
      double value = (dm_->scene_regions())[i]->min_depth();
      unsigned char depth = sph_depth_->get_depth_interval(value);
      this->draw_polygon(out_img, poly, depth);
    }
  }
}

void volm_query::depth_rgb_image(std::vector<unsigned char> const& values,
                                 unsigned const& cam_id,
                                 vil_image_view<vil_rgb<vxl_byte> >& out_img,
                                 const std::string& value_type)
{
  this->draw_depth_map_regions(out_img);

  vpgl_perspective_camera<double> cam = cam_space_->camera(cam_id);

  if (value_type == "orientation") {
    for (unsigned pidx = 0; pidx < query_size_; ++pidx) {
      vil_rgb<vxl_byte> color_id;
      if (values[pidx] == 0 || values[pidx] == 100)       //    invalid --> black
        color_id = vil_rgb<vxl_byte>(0,0,0);
      else if (values[pidx] == 1)                         // horizontal --> red
        color_id = vil_rgb<vxl_byte>(255,0,0);
      else if (values[pidx] > 1 && values[pidx] < 10)     //   vertical --> green
        color_id = vil_rgb<vxl_byte>(0,180,60);
      else if (values[pidx] == 254)                       //        sky --> blue
        color_id = vil_rgb<vxl_byte>(51,102,255);
      else
        color_id = vil_rgb<vxl_byte>(49,50,62);
      this->draw_dot(out_img, query_points_[pidx], color_id, cam);
    }
  }
  else if (value_type == "land") {
    for (unsigned pidx = 0; pidx < query_size_; ++pidx) {
      vil_rgb<vxl_byte> color_id;
      if (values[pidx] == 0)                   // invalid --> black
        color_id = vil_rgb<vxl_byte>(255,0,0);
      else if (values[pidx] == 254)            // sky --> blue
        color_id = vil_rgb<vxl_byte>(51,102,255);
      else
        color_id = volm_osm_category_io::volm_land_table[values[pidx]].color_;
        //color_id = volm_label_table::get_color(values[pidx]);
      this->draw_dot(out_img, query_points_[pidx], color_id, cam);
    }
  }
  else if (value_type == "depth") {
    for (unsigned pidx = 0; pidx < query_size_; ++pidx) {
      vil_rgb<vxl_byte> color_id;
      if (values[pidx] == 254)
        color_id = vil_rgb<vxl_byte>(51,102,255);
      else if (values[pidx] == 253)
        color_id = vil_rgb<vxl_byte>(0,0,0);
      else
        color_id = vil_rgb<vxl_byte>(bvrml_color::heatmap_classic[(int)values[pidx]][0],
                                     bvrml_color::heatmap_classic[(int)values[pidx]][1],
                                     bvrml_color::heatmap_classic[(int)values[pidx]][2]);
      this->draw_dot(out_img, query_points_[pidx], color_id, cam);
    }
  }
  else {
    std::cerr << "WARNING: given image type " << value_type << " is not found in volm_query::depth_rgb_image, generate depth image instead\n";
    for (unsigned pidx = 0; pidx < query_size_; ++pidx)
      if (values[pidx] < 255)
        this->draw_dot(out_img, query_points_[pidx], values[pidx], cam);
  }
}

void volm_query::draw_query_image(unsigned cam_i, std::string const& out_name)
{
  // create an rgb image instance
  vil_image_view<vil_rgb<vxl_byte> > img(ni_, nj_);
  // initialize the image
  for (unsigned i = 0; i < ni_; ++i)
    for (unsigned j = 0; j < nj_; ++j) {
      img(i,j).r = (unsigned char)120;
      img(i,j).g = (unsigned char)120;
      img(i,j).b = (unsigned char)120;
    }
  std::vector<unsigned char> current_query = min_dist_[cam_i];
  this->depth_rgb_image(current_query, cam_i, img);
  // save the images
  vil_save(img,out_name.c_str());
}

void volm_query::draw_query_regions(std::string const& out_name)
{
  // create an rgb image instance
  vil_image_view<vil_rgb<vxl_byte> > img(ni_, nj_);
  // initialize the image
  for (unsigned i = 0; i < ni_; ++i)
    for (unsigned j = 0; j < nj_; ++j) {
      img(i,j).r = (unsigned char)120;
      img(i,j).g = (unsigned char)120;
      img(i,j).b = (unsigned char)120;
    }
  this->draw_depth_map_regions(img);
  // save the images
  vil_save(img,out_name.c_str());
}

void volm_query::draw_query_images(std::string const& out_dir)
{
  // create a png img associated with each camera hypothesis, containing the geometry defined
  //  in depth_map_scene and the img points corresponding to points inside the container
  // loop over fist 100 camera
  unsigned img_num = (this->get_cam_num() > 20) ? 20 : (unsigned)cameras_.size();
  for (unsigned i = 0; i < img_num; ++i) {
    std::stringstream s_idx;
    s_idx << i;
    std::string fs = out_dir + "query_img_" + this->get_cam_string(i) + ".png";
    this->draw_query_image(i, fs);
  }
}

void volm_query::visualize_query(std::string const& prefix)
{
  // visualize the spherical shell by the query depth value -- used to compare with the generated index spherical shell
  // loop over all camera
  for (unsigned i = 0; i < this->get_cam_num(); ++i) {
    std::vector<unsigned char> single_layer = min_dist_[i];
    std::stringstream str;
    str << prefix << "_query_" << i << ".vrml";
    sph_->draw_template(str.str(), single_layer, 254);
  }
}

unsigned volm_query::get_num_top_fov(double const& top_fov) const
{
  unsigned count = 0;
  for (unsigned i = 0; i < this->get_cam_num(); ++i) {
    std::string cam_string = this->get_cam_string(i);
    auto sindx = (unsigned)cam_string.find("top_fov");
    sindx += 8;
    std::stringstream ss( cam_string.substr(sindx, cam_string.size()-1) );
    double top;
    ss >> top;
    if ((unsigned)top == (unsigned)top_fov) {
      ++count;
    }
  }
  return count;
}

double volm_query::get_top_fov(unsigned const& id) const
{
  std::string cam_string = this->get_cam_string(id);
  auto sindx = (unsigned)cam_string.find("top_fov");
  sindx += 8;
  std::stringstream ss( cam_string.substr(sindx, cam_string.size()-1) );
  double top;
  ss >> top;
  return top;
}

std::vector<double> volm_query::get_valid_top_fov() const
{
  std::set<double> top_fov_set;
  for (unsigned i = 0; i < this->get_cam_num(); ++i) {
    top_fov_set.insert(this->get_top_fov(i));
  }
  std::vector<double> valid_top_fov;
  valid_top_fov.reserve(top_fov_set.size());
for (const auto & it : top_fov_set)
    valid_top_fov.push_back(it);
  return valid_top_fov;
}

unsigned volm_query::get_order_size() const
{
  unsigned count = 0;
  unsigned cam_num = this->get_cam_num();
  unsigned obj_num = this->get_obj_order_num();
  for (unsigned i = 0; i < cam_num; ++i)
    for (unsigned j = 0; j < obj_num; ++j)
      count += (unsigned)order_index_[i][j].size();
  return count;
}

unsigned volm_query::obj_based_query_size_byte() const
{
  unsigned size_byte = 0;
  // ground voxel size
  size_byte += this->get_ground_id_size()*4;     // unsigned id
  size_byte += this->get_ground_dist_size();       // unsigned char distance
  size_byte += this->get_ground_id_size()*4;     // unsigned char land classfication
  size_byte += this->get_ground_id_size()*4*4; // float land fallback category weight
  size_byte += 1;                                  // unsigned char orientation
  size_byte += (unsigned)ground_offset_.size()*4;  // unsinged ground offset

  // sky voxel size
  size_byte += this->get_sky_id_size()*4;        // unsigned id
  size_byte += (unsigned)sky_offset_.size()*4;     // unsigned sky offset
  //size_byte += 4;                                // float weight

  // non-sky, non-ground object
  size_byte += this->get_dist_id_size()*4;         // unsigned id
  size_byte += (unsigned)dist_offset_.size()*4;    // unsigned offset
  size_byte += (unsigned)min_obj_dist_.size();     // unsigned char distance
  size_byte += (unsigned)max_obj_dist_.size();     // unsigned char distance
  size_byte += (unsigned)obj_orient_.size();       // unsigned char orientation
  size_byte += (unsigned)obj_land_id_.size()*4;    // unsigned char land clarification
  size_byte += (unsigned)obj_land_wgt_.size()*4*4; // float land fallback category weight
  size_byte += (unsigned)order_obj_.size();    // unsigned char order
  //size_byte += (unsigned)weight_obj_.size()*4; // float weight
  return size_byte;
}

//: binary IO write
void volm_query::write_data(vsl_b_ostream& os)
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, invalid_);
  vsl_b_write(os, d_threshold_);
  vsl_b_write(os, camera_strings_);
  vsl_b_write(os, order_set_);  // store the non-ground order, using set to ensure objects having same order are put together

  vsl_b_write(os, (unsigned)(order_index_.size()));
  for (const auto & i : order_index_)
    vsl_b_write(os, i);

  vsl_b_write(os, ground_id_);
  vsl_b_write(os, ground_dist_);

  //vsl_b_write(os, ground_land_id_);
  vsl_b_write(os, (unsigned)(ground_land_id_.size()));
  for (const auto & i : ground_land_id_)
    vsl_b_write(os, i);

  vsl_b_write(os, (unsigned)(ground_land_wgt_.size()));
  for (const auto & i : ground_land_wgt_)
    vsl_b_write(os, i);

  vsl_b_write(os, ground_offset_);
  vsl_b_write(os, ground_orient_);
  vsl_b_write(os, sky_id_);
  vsl_b_write(os, sky_offset_);
  vsl_b_write(os, sky_orient_);

  vsl_b_write(os, (unsigned)(dist_id_.size()));
  for (const auto & i : dist_id_)
    vsl_b_write(os, i);

  vsl_b_write(os, dist_offset_);
  vsl_b_write(os, min_obj_dist_);
  vsl_b_write(os, max_obj_dist_);
  vsl_b_write(os, order_obj_);
  vsl_b_write(os, obj_orient_);
  vsl_b_write(os, obj_land_id_);
  vsl_b_write(os, obj_land_wgt_);
}

//: binary IO read
void volm_query::read_data(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver ==1) {
    vsl_b_read(is, invalid_);
    vsl_b_read(is, d_threshold_);
    vsl_b_read(is, camera_strings_);
    vsl_b_read(is, order_set_);  // store the non-ground order, using set to ensure objects having same order are put together
    unsigned size;
    vsl_b_read(is, size);
    for (unsigned i = 0; i < size; i++) {
      std::vector<std::vector<unsigned> > o;
      vsl_b_read(is, o);
      order_index_.push_back(o);
    }

    vsl_b_read(is, ground_id_);
    vsl_b_read(is, ground_dist_);

    // vsl_b_read(is, ground_land_id_);
    vsl_b_read(is, size);
    for (unsigned i = 0; i < size; i++) {
      std::vector<std::vector<unsigned char> > temp;
      vsl_b_read(is, temp);
      ground_land_id_.push_back(temp);
    }

    vsl_b_read(is, size);
    for (unsigned i = 0; i < size; i++) {
      std::vector<std::vector<float> > temp;
      vsl_b_read(is, temp);
      ground_land_wgt_.push_back(temp);
    }

    vsl_b_read(is, ground_offset_);
    vsl_b_read(is, ground_orient_);
    vsl_b_read(is, sky_id_);
    vsl_b_read(is, sky_offset_);
    vsl_b_read(is, sky_orient_);

    vsl_b_read(is, size);
    for (unsigned i = 0; i < size; i++) {
      std::vector<std::vector<unsigned> > o;
      vsl_b_read(is, o);
      dist_id_.push_back(o);
    }

    vsl_b_read(is, dist_offset_);
    vsl_b_read(is, min_obj_dist_);
    vsl_b_read(is, max_obj_dist_);
    vsl_b_read(is, order_obj_);
    vsl_b_read(is, obj_orient_);
    vsl_b_read(is, obj_land_id_);
    vsl_b_read(is, obj_land_wgt_);
  }
  else {
    std::cerr << "volm_spherical_shell_container - unknown binary io version " << ver <<'\n';
    return;
  }
}

bool volm_query::operator== (const volm_query &other) const
{
  return this->get_cam_num() == other.get_cam_num() &&
  ni_ == other.ni_ && nj_ == other.nj_ &&
  log_downsample_ratio_ == other.log_downsample_ratio_ &&
  d_threshold_ == other.d_threshold_ &&
  depth_regions_.size() == other.depth_regions_.size() &&
  camera_strings_ == other.camera_strings_ &&
  order_set_ == other.order_set_ &&
  order_index_ == other.order_index_ &&
  ground_id_ == other.ground_id_ &&
  ground_dist_ == other.ground_dist_ &&
  ground_land_id_ == other.ground_land_id_ &&
  ground_offset_ == other.ground_offset_ &&
  ground_orient_ == other.ground_orient_ &&
  sky_id_ == other.sky_id_ &&
  sky_offset_ == other.sky_offset_ &&
  sky_orient_ == other.sky_orient_ &&
  dist_id_ == other.dist_id_ &&
  dist_offset_ == other.dist_offset_ &&
  min_obj_dist_ == other.min_obj_dist_ &&
  max_obj_dist_ == other.max_obj_dist_ &&
  order_obj_ == other.order_obj_ &&
  obj_orient_ == other.obj_orient_ &&
  obj_land_id_ == other.obj_land_id_;
}
