#ifndef boxm_edge_tangent_updater_h_
#define boxm_edge_tangent_updater_h_

#include <vcl_vector.h>
#include <vcl_string.h>

#include "boxm_aux_traits.h"
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene.h>

#include <boxm/boxm_inf_line_sample.h>

template<class T_loc, class APM, class AUX>
class boxm_edge_tangent_updater
{
public:

  boxm_edge_tangent_updater(boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene, 
                            vcl_vector<vcl_string> const& image_ids);

  ~boxm_edge_tangent_updater(){}

  bool add_cells();

protected:

  vcl_vector<vcl_string> image_ids_;

  boxm_scene<boct_tree<T_loc, boxm_inf_line_sample<APM> > > &scene_;

};















#endif
