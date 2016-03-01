#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <boxm/sample/boxm_inf_line_sample.h>

typedef boct_tree<short,boxm_edge_tangent_sample<float> > aux_tree;
BOXM_SCENE_INSTANTIATE(aux_tree);

typedef boct_tree<short,boxm_inf_line_sample<float> > tree;
BOXM_SCENE_INSTANTIATE(tree);
