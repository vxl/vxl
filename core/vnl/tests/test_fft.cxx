/*
  fsm@robots.ox.ac.uk
*/
#include <vcl_cmath.h>
#include <vcl_iostream.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_sample.h>
#include <vnl/vnl_matlab_print.h>

#include <vnl/algo/vnl_fft_1d.h>
#include <vnl/algo/vnl_fft_2d.h>

void fill_cplx(vcl_complex<double> *b,
               vcl_complex<double> *e)
{
  for (vcl_complex<double> *p=b; p<e; ++p)
    *p = vcl_complex<double>(vnl_sample_uniform(-1, +1), vnl_sample_uniform(-1, +1));
}

void test_fft_1d(int N)
{
  vnl_vector<vcl_complex<double> > signal(N);
  fill_cplx(signal.begin(), signal.end());

  vnl_fft_1d<double> fft(N);

  vnl_vector<vcl_complex<double> > tmp = signal;
  vnl_matlab_print(vcl_cout, tmp, "tmp");

  fft.fwd_transform(tmp); tmp /= vcl_sqrt(double(N));
  vnl_matlab_print(vcl_cout, tmp, "tmp");

  fft.bwd_transform(tmp); tmp /= vcl_sqrt(double(N));
  vnl_matlab_print(vcl_cout, tmp, "tmp");

  double err = (tmp - signal).two_norm();
  vcl_cout << "err = " << err << vcl_endl;
  vnl_test_assert("test fwd-bwd", err <= 1e-10);
}

void test_fft_2d(int M, int N)
{
  vnl_matrix<vcl_complex<double> > signal(M, N);
  fill_cplx(signal.begin(), signal.end());

  vnl_fft_2d<double> fft(M, N);

  vnl_matrix<vcl_complex<double> > tmp = signal;
  vnl_matlab_print(vcl_cout, tmp, "tmp");

  fft.fwd_transform(tmp); tmp /= vcl_sqrt(double(M*N));
  vnl_matlab_print(vcl_cout, tmp, "tmp");

  fft.bwd_transform(tmp); tmp /= vcl_sqrt(double(M*N));
  vnl_matlab_print(vcl_cout, tmp, "tmp");

  double err = (tmp - signal).fro_norm();
  vcl_cout << "err = " << err << vcl_endl;
  vnl_test_assert("test fwd-bwd", err <= 1e-10);
}

void test_fft()
{
  test_fft_1d(24);
  test_fft_2d(25, 30);
}

TESTMAIN (test_fft);
