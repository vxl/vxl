#include "brad_eigenspace_base.h"
//
#include <brad/brad_eigenspace.h>
#include <brad/brad_hist_prob_feature_vector.h>
#include <brad/brad_grad_hist_feature_vector.h>
#include <brad/brad_grad_int_feature_vector.h>

void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace_sptr& es_ptr)
{
  if (es_ptr->feature_vector_type() == "brad_hist_prob_feature_vector"){
    std::string t = "brad_hist_prob_feature_vector";
    auto* hp =
      dynamic_cast<brad_eigenspace<brad_hist_prob_feature_vector>* >(es_ptr.ptr());
    vsl_b_write(os, t);
    vsl_b_write(os, *hp);
    return;
  }
  else if (es_ptr->feature_vector_type() == "brad_grad_hist_feature_vector"){
    std::string t = "brad_grad_hist_feature_vector";
    auto* hp =
      dynamic_cast<brad_eigenspace<brad_grad_hist_feature_vector>* >(es_ptr.ptr());
    vsl_b_write(os, t);
    vsl_b_write(os, *hp);
    return;
  }
  else if (es_ptr->feature_vector_type() == "brad_grad_int_feature_vector"){
    std::string t = "brad_grad_int_feature_vector";
    auto* hp =
      dynamic_cast<brad_eigenspace<brad_grad_int_feature_vector>* >(es_ptr.ptr());
    vsl_b_write(os, t);
    vsl_b_write(os, *hp);
    return;
  }
  std::cout << "Attempt to binary write eigenspace with unknown feature vector\n";
}


void vsl_b_read(vsl_b_istream &is, brad_eigenspace_sptr& es_ptr)
{
  std::string t;
  vsl_b_read(is, t);
  if (t == "brad_hist_prob_feature_vector"){
    brad_eigenspace<brad_hist_prob_feature_vector> beig;
    vsl_b_read(is, beig);
    es_ptr = new brad_eigenspace<brad_hist_prob_feature_vector>(beig);
    return;
  }
  else if (t == "brad_grad_hist_feature_vector"){
    brad_eigenspace<brad_grad_hist_feature_vector> beig;
    vsl_b_read(is, beig);
    es_ptr = new brad_eigenspace<brad_grad_hist_feature_vector>(beig);
    return;
  }
  else if (t == "brad_grad_int_feature_vector"){
    brad_eigenspace<brad_grad_int_feature_vector> beig;
    vsl_b_read(is, beig);
    es_ptr = new brad_eigenspace<brad_grad_int_feature_vector>(beig);
    return;
  }
  std::cout << "Attempt to binary read eigenspace with unknown feature vector\n";
}
