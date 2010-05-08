#include <boxm2/boxm_scene.txx>
#include <boct/boct_tree.txx>
#include <vgl/io/vgl_io_point_3d.h>

typedef boct_tree<short,vgl_point_3d<double> > tree;
BOXM2_SCENE_INSTANTIATE(tree);
