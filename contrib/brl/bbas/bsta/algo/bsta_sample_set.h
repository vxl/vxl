// This is brl/bbas/bsta/algo/bsta_sample_set.h
#ifndef bsta_sample_set_h_
#define bsta_sample_set_h_
//:
// \file
// \brief Classes to collect samples
//
// \author Ozge C. Ozcanli
// \date March 04, 2009
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//
#include <iostream>
#include <fstream>
#include <utility>
#include <bsta/bsta_parzen_sphere.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_gaussian_full.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define MIN_VAR_  0.0001

//: A class to hold samples, the window width parameter, weights for each sample, assignments of each sample to modes/cluster centers/classes
//  This class is used by mean-shift and EM algorithms
template <class T, unsigned n>
class bsta_sample_set : public bsta_parzen_sphere<T,n>
{
 public:

  typedef typename bsta_parzen_sphere<T,n>::vector_type vector_;

  // Constructor
  bsta_sample_set(T bandwidth = T(1)) : bsta_parzen_sphere<T,n>() { this->set_bandwidth(bandwidth); }

  //: Compute the mean in a window around the given pt, the window size is the bandwidth
  //  If there are no points within bandwidth of the input pt, \return false
  bool mean(vector_ const& pt, vector_& out);

  //: Insert a weighted sample into the distribution
  void insert_sample(vector_ const& sample, T weight = T(1.0));

  T weight(unsigned i) const { return weights_[i]; }
  std::vector<T>& weights() { return weights_; }

  //: one may need to normalize the weights after the insertion is over
  void normalize_weights();

  //: must call this method before using the assignment vector
  void initialize_assignments();  // initializes each sample's assignment to -1 (null assignment)
  std::vector<int>& assignments() { return assignments_; }
  int assignment(unsigned i) const { return assignments_[i]; }
  void set_assignment(unsigned i, int mode) { assignments_[i] = mode; }

  //: compute the mean of a particular assignment/mode/cluster
  bool mode_mean(int mode, vector_& out) const;

  typename std::vector<typename bsta_parzen_sphere<T,n>::vector_type >::const_iterator samples_begin() const { return bsta_parzen<T,n>::samples_.begin(); }
  typename std::vector<typename bsta_parzen_sphere<T,n>::vector_type >::const_iterator samples_end() const { return bsta_parzen<T,n>::samples_.end(); }

  typename std::vector<T >::const_iterator weights_begin() const { return weights_.begin(); }
  typename std::vector<T >::const_iterator weights_end() const { return weights_.end(); }

  std::vector<int >::const_iterator assignments_begin() const { return assignments_.begin(); }
  std::vector<int >::const_iterator assignments_end() const { return assignments_.end(); }

  bool check_initializations() const { return bsta_parzen<T,n>::samples_.size() == weights_.size() &&
                                        bsta_parzen<T,n>::samples_.size() == assignments_.size(); }

  //: return number of assignments to this mode
  int mode_size(int mode) const;

  //: return total weight of assignments to this mode
  T mode_weight(int mode) const;

  //: return number of modes in the current assignment vector
  unsigned mode_cnt() const;

  //: return total weight of all assignments
  T total_weight() const;

 private:
  //: hold a vector of weights for each data sample
  //  Needs to be set separately with each insert into the data set,
  //  otherwise it's set to 1.0 by default at the first call to mean()
  std::vector<T> weights_;

  std::vector<int> assignments_;  // a negative value indicates "null assignment"
};

//: compute the variance of a particular assignment in a bsta_sample_set
template <class T>
bool bsta_sample_set_variance(const bsta_sample_set<T,1>& set, int mode, T min_var, T& out)
{
  typedef typename std::vector<T >::const_iterator sit_t;
  typedef typename std::vector<T >::const_iterator wit_t;
  typedef typename std::vector<int >::const_iterator ait_t;

  if (!set.check_initializations()) {
    std::cout << "Error in - bsta_sample_set<T,n>::mean() : assignments not initialized!\n";
    return false;
  }

  T mv;
  set.mode_mean(mode, mv);

  T sum(T(0));
  sit_t sit = set.samples_begin();
  wit_t wit = set.weights_begin();
  ait_t ait = set.assignments_begin();
  T nsamp = 0;
  for (; sit != set.samples_end(); ++sit, ++wit, ++ait) {
    if (*ait != mode)
      continue;

    T s = (*sit-mv)*(*sit-mv);
    sum += (*wit)*s;
    nsamp += (*wit);
  }
  if (nsamp > 0) {
    out = sum / nsamp;
    if (out < min_var)
      out = min_var;
    return true;
  }

  return false;
}

