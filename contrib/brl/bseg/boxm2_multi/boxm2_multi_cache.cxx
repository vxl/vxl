#include <sstream>
#include <iostream>
#include <algorithm>
#include "boxm2_multi_cache.h"
//:
// \file
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsph/vsph_camera_bounds.h>

//: init opencl cache for each device
boxm2_multi_cache::boxm2_multi_cache(const boxm2_scene_sptr&             scene,
                               const std::vector<bocl_device_sptr> &devices)
{
  scene_ = scene;
  boxm2_lru_cache1::create(scene);

#if  1
  unsigned int blocksAdded = 0;
  //create a sub scene for each device
  for (const auto & device : devices) {
    //create scene
    boxm2_scene_sptr sub_scene = new boxm2_scene();
    sub_scene->set_local_origin(scene->local_origin());
    sub_scene->set_xml_path(scene->xml_path());
    sub_scene->set_data_path(scene->data_path());
    sub_scenes_.push_back(sub_scene);

    //create ocl_cache for this scene...
    boxm2_opencl_cache1* ocl_cache = new boxm2_opencl_cache1(sub_scene, device);
    ocl_caches_.push_back(ocl_cache);
  }

  //get scene block bounds
  vgl_vector_3d<unsigned int> scene_dim = scene->scene_dimensions();
  vgl_point_3d<int> min_ids, max_ids;
  vgl_point_3d<double> min_origin, max_origin;
  scene->min_block_index(min_ids, min_origin);
  scene->max_block_index(max_ids, max_origin);
  std::cout<<"Min ids, max ids: "<<min_ids<<','<<max_ids<<std::endl;

  //check if scene dimensions will work
  int groupSizeX = 1, groupSizeY = 1;
  if (sub_scenes_.size() == 2) {
    if (scene_dim.y() % sub_scenes_.size() != 0) {
      std::cerr<<"  Boxm2_multi_cache::scene y dimension is not divisible by num devices\n"
              <<"  "<<scene_dim.y()<<" blocks by "<<sub_scenes_.size()<<" devices\n";
      throw -1;
    }
    groupSizeY = 2;
    groupSizeX = 1;
  }
  else if (sub_scenes_.size() == 4) {
    if (scene_dim.y() % 2 != 0 || scene_dim.x() % 2 != 0) {
      std::cerr<<"  Boxm2_multi_cache::scene x/y dimension not divisible by 2\n"
              <<"  "<<scene_dim.y()<<" blocks by "<<scene_dim.x()<<'\n';
      throw -1;
    }
    groupSizeY = 2;
    groupSizeX = 2;
  }

  //divy up the blocks
  //iterate over groups in x/y
  for (int startX=min_ids.x(); startX<max_ids.x()+1; startX+=groupSizeX) {
    for (int startY=min_ids.y(); startY<max_ids.y()+1; startY+=groupSizeY) {
      //create a block group
      auto* grp = new boxm2_multi_cache_group;
      //add the vertical row of blocks to scene with dev_id
      int dev_id = 0;
      for (int i=0; i<groupSizeX; ++i) {
        for (int j=0; j<groupSizeY; ++j) {
          for (int k=min_ids.z(); k<max_ids.z()+1; ++k) {
            boxm2_block_id id(i+startX, j+startY, k);
            std::cout<<"Attempting to add block id: "<<id<<std::endl;
            if (scene->block_exists(id)) {
              boxm2_block_metadata md = scene->get_block_metadata(id);
              grp->add_block(md, ocl_caches_[dev_id]);
              sub_scenes_[dev_id]->add_block_metadata(md);
              ++blocksAdded;
            }
          }
          dev_id = (dev_id+1) % ocl_caches_.size();
        }
      }
      groups_.push_back(grp);
    }
  }

  for (unsigned int i=0; i<groups_.size(); ++i)
    std::cout<<"Group "<<i<<": "<<groups_[i]<<std::endl;
#endif
#if 0
  //partition the scene into smaller (continuous) scenes
  vgl_point_3d<int> min_ids, max_ids;
  vgl_point_3d<double> min_origin, max_origin;
  scene->min_block_index(min_ids, min_origin);
  scene->max_block_index(max_ids, max_origin);

  //divide by half in each direction (take ceiling)
  vgl_point_3d<int> incr_ids( (int) std::ceil( (float)(max_ids.x()+1)/devices.size() ),
                              (int) std::ceil( (float)(max_ids.y()+1)/devices.size() ),
                              (int) std::ceil( (float)(max_ids.z()+1)/devices.size() ) );

  //for each device create a new scene
  unsigned int blocksAdded = 0;
  for (unsigned int dev_id=0; dev_id<devices.size(); ++dev_id)
  {
    std::map<boxm2_block_id, boxm2_block_metadata> sub_scene_blocks;

    //figure out which blocks to add to it...
    std::map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
    std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
    for (iter=blocks.begin(); iter!=blocks.end(); ++iter)
    {
      boxm2_block_id id = iter->first;
      int lower = dev_id * incr_ids.x();
      int upper = (dev_id+1) * incr_ids.x();
      if (id.i() >= lower && id.i() < upper) {
        boxm2_block_metadata md = iter->second;
        sub_scene_blocks[id] = md;
        std::cout<<" added: "<<id<<" to dev "<<dev_id<<std::endl;
        ++blocksAdded;
      }
    }

    //create scene
    boxm2_scene_sptr sub_scene = new boxm2_scene();
    sub_scene->set_local_origin(scene->local_origin());
    sub_scene->set_xml_path(scene->xml_path());
    sub_scene->set_data_path(scene->data_path());
    sub_scene->set_blocks(sub_scene_blocks);
    sub_scenes_.push_back(sub_scene);

    //create ocl_cache for this scene...
    boxm2_opencl_cache1* ocl_cache = new boxm2_opencl_cache1(sub_scene, devices[dev_id]);
    ocl_caches_.push_back(ocl_cache);
  }
#endif
  if ( blocksAdded != scene->blocks().size() ) {
    std::cout<<"boxm2_multi_cache blocks added not equal to number of blocks in original scene:\n"
            <<"  Num gpus: "<<devices.size()<< '\n'
            <<"  blocks added: "<<blocksAdded<<" != "<<scene->blocks().size()<<std::endl;
    throw -1;
  }
}

