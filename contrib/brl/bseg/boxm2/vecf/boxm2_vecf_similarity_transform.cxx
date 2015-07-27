#include "boxm2_vecf_similarity_transform.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>

boxm2_vecf_similarity_transform::boxm2_vecf_similarity_transform(vgl_rotation_3d<double> rot,
                                                                         vgl_vector_3d<double> trans,
                                                                         vgl_vector_3d<double> scale)
  : rot_(rot), trans_(trans), scale_(scale)
{
}

bool boxm2_vecf_similarity_transform::
compute_forward_transform(boxm2_scene_sptr source,
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
          vgl_point_3d<double> transformed_center = rot_ * cell_center;
          transformed_center.set(transformed_center.x()*scale_.x(),
                                 transformed_center.y()*scale_.y(),
                                 transformed_center.z()*scale_.z());
          transformed_center += trans_;
          vgl_vector_3d<double> v(transformed_center - cell_center);

          vec_field[data_idx] = vnl_vector_fixed<float,4>(v.x(), v.y(), v.z(), 0.0f);
        }
      }
    }
  }
  return true;
}

bool boxm2_vecf_similarity_transform::
compute_inverse_transform(boxm2_scene_sptr target,
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
          vgl_point_3d<double> inv_transformed_center = rot_.inverse() * (cell_center - trans_);
          inv_transformed_center.set(inv_transformed_center.x()/scale_.x(),
                                     inv_transformed_center.y()/scale_.y(),
                                     inv_transformed_center.z()/scale_.z());
          vgl_vector_3d<double> v(inv_transformed_center - cell_center);

          vec_field[data_idx] = vnl_vector_fixed<float,4>(v.x(), v.y(), v.z(), 0.0f);
        }
      }
    }
  }
  return true;
}
