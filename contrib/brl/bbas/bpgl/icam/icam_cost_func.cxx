// This is bbas/bpgl/icam/icam_cost_func.cxx
#include "icam_cost_func.h"
//:
// \file

#include <vbl/vbl_array_1d.h>
#include <icam/icam_sample.h>
#include <vnl/vnl_numeric_traits.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
icam_cost_func::icam_cost_func( const vil_image_view<float>& source_img,
                                const vil_image_view<float>& dest_img,
                                const icam_depth_transform& dt,
                                unsigned nbins)
 : vnl_least_squares_function(1,1),
   source_image_(source_img),
   dest_image_(dest_img),
   dt_(dt), max_samples_(1), n_samples_(0), nbins_(nbins)
{
  unsigned ni = dest_image_.ni()-2, nj = dest_image_.nj()-2;
  max_samples_ = ni*nj;
  assert(max_samples_>0);
  dest_samples_.set_size(max_samples_);
  dest_samples_.fill(0.0);
  unsigned index = 0;
  for (unsigned j = 1; j<=nj; ++j)
    for (unsigned i = 1; i<=ni; ++i)
      dest_samples_[index++]=dest_image_(i,j);
#if 0
  std::cout << "dest samples\n";


  int cent = dest_samples_.size()/2-(dest_image_.ni()-2)/2;
  for (int i = -10; i<=10; ++i)
    std::cout << dest_samples_[i+cent] << '\n';

  std::cout << '\n';
#endif
  use_gradient_ = false;
  vnl_least_squares_function::init(dt_.n_params(), dest_samples_.size());
}

//: The main function.
//  Given the parameter vector x, compute the vector of residuals fx.
void
icam_cost_func::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
{
  dt_.set_params(x);
  vnl_vector<double> from_samples, from_mask;
  icam_sample::sample(dest_image_.ni(), dest_image_.nj(), source_image_,
                      dt_, from_samples, from_mask, n_samples_);
  fx = element_product<double>(from_mask,(from_samples - dest_samples_));
}

double icam_cost_func::error(vnl_vector_fixed<double,3> rodrigues,
                             vgl_vector_3d<double> trans,
                             double min_allowed_overlap)
{
  vnl_vector<double> pr(dt_.n_params());
  vnl_vector<double> res;
  pr[0]=rodrigues[0];   pr[1]=rodrigues[1];   pr[2]=rodrigues[2];
  pr[3]=trans.x();   pr[4]=trans.y();   pr[5]=trans.z();
  dt_.set_params(pr);
  vnl_vector<double> from_samples, from_mask;
  icam_sample::sample(dest_image_.ni(), dest_image_.nj(), source_image_,
                      dt_, from_samples, from_mask, n_samples_);
  double mag = 0;
  for (unsigned i = 0; i<from_samples.size(); ++i)
    if (from_mask[i]>0.0)
      mag += std::fabs(from_samples[i]-dest_samples_[i]);
  if (this->frac_samples()>min_allowed_overlap) {
    mag/= n_samples_;
    return mag;
  }
  return vnl_numeric_traits<double>::maxval;
}

void icam_cost_func::samples(vnl_vector_fixed<double, 3> rodrigues,
                             vgl_vector_3d<double> trans,
                             vnl_vector<double>& trans_source,
                             vnl_vector<double>& mask,
                             vnl_vector<double>& dest)
{
  vnl_vector<double> pr(dt_.n_params());
  vnl_vector<double> res;
  pr[0]=rodrigues[0];   pr[1]=rodrigues[1];   pr[2]=rodrigues[2];
  pr[3]=trans.x();   pr[4]=trans.y();   pr[5]=trans.z();
  dt_.set_params(pr);
  icam_sample::sample(dest_image_.ni(), dest_image_.nj(), source_image_,
                      dt_, trans_source, mask, n_samples_);
  dest = dest_samples_;
}

