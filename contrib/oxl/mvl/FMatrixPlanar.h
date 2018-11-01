// This is oxl/mvl/FMatrixPlanar.h
#ifndef FMatrixPlanar_h_
#define FMatrixPlanar_h_

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
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//

#include <vnl/vnl_double_3x3.h>
#include <mvl/HomgPoint2D.h>
#include <vgl/vgl_homg_point_2d.h>
#include <mvl/FMatrix.h>

class FMatrixPlanar : public FMatrix
{
 public:
  FMatrixPlanar();
  FMatrixPlanar(const double* f_matrix);
  FMatrixPlanar(const vnl_double_3x3& f_matrix);
  ~FMatrixPlanar() override;

  // Computations-----------------------------------------------------------
  void init(const FMatrix& F);

  //: Null function since planar FMatrix has already rank 2.
  inline void set_rank2_using_svd() {}
  //: Returns current matrix which is already Rank 2.
  inline FMatrixPlanar get_rank2_truncated() { return *this; }
  void find_nearest_perfect_match (const HomgPoint2D& in1, const HomgPoint2D& in2,
                                   HomgPoint2D *out1, HomgPoint2D *out2) const;
  void find_nearest_perfect_match (vgl_homg_point_2d<double> const& in1,
                                   vgl_homg_point_2d<double> const& in2,
                                   vgl_homg_point_2d<double>& out1,
                                   vgl_homg_point_2d<double>& out2) const;

  // Data Access------------------------------------------------------------

  bool set (const double* f_matrix ) override;
  bool set (const vnl_double_3x3& f_matrix ) override;
  //: Returns the rank2 flag which is always true for FMatrixPlanar.
  inline bool get_rank2_flag (void) const { return true; }
  //: Null function since planar FMatrix has already rank 2.
  inline void set_rank2_flag (bool /*rank2_flag*/) const {}
};

#endif // FMatrixPlanar_h_
