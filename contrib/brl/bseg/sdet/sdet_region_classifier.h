#ifndef sdet_region_classifier_h_
#define sdet_region_classifier_h_
//---------------------------------------------------------------------
//:
// \file
// \brief A classifier for regions found by selective search
//
//
// \author
//  J.L. Mundy - December 26, 2016
//
// \verbatim
// Modifications
//  None
// \endverbatim
//
#include <iostream>
#include <map>
#include <queue>
#include <set>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include "sdet_region.h"
#include "sdet_region_classifier_params.h"
#include "sdet_selective_search.h"
#include <bsta/bsta_histogram.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_rtree.h>
#include <vgl/algo/vgl_rtree_c.h>
#include <vgl/vgl_area.h>
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
class sdet_rtree_region_box_2d
{
  // only static methods
  sdet_rtree_region_box_2d() = delete;
  ~sdet_rtree_region_box_2d() = delete;
 public:
  typedef sdet_region_sptr v_type;
  typedef vgl_box_2d<T> b_type;

  // Operations------
  static void  init  (vgl_box_2d<T>& b, sdet_region_sptr const& r)
  { b = r->bbox();}

  static void  update(vgl_box_2d<T>& b, sdet_region_sptr const& r)
  { b.add(r->bbox());}

  static void  update(vgl_box_2d<T>& b0, vgl_box_2d<T> const &b1)
  { b0.add(b1.min_point());  b0.add(b1.max_point()); }

  static bool  meet(vgl_box_2d<T> const& b, sdet_region_sptr const& r){
    return sdet_rtree_region_box_2d::meet(b, r->bbox());
  }
  static bool  meet(vgl_box_2d<T> const& b0, vgl_box_2d<T> const& b1) {
    vgl_box_2d<float> intr = vgl_intersection<float>(b0, b1);
    return !intr.is_empty();
  }
  static float volume(vgl_box_2d<T> const& b)
  { return static_cast<float>(vgl_area(b)); }

  // point meets for a polygon, used by generic rtree probe
  static bool meets(sdet_region_sptr const& r, vgl_polygon<T> poly)
  { return vgl_intersection<T>(r->bbox(), poly);}

  // box meets for a polygon, used by generic rtree probe
  static bool meets(vgl_box_2d<T> const& b, vgl_polygon<T> poly)
  { return vgl_intersection<T>(b, poly); }
};

class sdet_region_classifier : public sdet_region_classifier_params
{
 public:
  typedef sdet_rtree_region_box_2d<float> C_; // the helper class
  typedef C_::v_type V_; // the contained object type
  typedef C_::b_type B_; // the bounding object type
  //Constructors/destructor
  //:use default parameters
  sdet_region_classifier()= default;
  //:specify different parameters
 sdet_region_classifier(sdet_region_classifier_params& rpp):sdet_region_classifier_params(rpp){}
  ~sdet_region_classifier() override= default;;
  void set_diverse_regions(const std::map<unsigned, sdet_region_sptr >& regions){diverse_regions_ = regions;}
  const std::map<unsigned, sdet_region_sptr>& diverse_regions(){return diverse_regions_;}
  void set_diverse_hists(const std::map<unsigned, bsta_histogram<float> >& hists){diverse_hists_ = hists;}
  const std::map<unsigned, bsta_histogram<float> >& diverse_hists() {return diverse_hists_;}
  const std::map<unsigned, bsta_histogram<float> >& neighbors_hists() {return neighbors_hists_;}
  const std::map< unsigned, std::map<unsigned, float> >& iou_clusters(){return iou_clusters_;}
  const std::map< unsigned, std::map<unsigned, region_sim> >& cluster_sim(){return cluster_similarity_;}
  void find_iou_clusters(const std::map<unsigned, sdet_region_sptr >& regions);
  void remove_diverse_region(unsigned label);
  void compute_iou_cluster_similarity();
  float compute_partition_quality(std::map< unsigned, std::map<unsigned, region_sim> > const& cluster_sim);
  bool merge_similarity_map(std::map< unsigned, std::map<unsigned, region_sim> > const& sim_before,
                            std::map< unsigned, std::map<unsigned, region_sim> >& sim_after,
                            unsigned labi, unsigned labj, unsigned new_label);
  void compute_hist_of_nbrs();
  void compute_bright_regions();
  const std::set<unsigned>& bright_regions(){return bright_regions_;}
  void process(){
  }
    //: io functions
 private:
  std::map<unsigned, sdet_region_sptr> diverse_regions_;
  std::map<unsigned, bsta_histogram<float> >  diverse_hists_;
  std::map<unsigned, bsta_histogram<float> >  neighbors_hists_;
  std::set<unsigned> bright_regions_;
  std::map< unsigned, std::map<unsigned, float> > iou_clusters_;
  std::map< unsigned, std::map<unsigned, region_sim> > cluster_similarity_;
};
#endif // sdet_region_classifier_h_
