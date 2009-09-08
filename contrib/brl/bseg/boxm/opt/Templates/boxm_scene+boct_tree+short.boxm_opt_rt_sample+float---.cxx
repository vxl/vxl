#include <boxm/boxm_scene.txx>
#include <boct/boct_tree.h>
#include "boxm/boxm_rt_sample.h"

typedef boct_tree<short,boxm_rt_sample<float> > tree;
BOXM_SCENE_INSTANTIATE(tree);
