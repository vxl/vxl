#ifndef vcsl_matrix_param_h
#define vcsl_matrix_param_h

#include <vcsl/vcsl_matrix_param_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>

class vcsl_matrix_param : public vbl_ref_count
{
 public:
  vcsl_matrix_param() : vbl_ref_count() {}
  vcsl_matrix_param(vcsl_matrix_param const& x)
    : vbl_ref_count(), xl(x.xl), yl(x.yl), zl(x.zl),
      omega(x.omega), phi(x.phi), kappa(x.kappa) {}
  double xl,yl,zl;
  double omega,phi,kappa;
};

typedef vcl_vector<vcsl_matrix_param_sptr> list_of_vcsl_matrix_param_sptr;

#endif // vcsl_matrix_param_h
