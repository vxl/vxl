#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_rt_sample.h>

typedef boct_tree<short,boxm_rt_sample<float> > tree;
BOXM_SCENE_INSTANTIATE(tree);
