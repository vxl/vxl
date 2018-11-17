#include "sdet_mrf_bp.h"
#include <sdet/sdet_mrf_site_bp.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_new.h>
#include <vnl/vnl_numeric_traits.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//
// index for relative image position of neighbors
//      u
//    l   r
//      d
//                       u   l  r  d
static const int di[4]={ 0, -1, 1, 0};
static const int dj[4]={-1,  0, 0, 1};

void lower_envelope_linear(float w, std::vector<float>& msg)
{
  unsigned nlabels = msg.size();
  // pass 1
  for (unsigned fq=1; fq<nlabels; ++fq) {
    float mfq = msg[fq];
    float mfqm1 = msg[fq-1] + w;
    if (mfq<mfqm1)
      msg[fq]=mfq;
    else
      msg[fq]=mfqm1;
  }
  // pass 2
  for (int fq=nlabels-2; fq>=0; --fq) {
    float mfq = msg[fq];
    float mfqp1 = msg[fq+1]+w;
    if (mfq<mfqp1)
      msg[fq]=mfq;
    else
      msg[fq]=mfqp1;
  }
}

std::vector<float> lower_envelope_quadratic(float w,
                                           std::vector<float> const& h)
{
  int nlabels = h.size();
  std::vector<float> env_out(nlabels);
  std::vector<int> v(nlabels);
  std::vector<float> z(nlabels+1);
  int k = 0;
  v[0] = 0;
  z[0] = -vnl_numeric_traits<float>::maxval;
  z[1] = -z[0];

  for (int fq = 1; fq <= nlabels-1; ++fq) {
    float s  = ((h[fq]+w*fq*fq)-(h[v[k]]+w*v[k]*v[k])) / (2*w*(fq-v[k]));
    while (s <= z[k]) {
      k--;
      s  = ((h[fq]+fq*fq)-(h[v[k]]+v[k]*v[k])) / (2*(fq-v[k]));
    }
    ++k;
    v[k] = fq;
    z[k] = s;
    z[k+1] = vnl_numeric_traits<float>::maxval;
  }
  k = 0;
  for (int fq = 0; fq <= nlabels-1; ++fq) {
    while (z[k+1] < fq)
      ++k;
    env_out[fq] = w*(fq-v[k])*(fq-v[k]) + h[v[k]];
  }
  return env_out;
}

sdet_mrf_bp::sdet_mrf_bp(unsigned ni, unsigned nj,
                         unsigned n_labels)
  : ni_(ni), nj_(nj), n_labels_(n_labels), discontinuity_cost_(1.0f),
    truncation_cost_(1.0f), kappa_(1.0f), lambda_(1.0f), min_(0.0f), max_(0.0f)
{
  sites_.resize(nj_, ni_);
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i)
      sites_[j][i]=new sdet_mrf_site_bp(n_labels_,lambda_, truncation_cost_);
}

sdet_mrf_bp::sdet_mrf_bp(const vil_image_resource_sptr& obs_labels, unsigned n_labels,
                         float discontinuity_cost, float truncation_cost,
                         float kappa, float lambda)
  : ni_(0), nj_(0), n_labels_(n_labels),
    discontinuity_cost_(discontinuity_cost),
    truncation_cost_(truncation_cost), kappa_(kappa),
    lambda_(lambda), min_(0.0f), max_(0.0f)
{
  if (!obs_labels) return;
  ni_=obs_labels->ni();   nj_=obs_labels->nj();
  vil_image_view_base_sptr temp = obs_labels->get_view();
  vil_image_view<float> view = *vil_convert_cast(float(), temp);
  vil_math_value_range(view, min_, max_);
  if (min_ >= max_) return;
  sites_.resize(nj_, ni_);
  float scale = (n_labels-1)/(max_-min_);
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      sdet_mrf_site_bp_sptr s =
        new sdet_mrf_site_bp(n_labels_, lambda_, truncation_cost_);
      s->set_label(scale*(view(i,j)-min_));
      sites_[j][i]=s;
    }
}

