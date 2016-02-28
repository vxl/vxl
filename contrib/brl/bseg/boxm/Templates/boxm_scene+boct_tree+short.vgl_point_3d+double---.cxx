#include <boxm/boxm_scene.hxx>
#include <boct/boct_tree.hxx>
#include <vgl/io/vgl_io_point_3d.h>

typedef boct_tree<short,vgl_point_3d<double> > tree;
BOXM_SCENE_INSTANTIATE(tree);
