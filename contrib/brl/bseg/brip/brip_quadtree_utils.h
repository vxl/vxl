#ifndef brip_quadtree_utils_h
#define brip_quadtree_utils_h
//-----------------------------------------------------------------
//:
// \file
// \author  J.L. Mundy March 8, 2008
// \brief brip_quadtree_utils - Various utilities using quadtrees
//
// \verbatim
//  Modifications:
//   None
// \endverbatim
//------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <brip/brip_quadtree_node.h>
#include <vbl/vbl_array_2d.h>

template <class T>
class brip_quadtree_utils
{
 public:
  //:Given a leaf quadtree node, fill the corresponding image region with its value -- the base case
  static void fill_image_region(brip_quadtree_node_base_sptr node,
                                vil_image_view<T>& img);

  //:Given a tree root, recursively fill the image values according to the tree nodes
  static void fill_image_from_node(brip_quadtree_node_base_sptr node,
                                   vil_image_view<T>& img);

  //: Construct an array of quadtree nodes from an image and a parent image
  //  The masks determine if a pixel is to be included in the tree
  //  Scale is the ratio of the child resolution to the parent resolution,
  //  e.g. scale = 2 indicates that the dimensions of the child are twice
  //  that of the parent.
  static void
    extract_nodes_from_image(vil_image_view<T> const & img,
                             vil_image_view<bool> const & mask,
                             vil_image_view<T> const& parent_img,
                             vil_image_view<bool> const& parent_mask,
                             unsigned scale,
                             vbl_array_2d<brip_quadtree_node_base_sptr>& nodes);

  //: given two layers of a pyramid of quadtree nodes, establish the parent-chlid links
static void connect_children(vbl_array_2d<brip_quadtree_node_base_sptr>& nodes,
                             unsigned scale,
                             vbl_array_2d<brip_quadtree_node_base_sptr> const& prev);

//: Construct the quadtree from an image pyramid, with masks defined for each pyramid layer
static
void quadtrees_from_pyramid(vcl_vector<vil_image_view<T> > levels,
                            vcl_vector<vil_image_view<bool> > masks,
                            vbl_array_2d<brip_quadtree_node_base_sptr>& roots);

//: Recursively print all the levels of the quadtree below the node
 static void print_node( brip_quadtree_node_base_sptr const& node,
                         vcl_ostream& os = vcl_cout,
                         vcl_string indent = "");
};


#define BRIP_QUADTREE_UTILS_INSTANTIATE(T) extern "please include brip_quadtree_utils.txx first"


#endif
