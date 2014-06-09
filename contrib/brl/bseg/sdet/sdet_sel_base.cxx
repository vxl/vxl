#include "sdet_sel_base.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_deque.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_algorithm.h>
#include <pdf1d/pdf1d_calc_mean_var.h>
#include <mbl/mbl_stats_1d.h>

#include "sdet_edgemap.h"

//: Constructor
sdet_sel_base::sdet_sel_base(sdet_edgemap_sptr edgemap, 
                               sdet_curvelet_map& cvlet_map, 
                               sdet_edgel_link_graph& edge_link_graph, 
                               sdet_curve_fragment_graph& curve_frag_graph,
                               sdet_curvelet_params cvlet_params) :
edgemap_(edgemap), curvelet_map_(cvlet_map), 
edge_link_graph_(edge_link_graph), curve_frag_graph_(curve_frag_graph),
nrows_(edgemap->height()), ncols_(edgemap->width()),
app_usage_(0), app_thresh_(2),
rad_(cvlet_params.rad_), gap_(cvlet_params.gap_),dtheta_(cvlet_params.dtheta_*vnl_math::pi/180), dpos_(cvlet_params.dpos_), 
badap_uncer_(cvlet_params.badap_uncer_),
token_len_(cvlet_params.token_len_), max_k_(cvlet_params.max_k_), max_gamma_(cvlet_params.max_gamma_),
nrad_((unsigned) vcl_ceil(rad_)+1), maxN_(2*nrad_),
centered_(cvlet_params.centered_), bidir_(cvlet_params.bidirectional_),
use_anchored_curvelets_(true),
min_deg_to_link_(4), 
use_hybrid_(false),
DHT_mode_(true), propagate_constraints(true)
{
  //save the parameters in the curvelet map
  curvelet_map_.set_edgemap(edgemap);
  curvelet_map_.set_parameters(cvlet_params);
}

//:destructor
sdet_sel_base::~sdet_sel_base()
{
}

//********************************************************************//
// User Friendly functions
//********************************************************************//

//: use the recommended sub-algorithms to extract final contour set (naive users should call this function)
void sdet_sel_base::extract_image_contours()
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
void sdet_sel_base::build_pairs()
{
  vcl_cout << "Building pairs ...";
  vcl_cout.flush();

  //vcl_ofstream outfp("pair_distribution.txt", vcl_ios::out);

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
    //outfp << cnt <<vcl_endl;
  }
    
  //close file
  //outfp.close();

  vcl_cout << "done!" << vcl_endl;
}


//: form curvelets around each edgel in a greedy fashion
void sdet_sel_base::build_curvelets_greedy(unsigned max_size_to_group,bool use_flag, bool clear_existing, bool verbose)
{
  if (verbose){
    vcl_cout << "Building All Possible Curvelets (Greedy) ..." ;
    vcl_cout.flush();
  }

  if (clear_existing){
    //clear the curvelet map
    curvelet_map_.clear();

    //form a new curvelet map
    curvelet_map_.resize(edgemap_->num_edgels());

    if (verbose){
      vcl_cout << "curvelet map cleared..." ;
      vcl_cout.flush();
    }
  }

  //store this parameter
  maxN_ = max_size_to_group;

  if (centered_) {
    if (bidir_) {
      for (unsigned i=0; i<edgemap_->edgels.size(); i++) {
        sdet_edgel* eA = edgemap_->edgels[i];
        // centered_ && bidir_
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, false); //first in the forward direction
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, false, centered_, false); //then in the other direction
      }
    } else {
      for (unsigned i=0; i<edgemap_->edgels.size(); i++) {
        sdet_edgel* eA = edgemap_->edgels[i];
        // centered_ && !bidir_
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, false); //first in the forward direction
      }
    }
  } else {
    if (bidir_) {
      for (unsigned i=0; i<edgemap_->edgels.size(); i++) {
        sdet_edgel* eA = edgemap_->edgels[i];
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, true); //forward half
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, false, centered_, true); //backward half 
        // !centered_ && bidir_
      }
    } else {
      for (unsigned i=0; i<edgemap_->edgels.size(); i++) {
        sdet_edgel* eA = edgemap_->edgels[i];
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, true); //forward half
        build_curvelets_greedy_for_edge(eA, max_size_to_group, use_flag, true, centered_, false); //ENO style forward
        // !centered_ && !bidir_
      }
    }
  }

  if (verbose)
    vcl_cout << "done!" << vcl_endl;
}


