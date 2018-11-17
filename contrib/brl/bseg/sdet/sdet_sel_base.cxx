#include <iostream>
#include <fstream>
#include <deque>
#include <map>
#include <set>
#include <algorithm>
#include "sdet_sel_base.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//#include <mbl/mbl_stats_1d.h>

#include "sdet_edgemap.h"
// remove dependency on mul
#include <vnl/vnl_vector.h>
static void calc_mean_var(double& mean, double& var,
                          const double* d, int n){
    double sum=0;
    double sum2 = 0;
    for (int i=0;i<n;++i)
      {
        sum+=d[i];
        sum2+=d[i]*d[i];
      }

    mean = sum/n;
    var  = (sum2 - n*mean*mean)/(n-1);
}
static void calc_mean_var(double& mean, double& var,
                          const vnl_vector<double>& d){
  calc_mean_var(mean,var,d.data_block(),d.size());
}
//: Constructor
sdet_sel_base
::sdet_sel_base(const sdet_edgemap_sptr& edgemap,
                sdet_curvelet_map& cvlet_map,
                sdet_edgel_link_graph& edge_link_graph,
                sdet_curve_fragment_graph& curve_frag_graph,
                sdet_curvelet_params cvlet_params) :
  edgemap_(edgemap),
  curvelet_map_(cvlet_map),
  edge_link_graph_(edge_link_graph),
  curve_frag_graph_(curve_frag_graph),
  nrows_(edgemap->height()),
  ncols_(edgemap->width()),
  app_usage_(0),
  app_thresh_(2),
  rad_(cvlet_params.rad_),
  dtheta_(cvlet_params.dtheta_*vnl_math::pi/180),
  dpos_(cvlet_params.dpos_),
  badap_uncer_(cvlet_params.badap_uncer_),
  token_len_(cvlet_params.token_len_),
  max_k_(cvlet_params.max_k_),
  max_gamma_(cvlet_params.max_gamma_),
  nrad_((unsigned) std::ceil(rad_)+1),
  gap_(cvlet_params.gap_),
  maxN_(2*nrad_),
  centered_(cvlet_params.centered_),
  bidir_(cvlet_params.bidirectional_),
  use_anchored_curvelets_(true),
  min_deg_to_link_(4),
  use_hybrid_(false),
  DHT_mode_(true),
  propagate_constraints(true)
{
  //save the parameters in the curvelet map
  curvelet_map_.set_edgemap(edgemap);
  curvelet_map_.set_parameters(cvlet_params);
}

//:destructor
sdet_sel_base
::~sdet_sel_base()
= default;

//********************************************************************//
// User Friendly functions
//********************************************************************//

//: use the recommended sub-algorithms to extract final contour set (naive users should call this function)
void
sdet_sel_base
::extract_image_contours()
{
  //1) perform local edgel grouping
  set_appearance_usage(0); //do not use
  set_appearance_threshold(0.2);

  build_curvelets_greedy(2*nrad_, false, true, true); //greedy (depth first grouping)

  //2) form the link graph
  use_anchored_curvelets_only();
  construct_the_link_graph(4, 0);

  //extract contours
  extract_regular_contours_from_the_link_graph();

  //extract_image_contours_from_the_link_graph(1);

}

//: group pairs of edgels into curvelets
void
sdet_sel_base
::build_pairs()
{
  std::cout << "Building pairs ...";
  std::cout.flush();

  //std::ofstream outfp("pair_distribution.txt", std::ios::out);

  //form pairs from the edgels in the local neighborhood
  for (unsigned i=0; i<edgemap_->edgels.size(); i++){
    sdet_edgel* eA = edgemap_->edgels[i];

    //get the grid coordinates of this edgel
    unsigned ii = sdet_round(eA->pt.x());
    unsigned jj = sdet_round(eA->pt.y());

    int cnt = 0; //count the # of neighboring edgels

    //iterate over the cell neighborhoods around this edgel that contains the full edgel neighborhood
    for (int xx=(int)ii-(int)nrad_; xx<=(int)(ii+nrad_) ; xx++){
      for (int yy=(int)jj-(int)nrad_; yy<=(int)(jj+nrad_) ; yy++){

        if (xx<0 || xx>=(int)ncols_ || yy<0 || yy>=(int)nrows_)
          continue;

        unsigned N = edgemap_->cell(xx, yy).size();
        for (unsigned k=0; k<N; k++){
          sdet_edgel* eB = edgemap_->cell(xx, yy)[k];
          if (eB == eA) continue;

          cnt++;

          // form edgel pair eA->eB.
          // there is no need to form eB->eA because this will be done by eB
          form_an_edgel_pair(eA,eB);
        }
      }
    }
    // save the neighboring edge count
    //outfp << cnt <<std::endl;
  }

  //close file
  //outfp.close();

  std::cout << "done!" << std::endl;
}


//: form curvelets around each edgel in a greedy fashion
void
sdet_sel_base
::build_curvelets_greedy(unsigned max_size_to_group,bool use_flag, bool clear_existing, bool verbose)
{
  if (verbose){
    std::cout << "Building All Possible Curvelets (Greedy) ..." ;
    std::cout.flush();
  }

  if (clear_existing){
    //clear the curvelet map
    curvelet_map_.clear();

    //form a new curvelet map
    curvelet_map_.resize(edgemap_->num_edgels());

    if (verbose){
      std::cout << "curvelet map cleared..." ;
      std::cout.flush();
    }
  }

  //store this parameter
  maxN_ = max_size_to_group;

  if (centered_) {
    if (bidir_) {
      for (auto eA : edgemap_->edgels) {
        // centered_ && bidir_
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, false); //first in the forward direction
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, false, centered_, false); //then in the other direction
      }
    } else {
      for (auto eA : edgemap_->edgels) {
        // centered_ && !bidir_
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, false); //first in the forward direction
      }
    }
  } else {
    if (bidir_) {
      for (auto eA : edgemap_->edgels) {
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, true); //forward half
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, false, centered_, true); //backward half
        // !centered_ && bidir_
      }
    } else {
      for (auto eA : edgemap_->edgels) {
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, true); //forward half
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, false); //ENO style forward
        // !centered_ && !bidir_
      }
    }
  }

  if (verbose)
    std::cout << "done!" << std::endl;
}


//: form the full curvelet map (curvelet map lists all the curvelets it participated in and not just the ones anchored to it)
void
sdet_sel_base
::form_full_cvlet_map()
{
  // This method forms a curvelet map which is a mapping from each edgel to all the
  // curvelets it participates in and not just the ones anchored to it

  for (unsigned i=0; i<edgemap_->edgels.size(); i++){
    sdet_edgel* eA = edgemap_->edgels[i];

    //add all the curvelets anchored at this edgel to all the other edgels in it
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      sdet_curvelet* cvlet = (*cv_it);

      //only add the ones that are anchored to this edgel
      if (cvlet->ref_edgel != eA)
        continue;

      //add this curvelet to each of the edgels of the grouping
      for (unsigned n=0; n<cvlet->edgel_chain.size(); n++){
        sdet_edgel* eB = cvlet->edgel_chain[n];

        //make sure that there are no duplicates (this is not strictly necessary)
        bool cvlet_exists = false; //reset flag

        //go over all the curvelets (not just anchored) formed by the current edgel
        auto cv_it2 = curvelet_map_.curvelets(eB->id).begin();
        for ( ; cv_it2!=curvelet_map_.curvelets(eB->id).end(); cv_it2++){
          sdet_curvelet* cvlet2 = (*cv_it2);

          if (cvlet2==cvlet){
            cvlet_exists=true;
            break;
          }

          if (cvlet2->edgel_chain.size() != cvlet->edgel_chain.size())
            continue;

          bool exists = true;
          for (unsigned k=0; k<cvlet2->edgel_chain.size(); k++)
            exists = exists && (cvlet2->edgel_chain[k]==cvlet->edgel_chain[k]);

          //the flag will remain true only if all the edgels match
          if (exists){
            cvlet_exists= true;
            break;
          }
        }

        if (!cvlet_exists)
          curvelet_map_.curvelets(eB->id).push_back((*cv_it)); //insert this into the map

      }
    }
  }

}

//: check to see if curvelets are balanced
bool
sdet_sel_base
::curvelet_is_balanced(sdet_edgel* ref_e, std::deque<sdet_edgel*> &edgel_chain)
{
  //looks like this is one of the qualitites we need of an edgel grouping before it can qualify to be a curvelet

  //find out the # of edgels before and after the reference edgel
  //also find out the length before and after the reference edgel
  int num_before=0, num_after=0;
  double Lm=0, Lp=0;

  bool before_ref = true;
  for (unsigned i=0; i<edgel_chain.size()-1; i++){
    if (before_ref) { Lm += vgl_distance(edgel_chain[i]->pt, edgel_chain[i+1]->pt); num_before++; }
    else            { Lp += vgl_distance(edgel_chain[i]->pt, edgel_chain[i+1]->pt); num_after++; }

    if (edgel_chain[i+1]==ref_e)
      before_ref = false;
  }

  //simple measure of balance.
  if (num_before<1 || num_after<1)
    return false;
  else
    return true;
}

void
sdet_sel_base
::recompute_curvelet_quality()
{
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++)
      (*cv_it)->compute_properties(rad_, token_len_);
  }
}

//: prune the curvelets with gaps larger than the one specified
void
sdet_sel_base
::prune_curvelets_by_gaps(double gap_threshold)
{
  //go through the curvelet map and remove all the curvelets below threshold

  //1) first clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over the curvelets in the curvelet map
  std::vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      sdet_curvelet* cvlet = (*cv_it);

      //determine largest gap
      for (unsigned j=0; j<cvlet->edgel_chain.size()-1; j++){
        if (vgl_distance(cvlet->edgel_chain[j]->pt, cvlet->edgel_chain[j+1]->pt)>gap_threshold){
          cvlets_to_del.push_back(cvlet);
          break;
        }
      }
    }
  }

  //now actually delete them
  for (auto & i : cvlets_to_del)
    curvelet_map_.remove_curvelet(i);
  cvlets_to_del.clear();
}

//: prune the curvelets with lengths (extent) smaller than the one specified
void
sdet_sel_base
::prune_curvelets_by_length(double length_threshold)
{
  //go through the curvelet map and remove all the curvelets below threshold

  //1) first clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over the curvelets in the curvelet map
  std::vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      sdet_curvelet* cvlet = (*cv_it);

      if (cvlet->length<length_threshold)
        cvlets_to_del.push_back(cvlet);
    }
  }

  //now actually delete them
  for (auto & i : cvlets_to_del)
    curvelet_map_.remove_curvelet(i);
  cvlets_to_del.clear();
}

//: prune the curvelets that are below the quality threshold and hence considered spurious
void
sdet_sel_base
::prune_the_curvelets(double quality_threshold)
{
  //go through the curvelet map and remove all the curvelets below threshold

  //1) first clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over the curvelets in the curvelet map
  std::vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      if ((*cv_it)->quality<quality_threshold)
        cvlets_to_del.push_back(*cv_it);
    }
  }

  //now actually delete them
  for (auto & i : cvlets_to_del)
    curvelet_map_.remove_curvelet(i);
  cvlets_to_del.clear();
}

//: prne the curvelets that are not locally geometrically consistent (i.e., c1)
void
sdet_sel_base
::prune_curvelets_by_c1_condition()
{
  //Note: this is currently only meaningful for GENO style curvelets
  //goal: to locally do a test of viability beyond the local neighborhood by looking for viability across
  //two neighborhoods using the c1 test (or biarc condition)

  //1) first clear the link graph so that new one can be formed later
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over all the edgels and look at the curvelets on it
  std::vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    //for each cvlet before the edgel, see if a c1 cvlet can be found after it
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++)
    {
      sdet_curvelet* cvlet1 = (*cv_it);
      bool before = (cvlet1->edgel_chain.back()==eA);
      bool c1_pair_found = false;
      bool cvlet2_found = false;

      //go over all the cvlet after
      auto cv_it2 = curvelet_map_.curvelets(i).begin();
      for ( ; cv_it2!=curvelet_map_.curvelets(i).end(); cv_it2++)
      {
        sdet_curvelet* cvlet2 = (*cv_it2);
        if (cvlet2 == cvlet1) continue;
        cvlet2_found = true;

        //do the c1 test
        if ( (before && cvlet2->edgel_chain.front()==eA) ||
            (!before && cvlet2->edgel_chain.back()==eA) )
          c1_pair_found = c1_pair_found || cvlet1->curve_model->is_C1_with(cvlet2->curve_model);
      }

      if (cvlet2_found && !c1_pair_found) //mark for deletion
        cvlets_to_del.push_back(cvlet1);
    }
  }

  //now actually delete them
  for (auto & i : cvlets_to_del)
    curvelet_map_.remove_curvelet(i);
  cvlets_to_del.clear();

}

//: construct a simple link grpah by connectng edgels to all its neighbors
void
sdet_sel_base
::construct_naive_link_graph(double proximity_threshold, double affinity_threshold)
{
  //1) clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  auto R = (unsigned) std::ceil(proximity_threshold);

  // 2a) go over all the curvelets and reset the used flags
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    //get the grid coordinates of this edgel
    unsigned ii = sdet_round(eA->pt.x());
    unsigned jj = sdet_round(eA->pt.y());

    // 2) iterate over the neighboring cells around this edgel
    for (int xx=(int)ii-(int)R; xx<=(int)(ii+R) ; xx++){
      for (int yy=(int)jj-(int)R; yy<=(int)(jj+R) ; yy++){

        if (xx<0 || xx>=(int)ncols_ || yy<0 || yy>=(int)nrows_)
          continue;

        //for all the edgels in its neighborhood
        for (auto eB : edgemap_->cell(xx, yy)){
          if (eB == eA) continue;

          //form a link between eA and eB (if affinity is high)

          //compute pairwise affinity

          //determine the intrinsic parameters for this edgel pair
          sdet_int_params params = sdet_get_intrinsic_params(eA->pt, eB->pt, eA->tangent, eB->tangent);

          double kk, gamma, len;
          double k0_max_error, gamma_max_error, len_max_error; //other params (unimportant)
          // read the ES solutions from the table and scale appropriately
          bvgl_eulerspiral_lookup_table::instance()->look_up( params.t1, params.t2,
                                                              &kk, &gamma, &len,
                                                              &k0_max_error, &gamma_max_error, &len_max_error );
          kk = kk/params.d; gamma= gamma/(params.d*params.d);

          //some energy function
          double E = gamma*gamma*len;

          //threshold using a simple energy function
          if (E<affinity_threshold)
            edge_link_graph_.link(eA, eB, nullptr);
        }
      }
    }
  }
}

