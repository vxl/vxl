// This is brl/bseg/brec/brec_hierarchy_edge.h
#ifndef brec_hierarchy_edge_h_
#define brec_hierarchy_edge_h_
//:
// \file
// \brief a class to represent part hierarchy edges
//
// The relative occurrence information of the parts will be stored in the edge
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 16, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "brec_part_base.h"
#include "brec_part_base_sptr.h"

#include <bgrl2/bgrl2_edge.h>
#include <bsta/bsta_gauss_f1.h>
#include <vgl/vgl_box_2d.h>
#include <bxml/bxml_document.h>

class brec_hierarchy_edge : public bgrl2_edge<brec_part_base>
{
 public:

  //: edge between v1 in layer i to v2 in layer i-1
  //  If no relative spatial arrangement model then this is an edge to the central part in the previous layer.
  //  Default is the central
  brec_hierarchy_edge(brec_part_base_sptr v1, brec_part_base_sptr v2) : bgrl2_edge<brec_part_base>(v1, v2), to_central_(true), min_stad_dev_dist_(2.0f), min_stad_dev_angle_(10.0f) {}

  //: this constructor should only be used during parsing
  brec_hierarchy_edge() : bgrl2_edge<brec_part_base>(), to_central_(true), min_stad_dev_dist_(2.0f), min_stad_dev_angle_(10.0f) {}

  //: if the model is updated then the to_central flag is made false since the edge becomes a non-central edge
  void update_dist_model(const float dist);
  void update_angle_model(const float angle);
  //void set_model(const bsta_gauss_f2& mod) { loc_model_ = mod; to_central_ = false; }
  void set_model(const bsta_gauss_f1& dist_mod, const bsta_gauss_f1& angle_mod) { dist_model_ = dist_mod; angle_model_ = angle_mod; to_central_ = false; }

  float prob_density(const float dist, const float angle);  //vnl_vector_fixed<float,2>& pt);
  float prob_density_dist(const float dist);
  float prob_density_angle(const float angle);
  //vnl_vector_fixed<float,2> mean() { return loc_model_.mean(); }
  float mean_dist() { return dist_model_.mean(); }
  float mean_angle() { return angle_model_.mean(); }
  float var_dist() { return dist_model_.var(); }
  float var_angle() { return angle_model_.var(); }

  void calculate_dist_angle(brec_part_instance_sptr pi, vnl_vector_fixed<float,2>& dif_to_center, float& dist, float& angle);

  bool to_central() { return to_central_; }

  void set_min_stand_dev_dist(float d) { min_stad_dev_dist_ = d; }
  void set_min_stand_dev_angle(float a) { min_stad_dev_angle_ = a; }

  vgl_box_2d<float> get_probe_box(brec_part_instance_sptr central_p);

  virtual bxml_data_sptr xml_element();
  virtual bool xml_parse_element(bxml_data_sptr data);

 protected:
  bool to_central_;

  //: 2 1D gaussian models to model location of v2 wrt central part in v1
  bsta_gauss_f1 dist_model_; // distance between the centers
  //: model angle in radians
  bsta_gauss_f1 angle_model_;  // angle between the centers wrt the orientation of the central model

  float min_stad_dev_dist_; // default 2 pixels
  float min_stad_dev_angle_; // default 10 degrees
};

#endif  //brec_hierarchy_edge_h_