//: form the full curvelet map (curvelet map lists all the curvelets it participated in and not just the ones anchored to it)
void sdet_sel_base::form_full_cvlet_map()
{
  // This method forms a curvelet map which is a mapping from each edgel to all the 
  // curvelets it participates in and not just the ones anchored to it

  for (unsigned i=0; i<edgemap_->edgels.size(); i++){
    sdet_edgel* eA = edgemap_->edgels[i];

    //add all the curvelets anchored at this edgel to all the other edgels in it
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
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
        sdet_curvelet_list_iter cv_it2 = curvelet_map_.curvelets(eB->id).begin();
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
bool sdet_sel_base::curvelet_is_balanced(sdet_edgel* ref_e, vcl_deque<sdet_edgel*> &edgel_chain)
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


void sdet_sel_base::recompute_curvelet_quality()
{
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++)
      (*cv_it)->compute_properties(rad_, token_len_);
  }
}

//: prune the curvelets with gaps larger than the one specified
void sdet_sel_base::prune_curvelets_by_gaps(double gap_threshold)
{
  //go through the curvelet map and remove all the curvelets below threshold

  //1) first clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over the curvelets in the curvelet map
  vcl_vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
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
  for (unsigned i=0; i<cvlets_to_del.size(); i++)
    curvelet_map_.remove_curvelet(cvlets_to_del[i]);
  cvlets_to_del.clear();
}

//: prune the curvelets with lengths (extent) smaller than the one specified
void sdet_sel_base::prune_curvelets_by_length(double length_threshold)
{
  //go through the curvelet map and remove all the curvelets below threshold

  //1) first clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over the curvelets in the curvelet map
  vcl_vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      sdet_curvelet* cvlet = (*cv_it);

      if (cvlet->length<length_threshold)
        cvlets_to_del.push_back(cvlet);
    }
  }

  //now actually delete them
  for (unsigned i=0; i<cvlets_to_del.size(); i++)
    curvelet_map_.remove_curvelet(cvlets_to_del[i]);
  cvlets_to_del.clear();
}

//: prune the curvelets that are below the quality threshold and hence considered spurious
void sdet_sel_base::prune_the_curvelets(double quality_threshold)
{
  //go through the curvelet map and remove all the curvelets below threshold

  //1) first clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over the curvelets in the curvelet map
  vcl_vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      if ((*cv_it)->quality<quality_threshold)
        cvlets_to_del.push_back(*cv_it);
    }
  }

  //now actually delete them
  for (unsigned i=0; i<cvlets_to_del.size(); i++)
    curvelet_map_.remove_curvelet(cvlets_to_del[i]);
  cvlets_to_del.clear();
}

//: prne the curvelets that are not locally geometrically consistent (i.e., c1)
void sdet_sel_base::prune_curvelets_by_c1_condition()
{
  //Note: this is currently only meaningful for GENO style curvelets
  //goal: to locally do a test of viability beyond the local neighborhood by looking for viability across 
  //two neighborhoods using the c1 test (or biarc condition)

  //1) first clear the link graph so that new one can be formed later
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  //go over all the edgels and look at the curvelets on it
  vcl_vector<sdet_curvelet*> cvlets_to_del;
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    //for each cvlet before the edgel, see if a c1 cvlet can be found after it
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++)
    {
      sdet_curvelet* cvlet1 = (*cv_it);
      bool before = (cvlet1->edgel_chain.back()==eA);
      bool c1_pair_found = false;
      bool cvlet2_found = false;

      //go over all the cvlet after
      sdet_curvelet_list_iter cv_it2 = curvelet_map_.curvelets(i).begin();
      for ( ; cv_it2!=curvelet_map_.curvelets(i).end(); cv_it2++)
      {
        sdet_curvelet* cvlet2 = (*cv_it2);
        if (cvlet2 == cvlet1) continue;
        cvlet2_found = true;
        
        //do the c1 test
        if ( before && cvlet2->edgel_chain.front()==eA ||
            !before && cvlet2->edgel_chain.back()==eA)
          c1_pair_found = c1_pair_found || cvlet1->curve_model->is_C1_with(cvlet2->curve_model);
      }

      if (cvlet2_found && !c1_pair_found) //mark for deletion
        cvlets_to_del.push_back(cvlet1);
    }
  }

  //now actually delete them
  for (unsigned i=0; i<cvlets_to_del.size(); i++)
    curvelet_map_.remove_curvelet(cvlets_to_del[i]);
  cvlets_to_del.clear();

}


