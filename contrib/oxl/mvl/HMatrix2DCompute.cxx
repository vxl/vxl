#include <iostream>
#include <cstdlib>
#include <vector>
#include "HMatrix2DCompute.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mvl/HMatrix2D.h>
#include <mvl/PairMatchSetCorner.h>

bool
HMatrix2DCompute::compute_p(PointArray const&p1,
                            PointArray const&p2,
                            HMatrix2D * H)
{
  LineArray empty;
  return compute_pl(p1, p2, empty, empty, H);
}

bool
HMatrix2DCompute::compute_l(LineArray const&l1,
                            LineArray const&l2,
                            HMatrix2D * H)
{
  PointArray empty;
  return compute_pl(empty, empty, l1, l2, H);
}

bool
HMatrix2DCompute::compute_pl(PointArray const&,
                             PointArray const&,
                             LineArray const&,
                             LineArray const&,
                             HMatrix2D *)
{
  std::cerr << std::endl;
  std::cerr << "HMatrix2DCompute::compute_pl() :" << std::endl;
  std::cerr << "This is a virtual method which should have been" << std::endl;
  std::cerr << "overridden by a class derived from HMatrix2DCompute." << std::endl;
  std::cerr << "The derived class may have omitted to implement" << std::endl;
  std::cerr << "enough of the methods compute_p(),compute_l() and" << std::endl;
  std::cerr << "compute_pl()." << std::endl;
  std::abort();
  return false;
}

//--------------------------------------------------------------------------------
//
//  the remaining functions just call the above functions.
//
//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute(PointArray const&p1,
                          PointArray const&p2,
                          HMatrix2D * H)
{
  return compute_p(p1, p2, H);
}

HMatrix2D
HMatrix2DCompute::compute(PointArray const&p1,
                          PointArray const&p2)
{
  HMatrix2D H;
  compute_p(p1, p2, &H);
  return H;
}

//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute(LineArray const&l1,
                          LineArray const&l2,
                          HMatrix2D * H)
{
  return compute_l(l1, l2, H);
}

HMatrix2D
HMatrix2DCompute::compute(LineArray const&l1,
                          LineArray const&l2)
{
  HMatrix2D H;
  compute_l(l1, l2, &H);
  return H;
}

//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute(PointArray const&p1,
                          PointArray const&p2,
                          LineArray const&l1,
                          LineArray const&l2,
                          HMatrix2D*H)
{
  return compute_pl(p1, p2, l1, l2, H);
}

HMatrix2D
HMatrix2DCompute::compute(PointArray const&p1,
                          PointArray const&p2,
                          LineArray const&l1,
                          LineArray const &l2)
{
  HMatrix2D H;
  compute_pl(p1, p2, l1, l2, &H);
  return H;
}

//--------------------------------------------------------------------------------

bool
HMatrix2DCompute::compute(PairMatchSetCorner const &matches,
                          HMatrix2D *H)
{
  std::vector<HomgPoint2D> pts1(matches.count());
  std::vector<HomgPoint2D> pts2(matches.count());
  matches.extract_matches(pts1, pts2);
  return compute(pts1, pts2, H);
}

HMatrix2D
HMatrix2DCompute::compute(PairMatchSetCorner const &matches)
{
  HMatrix2D H;
  compute(matches, &H);
  return H;
}
