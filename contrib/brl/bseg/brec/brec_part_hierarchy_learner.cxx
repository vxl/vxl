#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <sstream>
#include "brec_part_hierarchy_learner.h"
//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Jan 19, 2009

#include "brec_part_hierarchy_learner_sptr.h"

#include <bsta/bsta_histogram.h>
#include <brec/brec_part_gaussian.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_hierarchy_detector.h>

#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: check the "true" part of the mask
bool check_equal(vbl_array_2d<bool>& left_array, vbl_array_2d<bool>& right_array)
{
  vbl_array_2d<bool>::size_type i1, j1;

  // first check if they have the same number of true pixels
  int cnt_left = 0;
  for (i1 = 0; i1 < left_array.rows(); i1++) {
    for (j1 = 0; j1 < left_array.cols(); j1++) {
      if (left_array.get_rows()[i1][j1])
        cnt_left++;
    }
  }
  int cnt_right = 0;
  for (i1 = 0; i1 < right_array.rows(); i1++) {
    for (j1 = 0; j1 < right_array.cols(); j1++) {
      if (right_array.get_rows()[i1][j1])
        cnt_right++;
    }
  }
  if (cnt_left != cnt_right)
    return false;

  // find the first "true" pixel on both masks and use that pixel to centralize the second part on top of the other one
  bool found_it = false;
  for (i1 = 0; i1 < left_array.rows(); i1++) {
    for (j1 = 0; j1 < left_array.cols(); j1++) {
      if (left_array.get_rows()[i1][j1]) {
        found_it = true;
        break;
      }
    }
    if (found_it)
      break;
  }
  if (!found_it)
    return false;

  vbl_array_2d<bool>::size_type i2, j2;
  found_it = false;
  for (i2 = 0; i2 < right_array.rows(); i2++) {
    for (j2 = 0; j2 < right_array.cols(); j2++) {
      if (right_array.get_rows()[i2][j2]) {
        found_it = true;
        break;
      }
    }
    if (found_it)
      break;
  }
  if (!found_it)
    return false;

  // now we think i1,j1 and i2,j2 correspond
  vbl_array_2d<bool>::size_type i11, j11;
  for (i11 = 0; i11 < left_array.rows(); i11++) {
    for (j11 = 0; j11 < left_array.cols(); j11++) {
      if (left_array.get_rows()[i11][j11]) {
        // we expect right_array to have a true value at the corresponding spot when centered on i1, j2, otherwise they're not equal
        int i22, j22;
        i22 = i11-i1+i2;
        j22 = j11-j1+j2;
        if (i22 < 0 || i22 >= (int)right_array.rows() || j22 < 0 || j22 >= (int)right_array.cols() || !right_array.get_rows()[i22][j22])
          return false;
      }
    }
  }
  return true;
}

void brec_part_hierarchy_learner::initialize_layer0_as_gaussians(int ndirs, float lambda_range, float lambda_inc, int n)
{
  //n_ = (int)(lambda_range/lambda_inc);
  n_ = n; // visualization parameter to plot histograms into m files
  // stats for bright operators
  float theta = 0.0f;
  float theta_inc = 180.0f/(float)ndirs;
  unsigned type_cnt = 0;

  std::vector<vbl_array_2d<bool> > masks;

  for (float lambda0 = lambda_inc; lambda0 <= lambda_range; lambda0 += lambda_inc)
  {
    for (float lambda1 = lambda_inc; lambda1 <= lambda_range; lambda1 += lambda_inc)
    {
      theta = 0.0f;
      for (int i = 0; i < ndirs; i++) {
        // initialize bright operator
        float adjusted_theta = theta > 90 ? -(180-theta) : theta;  // operators work in [-pi, pi] range
        brec_part_gaussian_sptr p = new brec_part_gaussian(0.0f, 0.0f, 0.0f, lambda0, lambda1, adjusted_theta, true, type_cnt);
        // do not add it if it has exactly the same one as a previous operator
        bool already_exists = false;
        for (auto & mask : masks) {
          if (check_equal(p->mask_, mask)) {
            already_exists = true;
            break;
          }
        }

        if (already_exists)
          continue;

        masks.push_back(p->mask_);

        // create histogram for foreground stats
        auto* h = new bsta_histogram<float>(0.0f, 2.0f, 100); // was (-7.0f, 1.0f, 32)
        std::pair<brec_part_instance_sptr, bsta_histogram<float>* > pa(p->cast_to_instance(), h);
        stats_layer0_.push_back(pa);
        type_cnt++;

        // initialize the dark operator as well
        brec_part_gaussian_sptr pd = new brec_part_gaussian(0.0f, 0.0f, 0.0f, lambda0, lambda1, adjusted_theta, false, type_cnt);
        auto* hd = new bsta_histogram<float>(0.0f, 2.0f, 100);
        std::pair<brec_part_instance_sptr, bsta_histogram<float>* > pad(pd->cast_to_instance(), hd);
        stats_layer0_.push_back(pad);
        type_cnt++;

        theta += theta_inc;
      }
    }
  }
  std::cout << "initialized: " << type_cnt << " (array size: " << stats_layer0_.size() << ") operators\n";
#if 1
  std::cout << "initialized as follows:\n";
  for (auto & i : stats_layer0_) {
    brec_part_gaussian_sptr p = i.first->cast_to_gaussian();
    std::cout << "l0: " << p->lambda0_ << " l1: " << p->lambda1_ << " t: " << p->theta_;
    if (p->bright_)
      std::cout << " bright\n";
    else
      std::cout << " dark\n";
  }
  std::cout << "--------------------------\n"
           << " masks size: " << masks.size() << std::endl;
  for (const auto & mask : masks) {
    std::cout << mask << std::endl
             << "--------------------------\n";
  }
#endif
}

