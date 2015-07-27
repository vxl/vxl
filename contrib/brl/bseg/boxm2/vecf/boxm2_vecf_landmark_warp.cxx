#include "boxm2_vecf_landmark_warp.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_stdexcept.h>
#include <vcl_utility.h> // for make_pair


boxm2_vecf_landmark_warp::boxm2_vecf_landmark_warp(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                                                   vcl_vector<vgl_point_3d<double> > const& control_pts_target)
{
  if (control_pts_source.size() != control_pts_target.size()) {
    throw vcl_runtime_error("number of source and target control points should be the same.");
  }
  for (vcl_vector<vgl_point_3d<double> >::const_iterator source_it = control_pts_source.begin(),
       target_it = control_pts_target.begin();
       source_it != control_pts_source.end(); ++source_it, ++target_it)
  {
    control_pts_.push_back(vcl_make_pair(*source_it, *target_it));
  }
}

//: write vector field to source's BOXM2_VEC3D data
bool boxm2_vecf_landmark_warp::compute_forward_transform(boxm2_scene_sptr source,
                                       boxm2_block_id const& blk_id,
                                       boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field)
{
  // get block data
  boxm2_block *blk = boxm2_cache::instance()->get_block(source, blk_id);
  vgl_vector_3d<double> subblk_dims = blk->sub_block_dim();

  //get the 3d array of trees
  typedef boxm2_block::uchar16 uchar16;
  const boxm2_array_3d<uchar16>& trees = blk->trees();
  //iterate through each block, filtering the root level first
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
      for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
        vgl_vector_3d<double> subblk_offset(x*subblk_dims.x(),
                                            y*subblk_dims.y(),
                                            z*subblk_dims.z());
        //load current tree
        uchar16 tree = trees(x, y, z);
        boct_bit_tree bit_tree((unsigned char*) tree.data_block(), blk->max_level());
        // for all leaves in current tree
        vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
        vcl_vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
          int bit_idx = (*iter);
          int data_idx = bit_tree.get_data_index(bit_idx);
          // get the cell center
          vgl_point_3d<double> cell_center_norm = bit_tree.cell_center( bit_idx );
          // convert to global coordinates
          vgl_vector_3d<double> cell_offset(cell_center_norm.x()*subblk_dims.x(),
                                            cell_center_norm.y()*subblk_dims.y(),
                                            cell_center_norm.z()*subblk_dims.z());

          vgl_point_3d<double> cell_center = blk->local_origin() + subblk_offset + cell_offset;
          // compute weighted sum of control points
          const double epsilon = subblk_dims.x() / 10.0;
          vgl_vector_3d<double> vec(0.0, 0.0, 0.0);
          double weight_sum = 0.0;
          for (vcl_vector<control_pt_t>::const_iterator pit=control_pts_.begin();
               pit != control_pts_.end(); ++pit) {
            double weight = 1.0 / ((pit->first - cell_center).sqr_length() + epsilon);
            weight_sum += weight;
            vec += (pit->second - pit->first);
          }
          vec_field[data_idx] = vnl_vector_fixed<float,4>(vec.x() / weight_sum,
                                                          vec.y() / weight_sum,
                                                          vec.z() / weight_sum,
                                                          0.0f);
        }
      }
    }
  }
  return true;
}

//: write vector field to target's BOXM2_VEC3D data
bool boxm2_vecf_landmark_warp::compute_inverse_transform(boxm2_scene_sptr target,
                                       boxm2_block_id const& blk_id,
                                       boxm2_data_traits<BOXM2_VEC3D>::datatype *vec_field)
{
  // get block data
  boxm2_block *blk = boxm2_cache::instance()->get_block(target, blk_id);
  vgl_vector_3d<double> subblk_dims = blk->sub_block_dim();

  //get the 3d array of trees
  typedef boxm2_block::uchar16 uchar16;
  const boxm2_array_3d<uchar16>& trees = blk->trees();
  //iterate through each block, filtering the root level first
  for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
    for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
      for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
        vgl_vector_3d<double> subblk_offset(x*subblk_dims.x(),
                                            y*subblk_dims.y(),
                                            z*subblk_dims.z());
        //load current tree
        uchar16 tree = trees(x, y, z);
        boct_bit_tree bit_tree((unsigned char*) tree.data_block(), blk->max_level());
        // for all leaves in current tree
        vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
        vcl_vector<int>::iterator iter;
        for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
          int bit_idx = (*iter);
          int data_idx = bit_tree.get_data_index(bit_idx);
          // get the cell center
          vgl_point_3d<double> cell_center_norm = bit_tree.cell_center( bit_idx );
          // convert to global coordinates
          vgl_vector_3d<double> cell_offset(cell_center_norm.x()*subblk_dims.x(),
                                            cell_center_norm.y()*subblk_dims.y(),
                                            cell_center_norm.z()*subblk_dims.z());

          vgl_point_3d<double> cell_center = blk->local_origin() + subblk_offset + cell_offset;
          // compute weighted sum of control points
          const double epsilon = subblk_dims.x() / 10.0;
          vgl_vector_3d<double> vec(0.0, 0.0, 0.0);
          double weight_sum = 0.0;
          for (vcl_vector<control_pt_t>::const_iterator pit=control_pts_.begin();
               pit != control_pts_.end(); ++pit) {
            double weight = 1.0 / ((pit->second - cell_center).sqr_length() + epsilon);
            weight_sum += weight;
            vec += (pit->first - pit->second);
          }
          vec_field[data_idx] = vnl_vector_fixed<float,4>(vec.x() / weight_sum,
                                                          vec.y() / weight_sum,
                                                          vec.z() / weight_sum,
                                                          0.0f);
        }
      }
    }
  }
  return true;
}
