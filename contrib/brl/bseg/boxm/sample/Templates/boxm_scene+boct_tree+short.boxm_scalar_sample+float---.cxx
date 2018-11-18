#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_scalar_sample.h>

typedef boct_tree<short,boxm_scalar_sample<float> > tree;
BOXM_SCENE_INSTANTIATE(tree);
