//-*- c++ -*-------------------------------------------------------------------
//
// linpack_svd.cxx
// Author: F. Schaffalitzky, Oxford RRG
// Created: 17 Feb 2000
// Purpose: Define linpack SVD routines with the same name, but different
//          signatures, callable by the template code in vnl_svd<T>.

#include <vnl/vnl_complex.h> // "system header" must come first.
#include <vcl/vcl_cstdlib.h> // needed for abort()
#include "linpack_svd.h"

//--------------------------------------------------------------------------------

// fortran entry point for : vnl_complex<double>
typedef vnl_double_complex fortran_complex;
extern "C" int
zsvdc_(fortran_complex *x, const int& ldx, const int& m, const int& n,
       fortran_complex *sv,
       fortran_complex *errors,
       fortran_complex *u, const int& ldu,
       fortran_complex *v, const int& ldv,
       fortran_complex *work,
       const int& job, int *info);

int linpack_svd(vnl_double_complex *x, const int& ldx, const int& m, const int& n,
		vnl_double_complex *sv,
		vnl_double_complex *errors,
		vnl_double_complex *u, const int& ldu,
		vnl_double_complex *v, const int& ldv,
		vnl_double_complex *work,
		const int& job, int *info)
{
  return zsvdc_(x, ldx, m, n, sv, errors, u, ldu, v, ldv, work, job, info);
}

//--------------------------------------------------------------------------------

// it seems we don't have fortran code for : vnl_complex<float>
int linpack_svd(vnl_float_complex *, const int& , const int& , const int& ,
		vnl_float_complex *,
		vnl_float_complex *,
		vnl_float_complex *, const int& ,
		vnl_float_complex *, const int& ,
		vnl_float_complex *,
		const int& , int *)
{
  abort();
  return 0;
}

//--------------------------------------------------------------------------------

// fortran entry point for : double
extern "C" int
dsvdc_(double *x, const int& ldx, const int& m, const int& n,
       double *sv,
       double *errors,
       double *u, const int& ldu,
       double *v, const int& ldv,
       double *work,
       const int& job, int *info);

int linpack_svd(double *x, const int& ldx, const int& m, const int& n,
		double *sv,
		double *errors,
		double *u, const int& ldu,
		double *v, const int& ldv,
		double *work,
		const int& job, int *info)
{
  return dsvdc_(x, ldx, m, n, sv, errors, u, ldu, v, ldv, work, job, info);
}

//--------------------------------------------------------------------------------

// fortran entry point for : float
extern "C" int
ssvdc_(float *x, const int& ldx, const int& m, const int& n,
       float *sv,
       float *errors,
       float *u, const int& ldu,
       float *v, const int& ldv,
       float *work,
       const int& job, int *info);

int linpack_svd(float *x, const int& ldx, const int& m, const int& n,
		float *sv,
		float *errors,
		float *u, const int& ldu,
		float *v, const int& ldv,
		float *work,
		const int& job, int *info)
{
  return ssvdc_(x, ldx, m, n, sv, errors, u, ldu, v, ldv, work, job, info);
}

//--------------------------------------------------------------------------------
