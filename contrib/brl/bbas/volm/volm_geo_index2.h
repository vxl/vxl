// This is brl/bbas/volm/volm_geo_index2.h
#ifndef volm_geo_index2_h_
#define volm_geo_index2_h_
//:
// \file
// \brief A templated class to enable fast access to geographic areas and attributes
// Units are in lat, lon
//        index is a quadtree
//        some of the 4 children of a node may have zero as sptr which means that region is pruned (out of ROI or eliminated for some reason)
//        WARNING: this class assumes that the areas in question are small enough so that Euclidean distances approximate geodesic distances
//                 also assumes that the regions do not cross over lon = 0
//
// \author Yi Dong
// \date July 09, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include "volm_tile.h"
#include "volm_geo_index2_node_base.h"


template <class Type>
class volm_geo_index2_node : public volm_geo_index2_node_base
{
public:
  //: Default constructor
  volm_geo_index2_node() = default;

  //: Constructor
  volm_geo_index2_node(vgl_box_2d<double> const& extent, volm_geo_index2_node_sptr& parent)
  {
    extent_ = extent;
    parent_ = parent;
  }

  volm_geo_index2_node(vgl_box_2d<double> const& extent)
  {
    extent_ = extent;
    parent_ = nullptr;
  }

  //: Destructor
  ~volm_geo_index2_node() override = default;

public:
  //: data inside current tile (for non-leaf tile, the data should be empty, i.e. the templated member should contain method that contents_.size() == 0 or contents_.empty() == true)
  Type contents_;
};

class volm_geo_index2
{
public:
  //: construct a tree such that the give tile is the root, and the hierarchy of its children form a quadtree space partition
  template <class Type>
  static volm_geo_index2_node_sptr construct_tree(volm_tile t, double const& min_size);
  template <class Type>
  static volm_geo_index2_node_sptr construct_tree(vgl_box_2d<double> bbox, double const& min_size);

  //: construct with a polygon with possible multiple sheets, only keep the children who intersect one of the sheets of the polygon
  template <class Type>
  static volm_geo_index2_node_sptr construct_tree(volm_tile t, double const& min_size, vgl_polygon<double> const& poly);
  template <class Type>
  static volm_geo_index2_node_sptr construct_tree(vgl_box_2d<double> bbox, double const& min_size, vgl_polygon<double> const& poly);
  template <class Type>
  static volm_geo_index2_node_sptr construct_tree(volm_tile t, double const& min_size, vgl_polygon<float> const& poly);
  template <class Type>
  static volm_geo_index2_node_sptr construct_tree(vgl_box_2d<double> bbox, double const& min_size, vgl_polygon<float> const& poly);

  //: create the quadtree from text file.  Note even if a child has zero pointer, it's order in the children array remains same such that
  //  the children have consistent clockwise geographic sequence
  template <class Type>
  static volm_geo_index2_node_sptr read_and_construct(std::string const& file_name, double& min_size);

  //: prune the children which do not intersect with given polygon
  static bool prune_tree(const volm_geo_index2_node_sptr& root, vgl_polygon<float>  const& poly);
  static bool prune_tree(const volm_geo_index2_node_sptr& root, vgl_polygon<double> const& poly);
  //: prune the tree by utm zone
  static bool prune_by_zone(const volm_geo_index2_node_sptr& root, unsigned utm_zone);

  //: write the bboxes of the nodes at the give depth to kml file
  static void write_to_kml(const volm_geo_index2_node_sptr& root, unsigned depth, std::string const& file_name);
  static void write_to_kml_node(std::ofstream& ofs, const volm_geo_index2_node_sptr& n, unsigned current_depth, unsigned depth, std::string explanation = "location");

  //: write the quadtree structure into a text file, only the tree structure and not the content on the leaf
  static void write(const volm_geo_index2_node_sptr& root, std::string const& file_name, double const& min_size);

  //: return all leaves of the quadtree
  static void get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves);

  //: return all leaves that intersect with a given rectangular area
  static void get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_box_2d<float>  const& area);
  static void get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_box_2d<double> const& area);

  //: return all leaves that intersect with a given polygon
  static void get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_polygon<float>  const& poly);
  static void get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, vgl_polygon<double> const& poly);

  //: return all leaves that intersect with a line (this line is a list of points)
  static void get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, std::vector<vgl_point_2d<float> > const& line);
  static void get_leaves(const volm_geo_index2_node_sptr& root, std::vector<volm_geo_index2_node_sptr>& leaves, std::vector<vgl_point_2d<double> > const& line);

  //: return the leave that contains the given loc point
  static void get_leaf(const volm_geo_index2_node_sptr& root, volm_geo_index2_node_sptr& leaf, vgl_point_2d<float> const& point);
  static void get_leaf(const volm_geo_index2_node_sptr& root, volm_geo_index2_node_sptr& leaf, vgl_point_2d<double> const& point);

  //: return the depth of quadtree at level node
  static unsigned depth(const volm_geo_index2_node_sptr& node);



};

#include <volm/volm_geo_index2_sptr.h>
#define VOLM_GEO_INDEX2_INSTANTIATE(T) extern "Please #include <volm/volm_geo_index2.hxx>"

#endif // volm_geo_index2_h_
