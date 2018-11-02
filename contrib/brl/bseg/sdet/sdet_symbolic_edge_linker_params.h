// This is brl/bseg/sdet/sdet_symbolic_edge_linker_params.h
#ifndef sdet_symbolic_edge_linker_params_h_
#define sdet_symbolic_edge_linker_params_h_
//:
// \file
// \brief parameter mixin for symbolic edge linker
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#include <gevd/gevd_param_mixin.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


class sdet_symbolic_edge_linker_params : public gevd_param_mixin
{
  public:
// enumeration for the curve model choices
  enum {SIMPLE_LINEAR, LINEAR, CIRC_ARC_WO_PERT, CIRC_ARC_W_CLASSES, CIRC_ARC_W_PERT,
        CIRC_ARC_3D_BUNDLE, ES_WO_PERT, ES_W_PERT};
// enumeration for algorithm choices
  enum {GREEDY_GROUP, VERY_GREEDY_GROUP};
// enumeration for curvelet choices
  enum {ANCHOR_CENT, ANCHOR_CENT_BI, ANCHOR_LEAD_BI, ENO_STYLE};
// enumeration for appearance choices
  enum {DO_NOT_USE, LOCAL_COMP, AGAINST_REF};
// enumeration for link graph choices
  enum {ALL, IMMEDIATE_ONLY, IMMEDIATE_RECIP, IMMEDIATE_RECIP_W_SUPPORT, TRIPLETS_W_SUPPORT};
// enumeration for link algo choices
  enum { DO_NOT_LINK, FROM_LINK_GRAPH, REGULAR_CONTOURS };

  /*
   * Parameters
   *
   *                  nrad - Radius of neighborhood
   *                   gap - Maximum pixel distance to complete
   *            adap_uncer - Get uncertanty from edges
   *                    dx - Position uncertanty
   *                    dt - Orientation uncertanty (deg)
   *           cvlet_model - Curve Model
   *             token_len - Token length
   *                 max_k - Maximum curvature
   *             max_gamma - Maximum curvature derivative
   *         grouping_algo - Local grouping algorithm
   *            cvlet_type - Curvelet type
   *             app_usage - Appearance usage
   *            app_thresh - Appearance threshold
   *     max_size_to_group - Maximum number of edgels to group
   *  formCompleteCvletMap - Form complete curvelet map
   *         formLinkGraph - Form link graph
   *         use_all_cvlet - Use all curvelets
   *        linkgraph_algo - Extract image contours
   *        num_link_iters - Number of linking iterations
   *    get_final_contours - Get final contours
   */

  sdet_symbolic_edge_linker_params(double nrad = 3.5, double gap = 2.0, bool adap_uncer = true,
                                   double dx = 0.4, double dt = 15.0, unsigned curve_model = 5,
                                   double token_len = 1.0, double max_k = 0.2, double max_gamma = 0.05,
                                   unsigned grouping_algo = 0, unsigned cvlet_type = 0,
                                   unsigned app_usage = 0, double app_thresh = 0.2, unsigned max_size_to_group = 7,
                                   bool formCompleteCvletMap = false, bool formLinkGraph = true,
                                   bool use_all_cvlet = false, unsigned linkgraph_algo = 0,
                                   unsigned min_size_to_link = 4, unsigned linking_algo = 0,
                                   unsigned num_link_iters = 7, bool get_final_contours = true);

  sdet_symbolic_edge_linker_params(const sdet_symbolic_edge_linker_params& old_params);
  ~sdet_symbolic_edge_linker_params() override= default;

  bool SanityCheck() override;

  friend std::ostream& operator<<(std::ostream&,const sdet_symbolic_edge_linker_params& dp);

protected:

  void InitParams(double nrad, double gap, bool adap_uncer,
                  double dx, double dt, unsigned curve_model,
                  double token_len, double max_k, double max_gamma,
                  unsigned grouping_algo, unsigned cvlet_type,
                  unsigned app_usage, double app_thresh, unsigned max_size_to_group,
                  bool formCompleteCvletMap, bool formLinkGraph,
                  bool use_all_cvlet, unsigned linkgraph_algo,
                  unsigned min_size_to_link, unsigned linking_algo,
                  unsigned num_link_iters, bool get_final_contours);

///////////////////////


 public:

  double nrad_, gap_, dx_, dt_;
  bool badap_uncer_;
  unsigned curve_model_type_;
  double token_len_, max_k_, max_gamma_;

  unsigned grouping_algo_;
  unsigned max_size_to_group_;
  unsigned cvlet_type_;
  bool bCentered_grouping_;
  bool bBidirectional_grouping_;

  bool bFormCompleteCvletMap_;
  bool bFormLinkGraph_;
  bool b_use_all_cvlets_;
  unsigned app_usage_;
  double app_thresh_;

  unsigned linkgraph_algo_;
  unsigned min_size_to_link_;

  unsigned linking_algo_;
  unsigned num_link_iters_;

  bool bGetfinalcontours_;
};

#endif // sdet_symbolic_edge_linker_params_h_
