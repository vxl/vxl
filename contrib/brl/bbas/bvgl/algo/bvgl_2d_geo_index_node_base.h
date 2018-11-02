// This is brl/bbas/bvgl/algo/bvgl_2d_geo_index_node_base.h
#ifndef bvgl_2d_geo_index_node_base_h_
#define bvgl_2d_geo_index_node_base_h_
//:
// \file
// \brief Abstract base class for bvgl_2d_geo_index_node
//
// \author Yi Dong
// \date October 29, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <iostream>
#include <string>
#include <vector>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include "bvgl_2d_geo_index_sptr.h"

class bvgl_2d_geo_index_node_base : public vbl_ref_count
{
public:
  //: default constrcutor
  bvgl_2d_geo_index_node_base() = default;

  //: destructor
  ~bvgl_2d_geo_index_node_base() override { children_.clear(); }

  //: return a name base on the extent boundary
  std::string get_string() const;

  //: return a name based on the extent boundary and an input identifier
  std::string get_label_name(std::string const& geo_index_name_pre, std::string const& identifier);

  //: add children
  void add_child(bvgl_2d_geo_index_node_sptr child) { children_.push_back(child); }

  void set_extent(vgl_box_2d<double> extent) { extent_ = extent; }

  //: parent node
  bvgl_2d_geo_index_node_sptr parent_;

  //: children nodes
  std::vector<bvgl_2d_geo_index_node_sptr> children_;

  //: node boundary
  vgl_box_2d<double> extent_;
};


#endif // bvgl_2d_geo_index_node_base_h_
