// This is brl/bbas/imesh/algo/imesh_kd_tree.h
#ifndef imesh_kd_tree_h_
#define imesh_kd_tree_h_
//:
// \file
// \brief A KD-Tree for mesh faces
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date June 2, 2008

#include <imesh/imesh_mesh.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>


//: The minimum square distance between \a p and any point in \a b
double imesh_min_sq_dist(const vgl_point_3d<double>& p,
                         const vgl_box_3d<double>& b);

//: The maximum square distance between \a p and any point in \a b
double imesh_max_sq_dist(const vgl_point_3d<double>& p,
                         const vgl_box_3d<double>& b);


//: A node in the KD-Tree
struct imesh_kd_tree_node
{
  //: Constructor for internal node
  imesh_kd_tree_node( const vgl_box_3d<double>& outer_box,
                      const vgl_box_3d<double>& inner_box,
                      vcl_auto_ptr<imesh_kd_tree_node> left,
                      vcl_auto_ptr<imesh_kd_tree_node> right)
    : outer_box_(outer_box), inner_box_(inner_box),
      index_(static_cast<unsigned int>(-1)),
      left_(left), right_(right) {}

  //: Constructor for leaf node
  imesh_kd_tree_node( const vgl_box_3d<double>& outer_box,
                      const vgl_box_3d<double>& inner_box,
                      unsigned int index )
    : outer_box_(outer_box), inner_box_(inner_box),
      index_(index), left_(0), right_(0) {}

  //: Copy Constructor (makes a deep copy recursively)
  imesh_kd_tree_node(const imesh_kd_tree_node& other)
    : outer_box_(other.outer_box_),
      inner_box_(other.inner_box_),
      index_(other.index_),
      left_(other.left_.get() ? new imesh_kd_tree_node(*other.left_) : 0),
      right_(other.right_.get() ? new imesh_kd_tree_node(*other.right_) : 0) {}


  //: Return true if this node is a leaf node
  bool is_leaf() const {return !left_.get() && !right_.get();}

  //: Outer bounding box
  vgl_box_3d<double> outer_box_;
  //: Inner bounding box
  vgl_box_3d<double> inner_box_;
  //: index into original data vector (at leaf nodes)
  //  Additional indices are assigned to internal nodes
  unsigned int index_;
  //: Left child
  vcl_auto_ptr<imesh_kd_tree_node> left_;
  //: Right child
  vcl_auto_ptr<imesh_kd_tree_node> right_;
};


//: Construct a kd-tree (in 3d) of axis aligned boxes
vcl_auto_ptr<imesh_kd_tree_node>
imesh_build_kd_tree(const vcl_vector<vgl_box_3d<double> >& boxes);


//: construct a kd-tree of mesh faces
vcl_auto_ptr<imesh_kd_tree_node>
imesh_build_kd_tree(const imesh_vertex_array<3>& verts,
                    const imesh_face_array_base& faces);


//: construct a kd-tree of mesh faces
inline vcl_auto_ptr<imesh_kd_tree_node>
imesh_build_kd_tree(const imesh_mesh& mesh)
{
  return imesh_build_kd_tree(mesh.vertices<3>(), mesh.faces());
}


//: A class used for sorting a queue of tree nodes by an assigned value
//  In the most common case this value is a distance measure
class imesh_kd_tree_queue_entry
{
 public:
  //: Constructor
  imesh_kd_tree_queue_entry() {}
  //: Constructor
  imesh_kd_tree_queue_entry( double val, imesh_kd_tree_node* node )
    : val_(val), node_(node) {}

  //: Used in sorting by distance
  bool operator< ( const imesh_kd_tree_queue_entry& right ) const
  { return right.val_ < this->val_; }

  //: The value
  double val_;
  //: pointer to the node
  imesh_kd_tree_node* node_;
};

//: compute the closest point on the mesh using the kd-tree
//  \returns the index of the closest triangle
//  \param dists (if specified) returns a vector of all explored nodes
//        and the closest square distance found so far
unsigned int
imesh_kd_tree_closest_point(const vgl_point_3d<double>& query,
                            const imesh_mesh& mesh,
                            const vcl_auto_ptr<imesh_kd_tree_node>& root,
                            vgl_point_3d<double>& cp,
                            vcl_vector<imesh_kd_tree_queue_entry>* dists = 0);


//: Traverse the tree looking for leaf nodes that contain the query
//  Returns a vector of leaf nodes paired with min square distance to the inner box
//  Also returns a vector of the unexplored internal node-distance^2 pairs
//  Resulting vectors are unsorted
void imesh_kd_tree_traverse(const vgl_point_3d<double>& query,
                            const vcl_auto_ptr<imesh_kd_tree_node>& root,
                            vcl_vector<imesh_kd_tree_queue_entry>& internal,
                            vcl_vector<imesh_kd_tree_queue_entry>& leaf);


#endif // imesh_kd_tree_h_
