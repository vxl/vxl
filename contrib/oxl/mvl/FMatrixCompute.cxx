#ifdef __GNUC__
#pragma implementation
#endif
//:
// \file

#include <mvl/FMatrix.h>
#include <mvl/FMatrixCompute.h>
#include <mvl/PairSetCorner.h>

//---------------------------------------------------------------
//
//: Construct an FMatrixCompute which points to the given list of 2D point matches.
// The list is owned by the caller, and must remain in scope at least
// as long as the FMatrixCompute.
//
FMatrixCompute::FMatrixCompute ()
{
}

//---------------------------------------------------------------
//
//: Destructor.
//
FMatrixCompute::~FMatrixCompute()
{
}

// == COMPUTATIONS ==

//-----------------------------------------------------------------------------
//: Compute fundamental matrix using given matchlist.
// This is implemented in terms of compute(FMatrix*).
// This circular dependency means that only one of these two functions need
// be implemented for both to work.
//
// It also means that calling one of these on a base FMatrixCompute will blow
// your stack, but FMatrixCompute is abstract so that ``can't happen''.

bool FMatrixCompute::compute (PairMatchSetCorner& matches, FMatrix* F_out)
{
  PairSetCorner inliers(matches);
  return compute(inliers.points1, inliers.points2, F_out);
}

//: Compute fundamental matrix using given matchlist and return an FMatrix object.
//  This is implemented in terms of compute(MatchList*, FMatrix*)

FMatrix FMatrixCompute::compute (PairMatchSetCorner& matches)
{
  FMatrix ret;
  compute(matches, &ret);
  return ret;
}

FMatrix FMatrixCompute::compute (vcl_vector<HomgPoint2D>& points1,
                                 vcl_vector<HomgPoint2D>& points2)
{
  FMatrix ret;
  compute(points1, points2, &ret);
  return ret;
}

bool FMatrixCompute::compute (vcl_vector<HomgPoint2D>& points1,
                              vcl_vector<HomgPoint2D>& points2,
                              FMatrix* out)
{
  *out = compute(points1, points2);
  return true;
}