// assumes float img with values in [0,1] range
void brec_part_hierarchy_learner::layer0_collect_stats(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img, vil_image_view<bool>& mask)
{
  for (auto & i : stats_layer0_) {
    brec_part_instance_sptr p = i.first;
    bsta_histogram<float> *h = i.second;
    p->update_response_hist(inp, fg_prob_img, mask, *h);
  }
}

void brec_part_hierarchy_learner::layer0_collect_stats(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img)
{
  vil_image_view<bool> mask(inp.ni(), inp.nj());
  mask.fill(true);
  layer0_collect_stats(inp, fg_prob_img, mask);
}

void brec_part_hierarchy_learner::layer0_fit_parametric_dist()
{
  for (auto & i : stats_layer0_) {
    brec_part_instance_sptr p = i.first;
    bsta_histogram<float> *h = i.second;
    p->fit_distribution_to_response_hist(*h);  // the computed params are saved at the instance
  }
}

void brec_part_hierarchy_learner::layer0_collect_posterior_stats(vil_image_view<float>& inp,
                                                                 vil_image_view<float>& fg_prob_img,
                                                                 vil_image_view<bool>& mask,
                                                                 vil_image_view<float>& mean_img,
                                                                 vil_image_view<float>& std_dev_img)
{
  for (auto & i : stats_layer0_) {
    brec_part_instance_sptr p = i.first;
    p->update_foreground_posterior(inp, fg_prob_img, mask, mean_img, std_dev_img);  // the computed params are saved at the instance
  }
}

void brec_part_hierarchy_learner::layer0_collect_posterior_stats(vil_image_view<float>& inp,
                                                                 vil_image_view<float>& fg_prob_img,
                                                                 vil_image_view<float>& mean_img,
                                                                 vil_image_view<float>& std_dev_img)
{
  vil_image_view<bool> mask(inp.ni(), inp.nj());
  mask.fill(true);
  layer0_collect_posterior_stats(inp, fg_prob_img, mask, mean_img, std_dev_img);
}

bool rho_more(const std::pair<brec_part_instance_sptr, bsta_histogram<float>*>& p1,
              const std::pair<brec_part_instance_sptr, bsta_histogram<float>*>& p2)
{
  return p1.first->rho_c_f_ > p2.first->rho_c_f_;
}

//: create a part hierarchy of primitive parts which are added with respect to their average rho_ (posterior ratios)
//  This will be used to construct layers 1 and above
brec_part_hierarchy_sptr brec_part_hierarchy_learner::layer0_rank_and_create_hierarchy(int N)
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  std::sort(stats_layer0_.begin(), stats_layer0_.end(), rho_more);

  int cnt = (N < (int)stats_layer0_.size() ? N : (int)stats_layer0_.size());
  for (int i = 0; i < cnt; i++) {
    brec_part_instance_sptr p = stats_layer0_[i].first;
    std::cout << "adding layer0 i: " << i << " type: " << p->type_ << " rho_: " << p->rho_c_f_ << std::endl;
    brec_part_base_sptr p_0 = new brec_part_base(0, p->type_);
    h->add_vertex(p_0);
    h->add_dummy_primitive_instance(p);
  }

  for (auto & i : stats_layer0_) {
    delete i.second;
  }
  stats_layer0_.clear();

  return h;
}