std::vector<double> icam_cost_func::error(vnl_vector<double> const& x,
                                         unsigned param_index, double pmin,
                                         double pmax, double pinc)
{
  std::vector<double> ret;
  vnl_vector<double> pr(x);
  vnl_vector<double> res;
  for (double p = pmin; p<=pmax; p+=pinc)
  {
    pr[param_index] = p;
    this->f(pr, res);
    double mag = res.magnitude();
    if (n_samples_) {
      mag/= n_samples_;
      ret.push_back(mag);
    }
    else
    {ret.push_back(vnl_numeric_traits<double>::maxval);}
  }
  return ret;
}

vbl_array_2d<double> icam_cost_func::
joint_probability(vnl_vector<double> const& samples, vnl_vector<double> const& mask)
{
  double scl = 1.0/(256.0/nbins_);
  vbl_array_2d<double> h(nbins_, nbins_, 0.0);

  //compute the intensity histogram
  for (unsigned i = 0; i<samples.size(); ++i)
    if (mask[i]>0.0) {

#if 0
      unsigned id = static_cast<unsigned>(dest_samples_[i]*scl + 0.5),
        is = static_cast<unsigned>(samples[i]*scl + 0.5);
#endif
      //match the gpu implementation, which does a floor operation
      auto id = static_cast<unsigned>(std::floor(dest_samples_[i]*scl)),
        is = static_cast<unsigned>(std::floor(samples[i]*scl));

      if (id>nbins_-1 || is> nbins_-1)
        continue;
      h[id][is] += 1.0;
    }
  // convert to probability
  for (unsigned r = 0; r<nbins_; ++r)
    for (unsigned c = 0; c<nbins_; ++c)
      h[r][c] /= n_samples_;
  return h;
}


vbl_array_2d<double> icam_cost_func::
joint_probability(vnl_vector_fixed<double, 3> rodrigues,
                  vgl_vector_3d<double> trans)
{
  vnl_vector<double> pr(dt_.n_params());
  vnl_vector<double> res;
  pr[0]=rodrigues[0];   pr[1]=rodrigues[1];   pr[2]=rodrigues[2];
  pr[3]=trans.x();   pr[4]=trans.y();   pr[5]=trans.z();
  dt_.set_params(pr);
  vnl_vector<double> from_samples, from_mask;
  icam_sample::sample(dest_image_.ni(), dest_image_.nj(), source_image_,
                      dt_, from_samples, from_mask, n_samples_);

  #if 0
  std::cout << "Native produced ";
  std::cout << "mapped/dest/mask samples\n";
  int cent = dest_samples_.size()/2-(dest_image_.ni()-2)/2;
  for (int i = -10; i<=10; ++i) {
    std::cout << from_samples[i+cent] << ' '
             << dest_samples_[i+cent] << ' '
             << 50.0f*from_mask[i+cent] << '\n';
  }
#endif
  return joint_probability(from_samples, from_mask);
}

vbl_array_2d<double>
icam_cost_func::joint_probability(vil_image_view<float> const& map_dest,
                                  vil_image_view<float> const& map_mask)
{
  vnl_vector<double> from_samples, from_mask;
  icam_sample::sample(map_dest, map_mask,from_samples, from_mask, n_samples_);

#if 0
  std::cout << "GPU produced ";
  std::cout << "mapped/dest/mask samples\n";
  int cent = dest_samples_.size()/2-(dest_image_.ni()-2)/2;
  for (int i = -10; i<=10; ++i) {
    std::cout << from_samples[i+cent] << ' '
             << dest_samples_[i+cent] << ' '
             << 50.0f*from_mask[i+cent] << '\n';
  }
#endif
  return joint_probability(from_samples, from_mask);
}

