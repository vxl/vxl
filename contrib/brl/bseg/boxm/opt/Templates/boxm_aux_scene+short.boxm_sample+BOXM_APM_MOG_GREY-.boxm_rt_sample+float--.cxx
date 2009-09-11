#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_aux_traits.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_sample.h>
#include "../boxm_aux_scene.txx"

BOXM_AUX_SCENE_INSTANTIATE(short,boxm_sample<BOXM_APM_MOG_GREY>,boxm_rt_sample<float>);