//: initialize learner to construct layer_n as pairs of layer_n-1 of the given hierarchy.
//  radius is used to initialize the histograms
//  we use 8 bins for angle in [0, 2*pi] range and 8 bins for distance in [0,radius] range
bool brec_part_hierarchy_learner::initialize_layer_n_as_pairs(const brec_part_hierarchy_sptr& h, unsigned layer_id, unsigned nclasses, float radius)
{
  if (!layer_id)
    return false;

  h_ = h;
  radius_ = radius;

  type_cnt_ = 0;
  for (unsigned c = 0; c < nclasses; c++) {
    auto* map = new class_map();

    for (auto it = h->vertices_begin(); it != h->vertices_end(); it++) {
      if ((*it)->layer_ == layer_id-1) {
        brec_part_base_sptr p1 = (*it);

        for (auto it2 = h->vertices_begin(); it2 != h->vertices_end(); it2++) {
          if ((*it2)->layer_ == layer_id-1) {
            brec_part_base_sptr p2 = (*it2);

            brec_part_instance_sptr p_n = new brec_part_instance(layer_id, type_cnt_, brec_part_instance_kind::COMPOSED, 0.0f, 0.0f, 0.0f);
            type_cnt_++;
            brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(p_n->cast_to_base(), p1, true);
            p_n->add_outgoing_edge(e1);

            brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(p_n->cast_to_base(), p2, false);
            p_n->add_outgoing_edge(e2);

            auto d_hist = new hist(radius_, 16);
            auto a_hist = new hist(-vnl_math::pi, vnl_math::pi, 16);

            //sample_set_ptr d_mss = new sample_set(radius_/8.0f); //set mean shift bandwidth to the size of 2 bins
            //sample_set_ptr a_mss = new sample_set(float(2.0f*vnl_math::pi/8.0f)); //set mean shift bandwidth to the size of 2 bins
            auto mss = new sample_set();  // bandwidth is unimportant - because mean-shift will be applied on 1D marginalized version of this 2D data
            d_bandwidth_ = radius_/8.0f;  //set mean shift bandwidth to the size of 2 bins
            a_bandwidth_ = float(2.0f*vnl_math::pi/8.0f);  //set mean shift bandwidth to the size of 2 bins

            hist_ptr_pair ph(d_hist, a_hist);
            //sample_set_ptr_pair ps(d_mss, a_mss);

            //std::pair<hist_ptr_pair, sample_set_ptr_pair> hist_pair(ph, ps);
            std::pair<hist_ptr_pair, sample_set_ptr> hist_pair(ph, mss);

            std::pair<brec_part_instance_sptr, std::pair<hist_ptr_pair, sample_set_ptr> > pa(p_n, hist_pair);

            std::pair<unsigned, unsigned> pa_id(p1->type_, p2->type_);
            (*map)[pa_id] = pa;
          }
        }
      }
    }

    stats_layer_n_[c] = map;
  }

  std::cout << "initialized learner for: " << stats_layer_n_.size() << " classes\n";
#if 1
  unsigned cnt = 0;
  for (auto & it : stats_layer_n_) {
    class_map* map = it.second;
    std::cout << "\t class: " << cnt++ << ", initialized with " << map->size() << " pairs:\n";
    class_map::iterator m_it;
    for (m_it = map->begin(); m_it != map->end(); m_it++) {
      std::pair<unsigned, unsigned> id_p = (*m_it).first;
      std::pair<brec_part_instance_sptr, std::pair<hist_ptr_pair, sample_set_ptr> > pa = (*m_it).second;
      std::cout << '(' << id_p.first << ", " << id_p.second << ") ";
      for (auto eit = pa.first->out_edges_begin(); eit != pa.first->out_edges_end(); eit++) {
        std::cout << '(' << (*eit)->target()->layer_ << ", " << (*eit)->target()->type_ << ") ";
      }
      std::cout << std::endl;
    }
  }

#endif

  return true;
}

//: a helper function to find a bin in one of 8 bins in the range [0,radius] for a given angle value.
//  angle values are in the range [-pi, pi], so map this range to [0,radius]
float map_range_for_angle(float a, float radius)
{
  float val = float(vnl_math::pi)+a;  // mapped to [0,2*pi]
  val = val/(2.0f*float(vnl_math::pi));  // mapped to [0,1]
  val = val*radius;  // mapped to [0,radius]
  return val;
}

void map_to_cartesian(float angle, float radius, float max_radius, float& x, float& y)
{
  x = max_radius + (float)radius*std::cos(angle);
  y = max_radius + (float)radius*std::sin(angle);
}

