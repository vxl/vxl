#ifndef bwm_texture_map_generator_h
#define bwm_texture_map_generator_h
// :
// \file

#include "bwm_observer_rat_cam.h"
#include "bwm_observable_mesh.h"
#include "bwm_observer_mgr.h"

#include <vcl_vector.h>
#include <vcl_string.h>

#include <vgl/vgl_vector_3d.h>
#include <vpgl/vpgl_lvcs.h>

class bwm_texture_map_generator
{
public:
  // : constructors
  bwm_texture_map_generator() {observers_ = bwm_observer_mgr::instance()->observers_rat_cam(); }

  // : generate the texture map
  bool generate_texture_map(bwm_observable_mesh_sptr obj, vcl_string texture_filename, vpgl_lvcs lvcs);

private:
  vgl_vector_3d<double> compute_face_normal_lvcs(bmsh3d_face* face, vpgl_lvcs lvcs);

  vcl_vector<bwm_observer_rat_cam *> observers_;
};

#endif
