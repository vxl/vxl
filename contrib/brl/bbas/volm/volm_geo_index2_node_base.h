// This is brl/bbas/volm/volm_geo_index2_node_base.h
#ifndef volm_geo_index2_node_base_h_
#define volm_geo_index2_node_base_h_
//:
// \file
// \brief Abstract base class for volm_geo_index2_node
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
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include "volm_geo_index2_sptr.h"

class volm_geo_index2_node_base : public vbl_ref_count
{
public:
  //: Default constructor
  volm_geo_index2_node_base() {}

  //: Destructor
  virtual ~volm_geo_index2_node_base() { children_.clear(); }

  //: retrive the mini tile name based on the bbox
  vcl_string get_string() const;

  //: retrive the desired binary filename for current mini tile
  vcl_string get_label_name(vcl_string const& geo_index_name_pre, vcl_string const& identifier);

public:
  //: parent tile
  volm_geo_index2_node_sptr parent_;
  //: 4 children
  vcl_vector<volm_geo_index2_node_sptr> children_;
  //: mini tile ( min point of this bbox is lower left corner of the mini tile and max point of this bbox is upper bound )
  //   x is lon, y is lat (xmin: lower left lon, ymin: lower left lat, xmax: upper right lon, ymax: upper right lat)
  vgl_box_2d<double> extent_;

};

#endif // volm_geo_index2_node_base_h_
