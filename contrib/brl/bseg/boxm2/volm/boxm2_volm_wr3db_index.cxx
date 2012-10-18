#include "boxm2_volm_wr3db_index.h"
//:
// \file
#include <bbas/volm/volm_spherical_container.h>
#include <boxm2/volm/boxm2_volm_locations.h>


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
bool boxm2_volm_wr3db_index::index_location(boxm2_scene_sptr scene, vgl_point_3d<float>& loc, vcl_vector<char>& values)
{
  values.clear(); values.resize(end_offset_-offset_);

  // compute a depth value by shooting a ray to voxels given by offset_ and end_offset_ (the first layer with min_res*2 resolution in the container)
  //   from current location
  ///
  //////////////

  // for now put '0'
  for (unsigned i = 0; i < values.size(); i++)
    values[i] = '0';

  ////////////

  return true;
}

//: create a condensed index for given locations of a scene and write as binary, use the vmin layer of spherical container
bool boxm2_volm_wr3db_index::index_locations(boxm2_scene_sptr scene, boxm2_volm_loc_hypotheses_sptr h)
{
  for (unsigned i = 0; i < h->locs_.size(); i++) {
    vcl_vector<char> values;
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
bool inflate_index_vis_and_prob(unsigned i, vcl_vector<char>& vis_prob)
{
  return true;
}


