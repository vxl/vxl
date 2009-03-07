//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date Jan 19, 2009

#include "brec_part_hierarchy_learner.h"
#include "brec_part_hierarchy_learner_sptr.h"

#include <bsta/bsta_histogram.h>
#include <brec/brec_part_gaussian.h>
#include <brec/brec_part_hierarchy.h>
#include <brec/brec_part_hierarchy_detector.h>

#include <vil/vil_math.h>

void brec_part_hierarchy_learner::initialize_layer0_as_gaussians(int ndirs, float lambda_range, float lambda_inc)
{
  n_ = (int)(lambda_range/lambda_inc);
  // stats for bright operators
  float theta = -90.0f;
  float theta_inc = 180.0f/ndirs;
  bool bright = true;
  unsigned type_cnt = 0;
  for (int i = 0; i < ndirs; i++) {
    for (float lambda0 = 1.0f; lambda0 <= lambda_range; lambda0 += lambda_inc) {
      for (float lambda1 = 1.0f; lambda1 <= lambda_range; lambda1 += lambda_inc) {
        brec_part_gaussian_sptr p = new brec_part_gaussian(0.0f, 0.0f, 0.0f, lambda0, lambda1, theta, bright, type_cnt);
        type_cnt++;
        // create histogram for foreground stats
        bsta_histogram<float>* h = new bsta_histogram<float>(0.0f, 2.0f, 100); // was (-7.0f, 1.0f, 32);
        vcl_pair<brec_part_instance_sptr, bsta_histogram<float>* > pa(p->cast_to_instance(), h);
        stats_layer0_.push_back(pa);
      }
    }

    theta += theta_inc;
  }

  theta = -90.0f;
  bright = false;
  // stats for dark operators
  for (int i = 0; i < ndirs; i++) {
    for (float lambda0 = 1.0f; lambda0 <= lambda_range; lambda0 += lambda_inc) {
      for (float lambda1 = 1.0f; lambda1 <= lambda_range; lambda1 += lambda_inc) {
        brec_part_gaussian_sptr p = new brec_part_gaussian(0.0f, 0.0f, 0.0f, lambda0, lambda1, theta, bright, type_cnt);
        type_cnt++;
        // create histogram for foreground stats
        bsta_histogram<float>* h = new bsta_histogram<float>(0.0f, 2.0f, 100); // was (-7.0f, 1.0f, 32);
        vcl_pair<brec_part_instance_sptr, bsta_histogram<float>* > pa(p->cast_to_instance(), h);
        stats_layer0_.push_back(pa);
      }
    }

    theta += theta_inc;
  }
}

// assumes float img with values in [0,1] range
void brec_part_hierarchy_learner::layer0_collect_stats(vil_image_view<float>& inp, vil_image_view<float>& fg_prob_img, vil_image_view<bool>& mask)
{
  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    brec_part_instance_sptr p = stats_layer0_[i].first;
    bsta_histogram<float> *h = stats_layer0_[i].second;
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
  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    brec_part_instance_sptr p = stats_layer0_[i].first;
    bsta_histogram<float> *h = stats_layer0_[i].second;
    p->fit_distribution_to_response_hist(*h);  // the computed params are saved at the instance
  }
}

void brec_part_hierarchy_learner::layer0_collect_posterior_stats(vil_image_view<float>& inp,
                                                                 vil_image_view<float>& fg_prob_img,
                                                                 vil_image_view<bool>& mask,
                                                                 vil_image_view<float>& mean_img,
                                                                 vil_image_view<float>& std_dev_img)
{
  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    brec_part_instance_sptr p = stats_layer0_[i].first;
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

bool rho_more(const vcl_pair<brec_part_instance_sptr, bsta_histogram<float>*>& p1,
              const vcl_pair<brec_part_instance_sptr, bsta_histogram<float>*>& p2)
{
  return p1.first->rho_ > p2.first->rho_;
}

//: create a part hierarchy of primitive parts which are added with respect to their average rho_ (posterior ratios)
//  This will be used to construct layers 1 and above
brec_part_hierarchy_sptr brec_part_hierarchy_learner::layer0_rank_and_create_hierarchy(int N)
{
  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  vcl_sort(stats_layer0_.begin(), stats_layer0_.end(), rho_more);

  int cnt = (N < (int)stats_layer0_.size() ? N : (int)stats_layer0_.size());
  for (int i = 0; i < cnt; i++) {
    brec_part_instance_sptr p = stats_layer0_[i].first;
    vcl_cout << "adding layer0 i: " << i << " type: " << p->type_ << " rho_: " << p->rho_ << vcl_endl;
    brec_part_base_sptr p_0 = new brec_part_base(0, p->type_);
    h->add_vertex(p_0);
    h->add_dummy_primitive_instance(p);
  }

  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    delete stats_layer0_[i].second;
  }
  stats_layer0_.clear();

  return h;
}