//: form the link graph from the existing edgel groupings
// method = 0 : include all the curvelets
// method = 1 : curve model consistency
// method = 2 : reciprocal immediate links only
// method = 3 : immediate links only
void
sdet_sel_base
::construct_the_link_graph(unsigned min_group_size, int method)
{
  //1) clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());
  if (!use_anchored_curvelets_){
    // First update the curvelet map to include all links
    form_full_cvlet_map();
  }

  // 2) now construct the link graph from the curvelet map
  std::cout << "Constructing the Link Graph using (N >= " << min_group_size << ")..." ;

  // 2a) go over all the curvelets and reset the used flags
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    //for all curvelets that are larger than the group size threshold
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++)
      (*cv_it)->used = false; //reset this flag
  }

  // 2b) go over all the curvelets above the min size and determine which links can be formed
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    //for all curvelets that are larger than the group size threshold
    auto cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      sdet_curvelet* cvlet = (*cv_it);
      if (cvlet->order() < min_group_size) continue;

      //form all possible links from this curvelet
      form_links_from_a_curvelet(eA, cvlet, min_group_size, method);
    }
  }
  std::cout << "done!" << std::endl;

  ////3) prune duplicate curvelets (Assuming that once the curvelets are formed, they are independent)
  //for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++){
  //  //all child links of each edgel covers all the links
  //  sdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
  //  for (; l_it != edge_link_graph_.cLinks[i].end(); l_it++){
  //    (*l_it)->prune_redundant_curvelets();
  //  }
  //}

  //4) after forming the link graph, determine the degree of overlap of the links in the graph
  for (auto & cLink : edge_link_graph_.cLinks){
    //all child links of each edgel covers all the links
    auto l_it = cLink.begin();
    for (; l_it != cLink.end(); l_it++){
      (*l_it)->deg_overlap = count_degree_overlap((*l_it));
    }
  }

}

//: count the degree of overlap between pairs of curvelets at a link
int
sdet_sel_base
::count_degree_overlap(sdet_link* link)
{
  int max_deg = 0;

  //go over all pairs of curvelets causing the link
  auto c_it = link->curvelets.begin();
  for (; c_it != link->curvelets.end(); c_it++){
    auto c_it2 = c_it;
    c_it2++;
    for (; c_it2 != link->curvelets.end(); c_it2++)
    {
      unsigned k1=0, k2=0;

      //count the degree of overlap between these two curvelets
      for (unsigned i=0; i<(*c_it)->edgel_chain.size(); i++){
        if ((*c_it)->edgel_chain[i]==link->pe){
          k1 = i;
          break;
        }
      }

      for (unsigned i=0; i<(*c_it2)->edgel_chain.size(); i++){
        if ((*c_it2)->edgel_chain[i]==link->pe){
          k2 = i;
          break;
        }
      }

      int deg = count_degree_overlap((*c_it), (*c_it2), k1, k2);
      if (deg>max_deg)
        max_deg = deg;
    }
  }
  return max_deg;
}

//: count the degree of overlap between two curvelets
int
sdet_sel_base
::count_degree_overlap(sdet_curvelet* cvlet1, sdet_curvelet* cvlet2, unsigned k1, unsigned k2)
{
  //ks are the indices into the edgel chain of the cvlets
  int cnt_overlap=0;

  //count the overlaps before the link
  int kk1=k1-1;
  int kk2=k2-1;
  bool continuous = true;
  for (; kk1>=0 && kk2>=0 && continuous; kk1--, kk2--){
    if (cvlet1->edgel_chain[kk1]==cvlet2->edgel_chain[kk2])
      cnt_overlap++;
    else
      continuous = false;
  }

  //count the overlaps after the link
  kk1=k1+2; kk2=k2+2;
  continuous = true;
  for (; kk1<(int)cvlet1->edgel_chain.size() && kk2<(int)cvlet2->edgel_chain.size() && continuous; kk1++, kk2++){
    if (cvlet1->edgel_chain[kk1]==cvlet2->edgel_chain[kk2])
      cnt_overlap++;
    else
      continuous = false;
  }

  return cnt_overlap;
}

//: form all appropriate links from a curvelet
void sdet_sel_base::form_links_from_a_curvelet(sdet_edgel* eA, sdet_curvelet* cvlet, unsigned min_group_size, int method)
{
  if (method ==0)
  {
    //Link all edgels in the group
    //
    // Explanation:
    // if xyzAbcd then X-Y, Y-Z, Z-A, A-B, B-C and C-D

    //Amir: new feature
    //      since some curvelets are constructed in the reverse direction and the link graph is a directed graph,
    //      we ought to either make the link graph an undirected graph or add the links from the reverse curvelet in reverse
      for (unsigned k=0; k<cvlet->edgel_chain.size()-1; k++)
       edge_link_graph_.link(cvlet->edgel_chain[k], cvlet->edgel_chain[k+1], cvlet);
    //all cvlets are used
    cvlet->used = true;
  }
  else if (method==1)
  {
    //Link immediate edgels only (only the ones it's directly connected to)
    //
    // Explanation:
    // if -zAb- then Z-A and A-B

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (k>0 && cvlet->edgel_chain[k-1]==eA) //the link after it (eA --> edgel_chain[k])
        edge_link_graph_.link(eA, cvlet->edgel_chain[k], cvlet);

      if (k<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[k+1]==eA) //the link before it (edgel_chain[k] --> eA)
        edge_link_graph_.link(cvlet->edgel_chain[k], eA, cvlet);
    }

    //all cvlets are used
    cvlet->used = true;
  }
  else if (method==2)
  {
    // Link immediate reciprocal edgels only
    //
    // Explanation: (caps mean anchored)
    // (1)  -Ab- + -aB- = A-B

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (k>0 && cvlet->edgel_chain[k-1]==eA) //the link after it (eA --> edgel_chain[k])
      {
        if (link_is_reciprocal(eA, cvlet->edgel_chain[k], eA, min_group_size)){
          edge_link_graph_.link(eA, cvlet->edgel_chain[k], cvlet);
          cvlet->used = true;
        }
      }
      if (k<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[k+1]==eA) //the link before it (edgel_chain[k] --> eA)
      {
        if (link_is_reciprocal(cvlet->edgel_chain[k], eA, eA, min_group_size)){
          edge_link_graph_.link(cvlet->edgel_chain[k], eA, cvlet);
          cvlet->used = true;
        }
      }
    }
  }
  else if (method==3)
  {
    // Link immediate reciprocal links if they are supported
    //
    // Explanation: (caps mean anchored)
    // (1)  -Abc- + -aBc- = A-B (if A is a terminal edgel)
    // (2)  -zAb- + -zaB- = A-B (if B is a terminal edgel)
    // (3)  -zAbc- + -zaBc- = A-B

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      sdet_edgel *eX=nullptr, *eY=nullptr;
      if (k>0 && cvlet->edgel_chain[k-1]==eA) //the link after it (eA --> edgel_chain[k])
      {
        if (k>1) eX = cvlet->edgel_chain[k-2];
        if (k<cvlet->edgel_chain.size()-1) eY=cvlet->edgel_chain[k+1];

        if (link_is_supported(eX, eA, cvlet->edgel_chain[k], eY, eA, min_group_size)){
          edge_link_graph_.link(eA, cvlet->edgel_chain[k], cvlet);
          cvlet->used = true;
        }
      }
      if (k<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[k+1]==eA) //the link before it (edgel_chain[k] --> eA)
      {
        if (k>0) eX = cvlet->edgel_chain[k-1];
        if (k<cvlet->edgel_chain.size()-2) eY=cvlet->edgel_chain[k+2];

        if (link_is_supported(eX, cvlet->edgel_chain[k], eA, eY, eA, min_group_size)){
          edge_link_graph_.link(cvlet->edgel_chain[k], eA, cvlet);
          cvlet->used = true;
        }
      }
    }
  }
  else if (method==4)
  {
    // Put a triplet overlap condition
    //
    // Basic Idea:
    //
    //   Every edgel gets to pick its neighbors (before and after, in one shot)
    //   But they have to be supported by the curvelets of the neighbors.
    //
    // Explanation: (caps mean anchored)
    // -Abc- + -aBc- + abC- = A-B-C
    //

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++)
    {
      sdet_edgel *eX=nullptr, *eY=nullptr;
      if (k<cvlet->edgel_chain.size()-2 && cvlet->edgel_chain[k+1]==eA) //the link before it (edgel_chain[k] --> eA)
      {
        eX = cvlet->edgel_chain[k];
        eY = cvlet->edgel_chain[k+2];

        if (triplet_is_supported(eX, eA, eY, min_group_size)){
          edge_link_graph_.link(eX, eA, cvlet);
          edge_link_graph_.link(eA, eY, cvlet);
          cvlet->used = true;
        }
      }
    }
  }

}

//: check to see if link between two edges is reciprocal (i.e., x-A->b-y && p-a->B-q)
bool
sdet_sel_base
::link_is_reciprocal(sdet_edgel* eA, sdet_edgel* eB, unsigned min_group_size)
{
  bool link_found = false;

  //for all curvelets of eA that are larger than the group size threshold
  auto cv_it = curvelet_map_.curvelets(eA->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eA->id).end() && !link_found; cv_it++)
  {
    sdet_curvelet* cvlet = (*cv_it);

    if (cvlet->order()<min_group_size)
      continue;

    //check if eA is directly connected to eB
    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (cvlet->edgel_chain[k]==eA && (k<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[k+1]==eB)){
        link_found = true;
        break;
      }
    }
  }

  if (!link_found)
    return false;

  //for all curvelets of eB that are larger than the group size threshold
  link_found = false;
  cv_it = curvelet_map_.curvelets(eB->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eB->id).end() && !link_found; cv_it++)
  {
    sdet_curvelet* cvlet = (*cv_it);

    if (cvlet->order()<min_group_size)
      continue;

    //check if eA is directly connected to eB
    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (cvlet->edgel_chain[k]==eB && (k>0 && cvlet->edgel_chain[k-1]==eA)){
        link_found = true;
        break;
      }
    }
  }

  return link_found; //link found in both directions
}

//: check to see if link between two edges is reciprocal (i.e., x-A-b-y && p-a-B-q)
bool
sdet_sel_base
::link_is_reciprocal(sdet_edgel* eA, sdet_edgel* eB, sdet_edgel* ref_e, unsigned min_group_size)
{
  sdet_edgel *eC, *eN; //current edgel and neighboring edgel

  if (ref_e == eA) { eC = eA; eN = eB; }
  else             { eC = eB; eN = eA; }

  //we know eC-->eN, now look for eN-->eC link

  //for all curvelets that are larger than the group size threshold
  auto cv_it = curvelet_map_.curvelets(eN->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eN->id).end(); cv_it++)
  {
    sdet_curvelet* cvlet = (*cv_it);

    if (cvlet->order()<min_group_size)
      continue;

    //check if eN is directly connected to eC
    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      if (cvlet->edgel_chain[k]==eN && ((k>0 && cvlet->edgel_chain[k-1]==eC) ||
                                        (k<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[k+1]==eC))
         )
      {
        cvlet->used = true;
        return true;
      }
    }
  }

  return false;
}

//: check to see if link is reciprocal and supported by other edgels  (i.e., x-A->b-y && x-a->B-y)
bool
sdet_sel_base
::link_is_supported(sdet_edgel* eA, sdet_edgel* eB, unsigned min_group_size)
{
  bool link_found = false;

  //go over all curvelets of eA and find the ones that contain eB
  auto cv_it = curvelet_map_.curvelets(eA->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eA->id).end() && !link_found; cv_it++)
  {
    sdet_curvelet* cvlet = (*cv_it);

    if (cvlet->order()<min_group_size)
      continue;

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      sdet_edgel *eX=nullptr, *eY=nullptr;
      if (k<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[k]==eA && cvlet->edgel_chain[k+1]==eB)
      {
        if (k>0) eX = cvlet->edgel_chain[k-1];
        if (k<cvlet->edgel_chain.size()-2) eY=cvlet->edgel_chain[k+2];

        link_found = link_is_supported(eX, eA, eB, eY, eA, min_group_size);

        if (link_found)
          break;
      }
    }
  }

  return link_found;
}


