#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/sample/boxm_scalar_sample.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_aux_scene.hxx>

BOXM_AUX_SCENE_INSTANTIATE(short,boxm_sample<BOXM_APM_SIMPLE_GREY>,boxm_scalar_sample<float>);
BOXM_AUX_SCENE_INSTANTIATE(short,boxm_sample<BOXM_APM_MOG_GREY>,boxm_scalar_sample<float>);
