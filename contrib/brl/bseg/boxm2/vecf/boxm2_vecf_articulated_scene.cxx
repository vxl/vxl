#include <cmath>
#include "boxm2_vecf_articulated_scene.h"
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/cpp/algo/boxm2_refine_block_multi_data.h>
#include <boxm2/cpp/algo/boxm2_surface_distance_refine.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_3d.h>
double boxm2_vecf_articulated_scene::gauss(double d, double sigma){
  return std::exp((-0.5*d*d)/(sigma*sigma));
}

void boxm2_vecf_articulated_scene::clear_target(boxm2_scene_sptr target_scene){
  std::string gray_APM_prefix  = boxm2_data_traits<BOXM2_MOG3_GREY>::prefix();
  std::string color_APM_prefix = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix();
  std::string alpha_prefix     = boxm2_data_traits<BOXM2_ALPHA>::prefix();

  std::vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  boxm2_data_base* alpha_db     = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], alpha_prefix);
  boxm2_data_base* gray_app_db  = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], gray_APM_prefix);
  boxm2_data_base* color_app_db = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], color_APM_prefix + "_" + (this->color_apm_id_));
  boxm2_block_metadata m_data = target_scene->get_block_metadata(blocks[0]);
  gray_app_db->set_default_value(gray_APM_prefix,   m_data);
  color_app_db->set_default_value(color_APM_prefix, m_data);
  alpha_db->set_default_value(alpha_prefix, m_data);

}
  double boxm2_vecf_articulated_scene::interp_generic_double(vgl_point_3d<double>* neighbors, const double* probs, vgl_point_3d<double> p ){

  double dx00 = LERP(probs[0],probs[2],  p.x(),neighbors[0].x(),neighbors[2].x()); // interp   between (x0,y0,z0) and (x1,y0,z0)
  double dx10 = LERP(probs[1],probs[3],  p.x(),neighbors[1].x(),neighbors[3].x()); // interp   between (x0,y1,z0) and (x1,y1,z0)
  double dx01 = LERP(probs[4],probs[6],  p.x(),neighbors[4].x(),neighbors[6].x()); // interp   between (x0,y0,z1) and (x0,y0,z1)
  double dx11 = LERP(probs[5],probs[7],  p.x(),neighbors[5].x(),neighbors[7].x()); // interp x between x-1 and x+1 z = 1 y =1


  double dxy0 = LERP( dx00, dx10,p.y(),neighbors[0].y(),neighbors[1].y());
  double dxy1 = LERP( dx01, dx11,p.y(),neighbors[0].y(),neighbors[1].y());
  double dxyz = LERP( dxy0, dxy1,p.z(),neighbors[0].z(),neighbors[4].z());
  return dxyz;

}

 double8 boxm2_vecf_articulated_scene::interp_generic_double8(vgl_point_3d<double>* neighbors, double8* probs, vgl_point_3d<double> p ){

  double8 dx00 = LERP(probs[0],probs[2],  p.x(),neighbors[0].x(),neighbors[2].x()); // interp   between (x0,y0,z0) and (x1,y0,z0)
  double8 dx10 = LERP(probs[1],probs[3],  p.x(),neighbors[1].x(),neighbors[3].x()); // interp   between (x0,y1,z0) and (x1,y1,z0)
  double8 dx01 = LERP(probs[4],probs[6],  p.x(),neighbors[4].x(),neighbors[6].x()); // interp   between (x0,y0,z1) and (x0,y0,z1)
  double8 dx11 = LERP(probs[5],probs[7],  p.x(),neighbors[5].x(),neighbors[7].x()); // interp x between x-1 and x+1 z = 1 y =1


  double8 dxy0 = LERP( dx00, dx10,p.y(),neighbors[0].y(),neighbors[1].y());
  double8 dxy1 = LERP( dx01, dx11,p.y(),neighbors[0].y(),neighbors[1].y());
  double8 dxyz = LERP( dxy0, dxy1,p.z(),neighbors[0].z(),neighbors[4].z());

  return dxyz;

}
void boxm2_vecf_articulated_scene::extract_source_block_data(){

  std::vector<boxm2_block_id> blocks = base_model_->get_block_ids();
  auto iter_blk = blocks.begin();
  blk_id_ = *iter_blk;
  blk_ = boxm2_cache::instance()->get_block(base_model_, blk_id_);
  if(!blk_){
    std::cout << "FATAL! - NULL source block for id " << blk_id_ << '\n';
    return;
  }
  // get block metadata items
  boxm2_block_metadata& metad = base_model_->get_block_metadata(blk_id_);
  double p_init = metad.p_init_;
  //the default alpha value
  alpha_init_ = static_cast<float>(-std::log(1.0 - p_init));

  source_bb_ = blk_->bounding_box_global();
  n_= blk_->sub_block_num();
  dims_= blk_->sub_block_dim();
  origin_ = blk_->local_origin();
  std::cout << "Extracting from block with " << blk_->num_cells() << " cells\n";
  sigma_ = static_cast<float>(dims_.x());
  // somewhat dangerous but as long as trees_ is treated as read only we are safe
  //trees_ = const_cast<boxm2_array_3d<uchar16>&>(blk_->trees());
  trees_ = blk_->trees_copy();
  alpha_base_  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base_->enable_write();
  alpha_data_= reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(alpha_base_->data_buffer());

  app_base_  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  app_base_->enable_write();
  app_data_= reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(app_base_->data_buffer());

  nobs_base_  = boxm2_cache::instance()->get_data_base(base_model_,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  nobs_base_->enable_write();
  nobs_data_=reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(nobs_base_->data_buffer());
}

void boxm2_vecf_articulated_scene::extract_target_block_data(boxm2_scene_sptr target_scene){
  std::vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  auto iter_blk = blocks.begin();
  target_blk_ = boxm2_cache::instance()->get_block(target_scene, *iter_blk);
  targ_n_= target_blk_->sub_block_num();
  targ_dims_= target_blk_->sub_block_dim();
  targ_origin_ = target_blk_->local_origin();
  target_alpha_base_  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  target_alpha_base_->enable_write();
  target_alpha_data_= reinterpret_cast<boxm2_data_traits<BOXM2_ALPHA>::datatype*>(target_alpha_base_->data_buffer());

  target_app_base_  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  target_app_base_->enable_write();
  target_app_data_=reinterpret_cast<boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*>(target_app_base_->data_buffer());

  target_nobs_base_  = boxm2_cache::instance()->get_data_base(target_scene,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  target_nobs_base_->enable_write();
  target_nobs_data_= reinterpret_cast<boxm2_data_traits<BOXM2_NUM_OBS>::datatype*>(target_nobs_base_->data_buffer());
#if 0
  // caution fill target block only works for unrefined target scenes
  // should not be used after refinement!!!!
  if(has_background_){
    std::cout<< " Darkening background "<<std::endl;
    this->fill_target_block();
  }
#endif
}

void boxm2_vecf_articulated_scene::extract_unrefined_cell_info(){
  if(!target_blk_){
    std::cout << "FATAL! - NULL target block\n";
    return;
  }
  //iterate through the trees of the target. At this point they are unrefined
  unrefined_cell_info_.resize(targ_n_.x()*targ_n_.y()*targ_n_.z());
  for(unsigned ix = 0; ix<targ_n_.x(); ++ix){
    for(unsigned iy = 0; iy<targ_n_.y(); ++iy){
      for(unsigned iz = 0; iz<targ_n_.z(); ++iz){
        double x = targ_origin_.x() + ix*targ_dims_.x();
        double y = targ_origin_.y() + iy*targ_dims_.y();
        double z = targ_origin_.z() + iz*targ_dims_.z();
        auto lindex = static_cast<unsigned>(target_linear_index(ix, iy, iz));
        unrefined_cell_info cinf;
        cinf.linear_index_ = lindex;
        cinf.ix_ = ix; cinf.iy_ = iy; cinf.iz_ = iz;
        cinf.pt_.set(x+0.5*targ_dims_.x(), y+0.5*targ_dims_.y(), z+0.5*targ_dims_.z());
        unrefined_cell_info_[lindex]=cinf;
      }
    }
   }
}

void boxm2_vecf_articulated_scene::prerefine_target(boxm2_scene_sptr target_scene){
    if(!target_blk_){
    std::cout << "FATAL! - NULL target block\n";
    return;
  }
  vul_timer t;
  int deepest_cell_depth = 0;

  // the array of depths found in the source intersecting the inversely transformed sub_block (tree) bounding box.
  vbl_array_3d<int> depths_to_match(targ_n_.x(), targ_n_.y(), targ_n_.z());
  depths_to_match.fill(0);

  //iterate through the trees of the target. At this point they are unrefined
  for(auto & uit : unrefined_cell_info_){
    const vgl_point_3d<double>& pt = uit.pt_;
    unsigned lindex = uit.linear_index_;
    //record the deepest tree found
    int max_depth = this->prerefine_target_sub_block(pt, lindex);
    // if max_depth == -1  then don't change the refinement level
    // since the target didn't map to a valid source position
    depths_to_match(uit.ix_, uit.iy_, uit.iz_) = max_depth;
    if(max_depth>deepest_cell_depth){
      deepest_cell_depth = max_depth;
    }
  }
  std::cout << "deepest cell depth in prerefine_target " << deepest_cell_depth << '\n';

  //fully refine the target trees to the required depth
  std::vector<std::string> prefixes;
  prefixes.emplace_back("alpha");  prefixes.emplace_back("boxm2_mog3_grey"); prefixes.emplace_back("boxm2_num_obs");
  boxm2_refine_block_multi_data_function(target_scene, target_blk_, prefixes, depths_to_match);
  std::cout << "prefine in " << t.real() << " msec\n";
}