//: check to see if link is reciprocal and supported by other edgels  (i.e., x-A->b-y && x-a->B-y)
bool
sdet_sel_base
::link_is_supported(sdet_edgel* eX, sdet_edgel* eA, sdet_edgel* eB, sdet_edgel* eY,
                                       sdet_edgel* ref_e, unsigned min_group_size)
{
  sdet_edgel *eC, *eN; //current edgel and neighboring edgel

  if (ref_e == eA) { eC = eA; eN = eB; }
  else             { eC = eB; eN = eA; }

  // we know eC-->eN, now we need to verify that for those curvelets which have the eN-->eC link,
  // they are supported by the same edgels (i.e., that eX-eC->eN-eY and eX-eC<-eN-eY both exist)

  //for all curvelets of eN that are larger than the group size threshold
  auto cv_it = curvelet_map_.curvelets(eN->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eN->id).end(); cv_it++)
  {
    sdet_curvelet* cvlet = (*cv_it);

    if (cvlet->order()<min_group_size)
      continue;

    //check if eN is directly connected to eC
    for (unsigned j=0; j<cvlet->edgel_chain.size(); j++)
    {
      //eN-eC <=> eA-eB
      if (cvlet->edgel_chain[j]==eN && j<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[j+1]==eC)
      {
        assert (eA==eN && eB==eC); //just making sure
        if (eX && eY && j>0 && cvlet->edgel_chain[j-1]==eX && j<cvlet->edgel_chain.size()-2 && cvlet->edgel_chain[j+2]==eY){
          cvlet->used = true;
          return true; //found eX-eA-eB-eY grouping
        }

        //if ((!eY && eX && j>0 && cvlet->edgel_chain[j-1]==eX) ||                         //found eX-eA-eB grouping
        //    (!eX && eY && j<cvlet->edgel_chain.size()-2 && cvlet->edgel_chain[j+2]==eY)) //found eA-eB-eY grouping
        //  return true;
      }
      //eC-eN <=> eA-eB
      if (cvlet->edgel_chain[j]==eN && j>0 && cvlet->edgel_chain[j-1]==eC)
      {
        assert (eA==eC && eB==eN); //just making sure
        if (eX && eY && j>1 && cvlet->edgel_chain[j-2]==eX && j<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[j+1]==eY){
          cvlet->used = true;
          return true; //found eX-eA-eB-eY grouping
        }

        //if ((!eY && eX && j>1 && cvlet->edgel_chain[j-2]==eX) ||                         //found eX-eA-eB grouping
        //    (!eX && eY && j<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[j+1]==eY)) //found eA-eB-eY grouping
        //  return true;
      }
    }
  }

  return false; //this link is not supported by the same edgel groups
}

//: check to see if this triplet is supported
bool
sdet_sel_base
::triplet_is_supported(sdet_edgel* eX, sdet_edgel* eA, sdet_edgel* eY,
                                          unsigned min_group_size)
{
  //we know eX-->eA-->eY exists, we need to verify that -eX-eA-eY exists for eX and eY.
  sdet_curvelet* cvlet1 = nullptr;
  sdet_curvelet* cvlet2 = nullptr;

  bool cvlet_found = false;

  //for all curvelets of eX that are larger than the group size threshold
  auto cv_it = curvelet_map_.curvelets(eX->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eX->id).end() && !cvlet_found; cv_it++)
  {
    cvlet1 = (*cv_it);

    if (cvlet1->order()<min_group_size)
      continue;

    //check if eX is directly connected to eA and eY
    for (unsigned j=0; j<cvlet1->edgel_chain.size(); j++){
      if (cvlet1->edgel_chain[j]==eX && j<cvlet1->edgel_chain.size()-2 && cvlet1->edgel_chain[j+1]==eA && cvlet1->edgel_chain[j+2]==eY)
      {
        cvlet_found =  true; //found eX-eA-eY grouping
        break;
      }
    }
  }

  if (!cvlet_found)
    return false; // no supporting triplet found in eX

  //for all curvelets of eY that are larger than the group size threshold
  cvlet_found = false; //reset flag
  cv_it = curvelet_map_.curvelets(eY->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eY->id).end() && !cvlet_found; cv_it++)
  {
    cvlet2 = (*cv_it);

    if (cvlet2->order()<min_group_size)
      continue;

    //check if eY is directly connected to eX and eA
    for (unsigned j=0; j<cvlet2->edgel_chain.size(); j++){
      if (cvlet2->edgel_chain[j]==eX && j<cvlet2->edgel_chain.size()-2 && cvlet2->edgel_chain[j+1]==eA && cvlet2->edgel_chain[j+2]==eY)
      {
        cvlet_found =  true; //found eX-eA-eY grouping
        break;
      }
    }
  }

  if (cvlet_found){
    cvlet1->used = true;
    cvlet1->used = false;
  }

  return cvlet_found;
}

void
sdet_sel_base
::prune_the_link_graph()
{
  //prune the link graph of spurious links (ie links that cannot be extended)
  std::vector<sdet_link*> links_to_del;
  for (auto & cLink : edge_link_graph_.cLinks)
  {
    auto l_it = cLink.begin();
    for (;l_it!=cLink.end(); l_it++)
    {
      if (!link_valid(*l_it)) //invalid link
        links_to_del.push_back(*l_it);
    }
  }

  //now delete all these single links
  for (auto & i : links_to_del)
    edge_link_graph_.remove_link(i);

  links_to_del.clear();
}

//: make the link graph bidirectionally consistent
void
sdet_sel_base
::make_link_graph_consistent()
{
  //this ought to be an iterative algorithm
  bool LG_consistent = false;
  while (!LG_consistent)
  {
    std::vector<sdet_link*> links_to_del;

    // go over all the links of the link graph and determine if there is a counterpart to each of the links
    for (auto & cLink : edge_link_graph_.cLinks)
    {
      auto l_it = cLink.begin();
      for (;l_it!=cLink.end(); l_it++)
      {
        if (!link_bidirectional(*l_it)) //invalid link
          links_to_del.push_back(*l_it);
      }
    }

    LG_consistent = (links_to_del.size()==0);

    //delete the inconsistent links along with the curvelets causing them
    for (auto & j : links_to_del)
    {
      //Note: comment this part to visualize the links that are removed by this process

      //first remove the cvlets
      auto cv_it = j->curvelets.begin();
      for (; cv_it != j->curvelets.end(); cv_it++)
        curvelet_map_.remove_curvelet(*cv_it);
    }

    //reconstruct the link graph from the remainng curvelets
    construct_the_link_graph(0, 0); //minsize, method
  }
}

//: clear all contours
void
sdet_sel_base
::clear_all_contours()
{
  //reset linked flags on the link graph
  edge_link_graph_.linked.assign(edgemap_->edgels.size(), false);

  //clear the curve fragment graph
  curve_frag_graph_.clear();

  //form a new contour fragment graph
  curve_frag_graph_.resize(edgemap_->num_edgels());
}

// Extract image contours from the link graph by
// extracting regular contours in successive stages
void
sdet_sel_base
::extract_image_contours_from_the_link_graph(unsigned num_link_iters)
{
  std::cout << "Extracting regular contours from the Link Graph..." ;

  //first remove any existing contours
  clear_all_contours();

  //find the max deg of overlap for all the links
  unsigned max_overlap = 0;
  for (auto & cLink : edge_link_graph_.cLinks){
    auto l_it = cLink.begin();
    for (;l_it!=cLink.end(); l_it++){
      if ((*l_it)->deg_overlap > (int)max_overlap)
        max_overlap = (*l_it)->deg_overlap;
    }
  }

  //clear the linked flags
  edge_link_graph_.clear_linked_flag();

  //now iterate over the range of overlaps to link contours
  //starting from the best overlaps
  sdet_edgel_link_graph link_graph_temp;
  link_graph_temp.linked.resize(edgemap_->edgels.size()); //create a common flag matrix for the iterations

  for (int i=max_overlap; (i>int(max_overlap-num_link_iters) && i>0); i--){
    min_deg_to_link_ = i;

    //form a partial link graph from the links that satisfy the min deg of overlap condition
    link_graph_temp.cLinks.clear();
    link_graph_temp.pLinks.clear();
    link_graph_temp.cLinks.resize(edgemap_->edgels.size());
    link_graph_temp.pLinks.resize(edgemap_->edgels.size());

    for (unsigned j=0; j<edge_link_graph_.cLinks.size(); j++){
      auto l_it = edge_link_graph_.cLinks[j].begin();
      for (;l_it!=edge_link_graph_.cLinks[j].end(); l_it++){
        if ((*l_it)->deg_overlap >= (int)min_deg_to_link_)
          link_graph_temp.cLinks[j].push_back(*l_it);
      }
    }

    for (unsigned j=0; j<edge_link_graph_.pLinks.size(); j++){
      auto l_it = edge_link_graph_.pLinks[j].begin();
      for (;l_it!=edge_link_graph_.pLinks[j].end(); l_it++){
        if ((*l_it)->deg_overlap >= (int)min_deg_to_link_)
          link_graph_temp.pLinks[j].push_back(*l_it);
      }
    }

    extract_one_chains_from_the_link_graph(link_graph_temp);

    //clear the temp links so that they are not deleted (hack)
    link_graph_temp.cLinks.clear();
    link_graph_temp.pLinks.clear();

    std::cout << i << "...";
  }

  //Now sew together any distinct but connected edgel chains
  //TODO: post_process_to_link_fragments();

  //remove any short segments
  //TODO: prune_contours(0.0, 4.0);

  std::cout << "done!" << std::endl;
}


//: extract the regular contours from the link graph
void
sdet_sel_base
::extract_regular_contours_from_the_link_graph()
{
  //first remove any existing contours
  clear_all_contours();

  //clear the linked flags
  edge_link_graph_.clear_linked_flag();

  //exract one chains from the main link graph
  extract_one_chains_from_the_link_graph(edge_link_graph_);
}

//: extract the one chains from a given link graph (from the primary link grpah of ELG)
void
sdet_sel_base
::extract_one_chains_from_the_link_graph(sdet_edgel_link_graph& ELG)
{
  //initialize the curve fragment map
  curve_frag_graph_.resize(edgemap_->edgels.size());
  double thres=gap_;
  //now look for edgel chains
  //Rules:
  //    (a) start from an edgel that is locally legal
  //    (b) trace in both directions until an illegal edgel is reached
  //    (c) prune out short chains

  for (auto first_edgel : edgemap_->edgels)
  {
    //if it's already linked, ignore
    if (ELG.linked[first_edgel->id])
      continue;

    // Check edgel to see if it is legal to start a chain here
    if (ELG.edgel_is_legal_first_edgel(first_edgel))
    {
      //start a chain from this edgel
      auto* chain = new sdet_edgel_chain();

      //add the first edgel to the chain
      chain->push_back(first_edgel);
      ELG.linked[first_edgel->id] = true; //mark it as linked

      //now start tracing FORWARD from its child
      sdet_edgel* eA = ELG.cLinks[first_edgel->id].front()->ce;
      sdet_edgel* eB;
      if(vgl_distance(first_edgel->pt,eA->pt)<thres)
      chain->push_back(eA);

      //trace FORWARD through the link graph until an illegal or terminal edgel is reached
      while (ELG.edgel_is_legal(eA))
      {
        // Mark the last edgel as linked.

        // Note:
        //   By doing this here, we can get the edgels at junctions to be added to the contour
        //   without marking them as linked. This means that other contours arriving at
        //   the junction can also claim the junction edgel as being on their chains.
        ELG.linked[eA->id] = true;

        //is this a terminal edgel?
        if (ELG.cLinks[eA->id].size()==0)
          break; //terminate chain

        //else advance to child node
        eB=eA;
        eA = ELG.cLinks[eA->id].front()->ce;
        if(vgl_distance(eB->pt,eA->pt)<thres)
        chain->push_back(eA);
        else break;
      }

      //Note: Junction edgels will still be marked as unlinked after the tracing is done!

      //now start tracing BACKWARD from the first edgel

      //first determine if this is a closed contour
      //with closed contours, the chain might already include the first edgel twice
      if (eA != first_edgel){
        //not a closed contour, start tracing
        eA = ELG.pLinks[first_edgel->id].front()->pe;
        if(vgl_distance(first_edgel->pt,eA->pt)<thres)
        chain->push_front(eA);
      }

      while (ELG.edgel_is_legal(eA))
      {
        // Mark the last edgel as linked.
        ELG.linked[eA->id] = true;

        //is this a terminal edge?
        if (ELG.pLinks[eA->id].size()==0)
          break; //terminate chain

        //else advance to parent node
        eB=eA;
        eA = ELG.pLinks[eA->id].front()->pe;
        if(vgl_distance(eB->pt,eA->pt)<thres)
        chain->push_front(eA);
        else break;
        }

      //save the current chain on the curve fragment graph
      if (chain->edgels.size()>2)
        curve_frag_graph_.insert_fragment(chain); //prune out the short ones
      else
        delete chain;

    }
  }
}


//: determine if this links from the link grpah is valid
bool
sdet_sel_base
::link_valid(sdet_link* link)
{

  //if (link->deg_overlap >= min_deg_to_link_)
  //  return true;
  //else
  //  return false;

  if (link->vote>1)
    return true;
  else {
    //return false; //simple pruning (dangerous: will remove good links at large gaps)

    //make sure that this link is locally supported by a curvelet, otherwise it's spurious
    //go over all the cvlets on this link and make sure that at least one of them is anchored on its parent
    auto cv_it = link->curvelets.begin();
    for (; cv_it != link->curvelets.end(); cv_it++)
    {
      sdet_curvelet* cvlet = (*cv_it);

      if (cvlet->ref_edgel == link->pe)
        return true;
    }
    return false;
  }
}

//: determine if this link is bidirectional
bool
sdet_sel_base
::link_bidirectional(sdet_link* link)
{
  unsigned src_id = link->pe->id;
  unsigned tar_id = link->ce->id;

  //is there a link from the target to the source?
  auto l_it = edge_link_graph_.cLinks[tar_id].begin();
  for (;l_it!=edge_link_graph_.cLinks[tar_id].end(); l_it++)
  {
    if ((*l_it)->ce->id == (int)src_id)
      return true;
  }

  return false;
}

//: connect pieces of contours that are connected but separated
void
sdet_sel_base
::post_process_to_link_contour_fragments()
{

}


//***********************************************************************//
// Hybrid methods
//***********************************************************************//

//: construct a mapping between edgel ids and curve ids
void
sdet_sel_base
::compile_edge_to_contour_mapping()
{
  //clear the mapping
  cId_.clear();
  cId_.resize(edgemap_->num_edgels());

  //now go over the CFG and create the mapping
  int cnt=1;

  auto f_it = curve_frag_graph_.frags.begin();
  for (; f_it != curve_frag_graph_.frags.end(); f_it++)
  {
    sdet_edgel_chain* chain = (*f_it);

    //go over the edgel chain and insert them into the mapping
    for (auto e : chain->edgels)
    {
      cId_[e->id] = cnt;
    }

    //update contour count
    cnt++;
  }

}

