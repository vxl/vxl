#ifndef boxm_block_h_
#define boxm_block_h_

#include <vpgl/vpgl_camera.h>
#include <vgl/vgl_box_3d.h>
#include <boct/boct_tree.h>

template <class T>
class boxm_block
{
public:
  boxm_block(): octree_(0) {}
  ~boxm_block(){}
  boxm_block(vgl_box_3d<double> bbox): bbox_(bbox) {}
  boxm_block(vgl_box_3d<double> bbox, boct_tree<short,T>* tree) : bbox_(bbox), octree_(tree) {}
  bool is_visible(const vpgl_camera_double_sptr camera) {return true; }
  void b_read(vsl_b_istream &s);
  void b_write(vsl_b_ostream &s);
  short version_no() { return 1; }
private:
  vgl_box_3d<double> bbox_;
  boct_tree<short,T>* octree_;
  //boxm_visibility_graph_node* vis_graph_;
};

#endif