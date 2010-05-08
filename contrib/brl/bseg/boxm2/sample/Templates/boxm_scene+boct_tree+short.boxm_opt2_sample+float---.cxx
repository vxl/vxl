#include <boxm2/boxm_scene.txx>
#include <boct/boct_tree.h>
#include <boxm2/sample/boxm_opt2_sample.h>

typedef boct_tree<short,boxm_opt2_sample<float> > tree;
BOXM2_SCENE_INSTANTIATE(tree);