//: attempt to form curvelets from the traced contour fragments
void
sdet_sel_base
::form_curvelets_from_contours(bool clear_existing)
{
  if (clear_existing)
  {
    curvelet_map_.clear();

    //form a new curvelet map
    curvelet_map_.resize(edgemap_->num_edgels());
  }

  auto f_it = curve_frag_graph_.frags.begin();
  for (; f_it != curve_frag_graph_.frags.end(); f_it++)
  {
    sdet_edgel_chain* chain = (*f_it);

    //go over the edgel chain and try to form curvelets using the edgels in the chain
    for (unsigned i=0; i<chain->edgels.size(); i++)
    {
      sdet_edgel* ref_e = chain->edgels[i];

      //Issue: how do we find out whether the ordering is forward/backward wrt the edge chain?
      bool rel_dir;
      if (i==0) rel_dir = sdet_dot(ref_e->tangent, sdet_vPointPoint(ref_e->pt, chain->edgels[i+1]->pt))>0;
      else      rel_dir = sdet_dot(ref_e->tangent, sdet_vPointPoint(chain->edgels[i-1]->pt, ref_e->pt))>0;

      if (centered_ && !bidir_) //form regular curvelets
      {
        //use a symmetric set around the ref of the desired size
        std::deque<sdet_edgel*> cvlet_chain;

        //backward direction
        for (unsigned j=1; j<2*nrad_; j++)
        {
          if (int(i)-int(j)<0) continue;

          sdet_edgel* e=chain->edgels[i-j];
          if (vgl_distance(ref_e->pt, e->pt)>rad_)
            break;

          cvlet_chain.push_front(e); //add it to the sub-chain
        }

        cvlet_chain.push_back(ref_e); //add the current edgel to the subchain

        //forward direction
        for (unsigned j=1; j<2*nrad_; j++)
        {
          if (i+j>=chain->edgels.size()) continue;

          sdet_edgel* e=chain->edgels[i+j];
          if (vgl_distance(ref_e->pt, e->pt)>rad_)
            break;

          cvlet_chain.push_back(e); //add it to the sub-chain
        }

        if (!rel_dir)
          std::reverse(cvlet_chain.begin(), cvlet_chain.end());

        //now form a curvelet from this subchain
        sdet_curvelet* cvlet = nullptr;
        if (cvlet_chain.size()>3)
          cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, true, centered_, false);

        if (cvlet){
          curvelet_map_.add_curvelet(cvlet);
        }
      }

      if (!centered_ && !bidir_) //ENO style curvelets
      {
        //2 curvelets around the ref edgel
        std::deque<sdet_edgel*> cvlet_chainb, cvlet_chaina;

        //backward direction
        for (unsigned j=0; j<2*nrad_; j++)
        {
          if (int(i)-int(j)<0) continue;

          sdet_edgel* e=chain->edgels[i-j];
          if (vgl_distance(ref_e->pt, e->pt)>rad_)
            break;

          cvlet_chainb.push_front(e); //add it to the sub-chain
        }

        if (!rel_dir)
          std::reverse(cvlet_chainb.begin(), cvlet_chainb.end());

        //now form a curvelet from this subchain
        sdet_curvelet* cvletb = nullptr;
        if (cvlet_chainb.size()>3)
          cvletb = form_an_edgel_grouping(ref_e, cvlet_chainb);

        if (cvletb) curvelet_map_.add_curvelet(cvletb);

        //forward direction
        for (unsigned j=0; j<2*nrad_; j++)
        {
          if (i+j>=chain->edgels.size()) continue;

          sdet_edgel* e=chain->edgels[i+j];
          if (vgl_distance(ref_e->pt, e->pt)>rad_)
            break;

          cvlet_chaina.push_back(e); //add it to the sub-chain
        }

        if (!rel_dir)
          std::reverse(cvlet_chaina.begin(), cvlet_chaina.end());

        //now form a curvelet from this subchain
        sdet_curvelet* cvleta = nullptr;
        if (cvlet_chainb.size()>3)
          cvleta = form_an_edgel_grouping(ref_e, cvlet_chaina);

        if (cvleta) curvelet_map_.add_curvelet(cvleta);

      }
    }
  }

}

//: attempt to form curvelets from the traced contour fragments
void
sdet_sel_base
::form_curvelets_from_contours(unsigned max_size_to_group)
{
  //form a new curvelet map
  curvelet_map_.resize(edgemap_->num_edgels());

  auto half_size = (unsigned) std::floor((max_size_to_group-1)/2.0);

  auto f_it = curve_frag_graph_.frags.begin();
  for (; f_it != curve_frag_graph_.frags.end(); f_it++)
  {
    sdet_edgel_chain* chain = (*f_it);

    if (chain->edgels.size()<max_size_to_group)
      continue;

    //go over the edgel chain and try to form curvelets using the edgels in the chain
    for (unsigned i=0; i<chain->edgels.size(); i++)
    {
      sdet_edgel* ref_e = chain->edgels[i];

      //Issue: how do we find out whether the ordering is forward/backward wrt the edge chain?
      bool rel_dir;
      if (i==0) rel_dir = sdet_dot(ref_e->tangent, sdet_vPointPoint(ref_e->pt, chain->edgels[i+1]->pt))>0;
      else      rel_dir = sdet_dot(ref_e->tangent, sdet_vPointPoint(chain->edgels[i-1]->pt, ref_e->pt))>0;

      //use different groups according to the nature of the grouping process
      if (centered_) //regular curvelet
      {
        //first in the forward direction
        //use a symmetric set around the ref of the desired size
        std::deque<sdet_edgel*> cvlet_chain;
        for (int j=(int)i-(int)half_size; j<(int)(i+half_size+1); j++){
          if (j>=0 && j<(int)chain->edgels.size())
            cvlet_chain.push_back(chain->edgels[j]);
        }

        sdet_curvelet* cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, rel_dir, centered_, false);
        if (cvlet)
          curvelet_map_.add_curvelet(cvlet, rel_dir);

        if (bidir_){ //also in the other direction
          sdet_curvelet* cvlet2 = form_an_edgel_grouping(ref_e, cvlet_chain, !rel_dir, centered_, false);
          if (cvlet2)
            curvelet_map_.add_curvelet(cvlet2, !rel_dir);
        }
      }
      else {
        //form it as two separate one sided ones ones

        //forward half
        std::deque<sdet_edgel*> cvlet_chain;
        for (unsigned j=0; j<i+half_size+1; j++){
          if (j<chain->edgels.size())
            cvlet_chain.push_back(chain->edgels[j]);
        }
        sdet_curvelet* cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, rel_dir, centered_, true);
        if (cvlet)
          curvelet_map_.add_curvelet(cvlet);

        if (bidir_)//other half in the other direction
        {
          std::deque<sdet_edgel*> cvlet_chain;
          for (int j=i-half_size; j<1; j++){
            if (j>=0 && j<(int)chain->edgels.size())
              cvlet_chain.push_back(chain->edgels[j]);
          }
          sdet_curvelet* cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, !rel_dir, centered_, true);
          if (cvlet)
            curvelet_map_.add_curvelet(cvlet);
        }
        else { //other half in the same direction (ENO style)
          std::deque<sdet_edgel*> cvlet_chain;
          for (int j=i-half_size; j<1; j++){
            if (j>=0 && j<(int)chain->edgels.size())
              cvlet_chain.push_back(chain->edgels[j]);
          }
          sdet_curvelet* cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, rel_dir, centered_, false);
          if (cvlet)
            curvelet_map_.add_curvelet(cvlet);
        }
      }
    }
  }
}

//: Break contours at places where curvelets cannot form
void
sdet_sel_base
::post_process_to_break_contours()
{
  //Assume that curvelets have been computed already and the link graph reflects all the links from the curvelets

  std::cout << "Breaking contours....";

  //container for all fragments to be deleted
  std::vector<sdet_edgel_chain*> frags_to_del;

  //container for all the new sub fragments
  std::vector<sdet_edgel_chain*> new_frags;

  //size of segment to ignore at the beginning and end of a contour
  unsigned max_size_to_group=7; //temp (should be passed in to this function)
  auto half_size = (unsigned) std::floor((max_size_to_group-1)/2.0);

  auto f_it = curve_frag_graph_.frags.begin();
  for (; f_it != curve_frag_graph_.frags.end(); f_it++)
  {
    sdet_edgel_chain* chain = (*f_it);

    if (chain->edgels.size()<max_size_to_group)
    {
      frags_to_del.push_back(chain);
      continue;
    }

    //Now trace through the curve fragment and verify that each edgel has a legal curve bundle
    //if not form sub chains
    sdet_edgel_chain* sub_chain = nullptr;
    bool needs_to_be_deleted = false;
    //bool first_frag = true;
    bool break_found = false;
    bool forming_sub_chain = false;

    for (unsigned j=half_size-1; j<chain->edgels.size()-half_size; j++)
    {
      bool cvlet_exists = curvelet_map_.curvelets(chain->edgels[j]->id).size()>0;
      needs_to_be_deleted = needs_to_be_deleted || !cvlet_exists;

      if (!break_found && !cvlet_exists)
        break_found = true;

      if (break_found && cvlet_exists) //create new sub chain from here
      {
        sub_chain = new sdet_edgel_chain();
        break_found = false;
        forming_sub_chain = true;
      }

      //grow or terminate current sub chain
      if (forming_sub_chain){
        //add edgel to the current sub chain
        sub_chain->edgels.push_back(chain->edgels[j]);

        if (!cvlet_exists)//terminate the sub chain
        {
          //add it to the fragment list
          new_frags.push_back(sub_chain);
          forming_sub_chain = false;
        }
      }
    }

    if (forming_sub_chain) //terminate the final sub chain
    {
      //now add it to the fragment list
      new_frags.push_back(sub_chain);
    }

    //delete the curve fragments that were broken
    if (needs_to_be_deleted)
      frags_to_del.push_back(chain); //mark for deletion

  }

  //First, delete all the fragments marked for deletion
  for (auto & j : frags_to_del)
    curve_frag_graph_.remove_fragment(j);

  //Finally, add all the new fragments to curve fragment graph
  for (auto & new_frag : new_frags)
  {
    if (new_frag->edgels.size()>2)
      curve_frag_graph_.insert_fragment(new_frag);
  }

  std::cout << "done." << std::endl;
}

//: evauate the qualities of curvelets using various functions
void
sdet_sel_base
::evaluate_curvelet_quality(int method)
{
  //for each edgel, for each curvelet, compute quality using the specified method
  for (auto eA : edgemap_->edgels)
  {
    auto it = curvelet_map_.curvelets(eA->id).begin();
    for (; it != curvelet_map_.curvelets(eA->id).end(); it++){
      sdet_curvelet* cvlet = (*it);

      switch(method){
        case 0: //average distance between edgels
        {
          double L=0;
          for (unsigned i=0; i<cvlet->edgel_chain.size()-1; i++)
            L += vgl_distance(cvlet->edgel_chain[i]->pt, cvlet->edgel_chain[i+1]->pt);

          cvlet->quality = L/(cvlet->edgel_chain.size()-1);
          break;
        }
        case 1: //largest distance between edgels in the curvelet
        {
          double maxL = -1.0;
          for (unsigned i=0; i<cvlet->edgel_chain.size()-1; i++){
            double d = vgl_distance(cvlet->edgel_chain[i]->pt, cvlet->edgel_chain[i+1]->pt);
            if (d>maxL) maxL = d;
          }
          cvlet->quality = maxL;
          break;
        }
        case 3: //ratio between the largest distance and the total length
        {
          double maxL = -1.0;
          double L=0;
          for (unsigned i=0; i<cvlet->edgel_chain.size()-1; i++){
            double d = vgl_distance(cvlet->edgel_chain[i]->pt, cvlet->edgel_chain[i+1]->pt);
            L += d;
            if (d>maxL) maxL = d;
          }

          cvlet->quality = maxL/L;
          break;
        }
        case 4: //ratio/diff in length beween the polyline and the length of the curve fit
        {
          break;
        }
      }


    }
  }

}

