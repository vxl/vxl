#ifndef vcsl_matrix_param_h
#define vcsl_matrix_param_h

#include <vcsl/vcsl_matrix_param_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>

class vcsl_matrix_param : public vbl_ref_count {
   public :
        double xl,yl,zl;
        double omega,phi,kappa;
};

typedef vcl_vector<vcsl_matrix_param_sptr> list_of_vcsl_matrix_param_sptr;

#endif // vcsl_matrix_param_h
