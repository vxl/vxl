#ifndef FMatrixPlanar_h_
#define FMatrixPlanar_h_
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//:
// \file
// \brief planar fundamental matrix
//
// A class to hold a Fundamental Matrix of the planar form
// which occurs when the translation is confined to the plane
// perpendicular to the rotation axis.
// Some common operations e.g. generate epipolar lines,
// are inherited from the class FMatrix.
//

#include <vnl/vnl_matrix.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>

class FMatrixPlanar : public FMatrix
{
public:
  FMatrixPlanar();
  FMatrixPlanar(const double* f_matrix);
  FMatrixPlanar(const vnl_matrix<double>& f_matrix);
  ~FMatrixPlanar();

  // Computations-----------------------------------------------------------
  void init(const FMatrix& F);

  inline void set_rank2_using_svd();
  inline FMatrixPlanar get_rank2_truncated();
  void find_nearest_perfect_match (const HomgPoint2D& in1, const HomgPoint2D& in2,
                                   HomgPoint2D *out1, HomgPoint2D *out2) const;

  // Data Access------------------------------------------------------------

  bool set (const double* f_matrix );
  inline bool set (const vnl_matrix<double>& f_matrix );
  inline bool get_rank2_flag (void) const;
  inline void set_rank2_flag (bool rank2_flag) const;
};

#endif // FMatrixPlanar_h_
