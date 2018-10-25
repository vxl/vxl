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
//   <none yet>
// \endverbatim

#include "brec_part_base_sptr.h"
#include "brec_hierarchy_edge_sptr.h"
class brec_part_gaussian;

#include <bgrl2/bgrl2_vertex.h>
#include <bsta/bsta_histogram.h>
#include <bxml/bxml_document.h>

#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_2d.h>

class brec_part_base : public bgrl2_vertex<brec_hierarchy_edge>
{
 public:

  brec_part_base(unsigned layer, unsigned type) : bgrl2_vertex<brec_hierarchy_edge>(), layer_(layer), type_(type), activation_radius_(0.0f), detection_threshold_(0.01f) {}

  //: this constructor should only be used during parsing
  brec_part_base() : layer_(0), type_(0), prior_prob_(0) {}

  //: we assume that the part that is added first as the outgoing part is the central part
  brec_part_base_sptr central_part();

  //: we assume that the part that is added first as the outgoing part is the central part
  brec_hierarchy_edge_sptr edge_to_central_part();
  brec_hierarchy_edge_sptr edge_to_second_part();

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

  double prior_prob_;  // prior probability of the composition
  double log_likelihood_; // only used during training to pick the best parts
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

class brec_posterior_types
{
 public:
  enum types {
    CLASS_FOREGROUND,
    CLASS_BACKGROUND,
    NON_CLASS_FOREGROUND,
    NON_CLASS_BACKGROUND,
  };
};

class brec_part_instance : public brec_part_base
{
 public:

  brec_part_instance(unsigned layer, unsigned type, unsigned kind, float x, float y, float strength) : brec_part_base(layer, type),
    x_(x), y_(y), strength_(strength), kind_(kind), rho_c_f_(0.0), rho_c_b_(0.0), rho_nc_f_(0.0), rho_nc_b_(0.0), cnt_(0) {}

  //: this constructor should only be used during parsing
  brec_part_instance() : brec_part_base(0, 0), x_(0), y_(0), strength_(0), kind_(0) {}

  brec_part_gaussian* cast_to_gaussian(void) override;
  brec_part_instance* cast_to_instance(void) override;

  bool mark_receptive_field(vil_image_view<vxl_byte>& img, unsigned plane) override;
  bool mark_center(vil_image_view<vxl_byte>& img, unsigned plane) override;
  virtual bool mark_receptive_field(vil_image_view<float>& img, float val);
  virtual bool mark_center(vil_image_view<float>& img, float val);

  virtual vnl_vector_fixed<float,2> direction_vector(void);  // return a unit vector that gives direction of this instance in the image

  //: collect operator responses from the input image's foreground regions
  //  The input \p img and the \p fg_prob_img (foreground probability image) are float images with values in [0,1] range
  //  assumes histogram is initialized
  //  virtual method needs to be implemented by inheriting classes depending on the nature of the part
  virtual bool update_response_hist(vil_image_view<float>& /*img*/, vil_image_view<float>& /*fg_prob_img*/, vil_image_view<bool>& /*mask_img*/, bsta_histogram<float>& /*fg_h*/) { return false; }
  virtual bool fit_distribution_to_response_hist(bsta_histogram<float>& /*fg_h*/) { return false; }

  //: use the background \p mean_img and \p std_dev_img to construct response model for background and calculate posterior ratio of foreground and background
  virtual bool update_foreground_posterior(vil_image_view<float>& /*inp*/,
                                           vil_image_view<float>& /*fg_prob_img*/,
                                           vil_image_view<bool>& /*mask*/,
                                           vil_image_view<float>& /*mean_img*/,
                                           vil_image_view<float>& /*std_dev_img*/) { return false; }

  bxml_data_sptr xml_element() override;
  bool xml_parse_element(bxml_data_sptr data) override;

  //: return a simple box with diagonal 2*radius centered on this instance
  vgl_box_2d<float> get_probe_box(float radius);
  vnl_vector_fixed<float,2> location() const { return vnl_vector_fixed<float,2>(x_, y_); }

  float x_, y_;  // location
  float strength_;
  unsigned kind_;   // one of brec_part_instance_kind enum types
  //double rho_;  // the posterior probability of observing this instance
               // if the part is a primitive part, this is given by:
               // rho(alpha) = [p(alpha in foreground)p(strength|alpha in foreground)] / [p(alpha in background)p(strength|alpha in background)]
               //
               // each instance should have a foreground probability p(alpha in foreground) (for some operators, this may be calculated from a kernel region, e.g. for gaussian operators)
               // p(alpha in background) = 1 - p(alpha in foreground) by convention

  //: there are 4 possibilities regarding existence of a part: class instance in the foreground, class instance in the background, non-class instance in the foreground, non-class instance in the background
  double rho_c_f_;  //posterior of class, fore
  double rho_c_b_;  // posterior of class, back
  double rho_nc_f_;  // posterior of non-class, fore
  double rho_nc_b_;  // posterior of non-class, back

  int cnt_; // a variable used to update expected rho_ as new data comes in
};

#endif  //brec_part_base_h_
