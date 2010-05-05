#include <boxm/boxm_scene.txx>
#include <boct/boct_tree.h>
#include <boxm/opt/boxm_opt2_sample.h>

typedef boct_tree<short,boxm_opt2_sample<float> > tree;
BOXM_SCENE_INSTANTIATE(tree);
