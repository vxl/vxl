#ifndef HMatrix2DEuclideanCompute_h_
#define HMatrix2DEuclideanCompute_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
//
// HMatrix2DEuclideanCompute contains a linear method to compute
// a 2D Euclidean transformation. The H returned is such that
// \f\[ x_2 \sim H x_1 \f\]
//
// \author
//     David Capel, Oxford RRG, 13 May 98
// \verbatim
// Modifications:
//     FSM 23-08-98 made modification as described in HMatrix2DAffineCompute.h
// \endverbatim

#include <mvl/HMatrix2DCompute.h>

class HomgPoint2D;

class HMatrix2DEuclideanCompute : public HMatrix2DCompute
{
public:
  HMatrix2DEuclideanCompute(void);
  ~HMatrix2DEuclideanCompute();

  // left in for capes :
  static HMatrix2D compute(PairMatchSetCorner const& matches);
  static HMatrix2D compute(PointArray const& p1, PointArray const& p2);
  int minimum_number_of_correspondences() const { return 2; }
protected:
  bool compute_p(PointArray const&, PointArray const&, HMatrix2D *);
private:
  static bool tmp_fun(PointArray const&, PointArray const&, HMatrix2D*);
};

#endif // HMatrix2DEuclideanCompute_h_
