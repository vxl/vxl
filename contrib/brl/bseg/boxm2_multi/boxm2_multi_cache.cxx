#include "boxm2_multi_cache.h"
#include <vcl_sstream.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_algorithm.h>

//: init opencl cache for each device
boxm2_multi_cache::boxm2_multi_cache(boxm2_scene_sptr              scene,
                                     vcl_vector<bocl_device*> &    devices)
{
  scene_ = scene; 
  boxm2_lru_cache::create(scene);
  
  //partition the scene into smaller (continuous) scenes
  vgl_point_3d<int> min_ids, max_ids; 
  vgl_point_3d<double> min_origin, max_origin; 
  scene->min_block_index(min_ids, min_origin); 
  scene->max_block_index(max_ids, max_origin); 

  //divide by half in each direction (take ceiling)
  vgl_point_3d<int> incr_ids( (int) vcl_ceil( (float)max_ids.x()/devices.size() ), 
                              (int) vcl_ceil( (float)max_ids.y()/devices.size() ), 
                              (int) vcl_ceil( (float)max_ids.z()/devices.size() ) ); 
  
  //for each device create a new scene 
  int blocksAdded = 0; 
  for(int dev_id=0; dev_id<devices.size(); ++dev_id) {
    
    vcl_map<boxm2_block_id, boxm2_block_metadata> sub_scene_blocks; 
      
    //figure out which blocks to add to it...
    vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks(); 
    vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter; 
    for(iter=blocks.begin(); iter!=blocks.end(); ++iter) {
      
      boxm2_block_id id = iter->first; 
      int lower = dev_id * incr_ids.x(); 
      int upper = (dev_id+1) * incr_ids.x(); 
      if(id.i() >= lower && id.i() < upper) {
        boxm2_block_metadata md = iter->second; 
        sub_scene_blocks[id] = md; 
        vcl_cout<<" added: "<<id<<" to dev "<<dev_id<<vcl_endl;
        blocksAdded++; 
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
    boxm2_opencl_cache* ocl_cache = new boxm2_opencl_cache(sub_scene, devices[dev_id]); 
    ocl_caches_.push_back(ocl_cache); 
  }
  
  if( blocksAdded != scene->blocks().size() ) {
    vcl_cout<<"boxm2_multi_cache blocks added not equal to number of blocks in original scene: \n"
            <<"  Num gpus: "<<devices.size()<< '\n'
            <<"  blocks added: "<<blocksAdded<<" != "<<scene->blocks().size()<<vcl_endl;
            
    throw -1; 
  }
}

boxm2_multi_cache::~boxm2_multi_cache()
{
  //delete ocl caches
  //for(int i=0; i<ocl_caches_.size(); ++i)
  //  if(ocl_caches_[i]) delete ocl_caches_[i]; 
}

vcl_vector<boxm2_opencl_cache*> boxm2_multi_cache::get_vis_sub_scenes(vpgl_perspective_camera<double>* cam) 
{
  vcl_vector<boxm2_opencl_cache*> vis_order;
  if (!cam) {
    vcl_cout << "null camera in boxm2_scene::get_vis_blocks(.)\n";
    return vis_order;
  }
  //get camera center and order blocks distance from the cam center
  //for non-projective cameras there may not be a single center of projection
  //so instead get the ray origin farthest from the scene origin.
  vgl_point_3d<double> cam_center = cam->camera_center();
  
  //Map of distance, id
  return this->get_vis_order_from_pt(cam_center);
}

vcl_vector<boxm2_opencl_cache*> boxm2_multi_cache::get_vis_order_from_pt(vgl_point_3d<double> const& pt)
{
  //Map of distance, id
  vcl_vector<boxm2_opencl_cache*>   vis_order;
  vcl_vector<boxm2_dist_cache_pair> distances;

  //iterate through each block
  for (int idx=0; idx<ocl_caches_.size(); ++idx) {
    boxm2_opencl_cache*     cache   = ocl_caches_[idx]; 
    boxm2_scene_sptr        sscene  = cache->get_scene(); 
    vgl_box_3d<double>      bbox    = sscene->bounding_box(); 
    vgl_point_3d<double>    center  = bbox.centroid();
    double                  dist    = vgl_distance( center, pt );
    distances.push_back( boxm2_dist_cache_pair(dist, cache) );
  }

  //sort distances
  vcl_sort(distances.begin(), distances.end());

  //put blocks in "vis_order"
  vcl_vector<boxm2_dist_cache_pair>::iterator di;
  for (di = distances.begin(); di != distances.end(); ++di) {
    vis_order.push_back(di->cache_);
  }
  return vis_order;
}


vcl_string boxm2_multi_cache::to_string() 
{
  vcl_stringstream s; 
  s <<"******* boxm2_multi_cache ************************************\n"
    <<" num sub scenes: "<<sub_scenes_.size()<<'\n'; 
  for(int i=0; i<sub_scenes_.size(); ++i) {
    s << "Sub Scene "<<i<<":\n"; 
    s <<(*sub_scenes_[i])<<vcl_endl;
  }
  return s.str(); 
}


//----------------------- stream io----------------------------------------//

//: shows elements in cache
vcl_ostream& operator<<(vcl_ostream &s, boxm2_multi_cache& cache)
{
  s << cache.to_string();
  return s;
}
