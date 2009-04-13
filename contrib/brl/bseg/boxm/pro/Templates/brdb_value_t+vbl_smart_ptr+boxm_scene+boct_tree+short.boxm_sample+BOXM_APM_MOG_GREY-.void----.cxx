#include <brdb/brdb_value.txx>

#include <vbl/io/vbl_io_smart_ptr.h>
#include <boxm/boxm_scene.h>

typedef vbl_smart_ptr<boxm_scene<boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> ,void> > > boxm_scene_tree_short_sample_mog_grey_sptr;
BRDB_VALUE_INSTANTIATE(boxm_scene_tree_short_sample_mog_grey_sptr, "boxm_scene_short_sample_mog_grey_sptr");