//: method to look at the spread of differential estimates
void
sdet_sel_base
::determine_accuracy_of_measurements()
{
  //determine # of curvelets
  int cvlet_cnt=0;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
    cvlet_cnt += curvelet_map_.curvelets(i).size();

  //estimates
  double estimates[3], min_estimates[3], max_estimates[3];
  vnl_vector<double> theta_est(cvlet_cnt), k_est(cvlet_cnt), gamma_est(cvlet_cnt);
  vnl_vector<double> theta_error(cvlet_cnt), k_error(cvlet_cnt), gamma_error(cvlet_cnt);

  cvlet_cnt=0;
  //for each edgel, for each curvelet, get accuracy measurements from the curve_model
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    auto it = curvelet_map_.curvelets(i).begin();
    for (; it != curvelet_map_.curvelets(i).end(); it++){
      sdet_curvelet* cvlet = (*it);

      //get the measurements from the curvelets
      cvlet->curve_model->report_accuracy(estimates, min_estimates, max_estimates);

      theta_est(cvlet_cnt) = estimates[0];
      k_est(cvlet_cnt)     = estimates[1];
      gamma_est(cvlet_cnt) = estimates[2];

      theta_error(cvlet_cnt) = std::min(std::abs(estimates[0]-min_estimates[0]), std::abs(estimates[0]-max_estimates[0]));
      k_error(cvlet_cnt)     = std::min(std::abs(estimates[1]-min_estimates[1]), std::abs(estimates[1]-max_estimates[1]));
      gamma_error(cvlet_cnt) = std::min(std::abs(estimates[2]-min_estimates[2]), std::abs(estimates[2]-max_estimates[2]));

      cvlet_cnt++;
    }
  }

  double theta_mean, theta_std, theta_error_mean, theta_error_std;
  double k_mean, k_std, k_error_mean, k_error_std;
  double gamma_mean, gamma_std, gamma_error_mean, gamma_error_std;

  //calculate the meaan and variance of the measurements
  calc_mean_var(theta_mean, theta_std, theta_est);
  calc_mean_var(k_mean, k_std, k_est);
  calc_mean_var(gamma_mean, gamma_std, gamma_est);

  //calculate mean and variance of the errors
  calc_mean_var(theta_error_mean, theta_error_std, theta_error);
  calc_mean_var(k_error_mean, k_error_std, k_error);
  calc_mean_var(gamma_error_mean, gamma_error_std, gamma_error);

  std::cout << "======================================" << std::endl;
  std::cout << "Derivative estimate accuracy Report"    << std::endl;
  std::cout << "======================================" << std::endl;
  std::cout << "theta estimate (mean, std): (" << theta_mean << ", " << std::sqrt(theta_std) << ")" << std::endl;
  std::cout << "  k   estimate (mean, std): (" << k_mean << ", " << std::sqrt(k_std) << ")" << std::endl;
  std::cout << "gamma estimate (mean, std): (" << gamma_mean << ", " << std::sqrt(gamma_std) << ")" << std::endl;

  std::cout << "spread of theta estimate (mean, std): (" << theta_error_mean << ", " << std::sqrt(theta_error_std) << ")" << std::endl;
  std::cout << "spread of  k   estimate (mean, std): (" << k_error_mean << ", " << std::sqrt(k_error_std) << ")" << std::endl;
  std::cout << "spread of gamma estimate (mean, std): (" << gamma_error_mean << ", " << std::sqrt(gamma_error_std) << ")" << std::endl;

}


void
sdet_sel_base
::report_stats()
{
  std::cout << "======================================" << std::endl;
  std::cout << "Edge Linking Summary\n";
  std::cout << "======================================" << std::endl;
  std::cout << "# of edgels:   " << edgemap_->edgels.size() << std::endl;
  std::cout << "Parameters: ";
  std::cout << "dx = " << dpos_ << ", dt = " << dtheta_ << std::endl;
  std::cout << "neighborhood radius = " << rad_ << ", maxN = " << maxN_ << std::endl;

  unsigned max_size = 0;
  //find maximum size of curvelet
  for (unsigned i=0; i<edgemap_->edgels.size(); i++){
    auto it = curvelet_map_.curvelets(i).begin();
    for (; it != curvelet_map_.curvelets(i).end(); it++){
      sdet_curvelet* cvlet = (*it);

      if (cvlet->order()>max_size)
        max_size = cvlet->order();
    }
  }

  //count the raw number of edgel groupings by size
  std::vector<int> cvlet_cnt(max_size+1, 0);

  //count the # of edgels that have a particular sized grouping
  std::vector<int> cvlet_edgel_cnt(max_size+1, 0);

  //count the # of edgels that have a curvelets of a certain size or lower
  std::vector<int> min_cvlet_edgel_cnt(max_size+1, 0);

  //count the # of curvelets that are consistent with their neighbors (criteria 1)
  std::vector<int> consistent_cvlet_cnt(max_size+1, 0);

  //count the # of edgels that have consistent curvelets of a particular size
  std::vector<int> consistent_cvlet_edgel_cnt(max_size+1, 0);

  //count the # of edgels that have consistent curvelets of a particular size or lower
  std::vector<int> min_consistent_cvlet_edgel_cnt(max_size+1, 0);

  for (auto eA : edgemap_->edgels)
  {
    //keep track of the various sized curvelets formed by this edgel
    std::vector<bool> size_exists(max_size+1, false);
    std::vector<bool> consistent_size_exists(max_size+1, false);

    //for each edgel go over all the curvelets it forms
    auto it = curvelet_map_.curvelets(eA->id).begin();
    for (; it != curvelet_map_.curvelets(eA->id).end(); it++)
    {
      sdet_curvelet* cvlet = (*it);

      //increment the raw cvlet counter for the current size
      cvlet_cnt[cvlet->order()]++;

      //make a note of the size of this curvelet
      size_exists[cvlet->order()] = true;

      //check to see if this curvelet is consistent with its neighbors
      //This requires a parameter of which neighboring curvelets to consider to judge consistency
      //so use the same sized neighbors as itself
      bool cvlet_has_consistent_neighbor = false;
      for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
        if (k>0 && cvlet->edgel_chain[k-1]==eA) //the link after it (eA --> edgel_chain[k])
        {
          if (link_is_reciprocal(eA, cvlet->edgel_chain[k], eA, cvlet->order()))
            cvlet_has_consistent_neighbor = true;
        }
        if (k<cvlet->edgel_chain.size()-1 && cvlet->edgel_chain[k+1]==eA) //the link before it (edgel_chain[k] --> eA)
        {
          if (link_is_reciprocal(cvlet->edgel_chain[k], eA, eA, cvlet->order()))
            cvlet_has_consistent_neighbor = true;
        }
      }

      if (cvlet_has_consistent_neighbor){
        consistent_cvlet_cnt[cvlet->order()]++;
        consistent_size_exists[cvlet->order()] = true;
      }
    }

    //now update the cvlet_edgel_cnt with the various sized curvelets that this edgel had formed
    for (unsigned j=3; j<=max_size; j++)
    {
      if (size_exists[j]) cvlet_edgel_cnt[j]++;

      bool larger = false;
      for (unsigned k=j+1; k<=max_size; k++)
        larger = larger || size_exists[k];

      //this is the largest curvelet that this edgel has
      if (!larger)  min_cvlet_edgel_cnt[j]++;

      //repeat for consistent curvelets only
      if (consistent_size_exists[j]) consistent_cvlet_edgel_cnt[j]++;

      larger = false;
      for (unsigned k=j+1; k<=max_size; k++)
        larger = larger || consistent_size_exists[k];

      //this is the largest curvelet that this edgel has
      if (!larger)  min_consistent_cvlet_edgel_cnt[j]++;
    }
  }

  //report
  std::cout << "==========================================================================================================" << std::endl;
  std::cout << "| cvlet size | raw # cvlet | % of edgels | % of edgels with no other larger curvelets | # of consistent cvlets | % of edgels with consistent curvelets |" << std::endl;
  for (unsigned i=3; i<=max_size; i++){
    std::cout << i << " " << cvlet_cnt[i] << " " << cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << " " << min_cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << " " ;
    std::cout <<  consistent_cvlet_cnt[i] << " " << consistent_cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << " " << min_consistent_cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << std::endl;
  }

  std::cout << std::endl;
  std::cout << "# of image curves: " << curve_frag_graph_.frags.size() << std::endl;
  std::cout << "======================================" << std::endl;
}

// Moved sel_base member functions from sdet_sel_base_CFTG_algo to here
// to keep all together

//********************************************************************//
// Functions for constructing hypothesis trees and
// tracing between contour end points.
//********************************************************************//


#define SM_TH 0.6

#define Theta_1 0.6
#define Theta_2 0.0
#define Strength_Diff1 2.0
#define Strength_Diff2 4.0

sdet_EHT*
sdet_sel_base
::construct_hyp_tree(sdet_edgel* edge)
{
  if (edge_link_graph_.cLinks.size()==0)
    {
    std::cout << "No Link Graph !" <<std::endl;
    return nullptr;
    }


 //construct 2 HTs: one in the forward direction and one in the reverse direction ????  by yuliang no forword
//Modify: for each node, consider all the child links and parent links(exact the one linking parent node), as its child node
  std::queue<sdet_EHT_node*> BFS_queue;

  //forward HT
  auto* HTF = new sdet_EHT();

  auto* root1 = new sdet_EHT_node(edge);
  HTF->root = root1;
  BFS_queue.push(root1);

  int depth = 0; // comment by Yuliang, this is not the depth of the tree, but number of nodes actually

  //How far do we wanna go (if we don't hit a node)?
  while (!BFS_queue.empty() && std::log10(double(depth))<3)
  {
    sdet_EHT_node* cur_node = BFS_queue.front();
    BFS_queue.pop();

    //are we at a CFG node? if we are we don't need to go any further
    if (cur_node!= root1 &&
        (curve_frag_graph_.pFrags[cur_node->e->id].size()>0 ||
         curve_frag_graph_.cFrags[cur_node->e->id].size()>0))
      continue;

    //also if we hit an edgel that is already linked no need to go further (this might ensure planarity)
    if (edge_link_graph_.linked[cur_node->e->id])
      continue;

    //propagate this node
    auto lit = edge_link_graph_.cLinks[cur_node->e->id].begin();
    for (; lit != edge_link_graph_.cLinks[cur_node->e->id].end(); lit++)
    {
      if (edge_link_graph_.linked[(*lit)->ce->id]) //don't go tracing in linked contours
        continue;

      if (cur_node->parent) {
        //make a simple consistency check
        double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
        double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
        double dx2 = (*lit)->ce->pt.x() - cur_node->e->pt.x();
        double dy2 = (*lit)->ce->pt.y() - cur_node->e->pt.y();


        if (((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2))<SM_TH) //not consistent, but with lower TH to keep more Hypothesis by Yuliang ////////// Cosine Formula
          continue;
      }

      //else extend the tree to this edgel
      auto* new_node = new sdet_EHT_node((*lit)->ce);

      cur_node->add_child(new_node);
      BFS_queue.push(new_node);
      depth++;
    }
    // by Yuliang
    // explore in pLinks
    lit = edge_link_graph_.pLinks[cur_node->e->id].begin();
    for (; lit != edge_link_graph_.pLinks[cur_node->e->id].end(); lit++)
    {
      if (edge_link_graph_.linked[(*lit)->pe->id]) //don't go tracing in linked contours
        continue;
      if (cur_node->parent) {
          if((*lit)->pe->id == cur_node->parent->e->id)// if this parent link the same from parent node, don't trace
        continue;
        //make a simple consistency check
        double dx1 = cur_node->e->pt.x() - cur_node->parent->e->pt.x();
        double dy1 = cur_node->e->pt.y() - cur_node->parent->e->pt.y();
        double dx2 = (*lit)->pe->pt.x() - cur_node->e->pt.x();
        double dy2 = (*lit)->pe->pt.y() - cur_node->e->pt.y();

        if (((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2))<SM_TH) //not consistent, but with lower TH to keep more Hypothesis by Yuliang
          continue;
      }

      //else extend the tree to this edgel
      auto* new_node = new sdet_EHT_node((*lit)->pe);

      cur_node->add_child(new_node);
      BFS_queue.push(new_node);
      depth++;
    }
  }

  //empty the bfs queue
  while (!BFS_queue.empty())
    BFS_queue.pop();

  return HTF;
}

//: construct all possible EHTs from the terminal nodes and find legal contour paths
void
sdet_sel_base
::construct_all_path_from_EHTs()
{
  // modify by Yuliang, use a local filter for contours size 3 and 4 to prune some, and merge some continuous regular contours first
  regular_contour_filter();
  //go over the contour fragment graph and form an EHT from every terminal node
  //validate each of the paths in the EHT

  std::vector<sdet_edgel_chain*> new_frags;

  //going over the edgemap instead so that an EHT only starts once from a node when there are two
  //contour fragments terminating there
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    if (curve_frag_graph_.pFrags[i].size()==0 &&
        curve_frag_graph_.cFrags[i].size()==0)
      continue; //no terminal nodes here

    //1) Terminal node found, construct an EHT from here
    sdet_EHT* EHT1 = construct_hyp_tree(eA);

    //2) traverse each path and determine its validity
    if (EHT1)
    {
      //traverse the EHT and test all the paths
      sdet_EHT::path_iterator pit = EHT1->path_begin();
      for (; pit != EHT1->path_end(); pit++){
        std::vector<sdet_edgel*>& edgel_chain = pit.get_cur_path();

    sdet_edgel* le = edgel_chain.back();

        if (curve_frag_graph_.pFrags[le->id].size()==0 && curve_frag_graph_.cFrags[le->id].size()==0)
        {
          //not a valid termination node
          //delete the node associated  with this path ( it will delete the entire path, by definition)
          EHT1->delete_subtree(pit);
          continue;
        }

        //test this path to see if it is valid
        if (!is_EHT_path_legal(edgel_chain)){
          EHT1->delete_subtree(pit);
          continue;
        }

        //copy this chain
        auto* new_chain = new sdet_edgel_chain();
        new_chain->append(edgel_chain);
        new_chain->temp = true; //make sure that the new frags are initialized with temp flags

        curve_frag_graph_.CFTG.insert_fragment(new_chain);
        //now that its added delete the apth
        EHT1->delete_subtree(pit);
      }

      //finally delete the EHT
      delete EHT1;
    }
  }
  std::cout<<"Finish constructing all hypothesis trees"<<std::endl;
  ////Now add all the new curve fragments into the CFG (as tentative fragments)
  //for (unsigned i=0; i<new_frags.size(); i++)
  //  curve_frag_graph_.insert_fragment(new_frags[i]);
}

