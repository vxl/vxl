// This is brl/bseg/brec/brec_part_hierarchy_detector.h
#ifndef brec_part_hierarchy_detector_h_
#define brec_part_hierarchy_detector_h_
//:
// \file
// \brief class to detect instances of a hierarchy of composable parts in images
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Oct 16, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "brec_part_hierarchy.h"
#include "brec_part_hierarchy_sptr.h"

#include <vbl/vbl_ref_count.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>
#include <vgl/algo/vgl_rtree.h>
#include <vgl/algo/vgl_rtree_c.h>
#include <vgl/vgl_intersection.h>

// C must have the following (static method) signatures :
// \code
//   void  C::init  (B &, V const &);
//   void  C::update(B &, V const &);
//   void  C::update(B &, B const &);
//   bool  C::meet  (B const &, V const &);
//   bool  C::meet  (B const &, B const &);
//   float C::volume(B const &);
// \endcode
template <class T>
class rtree_brec_instance_box_2d
{
  // only static methods
  rtree_brec_instance_box_2d() = delete;
  ~rtree_brec_instance_box_2d() = delete;

 public:
  typedef brec_part_instance_sptr v_type;
  typedef vgl_box_2d<T> b_type;
  typedef T t_type;

  // Operations------
  static void  init  (vgl_box_2d<T>& b, brec_part_instance_sptr const& pi)
  { vgl_point_2d<T> p(pi->x_, pi->y_); b = vgl_box_2d<T>();  b.add(p); }

  static void  update(vgl_box_2d<T>& b, brec_part_instance_sptr const& pi)
  { vgl_point_2d<T> p(pi->x_, pi->y_); b.add(p); }

  static void  update(vgl_box_2d<T>& b0, vgl_box_2d<T> const &b1)
  { b0.add(b1.min_point());  b0.add(b1.max_point()); }

  static bool  meet(vgl_box_2d<T> const& b, brec_part_instance_sptr const& pi)
  { vgl_point_2d<T> p(pi->x_, pi->y_);  return b.contains(p); }

  static bool  meet(vgl_box_2d<T> const& b0, vgl_box_2d<T> const& b1) {
    vgl_box_2d<T> bint = vgl_intersection<T>(b0, b1);
    return !bint.is_empty();
  }
  static float volume(vgl_box_2d<T> const& b)
  { return static_cast<float>(vgl_area(b)); }

  // point meets for a polygon, used by generic rtree probe
  static bool meets(brec_part_instance_sptr const& pi, vgl_polygon<T> poly)
  { vgl_point_2d<T> p(pi->x_, pi->y_); return poly.contains(p); }

  // box meets for a polygon, used by generic rtree probe
  static bool meets(vgl_box_2d<T> const& b, vgl_polygon<T> poly)
  { return vgl_intersection<T>(b, poly); }
};

// rtree definitions
typedef brec_part_instance_sptr V_type;  // the contained object type
typedef vgl_box_2d<float> B_type;    // the bounding object type
typedef rtree_brec_instance_box_2d<float> C_type; // the helper class
typedef vgl_rtree<V_type, B_type, C_type> Rtree_type;
typedef vgl_rtree_polygon_probe<V_type, B_type, C_type> Probe_type;

class brec_detector_methods
{
 public:
  enum possible_methods {
    POSTERIOR_NUMERATOR,   // uses posterior for the primitives (given fg and bg app models) but does not divide posterior with a denominator
    DENSITY_FOR_TRAINING,        // uses the density given by the appearance models of primitives  p(d,angle | ci)
    POSTERIOR,   // calculates the posterior probability, the denominator is calculated using other class hierarchies
                 // background geometry model is also required for the denominator, we assume uniform distributions for the distance and angle
  };
};

class brec_part_hierarchy_detector : public vbl_ref_count
{
 public:

  brec_part_hierarchy_detector(brec_part_hierarchy_sptr h) : h_(h), prior_c_f_(0.15f), prior_non_c_f_(0.15f), prior_c_b_(0.15f) {}
  ~brec_part_hierarchy_detector() override;