//: collect joint stats to construct parts of layer with layer_id using detected parts of layer_id-1
//  Collect stats for a pair if they exist within radius pixels of each other
bool brec_part_hierarchy_learner::layer_n_collect_stats(const brec_part_hierarchy_detector_sptr& hd, unsigned layer_id, unsigned class_id)
{
  if (!layer_id) {
    std::cout << "In brec_part_hierarchy_learner::layer_n_collect_stats() -- layer_id is zero!!\n";
    return false;
  }

  brec_part_hierarchy_sptr h = hd->get_hierarchy();
  std::vector<brec_part_instance_sptr> parts = hd->get_parts(layer_id-1);
  Rtree_type* rtree = hd->get_tree(layer_id-1);

  class_map* map;
  layer_n_map::iterator it;
  it = stats_layer_n_.find(class_id);
  if (it != stats_layer_n_.end())
    map = (*it).second;
  else {
    std::cout << "WARNING: This class: " << class_id << " was not initialized!\n";
    return false;
  }

  std::cout << "there are " << parts.size() << " parts of layer: " << layer_id-1 << " in the detector!\n";
  class_map::iterator qit;

  vnl_random rng;

  // go through each detected instance as central part
  for (auto & part : parts) {
    if (part->layer_ == layer_id-1)
    {
      vgl_box_2d<float> probe = part->get_probe_box(radius_);
      std::vector<brec_part_instance_sptr> found;
      rtree->get(probe, found);

      bsta_gaussian_sphere<float, 2> jj_dist;
      jj_dist.set_mean(part->location()); jj_dist.set_var(1.0f);

      for (auto & kk : found) {
        if (kk == part)
          continue;
        if (kk->layer_ == layer_id-1) {
          // update stats for this pair
          vnl_vector_fixed<float,2> cent_dif = kk->location() - part->location();
          // calculate angle and dists
          float a, d;
          brec_hierarchy_edge::calculate_dist_angle(part->cast_to_instance(), cent_dif, d, a);

#if 0  // this was 1 for the digit application
          // make sure the samples are well-separated
          if (d < radius_/2)
            continue;  // discard this pair
#endif
          // create a bunch of samples by assuming 1 pixel variance in the pixel locations
          bsta_gaussian_sphere<float, 2> kk_dist;
          kk_dist.set_mean(kk->location()); kk_dist.set_var(1.0f);

          std::vector<float> as, ds;
          as.push_back(a); ds.push_back(d);
          for (unsigned mmm = 0; mmm < 10; mmm++) {
            cent_dif = kk_dist.sample(rng) - jj_dist.sample(rng);
            brec_hierarchy_edge::calculate_dist_angle(part->cast_to_instance(), cent_dif, d, a);
            as.push_back(a); ds.push_back(d);
          }

          std::pair<unsigned, unsigned> qid(part->type_, kk->type_);
          qit = map->find(qid);
          if (qit != map->end()) {  // found the histogram
            hist_ptr d_hist = (qit->second).second.first.first;
            hist_ptr a_hist = (qit->second).second.first.second;

            sample_set_ptr set = (qit->second).second.second;

            //brec_part_instance_sptr layer_n_part = (qit->second).first;

            auto w1 = (float)part->cast_to_instance()->rho_c_f_;
            auto w2 = (float)kk->cast_to_instance()->rho_c_f_;

            for (unsigned mmm = 0; mmm < as.size(); mmm++) {
              d_hist->upcount(ds[mmm], w1*w2);
              a_hist->upcount(as[mmm], w1*w2);
              vnl_vector_fixed<double, 2> sample(ds[mmm], as[mmm]);
              set->insert_sample(sample, w1*w2);
            }
          }
        }
      }
    }
  }
  return true;
}