boxm2_multi_cache::~boxm2_multi_cache()
{
#if 0
  //delete ocl caches
  for (unsigned int i=0; i<ocl_caches_.size(); ++i)
    delete ocl_caches_[i];
#endif
  for (auto & group : groups_)
    if (group) delete group;
}

std::vector<boxm2_opencl_cache1*> boxm2_multi_cache::get_vis_sub_scenes(vpgl_perspective_camera<double>* cam)
{
  std::vector<boxm2_opencl_cache1*> vis_order;
  if (!cam) {
    std::cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }
  //get camera center and order blocks distance from the cam center
  //for non-projective cameras there may not be a single center of projection
  //so instead get the ray origin farthest from the scene origin.
  vgl_point_3d<double> cam_center = cam->camera_center();

  //Map of distance, id
  return this->get_vis_order_from_pt(cam_center);
}

std::vector<boxm2_opencl_cache1*>
boxm2_multi_cache::get_vis_sub_scenes(vpgl_generic_camera<double>* cam)
{
  vgl_point_3d<double> cam_center = cam->max_ray_origin();
  return this->get_vis_order_from_pt(cam_center);
}

std::vector<boxm2_opencl_cache1*>
boxm2_multi_cache::get_vis_sub_scenes(const vpgl_camera_double_sptr& cam)
{
  if ( cam->type_name() == "vpgl_generic_camera" )
    return this->get_vis_sub_scenes( (vpgl_generic_camera<double>*) cam.ptr() );
  else if ( cam->type_name() == "vpgl_perspective_camera" )
    return this->get_vis_sub_scenes( (vpgl_perspective_camera<double>*) cam.ptr() );
  else
    std::cout<<"boxm2_scene::get_vis_blocks doesn't support camera type "<<cam->type_name()<<std::endl;

  //else return empty
  std::vector<boxm2_opencl_cache1*> empty;
  return empty;
}

