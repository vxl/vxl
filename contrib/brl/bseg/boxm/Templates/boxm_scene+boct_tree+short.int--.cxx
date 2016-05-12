#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.hxx>

typedef boct_tree<short,int > tree;
BOXM_SCENE_INSTANTIATE(tree);

typedef boct_tree<short, short int > short_tree;
BOXM_SCENE_INSTANTIATE(short_tree);