//: uses the joint histograms to fit Gaussian distributions to distance for 8 orientations.
//  Replaces the histograms with the fitted distributions' histograms
//  Populate layer_n of current hierarchy of the class with parts which have models that have highest data log-likelihood
bool brec_part_hierarchy_learner::layer_n_fit_distributions(unsigned class_id, unsigned layer_id, unsigned M)
{
  class_map* map;
  layer_n_map::iterator it;
  it = stats_layer_n_.find(class_id);
  if (it != stats_layer_n_.end())
    map = (*it).second;
  else {
    std::cout << "WARNING: This class: " << class_id << " was not initialized!\n";
    return false;
  }

  // get the class hierarchy
  brec_part_hierarchy_sptr class_h;
  auto h_it = h_map_.find(class_id);
  if (h_it != h_map_.end()) {
    class_h = (*h_it).second;
  }
  else {  // this means we're building up layer 1 (i.e. n = 1)
    class_h = new brec_part_hierarchy();
    // add the primitives from h_
    for (unsigned i = 0; i < h_->get_dummy_primitive_instances().size(); i++)
      class_h->add_dummy_primitive_instance(h_->get_dummy_primitive_instances()[i]);
    // add layer 0 from h_
    for (auto v_it = h_->vertices_begin(); v_it != h_->vertices_end(); v_it++) {
      class_h->add_vertex(*v_it);
    }
    for (auto e_it = h_->edges_begin(); e_it != h_->edges_end(); e_it++) {
      class_h->add_edge_no_check(*e_it);
    }

    h_map_[class_id] = class_h;
  }

  //  first we need the total_weight of all the samples in all the pairs's data
  double total_weight = 0.0;
  class_map::iterator qit;
  for (qit = map->begin(); qit != map->end(); qit++) {
    sample_set_ptr set = (qit->second).second.second;
    total_weight += set->total_weight();
  }
  std::cout << "total weight of all the data from all the pairs for class: " << class_id << " is: " << total_weight << std::endl;

  for (qit = map->begin(); qit != map->end(); qit++) {
    hist_ptr d_hist = (qit->second).second.first.first;
    hist_ptr a_hist = (qit->second).second.first.second;

    if (qit->first.first == 23 && qit->first.second == 3)
      std::cout << "here!\n";

    //sample_set_ptr d_set = (qit->second).second.second.first;
    //sample_set_ptr a_set = (qit->second).second.second.second;
    sample_set_ptr set = (qit->second).second.second;

    brec_part_instance_sptr layer_n_part = (qit->second).first;

    unsigned d_nbins = d_hist->nbins();
    float d_delta = radius_/float(d_nbins);

    // create 1D marginalized distance sample set from nD set
    bsta_sample_set<double,1> d_set;
    if (!bsta_sample_set_marginalize(*set, 0, d_set)) {
      std::cout << "ERROR: cannot create 1D distance set from set!\n";
      return false;
    }
    d_set.set_bandwidth(d_bandwidth_);

    bsta_sample_set<double,1> a_set;
    if (!bsta_sample_set_marginalize(*set, 1, a_set)) {
      std::cout << "ERROR: cannot create 1D distance set from set!\n";
      return false;
    }
    a_set.set_bandwidth(a_bandwidth_);

    // run mean_shift on distance sample set
    bsta_mean_shift<double,1> d_ms;
    d_ms.find_modes(d_set, 0.01);
    d_ms.trim_modes(d_set, 2*d_delta);
    //std::cout << "In layer_n_fit_distributions(" << class_id << ") - type: " << layer_n_part->type_ << ", # of d modes: " << d_set.mode_cnt() << std::endl;
    d_ms.merge_modes(d_set, 3, 0.01);  // merge the modes with samples less then 3
    std::cout << "In layer_n_fit_distributions(" << class_id << ") - type: " << layer_n_part->type_ << ", # of d modes: " << d_set.mode_cnt() << std::endl;

    // get the fitted mixture using ss, ms set the assignments after trimming
    bool ok = true;
    bsta_mixture<bsta_num_obs<bsta_gaussian_sphere<double,1> > > d_out_dist;
    delete d_hist;
    d_hist = new bsta_histogram<double>(radius_, d_nbins);
    if (!bsta_sample_set_fit_distribution<double>(d_set, d_out_dist)) {
      std::cout << "Warning: Cannot fit a mixture to the distribution of the class: " << class_id << '\n';
      ok = false;
    }
    else {
      // now replace the histogram with the mean shift fitted one
      for (unsigned aa = 0; aa < d_nbins; aa++) {
        float pt = float(aa+1)*d_delta;
        double val = d_out_dist.prob_density(pt);
        d_hist->upcount(float(aa+1)*d_delta, val);
      }
    }

    (qit->second).second.first.first = d_hist;

    // run mean_shift on angle sample set
    unsigned a_nbins = a_hist->nbins();
    auto a_delta = float(vnl_math::twopi/a_nbins);

    bsta_mean_shift<double,1> a_ms;
    a_ms.find_modes(a_set, 0.01f);
    a_ms.trim_modes(a_set, 2*a_delta);
    //std::cout << "In layer_n_fit_distributions(" << class_id << ") - type: " << layer_n_part->type_ << ", # of a modes: " << a_set.mode_cnt() << std::endl;
    a_ms.merge_modes(a_set, 3, 0.01f);  // merge the modes with samples less then 3
    std::cout << "In layer_n_fit_distributions(" << class_id << ") - type: " << layer_n_part->type_ << ", # of a modes: " << a_set.mode_cnt() << std::endl;

    // get the fitted mixture using ss, ms set the assignments after trimming
    bsta_mixture<bsta_num_obs<bsta_gaussian_sphere<double,1> > > a_out_dist;
    delete a_hist;
    a_hist = new bsta_histogram<double>(-vnl_math::pi, vnl_math::pi, a_nbins);
    if (!bsta_sample_set_fit_distribution<double>(a_set, a_out_dist)) {
      std::cout << "Warning: Cannot fit a mixture to the distribution of the class: " << class_id << '\n';
      ok = false;
    }
    else {
      // now replace the histogram with the mean shift fitted one
      for (unsigned aa = 0; aa < a_nbins; aa++) {
        float pt = float(aa+1)*a_delta;
        double val = a_out_dist.prob_density(pt);
        a_hist->upcount(float(aa+1)*a_delta, val);
      }
    }

    (qit->second).second.first.second = a_hist;

    if (ok)
    {
      // find the likelihood for each of d_out_dist.num_components()*a_out_dist.num_components() possible models
      //  update the layer_n parts of current hierarchy for the class
      for (unsigned mi = 0; mi < d_out_dist.num_components(); mi++) {
        for (unsigned mj = 0; mj < a_out_dist.num_components(); mj++) {
          double w_sum = 0.0;
          // compute ll which is the data likelihood for this pair, and w_sum/total_weight is the prior probability
          double ll = bsta_sample_set_log_likelihood(*set, d_out_dist.distribution(mi), d_out_dist.weight(mi), a_out_dist.distribution(mj), a_out_dist.weight(mj), w_sum);

          if (ll < -1e10)
            continue;  // skip it if it overflowed

          std::cout << "\t ll: " << ll << ' ';

          double ratio = ll;

          if (stats_layer_n_.size() > 1) {
          // find the likelihood for each class's sample sets
          double best_class_ll = -std::numeric_limits<double>::infinity();
          for (auto class_it = stats_layer_n_.begin(); class_it != stats_layer_n_.end(); class_it++) {
            if (it == class_it)
              continue;

            std::pair<unsigned, unsigned> op_pair(qit->first.first, qit->first.second);
            auto class_pair_it = (*class_it).second->find(op_pair);
            if (class_pair_it == (*class_it).second->end()) {
              std::cout << "Error: One of the classes was not initialized for the pair: " << qit->first.first << ' ' << qit->first.second << '\n';
              return false;
            }
            sample_set_ptr class_set = (class_pair_it->second).second.second;
            double class_w_sum = 0.0;
            double class_ll = bsta_sample_set_log_likelihood(*class_set, d_out_dist.distribution(mi), d_out_dist.weight(mi), a_out_dist.distribution(mj), a_out_dist.weight(mj), class_w_sum);
            if (class_ll < -1e10) // skip it if overflowed
              continue;
            if (class_ll > best_class_ll)
              best_class_ll = class_ll;
          }
          if (best_class_ll < -1e10) // skip this distribution pair if overflowed for all the classes
            continue;

          ratio -= best_class_ll;
          std::cout << "\t best_class_ll: " << best_class_ll << " ratio: " << ratio << std::endl;
          }

          std::cout << "\t ratio: " << ratio << std::endl;

          // prepare the part
          brec_part_base_sptr p_n = new brec_part_base(layer_id, type_cnt_);
          type_cnt_++;

          unsigned cpl = layer_n_part->edge_to_central_part()->target()->layer_;
          unsigned cpt = layer_n_part->edge_to_central_part()->target()->type_;
          brec_part_base_sptr p_n_p1 = class_h->get_node(cpl, cpt);

          unsigned spl = layer_n_part->edge_to_second_part()->target()->layer_;
          unsigned spt = layer_n_part->edge_to_second_part()->target()->type_;
          brec_part_base_sptr p_n_p2 = class_h->get_node(spl, spt);

          brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(p_n, p_n_p1, true);
          p_n->add_outgoing_edge(e1);

          brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(p_n, p_n_p2, false);
          p_n->add_outgoing_edge(e2);

          //p_n->prior_prob_ = w_sum/total_weight;
          p_n->prior_prob_ = w_sum;
          p_n->log_likelihood_ = ratio;
          e2->set_model(d_out_dist.distribution(mi), a_out_dist.distribution(mj), d_out_dist.weight(mi)*a_out_dist.weight(mj));

          // if this is better than top M modes replace/place it in the hierarchy
          if (class_h->layer_cnt(layer_id) < M) {  // insert directly
            class_h->add_vertex(p_n);
          }
          else {  // replace one of the existing ones
            // traverse all layer_n nodes and replace the one with worst ll
            double min = 1e6;
            brec_part_hierarchy::vertex_iterator v_min_it;
            for (auto v_it = class_h->vertices_begin(); v_it != class_h->vertices_end(); v_it++) {
              if ((*v_it)->layer_ != layer_id)
                continue;

              if (min > (*v_it)->log_likelihood_) {
                v_min_it = v_it;
                min = (*v_it)->log_likelihood_;
              }
            }
            if (min < ratio) {
              if (!class_h->remove_vertex(*v_min_it)) {
                std::cout << "ERROR: brec_part_hierarchy_learner::layer_n_fit_distributions() -- cannot delete vertex from hierarchy!\n";
                return false;
              }
              class_h->add_vertex(p_n);
            }
          }
        }
      }
    }  // if ok (both histograms have been fitted)
  }

  // now fix the hierarchy with the added nodes
  for (auto v_it = class_h->vertices_begin(); v_it != class_h->vertices_end(); v_it++) {
    if ((*v_it)->layer_ != layer_id)
      continue;
    brec_part_base_sptr p = (*v_it);
    // add the edges of this part to the hierarchy
    for (auto e_it = p->out_edges_begin(); e_it != p->out_edges_end(); e_it++) {
      class_h->add_edge_no_check((*e_it));
      (*e_it)->target()->add_incoming_edge((*e_it));
    }
  }

  return true;
}

