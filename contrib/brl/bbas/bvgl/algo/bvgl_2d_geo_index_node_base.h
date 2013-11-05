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

#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include "bvgl_2d_geo_index_sptr.h"

class bvgl_2d_geo_index_node_base : public vbl_ref_count
{
public:
  //: default constrcutor
  bvgl_2d_geo_index_node_base() {}

  //: destructor
  virtual ~bvgl_2d_geo_index_node_base() { children_.clear(); }

  //: return a name base on the extent boundary
  vcl_string get_string() const;

  //: return a name based on the extent boundary and an input identifier
  vcl_string get_label_name(vcl_string const& geo_index_name_pre, vcl_string const& identifier);

  //: add children
  void add_child(bvgl_2d_geo_index_node_sptr child) { children_.push_back(child); }

  void set_extent(vgl_box_2d<double> extent) { extent_ = extent; }

  //: parent node
  bvgl_2d_geo_index_node_sptr parent_;

  //: children nodes
  vcl_vector<bvgl_2d_geo_index_node_sptr> children_;

  //: node boundary
  vgl_box_2d<double> extent_;
};


#endif // bvgl_2d_geo_index_node_base_h_
