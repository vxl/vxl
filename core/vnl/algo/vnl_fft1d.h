#ifndef vnl_fft1d_h_
#define vnl_fft1d_h_

//:
//  \file
//  \brief Templated 1D FFT class based on Temperton FFT routines
//  \author Veit U.B. Schenk, Oxford RRG
//  \date   19 Mar 98

#include <vcl_complex.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_fftxd_prime_factors.h>

//: Templated 1D FFT class based on Temperton FFT routines
//  C TEMPERTON \n
//  A GENERALIZED PRIME FACTOR FFT ALGORITHM FOR ANY $N = 2^P 3^Q 5^R$ \n
//  SIAM J. SCI. STAT. COMP., MAY 1992.)
//
//  original temperton-code (fortran) converted using f2c with -C++
//  option. Two versions created: float and double.
//
//  subclassed from vnl_vector<vcl_complex<T> >, i.e. internally uses
//  vnl_vector<vcl_complex<T> > for storing the data
//
//  two super-simple constructors, first takes as input is an vnl_vector<T>
//  for which it will compute  the FFT. Second takes vnl_complex_vector_t<T>
//   and direction. Calculates FFT in that direction.
//  For efficiency, all other constructors take as an argument two
//  PrimeFactors<T>-objects  which have to be initialised before the
//  call do doFFT. These PrimeFactors<T> objects contain the prime-factors
//  of the 'number' (the size of the array) to be FFT'd.
//  (integral part of Temperton's algorithm)

template<class T>
class vnl_fft1d : public vnl_vector<vcl_complex<T> > {
  typedef vnl_vector<vcl_complex<T> > base;
public:
  //: real-constructors
  vnl_fft1d (vnl_vector<T> const& r); // only real data, always forward
  vnl_fft1d (vnl_vector<vcl_complex<T> > const& z, int dir); // forw & backw

  //: (real,imag)-constructors
  vnl_fft1d (vnl_vector<T> const& r, vnl_vector<T> const& i,
             vnl_fftxd_prime_factors<T> const& oPF, int dir); // vnl_vectors r,i
  vnl_fft1d (vnl_vector<T> const& r,
             vnl_fftxd_prime_factors<T> const& oPF, int dir);  // Imag defaults to 0.0
  vnl_fft1d (const T *realdata, const T *imagdata, unsigned len,
             vnl_fftxd_prime_factors<T> const&, int dir);  // 'raw' r,i
  vnl_fft1d (const T *data, unsigned len,
             vnl_fftxd_prime_factors<T> const&, int dir);  // 'raw' r. I defaults to 0.0

  //: complex-constructor
#ifndef VCL_VC
  vnl_fft1d (vnl_vector<vcl_complex<T> > const& c,
      vnl_fftxd_prime_factors<T> const& , int dir);   //  complex vnl_vector
#endif
  //: complex-constructor
  vnl_fft1d (const vcl_complex<T> *cdata, unsigned len,
      vnl_fftxd_prime_factors<T> const& , int dir);  // 'raw' complex data

  //: static member function: avoid copy-overhead
  static int doFFT_IP (vcl_complex<T> *cdata, unsigned len,
                       vnl_fftxd_prime_factors<T> const& , int dir);
private:
  int doFFT (vnl_fftxd_prime_factors<T> const& , int dir);
};

#endif // vnl_fft1d_h_