void brec_part_hierarchy_learner::print_layer0()
{
  for (auto & i : stats_layer0_) {
    brec_part_instance_sptr pi = i.first;
    bsta_histogram<float> *h = i.second;
    if (pi->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = pi->cast_to_gaussian();
      if (p->bright_)
        std::cout << "--- lambda0 " << p->lambda0_ << " --- lambda1 " << p->lambda1_ << " --- theta " << p->theta_ << " --- bright ---\n";
      else
        std::cout << "--- lambda0 " << p->lambda0_ << " --- lambda1 " << p->lambda1_ << " --- theta " << p->theta_ << " --- dark ---\n";
    }
    std::cout << "----- foreground hist ----------\n";
    h->print();
    std::cout << "-------------------------------------------\n";
  }
}

void brec_part_hierarchy_learner::print_to_m_file_layer0(const std::string& file_name)
{
  std::ofstream ofs(file_name.c_str());
  ofs << "% dump histograms\n";

  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    brec_part_instance_sptr pi = stats_layer0_[i].first;
    bsta_histogram<float> *h = stats_layer0_[i].second;

    if (i%(n_*n_) == 0)
      ofs << "figure;\n";

    ofs << "subplot(1," << n_*n_ << ", " << (i%(n_*n_))+1 << "), ";

    h->print_to_m(ofs);
    if (pi->kind_ == brec_part_instance_kind::GAUSSIAN) {
      ofs << "title('";
      brec_part_gaussian_sptr p = pi->cast_to_gaussian();

      if (p->bright_)
        ofs << "l0: " << p->lambda0_ << " l1: " << p->lambda1_ << " t: " << p->theta_ << " b foreg');\n";
      else
        ofs << "l0: " << p->lambda0_ << " l1: " << p->lambda1_ << " t: " << p->theta_ << " d foreg');\n";
    }
    //ofs << "axis([-7.0 1.0 0.0 1.0]);\n";
  }

  ofs.close();
}

