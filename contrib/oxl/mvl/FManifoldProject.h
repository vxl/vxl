// This is oxl/mvl/FManifoldProject.h
#ifndef FManifoldProject_h_
#define FManifoldProject_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Fast 2-view Hartley-Sturm
//
//    FManifoldProject is a class which allows repeated fast application of the
//    manifold projection ("Hartley-Sturm") correction to points in two views.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 22 Jan 97
//
// \verbatim
// Modifications:
//   AWF 030897 Moved to MViewBasics
//   210598 AWF Return squared error, as \sqrt(|x - p|^2 + |x' - p'|^2) is meaningless.
//   AWF Handle affine F.
//   P. Torr added in a check for multiple solutions
//     this might be necessary to flag the instance when a particular correspondence
//     might have severla possible closest points all near to each other,
//     indicating high structure variability and high curvature in the F manifold.
//     These points should be treated with care, but are interesting as
//     they are in loci of high information.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_4.h>

class FMatrix;
class HomgPoint2D;

class FManifoldProject {
public:
  FManifoldProject();
  FManifoldProject(const FMatrix& F);

  void set_F(const FMatrix& F);
  double correct(const HomgPoint2D& point1, const HomgPoint2D& point2, HomgPoint2D *, HomgPoint2D *) const;
  double correct(double   x1, double   y1, double   x2, double   y2,
		 double *ox1, double *oy1, double *ox2, double *oy2) const;
  
  vnl_double_3x3 const& get_F() const { return F_; }
  
private:
  vnl_double_3x3 F_;

  // Information to be used for each point
  vnl_double_4x4 A_;
  vnl_double_4 t_;
  vnl_double_4x4 V_;
  vnl_double_4 d_;

  bool affine_F_;

};

#endif // FManifoldProject_h_
