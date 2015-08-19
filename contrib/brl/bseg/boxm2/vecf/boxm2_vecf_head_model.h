#ifndef boxm2_vecf_head_model_h_
#define boxm2_vecf_head_model_h_
//:
// \file
// \brief  boxm2_vecf_head_model static model of human head (no eyes)
//
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include "boxm2_vecf_articulated_scene.h"
#include "boxm2_vecf_articulated_params.h"
typedef vnl_vector_fixed<float,4> float4;

#define LERP(w1,w2,p,p1,p2) (w1 * (p2 - p) + w2 * (p-p1))/(p2 - p1)

static double interp_generic_double(vgl_point_3d<double>* neighbors, double* probs, vgl_point_3d<double> p ){
  double dx00 = LERP(probs[0],probs[2],  p.x(),neighbors[0].x(),neighbors[2].x()); // interp   between (x0,y0,z0) and (x1,y0,z0)
  double dx10 = LERP(probs[1],probs[3],  p.x(),neighbors[1].x(),neighbors[3].x()); // interp   between (x0,y1,z0) and (x1,y1,z0)
  double dx01 = LERP(probs[4],probs[6],  p.x(),neighbors[4].x(),neighbors[6].x()); // interp   between (x0,y0,z1) and (x0,y0,z1)
  double dx11 = LERP(probs[5],probs[7],  p.x(),neighbors[5].x(),neighbors[7].x()); // interp x between x-1 and x+1 z = 1 y =1


  double dxy0 = LERP( dx00, dx10,p.y(),neighbors[0].y(),neighbors[1].y());
  double dxy1 = LERP( dx01, dx11,p.y(),neighbors[0].y(),neighbors[1].y());
  double dxyz = LERP( dxy0, dxy1,p.z(),neighbors[0].z(),neighbors[4].z());

  return dxyz;

}
class boxm2_vecf_head_model : public boxm2_vecf_articulated_scene
{
 public:
  boxm2_vecf_head_model(vcl_string const& scene_file,vcl_string color_apm_ident = "frontalized");

  //: map eye data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene);

  void clear_target(boxm2_scene_sptr target_scene);

  void set_scale(vgl_vector_3d<double> scale);

  void set_intrinsic_change( bool change){intrinsic_change_ = change;}

  virtual bool set_params(boxm2_vecf_articulated_params const& params){return true;} //blank for now

friend class boxm2_vecf_appearance_extractor; //the appearance extractor needs to signal a change to the original model when its apm is updated
 protected:
  bool intrinsic_change_;
  vgl_vector_3d<double> scale_;

private:
  bool get_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                boxm2_data_base **alpha_data,
                boxm2_data_base **app_data,
                boxm2_data_base **nobs_data);
};
#endif // boxm2_vecf_head_model_h_
