#ifndef vnl_eigensystem_h_
#define vnl_eigensystem_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_eigensystem - Unsymmetric real eigensystem.
// .LIBRARY     vnl
// .HEADER	vnl Package
// .INCLUDE     vnl/algo/vnl_eigensystem.h
// .FILE        vnl/algo/vnl_eigensystem.cxx
// .EXAMPLE     vnl/examples/vnl_eigensystem.cxx
//
// .SECTION Description
//    vnl_eigensystem is a full-bore real eigensystem.  If your matrix is symmetric,
//    it is *much* better to use vnl_symmetric_eigensystem.
//
// Author:  Andrew W. Fitzgibbon, Oxford RRG, 23 Jan 97

#include <vnl/vnl_complex.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_diag_matrix.h>

class vnl_real_eigensystem {
public:
  vnl_real_eigensystem(const vnl_matrix<double>& M);
  
public:
  vnl_matrix<double> Vreal;
  
  // -- Output matrix of eigenvectors, which will in general be complex.
  vnl_matrix<vnl_double_complex> V;
  
  // -- Output diagonal matrix of eigenvalues.
  vnl_diag_matrix<vnl_double_complex> D;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_eigensystem.

