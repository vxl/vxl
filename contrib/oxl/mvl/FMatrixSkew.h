// This is oxl/mvl/FMatrixSkew.h
#ifndef FMatrixSkew_h_
#define FMatrixSkew_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//--------------------------------------------------------------
//:
// \file
// \brief Skew fundamental matrix
//
// A class to hold a Fundamental Matrix of the skew form
// which occurs with a pure translational motion.
// Some common operations e.g. generate epipolar lines,
// are inherited from the class FMatrix.
//

#include <vnl/vnl_matrix.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>

class FMatrixSkew : public FMatrix
{
 public:

  // Constructors/Initializers/Destructors----------------------------------

  FMatrixSkew();
  FMatrixSkew(const double* f_matrix);
  FMatrixSkew(const vnl_matrix<double>& f_matrix);
  ~FMatrixSkew();

  // Computations

  inline void set_rank2_using_svd();
  inline FMatrixSkew get_rank2_truncated();
  bool get_epipoles (HomgPoint2D* e1_out, HomgPoint2D* e2_out) const;
  void decompose_to_skew_rank3 (vnl_matrix<double> *skew,
                                vnl_matrix<double> *rank3) const;
  void find_nearest_perfect_match (const HomgPoint2D& in1, const HomgPoint2D& in2,
                                   HomgPoint2D *out1, HomgPoint2D *out2) const;

  // Data Access------------------------------------------------------------

  bool set (const double* f_matrix );
  inline bool set (const vnl_matrix<double>& f_matrix );
  inline bool get_rank2_flag (void) const;
  inline void set_rank2_flag (bool rank2_flag);
};

#endif // FMatrixSkew_h_
