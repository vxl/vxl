#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_sample.h>

typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> >  tree_type;
BOXM_SCENE_INSTANTIATE(tree_type);