//: compute the variance of a particular assignment in a bsta_sample_set
template <class T, unsigned n>
bool bsta_sample_set_variance(const bsta_sample_set<T,n>& set, int mode, vnl_matrix_fixed<T,n,n>& out)
{
  typedef typename std::vector<vnl_vector_fixed<T,n> >::const_iterator sit_t;
  typedef typename std::vector<T >::const_iterator wit_t;
  typedef typename std::vector<int >::const_iterator ait_t;

  if (!set.check_initializations()) {
    std::cout << "Error in - bsta_sample_set<T,n>::mean() : assignments not initialized!\n";
    return false;
  }

  vnl_vector_fixed<T,n> mv;
  set.mode_mean(mode, mv);

  vnl_matrix_fixed<T,n,n> sum(T(0));
  sit_t sit = set.samples_begin();
  wit_t wit = set.weights_begin();
  ait_t ait = set.assignments_begin();
  T nsamp = 0;
  for (; sit != set.samples_end(); ++sit, ++wit, ++ait) {
    if (*ait != mode)
      continue;

    vnl_vector_fixed<T,n> diff = (*sit)-mv;
    sum += (*wit)*outer_product(diff,diff);
    nsamp += (*wit);
  }
  if (nsamp > 0) {
    out = sum / nsamp;

    return true;
  }

  return false;
}

//: compute the marginalized 1D sample set distribution from nD set
template <class T, unsigned n>
bool bsta_sample_set_marginalize(const bsta_sample_set<T,n>& set, unsigned component, bsta_sample_set<T,1>& out_set)
{
  typedef typename std::vector<vnl_vector_fixed<T,n> >::const_iterator sit_t;
  typedef typename std::vector<T >::const_iterator wit_t;

  if (n <= component)  // if the vector is not as large to have component return false
    return false;

  sit_t sit = set.samples_begin();
  wit_t wit = set.weights_begin();

  for (; sit != set.samples_end(); ++sit, ++wit) {
    out_set.insert_sample((*sit)[component], (*wit));
  }

  return true;
}

template <class T>
bool bsta_sample_set_fit_distribution(const bsta_sample_set<T,1>& set, bsta_mixture<bsta_num_obs<bsta_gaussian_sphere<T,1> > >& out)
{
  if (!set.check_initializations()) {
    std::cout << "Error in - bsta_sample_set<T,n>::mean() : assignments not initialized!\n";
    return false;
  }

  // compute mean and variance for each mode
  unsigned mode_cnt = set.mode_cnt();

  while (out.num_components() != 0) {
    out.remove_last();
  }

  T total_weight = set.total_weight();
  for (unsigned mi = 0; mi < mode_cnt; mi++) {
    T meanv;
    set.mode_mean(mi, meanv);
    T var;
    if (!bsta_sample_set_variance(set, mi, T(MIN_VAR_), var))
      return false;
    T w = set.mode_weight(mi);
    bsta_gaussian_sphere<T,1> gauss_d(meanv,var);
    bsta_num_obs<bsta_gaussian_sphere<T,1> > gauss(gauss_d, w);
    if (!out.insert(gauss, w/total_weight))
      return false;
  }

  return true;
}

template <class T, unsigned n>
bool bsta_sample_set_fit_distribution(const bsta_sample_set<T,n>& set, bsta_mixture<bsta_num_obs<bsta_gaussian_full<T,n> > >& out)
{
  if (!set.check_initializations()) {
    std::cout << "Error in - bsta_sample_set<T,n>::mean() : assignments not initialized!\n";
    return false;
  }

  // compute mean and variance for each mode
  unsigned mode_cnt = set.mode_cnt();

  while (out.num_components() != 0) {
    out.remove_last();
  }

  T total_weight = set.total_weight();
  for (unsigned mi = 0; mi < mode_cnt; mi++) {
    vnl_vector_fixed<T,n> meanv;
    set.mode_mean(mi, meanv);
    vnl_matrix_fixed<T,n,n> covar;
    if (!bsta_sample_set_variance(set, mi, covar))
      return false;
    T w = set.mode_weight(mi);
    bsta_gaussian_full<T,n> gauss_d(meanv,covar);
    bsta_num_obs<bsta_gaussian_full<T,n> > gauss(gauss_d, w);
    if (!out.insert(gauss, w/total_weight))
      return false;
  }

  return true;
}

