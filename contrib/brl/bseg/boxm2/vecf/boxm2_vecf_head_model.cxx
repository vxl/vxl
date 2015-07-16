#include "boxm2_vecf_head_model.h"
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_sphere_3d.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vcl_algorithm.h>
#include <vcl_limits.h>

boxm2_vecf_head_model::boxm2_vecf_head_model(vcl_string const& scene_file,vcl_string color_apm_ident):
  boxm2_vecf_articulated_scene(scene_file,color_apm_ident),
  scale_(1.0, 1.0, 1.0)
{

}

void boxm2_vecf_head_model::set_scale(vgl_vector_3d<double> scale) {
  if( (scale_-scale).length() >0.0001 ){
    scale_ = scale;
    intrinsic_change_ = true;
  }

}
bool boxm2_vecf_head_model::get_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                                     boxm2_data_base** alpha_data,
                                     boxm2_data_base** app_data,
                                     boxm2_data_base** nobs_data)
{
  *alpha_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                       boxm2_data_traits<BOXM2_ALPHA>::prefix());

  *app_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                     boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());

  *nobs_data = boxm2_cache::instance()->get_data_base(scene, blk_id,
                                                      boxm2_data_traits<BOXM2_NUM_OBS>::prefix());

  return true;
}

  bool get_color_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                      vcl_string app_id, boxm2_data_base **color_data){
    *color_data = boxm2_cache::instance()->get_data_base(scene, blk_id, app_id);
    if (color_data)
      return true;
    return false;
  }

void boxm2_vecf_head_model::map_to_target(boxm2_scene_sptr target_scene)
{
  intrinsic_change_ = false;
  // for each block of the target scene
  vcl_vector<boxm2_block_id> target_blocks = target_scene->get_block_ids();
  vcl_vector<boxm2_block_id> source_blocks = base_model_->get_block_ids();

  for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin();
       tblk != target_blocks.end(); ++tblk) {

    boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene, *tblk);

    boxm2_data_base *target_alpha;
    boxm2_data_base *target_app;
    boxm2_data_base *target_nobs;
    boxm2_data_base *target_color = boxm2_cache::instance()->get_data_base(target_scene, *tblk, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
    if (!get_data(target_scene, *tblk, &target_alpha, &target_app, &target_nobs)) {
      vcl_cerr << "ERROR: boxm2_vecf_head_model::map_to_target(): error getting target block data block=" << tblk->to_string() << vcl_endl;
      return;
    }
    target_alpha->enable_write();
    target_app->enable_write();
    target_nobs->enable_write();

    //3d array of trees
    typedef boxm2_block::uchar16 uchar16;
    const boxm2_array_3d<uchar16>& trees = target_blk->trees();
    boxm2_data_traits<BOXM2_ALPHA>::datatype *target_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(target_alpha->data_buffer());
    boxm2_data_traits<BOXM2_MOG3_GREY>::datatype *target_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(target_app->data_buffer());
    boxm2_data_traits<BOXM2_NUM_OBS>::datatype *target_nobs_data = reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(target_nobs->data_buffer());
    boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype *target_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(target_color->data_buffer());
    // for each block of the base model
    for (vcl_vector<boxm2_block_id>::iterator sblk = source_blocks.begin();
         sblk != source_blocks.end(); ++sblk) {

      boxm2_block *source_blk = boxm2_cache::instance()->get_block(base_model_, *sblk);

      boxm2_data_base* source_alpha;
      boxm2_data_base* source_app;
      boxm2_data_base* source_nobs;

      boxm2_data_base* source_color = boxm2_cache::instance()->get_data_base(base_model_, *sblk, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
      if (!get_data(base_model_, *sblk, &source_alpha, &source_app, &source_nobs)) {
        vcl_cerr << "ERROR: boxm2_vecf_head_model::map_to_source(): error getting source block data block=" << sblk->to_string() << vcl_endl;
        return ;
      }
      boxm2_data_traits<BOXM2_ALPHA>::datatype *source_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(source_alpha->data_buffer());
      boxm2_data_traits<BOXM2_MOG3_GREY>::datatype *source_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(source_app->data_buffer());
      boxm2_data_traits<BOXM2_NUM_OBS>::datatype *source_nobs_data = reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(source_nobs->data_buffer());
      boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype *source_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(source_color->data_buffer());

      //iterate through each block, filtering the root level first
      for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
        for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
          for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
            //load current tree
            uchar16 tree = trees(x, y, z);
            boct_bit_tree bit_tree((unsigned char*) tree.data_block(), target_blk->max_level());

            //FOR ALL LEAVES IN CURRENT TREE
            vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
            vcl_vector<int>::iterator iter;
            for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
              int bit_idx = (*iter);
              int data_idx = bit_tree.get_data_index(bit_idx);

              vgl_point_3d<double> cell_center_norm = bit_tree.cell_center( bit_idx );
              // convert to global coordinates
              vgl_vector_3d<double> subblk_dims = target_blk->sub_block_dim();
              vgl_vector_3d<double> subblk_offset(x*subblk_dims.x(),
                                                  y*subblk_dims.y(),
                                                  z*subblk_dims.z());

              vgl_vector_3d<double> cell_offset(cell_center_norm.x()*subblk_dims.x(),
                                                cell_center_norm.y()*subblk_dims.y(),
                                                cell_center_norm.z()*subblk_dims.z());

              vgl_point_3d<double> cell_center = target_blk->local_origin() + subblk_offset + cell_offset;

              // apply (inverse) scaling
              vgl_point_3d<double> inv_scaled_cell_center(cell_center.x() / scale_.x(),
                                                          cell_center.y() / scale_.y(),
                                                          cell_center.z() / scale_.z());

              // retreive the correct cell from the source data
              vgl_point_3d<double> local_tree_coords, source_cell_center;
              double side_len;
              if ( source_blk->contains( inv_scaled_cell_center, local_tree_coords, source_cell_center, side_len )) {
                unsigned source_data_idx;
                source_blk->data_index( inv_scaled_cell_center, source_data_idx);
                float alpha = source_alpha_data[source_data_idx];
                float prob = 1 - vcl_exp(-alpha*side_len);
                const float prob_thresh = 0.0;
                if (prob > prob_thresh) {
                  // get data can copy from source to target
                  target_alpha_data[data_idx] = source_alpha_data[source_data_idx];
                  target_app_data[data_idx] = source_app_data[source_data_idx];
                  target_nobs_data[data_idx] = source_nobs_data[source_data_idx];
                  target_color_data[data_idx] =source_color_data[source_data_idx];
                }
              }
            }
          }
        }
      }
    } // for each source block
  } // for each target block
}