double icam_cost_func::minfo(vbl_array_2d<double>& joint_prob)
{
  unsigned nr = joint_prob.rows(), nc = joint_prob.cols();

  //marginal distributions
  vbl_array_1d<double> pmr(nc,0.0), pmc(nr, 0.0);
  for (unsigned r = 0; r<nr; ++r)
    for (unsigned c = 0; c<nc; ++c) {
      double p = joint_prob[r][c];
      pmr[c]+=p;
      pmc[r]+=p;
    }
  double sum = 0.0;
  for (unsigned r = 0; r<nr; ++r) {
    double pc = pmc[r];
    if (pc==0) continue;
    for (unsigned c = 0; c<nc; ++c) {
      double prc = joint_prob[r][c];
      double pr = pmr[c];
      if (prc>0&&pr>0)
        sum+= prc*std::log(prc/(pr*pc));
    }
  }
  return sum/std::log(2.0);
}

double icam_cost_func::mutual_info(vnl_vector_fixed<double, 3> rodrigues,
                                   vgl_vector_3d<double> trans,
                                   double min_allowed_overlap)
{
  vbl_array_2d<double> jp = this->joint_probability(rodrigues, trans);
  if (this->frac_samples()<=min_allowed_overlap)
    return vnl_numeric_traits<double>::maxval;
  else
    return this->minfo(jp);
}

double icam_cost_func::mutual_info(vil_image_view<float> const& map_dest,
                                   vil_image_view<float> const& map_mask,
                                   double min_allowed_overlap)
{
  vbl_array_2d<double> jp = this->joint_probability(map_dest, map_mask);
  if (this->frac_samples()<=min_allowed_overlap)
    return 0.0;
  else
    return this->minfo(jp);
}

double icam_cost_func::entropy_diff(vbl_array_2d<double>& joint_prob)
{
  unsigned nr = joint_prob.rows(), nc = joint_prob.cols();
  //marginal distribution for mapped dest intensities
  vbl_array_1d<double> pmr(nc,0.0);
  for (unsigned r = 0; r<nr; ++r)
    for (unsigned c = 0; c<nc; ++c)
      pmr[c]+=joint_prob[r][c];
  double jsum = 0.0, msum = 0.0;
  for (unsigned c = 0; c<nc; ++c)
  {
    double pr = pmr[c];
#ifdef DEBUG
    std::cout << pr << '\n';
#endif
    if (pr>0)
      msum += pr*std::log(pr);
  }
  for (unsigned r = 0; r<nr; ++r)
    for (unsigned c = 0; c<nc; ++c) {
        double prc = joint_prob[r][c];
        if (prc>0)
          jsum+= prc*std::log(prc);
    }
  double ent_dif = jsum - msum;
  return -ent_dif/std::log(2.0);
}

double icam_cost_func::entropy_diff(vnl_vector_fixed<double, 3> rodrigues,
                                    vgl_vector_3d<double> trans,
                                    double min_allowed_overlap)
{
  vbl_array_2d<double> jp = this->joint_probability(rodrigues, trans);
  if (this->frac_samples()<=min_allowed_overlap)
    return vnl_numeric_traits<double>::maxval;
  else
    return this->entropy_diff(jp);
}


vil_image_view<float>
icam_cost_func::mapped_dest(vnl_vector_fixed<double, 3> rodrigues,
                            vgl_vector_3d<double> trans)
{
  vnl_vector<double> pr(dt_.n_params());
  vnl_vector<double> res;
  pr[0]=rodrigues[0];   pr[1]=rodrigues[1];   pr[2]=rodrigues[2];
  pr[3]=trans.x();   pr[4]=trans.y();   pr[5]=trans.z();
  dt_.set_params(pr);
  vil_image_view<float> mdest, mask;
  icam_sample::resample(dest_image_.ni(), dest_image_.nj(), source_image_,
                        dt_, mdest, mask, n_samples_);
  for (unsigned j = 0; j<dest_image_.nj(); ++j)
    for (unsigned i = 0; i<dest_image_.ni(); ++i)
      if (mask(i,j)==0.0f)
        mdest(i,j) = 0.0f;
  return mdest;
}

double icam_scalar_cost_func::f(vnl_vector<double> const& x)
{
  vnl_vector_fixed<double, 3> rod = x;
  vgl_vector_3d<double> trans(0,0,0);
  return cost_func_.entropy_diff(rod, trans,min_allowed_overlap_);
}
