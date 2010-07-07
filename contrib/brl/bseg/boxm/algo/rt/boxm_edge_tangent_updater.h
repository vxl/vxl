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

template<class T_loc, class APM, class AUX>
class boxm_edge_tangent_updater
{
 public:

  boxm_edge_tangent_updater(boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene,
                            vcl_vector<vcl_string> const& image_ids, bool use_ransac=true, float ransac_ortho_thres = 0.01f, float ransac_volume_ratio = 128.0f, int ransac_concensus_cnt = 3);

  ~boxm_edge_tangent_updater(){}

  bool add_cells();

 protected:
  vcl_vector<vcl_string> image_ids_;
  bool use_ransac_;

  float ransac_ortho_thres_;  
  float ransac_volume_ratio_;
  int ransac_concensus_cnt_;  // number of images that need to contribute planes to a hypothesis for a cell to be assigned a 3D edge
                              // there should be at least ransac_concensus_cnt_+1 training images for some concensus to be met at any voxel, otherwise edge world will be empty

  boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene_;
};

#endif
