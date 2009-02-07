// This is brl/bseg/brec/brec_part_hierarchy_detector.h
#ifndef brec_part_hierarchy_detector_h_
#define brec_part_hierarchy_detector_h_
//:
// \file
// \brief class to detect instances of a hiearachy of composable parts in images
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
#include <vgl/algo/vgl_rtree.h>
#include <vgl/algo/vgl_rtree_c.h>

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
  rtree_brec_instance_box_2d();
  ~rtree_brec_instance_box_2d();

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
    vgl_point_2d<T> b0min = b0.min_point();
    vgl_point_2d<T> b1min = b1.min_point();
    vgl_point_2d<T> b0max = b0.max_point();
    vgl_point_2d<T> b1max = b1.max_point();
    vgl_point_2d<T> max_of_mins(b0min.x() > b1min.x() ? b0min.x() : b1min.x(), b0min.y() > b1min.y() ? b0min.y() : b1min.y());
    vgl_point_2d<T> min_of_maxs(b0min.x() < b1min.x() ? b0min.x() : b1min.x(), b0min.y() < b1min.y() ? b0min.y() : b1min.y());

    return b0.contains(b1min) || b0.contains(b1max) ||
           b1.contains(b0min) || b1.contains(b0max) ||
           ( (b0.contains(max_of_mins) || b0.contains(min_of_maxs)) &&
             (b1.contains(max_of_mins) || b1.contains(min_of_maxs)) );

    //bool resultf =(b0.contains(b1.min_point()) || b0.contains(b1.max_point()));
    //bool resultr =(b1.contains(b0.min_point()) || b1.contains(b0.max_point()));
    //return resultf||resultr;
  }
  static float volume(vgl_box_2d<T> const& b)
  { return static_cast<float>(b.area()); }

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

class brec_part_hierarchy_detector : public vbl_ref_count
{
 public:

  brec_part_hierarchy_detector(brec_part_hierarchy_sptr h) : h_(h) {}
  ~brec_part_hierarchy_detector();

  //: check for existency of upper_p with central_p as its central part and map will tell if all the other parts exist
  brec_part_instance_sptr exists(brec_part_base_sptr upper_p, brec_part_instance_sptr central_p, unsigned ni, unsigned nj, Rtree_type* lower_rtree, float det_threshold);

  //: check for existency of upper_p with central_p as its central part and map will tell if all the other parts exist
  //  No thresholding, \return a probabilistic score
  brec_part_instance_sptr exists(brec_part_base_sptr upper_p, brec_part_instance_sptr central_p, Rtree_type* lower_rtree);

  //: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
  void extract_upper_layer(vcl_vector<brec_part_instance_sptr>& extracted_parts, unsigned ni, unsigned nj, Rtree_type* extracted_parts_rtree,
    vcl_vector<brec_part_instance_sptr>& extracted_upper_parts);

  //: given a set of detected lower level parts, create a set of instance detections for one layer above in the hierarchy
  //  No thresholding, \return a probabilistic score
  void extract_upper_layer(vcl_vector<brec_part_instance_sptr>& extracted_parts, Rtree_type* extracted_parts_rtree,
    vcl_vector<brec_part_instance_sptr>& extracted_upper_parts);

  //: extracts instances of each layer in the given image
  bool detect(vil_image_resource_sptr img);

  //: extracts instances of each layer in the given image, by rotating the detector with the given amount
  bool detect(vil_image_resource_sptr img, float angle);

  //: extracts instances of each layer in the given image, by rotating the detector with the given amount
  //  Assumes that training is complete and a training directory path is set accordingly in h_
  //  Reads response model parameters for each primitive from this training directory
  bool detect_using_trained_response_models(vil_image_view<float>& img, vil_image_view<float>& prob_img, float angle);

  bool detect_for_training(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img, vil_image_view<bool>& mask);

  vcl_vector<brec_part_instance_sptr>& get_parts(unsigned layer) { return map_instance_[layer]; }
  Rtree_type* get_tree(unsigned layer) { return map_rtree_[layer]; }

  brec_part_hierarchy_sptr get_hierarchy() { return h_; }

 protected:
  brec_part_hierarchy_sptr h_;

  //: map each layer to a vector of instances of it
  vcl_map<unsigned, vcl_vector<brec_part_instance_sptr> > map_instance_;

  //: map each layer to an rtree of its instances
  vcl_map<unsigned, Rtree_type*> map_rtree_;
};

// Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_detector as a brdb_value
void vsl_b_write(vsl_b_ostream & os, brec_part_hierarchy_detector const &hl);
void vsl_b_read(vsl_b_istream & is, brec_part_hierarchy_detector &hl);
void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy_detector* hl);
void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy_detector* &hl);

#endif  //brec_part_hierarchy_detector_h_
