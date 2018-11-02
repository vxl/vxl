// This is brl/bseg/sdet/sdet_symbolic_edge_linker_params.cxx
#include <sstream>
#include <iostream>
#include "sdet_symbolic_edge_linker_params.h"
//:
// \file
// See sdet_symbolic_edge_linker_params.h
//
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------
// Constructors
//

sdet_symbolic_edge_linker_params::
sdet_symbolic_edge_linker_params(const sdet_symbolic_edge_linker_params& dp)
  : gevd_param_mixin()
{
  InitParams(dp.nrad_, dp.gap_, dp.badap_uncer_, dp.dx_, dp.dt_, dp.curve_model_type_, dp.token_len_, dp.max_k_, dp.max_gamma_,
             dp.grouping_algo_, dp.cvlet_type_, dp.app_usage_, dp.app_thresh_, dp.max_size_to_group_,
             dp.bFormCompleteCvletMap_, dp.bFormLinkGraph_, dp.b_use_all_cvlets_, dp.linkgraph_algo_,
             dp.min_size_to_link_, dp.linking_algo_, dp.num_link_iters_, dp.bGetfinalcontours_);
}

sdet_symbolic_edge_linker_params::
sdet_symbolic_edge_linker_params(double nrad, double gap, bool adap_uncer,
                                 double dx, double dt, unsigned curve_model,
                                 double token_len, double max_k, double max_gamma,
                                 unsigned grouping_algo, unsigned cvlet_type,
                                 unsigned app_usage, double app_thresh, unsigned max_size_to_group,
                                 bool formCompleteCvletMap, bool formLinkGraph,
                                 bool use_all_cvlet, unsigned linkgraph_algo,
                                 unsigned min_size_to_link, unsigned linking_algo,
                                 unsigned num_link_iters, bool get_final_contours)
{
  InitParams(nrad, gap, adap_uncer, dx, dt, curve_model, token_len, max_k, max_gamma,
             grouping_algo, cvlet_type, app_usage, app_thresh, max_size_to_group,
             formCompleteCvletMap, formLinkGraph, use_all_cvlet, linkgraph_algo,
             min_size_to_link, linking_algo, num_link_iters, get_final_contours);
}

void sdet_symbolic_edge_linker_params::InitParams(double nrad, double gap, bool adap_uncer,
                                                  double dx, double dt, unsigned curve_model,
                                                  double token_len, double max_k, double max_gamma,
                                                  unsigned grouping_algo, unsigned cvlet_type,
                                                  unsigned app_usage, double app_thresh, unsigned max_size_to_group,
                                                  bool formCompleteCvletMap, bool formLinkGraph,
                                                  bool use_all_cvlet, unsigned linkgraph_algo,
                                                  unsigned min_size_to_link, unsigned linking_algo,
                                                  unsigned num_link_iters, bool get_final_contours)
{
  nrad_ = nrad;
  gap_ = gap;
  dx_ = dx;
  dt_ = dt;
  badap_uncer_ = adap_uncer;
  curve_model_type_ = curve_model;
  token_len_ = token_len;
  max_k_ = max_k;
  max_gamma_ = max_gamma;
  grouping_algo_ = grouping_algo;
  max_size_to_group_ = max_size_to_group;
  cvlet_type_ = cvlet_type;
  bFormCompleteCvletMap_ = formCompleteCvletMap;
  bFormLinkGraph_ = formLinkGraph;
  b_use_all_cvlets_ = use_all_cvlet;
  app_usage_ = app_usage;
  app_thresh_ = app_thresh;
  linkgraph_algo_ = linkgraph_algo;
  min_size_to_link_ = min_size_to_link;
  linking_algo_ = linking_algo;
  num_link_iters_ = num_link_iters;
  bGetfinalcontours_ = get_final_contours;

  switch(cvlet_type) //set the grouping flags from the choice of cvlet type
  {
    case 0: //Anchor Centered
      bCentered_grouping_ = true;
      bBidirectional_grouping_ = false;
      break;
    case 1: //Anchor Centered/Bidirectional
      bCentered_grouping_ = true;
      bBidirectional_grouping_ = true;
      break;
    case 2: //Anchor Leading/Bidirectional
      bCentered_grouping_ = false;
      bBidirectional_grouping_ = true;
      break;
    case 3: //ENO Style around Anchor
      bCentered_grouping_ = false;
      bBidirectional_grouping_ = false;
      break;
    default:
      bCentered_grouping_ = true;
      bBidirectional_grouping_ = false;
   }
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool sdet_symbolic_edge_linker_params::SanityCheck()
{
  //TODO
  return true;
}

std::ostream& operator<< (std::ostream& os, const sdet_symbolic_edge_linker_params& dp)
{

  return os << "Radius of neighborhood: " << dp.nrad_ << std::endl
            << "Maximum pixel distance to complete: " << dp.gap_ << std::endl
            << "Curve model: " << dp.curve_model_type_ << std::endl
            << "Local grouping Algorithm: " << dp.grouping_algo_ << std::endl
            << "Curvelet type: " << dp.cvlet_type_ << std::endl
            << "Form linkgraph: " << dp.bFormLinkGraph_ << std::endl
            << "Use all curvelets: " << dp.b_use_all_cvlets_ << std::endl
            << "Extract final contours: " << dp.linkgraph_algo_ << std::endl
            << "Get final contours: " << dp.bGetfinalcontours_ << std::endl;
}
