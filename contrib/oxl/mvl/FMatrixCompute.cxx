// This is oxl/mvl/FMatrixCompute.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "FMatrixCompute.h"
#include <mvl/PairMatchSetCorner.h>
#include <mvl/HomgInterestPointSet.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

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
// This circular dependency means that only one of these three functions needs
// to be implemented for all three to work.
//
// It also means that calling one of these on a base FMatrixCompute will blow
// your stack, but FMatrixCompute is abstract so that ``can't happen''.

bool FMatrixCompute::compute (PairMatchSetCorner& matches, FMatrix* F_out)
{
  // Copy matching points from matchset.
  vcl_vector<HomgPoint2D> points1(matches.count());
  vcl_vector<HomgPoint2D> points2(matches.count());
  matches.extract_matches(points1, points2);
  return compute(points1, points2, F_out);
}

bool FMatrixCompute::compute (vcl_vector<vgl_homg_point_2d<double> >& points1,
                              vcl_vector<vgl_homg_point_2d<double> >& points2,
                              FMatrix& F_out)
{
  if (points1.size() != points2.size())
    vcl_cerr << "FMatrixCompute::compute(): Point vectors are not of equal length\n";
  assert(points1.size() <= points2.size());
  HomgInterestPointSet const* p1 = new HomgInterestPointSet(points1,0);
  HomgInterestPointSet const* p2 = new HomgInterestPointSet(points2,0);
  // the two above lines cause a memory leak!
  PairMatchSetCorner matches(p1, p2);
  int count = matches.size();
  vcl_vector<bool> inliers(count, true);
  vcl_vector<int> ind1(count), ind2(count);
  for (int i = 0; i < count; i++)  ind1[i] = ind2[i] = i;
  matches.set(inliers, ind1, ind2);
  return compute(matches, &F_out);
}

bool FMatrixCompute::compute (vcl_vector<HomgPoint2D>& points1,
                              vcl_vector<HomgPoint2D>& points2,
                              FMatrix* F_out)
{
  if (points1.size() != points2.size())
    vcl_cerr << "FMatrixCompute::compute(): Point vectors are not of equal length\n";
  assert(points1.size() <= points2.size());
  HomgInterestPointSet const* p1 = new HomgInterestPointSet(points1,0);
  HomgInterestPointSet const* p2 = new HomgInterestPointSet(points2,0);
  // the two above lines cause a memory leak!
  PairMatchSetCorner matches(p1, p2);
  int count = matches.size();
  vcl_vector<bool> inliers(count, true);
  vcl_vector<int> ind1(count), ind2(count);
  for (int i = 0; i < count; i++)  ind1[i] = ind2[i] = i;
  matches.set(inliers, ind1, ind2);
  return compute(matches, F_out);
}