//: perform a geometric consistency check to determine whether a given temp path is valid
bool
sdet_sel_base
::is_EHT_path_legal(std::vector<sdet_edgel*>& edgel_chain)
{
  //what makes a path legal?

  // Modified by Yuliang, before (a), (b), if path only have < 3 edgels, prune out direct link over 3 pixels
  if(edgel_chain.size()<3)
  {
      sdet_edgel* eS = edgel_chain.front();
      sdet_edgel* eE = edgel_chain.back();
      double dx1 = eE->pt.x() - eS->pt.x();
      double dy1 = eE->pt.y() - eS->pt.y();
      double dist= std::sqrt(dx1*dx1+dy1*dy1);
      if(dist>5)
        return false;
   }

  // by Yuliang, construct two lists of end nodes from other Curve Fragments end poits linking the end points of the path
  std::vector<sdet_edgel*> S_link_end_nodes, E_link_end_nodes;

  // (a) if a c1 polyarc bundle can form within it
  // (b) if it is c1 compatible with the end points

  //For now, just check for...

  //1) continuity consistency at the end points
  //1a) at the start point
  sdet_edgel* eS = edgel_chain.front();
  sdet_edgel* e2 = edgel_chain[1];
  double dx1 = e2->pt.x() - eS->pt.x();
  double dy1 = e2->pt.y() - eS->pt.y();
  bool cons = false;


  //is this at a start or an end of an unambiguous chain?
  auto pcit = curve_frag_graph_.pFrags[eS->id].begin();
  for ( ; pcit != curve_frag_graph_.pFrags[eS->id].end(); pcit++)
  {
    sdet_edgel* pe = (*pcit)->edgels[(*pcit)->edgels.size()-2];
    S_link_end_nodes.push_back((*pcit)->edgels.front());

    //make a simple consistency check for child Frags
    double dx2 = eS->pt.x() - pe->pt.x();
    double dy2 = eS->pt.y() - pe->pt.y();

    cons = cons || ((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2))>0;
  }
  auto ccit = curve_frag_graph_.cFrags[eS->id].begin();
  for ( ; ccit != curve_frag_graph_.cFrags[eS->id].end(); ccit++)
  {
    sdet_edgel* ce = (*ccit)->edgels[1];
    S_link_end_nodes.push_back((*ccit)->edgels.back());

    //make a simple consistency check for parent Frags
    double dx2 = eS->pt.x() - ce->pt.x();
    double dy2 = eS->pt.y() - ce->pt.y();

    cons = cons || ((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2))>0;
  }
  if (!cons) return false; //no good at the start point

  //1b) at the end point
  sdet_edgel* eE = edgel_chain.back();
  e2 = edgel_chain[edgel_chain.size()-2];
  dx1 = eE->pt.x() - e2->pt.x();
  dy1 = eE->pt.y() - e2->pt.y();
  cons = false;

  //is this at a start or an end of an unambiguous chain?
  pcit = curve_frag_graph_.pFrags[eE->id].begin();
  for ( ; pcit != curve_frag_graph_.pFrags[eE->id].end(); pcit++)
  {
    sdet_edgel* pe = (*pcit)->edgels[(*pcit)->edgels.size()-2];
    E_link_end_nodes.push_back((*pcit)->edgels.front());

    //make a simple consistency check
    double dx2 = pe->pt.x() - eE->pt.x();
    double dy2 = pe->pt.y() - eE->pt.y();

    cons = cons || ((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2))>0;
  }
  ccit = curve_frag_graph_.cFrags[eE->id].begin();
  for ( ; ccit != curve_frag_graph_.cFrags[eE->id].end(); ccit++)
  {
    sdet_edgel* ce = (*ccit)->edgels[1];
    E_link_end_nodes.push_back((*ccit)->edgels.back());

    //make a simple consistency check
    double dx2 = ce->pt.x() - eE->pt.x();
    double dy2 = ce->pt.y() - eE->pt.y();

    cons = cons || ((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2))>0;
  }
  if (!cons) return false; //no good at the end point

  //2) By Yuliang, use the two lists, check if it is a path between which unambiguous contours linked
  for (auto S_l_e : S_link_end_nodes)
    {
    if(S_l_e == eE)
      return false;
    for (auto E_l_e : E_link_end_nodes)
      {
      if(E_l_e == eS)
        return false;
      // the case two connected contours filling the path
      if(E_l_e== S_l_e)
        return false;
      }
    }

  // comment by Yuliang, in most cases, it is no need, because of short paths.
  //fit_polyarc_to_chain(&edgel_chain);
  return true;
}

//: New Quality Metric by Naman Kumar :: compute a path metric based on the Gap, Orientation, Strength and Size of the chain
double
sdet_sel_base
::compute_path_metric2(std::vector<sdet_edgel*>& Pchain,
                       std::vector<sdet_edgel*>& Tchain,
                       std::vector<sdet_edgel*>& Cchain)
{
  double cost = 0.0;double ds=0;double dt=0;

  //construct an edgel chain out of all three chains
  std::vector<sdet_edgel*> chain;
  if (Pchain.size())
    for (auto i : Pchain) chain.push_back(i);
  if (Tchain.size())
    for (auto i : Tchain) chain.push_back(i);
  if (Cchain.size())
    for (auto i : Cchain) chain.push_back(i);

  //now compute the metric
  sdet_edgel *eA=nullptr, *eP=nullptr;
  double dsp = 0, thp = 0, total_ds =0.0, a=0.0,s1=0,s2=0,s=0,size=chain.size();
  for (unsigned i=1; i<chain.size(); i++)
  {
    eA = chain[i];
    eP = chain[i-1];
    s1=(eA)->strength;
    s2=(eP)->strength;
    s=std::fabs(s1-s2);
    //compute ds
    ds = vgl_distance(eA->pt, eP->pt);
    if(ds>1.0) a=2.0; else a=1.0;
    total_ds += ds;
    //compute dtheta
    double thc = sdet_vPointPoint(eP->pt, eA->pt);
    dt = std::fabs(thc-thp);
    dt = (dt>vnl_math::pi)? 2*vnl_math::pi-dt : dt;
    cost += std::pow((s+dt + a*ds)/size, 2.0);
    thp = thc;//save the current vector for the next iteration
  }
  return cost;
}


bool link_cost_less(sdet_CFTG_link* link1, sdet_CFTG_link* link2)
{return link1->cost < link2->cost;}


//: disambiguate the CFG, basically to produce a disjoint set
void sdet_sel_base::disambiguate_the_CFTG()
{
  //At the moment, I cannot verify that the CFTG is topologically sound (i.e., a planar graph)
  //so within this limit, the goal is to break the links at junctions

  //Alternatively, it is possible to splice the two contours and mark the connection with the others as a junction
  //these others might be pruned off at a postprocessing stage if desired

  //Note: the temp flag on the contours distinguish it from the unambiguous contours

  //A) disambiguate the links first : Only keep the best path
  //   Note: remember to search in both directions

  //go over all the links of the CFTG
  std::vector<sdet_edgel*> dummy_chain;

  auto l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    sdet_CFTG_link* cur_Link = (*l_it);


    //is this an ambiguous link?
    if (cur_Link->cCFs.size()>1)
    {
      //needs disambiguation
      double min_cost = 10000;
      sdet_edgel_chain* best_chain = nullptr;
    auto f_it = cur_Link->cCFs.begin();
    for(; f_it != cur_Link->cCFs.end(); f_it++)
      {
        sdet_edgel_chain* edgel_chain = (*f_it);
        std::vector<sdet_edgel*> chain(edgel_chain->edgels.begin(), edgel_chain->edgels.end());

        double path_cost = compute_path_metric2(dummy_chain, chain, dummy_chain);
        if (path_cost < min_cost){
          min_cost = path_cost;
          best_chain = edgel_chain;
        }

      }

      //remove all except the best chain
      if (best_chain){
        auto f_it = cur_Link->cCFs.begin();
        for(; f_it != cur_Link->cCFs.end(); f_it++)
          if ((*f_it) != best_chain)
            delete (*f_it);

        cur_Link->cCFs.clear();
        cur_Link->cCFs.push_back(best_chain);
        cur_Link->cost = min_cost;
}
    }
    else { //just comptue cost for this path

      sdet_edgel_chain* edgel_chain = cur_Link->cCFs.front();
      std::vector<sdet_edgel*> chain(edgel_chain->edgels.begin(), edgel_chain->edgels.end());
      cur_Link->cost = compute_path_metric2(dummy_chain, chain, dummy_chain);
    }
  }
  //B) disambiguate between duplicates (A->B vs B->A)
  l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    sdet_CFTG_link* cur_Link = (*l_it);

    if (cur_Link->cCFs.size()==0)
      continue;

    //look for the link from the other direction
    auto l_it2 = curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].begin();
    for (; l_it2 != curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].end(); l_it2++){
      if ((*l_it2)->eE == cur_Link->eS){
        //duplicate found

        if ((*l_it2)->cCFs.size()==0)
          continue;

        sdet_edgel_chain* edgel_chain1 = cur_Link->cCFs.front();
        sdet_edgel_chain* edgel_chain2 = (*l_it2)->cCFs.front();

        std::vector<sdet_edgel*> chain1(edgel_chain1->edgels.begin(), edgel_chain1->edgels.end());
        std::vector<sdet_edgel*> chain2(edgel_chain2->edgels.begin(), edgel_chain2->edgels.end());

        double path_cost1 = compute_path_metric2(dummy_chain, chain1, dummy_chain);
        double path_cost2 = compute_path_metric2(dummy_chain, chain2, dummy_chain);

        if (path_cost1<path_cost2){
          //keep current link and delete the other
          delete edgel_chain2;
          (*l_it2)->cCFs.clear();
          (*l_it2)->cost = 1000;
        }
        else{
          delete edgel_chain1;
          cur_Link->cCFs.clear();
          cur_Link->cost = 1000;
        }
      }
    }
  }

  //C) Gradient descent to prune bifurcations from the CFTG

  //go over list of Links and find any with degree > 1
  //these need to be disambiguated (gradient descent)
  std::list<sdet_CFTG_link*> GD_list;

  //populate the map
  l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    //compute degree at each end
    int deg_S = curve_frag_graph_.CFTG.cLinks[(*l_it)->eS->id].size() + curve_frag_graph_.CFTG.pLinks[(*l_it)->eS->id].size();
    int deg_E = curve_frag_graph_.CFTG.cLinks[(*l_it)->eE->id].size() + curve_frag_graph_.CFTG.pLinks[(*l_it)->eE->id].size();

    if (deg_S>1){
      GD_list.push_back(*l_it);
      continue;
    }

    if (deg_E>1){
      GD_list.push_back(*l_it);
      continue;
    }

  }


  //sort the cost list
  GD_list.sort(link_cost_less);

  //gradient descent
  while (GD_list.size()>0)
  {
    sdet_CFTG_link* cur_Link = GD_list.front();
    GD_list.pop_front();

    //now remove the other links connected to the end points of this link
    //clinks from eS
    std::vector<sdet_CFTG_link*> links_to_del;
    l_it = curve_frag_graph_.CFTG.cLinks[cur_Link->eS->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.cLinks[cur_Link->eS->id].end(); l_it++){

      if ((*l_it) != cur_Link)
    links_to_del.push_back((*l_it));
    }

    //by yuliang, also consider plinks from eS
    l_it = curve_frag_graph_.CFTG.pLinks[cur_Link->eS->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.pLinks[cur_Link->eS->id].end(); l_it++){

      if ((*l_it) != cur_Link)
        links_to_del.push_back((*l_it));
    }

    for (auto & j : links_to_del){
      GD_list.remove(j);//also remove it from the GD list
      curve_frag_graph_.CFTG.remove_link(j);
    }
    links_to_del.clear();

    //plinks from eE
    l_it = curve_frag_graph_.CFTG.pLinks[cur_Link->eE->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.pLinks[cur_Link->eE->id].end(); l_it++){

      if ((*l_it) != cur_Link)
        links_to_del.push_back((*l_it));
    }

    //by yuliang, also consider clinks from eE
    l_it = curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].begin();
    for (; l_it != curve_frag_graph_.CFTG.cLinks[cur_Link->eE->id].end(); l_it++){

      if ((*l_it) != cur_Link)
        links_to_del.push_back((*l_it));
    }

    for (auto & j : links_to_del){
      GD_list.remove(j);//also remove it from the GD list
      curve_frag_graph_.CFTG.remove_link(j);
    }
    links_to_del.clear();
  }

  //D) Add it all back to the CFG (clear the CFTG in the process)
  l_it = curve_frag_graph_.CFTG.Links.begin();
  for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
  {
    if ((*l_it)->cCFs.size()==1)
      curve_frag_graph_.insert_fragment((*l_it)->cCFs.front());
  }
  curve_frag_graph_.CFTG.clear();
  curve_frag_graph_.CFTG.resize(edgemap_->edgels.size());
   std::cout<<"Finish disambiguating the CFTG"<<std::endl;
}

// Following part by Yuliang Guo.
static bool is_continue (const sdet_edgel_chain *c1, const sdet_edgel_chain *c2); // test genenal/local continuity

static bool is_longer (const sdet_edgel_chain *c1, const sdet_edgel_chain *c2){ // whether contour 1 is longer
    if (c1->edgels.size()>c2->edgels.size()){
        return true;
    }
    return false;

}

static double get_continuity (const sdet_edgel_chain *c1, const sdet_edgel_chain *c2);
//: correct the CFG topology to produce a disjoint set


