#include <boxm2/boxm_scene.txx>
#include <boct/boct_tree.h>
#include <boxm2/sample/boxm_sample.h>

typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> >  tree_type;
BOXM2_SCENE_INSTANTIATE(tree_type);
