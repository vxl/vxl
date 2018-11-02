// This is brcv/seg/sdet/algo/sdet_sel.h
#ifndef sdet_sel_h
#define sdet_sel_h
//:
//\file
//\brief Templatized Symbolic edge linking alogorithm(new version)
//\author Amir Tamrakar
//\date 03/15/06
//
//\verbatim
//  Modifications
//  Amir Tamrakar 09/05/06   Removed all the other classes to their own files
//  Amir Tamrakar 09/07/06   Templatized this class to enable it to operate on
//                           different curve models
//  Amir Tamrakar 09/11/06   Moved the meat of the algorithm to a new base class
//                           and left all the functions that depend on the template
//                           in this class.
//  Ricardo Fabbri 09/08/09  Various optimizations to the linker
//
//  Caio SOUZA 05/28/2014    Moved implementation of vitual function from sdet_sel.cxx to sdet_sel.h
//                           Made to fix linking error,when the linker/compiler neglected to put the functions
//                           body in place, since there was no code using the virtual functions(yet), and
//                           then failed to create the vtable causing the error.
//\endverbatim

#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include <sdet/sdet_sel_utils.h>
#include <sdet/sdet_sel_base.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A templatized subclass that can work with different curve models
template <class curve_model>
class sdet_sel : public sdet_sel_base
{
public:

  //: constructor
  sdet_sel<curve_model>(sdet_edgemap_sptr edgemap,
                         sdet_curvelet_map& cvlet_map,
                         sdet_edgel_link_graph& edge_link_graph,
                         sdet_curve_fragment_graph& curve_frag_graph,
                         sdet_curvelet_params cvlet_params=sdet_curvelet_params()) :
      sdet_sel_base(edgemap, cvlet_map, edge_link_graph, curve_frag_graph, cvlet_params)
  {
    curvelet_map_.params_.C_type = curve_model().type; //set the right curvelet type
  }

  //: destructor
  ~sdet_sel<curve_model>() override= default;

  //: form a curve hypothesis of the appropriate model given a pair of edgels
  inline curve_model* form_a_hypothesis(sdet_edgel* ref_e, sdet_edgel* e2, bool &ref_first,
      bool forward=true, bool centered=true, bool leading=true)
  {
    // First check for consistency in the appearance information
    if (app_usage_==2){
      if (e2->left_app->dist(*(ref_e->left_app))>app_thresh_ ||
          e2->right_app->dist(*(ref_e->right_app))>app_thresh_)
          return nullptr; //appearance is not consistent
    }

    // Do the dot product test to determine if this pair of edgels can produce a valid hypothesis
    // The two possible hypotheses are:
    //    a) (ref_e->e2)
    //    b) (e2->ref_e)
    //

    const double ref_dir = sdet_vPointPoint(ref_e->pt, e2->pt); //reference dir (ref_e->e2)

    //ref centered grouping
    if (centered) {
      if (sdet_dot(ref_dir, ref_e->tangent)>0) {
        if (forward){
          ref_first = true;
          return new curve_model(ref_e, e2, ref_e, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);
        }
        else {
          ref_first = false;
          return new curve_model(e2, ref_e, ref_e, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);
        }
      }
      else {
        if (forward){
          ref_first = false;
          return new curve_model(e2, ref_e, ref_e, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);
        }
        else {
          ref_first = true;
          return new curve_model(ref_e, e2, ref_e, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);
        }
      }
    }
    else { //not centered
      if (sdet_dot(ref_dir, ref_e->tangent)>0 && forward && leading) {
        ref_first = true;
        return new curve_model(ref_e, e2, ref_e, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);
      }
      if (sdet_dot(ref_dir, ref_e->tangent)<0) {
        if (forward && !leading){
          ref_first = false;
          return new curve_model(e2, ref_e, ref_e, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);
        }
        if (!forward){
          ref_first = true;
          return new curve_model(ref_e, e2, ref_e, dpos_, dtheta_, token_len_, max_k_, max_gamma_, badap_uncer_);
        }
      }
    }
    return nullptr;
  }

