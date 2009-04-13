#include <boxm/boxm_scene.txx>
#include <boct/boct_tree.txx>
#include <boxm/boxm_sample.h>

typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY>,void >  tree_type;
BOXM_SCENE_INSTANTIATE(tree_type);
