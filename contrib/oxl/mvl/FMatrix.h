// This is oxl/mvl/FMatrix.h
#ifndef _FMatrix_h
#define _FMatrix_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief General fundamental matrix
//
// A class to hold a Fundamental Matrix of the general form and to
// perform common operations e.g. generate epipolar lines

#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <vcl_iosfwd.h>

#include <vnl/vnl_double_3x3.h>

class PMatrix;

//:
// A class to hold a Fundamental Matrix of the general form and to
// perform common operations e.g. generate epipolar lines
class FMatrix
{
 public:

  // Constructors/Initializers/Destructors----------------------------------

  FMatrix();
  FMatrix(vcl_istream& f);
  FMatrix(const double *f_matrix);
  FMatrix(const vnl_matrix<double>& f_matrix);
  FMatrix(const PMatrix& P1, const PMatrix& P2);
  FMatrix(const PMatrix& P2);
  FMatrix(const FMatrix& that) { *this = that; }
  virtual ~FMatrix();

  static FMatrix read(char const* filename);
  static FMatrix read(vcl_istream& s);

  // Operations------------------------------------------------------------

  HomgLine2D image1_epipolar_line (const HomgPoint2D& x2) const;
  HomgLine2D image2_epipolar_line (const HomgPoint2D& x1) const;

  double image1_epipolar_distance_squared (HomgPoint2D *point1_ptr, HomgPoint2D *point2_ptr) const;
  double image2_epipolar_distance_squared (HomgPoint2D *point1_ptr, HomgPoint2D *point2_ptr) const;

  // Computations------------------------------------------------------------

  void set_rank2_using_svd();
  FMatrix get_rank2_truncated();
  FMatrix transpose() const;
  bool get_epipoles (HomgPoint2D* e1_out, HomgPoint2D* e2_out) const;
  void decompose_to_skew_rank3 (vnl_matrix<double> *skew,
                                vnl_matrix<double> *rank3) const;

  void find_nearest_perfect_match (const HomgPoint2D& in1, const HomgPoint2D& in2,
                                   HomgPoint2D *out1, HomgPoint2D *out2) const;

  void find_nearest_perfect_match (const HomgPoint2D& in1, const HomgPoint2D& in2,
                                   const HomgPoint2D& e1, const HomgPoint2D& e2,
                                   HomgPoint2D *out1, HomgPoint2D *out2) const;

  // Data Access------------------------------------------------------------

  double get (unsigned int row_index, unsigned int col_index) const;

  virtual bool set (const double* f_matrix);
  void get (double *f_matrix) const;

  virtual bool set (const vnl_matrix<double>& f_matrix);
  void get (vnl_matrix<double>* f_matrix) const;

  void set(const PMatrix& P1, const PMatrix& P2);
  void set(const PMatrix& P2);
  void set(const FMatrix&);

  //: Return a const reference to the internal vnl_matrix.
  const vnl_matrix<double>& get_matrix () const { return _f_matrix; }

  // vnl_matrix<double>& get_matrix () { return _f_matrix; }

  bool get_rank2_flag (void) const;
  void set_rank2_flag (bool rank2_flag);

friend vcl_ostream& operator<<(vcl_ostream& s, const FMatrix& F);
friend vcl_istream& operator>>(vcl_istream& s, FMatrix& F);

  bool read_ascii(vcl_istream& f);

  // INTERNALS---------------------------------------------------------------

    // Data Members------------------------------------------------------------

 protected:

  // Fundamental matrix
  vnl_double_3x3 _f_matrix;

  // vnl_transpose of Fundamental matrix
  vnl_double_3x3 _ft_matrix;

  // True if the Fundamental matrix is rank 2
  // (the result of a linear computation is not normally a rank 2 matrix)
  bool _rank2_flag;
};

#endif // _FMatrix_h