void sdet_sel_base::correct_CFG_topology()
{
  //D) Final T-junction type disambiguation can be done on the CFG
  // Basically, go over all the nodes of the CFG and operate on the ones with degree>2
  // also merge all segments that are adjacent

  // going over the edgemap instead so that a node is visited only once and so that I
  // don't have to deal with iterator issues

  for (unsigned int i=0; i<edgemap_->edgels.size(); ++i)
  {
    sdet_edgel_chain *c1=nullptr, *c2=nullptr;

    int deg = curve_frag_graph_.pFrags[i].size()+ curve_frag_graph_.cFrags[i].size();
    if (deg<2)
      continue; //nodes

    if (deg==2){ //degree 2 segments will trigger a splice

      //standard operation: extract them from the graph, reorder them, either merge or put them back

      //segments need to meet continuity criteria (simple one for now)
      if (curve_frag_graph_.pFrags[i].size()>1){
        auto fit = curve_frag_graph_.pFrags[i].begin();
        c1 =  (*fit); fit++;
        c2 =  (*fit);

        curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);

        //reverse the sequence of edgels
        std::reverse(c2->edgels.begin(), c2->edgels.end());
        curve_frag_graph_.insert_fragment(c1);
        curve_frag_graph_.insert_fragment(c2);
      }
      else if (curve_frag_graph_.pFrags[i].size()==1){
        c1 =  curve_frag_graph_.pFrags[i].front();
        c2 =  curve_frag_graph_.cFrags[i].front();
    //for the closed contour case
    if(c1==c2)
        continue;
      }
      else {
        auto fit = curve_frag_graph_.cFrags[i].begin();
        c1 =  (*fit); fit++;
        c2 =  (*fit);

        //add the second one to the first one and delete it from the graph
        curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);

        //reverse the sequence of edgels
        std::reverse(c1->edgels.begin(), c1->edgels.end());
        curve_frag_graph_.insert_fragment(c1);
        curve_frag_graph_.insert_fragment(c2);
      }

      if (is_continue(c1,c2)){ //if two contours are generally/local continue based on a frag
        //merge the two contours
            curve_frag_graph_.extract_fragment(c1);
        curve_frag_graph_.extract_fragment(c2);
        c1->append(c2->edgels);
        curve_frag_graph_.insert_fragment(c1);
    //when it makes a closed contour, just count as the child frag rather than parent frag
        if(c1->edgels.front()==c1->edgels.back())
        curve_frag_graph_.pFrags[c1->edgels.front()->id].remove(c1);
        delete c2;
      }
     }
  }

  // deal with junction with another loop after dealing with deg 2

  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel_chain *c1=nullptr, *c2=nullptr;
    sdet_edgel* eA = edgemap_->edgels[i];

    int deg = curve_frag_graph_.pFrags[i].size()+ curve_frag_graph_.cFrags[i].size();
    //degree 3 is a junction (T-junction or Y-junction)
    if (deg>2) //  Make length of contour as the first priority
    {
        //goal is to see if any two will produce smooth continuation
    sdet_edgel_chain_list node_frags;
        if(curve_frag_graph_.pFrags[i].size()!=0){
        auto p_fit = curve_frag_graph_.pFrags[i].begin();
            for(;p_fit!=curve_frag_graph_.pFrags[i].end();p_fit++)
        {
            node_frags.push_back(*p_fit);
        }
    }

        if(curve_frag_graph_.cFrags[i].size()!=0){
        auto c_fit = curve_frag_graph_.cFrags[i].begin();
            for(;c_fit!=curve_frag_graph_.cFrags[i].end();c_fit++)
        {
            node_frags.push_back(*c_fit);
        }
    }

    node_frags.sort(is_longer); //sort all the pfrags and cfrags in length

        //compare each pair to decide merge or not
        auto fit_1=node_frags.begin();
    for (;fit_1!=--node_frags.end();){
        c1= *fit_1;

        if(c1->edgels.back()!= eA){
            curve_frag_graph_.extract_fragment(c1);
            std::reverse(c1->edgels.begin(), c1->edgels.end());
            curve_frag_graph_.insert_fragment(c1);
        }
        fit_1++;
        auto fit_2 = fit_1, max_fit = fit_1;
        double max_SM = 0;
        for (;fit_2!=node_frags.end();fit_2++){
            c2=*fit_2;
            if(c2->edgels.back()== eA){
                curve_frag_graph_.extract_fragment(c2);
                std::reverse(c2->edgels.begin(), c2->edgels.end());
                curve_frag_graph_.insert_fragment(c2);
            }

            double SM_0 = get_continuity(c1,c2);
            if(SM_0>max_SM){
                max_SM = SM_0;
                max_fit = fit_2;
            }
        }
        if(max_SM>=0.9){
            c2=*max_fit;
            curve_frag_graph_.extract_fragment(c1);
            curve_frag_graph_.extract_fragment(c2);
            c1->append(c2->edgels);
            curve_frag_graph_.insert_fragment(c1);
            break;
        }

        //if(fit_2!=node_frags.end())
        //    break;
        }
    }
  }
    // use the filter to prun out local problems again
    regular_contour_filter();
    std::cout<<"Finish correcting the CFG topology"<<std::endl;
}

//20th June, 2012:: New Merging of Contours Decision Making by Naman Kumar,function to test whether two contours are continuous
static bool is_continue (const sdet_edgel_chain *c1, const sdet_edgel_chain *c2)
{
         sdet_edgel* e1;sdet_edgel* e4;
        sdet_edgel* e2 = c1->edgels.back();
        sdet_edgel* e3 = c2->edgels.front();
        double dx1=0,dy1=0,dy2=0,dx2=0,s1=0,s2=0,s=0,SM_1=0;
        int j=0;
        for(int i=c1->edgels.size()-2;i>=0;i--)
        {
                e1 = c1->edgels[i];
                    j++;
                    if(j==c2->edgels.size()) break;
                e4 = c2->edgels[j];
                dx1 = e2->pt.x()-e1->pt.x();
                dy1 = e2->pt.y()-e1->pt.y();
                dx2 = e4->pt.x()-e3->pt.x();
                dy2 = e4->pt.y()-e3->pt.y();
                s1+=(e1)->strength;
                s2+=(e4)->strength;
                s=std::fabs((s1-s2)/j);
                SM_1 = (dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2);
                if(SM_1>=Theta_1 || s<=Strength_Diff1) return true;
                else if(SM_1<Theta_2 || s>Strength_Diff2) return false;
                else continue;
        }
            return false;
}

static double get_continuity (const sdet_edgel_chain *c1, const sdet_edgel_chain *c2)
{
        // using the median global continuity
        sdet_edgel* e1=nullptr;
        sdet_edgel* e2=nullptr;
        sdet_edgel* e3=nullptr;
        sdet_edgel* e4=nullptr;
        if(c1->edgels.size()>=5){
            e1 = c1->edgels[c1->edgels.size()-5];
            e2 = c1->edgels.back();
        }
        else{
            e1 = c1->edgels.front();
            e2 = c1->edgels.back();
        }
        if(c2->edgels.size()>=5){
            e3 = c2->edgels.front();
            e4 = c2->edgels[4];
        }
        else{
            e3 = c2->edgels.front();
            e4 = c2->edgels.back();
        }
        double dx1 = e2->pt.x()-e1->pt.x();
        double dy1 = e2->pt.y()-e1->pt.y();
        double dx2 = e4->pt.x()-e3->pt.x();
        double dy2 = e4->pt.y()-e3->pt.y();
        return (dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2);
}

static bool share_same_ends(sdet_edgel_chain *c1, sdet_edgel_chain *c2)
{
    if((c1->edgels.front()==c2->edgels.front()&&c1->edgels.back()==c2->edgels.back())
        || (c1->edgels.front()==c2->edgels.back()&&c1->edgels.back()==c2->edgels.front()))
        return true;
    return false;
}



void sdet_sel_base::regular_contour_filter(){
// first, deal with contours with length 3, which cause a lot of local problems
    sdet_edgel_chain_list Size_3_chain_list;
    auto fit = curve_frag_graph_.frags.begin();
    for(;fit!=curve_frag_graph_.frags.end();fit++)
    {
        sdet_edgel_chain *c1=*fit;
    // (a) for size 4 frags, git rid of the small closed triangle
    if(c1->edgels.size()==4 && (c1->edgels.front()==c1->edgels.back()))
    {
        c1->edgels.pop_back();
        curve_frag_graph_.pFrags[c1->edgels.back()->id].push_back(c1);
    }
    // push size 3 frags's pointer into a seperate list to same computation for next step
    if(c1->edgels.size()==3)
        Size_3_chain_list.push_back(c1);
    }


    auto fit_1 = Size_3_chain_list.begin();
    while(fit_1!=Size_3_chain_list.end())
    {
        // (b) change 3 edgels sharp path to be a direct line link
    sdet_edgel_chain *c1=*fit_1;
    double dx1 = c1->edgels[1]->pt.x() - c1->edgels.front()->pt.x();
    double dy1 = c1->edgels[1]->pt.y() - c1->edgels.front()->pt.y();
    double dx2 = c1->edgels.back()->pt.x() - c1->edgels[1]->pt.x();
    double dy2 = c1->edgels.back()->pt.y() - c1->edgels[1]->pt.y();
    double SM = (dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2);
    double length = std::sqrt(dx1*dx1+dy1*dy1) + std::sqrt(dx2*dx2+dy2*dy2);
    if(length>10)// only consider local problems
        continue;
    if(SM<=0)
    {
        auto eit = c1->edgels.begin();
        eit++;
        c1->edgels.erase(eit);
    }
    // (c) if two frags share same end nodes, change them into one direct line link
    auto fit_2 = ++fit_1;
    while(fit_2!=Size_3_chain_list.end())
    {
        sdet_edgel_chain *c2=*fit_2;
        if(share_same_ends(c1, c2))
        {
            // check if c1 is modifid in (b), if no, change it to a direct line link
            if(c1->edgels.size()==3)
            {
                auto eit = c1->edgels.begin();
                eit++;
                c1->edgels.erase(eit);
            }
            // remove c2 from CFG
            curve_frag_graph_.extract_fragment(c2);
            // only deal with one frag has same ends, if there are more, leave for iterations afterward
            break;
        }
        fit_2++;
    }
    }
}


