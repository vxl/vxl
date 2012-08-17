#ifndef boxm_edge_tangent_updater_h_
#define boxm_edge_tangent_updater_h_
//:
// \file
// \brief class to find the optimal intersecting 3D lines for each voxel in the scene
//
// \author G. Tunali
//
// \verbatim
//  Modifications
//    Ozge C. Ozcanli  July 7, 2010 : made some internal hard-coded defaults into parameters
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>

#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>
#include <boxm/sample/boxm_inf_line_sample.h>

#include <vpgl/vpgl_camera_double_sptr.h>
#include <vil/vil_image_view.h>

template<class T_loc, class APM, class AUX>
class boxm_edge_tangent_updater
{
 public:

  boxm_edge_tangent_updater(boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene,
                            vcl_vector<vcl_string> const& image_ids, bool use_ransac=true, float ransac_ortho_thres = 0.01f, float ransac_volume_ratio = 128.0f, int ransac_consensus_cnt = 3);

  ~boxm_edge_tangent_updater(){}

  bool add_cells();

 protected:
  vcl_vector<vcl_string> image_ids_;
  bool use_ransac_;

  float ransac_ortho_thres_;
  float ransac_volume_ratio_;
  int ransac_consensus_cnt_;  // number of images that need to contribute planes to a hypothesis for a cell to be assigned a 3D edge
                              // there should be at least ransac_consensus_cnt_+1 training images for some consensus to be met at any voxel, otherwise edge world will be empty

  boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene_;
};

template<class T_loc, class APM, class AUX>
class boxm_edge_tangent_refine_updates
{
 public:

  boxm_edge_tangent_refine_updates(boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene, int consensus_cnt,
                                   vcl_vector<vil_image_view<float> > const& edge_images,
                                   vcl_vector<vpgl_camera_double_sptr> const& cameras);

  ~boxm_edge_tangent_refine_updates() {}

  bool refine_cells();

 protected:
  vcl_vector<vil_image_view<float> > edge_images_; // edge_img(ix, iy, 0) = static_cast<float>(x); subpixel location of edge
                                                   // edge_img(ix, iy, 1) = static_cast<float>(y);
                                                   // edge_img(ix, iy, 2) = static_cast<float>(dir); tangent direction of the edgel
  vcl_vector<vpgl_camera_double_sptr> cameras_;

  int consensus_cnt_;  // how many images need to be in agreement for an edgel to survive
  boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene_;
};

#endif