void brec_part_hierarchy_learner::print_to_m_file_layer_n(const std::string& file_name, unsigned class_id, bool print_set)
{
  std::ofstream ofs(file_name.c_str());
  ofs << "% dump histograms\n";

  auto it = stats_layer_n_.find(class_id);
  if (it == stats_layer_n_.end()) {
    std::cout << "Error: Cannot find stats for class: " << class_id << '\n';
    ofs.close();
    return;
  }

  class_map* map = it->second;

  class_map::iterator m_it;
  //unsigned i = 0;
  for (m_it = map->begin(); m_it != map->end(); m_it++)
  {
    std::pair<unsigned, unsigned> id_p = (*m_it).first;

    hist_ptr d_hist = (*m_it).second.second.first.first;
    hist_ptr a_hist = (*m_it).second.second.first.second;

    //sample_set_ptr d_set = (*m_it).second.second.second.first;
    //sample_set_ptr a_set = (*m_it).second.second.second.second;
    sample_set_ptr set = (*m_it).second.second.second;

    brec_part_instance_sptr pa = (*m_it).second.first;

    brec_part_base_sptr cp = pa->edge_to_central_part()->target();
    brec_part_base_sptr sp = pa->edge_to_second_part()->target();
    std::string cp_sid = "";
    std::string sp_sid = "";
    brec_part_instance_sptr cpi = h_->get_node_instance(cp->layer_, cp->type_);
    brec_part_instance_sptr spi = h_->get_node_instance(sp->layer_, sp->type_);
    if (!cpi) {
      //std::cout << "instance could not be found in the hierarchy!\n";
      std::stringstream ss; ss << cp->layer_ << ' ' << cp->type_;
      cp_sid = ss.str();
    }
    else if (cpi->kind_ == brec_part_instance_kind::GAUSSIAN)
      cp_sid = cpi->cast_to_gaussian()->string_identifier();
    if (!spi) {
      //std::cout << "instance could not be found in the hierarchy!\n";
      std::stringstream ss; ss << sp->layer_ << ' ' << sp->type_;
      sp_sid = ss.str();
    }
    else if (spi->kind_ == brec_part_instance_kind::GAUSSIAN)
      sp_sid = spi->cast_to_gaussian()->string_identifier();

    ofs << "h = figure;\n";
    ofs << "set(h, 'Name', 'Class: " << class_id << " pair: (" << id_p.first << ": " << cp_sid << ", " << id_p.second << ": " << sp_sid << ") ');\n";

    //ofs << "subplot(1," << n_ << ", " << i%n_+1 << "), ";
    //i++;

    //if (print_set)
    //  ofs << "subplot(2,2,1), ";
    //else
    if (!print_set) {
      ofs << "subplot(1,2,1), ";
      d_hist->print_to_m(ofs);
      ofs << "xlabel('distance in range [0,"<< radius_ << "]');\n";
    }

    //ofs << "AXIS([0 " << d_hist->nbins()+1 << " 0.0 0.5]);\n";

    //if (print_set)
    //  ofs << "subplot(2,2,2), ";

    if (!print_set) {
      ofs << "subplot(1,2,2), ";
      a_hist->print_to_m(ofs);
      ofs << "xlabel('angle in range [0,2*pi]');\n";
    }
    //ofs << "AXIS([0 " << a_hist->nbins()+1 << " 0.0 0.5]);\n";

    if (print_set)
    {
      // create 1D marginalized distance sample set from nD set
      bsta_sample_set<double,1> d_set;
      if (!bsta_sample_set_marginalize(*set, 0, d_set)) {
        std::cout << "ERROR: cannot create 1D distance set from set!\n";
        ofs.close();
        return;
      }

      d_set.set_bandwidth(d_bandwidth_);
      // run mean_shift on distance sample set
      bsta_mean_shift<double,1> d_ms;
      d_ms.find_modes(d_set, 0.01);
      d_ms.trim_modes(d_set, 2*d_hist->delta());
      d_ms.merge_modes(d_set, 3, 0.01);  // merge the modes with samples less then 3

      bsta_sample_set<double,1> a_set;
      if (!bsta_sample_set_marginalize(*set, 1, a_set)) {
        std::cout << "ERROR: cannot create 1D distance set from set!\n";
        ofs.close();
        return;
      }

      a_set.set_bandwidth(a_bandwidth_);
      // run mean_shift on distance sample set
      bsta_mean_shift<double,1> a_ms;
      a_ms.find_modes(a_set, 0.01);
      a_ms.trim_modes(a_set, 2*a_hist->delta());
      a_ms.merge_modes(a_set, 3, 0.01);  // merge the modes with samples less then 3

      ofs << "subplot(1,2,1), ";
      //bsta_sample_set_print_to_m(*ss, ofs);
      bsta_sample_set_dist_print_to_m(d_set, ofs);
      ofs << "xlabel('distance in range [0,"<< radius_ << "]');\n";

      ofs << "subplot(1,2,2), ";
      //bsta_sample_set_print_to_m(*ss, ofs);
      bsta_sample_set_dist_print_to_m(a_set, ofs);
      ofs << "xlabel('angle in range [0,2*pi]');\n";
    }

    // ask for a character input after each figure
    ofs << "sscanf( input('','s'), '%c' );\n";
  }

  ofs.close();
}

