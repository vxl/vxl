#ifndef vnl_qr_h_
#define vnl_qr_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_qr - Undocumented class FIXME
// .LIBRARY     vnl
// .HEADER	vnl Package
// .INCLUDE     vnl/algo/vnl_qr.h
// .FILE        vnl/algo/vnl_qr.cxx
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 08 Dec 96
//
// .SECTION Modifications:
//     Christian Stoecklin, ETH Zurich, added QtB(v);
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vnl_qr {
public:
  vnl_qr(const vnl_matrix<double>& M);
 ~vnl_qr();
  
  vnl_matrix<double> inverse () const;
  vnl_matrix<double> tinverse () const;
  vnl_matrix<double> recompose () const;

  vnl_matrix<double> solve (const vnl_matrix<double>& rhs) const;
  vnl_vector<double> solve (const vnl_vector<double>& rhs) const;

  double determinant() const;
  vnl_matrix<double>& Q();
  vnl_matrix<double>& R();
  vnl_vector<double> QtB(const vnl_vector<double>& b) const;
  
  friend ostream& operator<<(ostream&, const vnl_qr& qr);

  void extract_q_and_r(vnl_matrix<double>* Q, vnl_matrix<double>* R);

private:
  vnl_matrix<double> qrdc_out_;
  vnl_vector<double> qraux_;
  vnl_vector<int>    jpvt_;
  vnl_matrix<double>* Q_;
  vnl_matrix<double>* R_;
  
  // Disallow assignment until I decide whether its a good idea
  vnl_qr(const vnl_qr& that);
  vnl_qr& operator=(const vnl_qr& that);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_qr.

