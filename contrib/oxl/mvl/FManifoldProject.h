//-*- c++ -*-------------------------------------------------------------------
#ifndef FManifoldProject_h_
#define FManifoldProject_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : FManifoldProject
//
// .SECTION Description
//    FManifoldProject is a class which allows repeated fast application of the
//    manifold projection ("Hartley-Sturm") correction to points in two views.
//
// .NAME        FManifoldProject - Fast 2-view Hartley-Sturm.
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/FManifoldProject.h
// .FILE        FManifoldProject.h
// .FILE        FManifoldProject.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 22 Jan 97
//
// .SECTION Modifications:
//     AWF 030897 Moved to MViewBasics
//
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

private:
  vnl_double_3x3 F_;

  // Information to be used for each point
  vnl_double_4x4 A_;
  vnl_double_4 t_;
  vnl_double_4x4 V_;
  vnl_double_4 d_;

  bool _affine_F;

  // Workspace that we don't want to repeatedly malloc.
  vnl_vector<double> _coeffs;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS FManifoldProject.

