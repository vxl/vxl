#include <boxm2/boxm_scene.txx>
#include <boct/boct_tree.h>
#include <boxm2/sample/boxm_rt_sample.h>

typedef boct_tree<short,boxm_rt_sample<float> > tree;
BOXM2_SCENE_INSTANTIATE(tree);
