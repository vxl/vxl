#include <boxm/boxm_scene.h>
#include <boxm/boxm_sample.h>
#include <vbl/vbl_smart_ptr.txx>

typedef boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY>> > boxm_scene_short_sample_mog_grey;
VBL_SMART_PTR_INSTANTIATE(boxm_scene_short_sample_mog_grey);