//New construction of Hypothesis Tree by Naman Kumar
void sdet_sel_base::Construct_Hypothesis_Tree()
{
  regular_contour_filter();
  int n1=0;
  std::vector<sdet_edgel*> new_chain0,new_chain2,new_chain3,new_chain6,new_chain33;
  auto* new_chain1=new sdet_edgel_chain();
  auto* new_chain4=new sdet_edgel_chain();
  double gap_thres=gap_;
  std::cout << "Construction of Hypothesis Tree is in Progress!! " << std::endl;
  // Calculating number of edges with degree as 1
  for (auto eA1 : edgemap_->edgels)
    {
    new_chain0.push_back(eA1);
    if ((curve_frag_graph_.pFrags[eA1->id].size() + curve_frag_graph_.cFrags[eA1->id].size()) ==1)
      new_chain1->edgels.push_back(eA1);
    else new_chain2.push_back(eA1);
    }

    //Calculating number of edges which are part of the contours and which are unused
    auto fit = curve_frag_graph_.frags.begin();
    for (; fit!=curve_frag_graph_.frags.end(); ++fit)
      {
      sdet_edgel_chain *test1=*fit;
      for (auto edgel : test1->edgels)
        {
        new_chain3.push_back(edgel);new_chain33.push_back(edgel);
        }
      }

      for (auto & i : new_chain0)
        {
        for (auto & j : new_chain3)
          {
          if(i!=j) continue;
          else
            {
            n1=5;
            break;
            }
          }
          if (n1==0)
            {
            new_chain4->edgels.push_back(i);
            }
          else
            {
            n1=0;
            continue;
            }
        }
        //Constucting the tree from end of an unambiguous chain and extending it till the end of edge chain
      double cost2=10.0, d1=0.0,dx=0.0,dy=0.0,cost=1000.0,costc=0.0;
      int m1=0,m2=0,m3=0,m4=0,m5=0,m7=0,m8=0,m9=0;
      sdet_edgel* ce=nullptr;sdet_edgel* pe=nullptr;sdet_edgel* ed=nullptr;sdet_edgel* imp=nullptr;sdet_edgel* im=nullptr;
      auto *c11=new sdet_edgel_chain();
      auto* xx=new sdet_edgel_chain();
      auto* end=new sdet_edgel_chain();
      while(new_chain1->edgels.size()>0)
        {
        a: ;
        if (0 == new_chain1->edgels.size()) break;
        ed=new_chain1->edgels[0];
        new_chain1->edgels.pop_front();
        for (auto & edgel : end->edgels)
          {
          if (ed==edgel) goto a;
          else continue;
          }
          auto* new_chain5 = new sdet_edgel_chain();
          new_chain5->edgels.push_back(ed);xx->edgels.push_back(ed);
          m4=0;m7=0;
          double dis=0, distance=0;
          //Forming the tree from the edge
          while (true)
            {
            auto eit1=new_chain4->edgels.begin();
            auto eit2=new_chain4->edgels.begin();
            if (m4==0)
              {
              if (1 == curve_frag_graph_.cFrags[ed->id].size())
                {
                auto ccit = curve_frag_graph_.cFrags[ed->id].begin();
                ce = (*ccit)->edgels[1];c11=*ccit;pe=ce;m7=1;
                for (unsigned int j=1; j<c11->edgels.size(); ++j)
                  {
                  dis=vgl_distance(c11->edgels[j]->pt,c11->edgels[j-1]->pt);
                  if (dis>distance) distance=dis;
                  }
                distance=distance + 0.25;
                if (distance <=1) gap_=1;
                else if (distance <gap_thres) gap_=distance;
                else gap_=gap_thres;
                }
              else if (1 == curve_frag_graph_.pFrags[ed->id].size())
                {
                auto pcit = curve_frag_graph_.pFrags[ed->id].begin();
                ce = (*pcit)->edgels[(*pcit)->edgels.size()-2];
                c11=*pcit;
                pe=ce;
                m7=2;
                for (unsigned int j=1; j<c11->edgels.size(); ++j)
                   {
                   dis=vgl_distance(c11->edgels[j]->pt,c11->edgels[j-1]->pt);
                   if (dis>distance) distance=dis;
                   }
                distance=distance + 0.25;
                if (distance <=1) gap_=1;
                else if (distance <gap_thres) gap_=distance;
                else gap_=gap_thres;
                }
              m4=1;
              }
              // Used later
              if (m7==2)
                {
                m8=c11->edgels.size()-5;
                if (m8<0) m8=0;
                m9=c11->edgels.size();
                }
              else if(m7==1)
                {
                m8=0;
                m9=5;
                if (m9>static_cast<int>(c11->edgels.size()))
                  m9=c11->edgels.size();
                }
              // Finding the closest unused edge
              cost=10000.0;
              double cost1=gap_;
              for (auto & edgel : new_chain4->edgels)
                {
                d1= vgl_distance(ed->pt,edgel->pt);
                //Checking Localization, Orientation,etc..
                if (d1<cost1)
                  {
                  std::vector<sdet_edgel*> dummy_chain;
                  auto* edgel_chain = new sdet_edgel_chain();
                  for (auto i : new_chain5->edgels)
                    edgel_chain->edgels.push_back(i);
                  edgel_chain->edgels.push_back(edgel);
                  std::vector<sdet_edgel*> chain(edgel_chain->edgels.begin(),edgel_chain->edgels.end());
                  costc = compute_path_metric2(dummy_chain, chain, dummy_chain);
                  if (costc<cost)
                    {
                    double d8=vgl_distance(edgel->pt,ce->pt);
                    double d9=vgl_distance(edgel->pt,ed->pt);
                    double d0=vgl_distance(edgel->pt,pe->pt);
                    double dx1 = ce->pt.x() - ed->pt.x();
                    double dy1 = ce->pt.y() - ed->pt.y();
                    double dx2 = ed->pt.x() - edgel->pt.x();
                    double dy2 = ed->pt.y() - edgel->pt.y();
                    double angle=((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2));
                    if (d0<d9)
                      {
                      ++eit1;
                      continue;
                      }
                    if (d8<d9 || angle<0)
                      {
                      ++eit1;
                      continue;
                      }
                    imp=edgel;
                    cost=costc;
                    m1=1;
                    eit2=eit1;
                    }
                  }
                else if (d1<cost2 && d1>1)
                  {
                  cost2=d1;
                  }
                ++eit1;
                }

              m3=0;m5=0;
              double cost3=gap_;
              // Finding the closest edge which is part of a fragment
              for (auto & t : new_chain3)
                {
                if (t==ed || t==ce) continue;
                d1 = vgl_distance(ed->pt,t->pt);
                if (d1<=cost3)
                  {
                  //Dont consider the previous 5 edges present in parent/child fragment of the starting edge
                  for (int c=m8;c<m9;c++)
                    {
                    if (t==c11->edgels[c]) goto z;
                    else continue;
                    }
                  //Dont use the edge which is part of the same tree again
                  for (auto & edgel : new_chain5->edgels)
                    {
                    if (t==edgel) goto z;
                    else continue;
                    }
                  im=t;
                  cost3=d1;m5=1;
                  dx=vgl_distance(im->pt,ce->pt);
                  dy=vgl_distance(im->pt,ed->pt);
                  }
                z: ;
                }
              if (dx>dy && m5==1)
                {
                m3=5;
                m1=1;
                imp=im;
                }
              if (m1==1)
                {
                m2=1;
                m1=0;
                ce=ed;
                ed=imp;
                xx->edgels.push_back(imp);
                new_chain5->edgels.push_back(imp);
                if (m3==0)
                  {
                  new_chain4->edgels.erase(eit2);
                  new_chain3.push_back(imp);
                  new_chain33.push_back(imp);
                  }
                if (m3!=0)
                  {
                  break;
                  }
                }
              else if (cost2>1) break;
            }
          //No double contours within the same 2 end points.
          if (m2==1)
            {
            if (c11->edgels.front()==c11->edgels.back()) continue;
            }
          //Add the tree
          new_chain5->temp = true;
          curve_frag_graph_.CFTG.insert_fragment(new_chain5);
          end->edgels.push_back(new_chain5->edgels.back());

        }
        sdet_edgel* edge1=nullptr;
        sdet_edgel* edge2=nullptr;
        auto *chain1=new sdet_edgel_chain();
        std::list<sdet_CFTG_link*> GD_list;
        double p1=1.0;
        int p2=0,p3=0,p16=0;
        auto l_it = curve_frag_graph_.CFTG.Links.begin();
        for (; l_it != curve_frag_graph_.CFTG.Links.end(); ++l_it)
          GD_list.push_back(*l_it);
        while (GD_list.size()>0)
          {
          sdet_CFTG_link* cur_Link = GD_list.front();
          GD_list.pop_front();
          auto f_it = cur_Link->cCFs.begin();
          sdet_edgel_chain* new_chain5=(*f_it);
          sdet_edgel* edge3=new_chain5->edgels.front();
          gap_=1;
          for (unsigned int i=0; i<new_chain5->edgels.size(); ++i)
            {
            p1=gap_;
            auto eit5=new_chain4->edgels.begin();
            auto eit6=new_chain4->edgels.begin();
            for (auto & edgel : new_chain4->edgels)
              {
              double p4=vgl_distance(new_chain5->edgels[i]->pt,edgel->pt);
              if (p4<p1)
                {
                edge1=edgel;
                p1=p4;
                p2=1;
                eit6=eit5;
                ++eit5;
                }
              else
                {
                ++eit5;
                continue;
                }
              }
            if (p2==1)
              {
              new_chain4->edgels.erase(eit6);
              sdet_edgel* edge4=nullptr;p2=0;double p5=gap_;
              for (auto & edgel : new_chain5->edgels)
                {
                double p6=vgl_distance(edge1->pt,edgel->pt);
                if (p6<p5)
                  {
                  edge2=edgel;
                  p5=p6;
                  }
                else continue;
                }
                auto *new_chain6a = new sdet_edgel_chain();
                new_chain6a->edgels.push_back(edge2); new_chain6a->edgels.push_back(edge1);
                int p7=0,p8=0,p9=0;
                if (curve_frag_graph_.cFrags[edge3->id].size()>=1)
                  {
                  auto ccit = curve_frag_graph_.cFrags[edge3->id].begin();
                  chain1=(*ccit);
                  p7=1;
                  }
                else if (curve_frag_graph_.pFrags[edge3->id].size()>=1)
                  {
                  auto pcit = curve_frag_graph_.pFrags[edge3->id].begin();
                  chain1=(*pcit);
                  p7=2;
                  }
                if (p7==2)
                  {
                  p8=chain1->edgels.size()-5;
                  if (p8<0) p8=0;
                  p9=chain1->edgels.size();
                  }
                else if (p7==1)
                  {
                  p8=0;
                  p9=5;
                  if (p9>static_cast<int>(chain1->edgels.size())) p9=chain1->edgels.size();
                  }
                while (true)
                  {
                  double p10=gap_,p11=0.0;
                  auto eit3=new_chain4->edgels.begin();auto eit4=new_chain4->edgels.begin();
                  for (auto & edgel : new_chain4->edgels)
                    {
                    p11= vgl_distance(edge1->pt,edgel->pt);
                    if (p11<p10)
                      {
                      double d8=vgl_distance(edgel->pt,edge2->pt);
                      double d9=vgl_distance(edgel->pt,edge1->pt);
                      double dx1 = edge1->pt.x() - edge2->pt.x(), dy1 = edge1->pt.y() - edge2->pt.y();
                      double dx2=edgel->pt.x()-edge1->pt.x();
                      double dy2=edgel->pt.y()-edge1->pt.y();
                      if (d8<d9 || ((dx1*dx2 + dy1*dy2)/std::sqrt(dx1*dx1+dy1*dy1)/std::sqrt(dx2*dx2+dy2*dy2))<0.4)
                        {
                        ++eit3;
                        continue;
                        }
                      p10=p11;edge4=edgel;eit4=eit3;p2=1;
                      }
                    ++eit3;
                    }
                    double p12=gap_,p13=0,p14=0.0,p15=0.0;p16=0;sdet_edgel* edge5=nullptr;
                    for (auto & t : new_chain3)
                      {
                      if (t==edge1) continue;
                      p13= vgl_distance(edge1->pt,t->pt);
                      if (p13<=p12)
                        {
                        for (int c=p8; c<p9; ++c)
                          {
                          if (t==chain1->edgels[c])
                            goto jump;
                          else continue;
                          }
                        for (auto & edgel : new_chain5->edgels)
                          {
                          if (t==edgel) goto jump;
                          else continue;
                          }
                        for (auto & edgel : new_chain6a->edgels)
                          {
                          if (t==edgel) goto jump;
                          else continue;
                          }
                        edge5=t;p12=p13;p14=vgl_distance(edge5->pt,edge2->pt);
                        p15=vgl_distance(edge5->pt,edge1->pt);
                        p16=1;
                        }
                        jump: ;
                      }
                    if (p14>p15 && p16==1)
                      {
                      edge4=edge5;
                      p2=1;
                      }
                    if (p2==1)
                      {
                      new_chain6a->edgels.push_back(edge4);p3=1;p2=0;
                      if (p16==0)
                        {
                        new_chain4->edgels.erase(eit4);
                        new_chain3.push_back(edge4);
                        edge2=edge1;
                        edge1=edge4;
                        }
                      else break;
                      }
                    else break;
                  }
                double p17=0,p18=0,p21=0;
                if (p3==1 && new_chain6a->edgels.size()>5)
                  {
                  sdet_edgel* edge6=new_chain6a->edgels[new_chain6a->edgels.size()/2];
                  for (auto & i : new_chain33)
                    {
                    p18=vgl_distance(edge6->pt,i->pt);
                    if (p18<1) p21=10;
                    if (p21==10)
                      {
                      p17=1;
                      break;
                      }
                    }
                  if (p17==0)
                    {
                    new_chain6a->temp = true;
                    curve_frag_graph_.CFTG.insert_fragment(new_chain6a);
                    }
                  p2=0;
                  p3=0;
                  }
              }
            }
          }
          std::cout << "Hypothesis Tree Constructed!!" << std::endl;
}

// New Disambiguation Process by Naman Kumar
void sdet_sel_base::Disambiguation()
{
        std::cout << "Disambiguating the Hypothesis Tree!!" << std::endl;
        auto l_it = curve_frag_graph_.CFTG.Links.begin();
        for (; l_it != curve_frag_graph_.CFTG.Links.end(); l_it++)
        {
                double cost=0.0;
                int deg_S = curve_frag_graph_.CFTG.cLinks[(*l_it)->eS->id].size() + curve_frag_graph_.CFTG.pLinks[(*l_it)->eS->id].size();
                sdet_CFTG_link* cur_Link = (*l_it);
                //Calculating Cost
                std::vector<sdet_edgel*> dummy_chain;
                sdet_edgel_chain* edgel_chain = cur_Link->cCFs.front();
                std::vector<sdet_edgel*> chain(edgel_chain->edgels.begin(),edgel_chain->edgels.end());
                 cost = compute_path_metric2(dummy_chain, chain, dummy_chain);
                //Degree=1
                if(deg_S==1) {curve_frag_graph_.insert_fragment((*l_it)->cCFs.front()); continue;}
                //Degree>1
                // To fill small gaps in closed contours
                if((curve_frag_graph_.pFrags[edgel_chain->edgels.front()->id].size()+curve_frag_graph_.cFrags[edgel_chain->edgels.front()->id].size())==1                            &&(curve_frag_graph_.pFrags[edgel_chain->edgels.back()->id].size()+curve_frag_graph_.cFrags[edgel_chain->edgels.back()->id].size())==1                         && edgel_chain->edgels.size()==2) curve_frag_graph_.insert_fragment(edgel_chain);
                if(cost<1.0 && edgel_chain->edgels.size()>2) curve_frag_graph_.insert_fragment(edgel_chain);
        }
        //clear the graph
        curve_frag_graph_.CFTG.clear();
        curve_frag_graph_.CFTG.resize(edgemap_->edgels.size());
}

// By Naman Kumar: a minor function just to prune some extra small part of contours
void sdet_sel_base::Post_Process()
{

        auto* new_chain= new sdet_edgel_chain();
        for (auto eA1 : edgemap_->edgels)
            {
                if ((curve_frag_graph_.pFrags[eA1->id].size() + curve_frag_graph_.cFrags[eA1->id].size()) ==1) new_chain->edgels.push_back(eA1);
            }
            for (auto edge : new_chain->edgels)
              {
              sdet_edgel* edge2=nullptr;auto* chain= new sdet_edgel_chain();sdet_edgel* edge3=nullptr;
              int n=0,number=0,diff=0;
              if (curve_frag_graph_.cFrags[edge->id].size()==1)
                {
                n=1;
                auto ccit = curve_frag_graph_.cFrags[edge->id].begin();
                chain=*ccit;
                edge2 = chain->edgels[1];
                if (chain->edgels.size()>2)
                  {
                  number=1;
                  edge3=chain->edgels[2];
                  }
                diff=1;
                }
              else if(curve_frag_graph_.pFrags[edge->id].size()==1)
                {
                n=1;auto pcit = curve_frag_graph_.pFrags[edge->id].begin();chain=*pcit;
                edge2 = chain->edgels[chain->edgels.size()-2];
                if (chain->edgels.size()>2)
                  {
                  number=1;
                  edge3=chain->edgels[chain->edgels.size()-3];
                  }
                diff=2;
                }
                auto fit = curve_frag_graph_.frags.begin();
                for (;fit!=curve_frag_graph_.frags.end();++fit)
                  {
                  sdet_edgel_chain *test1=*fit;
                  if (test1==chain) continue;
                  for (auto & edgel : test1->edgels)
                    {
                    if (edge==edgel) goto end;
                    }
                  }
                if (n==1 && chain->edgels.size()>1 && ((curve_frag_graph_.pFrags[edge2->id].size() + curve_frag_graph_.cFrags[edge2->id].size()) >=1))
                  {
                  curve_frag_graph_.extract_fragment(chain);
                  if (diff==1)
                    chain->edgels.pop_front();
                  else if (diff==2)
                    chain->edgels.pop_back();
                  curve_frag_graph_.insert_fragment(chain);
                  }
                else if(number==1 && chain->edgels.size()>1 && ((curve_frag_graph_.pFrags[edge3->id].size()+curve_frag_graph_.cFrags[edge3->id].size()) >=1))
                  {
                  curve_frag_graph_.extract_fragment(chain);
                  if (diff==1)
                    {
                    chain->edgels.pop_front();
                    chain->edgels.pop_front();
                    }
                  else if (diff==2)
                    {
                    chain->edgels.pop_back();
                    chain->edgels.pop_back();
                    }
                  curve_frag_graph_.insert_fragment(chain);
                  }
                end: ;
              }
}
