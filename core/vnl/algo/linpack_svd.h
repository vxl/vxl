#ifndef _linpack_svd_h_
#define _linpack_svd_h_

//-----------------------------------------------------------------------------

#include <vnl/vnl_complex.h>

int linpack_svd(vnl_double_complex *x, const int& ldx, const int& m, const int& n,
		vnl_double_complex *sv,
		vnl_double_complex *errors,
		vnl_double_complex *u, const int& ldu,
		vnl_double_complex *v, const int& ldv,
		vnl_double_complex *work,
		const int& job, int *info);

// this one is not actually implemented :
int linpack_svd(vnl_float_complex *, const int& , const int& , const int& ,
		vnl_float_complex *,
		vnl_float_complex *,
		vnl_float_complex *, const int& ,
		vnl_float_complex *, const int& ,
		vnl_float_complex *,
		const int& , int *);

int linpack_svd(double *x, const int& ldx, const int& m, const int& n,
		double *sv,
		double *errors,
		double *u, const int& ldu,
		double *v, const int& ldv,
		double *work,
		const int& job, int *info);

int linpack_svd(float *x, const int& ldx, const int& m, const int& n,
		float *sv,
		float *errors,
		float *u, const int& ldu,
		float *v, const int& ldv,
		float *work,
		const int& job, int *info);

//--------------------------------------------------------------------------------
#endif
