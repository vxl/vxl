#include <boxm2/boxm_apm_traits.h>
#include <boxm2/sample/boxm_aux_traits.h>
#include <boxm2/sample/boxm_rt_sample.h>
#include <boxm2/sample/boxm_sample.h>
#include <boxm2/boxm_aux_scene.txx>

BOXM_AUX_SCENE_INSTANTIATE(short,boxm_sample<BOXM_APM_MOB_GREY>,boxm_rt_sample<float>);
