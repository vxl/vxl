//This is brl/bbas/volm/volm_geo_index.h
#ifndef volm_geo_index_h_
#define volm_geo_index_h_
//:
// \file
// \brief Classes to enable fast access to geographic areas and attributes
// Units are in lat, lon
//        index is a quadtree
//        some of the 4 children of a node may have zero as sptr which means that region is pruned (out of ROI or eliminated for some reason)
//        WARNING: this class assumes that the areas in question are small enough so that Euclidean distances approximate geodesic distances
//                 also assumes that the regions do not cross over lon = 0
//
//
//
// \author Ozge C. Ozcanli
// \date Jan 10, 2013

#include <iostream>
#include <string>
#include <vector>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include "volm_tile.h"
#include "volm_geo_index_sptr.h"
#include "volm_loc_hyp_sptr.h"
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>

class volm_geo_index_node : public vbl_ref_count
{
 public:
   volm_geo_index_node(vgl_box_2d<double> const& extent, volm_geo_index_node_sptr& parent) : extent_(extent), parent_(parent) {}
   volm_geo_index_node(vgl_box_2d<double> const& extent) : extent_(extent), parent_(nullptr) {}
   ~volm_geo_index_node() override;
   std::string get_string();
   std::string get_hyp_name(std::string const& geo_index_name_pre) { return geo_index_name_pre + "_" + this->get_string() + ".bin"; }
   std::string get_index_name(std::string const& geo_index_name_pre) { return geo_index_name_pre + "_" + this->get_string() + "_index.bin"; }
   std::string get_label_index_name(std::string const& geo_index_name_pre, std::string const& identifier);

 public:
   // mini tile
   vgl_box_2d<double> extent_;   // min point of this box is lower left corner of the mini tile of this node
                                 // max point of this box is used as upper bound if pixels in the mini tile are iterated

   volm_geo_index_node_sptr parent_;
   std::vector<volm_geo_index_node_sptr> children_;

   // other data, e.g. keywords of geographic attributes that this tile contains, a set of hypotheses
   //
   volm_loc_hyp_sptr hyps_;
};

class volm_geo_index
{
 public:
  //: construct a tree such that the given tile is the root, and the hierarchy of its children form a quadtree space partition
  static volm_geo_index_node_sptr construct_tree(volm_tile t, float min_size);

  //: construct with a polygon with possibly multiple sheets, only keep the children who intersect one of the sheets of the polygon
  //static volm_geo_index_node_sptr construct_tree(volm_tile t, float min_size, vgl_polygon<float> const& poly);
  static volm_geo_index_node_sptr construct_tree(volm_tile t, float min_size, vgl_polygon<double> const& poly);

  //: prune the children which do not intersect the poly
  static bool prune_tree(const volm_geo_index_node_sptr& root, vgl_polygon<float> const& poly);
  static bool prune_tree(const volm_geo_index_node_sptr& root, vgl_polygon<double> const& poly);
  //: prune nodes whose bbox is not in the utm_zone
  static bool prune_by_zone(const volm_geo_index_node_sptr& root, unsigned utm_zone);

  //: depth at leaf level is 0
  static unsigned depth(const volm_geo_index_node_sptr& node);

  //: write the bboxes of the nodes at the given depth to kml file
  static void write_to_kml(const volm_geo_index_node_sptr& root, unsigned depth, std::string const& file_name);
  static void write_to_kml_node(std::ofstream& ofs, const volm_geo_index_node_sptr& n, unsigned current_depth, unsigned depth, std::string explanation = "location");

  //: write index quadtree in a text file, only the tree structure and not the leaf data
  static void write(const volm_geo_index_node_sptr& root, std::string const& file_name, float min_size);

  //: even if a child has zero pointer, it's order in the children_ array is the same, this is to make sure that the children have consistent geographic meaning
  static volm_geo_index_node_sptr read_and_construct(std::string const& file_name, float& min_size);

  static void get_leaves(const volm_geo_index_node_sptr& root, std::vector<volm_geo_index_node_sptr>& leaves);

  //: return all the leaves that intersect a given rectangular area
  static void get_leaves(const volm_geo_index_node_sptr& root, std::vector<volm_geo_index_node_sptr>& leaves, vgl_box_2d<double>& area);

  static void get_leaves_with_hyps(const volm_geo_index_node_sptr& root, std::vector<volm_geo_index_node_sptr>& leaves);
  //: write the geo index hyps
  static void write_hyps(const volm_geo_index_node_sptr& root, std::string const& file_name_pre);
  static void read_hyps(const volm_geo_index_node_sptr& root, std::string const& file_name_pre);

  static unsigned hypo_size(const volm_geo_index_node_sptr& root);

  static bool add_hypothesis(const volm_geo_index_node_sptr& root, double lon, double lat, double elev);

  //: return the leaf and the hyp id of the closest hyp to the given location
  static volm_geo_index_node_sptr get_closest(const volm_geo_index_node_sptr& root, double lat, double lon, unsigned& hyp_id);

  //: return true if a hyp exists within the given radius to the given point
  static bool exists(const volm_geo_index_node_sptr& root, double lat, double lon, double inc_in_sec_rad);
};

#endif // volm_geo_index_h_
