// This is brl/bbas/brad/brad_grad_int_feature_vector.h
#ifndef brad_grad_int_feature_vector_h
#define brad_grad_int_feature_vector_h
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
class brad_grad_int_feature_vector
{
 public:
  brad_grad_int_feature_vector()= default;

  brad_grad_int_feature_vector(float min_int, float max_int, float min_grad,
                               float max_grad, unsigned nbins)
    : min_int_(min_int), max_int_(max_int),
    min_grad_(min_grad), max_grad_(max_grad),
    nbins_(nbins) {}

  ~brad_grad_int_feature_vector() = default;

  //: vector of histogram probabilities computed from the input view, plus entropy
  vnl_vector<double> operator() (vil_image_view<float> const& view) const;

  //: the number of elements in the vector
  unsigned size() const { return 2*nbins_+2;}

  //: the type name
  std::string type() const {return "brad_grad_int_feature_vector";}

  //: accessors, setters
  unsigned nbins() const { return nbins_;}
  float min_int() const {return min_int_;}
  float max_int() const {return max_int_;}
  float min_grad() const {return min_grad_;}
  float max_grad() const {return max_grad_;}
  void set_nbins(unsigned nbins) {nbins_ = nbins;}
  void set_min_int(float min_int) {min_int_ = min_int;}
  void set_max_int(float max_int) {max_int_ = max_int;}
  void set_min_grad(float min_grad) {min_grad_ = min_grad;}
  void set_max_grad(float max_grad) {max_grad_ = max_grad;}
  //: print
  void print(std::ostream& os = std::cout) const {
    os << "nbins = " << nbins_
       << " min_int = "  << min_int_
       << " max_int = "  << max_int_
       << " min_grad = " << min_grad_
       << " max_grad = " << max_grad_
       << '\n';
  }

 private:
  float min_int_, max_int_;
  float min_grad_, max_grad_;
  unsigned nbins_;
};

void vsl_b_write(vsl_b_ostream &os, const brad_grad_int_feature_vector& fv);

void vsl_b_read(vsl_b_istream &is, brad_grad_int_feature_vector& fv);

void vsl_print_summary(std::ostream &os, brad_grad_int_feature_vector& fv);


#endif // brad_grad_int_feature_vector_h
