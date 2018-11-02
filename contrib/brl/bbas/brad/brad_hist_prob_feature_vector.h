// This is brl/bbas/brad/brad_hist_prob_feature_vector.h
#ifndef brad_hist_prob_feature_vector_h
#define brad_hist_prob_feature_vector_h
//:
// \file
// \brief Extract a feature vector from histogram probabilities
// \author J.L. Mundy
// \date June 30, 2011
//
//  This class acts as a functor to be used in eigenvector and
//  classifier processes
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <vsl/vsl_binary_io.h>

class brad_hist_prob_feature_vector
{
 public:
  brad_hist_prob_feature_vector()= default;

  brad_hist_prob_feature_vector(float minval, float maxval, unsigned nbins)
    : min_(minval), max_(maxval), nbins_(nbins) {};

  ~brad_hist_prob_feature_vector()= default;;

  //: vector of histogram probabilities computed from the input view, plus entropy
  vnl_vector<double> operator() (vil_image_view<float> const& view) const;

  //: the number of elements in the vector
  unsigned size() const { return nbins_+1;}

  //: the type name
  std::string type() const {return "brad_hist_prob_feature_vector";}

  //: accessors, setters
  unsigned nbins() const { return nbins_;}
  float min() const {return min_;}
  float max() const {return max_;}
  void set_nbins(unsigned nbins) {nbins_ = nbins;}
  void set_min(float minval) {min_ = minval;}
  void set_max(float maxval) {max_ = maxval;}
  //: print
  void print(std::ostream& os = std::cout) const {
    os << "nbins = " << nbins_ << " min = "
       << min_ << " max = " << max_ << '\n';
  }

 private:
  float min_, max_;
  unsigned nbins_;
};

void vsl_b_write(vsl_b_ostream &os, const brad_hist_prob_feature_vector& fv);

void vsl_b_read(vsl_b_istream &is, brad_hist_prob_feature_vector& fv);

void vsl_print_summary(std::ostream &os, brad_hist_prob_feature_vector& fv);


#endif // brad_hist_prob_feature_vector_h