void brec_part_hierarchy_learner::print_to_m_file_layer0_fitted_dists(const std::string& file_name)
{
  std::ofstream ofs(file_name.c_str());
  ofs << "% dump histograms of fitted distributions\n";

  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    brec_part_instance_sptr pi = stats_layer0_[i].first;
    bsta_histogram<float> *h = stats_layer0_[i].second;

    std::vector<float> x;
    for (float val = h->min(); val <= h->max(); val += h->delta()) {
      x.push_back(val);
    }

    if (i%(n_*n_) == 0)
      ofs << "figure;\n";

    ofs << "subplot(1," << n_*n_ << ", " << (i%(n_*n_))+1 << "), "
        << "x = [" << x[0];
    for (unsigned jj = 1; jj < x.size(); jj++)
      ofs << ", " << x[jj];
    ofs << "];\n";

    if (pi->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = pi->cast_to_gaussian();

      if (p->fitted_weibull_)
      {
        bsta_weibull<float> pdfg(p->lambda_, p->k_);

        ofs << "y = [" << pdfg.prob_density(x[0]);
        for (unsigned jj = 1; jj < x.size(); jj++)
          ofs << ", " << pdfg.prob_density(x[jj]);
        ofs << "];\n"
            << "bar(x,y,'r');\n"
            << "title('";
        if (p->bright_)
          ofs << "l0: " << p->lambda0_ << " l1: " << p->lambda1_ << " t: " << p->theta_ << " b weibull');\n";
        else
          ofs << "l0: " << p->lambda0_ << " l1: " << p->lambda1_ << " t: " << p->theta_ << " d weibull');\n";
      }
      else {
        std::cout << "WARNING: no fitted foreground response model for this operator! Cannot print to m file.\n";
      }
    }
    //ofs << "axis([-7.0 1.0 0.0 1.0]);\n";
  }

  ofs.close();
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, brec_part_hierarchy_learner const & /*ph*/)
{
  std::cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value\n";
  return;
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_read(vsl_b_istream & /*is*/, brec_part_hierarchy_learner & /*ph*/)
{
  std::cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, brec_part_hierarchy_learner* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new brec_part_hierarchy_learner();
    vsl_b_read(is, *ph);
  }
  else
    ph = nullptr;
}

void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy_learner* &ph)
{
  if (ph==nullptr)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}
