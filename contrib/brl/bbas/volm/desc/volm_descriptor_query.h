// This is brl/bbas/volm/desc/volm_descriptor_query.h
#ifndef volm_descriptor_query_h_
#define volm_descriptor_query_h_
//:
// \file
// \brief A representation of image query using volumetric descriptor
// \author Yi Dong
// \date May 20, 2013
//
//
// \verbatim
//  <none yet>
// \endverbatim
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <volm/volm_io.h>
#include <volm/desc/volm_descriptor.h>
#include <volm/desc/volm_descriptor_sptr.h>
#include <bpgl/depth_map/depth_map_scene_sptr.h>
#include <bpgl/depth_map/depth_map_scene.h>

class volm_descriptor_query
{
public:
  //: default constructor
  volm_descriptor_query() {}
  
  //: construct from depth_map_scene
  volm_descriptor_query(depth_map_scene_sptr const& dm,
                        vcl_vector<double> const& radius,
                        vcl_vector<double> const& height,
                        unsigned const& n_orient = 3,
                        unsigned const& n_land = volm_label_table::land_id.size(),
                        unsigned char const& initial_mag = 0);
  
  //: construct from labelme xml
  volm_descriptor_query(vcl_string const labelmexml,
                        vcl_vector<double> const& radius,
                        vcl_vector<double> const& height,
                        unsigned const& n_orient = 3,
                        unsigned const& n_land = volm_label_table::land_id.size(),
                        unsigned char const& initial_mag = 0);

  //: accessors
  depth_map_scene_sptr dm() const { return dm_; }
  volm_descriptor_sptr vd() const { return vd_; }
  bsta_histogram<unsigned char> const h() { return vd_->h(); }

  //: destructor
  ~volm_descriptor_query() {}

  void print() const;
  //: visualize the query via svg figure
  void visualize(vcl_string out_file, unsigned char const& y_max = 10) const
  {
    this->vd_->visualize(out_file, y_max);
  }

private:
  depth_map_scene_sptr dm_;
  volm_descriptor_sptr vd_;

};

#endif // volm_descriptor_query_h_
