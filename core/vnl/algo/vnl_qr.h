// This is vxl/vnl/algo/vnl_qr.h
#ifndef vnl_qr_h_
#define vnl_qr_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//  \brief Calculate inverse of a matrix using QR
//  \author  Andrew W. Fitzgibbon, Oxford RRG
//  \date   08 Dec 96
//
// \verbatim
//  Modifications:
//  081296 AWF Temporarily abandoned as I realized my problem was symmetric...
//  080697 AWF Recovered, implemented solve().
//  200897 AWF Added determinant().
//  071097 AWF Added Q(), R().
//  Christian Stoecklin, ETH Zurich, added QtB(v)
//  31-mar-2000 fsm@robots.ox.ac.uk: templated
//  dac (Manchester) 28/03/2001: tidied up documentation
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//: Extract the Q*R decomposition of matrix M.
//  The decomposition is stored in a compact and time-efficient
// packed form, which is most easily used via the "solve" and
// "determinant" methods.

export template <class T>
class vnl_qr
{
 public:
  vnl_qr(vnl_matrix<T> const & M);
 ~vnl_qr();

  vnl_matrix<T> inverse () const;   // inverse
  vnl_matrix<T> tinverse () const;  // transpose-inverse
  vnl_matrix<T> recompose () const;

  vnl_matrix<T> solve (const vnl_matrix<T>& rhs) const;
  vnl_vector<T> solve (const vnl_vector<T>& rhs) const;

  T determinant() const;
  vnl_matrix<T>& Q();
  vnl_matrix<T>& R();
  vnl_vector<T> QtB(const vnl_vector<T>& b) const;

  void extract_q_and_r(vnl_matrix<T>* Q, vnl_matrix<T>* R);

 private:
  vnl_matrix<T> qrdc_out_;
  vnl_vector<T> qraux_;
  vnl_vector<int> jpvt_;
  vnl_matrix<T>* Q_;
  vnl_matrix<T>* R_;

  // Disallow assignment.
  vnl_qr(const vnl_qr<T> &) { }
  void operator=(const vnl_qr<T> &) { }
};

//: Compute determinant of matrix "M" using QR.
template <class T>
inline T vnl_qr_determinant(vnl_matrix<T> const& m)
{
  return vnl_qr<T>(m).determinant();
}

export template <class T>
vcl_ostream& operator<<(vcl_ostream&, vnl_qr<T> const & qr);

#endif // vnl_qr_h_
