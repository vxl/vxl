#include <boxm/boxm_scene.txx>
#include <boct/boct_tree.txx>

typedef boct_tree<short,int > tree;
BOXM_SCENE_INSTANTIATE(tree);

typedef boct_tree<short, short int > short_tree;
BOXM_SCENE_INSTANTIATE(short_tree);