sdet_mrf_bp::sdet_mrf_bp(vil_image_view<float> const& obs_labels,
                         unsigned n_labels, float discontinuity_cost,
                         float truncation_cost, float kappa, float lambda)
  : n_labels_(n_labels), discontinuity_cost_(discontinuity_cost),
    truncation_cost_(truncation_cost), kappa_(kappa), lambda_(lambda),
    min_(0.0f), max_(0.0f)
{
  if (!obs_labels) return;
  ni_=obs_labels.ni();   nj_=obs_labels.nj();
  vil_math_value_range(obs_labels, min_, max_);
  if (min_ >= max_) return;
  sites_.resize(nj_, ni_);
  float scale = (n_labels-1)/(max_-min_);
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      sdet_mrf_site_bp_sptr s =
        new sdet_mrf_site_bp(n_labels_, lambda_, truncation_cost_);
      s->set_label(scale*(obs_labels(i,j)-min_));
      sites_[j][i]=s;
    }
}

sdet_mrf_bp::sdet_mrf_bp(vil_image_view<float> const& obs_labels,
                         vil_image_view<float> const& var,  unsigned n_labels,
                         float discontinuity_cost, float truncation_cost,
                         float kappa, float lambda)
  : n_labels_(n_labels), discontinuity_cost_(discontinuity_cost),
    truncation_cost_(truncation_cost), kappa_(kappa),
    lambda_(lambda), min_(0.0f), max_(0.0f)
{
  ni_=obs_labels.ni();   nj_=obs_labels.nj();
  if (!ni_||!nj_) return;
  vil_math_value_range(obs_labels, min_, max_);
  if (min_ >= max_) return;
  sites_.resize(nj_, ni_);
  float scale = (n_labels-1)/(max_-min_);
  int ni = static_cast<int>(ni_), nj = static_cast<int>(nj_);
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      float vlamb = lambda_;
      if (var(i,j)>0.0f)
        vlamb = lambda_/var(i,j);
      sdet_mrf_site_bp_sptr s =
        new sdet_mrf_site_bp(n_labels_,vlamb, truncation_cost_);
      s->set_label(scale*(obs_labels(i,j)-min_));
      sites_[j][i]=s;
    }
}

sdet_mrf_bp::sdet_mrf_bp(const vil_image_resource_sptr&  obs_labels,
                         const vil_image_resource_sptr&  var,
                         unsigned n_labels, float discontinuity_cost,
                         float truncation_cost, float kappa, float lambda)
  :  n_labels_(n_labels),discontinuity_cost_(discontinuity_cost),
     truncation_cost_(truncation_cost), kappa_(kappa), lambda_(lambda),
     min_(0.0f), max_(0.0f)
{
  if (!obs_labels) return;
  ni_=obs_labels->ni();   nj_=obs_labels->nj();
  vil_image_view_base_sptr temp = obs_labels->get_view();
  vil_image_view<float> view = *vil_convert_cast(float(), temp);
  vil_math_value_range(view, min_, max_);
  if (min_ >= max_) return;
  sites_.resize(nj_, ni_);
  float scale = (n_labels-1)/(max_-min_);
  vil_image_view_base_sptr tempv = var->get_view();
  vil_image_view<float> var_view = *vil_convert_cast(float(), temp);
  int ni = static_cast<int>(ni_), nj = static_cast<int>(nj_);
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      float vlamb = lambda_/var_view(i,j);
      sdet_mrf_site_bp_sptr s =
        new sdet_mrf_site_bp(n_labels_,vlamb, truncation_cost_);
      s->set_label(scale*(view(i,j)-min_));
      sites_[j][i]=s;
    }
}

