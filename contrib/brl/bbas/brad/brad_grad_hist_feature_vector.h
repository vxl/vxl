// This is brl/bbas/brad/brad_grad_hist_feature_vector.h
#ifndef brad_grad_hist_feature_vector_h
#define brad_grad_hist_feature_vector_h
//:
// \file
// \brief Extract a feature vector from image gradient magnitudes
// \author J.L. Mundy
// \date June 30, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//
//  This class acts as a functor to be used in eigenvector and
//  classifier processes
//
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <vsl/vsl_binary_io.h>
class brad_grad_hist_feature_vector
{
 public:
  brad_grad_hist_feature_vector()= default;

  brad_grad_hist_feature_vector(float minv, float maxv, unsigned nbins)
    : min_(minv), max_(maxv), nbins_(nbins) {}

  ~brad_grad_hist_feature_vector() = default;

  //: vector of histogram probabilities computed from the input view, plus entropy
  vnl_vector<double> operator() (vil_image_view<float> const& view) const;

  //: the number of elements in the vector
  unsigned size() const { return nbins_+1;}

  //: the type name
  std::string type() const {return "brad_grad_hist_feature_vector";}

  //: accessors, setters
  unsigned nbins() const { return nbins_;}
  float min() const {return min_;}
  float max() const {return max_;}
  void set_nbins(unsigned nbins) {nbins_ = nbins;}
  void set_min(float minv) {min_ = minv;}
  void set_max(float maxv) {max_ = maxv;}
  //: print
  void print(std::ostream& os = std::cout) const {
    os << "nbins = " << nbins_ << " min = "
       << min_ << " max = " << max_ << '\n';
  }

 private:
  float min_, max_;
  unsigned nbins_;
};

void vsl_b_write(vsl_b_ostream &os, const brad_grad_hist_feature_vector& fv);

void vsl_b_read(vsl_b_istream &is, brad_grad_hist_feature_vector& fv);

void vsl_print_summary(std::ostream &os, brad_grad_hist_feature_vector& fv);


#endif // brad_grad_hist_feature_vector_h
