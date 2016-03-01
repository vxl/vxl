#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_edge_sample.h>

typedef boct_tree<short,boxm_edge_sample<float> > tree;
BOXM_SCENE_INSTANTIATE(tree);

typedef boct_tree<short,boxm_aux_edge_sample<float> > aux_tree;
BOXM_SCENE_INSTANTIATE(aux_tree);