  //: check for existence of upper_p with central_p as its central part and map will tell if all the other parts exist
  brec_part_instance_sptr exists(const brec_part_base_sptr& upper_p, const brec_part_instance_sptr& central_p, unsigned ni, unsigned nj, Rtree_type* lower_rtree, float det_threshold);

  //: check for existence of upper_p with central_p as its central part and map will tell if all the other parts exist
  //  No thresholding, \return a probabilistic score
  brec_part_instance_sptr exists(const brec_part_base_sptr& upper_p, const brec_part_instance_sptr& central_p, Rtree_type* lower_rtree);

  brec_part_instance_sptr exists_for_training(const brec_part_base_sptr& upper_p, const brec_part_instance_sptr& central_p, Rtree_type* lower_rtree);

  brec_part_instance_sptr exists_using_hierarchies(const brec_part_base_sptr& upper_p, const brec_part_instance_sptr& central_p, Rtree_type* lower_rtree, double radius);

  //: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
  void extract_upper_layer(std::vector<brec_part_instance_sptr>& extracted_parts, unsigned ni, unsigned nj, Rtree_type* extracted_parts_rtree,
                           std::vector<brec_part_instance_sptr>& extracted_upper_parts);

  //: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
  //  No thresholding, \return a probabilistic score
  //  rho_calculation_method = 0 if probabilistic score
  //  rho_calculation_method = 1 if training
  //  rho_calculation_method = 2 if using other hierarchies to compute a posterior
  void extract_upper_layer(std::vector<brec_part_instance_sptr>& extracted_parts, Rtree_type* extracted_parts_rtree,
                           std::vector<brec_part_instance_sptr>& extracted_upper_parts, unsigned rho_calculation_method = 0, double radius = 10.0);

  //: extracts instances of each layer in the given image
  bool detect(const vil_image_resource_sptr& img);

  //: extracts instances of each layer in the given image, by rotating the detector with the given amount
  bool detect(const vil_image_resource_sptr& img, float angle);

  //: extracts instances of each layer in the given image, by rotating the detector with the given amount
  //  Assumes that training is complete and a training directory path is set accordingly in h_
  //  Reads response model parameters for each primitive from this training directory
  bool detect_primitives_using_trained_response_models(vil_image_view<float>& img, vil_image_view<float>& fg_prob_img, float angle, float prior_class);
  bool detect(vil_image_view<float>& img, vil_image_view<float>& fg_prob_img, float angle = 0.0f, unsigned rho_calculation_method = brec_detector_methods::POSTERIOR_NUMERATOR, double radius = 10.0, float prior_class = 0.1f, unsigned layer_id = 0);

  //: extracts instances of each layer in the given image, by rotating the detector with the given amount
  //  Sets rho parameter of the primitives differently during training
  bool detect_primitives_for_training(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img, float angle);

  std::vector<brec_part_instance_sptr>& get_parts(unsigned layer) { return map_instance_[layer]; }
  Rtree_type* get_tree(unsigned layer) { return map_rtree_[layer]; }

  brec_part_hierarchy_sptr get_hierarchy() { return h_; }

  void add_to_class_hierarchies(brec_part_hierarchy_sptr h) { class_hierarchies_.push_back(h); }

 protected:
  brec_part_hierarchy_sptr h_;

  //: map each layer to a vector of instances of it
  std::map<unsigned, std::vector<brec_part_instance_sptr> > map_instance_;

  //: map each layer to an rtree of its instances
  std::map<unsigned, Rtree_type*> map_rtree_;

  //: hierarchies of other classes if any
  //  Required for posterior computation for compositions during testing
  std::vector<brec_part_hierarchy_sptr> class_hierarchies_;

  double radius_;

 public:
  float prior_c_f_;
  float prior_non_c_f_;
  float prior_c_b_;     // prior_non_c_b_ = 1.0f - (prior_c_f_ + prior_non_c_f_ + prior_c_b);
};

// Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_detector as a brdb_value
void vsl_b_write(vsl_b_ostream & os, brec_part_hierarchy_detector const &hl);
void vsl_b_read(vsl_b_istream & is, brec_part_hierarchy_detector &hl);
void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy_detector* hl);
void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy_detector* &hl);

#endif  //brec_part_hierarchy_detector_h_