void sdet_mrf_bp::send_messages_optimized()
{
  const int ni = static_cast<int>(ni_), nj = static_cast<int>(nj_);
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      //site sending the messages
      sdet_mrf_site_bp_sptr sp = sites_[j][i];
      for (int n = 0; n<4; ++n) {
        int ki = i+di[n], kj = j+dj[n];
        if (ki<0||ki>=ni||kj<0||kj>=nj)
          continue;
        //site receiving a message
        sdet_mrf_site_bp_sptr sq = sites_[kj][ki];

        //initialize message with h(fp)
        std::vector<float> temp(n_labels_), msg;
        // minr is the smallest value of h
        float minh = vnl_numeric_traits<float>::maxval;
        for (unsigned fq = 0; fq<n_labels_; ++fq) {
          temp[fq]=sp->h(n, fq);
          if (temp[fq]<minh)
            minh = temp[fq];
        }
        // compute the lower bound on msg(q)
        msg = lower_envelope_quadratic(kappa_, temp);

        // clamp message value to an upper bound (msg min + disc cost)
        minh += discontinuity_cost_;
        for (unsigned fq=0; fq<n_labels_; ++fq)
          if (msg[fq]>minh)
            msg[fq]=minh;

        // normalize message values to prevent divergence
        // compute the average message value
        float summ = 0.0f;
        for (unsigned fq=0; fq<n_labels_; ++fq)
          summ += msg[fq];
        summ /= n_labels_;

        //subtract the average
        for (unsigned fq=0; fq<n_labels_; ++fq) {
          float ms = msg[fq]-summ;
          //if this assert fails, the number of labels is too large
          //compared to the dynamic range of the message elements
          assert(ms<=static_cast<float>(vnl_numeric_traits<short>::maxval)
              && ms>=-static_cast<float>(vnl_numeric_traits<short>::maxval)-1.f);
          sq->set_cur_message(3-n, fq, ms);
        }
      }
    }
  //all done sending messages so swap buffers
  for (int j = 0; j<nj; ++j)
    for (int i = 0; i<ni; ++i) {
      sdet_mrf_site_bp_sptr sp = sites_[j][i];
      sp->switch_buffers();
    }
}

void sdet_mrf_bp::set_prior_message(unsigned i, unsigned j, unsigned n,
                                    std::vector<float> const& msg)
{
  this->site(i,j)->set_prior_message(n, msg);
}

void sdet_mrf_bp::print_prior_messages()
{
  std::cout << "Neighbor layout\n"
           << "     0\n"
           << "  1  x  2\n"
           << "     3\n\n";

  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      sdet_mrf_site_bp_sptr sp = sites_[j][i];
      std::cout << " site(" << i << ' ' << j << ")==>\n";
      sp->print_prior_messages();
    }
}

void sdet_mrf_bp::print_belief_vectors()
{
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      sdet_mrf_site_bp_sptr sp = sites_[j][i];
      std::cout << " site(" << i << ' ' << j << ")==>\n";
      sp->print_belief_vector();
    }
}

vil_image_resource_sptr sdet_mrf_bp::belief_image()
{
  vil_image_resource_sptr ret = nullptr;
  if (nj_==0||ni_==0)
    return ret;
  vil_image_view<float> view(ni_, nj_);
  if (min_==max_)
    return ret;
  float scale = (max_-min_)/static_cast<float>(n_labels_-1);
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      sdet_mrf_site_bp_sptr sp = sites_[j][i];
      if (!sp) continue;
      auto label = static_cast<float>(sp->believed_label());
      view(i,j) = scale*label + min_;
    }
  ret = vil_new_image_resource_of_view(view);
  return ret;
}

std::vector<float> sdet_mrf_bp::prior_message(unsigned i, unsigned j, unsigned n)
{
  sdet_mrf_site_bp_sptr sp = sites_[j][i];
  return sp->prior_message(n);
}

void sdet_mrf_bp::clear()
{
  for (unsigned j = 0; j<nj_; ++j)
    for (unsigned i = 0; i<ni_; ++i) {
      sdet_mrf_site_bp_sptr sp = sites_[j][i];
      if (!sp) continue;
      sp->clear();
    }
}
