// This is core/vnl/vnl_trace.h
#ifndef vnl_trace_h_
#define vnl_trace_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//  \brief Calculate trace of a matrix
//  \author fsm
//
// \verbatim
//  Modifications
//   LSB (Manchester) 19/3/01 Documentation tidied
//   Peter Vanroose   27-Jun-2003  made inline and added trace(matrix_fixed)
// \endverbatim

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include "vnl/vnl_export.h"

//: Calculate trace of a matrix
// \relatesalso vnl_matrix
template <class T> VNL_TEMPLATE_EXPORT
T vnl_trace(vnl_matrix<T> const& M)
{
  T sum(0);
  const vxl::indexsize_t N = M.rows()<M.cols() ? M.rows() : M.cols();
  for (vxl::indexsize_t i=0; i<N; ++i)
    sum += M(i, i);
  return sum;
}

//: Calculate trace of a matrix
// \relatesalso vnl_matrix_fixed
template <class T, vxl::indexsize_t N1, vxl::indexsize_t N2> VNL_TEMPLATE_EXPORT
T vnl_trace(vnl_matrix_fixed<T,N1,N2> const& M)
{
  T sum(0);
  for (vxl::indexsize_t i=0; i<N1 && i<N2; ++i)
    sum += M(i, i);
  return sum;
}

#endif // vnl_trace_h_