//:
//  Total weight is used to normalize the weight of the distribution
//  (bsta_num_obs class contains total weight of samples assigned to this distribution)
template <class T>
T bsta_sample_set_log_likelihood(const bsta_sample_set<T,1>& set, bsta_num_obs<bsta_gaussian_sphere<T,1> >& dist, T total_weight)
{
  if (!set.size()) {
    std::cout << "Error in - bsta_sample_set<T,n>::bsta_sample_set_log_likelihood() : assignments not initialized!\n";
    return T(0);
  }

  T w = dist.num_observations;
  T p_dist = w/total_weight;
  T sum = T(0);
  for (unsigned i = 0; i < set.size(); i++) {
    if (std::sqrt(dist.sqr_mahalanobis_dist(set.sample(i))) < 3) {// we don't want zero to be logged
      T p = T(std::log(dist.prob_density(set.sample(i))));
      T pw = T(std::log(p_dist));
      sum += p + pw;  // log is natural logarithm
    }
  }

  return sum;
}

//:
//  Total weight is used to normalize the weight of the distribution
//  (bsta_num_obs class contains total weight of samples assigned to this distribution)
template <class T, unsigned n>
T bsta_sample_set_log_likelihood(const bsta_sample_set<T,n>& set, bsta_num_obs<bsta_gaussian_full<T,n> >& dist, T total_weight)
{
  if (!set.size()) {
    std::cout << "Error in - bsta_sample_set<T,n>::bsta_sample_set_log_likelihood() : assignments not initialized!\n";
    return T(0);
  }

  T w = dist.num_observations;
  T p_dist = w/total_weight;
  T sum = T(0);
  for (unsigned i = 0; i < set.size(); i++) {
    if (std::sqrt(dist.sqr_mahalanobis_dist(set.sample(i))) < 3) {// we don't want zero to be logged
      T p = T(std::log(dist.prob_density(set.sample(i))));
      T pw = T(std::log(p_dist));
      sum += p + pw;  // log is natural logarithm
    }
  }

  return sum;
}

//:
//  Total weight is used to normalize the weight of the distribution
//  (bsta_num_obs class contains total weight of samples assigned to this distribution)
template <class T>
T bsta_sample_set_log_likelihood(const bsta_sample_set<T,2>& set, bsta_num_obs<bsta_gaussian_sphere<T,1> >& dist0, T w0, bsta_num_obs<bsta_gaussian_sphere<T,1> >& dist1, T w1, T& w_sum)
{
  if (!set.size()) {
    std::cout << "Error in - bsta_sample_set<T,n>::bsta_sample_set_log_likelihood() : set is empty!\n";
    return T(0);
  }

  T total_weight = T(0);
  w_sum = T(0);
  T sum = T(0);
  unsigned cnt = 0;
  for (unsigned i = 0; i < set.size(); i++) {

    T d0 = dist0.sqr_mahalanobis_dist(set.sample(i)[0]);
    T d1 = dist1.sqr_mahalanobis_dist(set.sample(i)[1]);
    T d0_sqrt = std::sqrt(d0);
    T d1_sqrt = std::sqrt(d1);

    if (d0_sqrt < 3) {
      if (d1_sqrt < 3) {  // if this sample belongs to both of these modes
        w_sum += set.weight(i);
        cnt++;
      } else {
        d1 = 9; // make max distance to be 9
      }
    } else {
      d0 = 9;
    }

    T p = dist0.dist_prob_density(d0);
    p *= w0;
    T p0 = T(std::log(p));
    p = dist1.dist_prob_density(d1);
    p *= w1;
    T p1 = T(std::log(p));

    sum += p0 + p1;  // log is natural logarithm

    total_weight += set.weight(i);
  }

  // w_sum is the total weight of all the samples assigned to these two modes w_sum/total_weight becomes the probability of this joint mode
  T prior = w_sum/total_weight;
  T log_prior = T(std::log(prior));
  T tot_log_prior = set.size()*log_prior;
  sum += tot_log_prior;

  return sum;
}


