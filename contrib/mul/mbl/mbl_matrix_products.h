#ifndef mbl_matrix_product_h
#define mbl_matrix_product_h
#ifdef __GNUC__
#pragma interface
#endif


//:
// \file
// \author Tim Cootes
// \date 25-Apr-2001
// \brief Various specialised versions of matrix product operations

#include <vnl/vnl_fwd.h>

//: Compute product AB = A * B
void mbl_matrix_product(vnl_matrix<double>& AB, const vnl_matrix<double>& A,
                        const vnl_matrix<double>& B);

//: Compute product ABt = A * B.transpose()
void mbl_matrix_product_a_bt(vnl_matrix<double>& ABt, const vnl_matrix<double>& A,
                   const vnl_matrix<double>& B);

//: Compute product ABt = A * B.transpose()
void mbl_matrix_product_at_b(vnl_matrix<double>& AtB, const vnl_matrix<double>& A,
                   const vnl_matrix<double>& B);

//: ADB = A * D * B where D is diagonal with elements d
// A, d, and B should be compatible sizes, ADB will
// be resized to fit.
void mbl_matrix_product_adb(vnl_matrix<double>& ADB,
                             const vnl_matrix<double>& A,
                             const vnl_vector<double>& d,
                             const vnl_matrix<double>& B);

#endif // mbl_matrix_product_h
