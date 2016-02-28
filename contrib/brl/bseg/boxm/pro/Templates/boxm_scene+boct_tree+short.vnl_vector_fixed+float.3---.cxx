#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.h>
#include <vnl/vnl_vector_fixed.h>

typedef boct_tree<short,vnl_vector_fixed<float,3> > tree;
BOXM_SCENE_INSTANTIATE(tree);