bool boxm2_vecf_head_model::clear_target(boxm2_scene_sptr target_scene)
{
  // for each block of the target scene
  vcl_vector<boxm2_block_id> target_blocks = target_scene->get_block_ids();

  for (vcl_vector<boxm2_block_id>::iterator tblk = target_blocks.begin();
       tblk != target_blocks.end(); ++tblk) {

    boxm2_block *target_blk = boxm2_cache::instance()->get_block(target_scene, *tblk);

    boxm2_data_base *target_alpha;
    boxm2_data_base *target_app;
    boxm2_data_base *target_nobs;
    boxm2_data_base * target_color = boxm2_cache::instance()->get_data_base(target_scene, *tblk, boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix(color_apm_id_));
    if (!get_data(target_scene, *tblk, &target_alpha, &target_app, &target_nobs)) {
      vcl_cerr << "ERROR: boxm2_vecf_head_model::map_to_target(): error getting target block data block=" << tblk->to_string() << vcl_endl;
      return false;
    }
    target_alpha->enable_write();
    target_app->enable_write();
    target_nobs->enable_write();

    //3d array of trees
    typedef boxm2_block::uchar16 uchar16;
    const boxm2_array_3d<uchar16>& trees = target_blk->trees();
    boxm2_data_traits<BOXM2_ALPHA>::datatype *target_alpha_data = reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(target_alpha->data_buffer());
    boxm2_data_traits<BOXM2_MOG3_GREY>::datatype *target_app_data = reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(target_app->data_buffer());
    boxm2_data_traits<BOXM2_NUM_OBS>::datatype *target_nobs_data = reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(target_nobs->data_buffer());
    boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype *target_color_data = reinterpret_cast<boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype*>(target_color->data_buffer());
    //iterate through each block, filtering the root level first
    for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
      for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
        for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
          //load current tree
          uchar16 tree = trees(x, y, z);
          boct_bit_tree bit_tree((unsigned char*) tree.data_block(), target_blk->max_level());

          //FOR ALL LEAVES IN CURRENT TREE
          vcl_vector<int> leafBits = bit_tree.get_leaf_bits();
          vcl_vector<int>::iterator iter;
          for (iter = leafBits.begin(); iter != leafBits.end(); ++iter) {
            int bit_idx = (*iter);
            int data_idx = bit_tree.get_data_index(bit_idx);

            // get data can copy from source to target
            target_alpha_data[data_idx] = 0.0f;
            target_app_data[data_idx] = boxm2_data_traits<BOXM2_MOG3_GREY>::datatype();
            target_nobs_data[data_idx] = boxm2_data_traits<BOXM2_NUM_OBS>::datatype();
            target_color_data[data_idx] =boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype();
          }
        }
      }
    } // for each source block
  } // for each target block
  return true;
}
