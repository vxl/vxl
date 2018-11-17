// This is sdet_curvelet_map.h
#ifndef sdet_curvelet_map_h
#define sdet_curvelet_map_h
//:
//\file
//\brief Curvelet Map data structure
//\author Amir Tamrakar
//\date 11/25/07
//
//\verbatim
//  Modifications
//    Amir Tamrakar  Instead of storing the curvelets with the edgels, I've created this
//                   data structure to hold them so that the edgel classes are lighter and
//                   more relevant to other algorithms.
//
//    Amir Tamrakar  Moved the parameters of the curvelet formation into a separate params class
//
//\endverbatim

#include <vector>
#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <sdet/sdet_edgel.h>
#include <sdet/sdet_curvelet.h>
#include <sdet/sdet_edgemap_sptr.h>

//: curvelet formation parameters
class sdet_curvelet_params
{
public:
  sdet_curve_model::curve_type C_type; ///< curve model type

  double rad_;    ///< radius of the grouping neighborhood around each edgel
  double dtheta_; ///< assumed uncertainty in orientation
  double dpos_;   ///< assumed uncertainty in position
  bool badap_uncer_; ///< get the uncertainty from the edges
  double token_len_; ///< Length of the edgel token (puts bounds on legal curvature)
  double max_k_;  ///< maximum curvature of curves in the curve bundle
  double max_gamma_; ///< maximum curvature derivative of curves in the curve bundle
  unsigned nrad_; ///< the range of edgel cells to search (this should contain all edgels within rad_)
  double gap_;
  unsigned maxN_; ///< largest curvelet size to form
  bool centered_; ///< anchored centered curvelet
  bool bidirectional_; ///< form curvelets in both directions from the anchored

  sdet_curvelet_params(sdet_curve_model::curve_type ctype=sdet_curve_model::CC2,
                       double rad=7.0,
                       double gap=1.0,
                       double dtheta=15,
                       double dpos=0.5,
                       bool adap_uncer=true,
                       double token_len=0.7,
                       double max_k=0.5,
                       double max_gamma=0.05,
                       bool centered=true,
                       bool bidir=false):
    C_type(ctype),
    rad_(rad),
    dtheta_(dtheta),
    dpos_(dpos),
    badap_uncer_(adap_uncer),
    token_len_(token_len),
    max_k_(max_k),
    max_gamma_(max_gamma),
    nrad_((unsigned) std::ceil(rad)+1),
    gap_(gap),
    maxN_(2*nrad_),
    centered_(centered),
    bidirectional_(bidir)
  {}

  ~sdet_curvelet_params()= default;
};

//: This class stores the map of curvelets formed by the SEL edge linker.
//  This is an intermediate structure before actual edge linking occurs.
class sdet_curvelet_map
{
public:
  //: The edgemap on which these curvelets have been formed
  //  (due to this smart pointer to the edgemap, the curvelets remain valid even if the edgemap is deleted elsewhere)
  sdet_edgemap_sptr EM_;

  //: various parameters used for forming this map
  sdet_curvelet_params params_;

  //: The curvelet map, indexed by edgel IDs
  std::vector<sdet_curvelet_list > map_;

  //: The curvelet map for the other direction (only for DHT mode)
  std::vector<sdet_curvelet_list > map2_;

  //: constructor
  sdet_curvelet_map(const sdet_edgemap_sptr& EM=nullptr, sdet_curvelet_params params=sdet_curvelet_params());

  //: destructor
  ~sdet_curvelet_map();

  //: to check if the Curvelet map is valid
  bool is_valid() { return map_.size()>0 && map_.size()==EM_->num_edgels(); }

  //: set the edgemap
  void set_edgemap(sdet_edgemap_sptr EM) { EM_ = EM; resize(EM->num_edgels()); }

  //: set the linking parameters
  void set_parameters(sdet_curvelet_params params) { params_ = params; }

  //: access the curvelets for an edge using its id
  const sdet_curvelet_list& curvelets(unsigned id) const { return map_[id]; }
  sdet_curvelet_list& curvelets(unsigned id) { return map_[id]; }

  sdet_curvelet_list& Rcurvelets(unsigned id) { return map2_[id]; }

  //: resize the graph
  void resize(unsigned size);

  //: clear the graph
  void clear();

  //: clear all the curvelets in the graph
  void clear_all_curvelets();

  //: add a curvelet to an edgel
  void add_curvelet(sdet_curvelet* curvelet, bool dir=true);

  //: remove a curvelet from this edgel
  void remove_curvelet(sdet_curvelet* curvelet);

  //: delete all the curvelets formed by this edgel
  void delete_all_curvelets(sdet_edgel* e);

  //: does this curvelet exist at this edgel?
  sdet_curvelet* does_curvelet_exist(sdet_edgel* e, std::deque<sdet_edgel*> & chain);

  //: does the given pair exist on the ref edgel?
  sdet_curvelet* find_pair(sdet_edgel* ref, sdet_edgel* eA, sdet_edgel* eB);

  //: does the given triplet exist on the ref edgel?
  sdet_curvelet* find_triplet(sdet_edgel* ref, sdet_edgel* eA, sdet_edgel* eB, sdet_edgel* eC);

  //: return largest curvelet formed by an edgel
  sdet_curvelet* largest_curvelet(sdet_edgel* e);

  friend class sdet_edge_map;
};

#endif // sdet_curvelet_map_h