  void form_an_edgel_pair(sdet_edgel* ref_e, sdet_edgel* e2) override;
  void form_an_edgel_triplet(sdet_curvelet* /*p1*/, sdet_curvelet* /*p2*/) override{}
  void form_an_edgel_quad(sdet_curvelet* /*t1*/, sdet_curvelet* /*t2*/) override{}
  void build_curvelets_greedy_for_edge(sdet_edgel* eA, unsigned max_size_to_group,
      bool use_flag=false, bool forward=true,  bool centered=true, bool leading=true) override;

  //: check if two curvelets are consistent (is there an intersection in their curve bundles?)
  bool are_curvelets_consistent(sdet_curvelet* cvlet1, sdet_curvelet* cvlet2)
  {
    curve_model *cm1 = static_cast<curve_model *>(cvlet1->curve_model);
    curve_model *cm2 = static_cast<curve_model *>(cvlet2->curve_model);
    curve_model* new_cm = cm1->intersect(cm2);

    bool consistent = new_cm->bundle_is_valid();
    //house cleaning
    delete new_cm;

    return consistent;
  }

  //: form an edgel grouping from an ordered list of edgemap_->edgels
  sdet_curvelet* form_an_edgel_grouping(sdet_edgel* ref_e, std::deque<sdet_edgel*> &edgel_chain,
      bool forward=true,  bool centered=true, bool leading=true) override;
};

// Caio SOUZA - 2014
// Moved implementation of vitual function from sdet_sel.cxx to sdet_sel.h
//
// Made to fix linking error, the linker/compiler neglected to put the functions
// body in place, since there was no code using the virtual functions(yet), and
// then failed to create the vtable causing the error.

//: form an edgel pair (ref_e->e2) or (e2->ref_e)
template <class curve_model>
void sdet_sel<curve_model>::form_an_edgel_pair(sdet_edgel* ref_e, sdet_edgel* e2)
{
  //form a hypothetical curve model from this pair
  bool ref_first;
  curve_model* cm = form_a_hypothesis(ref_e, e2, ref_first);

  //if model is valid, form an edgel pair curvelet
  if (cm){
    sdet_curvelet* c1 = new sdet_curvelet(ref_e, cm);
    if (ref_first){
      c1->push_back(ref_e);
      c1->push_back(e2);
    }
    else {
      c1->push_back(e2);
      c1->push_back(ref_e);
    }

    //make links to the edgel-pair from the ref edgel
    curvelet_map_.add_curvelet(c1);
  }
}

