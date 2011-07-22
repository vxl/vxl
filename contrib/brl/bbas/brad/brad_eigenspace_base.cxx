#include <brad/brad_eigenspace_base.h>
#include <brad/brad_eigenspace.h>
#include <brad/brad_hist_prob_feature_vector.h>

void vsl_b_write(vsl_b_ostream &os, const brad_eigenspace_sptr& es_ptr)
{
  if(es_ptr->feature_vector_type() == "brad_hist_prob_feature_vector"){
    vcl_string t = "brad_hist_prob_feature_vector";
    brad_eigenspace<brad_hist_prob_feature_vector>* hp =
      dynamic_cast<brad_eigenspace<brad_hist_prob_feature_vector>* >(es_ptr.ptr());
    vsl_b_write(os, t);
    vsl_b_write(os, *hp);
  }
}


void vsl_b_read(vsl_b_istream &is, brad_eigenspace_sptr& es_ptr)
{
  vcl_string t;
  vsl_b_read(is, t);
  if(t == "brad_hist_prob_feature_vector"){
    brad_eigenspace<brad_hist_prob_feature_vector> beig;
    vsl_b_read(is, beig);    
    es_ptr = new brad_eigenspace<brad_hist_prob_feature_vector>(beig);
  }
}
