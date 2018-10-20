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

#include <vgl/vgl_box_3d.h>
#include <vsl/vsl_binary_io.h>

template <class T>
class boxm_block
{
 public:
  //: Default Constructor
  boxm_block(): octree_(nullptr) {}
  //: Destructor
  ~boxm_block() { if (octree_) delete octree_; }
  //: Constructor from boundinf box
  boxm_block(vgl_box_3d<double> bbox): bbox_(bbox), octree_(nullptr) {}
  //: Constructor from bounding box and octree
  boxm_block(vgl_box_3d<double> bbox, T* tree) : bbox_(bbox), octree_(tree) {octree_->set_bbox(bbox_);}
  //: Returns the number of leaf cells in the tree
  unsigned int size();
  //: Initializes octree and bounding-box
  void init_tree(T * octree);
  //: Sets the current octree
  void set_tree(T * octree);

  // Accessors
  vgl_box_3d<double> bounding_box() const { return bbox_; }
  T* get_tree() { return octree_; }

  //: Releases memory
  void delete_tree() { delete octree_; octree_ = nullptr; }

  // IO
  void b_read(vsl_b_istream &s);
  void b_write(vsl_b_ostream &s, const bool save_internal_nodes_, const bool platform_independent = true);
  static short version_no() { return 1; }

  bool operator==(boxm_block<T> const& other) const {
    return bbox_ == other.bounding_box()
        && octree_ == other.octree_;
  }

 private:
  vgl_box_3d<double> bbox_;
  T* octree_;
};

#endif
