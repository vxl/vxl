#ifndef boxm_block_vis_graph_node_h_
#define boxm_block_vis_graph_node_h_
//:
// \file
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vgl/vgl_point_3d.h>

//: A comparison functor for vgl_point_3d's. Needed to create a vcl_set of vgl_point_3d<int>'s.
template <class T>
class vgl_point_3d_cmp : public vcl_binary_function<vgl_point_3d<T>, vgl_point_3d<T>, bool>
{
 public:
  vgl_point_3d_cmp() {}

  bool operator()(vgl_point_3d<T> const& v0, vgl_point_3d<T> const& v1) const
  {
    if (v0.z() != v1.z())
      return v0.z() < v1.z();
    else if (v0.y() != v1.y())
      return v0.y() < v1.y();
    else
      return v0.x() < v1.x();
  }
};

template <class T>
class boxm_block_vis_graph_node
{
 public:
  typedef vcl_map<vgl_point_3d<int>, boxm_block_vis_graph_node<T>, vgl_point_3d_cmp<int> > vis_graph_type;

  boxm_block_vis_graph_node() : in_count(0) {}
  ~boxm_block_vis_graph_node(){}

  int dec_in_count() { return --in_count; }

  //: the number of incoming edges to this node
  int in_count;

  //: graph links to the outgoing blocks
  vcl_vector<typename vis_graph_type::iterator> out_links;
};

#define BOXM_BLOCK_VIS_GRAPH_NODE_INSTANTIATE(T) \
template class boxm_block_vis_graph_node<T >

#endif