//: collect joint stats to construct parts of layer with layer_id using detected parts of layer_id-1
bool brec_part_hierarchy_learner::layer_n_collect_stats(brec_part_hierarchy_detector_sptr hd, unsigned layer_id, unsigned class_id)
{
  brec_part_hierarchy_sptr h = hd->get_hierarchy();

  // if not already initialized, inialize the vector for this class
  vcl_map<unsigned, vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_joint_histogram<float>*> >* >::iterator v_it;
  v_it = stats_layer_n_.find(class_id);
   vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_joint_histogram<float>*> >* vect;
  if (v_it == stats_layer_n_.end()) {
    vect = new vcl_vector<vcl_pair<brec_part_instance_sptr, bsta_joint_histogram<float>*> >;
    // for now initialize a pair for only the "first/best" part of layer0 and all the others in the hierarchy
    // scan through all vertices to get the part that is added first from layer_id-1
    brec_part_base_sptr p_best;
    for (brec_part_hierarchy::vertex_iterator it = h->vertices_begin(); it != h->vertices_end(); it++) {
      if ((*it)->layer_ == layer_id-1) {
        p_best = (*it);
        break; // return the "first" part of this layer as the best (to be the first, it has to be added before the others into the hierarchy)
      }
    }

    if (!p_best)
      return false;

    unsigned type_cnt = 0;
    for (brec_part_hierarchy::vertex_iterator it = h->vertices_begin(); it != h->vertices_end(); it++) {
      brec_part_base_sptr current_p = *it;
      brec_part_instance_sptr p_n = new brec_part_instance(layer_id, type_cnt, brec_part_instance_kind::COMPOSED, 0.0f, 0.0f, 0.0f);
      brec_hierarchy_edge_sptr e1 = new brec_hierarchy_edge(p_n->cast_to_base(), p_best);
      p_n->add_outgoing_edge(e1);

      brec_hierarchy_edge_sptr e2 = new brec_hierarchy_edge(p_n->cast_to_base(), current_p);
      p_n->add_outgoing_edge(e2);
      type_cnt++;

      bsta_joint_histogram<float> *hist = new bsta_joint_histogram<float>(10.0f, 100);
      vcl_pair<brec_part_instance_sptr, bsta_joint_histogram<float>*> pa(p_n, hist);
      vect->push_back(pa);
    }
    stats_layer_n_[class_id] = vect;
  } else {
    vect = v_it->second;
  }

  // collect stats for ...
  return true;
}

void brec_part_hierarchy_learner::print_layer0()
{
  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    brec_part_instance_sptr pi = stats_layer0_[i].first;
    bsta_histogram<float> *h = stats_layer0_[i].second;
    if (pi->kind_ == brec_part_instance_kind::GAUSSIAN) {
      brec_part_gaussian_sptr p = pi->cast_to_gaussian();
      if (p->bright_)
        vcl_cout << "--- lambda0 " << p->lambda0_ << " --- lambda1 " << p->lambda1_ << " --- theta " << p->theta_ << " --- bright ---\n";
      else
        vcl_cout << "--- lambda0 " << p->lambda0_ << " --- lambda1 " << p->lambda1_ << " --- theta " << p->theta_ << " --- dark ---\n";
    }
    vcl_cout << "----- foreground hist ----------\n";
    h->print();
    vcl_cout << "-------------------------------------------\n";
  }
}

void brec_part_hierarchy_learner::print_to_m_file_layer0(vcl_string file_name)
{
  vcl_ofstream ofs(file_name.c_str());
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

void brec_part_hierarchy_learner::print_to_m_file_layer0_fitted_dists(vcl_string file_name)
{
  vcl_ofstream ofs(file_name.c_str());
  ofs << "% dump histograms of fitted distributions\n";

  for (unsigned i = 0; i < stats_layer0_.size(); i++) {
    brec_part_instance_sptr pi = stats_layer0_[i].first;
    bsta_histogram<float> *h = stats_layer0_[i].second;

    vcl_vector<float> x;
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
      } else {
        vcl_cout << "WARNING: no fitted foreground response model for this operator! Cannot print to m file.\n";
      }
    }
    //ofs << "axis([-7.0 1.0 0.0 1.0]);\n";
  }

  ofs.close();
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & /*os*/, brec_part_hierarchy_learner const & /*ph*/)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value\n";
  return;
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_read(vsl_b_istream & /*is*/, brec_part_hierarchy_learner & /*ph*/)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy_learner as a brdb_value\n";
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
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const brec_part_hierarchy_learner* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}

