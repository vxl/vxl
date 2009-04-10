#include <boxm/boxm_scene.h>
#include <vbl/io/vbl_io_smart_ptr.txx>

typedef boxm_scene<boct_tree<short,vgl_point_3d<double>,void > > boxm_scene_short_point_double;
VBL_IO_SMART_PTR_INSTANTIATE(boxm_scene_short_point_double);