std::vector<boxm2_opencl_cache1*>
boxm2_multi_cache::get_vis_order_from_pt(vgl_point_3d<double> const& pt)
{
  //Map of distance, id
  typedef boxm2_dist_pair<boxm2_opencl_cache1*> Pair;
  std::vector<Pair> distances;

  //iterate through each block
  for (auto cache : ocl_caches_) {
    boxm2_scene_sptr        sscene  = cache->get_scene();
    vgl_box_3d<double>      bbox    = sscene->bounding_box();
    vgl_point_3d<double>    center  = bbox.centroid();
    double                  dist    = vgl_distance( center, pt );
    distances.emplace_back(dist,cache);
  }

  //sort distances
  std::sort(distances.begin(), distances.end());

  //put blocks in "vis_order"
  std::vector<boxm2_opencl_cache1*>   vis_order;
  std::vector<Pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->dat_);
  return vis_order;
}

//----------------------------------------------
// Cache block group visibility order functions
//----------------------------------------------
std::vector<boxm2_multi_cache_group*>
boxm2_multi_cache::get_vis_groups(const vpgl_camera_double_sptr& cam)
{
  vgl_point_3d<double> center;
  vgl_box_2d<double> camBox;
  if ( cam->type_name() == "vpgl_generic_camera" ) {
    auto* gcam = (vpgl_generic_camera<double>*) cam.ptr();
    center = gcam->max_ray_origin();
  }
  else if ( cam->type_name() == "vpgl_perspective_camera" ) {
    auto* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
    center = pcam->camera_center();
    //find intersection box
    vgl_box_3d<double> sceneBB = scene_->bounding_box();
    vgl_box_2d<double> lowBox, highBox;
    vsph_camera_bounds::planar_bounding_box(*pcam, lowBox, sceneBB.min_z());
    vsph_camera_bounds::planar_bounding_box(*pcam, highBox, sceneBB.max_z());
    camBox.add(lowBox);
    camBox.add(highBox);
  }
  else {
    std::cout<<"boxm2_scene::get_vis_blocks doesn't support camera type "<<cam->type_name()<<std::endl;
    return std::vector<boxm2_multi_cache_group*>();
  }
  return this->group_order_from_pt(center, camBox);
}

std::vector<boxm2_multi_cache_group*>
boxm2_multi_cache::group_order_from_pt(vgl_point_3d<double> const& pt,
                                       vgl_box_2d<double> const& camBox)
{
  //Map of distance, id
  typedef boxm2_dist_pair<boxm2_multi_cache_group*> Pair;
  std::vector<Pair> distances;

  //iterate through each group
  for (auto grp : groups_) {
    //check if cam bbox intersectsa
    vgl_box_3d<double>& grpBox = grp->groupBox();
    vgl_box_2d<double> grp2d(grpBox.min_x(), grpBox.max_x(),
                             grpBox.min_y(), grpBox.max_y());
    vgl_box_2d<double> intersect = vgl_intersection(grp2d, camBox);
    //if (!intersect.is_empty() || camBox.is_empty()) {
      vgl_point_3d<double> center  = grpBox.centroid();
      double dist = vgl_distance( center, pt );
      distances.emplace_back(dist, grp );
    //}
  }

  //sort distances
  std::sort(distances.begin(), distances.end());

  //put blocks in "vis_order"
  std::vector<boxm2_multi_cache_group*>   vis_order;
  std::vector<Pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di)
    vis_order.push_back(di->dat_);
  return vis_order;
}

std::string boxm2_multi_cache::to_string()
{
  std::stringstream s;
  s <<"******* boxm2_multi_cache ************************************\n"
    <<" num sub scenes: "<<sub_scenes_.size()<<'\n';
  for (unsigned int i=0; i<sub_scenes_.size(); ++i) {
    s << "Sub Scene "<<i<<":\n"
      <<(*sub_scenes_[i])<<std::endl;
  }
  return s.str();
}

void boxm2_multi_cache::clear()
{
  for (auto & ocl_cache : ocl_caches_)
    ocl_cache->clear_cache();
}

//----------------------- stream io----------------------------------------//

//: shows elements in cache
std::ostream& operator<<(std::ostream &s, boxm2_multi_cache& cache)
{
  s << cache.to_string();
  return s;
}
