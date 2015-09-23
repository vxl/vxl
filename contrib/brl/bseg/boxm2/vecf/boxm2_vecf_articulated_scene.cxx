#include "boxm2_vecf_articulated_scene.h"
#include <boxm2/io/boxm2_lru_cache.h>
void boxm2_vecf_articulated_scene::clear_target(boxm2_scene_sptr target_scene){
  vcl_string gray_APM_prefix  = boxm2_data_traits<BOXM2_MOG3_GREY>::prefix();
  vcl_string color_APM_prefix = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix();
  vcl_string alpha_prefix     = boxm2_data_traits<BOXM2_ALPHA>::prefix();

  vcl_vector<boxm2_block_id> blocks = target_scene->get_block_ids();
  boxm2_data_base* alpha_db     = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], alpha_prefix);
  boxm2_data_base* gray_app_db  = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], gray_APM_prefix);
  boxm2_data_base* color_app_db = boxm2_cache::instance()->get_data_base(target_scene, blocks[0], color_APM_prefix + "_" + (this->color_apm_id_));
  boxm2_block_metadata m_data = target_scene->get_block_metadata(blocks[0]);
  gray_app_db->set_default_value(gray_APM_prefix,   m_data);
  color_app_db->set_default_value(color_APM_prefix, m_data);
  alpha_db->set_default_value(alpha_prefix, m_data);

}
  double boxm2_vecf_articulated_scene::interp_generic_double(vgl_point_3d<double>* neighbors, double* probs, vgl_point_3d<double> p ){

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
