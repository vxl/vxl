// This is brl/bbas/bvgl/algo/bvgl_2d_geo_index.h
#ifndef bvgl_2d_geo_index_h_
#define bvgl_2d_geo_index_h_
//:
// \file
// \brief A templated class using quad-tree to enable fast access to 2D geographic regions and attributes
//        Index is a quad-tree and each node has a square bounding box to define its extent
//        Some of the children node may have zero as sptr which means that region is pruned by bounding box, polygon etc
//
// \author Yi Dong
// \date October 29
//
// \verbatim
// Modifications
//   Yi Dong    Feb, 2014 -- add method to write tree structure given a lvcs if the tree is not in geo coordinates
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include "bvgl_2d_geo_index_node_base.h"

template <class Type>
class bvgl_2d_geo_index_node : public bvgl_2d_geo_index_node_base
{
public:
  //: default constructor
  bvgl_2d_geo_index_node() = default;

  //: constructor from a bounding box and a parent node
  bvgl_2d_geo_index_node(vgl_box_2d<double> const& extent, bvgl_2d_geo_index_node_sptr& parent)
  {
    extent_ = extent;
    parent_ = parent;
  }

  //: constructor without parent node
  bvgl_2d_geo_index_node(vgl_box_2d<double> const& extent)
  {
    extent_ = extent;
    parent_ = nullptr;
  }

  //: destructor
  ~bvgl_2d_geo_index_node() override = default;

  //: data inside current node
  Type contents_;
};

class bvgl_2d_geo_index
{
public:
  //: construct a tree such that the given bounding box is the root extent, and the hierarchy of its children form a quadtree space partition
  template <class Type>
  static bvgl_2d_geo_index_node_sptr construct_tree(vgl_box_2d<double> const& bbox, double const& min_size);
  template <class Type>
  static bvgl_2d_geo_index_node_sptr construct_tree(vgl_box_2d<float> const& bbox, float const& min_size);

  //: construct a tree with one polygon having multiple sheets, only keep the children intersecting with the polygon
  template <class Type>
  static bvgl_2d_geo_index_node_sptr construct_tree(vgl_box_2d<double> const& bbox, double const& min_size, vgl_polygon<double> const& poly);
  template <class Type>
  static bvgl_2d_geo_index_node_sptr construct_tree(vgl_box_2d<float> const& bbox, float const& min_size, vgl_polygon<float> const& poly);

  //: create a tree from text file.  Note even if a child has zero pointer, it's order in the children array remains same such that the children
  //  have consistent clockwise geographic sequence
  template <class Type>
  static bvgl_2d_geo_index_node_sptr read_and_construct(std::string const& file_name, double& min_size);

  //: prune the children which do not intersect with given polygon
  static bool prune_tree(const bvgl_2d_geo_index_node_sptr& root, vgl_polygon<double> const& poly);
  static bool prune_tree(const bvgl_2d_geo_index_node_sptr& root, vgl_polygon<float> const& poly);

  //: write out kml file at given depth of the tree
  static void write_to_kml(const bvgl_2d_geo_index_node_sptr& root, unsigned const& depth, std::string const& kml_file, std::string explanation="location", std::string name = " ");
  //: write out kml file for quadtree with non geo coordinates using given lvcs
  static void write_to_kml(const bvgl_2d_geo_index_node_sptr& root, unsigned const& depth, std::string const& kml_file, vpgl_lvcs_sptr const& lvcs, std::string explanation="location", std::string name = " ");
  //: write out kml file for the given node and its children
  static void write_to_kml_node(std::ofstream& ofs, const bvgl_2d_geo_index_node_sptr& n, unsigned const& current_depth, unsigned const& depth, const std::string& explanation="location", const std::string& name=" ");
  static void write_to_kml_node(std::ofstream& ofs, const bvgl_2d_geo_index_node_sptr& n, unsigned const& current_depth, unsigned const& depth,
                                vpgl_lvcs_sptr const& lvcs, const std::string& explanation="location", const std::string& name=" ");

  //: write the quadtree structure into a text file, only the tree structure and not the content on the node
  static void write(const bvgl_2d_geo_index_node_sptr& root, std::string const& file_name, double const& min_size);

  //: write the quadtree structure into a text file, the local coordinates inside the quadtree will be transferred to lon/lat using given lvcs
  static void write(const bvgl_2d_geo_index_node_sptr& root, std::string const& file_name, double const& min_size, vpgl_lvcs_sptr const& lvcs);

  //: return all leaves of the quadtree
  static void get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves);

  //: return all leaves that intersect with given rectangular area
  static void get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_box_2d<double> const& region);
  static void get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_box_2d<float>  const& region);

  //: return all leaves that intersect with given polygon
  static void get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_polygon<double> const& poly);
  static void get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, vgl_polygon<float>  const& poly);

  //: return all leaves that intersect with a line (vector of points)
  static void get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, std::vector<vgl_point_2d<double> > const& line);
  static void get_leaves(const bvgl_2d_geo_index_node_sptr& root, std::vector<bvgl_2d_geo_index_node_sptr>& leaves, std::vector<vgl_point_2d<float> >  const& line);

  //: locate the leave given a point
  static void get_leaf(const bvgl_2d_geo_index_node_sptr& root, bvgl_2d_geo_index_node_sptr& leaf, vgl_point_2d<double> const& point);
  static void get_leaf(const bvgl_2d_geo_index_node_sptr& root, bvgl_2d_geo_index_node_sptr& leaf, vgl_point_2d<float>  const& point);

  //: return the depth of the quadtree at given node
  static unsigned depth(const bvgl_2d_geo_index_node_sptr& node);

  //: type conversion
  static bool convert_polygon(vgl_polygon<float>  const& in_poly, vgl_polygon<double>& out_poly);
  static bool convert_polygon(vgl_polygon<double> const& in_poly, vgl_polygon<float>&  out_poly);
  static bool convert_box(vgl_box_2d<float>  const& in_box, vgl_box_2d<double>& out_box);
  static bool convert_box(vgl_box_2d<double> const& in_box, vgl_box_2d<float>&  out_box);

};

#include <bvgl/algo/bvgl_2d_geo_index_sptr.h>
#define BVGL_2D_GEO_INDEX_INSTANTIATE(T) extern "Please #include <bvgl/algo/bvgl_2d_geo_index.hxx>"

#endif // bvgl_2d_geo_index_h_
