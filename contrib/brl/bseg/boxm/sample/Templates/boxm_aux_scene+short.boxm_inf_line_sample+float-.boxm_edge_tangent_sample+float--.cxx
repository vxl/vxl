#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_edge_sample.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <boxm/boxm_aux_scene.hxx>

BOXM_AUX_SCENE_INSTANTIATE(short,boxm_inf_line_sample<float>,boxm_edge_tangent_sample<float>);
