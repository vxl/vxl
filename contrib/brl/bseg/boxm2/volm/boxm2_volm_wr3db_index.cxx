#include "boxm2_volm_wr3db_index.h"
//:
// \file
#include <bbas/volm/volm_spherical_container.h>
#include <boxm2/volm/boxm2_volm_locations.h>
#include <vgl/vgl_box_3d.h>


boxm2_volm_wr3db_index::boxm2_volm_wr3db_index(volm_spherical_container_sptr cont)
:      cont_(cont), voxels_size_((unsigned int)cont->get_voxels().size())
{
  // get the offset and depth of the first layer with vmin*2 resolution
  double depth;
  cont_->first_res(cont_->min_voxel_res()*2, offset_, end_offset_, depth);
}

boxm2_volm_wr3db_index::~boxm2_volm_wr3db_index()
{
  index_.clear();
}

//: create the index for a given location in the scene
//  for each location, keep a vector of values for the first layer of the container with vmin*2 resolution voxels
bool boxm2_volm_wr3db_index::index_location(boxm2_scene_sptr scene, vgl_point_3d<float>& loc, vcl_vector<unsigned char>& values)
{
  values.clear(); 
  values.resize(end_offset_-offset_);
  vcl_map<double, unsigned char>& depth_interval_map = cont_->get_depth_interval_map();

  // compute a depth value by shooting a ray to voxels given by offset_ and end_offset_ (the first layer with min_res*2 resolution in the container)
  //   from current location
  ///
  //////////////
  vgl_box_3d<double> bbox = scene->bounding_box();
  double dist = bbox.width();
  //: find the interval for this depth
  vcl_map<double, unsigned char>::iterator iter = depth_interval_map.lower_bound(dist);
  
  // for now put the same everywhere
  for (unsigned i = 0; i < values.size(); i++)
    values[i] = iter->second;

  ////////////

  return true;
}

//: create a condensed index for given locations of a scene and write as binary, use the vmin layer of spherical container
bool boxm2_volm_wr3db_index::index_locations(boxm2_scene_sptr scene, boxm2_volm_loc_hypotheses_sptr h)
{
  for (unsigned i = 0; i < h->locs_.size(); i++) {
    vcl_vector<unsigned char> values;
    if (!index_location(scene, h->locs_[i], values))
      return false;
    index_.push_back(values);
  }
  return false;
}

bool boxm2_volm_wr3db_index::write_index(vcl_string out_file)
{
  vsl_b_ofstream os(out_file.c_str());
  if (!os)
    return false;
  if (!index_.size()) {
    vcl_cerr << "In boxm2_volm_wr3db_index::write_index() -- index size is zero, cannot write!\n";
    return false;
  }
  vsl_b_write(os, index_.size());
  vsl_b_write(os, index_[0].size());
  for (unsigned i = 0; i < index_.size(); i++) {
    for (unsigned k = 0; k < index_[i].size(); k++) {
      vsl_b_write(os, index_[i][k]);
    }
  }
  os.close();
  return true;
}

bool boxm2_volm_wr3db_index::read_index(vcl_string in_file)
{
  vsl_b_ifstream ifs(in_file.c_str());
  if (!ifs)
    return false;

  unsigned int size;
  unsigned int layer_size;
  vsl_b_read(ifs, size);
  vsl_b_read(ifs, layer_size);
  index_.resize(size);
  for (unsigned i = 0; i < size; i++) {
    index_[i].resize(layer_size);
    for (unsigned k = 0; k < layer_size; k++) {
      char val;
      vsl_b_read(ifs, val);
      index_[i][k] = val;
    }
  }
  ifs.close();
  return true;
}


//: inflate the index for ith location and return a vector of char values where last bit is visibility and second to last is prob (occupied or not)
// (Not yet implemented)
bool boxm2_volm_wr3db_index::inflate_index_vis_and_prob(unsigned hyp_ind, vcl_vector<unsigned char>& vis_prob)
{
  // get the voxel on the indexed layer for a given voxel
  vcl_vector<volm_voxel>& voxels = cont_->get_voxels();
  vcl_map<double, unsigned int>& depth_offset_map = cont_->get_depth_offset_map();
  vcl_map<double, unsigned int>::iterator iter = depth_offset_map.begin();
  vgl_point_3d<double> origin(0,0,0);
  unsigned char current_depth_interval = 0; // to count the depth intervals

  while (iter != depth_offset_map.end()) {
    double depth = iter->first;
    unsigned int begin = iter->second;
    iter++;
    unsigned int end; 
    if (iter == depth_offset_map.end())
      end = (unsigned int)voxels.size();
    else 
      end = iter->second;
    
    // if at the indexed layer use itself to compute vis and prob
    if (begin == offset_) {
      for (unsigned ii = begin; ii < end; ii++) { 
        unsigned char observed_depth_interval = index_[hyp_ind][ii-begin]; // depth of the voxel as observed at this voxel during indexing
        if (observed_depth_interval < current_depth_interval) 
          vis_prob[ii] = (unsigned char)NONVIS_UNKNOWN;   // not visible 
        else if (observed_depth_interval == current_depth_interval) // same depth interval
          vis_prob[ii] = (unsigned char)VIS_OCC;
        else   // visible but not yet occupied
          vis_prob[ii] = (unsigned char)VIS_UNOCC;
      }
      
    } else {
      for (unsigned ii = begin; ii < end; ii++) { 
        
        // first find the voxel at the indexed layer that is closest 
        unsigned int closest = 0;
        double theta_closest = vnl_math::pi;
        double dist_closest = 1000000.0;
        
        vgl_vector_3d<double> dir2 = voxels[ii].center_-origin;
        vgl_vector_3d<double> dir2n = dir2;
        normalize(dir2n);
        for (unsigned jj = offset_; jj < end_offset_; jj++) {
          vgl_vector_3d<double> dir1 = voxels[jj].center_-origin;
          vgl_vector_3d<double> dif = dir1-dir2;
          double dist = dif.length();
          normalize(dir1);
          double theta = vcl_acos(dot_product(dir1, dir2n));
          if (theta < theta_closest && dist < dist_closest) {
            closest = jj;
            theta_closest = theta;
            dist_closest = dist;
          }
        }
        
        unsigned char observed_depth_interval = index_[hyp_ind][closest-offset_]; // depth of the voxel as observed at this voxel during indexing
        if (observed_depth_interval < current_depth_interval) 
          vis_prob[ii] = NONVIS_UNKNOWN;   // not visible 
        else if (observed_depth_interval == current_depth_interval) // same depth interval
          vis_prob[ii] = VIS_OCC;
        else   // visible but not yet occupied
          vis_prob[ii] = VIS_UNOCC;
        
      }
    }
    current_depth_interval++;
  }
  
  return true;
}


