#ifndef HomgConic_h_
#define HomgConic_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	HomgConic - Homogeneous 2D conic
// .LIBRARY	MViewBasics
// .HEADER	MultiView Package
// .INCLUDE	mvl/HomgConic.h
// .FILE	HomgConic.cxx
//
// .SECTION Description:
//
// @{Homogeneous representation of a 2D conic.  The conic is stored
// as a $3 \times 3$ symmetric matrix, and methods are provided for
// geometric operations and I/O.  The general form of the conic is
// \[ q({\bf x}) = {\bf x}^\top {\tt M} {\bf x} = 0 \]
// or
// \[ q(x, y) = A_{xx}x^2 + A_{xy} x y + A_{yy} y^2 + A_x x + A_y y + A_0 = 0 \]
// @}
//
// Note that this class represents only general conics and has no
// facilities for checking the conic's type etc.
// For these, use the Conic class.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 22 Oct 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>

class HomgPoint2D;
class HomgLine2D;
class Conic;

class HomgConic {
public:
  // Constructors/Destructors--------------------------------------------------

  HomgConic() {}
  HomgConic(double Axx, double Axy, double Ayy, double Ax, double Ay, double Ao);
  HomgConic(const Conic&);
  HomgConic(const vnl_matrix<double>& M): _matrix(M) {}
  // HomgConic(const HomgConic& that);- use default
  // HomgConic& operator=(const HomgConic& that); - use default
  // ~HomgConic(); - use default

  // Operations----------------------------------------------------------------
  void set(double Axx, double Axy, double Ayy, double Ax, double Ay, double Ao);
  void get(double& A, double& B, double& C, double& D, double& E, double& F);

  void set(const vnl_matrix<double>& M);
  void get(vnl_matrix<double>& M);

  vnl_double_3x3& get_matrix() { return _matrix; }
  const vnl_double_3x3& get_matrix() const { return _matrix; }

  // Computations--------------------------------------------------------------
  double F(const HomgPoint2D& p);
  // HomgLine2D GradF(const HomgPoint2D& p) { return polar(p); }
  HomgLine2D polar(const HomgPoint2D& p);
  double sampson_distance(const HomgPoint2D& p);
  double distance(const HomgPoint2D& p);
  HomgPoint2D closest_point(const HomgPoint2D& p);

  void closest_point(const HomgPoint2D& p, HomgPoint2D* pout, double* dout, vnl_vector<double>* gout = 0);
  int closest_points(const HomgPoint2D& p, HomgPoint2D pout[4]);

protected:
  // Data Members--------------------------------------------------------------
  vnl_double_3x3 _matrix;
};

#endif // HomgConic_h_
