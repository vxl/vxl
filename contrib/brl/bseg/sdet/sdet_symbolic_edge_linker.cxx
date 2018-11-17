// This is brl/bseg/sdet/sdet_symbolic_edge_linker.cxx
#include <vector>
#include <iostream>
#include <string>
#include "sdet_symbolic_edge_linker.h"
//:
// \file

#include <sdet/sdet_edgemap_sptr.h>
#include <sdet/sdet_sel_sptr.h>
#include <sdet/sdet_sel.h>
#include <sdet/sdet_curve_model.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <vil/vil_image_resource.h>

void sdet_symbolic_edge_linker::apply(const sdet_edgemap_sptr& edgemap) {

  //different types of linkers depending on the curve model
  typedef sdet_sel<sdet_simple_linear_curve_model> sdet_sel_simple_linear;
  typedef sdet_sel<sdet_linear_curve_model> sdet_sel_linear;
  typedef sdet_sel<sdet_CC_curve_model> sdet_sel_CC;
  typedef sdet_sel<sdet_CC_curve_model_new> sdet_sel_CC_new;
  typedef sdet_sel<sdet_CC_curve_model_perturbed> sdet_sel_CC_perturbed;
  typedef sdet_sel<sdet_CC_curve_model_3d> sdet_sel_CC_3d;
  typedef sdet_sel<sdet_ES_curve_model> sdet_sel_ES;
  typedef sdet_sel<sdet_ES_curve_model_perturbed> sdet_sel_ES_perturbed;

  //start the timer
  vul_timer t;

  //construct the linker
  sdet_sel_sptr edge_linker;

  //The curvelet formation parameters
  sdet_curvelet_params cvlet_params(sdet_curve_model::CC,
                                     nrad_, gap_, dt_, dx_, badap_uncer_,
                                     token_len_, max_k_, max_gamma_,
                                     bCentered_grouping_,
                                     bBidirectional_grouping_);

  switch (curve_model_type_)
  {
  case 0: //simple linear_model
    cvlet_params.C_type = sdet_curve_model::LINEAR;
    edge_linker = new sdet_sel_simple_linear(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  case 1: //linear_model
    cvlet_params.C_type = sdet_curve_model::LINEAR;
    edge_linker = new sdet_sel_linear(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  case 2: //CC_model
    cvlet_params.C_type = sdet_curve_model::CC;
    edge_linker = new sdet_sel_CC(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  case 3: //CC_model new
    cvlet_params.C_type = sdet_curve_model::CC2;
    edge_linker = new sdet_sel_CC_new(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  case 4: //CC_model with discrete perturbations
    cvlet_params.C_type = sdet_curve_model::CC;
    edge_linker = new sdet_sel_CC_perturbed(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  case 5: //CC_model 3d bundle
    cvlet_params.C_type = sdet_curve_model::CC3d;
    edge_linker = new sdet_sel_CC_3d(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  case 6: //ES_model
    cvlet_params.C_type = sdet_curve_model::ES;
    edge_linker = new sdet_sel_ES(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  case 7: //ES_model with discrete perturbations
    cvlet_params.C_type = sdet_curve_model::ES;
    edge_linker = new sdet_sel_ES_perturbed(edgemap, CM, ELG, CFG, cvlet_params);
    break;
  }

  //set appearance usage flags
  edge_linker->set_appearance_usage(app_usage_);
  edge_linker->set_appearance_threshold(app_thresh_);

  //perform local edgel grouping
  switch (grouping_algo_)
  {
  case 0: //greedy (depth first grouping)
    edge_linker->build_curvelets_greedy(max_size_to_group_, false);
    break;
  case 1: //extra greedy (depth first grouping)
    edge_linker->build_curvelets_greedy(max_size_to_group_, true);
    break;
  }

  if (bFormCompleteCvletMap_)
    edge_linker->form_full_cvlet_map();

  double group_time = t.real() / 1000.0;
  t.mark();
  std::cout << "Time taken to form groups and cunstruct curvelet map: " << group_time << " sec" << std::endl;

  if (bFormLinkGraph_){
  //form a link graph
    if (b_use_all_cvlets_)
      edge_linker->use_all_curvelets();
    else
      edge_linker->use_anchored_curvelets_only();

    //form the link graph
    edge_linker->construct_the_link_graph(min_size_to_link_, linkgraph_algo_);

    //extract contours
    switch (linking_algo_) {
      case 1: // iteratively
        edge_linker->extract_image_contours_from_the_link_graph(num_link_iters_);
        break;
      case 2: // regular contours
        edge_linker->extract_regular_contours_from_the_link_graph();
        break;
    }
  // By Yuliang Guo, Oct, 2010
    edge_linker->extract_regular_contours_from_the_link_graph();

    if (bGetfinalcontours_){
      edge_linker->Construct_Hypothesis_Tree();
      edge_linker->Disambiguation();
      edge_linker->correct_CFG_topology();
      edge_linker->Post_Process();
    }
    double link_time = t.real() / 1000.0;
    std::cout << "Time taken to link: " << link_time << " sec" << std::endl;
  }
}
