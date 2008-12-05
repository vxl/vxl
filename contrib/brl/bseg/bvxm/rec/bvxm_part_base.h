// This is brl/bseg/bvxm/rec/bvxm_part_base.h
#ifndef bvxm_part_base_h_
#define bvxm_part_base_h_
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

#include <rec/bvxm_part_base_sptr.h>
#include <rec/bvxm_hierarchy_edge_sptr.h>

#include <vnl/vnl_vector_fixed.h>

#include <vil/vil_image_view.h>

#include <bxml/bxml_document.h>

class bvxm_hierarchy_edge;
class bvxm_part_gaussian;

class bvxm_part_base : public bgrl2_vertex<bvxm_hierarchy_edge>
{
 public:

  bvxm_part_base(unsigned layer, unsigned type) : bgrl2_vertex<bvxm_hierarchy_edge>(), layer_(layer), type_(type), activation_radius_(0.0f), detection_threshold_(0.01f) {}

  //: this constructor should only be used during parsing
  bvxm_part_base() : layer_(0), type_(0) {}

  //: we assume that the part that is added first as the outgoing part is the central part
  bvxm_part_base_sptr central_part();

  //: we assume that the part that is added first as the outgoing part is the central part
  bvxm_hierarchy_edge_sptr edge_to_central_part();

  virtual bool mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_center(vil_image_view<vxl_byte>& img, unsigned plane);

  virtual bvxm_part_gaussian* cast_to_gaussian(void);
  virtual bvxm_part_instance* cast_to_instance(void);
  virtual bvxm_part_base* cast_to_base(void);

  virtual bxml_data_sptr xml_element();
  virtual bool xml_parse_element(bxml_data_sptr data);

  unsigned layer_;
  unsigned type_;

  float activation_radius_;
  float detection_threshold_;
};

class bvxm_part_instance_kind
{
 public:
  enum possible_kinds {
    GAUSSIAN,   // only GAUSSIAN is implemented for now in bvxm_part_gaussian
    EDGE,
    COMPOSED,   // the instance could be a composition if not primitive
  };
};

class bvxm_part_instance : public bvxm_part_base
{
 public:

  bvxm_part_instance(unsigned layer, unsigned type, unsigned kind, float x, float y, float strength) : bvxm_part_base(layer, type),
    x_(x), y_(y), strength_(strength), kind_(kind) {}

  //: this constructor should only be used during parsing
  bvxm_part_instance() : bvxm_part_base(0, 0), x_(0), y_(0), strength_(0), kind_(0) {}

  virtual bvxm_part_gaussian* cast_to_gaussian(void);
  virtual bvxm_part_instance* cast_to_instance(void);

  virtual bool mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_center(vil_image_view<vxl_byte>& img, unsigned plane);
  virtual bool mark_receptive_field(vil_image_view<float>& img, float val);

  virtual vnl_vector_fixed<float,2> direction_vector(void);  // return a unit vector that gives direction of this instance in the image

  virtual bxml_data_sptr xml_element();
  virtual bool xml_parse_element(bxml_data_sptr data);

  float x_, y_;  // location
  float strength_;
  unsigned kind_;   // one of bvxm_part_instance_kind enum types
};

#endif  //bvxm_part_base_h_
