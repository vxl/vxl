#ifndef vnl_fft2d_h_
#define vnl_fft2d_h_

//:
//  \file
//  \brief Templated 2D FFT class based on Temperton FFT routines
//  \author Veit U.B. Schenk, Oxford RRG
//  \date   19 Mar 98
//
// \verbatim
//  Modifications
//  dac (Manchester) 28/03/2001: tidied up documentation
// \endverbatim

#include <vcl_complex.h>
#include <vnl/vnl_matrix.h>
#include "vnl_fftxd_prime_factors.h"

//: Templated 2D FFT class based on Temperton FFT routines
// \verbatim
//  C TEMPERTON
//  A GENERALIZED PRIME FACTOR FFT ALGORITHM FOR ANY $N = 2^P 3^Q 5^R$
//  SIAM J. SCI. STAT. COMP., MAY 1992.)
// \endverbatim
//
//  original temperton-code (fortran) converted using f2c with -C++
//  option. Two versions created: float and double.
//
//  subclassed from vnl_matrix<vnl_complex>, i.e. internally uses
//  vnl_matrix<vnl_complex> for storing the data
//
//  one super-simple constructor, all it takes as input is an vnl_matrix<float>
//  for which it will compute  the FFT.
//  For efficiency, all other constructors take as an argument two
//  PrimeFactors<T>-objects  which have to be initialised before the
//  call do doFFT. These PrimeFactors<T> objects contain the prime-factors
//  of the 'number' (the size of the array) to be FFT'd.
//  (integral part of Temperton's algorithm)

template<class T>
class vnl_fft2d : public vnl_matrix<vcl_complex<T> > {
public:
  typedef vnl_matrix<vcl_complex<T> > base;

  vnl_fft2d (vnl_matrix<T> const& real); // always forward
  vnl_fft2d (vnl_matrix<vcl_complex<T> > const& z, int dir); // both forw and backw

  //: (real,imag)-constructors
  vnl_fft2d (vnl_matrix<T> const& r, vnl_matrix<T> const& i,
             vnl_fftxd_prime_factors<T> const& oPFx,
             vnl_fftxd_prime_factors<T> const& oPFy, int dir); // vnl_matrixs r,i
  vnl_fft2d (vnl_matrix<T> const& r,
             vnl_fftxd_prime_factors<T> const& oPFx,
             vnl_fftxd_prime_factors<T> const& oPFy, int dir);  // Imag defaults to 0.0
  vnl_fft2d (const T *realdata, const T *imagdata, unsigned int iRows, unsigned int iCols,
             vnl_fftxd_prime_factors<T> const& oPFx,
             vnl_fftxd_prime_factors<T> const& oPFy, int dir);  // 'raw' r,i
  vnl_fft2d (const T *data, unsigned int iRows, unsigned int iCols,
             vnl_fftxd_prime_factors<T> const& oPFx,
             vnl_fftxd_prime_factors<T> const& oPFy, int dir);  // 'raw' r. I defaults to 0.0

#ifndef VCL_VC60
  //: complex-constructor
  vnl_fft2d (vnl_matrix<vcl_complex<T> > const& c,
             vnl_fftxd_prime_factors<T> const& oPFx,
             vnl_fftxd_prime_factors<T> const& oPFy, int dir);          //  complex vnl_matrix
  //: complex-constructor
  vnl_fft2d (const vcl_complex<T> *cdata, unsigned int iRows, unsigned int iCols,
             vnl_fftxd_prime_factors<T> const& oPFx,
             vnl_fftxd_prime_factors<T> const& oPFy, int dir);  // 'raw' complex data
#endif

  //: static member function: avoid copy-overhead
  static int doFFT_IP (vcl_complex<T> *cdata, unsigned int rows, unsigned int cols,
                       vnl_fftxd_prime_factors<T> const& oPFx,
                       vnl_fftxd_prime_factors<T> const& oPFy, int dir);

private:
  int doFFT (vnl_fftxd_prime_factors<T> const& oPFx,
             vnl_fftxd_prime_factors<T> const& oPFy, int dir);
};

#endif // vnl_fft2d_h_
