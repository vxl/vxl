// This is brl/bseg/brec/brec_part_base.h
#ifndef brec_part_base_h_
#define brec_part_base_h_
//:
// \file
// \brief base class for composable parts
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct 16, 2008
//
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <bgrl2/bgrl2_vertex.h>

#include "brec_part_base_sptr.h"
#include "brec_hierarchy_edge_sptr.h"

#include <vnl/vnl_vector_fixed.h>

#include <vil/vil_image_view.h>

#include <bxml/bxml_document.h>

class brec_hierarchy_edge;
class brec_part_gaussian;

class brec_part_base : public bgrl2_vertex<brec_hierarchy_edge>
{
 public:

  brec_part_base(unsigned layer, unsigned type) : bgrl2_vertex<brec_hierarchy_edge>(), layer_(layer), type_(type), activation_radius_(0.0f), detection_threshold_(0.01f) {}

  //: this constructor should only be used during parsing
  brec_part_base() : layer_(0), type_(0) {}

  //: we assume that the part that is added first as the outgoing part is the central part
  brec_part_base_sptr central_part();

  //: we assume that the part that is added first as the outgoing part is the central part
  brec_hierarchy_edge_sptr edge_to_central_part();

  virtual bool mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_center(vil_image_view<vxl_byte>& img, unsigned plane);

  virtual brec_part_gaussian* cast_to_gaussian(void);
  virtual brec_part_instance* cast_to_instance(void);
  virtual brec_part_base* cast_to_base(void);

  virtual bxml_data_sptr xml_element();
  virtual bool xml_parse_element(bxml_data_sptr data);

  unsigned layer_;
  unsigned type_;

  float activation_radius_;
  float detection_threshold_;
};

class brec_part_instance_kind
{
 public:
  enum possible_kinds {
    GAUSSIAN,   // only GAUSSIAN is implemented for now in brec_part_gaussian
    EDGE,
    COMPOSED,   // the instance could be a composition if not primitive
  };
};

class brec_part_instance : public brec_part_base
{
 public:

  brec_part_instance(unsigned layer, unsigned type, unsigned kind, float x, float y, float strength) : brec_part_base(layer, type),
    x_(x), y_(y), strength_(strength), kind_(kind) {}

  //: this constructor should only be used during parsing
  brec_part_instance() : brec_part_base(0, 0), x_(0), y_(0), strength_(0), kind_(0) {}

  virtual brec_part_gaussian* cast_to_gaussian(void);
  virtual brec_part_instance* cast_to_instance(void);

  virtual bool mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_center(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_receptive_field(vil_image_view<float>& img, float val);

  virtual vnl_vector_fixed<float,2> direction_vector(void);  // return a unit vector that gives direction of this instance in the image

  virtual bxml_data_sptr xml_element();
  virtual bool xml_parse_element(bxml_data_sptr data);

  float x_, y_;  // location
  float strength_;
  unsigned kind_;   // one of brec_part_instance_kind enum types
};

#endif  //brec_part_base_h_