//: a specialized matlab file printer for 2D data
template<class T>
bool bsta_sample_set_print_to_m(const bsta_sample_set<T,2>& set, std::ofstream& of)
{
  // print samples in different colors according to the assignment
  unsigned mode_cnt = set.mode_cnt();

  of << "cmap = colormap(lines(" << mode_cnt << "));\n";

  for (unsigned m = 0; m < mode_cnt; m++) {
    std::vector<std::pair<T,T> > points;
    for (unsigned i = 0; i < set.size(); i++) {
      if (set.assignment(i) == m)
        points.push_back(std::pair<T,T>(T(set.sample(i)[0]), T(set.sample(i)[1])));
    }
    if (points.size() > 0) {
      of << "x = [" << points[0].first;
      for (unsigned i = 1; i < points.size(); i++) {
        of << ", " << points[i].first;
      }
      of << "];\n";
      of << "y = [" << points[0].second;
      for (unsigned i = 1; i < points.size(); i++) {
        of << ", " << points[i].second;
      }
      of << "];\n";
      of << "h = plot(x,y,'or');\nset(h, 'Color', cmap(" << m+1 << ",:));\n";
      of << "hold on\n";
    }

    vnl_vector_fixed<T,2> mode;
    set.mode_mean(m, mode);
    of << "xx = [" << mode[0] << "];\n";
    of << "yy = [" << mode[1] << "];\n";
    of << "h = plot(xx,yy,'+r');\nset(h, 'Color', cmap(" << m+1 << ",:));\n";
    of << "hold on\n";
  }

  return true;
}

//: a specialized matlab file printer to visualize printed distribution
template<class T>
bool bsta_sample_set_dist_print_to_m(const bsta_sample_set<T,2>& set, std::ofstream& of)
{
  // print samples in different colors according to the assignment
  unsigned mode_cnt = set.mode_cnt();

  of << "cmap = colormap(lines(" << mode_cnt << "));\n";

  // find range of data to construct surface properly
  T min = T(10000), max = T(0);
  for (unsigned i = 0; i < set.size(); i++) {
    if (set.sample(i)[0] < min)
      min = set.sample(i)[0];
    if (set.sample(i)[0] > max)
      max = set.sample(i)[0];
    if (set.sample(i)[1] < min)
      min = set.sample(i)[1];
    if (set.sample(i)[1] > max)
      max = set.sample(i)[1];
  }

  of << "[x,y] = meshgrid(" << min << ":.2:" << max << ", " << min << ":.2:" << max << ");\n";

  T total_weight = set.total_weight();
  for (unsigned m = 0; m < mode_cnt; m++) {
    vnl_vector_fixed<T,2> mode;
    set.mode_mean(m, mode);
    of << "mu = [" << mode[0] << ' ' << mode[1] << "];\n";
    vnl_matrix_fixed<T,2,2> covar;
    if (!bsta_sample_set_variance(set, m, covar))
      return false;
    of << "sigma = [";
    for (unsigned r = 0; r < 2; r++) {
      for (unsigned c = 0; c < 2; c++)
        of << covar[r][c] << ' ';
      if (r == 0)
        of << ';';
    }
    of << "];\n";
    of << "X = [x(:) y(:)];\n";
    of << "p = " << set.mode_weight(m)/total_weight << "*mvnpdf(X, mu, sigma);\n";
    of << "c = " << (m+1) << "*ones(size(x));\n";
    of << "surf(x,y,reshape(p,size(x,1),size(x,2)),c);\n";
    of << "hold on\n";
  }

  return true;
}

//: a specialized matlab file printer to visualize printed distribution
template<class T>
bool bsta_sample_set_dist_print_to_m(const bsta_sample_set<T,1>& set, std::ofstream& of)
{
  // print samples in different colors according to the assignment
  unsigned mode_cnt = set.mode_cnt();

  of << "cmap = colormap(lines(" << mode_cnt << "));\n";

  // find range of data to construct surface properly
  T min = T(10000), max = T(0);
  for (unsigned i = 0; i < set.size(); i++) {
    if (set.sample(i) < min)
      min = set.sample(i);
    if (set.sample(i) > max)
      max = set.sample(i);
  }

  of << "X = [" << min << ":.2:" << max << "]';\n";

  T total_weight = set.total_weight();
  for (unsigned m = 0; m < mode_cnt; m++) {
    T mode;
    set.mode_mean(m, mode);
    of << "mu = [" << mode << "];\n";
    T var;
    if (!bsta_sample_set_variance(set, m, MIN_VAR_, var))
      return false;
    of << "sigma = [" << var << "];\n";
    of << "p = " << set.mode_weight(m)/total_weight << "*mvnpdf(X, mu, sigma);\n";
    of << "hh = plot(X,p,'+r');\nset(hh, 'Color', cmap(" << m+1 << ",:));\n";
    of << "hold on\n";
  }

  return true;
}

#endif // bsta_sample_set_h_
