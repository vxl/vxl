// This is core/vnl/vnl_rotation_matrix.h
#ifndef vnl_rotation_matrix_h_
#define vnl_rotation_matrix_h_
#ifdef NOT_USED_BY_ITK_XX
#include "vnl/vnl_export.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"

//:
//  \file
//  \brief Functions to create a 3x3 rotation matrix
//
// The result is a (special) orthogonal 3x3 matrix which is a
// rotation about the axis, by an angle equal to ||axis||.
//
// \verbatim
//  Modifications:
//   12-Jan-2007 Peter Vanroose - Added vnl_matrix_fixed interface
// \endverbatim

template <class T, unsigned int n> class vnl_vector_fixed;
template <class T, unsigned int num_rows, unsigned int num_cols> class vnl_matrix_fixed;

VNL_EXPORT bool vnl_rotation_matrix(double const axis[3], double** R);
VNL_EXPORT bool vnl_rotation_matrix(double const axis[3], double* R0, double* R1, double* R2);
VNL_EXPORT bool vnl_rotation_matrix(double const axis[3], double R[3][3]);
VNL_EXPORT bool vnl_rotation_matrix(vnl_vector<double> const &axis, vnl_matrix<double>& R);
VNL_EXPORT bool vnl_rotation_matrix(vnl_vector_fixed<double,3> const& axis, vnl_matrix_fixed<double,3,3>& R);

//: Returns an orthogonal 3x3 matrix which is a rotation about the axis, by an angle equal to ||axis||.
// \relatesalso vnl_matrix_fixed
VNL_EXPORT vnl_matrix_fixed<double,3,3> vnl_rotation_matrix(vnl_vector_fixed<double,3> const& axis);

//: Returns an orthogonal 3x3 matrix which is a rotation about the axis, by an angle equal to ||axis||.
// \relatesalso vnl_matrix
VNL_EXPORT vnl_matrix<double> vnl_rotation_matrix(vnl_vector<double> const& axis);
#endif
#endif // vnl_rotation_matrix_h_