//: construct a simple link grpah by connectng edgels to all its neighbors
void sdet_sel_base::construct_naive_link_graph(double proximity_threshold, double affinity_threshold)
{
  //1) clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());

  unsigned R = (unsigned) vcl_ceil(proximity_threshold);

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
        for (unsigned k=0; k<edgemap_->cell(xx, yy).size(); k++){
          sdet_edgel* eB = edgemap_->cell(xx, yy)[k];
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
            edge_link_graph_.link(eA, eB, 0);
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
void sdet_sel_base::construct_the_link_graph(unsigned min_group_size, int method)
{
  //1) clear the link graph and form a new one
  edge_link_graph_.clear();
  edge_link_graph_.resize(edgemap_->num_edgels());
  if (!use_anchored_curvelets_){
    // First update the curvelet map to include all links
    form_full_cvlet_map();
  }

  // 2) now construct the link graph from the curvelet map
  vcl_cout << "Constructing the Link Graph using (N >= " << min_group_size << ")..." ;

  // 2a) go over all the curvelets and reset the used flags
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    //for all curvelets that are larger than the group size threshold 
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++)
      (*cv_it)->used = false; //reset this flag
  }

  // 2b) go over all the curvelets above the min size and determine which links can be formed
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    //for all curvelets that are larger than the group size threshold 
    sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(i).begin();
    for ( ; cv_it!=curvelet_map_.curvelets(i).end(); cv_it++){
      sdet_curvelet* cvlet = (*cv_it);
      if (cvlet->order() < min_group_size) continue;

      //form all possible links from this curvelet
      form_links_from_a_curvelet(eA, cvlet, min_group_size, method);
    }
  }
  vcl_cout << "done!" << vcl_endl;

  ////3) prune duplicate curvelets (Assuming that once the curvelets are formed, they are independent)
  //for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++){
  //  //all child links of each edgel covers all the links
  //  sdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
  //  for (; l_it != edge_link_graph_.cLinks[i].end(); l_it++){
  //    (*l_it)->prune_redundant_curvelets();
  //  }
  //}

  //4) after forming the link graph, determine the degree of overlap of the links in the graph
  for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++){
    //all child links of each edgel covers all the links
    sdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
    for (; l_it != edge_link_graph_.cLinks[i].end(); l_it++){
      (*l_it)->deg_overlap = count_degree_overlap((*l_it));
    }
  }

}

