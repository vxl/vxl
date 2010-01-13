#ifndef boxm_block_h_
#define boxm_block_h_
//:
// \file
// \brief  a block is a smallest area that is going to be processed with an octree
//
// \author Gamze Tunali
// \date Apr 01, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vpgl/vpgl_camera.h>
#include <vgl/vgl_box_3d.h>
#include <boct/boct_tree.h>

template <class T>
class boxm_block
{
 public:
  boxm_block(): octree_(0) {}
  ~boxm_block() { if (octree_) delete octree_; }
  boxm_block(vgl_box_3d<double> bbox): bbox_(bbox), octree_(0) {}
  boxm_block(vgl_box_3d<double> bbox, T* tree) : bbox_(bbox), octree_(tree) {}
  void init_tree(T * octree);
  void set_tree(T * octree) { octree_=octree; }
  vgl_box_3d<double> bounding_box() const { return bbox_; }
  T* get_tree() { return octree_; }
  void delete_tree() { delete octree_; octree_ = 0; }
  void b_read(vsl_b_istream &s);
  void b_write(vsl_b_ostream &s, const bool save_internal_nodes_);
  static short version_no() { return 1; }

 private:
  vgl_box_3d<double> bbox_;
  T* octree_;
};

#endif