//: form curvelets around the given edgel in a greedy fashion
template <class curve_model>
void sdet_sel<curve_model>::build_curvelets_greedy_for_edge(sdet_edgel* eA, unsigned max_size_to_group, bool use_flag,
                                bool forward, bool centered, bool leading)
{
  // 1) construct a structure to temporarily hold the pairwise-hypotheses
  std::vector<sel_hyp> eA_hyps;

  //get the grid coordinates of this edgel
  unsigned const ii = sdet_round(eA->pt.x());
  unsigned const jj = sdet_round(eA->pt.y());

  unsigned const rad_sqr = rad_*rad_;

  // 2) iterate over the neighboring cells around this edgel
  for (int xx=(int)ii-(int)nrad_; xx<=(int)(ii+nrad_) ; xx++){
    for (int yy=(int)jj-(int)nrad_; yy<=(int)(jj+nrad_) ; yy++){

      if (xx<0 || xx>=(int)ncols_ || yy<0 || yy>=(int)nrows_)
        continue;

      //for all the edgels in its neighborhood
      for (auto eB : edgemap_->cell(xx, yy)){
        if (eB == eA) continue;

        // 3) do a better check of circular radius because the bucketing neighborhood is very coarse
        if ( sqr_length(eA->pt - eB->pt) > rad_sqr) continue;

        // 4) form pair-wise hypotheses
        bool ref_first;
        curve_model* cm = form_a_hypothesis(eA, eB, ref_first, forward, centered, leading);

        if (cm){
          if (cm->bundle_is_valid()){ //if legal, record the hypothesis
            sel_hyp cur_hyp;

            cur_hyp.eN = eB;
            cur_hyp.cm = cm;
            cur_hyp.ref_first = ref_first;
            cur_hyp.d = vgl_distance(eA->pt, eB->pt);
            cur_hyp.flag = false;

            eA_hyps.push_back(cur_hyp);
          }
          else
            delete cm;
        }
      }
    }
  }

  // 5) first sort the pair-wise hyps by distance
  std::sort(eA_hyps.begin(), eA_hyps.end(), comp_dist_hyps_less);

  // 6) for each pair-wise hyps formed by this edgel
  for (unsigned h1=0; h1<eA_hyps.size(); h1++)
  {
    //this edgel has been used up already (only if we are using flags)
    if (use_flag && eA_hyps[h1].flag) continue;

    //for the hybrid algorithm (to avoid edgel skips within contour)
    if (use_hybrid_){ if (cId_[eA->id]==cId_[eA_hyps[h1].eN->id]) continue;}

    // 7) initialize a new edgel chain that will grow in a greedy depth-first fashion
    std::deque<sdet_edgel*> cur_edgel_chain; //chain can grow either way

    //insert ref edgel first
    cur_edgel_chain.push_back(eA);

    // 8)initialize the shrinking curve bundle from the cur hypothesis
    curve_model* cur_cm = static_cast<curve_model *>(eA_hyps[h1].cm);

    // 9) attempt to integrate the other pair-wise hypotheses (DEPTH-FIRST SEARCH)
    for (unsigned h2=0; h2<eA_hyps.size(); h2++)
    {
      if (h2==h1){
        //add the second edgel of the reference pair (no need for any computation)
        if (eA_hyps[h1].ref_first) cur_edgel_chain.push_back(eA_hyps[h1].eN);
        else                       cur_edgel_chain.push_front(eA_hyps[h1].eN);

        // check the size of the grouping
        if (cur_edgel_chain.size() >= max_size_to_group)
          break;
        else
          continue;
      }

      // 10) for the others we need to check for consistency with the current hypothesis
      //     (i.e., compute intersection of curve bundles)
      curve_model* new_cm = cur_cm->intersect(static_cast<curve_model *>(eA_hyps[h2].cm));

      // 11) if the intersection is valid, we can add this edgel to the grouping
      if (new_cm->bundle_is_valid())
      {
        //reassign the curve bundle for the growing grouping
        if (cur_cm != eA_hyps[h1].cm)
          delete cur_cm;
        cur_cm = new_cm;

        // 12) add the new edgel to the growing edgel chain
        if (eA_hyps[h2].ref_first) cur_edgel_chain.push_back(eA_hyps[h2].eN);
        else                       cur_edgel_chain.push_front(eA_hyps[h2].eN);

        //flag this hypothesis
        eA_hyps[h2].flag = true;
      }
      else
        delete new_cm; //delete this cb because it is not needed

      // 13) check the size of the grouping
      if (cur_edgel_chain.size() >= max_size_to_group)
        break;

    }

    // 14) form a new curvelet and assign the curve bundle and the edgel chain to it...
    //     ...if it passes a few tests :
    //                             (a) if it doesn't already exist and
    //                             (b) its a reasonable fit and
    //                             (c) its relatively symmetric
    if (cur_edgel_chain.size()>2 &&
        !curvelet_map_.does_curvelet_exist(eA, cur_edgel_chain) &&
        cur_cm->curve_fit_is_reasonable(cur_edgel_chain, eA, dpos_))// &&
        //curvelet_is_balanced(eA, cur_edgel_chain))
    {
      sdet_curvelet* new_cvlet = new sdet_curvelet(eA, cur_cm, cur_edgel_chain, forward);

      //compute curvelet quality
      new_cvlet->compute_properties(rad_, token_len_);

      //add links from the ref edgel to this curvelet
      curvelet_map_.add_curvelet(new_cvlet);
    }
    else {
      //delete cur_cm if no curvelet is formed
      if (cur_cm != eA_hyps[h1].cm)
        delete cur_cm;
    }
  }

  // 15) now delete all the hyps for this edgel, before moving on to a new edgel
  for (auto & eA_hyp : eA_hyps)
    delete eA_hyp.cm;

  eA_hyps.clear();
}