//: count the degree of overlap between pairs of curvelets at a link
int sdet_sel_base::count_degree_overlap(sdet_link* link)
{
  int max_deg = 0;

  //go over all pairs of curvelets causing the link
  sdet_curvelet_list_iter c_it = link->curvelets.begin();
  for (; c_it != link->curvelets.end(); c_it++){
    sdet_curvelet_list_iter c_it2 = c_it; 
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
int sdet_sel_base::count_degree_overlap(sdet_curvelet* cvlet1, sdet_curvelet* cvlet2, unsigned k1, unsigned k2)
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
      sdet_edgel *eX=0, *eY=0;
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
      sdet_edgel *eX=0, *eY=0;
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
bool sdet_sel_base::link_is_reciprocal(sdet_edgel* eA, sdet_edgel* eB, unsigned min_group_size)
{
  bool link_found = false;

  //for all curvelets of eA that are larger than the group size threshold 
  sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(eA->id).begin();
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
bool sdet_sel_base::link_is_reciprocal(sdet_edgel* eA, sdet_edgel* eB, sdet_edgel* ref_e, unsigned min_group_size)
{
  sdet_edgel *eC, *eN; //current edgel and neighboring edgel

  if (ref_e == eA) { eC = eA; eN = eB; }
  else             { eC = eB; eN = eA; }

  //we know eC-->eN, now look for eN-->eC link

  //for all curvelets that are larger than the group size threshold 
  sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(eN->id).begin();
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
bool sdet_sel_base::link_is_supported(sdet_edgel* eA, sdet_edgel* eB, unsigned min_group_size)
{
  bool link_found = false;

  //go over all curvelets of eA and find the ones that contain eB
  sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(eA->id).begin();
  for ( ; cv_it!=curvelet_map_.curvelets(eA->id).end() && !link_found; cv_it++)
  {
    sdet_curvelet* cvlet = (*cv_it);

    if (cvlet->order()<min_group_size)
      continue;

    for (unsigned k=0; k<cvlet->edgel_chain.size(); k++){
      sdet_edgel *eX=0, *eY=0;
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
bool sdet_sel_base::link_is_supported(sdet_edgel* eX, sdet_edgel* eA, sdet_edgel* eB, sdet_edgel* eY, 
                                       sdet_edgel* ref_e, unsigned min_group_size)
{
  sdet_edgel *eC, *eN; //current edgel and neighboring edgel

  if (ref_e == eA) { eC = eA; eN = eB; }
  else             { eC = eB; eN = eA; }

  // we know eC-->eN, now we need to verify that for those curvelets which have the eN-->eC link,
  // they are supported by the same edgels (i.e., that eX-eC->eN-eY and eX-eC<-eN-eY both exist)

  //for all curvelets of eN that are larger than the group size threshold 
  sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(eN->id).begin();
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
bool sdet_sel_base::triplet_is_supported(sdet_edgel* eX, sdet_edgel* eA, sdet_edgel* eY, 
                                          unsigned min_group_size)
{
  //we know eX-->eA-->eY exists, we need to verify that -eX-eA-eY exists for eX and eY. 
  sdet_curvelet* cvlet1 = 0;
  sdet_curvelet* cvlet2 = 0;

  bool cvlet_found = false;

  //for all curvelets of eX that are larger than the group size threshold 
  sdet_curvelet_list_iter cv_it = curvelet_map_.curvelets(eX->id).begin();
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

void sdet_sel_base::prune_the_link_graph()
{
  //prune the link graph of spurious links (ie links that cannot be extended)
  vcl_vector<sdet_link*> links_to_del;
  for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++)
  {
    sdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
    for (;l_it!=edge_link_graph_.cLinks[i].end(); l_it++)
    {
      if (!link_valid(*l_it)) //invalid link
        links_to_del.push_back(*l_it);
    }
  }

  //now delete all these single links
  for (unsigned i=0; i<links_to_del.size(); i++)
    edge_link_graph_.remove_link(links_to_del[i]);

  links_to_del.clear();
}

//: make the link graph bidirectionally consistent
void sdet_sel_base::make_link_graph_consistent()
{
  //this ought to be an iterative algorithm
  bool LG_consistent = false;
  while (!LG_consistent)
  {  
    vcl_vector<sdet_link*> links_to_del;

    // go over all the links of the link graph and determine if there is a counterpart to each of the links
    for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++)
    {
      sdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
      for (;l_it!=edge_link_graph_.cLinks[i].end(); l_it++)
      {
        if (!link_bidirectional(*l_it)) //invalid link
          links_to_del.push_back(*l_it);
      }
    }

    LG_consistent = (links_to_del.size()==0);

    //delete the inconsistent links along with the curvelets causing them
    for (unsigned j=0; j<links_to_del.size(); j++)
    {
      //Note: comment this part to visualize the links that are removed by this process

      //first remove the cvlets
      sdet_curvelet_list_iter cv_it = links_to_del[j]->curvelets.begin();
      for (; cv_it != links_to_del[j]->curvelets.end(); cv_it++)
        curvelet_map_.remove_curvelet(*cv_it);
    }

    //reconstruct the link graph from the remainng curvelets
    construct_the_link_graph(0, 0); //minsize, method
  }
}

//: clear all contours
void sdet_sel_base::clear_all_contours()
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
void sdet_sel_base::extract_image_contours_from_the_link_graph(unsigned num_link_iters)
{
  vcl_cout << "Extracting regular contours from the Link Graph..." ;

  //first remove any existing contours
  clear_all_contours();

  //find the max deg of overlap for all the links
  unsigned max_overlap = 0;
  for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++){
    sdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
    for (;l_it!=edge_link_graph_.cLinks[i].end(); l_it++){
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
      sdet_link_list_iter l_it = edge_link_graph_.cLinks[j].begin();
      for (;l_it!=edge_link_graph_.cLinks[j].end(); l_it++){
        if ((*l_it)->deg_overlap >= (int)min_deg_to_link_)
          link_graph_temp.cLinks[j].push_back(*l_it);
      }
    }

    for (unsigned j=0; j<edge_link_graph_.pLinks.size(); j++){
      sdet_link_list_iter l_it = edge_link_graph_.pLinks[j].begin();
      for (;l_it!=edge_link_graph_.pLinks[j].end(); l_it++){
        if ((*l_it)->deg_overlap >= (int)min_deg_to_link_)
          link_graph_temp.pLinks[j].push_back(*l_it);
      }
    }

    extract_one_chains_from_the_link_graph(link_graph_temp);

    //clear the temp links so that they are not deleted (hack)
    link_graph_temp.cLinks.clear();
    link_graph_temp.pLinks.clear();

    vcl_cout << i << "...";
  }

  //Now sew together any distinct but connected edgel chains
  //TODO: post_process_to_link_fragments();

  //remove any short segments
  //TODO: prune_contours(0.0, 4.0);

  vcl_cout << "done!" << vcl_endl;
}


//: extract the regular contours from the link graph
void sdet_sel_base::extract_regular_contours_from_the_link_graph()
{
  //first remove any existing contours
  clear_all_contours();

  //clear the linked flags
  edge_link_graph_.clear_linked_flag();

  //exract one chains from the main link graph
  extract_one_chains_from_the_link_graph(edge_link_graph_);
}

//: extract the one chains from a given link graph (from the primary link grpah of ELG)
void sdet_sel_base::extract_one_chains_from_the_link_graph(sdet_edgel_link_graph& ELG)
{
  //initialize the curve fragment map 
  curve_frag_graph_.resize(edgemap_->edgels.size());
  double thres=gap_;
  //now look for edgel chains
  //Rules: 
  //    (a) start from an edgel that is locally legal
  //    (b) trace in both directions until an illegal edgel is reached
  //    (c) prune out short chains

  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* first_edgel = edgemap_->edgels[i];

    //if it's already linked, ignore
    if (ELG.linked[first_edgel->id])
      continue;

    // Check edgel to see if it is legal to start a chain here
    if (ELG.edgel_is_legal_first_edgel(first_edgel)) 
    {
      //start a chain from this edgel
      sdet_edgel_chain* chain = new sdet_edgel_chain();

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
bool sdet_sel_base::link_valid(sdet_link* link)
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
    sdet_curvelet_list_iter cv_it = link->curvelets.begin();
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
bool sdet_sel_base::link_bidirectional(sdet_link* link)
{
  unsigned src_id = link->pe->id;
  unsigned tar_id = link->ce->id;

  //is there a link from the target to the source?
  sdet_link_list_iter l_it = edge_link_graph_.cLinks[tar_id].begin();
  for (;l_it!=edge_link_graph_.cLinks[tar_id].end(); l_it++)
  {
    if ((*l_it)->ce->id == (int)src_id)
      return true;
  }

  return false;
}

//: connect pieces of contours that are connected but separated
void sdet_sel_base::post_process_to_link_contour_fragments()
{

}


//***********************************************************************//
// Hybrid methods
//***********************************************************************//

//: construct a mapping between edgel ids and curve ids
void sdet_sel_base::compile_edge_to_contour_mapping()
{
  //clear the mapping
  cId_.clear();
  cId_.resize(edgemap_->num_edgels());

  //now go over the CFG and create the mapping
  int cnt=1;

  sdet_edgel_chain_list_iter f_it = curve_frag_graph_.frags.begin();
  for (; f_it != curve_frag_graph_.frags.end(); f_it++)
  {
    sdet_edgel_chain* chain = (*f_it);

    //go over the edgel chain and insert them into the mapping
    for (unsigned i=0; i<chain->edgels.size(); i++)
    {
      sdet_edgel* e = chain->edgels[i];
      cId_[e->id] = cnt;
    }
    
    //update contour count
    cnt++;
  }

}

//: attempt to form curvelets from the traced contour fragments
void sdet_sel_base::form_curvelets_from_contours(bool clear_existing)
{
  if (clear_existing)
  {
    curvelet_map_.clear();

    //form a new curvelet map
    curvelet_map_.resize(edgemap_->num_edgels());
  }

  sdet_edgel_chain_list_iter f_it = curve_frag_graph_.frags.begin();
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
        vcl_deque<sdet_edgel*> cvlet_chain;

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
          vcl_reverse(cvlet_chain.begin(), cvlet_chain.end());

        //now form a curvelet from this subchain
        sdet_curvelet* cvlet = 0;
        if (cvlet_chain.size()>3)
          cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, true, centered_, false);

        if (cvlet){
          curvelet_map_.add_curvelet(cvlet);
        }
      }

      if (!centered_ && !bidir_) //ENO style curvelets 
      {
        //2 curvelets around the ref edgel
        vcl_deque<sdet_edgel*> cvlet_chainb, cvlet_chaina;

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
          vcl_reverse(cvlet_chainb.begin(), cvlet_chainb.end());

        //now form a curvelet from this subchain
        sdet_curvelet* cvletb = 0;
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
          vcl_reverse(cvlet_chaina.begin(), cvlet_chaina.end());

        //now form a curvelet from this subchain
        sdet_curvelet* cvleta = 0;
        if (cvlet_chainb.size()>3)
          cvleta = form_an_edgel_grouping(ref_e, cvlet_chaina);

        if (cvleta) curvelet_map_.add_curvelet(cvleta);
        
      }
    }
  }

}

//: attempt to form curvelets from the traced contour fragments
void sdet_sel_base::form_curvelets_from_contours(unsigned max_size_to_group)
{
  //form a new curvelet map
  curvelet_map_.resize(edgemap_->num_edgels());

  unsigned half_size = (unsigned) vcl_floor((max_size_to_group-1)/2.0);

  sdet_edgel_chain_list_iter f_it = curve_frag_graph_.frags.begin();
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
        vcl_deque<sdet_edgel*> cvlet_chain;
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
        vcl_deque<sdet_edgel*> cvlet_chain;
        for (unsigned j=0; j<i+half_size+1; j++){
          if (j<chain->edgels.size())
            cvlet_chain.push_back(chain->edgels[j]);
        }
        sdet_curvelet* cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, rel_dir, centered_, true);
        if (cvlet) 
          curvelet_map_.add_curvelet(cvlet);
        
        if (bidir_)//other half in the other direction
        { 
          vcl_deque<sdet_edgel*> cvlet_chain;
          for (int j=i-half_size; j<1; j++){
            if (j>=0 && j<(int)chain->edgels.size())
              cvlet_chain.push_back(chain->edgels[j]);
          }
          sdet_curvelet* cvlet = form_an_edgel_grouping(ref_e, cvlet_chain, !rel_dir, centered_, true);
          if (cvlet) 
            curvelet_map_.add_curvelet(cvlet);
        }
        else { //other half in the same direction (ENO style)
          vcl_deque<sdet_edgel*> cvlet_chain;
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
void sdet_sel_base::post_process_to_break_contours()
{
  //Assume that curvelets have been computed already and the link graph reflects all the links from the curvelets

  vcl_cout << "Breaking contours....";

  //container for all fragments to be deleted
  vcl_vector<sdet_edgel_chain*> frags_to_del;

  //container for all the new sub fragments
  vcl_vector<sdet_edgel_chain*> new_frags;

  //size of segment to ignore at the beginning and end of a contour
  unsigned max_size_to_group=7; //temp (should be passed in to this function)
  unsigned half_size = (unsigned) vcl_floor((max_size_to_group-1)/2.0);

  sdet_edgel_chain_list_iter f_it = curve_frag_graph_.frags.begin();
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
    sdet_edgel_chain* sub_chain = 0;
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
      forming_sub_chain = false;
    }

    //delete the curve fragments that were broken
    if (needs_to_be_deleted)
      frags_to_del.push_back(chain); //mark for deletion

  }

  //First, delete all the fragments marked for deletion
  for (unsigned j=0; j<frags_to_del.size(); j++)
    curve_frag_graph_.remove_fragment(frags_to_del[j]); 

  //Finally, add all the new fragments to curve fragment graph
  for (unsigned j=0; j<new_frags.size(); j++)
  {
    if (new_frags[j]->edgels.size()>2)
      curve_frag_graph_.insert_fragment(new_frags[j]); 
  }

  vcl_cout << "done." << vcl_endl;
}

//: evauate the qualities of curvelets using various functions
void sdet_sel_base::evaluate_curvelet_quality(int method)
{
  //for each edgel, for each curvelet, compute quality using the specified method
  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    sdet_curvelet_list_iter it = curvelet_map_.curvelets(eA->id).begin();
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
void sdet_sel_base::determine_accuracy_of_measurements()
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
    sdet_curvelet_list_iter it = curvelet_map_.curvelets(i).begin();
    for (; it != curvelet_map_.curvelets(i).end(); it++){
      sdet_curvelet* cvlet = (*it);

      //get the measurements from the curvelets
      cvlet->curve_model->report_accuracy(estimates, min_estimates, max_estimates);

      theta_est(cvlet_cnt) = estimates[0];
      k_est(cvlet_cnt)     = estimates[1];
      gamma_est(cvlet_cnt) = estimates[2];

      theta_error(cvlet_cnt) = vcl_min(vcl_abs(estimates[0]-min_estimates[0]), vcl_abs(estimates[0]-max_estimates[0]));
      k_error(cvlet_cnt)     = vcl_min(vcl_abs(estimates[1]-min_estimates[1]), vcl_abs(estimates[1]-max_estimates[1]));
      gamma_error(cvlet_cnt) = vcl_min(vcl_abs(estimates[2]-min_estimates[2]), vcl_abs(estimates[2]-max_estimates[2]));

      cvlet_cnt++;
    }
  }

  double theta_mean, theta_std, theta_error_mean, theta_error_std;
  double k_mean, k_std, k_error_mean, k_error_std;
  double gamma_mean, gamma_std, gamma_error_mean, gamma_error_std;

  //calculate the meaan and variance of the measurements
  pdf1d_calc_mean_var(theta_mean, theta_std, theta_est);
  pdf1d_calc_mean_var(k_mean, k_std, k_est);
  pdf1d_calc_mean_var(gamma_mean, gamma_std, gamma_est);

  //calculate mean and variance of the errors
  pdf1d_calc_mean_var(theta_error_mean, theta_error_std, theta_error);
  pdf1d_calc_mean_var(k_error_mean, k_error_std, k_error);
  pdf1d_calc_mean_var(gamma_error_mean, gamma_error_std, gamma_error);

  vcl_cout << "======================================" << vcl_endl;
  vcl_cout << "Derivative estimate accuracy Report"    << vcl_endl;
  vcl_cout << "======================================" << vcl_endl;
  vcl_cout << "theta estimate (mean, std): (" << theta_mean << ", " << vcl_sqrt(theta_std) << ")" << vcl_endl;
  vcl_cout << "  k   estimate (mean, std): (" << k_mean << ", " << vcl_sqrt(k_std) << ")" << vcl_endl;
  vcl_cout << "gamma estimate (mean, std): (" << gamma_mean << ", " << vcl_sqrt(gamma_std) << ")" << vcl_endl;

  vcl_cout << "spread of theta estimate (mean, std): (" << theta_error_mean << ", " << vcl_sqrt(theta_error_std) << ")" << vcl_endl;
  vcl_cout << "spread of  k   estimate (mean, std): (" << k_error_mean << ", " << vcl_sqrt(k_error_std) << ")" << vcl_endl;
  vcl_cout << "spread of gamma estimate (mean, std): (" << gamma_error_mean << ", " << vcl_sqrt(gamma_error_std) << ")" << vcl_endl;

}


void sdet_sel_base::report_stats()
{
  vcl_cout << "======================================" << vcl_endl;
  vcl_cout << "Edge Linking Summary\n";
  vcl_cout << "======================================" << vcl_endl;
  vcl_cout << "# of edgels:   " << edgemap_->edgels.size() << vcl_endl;
  vcl_cout << "Parameters: ";
  vcl_cout << "dx = " << dpos_ << ", dt = " << dtheta_ << vcl_endl;
  vcl_cout << "neighborhood radius = " << rad_ << ", maxN = " << maxN_ << vcl_endl;

  unsigned max_size = 0;
  //find maximum size of curvelet
  for (unsigned i=0; i<edgemap_->edgels.size(); i++){
    sdet_curvelet_list_iter it = curvelet_map_.curvelets(i).begin();
    for (; it != curvelet_map_.curvelets(i).end(); it++){
      sdet_curvelet* cvlet = (*it);

      if (cvlet->order()>max_size)
        max_size = cvlet->order();
    }
  }

  //count the raw number of edgel groupings by size
  vcl_vector<int> cvlet_cnt(max_size+1, 0); 

  //count the # of edgels that have a particular sized grouping
  vcl_vector<int> cvlet_edgel_cnt(max_size+1, 0); 

  //count the # of edgels that have a curvelets of a certain size or lower
  vcl_vector<int> min_cvlet_edgel_cnt(max_size+1, 0);

  //count the # of curvelets that are consistent with their neighbors (criteria 1)
  vcl_vector<int> consistent_cvlet_cnt(max_size+1, 0);

  //count the # of edgels that have consistent curvelets of a particular size
  vcl_vector<int> consistent_cvlet_edgel_cnt(max_size+1, 0);

  //count the # of edgels that have consistent curvelets of a particular size or lower
  vcl_vector<int> min_consistent_cvlet_edgel_cnt(max_size+1, 0);

  for (unsigned i=0; i<edgemap_->edgels.size(); i++)
  {
    sdet_edgel* eA = edgemap_->edgels[i];

    //keep track of the various sized curvelets formed by this edgel
    vcl_vector<bool> size_exists(max_size+1, false); 
    vcl_vector<bool> consistent_size_exists(max_size+1, false);

    //for each edgel go over all the curvelets it forms
    sdet_curvelet_list_iter it = curvelet_map_.curvelets(eA->id).begin();
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
  vcl_cout << "==========================================================================================================" << vcl_endl;
  vcl_cout << "| cvlet size | raw # cvlet | % of edgels | % of edgels with no other larger curvelets | # of consistent cvlets | % of edgels with consistent curvelets |" << vcl_endl;
  for (unsigned i=3; i<=max_size; i++){
    vcl_cout << i << " " << cvlet_cnt[i] << " " << cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << " " << min_cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << " " ;
    vcl_cout <<  consistent_cvlet_cnt[i] << " " << consistent_cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << " " << min_consistent_cvlet_edgel_cnt[i]*100.0/edgemap_->edgels.size() << vcl_endl;
  }

  vcl_cout << vcl_endl;
  vcl_cout << "# of image curves: " << curve_frag_graph_.frags.size() << vcl_endl;
  vcl_cout << "======================================" << vcl_endl;
}

