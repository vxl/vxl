#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_aux_traits.h>
#include <boxm/opt/boxm_opt_rt_sample.h>

#include "../boxm_aux_scene.txx"

BOXM_AUX_SCENE_INSTANTIATE(short, boxm_sample<BOXM_APM_SIMPLE_GREY>, boxm_opt_rt_sample<float>);