//: form an edgel grouping from an ordered list of edgels
template <class curve_model>
sdet_curvelet*
sdet_sel<curve_model>::form_an_edgel_grouping(sdet_edgel* ref_e,
    std::deque<sdet_edgel*> &edgel_chain,
    bool forward,  bool centered, bool leading)
{
  //1) Go over the edgels in the chain and attempt to form a curvelet from it
  curve_model* chain_cm=nullptr;

  // 2) form an ordered list based on distance from ref_edgel
  std::map<double, unsigned > dist_order; // ordered list of edgels (closest to furthest)
  for (unsigned i=0; i< edgel_chain.size(); i++)
  {
    if (edgel_chain[i] == ref_e)   continue;
    dist_order.insert(std::pair<double, unsigned>(vgl_distance(ref_e->pt, edgel_chain[i]->pt), i));
  }

  std::map<double, unsigned >::iterator it = dist_order.begin();
  for (; it!=dist_order.end(); it++)
  {
    sdet_edgel* cur_e = edgel_chain[it->second];

    //form a pairwise curve bundle between the current edgel and the ref edgel
    bool ref_first; //this is not used here
    curve_model* cm = form_a_hypothesis(ref_e, cur_e, ref_first, forward, centered, leading);

    //if the bundle is legal intersect it with the current group's bundle
    if (cm && cm->bundle_is_valid())
    {
      //if this is the first pair, record it as the bundle of the grouping
      if (!chain_cm)
        chain_cm = cm;
      else {
        //intersect it with the existing bundle
        curve_model* new_cm = chain_cm->intersect(cm);
        delete cm; //no use for this anymore

        //if this intersection is valid, make it the chain's curve bundle
        if (new_cm->bundle_is_valid()){
          delete chain_cm;   //no use for this since it will be replaced
          chain_cm = new_cm; //replace it
        }
        else {
          //delete any existing curve bundles
          delete new_cm;

          if (chain_cm)  delete chain_cm;
          chain_cm = nullptr;

          break; //break the for loop (no curve bundle possible)
        }
      }
    }
    else {
      // the entire curvelet is invalid if anyone pairwise curve bundle is invalid
      //delete any existing curve bundles
      if (cm)        delete cm;
      if (chain_cm)  delete chain_cm;
      chain_cm = nullptr;

      break; //break the for loop (no curve bundle possible)
    }
  }

  sdet_curvelet* new_cvlet=nullptr;
  //if all the pairwise bundles intersect,
  //form a new curvelet and assign the curve bundle and the edgel chain to it...
  if (chain_cm)
  {
    chain_cm->curve_fit_is_reasonable(edgel_chain, ref_e, dpos_); //this is just to find the centroid

    new_cvlet = new sdet_curvelet(ref_e, chain_cm, forward);

    //add edgels
    for (auto & i : edgel_chain)
      new_cvlet->push_back(i);

    //compute curvelet quality
    new_cvlet->compute_properties(rad_, token_len_);

    //Note:: since this curvelet could be temporary, don't add it to the edgel's list of curvelets
    //curvelet_map_.add_curvelet(new_cvlet);
  }

  return new_cvlet;
}

#endif // sdet_sel_h
