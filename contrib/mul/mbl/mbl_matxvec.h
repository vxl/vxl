// This is mul/mbl/mbl_matxvec.h
#ifndef mbl_matxvec_h
#define mbl_matxvec_h
//:
// \file
// \brief Various specialised versions of simple linear algebra operators.
// \author Tim Cootes
// \date 3-Oct-97
// Modifications
// \verbatim
// TFC    Revised version 3-Oct-97
// TFC    Added TC_MatXVec2
// NPC    Added NC_VecXMat
// IMS    started conversion to VXL 18 April 2001
// \endverbatim
//
// Some of the code in this file has been converted to use VXL,
// the rest has been #if-ed out. Only convert the other functions
// as they are needed, and after checking that a suitable function doesn't
// already exist in VXL.

#include <iostream>
#include <vector>
#include <vnl/vnl_fwd.h>

#if 0 // commented out
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
  //: Compute R = V*M
  //  R is resized to the number of rows of V * cols of M
void NC_VecXMat(const vnl_vector<double>& V,const vnl_matrix<double>& M,
                vnl_matrix<double>& R);
#endif // commented out

  //: Compute R = M*V
  //  Only use first V.size() columns of M
  //  R is resized to the number of rows of M
void mbl_matxvec_prod_mv(const vnl_matrix<double>& M,
                         const vnl_vector<double>& V,
                         vnl_vector<double>& R);

  //: Compute R = M*V
  //  Only use first V.size() columns of M
  //  Only use first R.size() rows of M
  //  R is not resized - its size determines how many rows to use
void mbl_matxvec_prod_mv_2(const vnl_matrix<double>& M,
                           const vnl_vector<double>& V,
                           vnl_vector<double>& R);

#if 0
void TC_MatXVec(const vnl_matrix<double>& M,
                const vnl_vector<double>& V,
                vnl_vector<double>& R,
                const std::vector<int>& index);
#endif // commented out

  //: Fast Compute R = V' * M = ( M.transpose() * V ).transpose()
  // Only use the first R.size() columns of M
  // R is not resized - its size determines how many columns to use
  // Ensure that V.elems() == M.rows().
void mbl_matxvec_prod_vm(const vnl_vector<double>& V,
                         const vnl_matrix<double>& M,
                         vnl_vector<double>& R);

  //: Fast Compute R += V' * M = ( M.transpose() * V ).transpose()
  // Only use the first R.size() columns of M
  // R is not resized - its size determines how many columns to use
  // Ensure that V.elems() == M.rows().
void mbl_matxvec_add_prod_vm(const vnl_vector<double>& V,
                             const vnl_matrix<double>& M,
                             vnl_vector<double>& R);

#if 0 // commented out


//: Computes MD where D is diagonal with elements d(i)
void TC_ProductMD(vnl_matrix<double>& MD, const vnl_matrix<double>& M, const vnl_vector<double>& d);

//: Computes DM where D is diagonal with elements d(i)
void TC_ProductDM(vnl_matrix<double>& DM, const vnl_matrix<double>& M, const vnl_vector<double>& d);
#endif // commented out

#endif // mbl_matxvec_h
