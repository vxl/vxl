// This is brl/bbas/brad/brad_eigenspace_base.h
#ifndef brad_eigenspace_base_h_
#define brad_eigenspace_base_h_
//:
// \file
// \brief Abstract base class for brad_eigenspace
// \author Joseph Mundy
// \date July 2, 2011
//
// \verbatim
// \endverbatim

#include <iostream>
#include <string>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <brad/brad_eigenspace_sptr.h>
#include <brad/brad_hist_prob_feature_vector.h>
#include <brad/brad_grad_hist_feature_vector.h>
#include <brad/brad_grad_int_feature_vector.h>
class brad_eigenspace_base : public vbl_ref_count
{
 public:
  //: Default constructor
  brad_eigenspace_base() = default;

  ~brad_eigenspace_base() override = default;

  virtual std::string feature_vector_type() {return "unknown\n";}
};

void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace_sptr& /*eptr*/);

void vsl_b_read(vsl_b_istream &is, brad_eigenspace_sptr& /*eptr*/);

//:
//  This macro executes the function f that returns a bool, which if false
//  prints the string m. The generic eigenspace pointer es_ptr is specialized
//  to a templated eigenspace pointer named ep. If es_ptr cannot be cast,
//  an error message is printed and f is not called
#define CAST_CALL_EIGENSPACE(es_ptr, f, m) \
if (es_ptr->feature_vector_type()=="brad_hist_prob_feature_vector") { \
  brad_eigenspace<brad_hist_prob_feature_vector>* ep = \
    dynamic_cast<brad_eigenspace<brad_hist_prob_feature_vector>* >(es_ptr.ptr());\
  if (!f) { std::cout << m << '\n'; return false;} \
}else if (es_ptr->feature_vector_type()=="brad_grad_hist_feature_vector") { \
  brad_eigenspace<brad_grad_hist_feature_vector>* ep = \
    dynamic_cast<brad_eigenspace<brad_grad_hist_feature_vector>* >(es_ptr.ptr());\
  if (!f) { std::cout << m << '\n'; return false;} \
}else if (es_ptr->feature_vector_type() == "brad_grad_int_feature_vector") { \
  brad_eigenspace<brad_grad_int_feature_vector>* ep = \
    dynamic_cast<brad_eigenspace<brad_grad_int_feature_vector>* >(es_ptr.ptr());\
  if (!f) { std::cout << m << '\n'; return false;} \
}else{ \
  std::cout << "unknown feature vector type\n"; \
  return false; \
}

//: this macro produces a generic pointer to the specialized eigenspace defined by the feature vector type defined by string name t.
#define CAST_CREATE_EIGENSPACE(t, nbins, max_int, max_grad) \
if (t=="brad_hist_prob_feature_vector") { \
  brad_hist_prob_feature_vector func(0.0f, max_int, nbins); \
  eptr = new brad_eigenspace<brad_hist_prob_feature_vector>(nib,njb,func);\
}else if (t =="brad_grad_hist_feature_vector") { \
  brad_grad_hist_feature_vector func(0.0f, max_grad, nbins); \
  eptr = new brad_eigenspace<brad_grad_hist_feature_vector>(nib,njb,func); \
}else if (t == "brad_grad_int_feature_vector") { \
  brad_grad_int_feature_vector func(0.0f, max_int, 0.0f, max_grad, nbins); \
  eptr = new brad_eigenspace<brad_grad_int_feature_vector>(nib,njb,func); \
}else{ \
  std::cout << "unknown feature vector type\n"; \
  return false; \
}
#endif // brad_eigenspace_base_h_
