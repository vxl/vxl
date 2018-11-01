// This is oxl/mvl/HMatrix2DSimilarityCompute.h
#ifndef HMatrix2DSimilarityCompute_h_
#define HMatrix2DSimilarityCompute_h_
//:
// \file
//
// HMatrix2DSimilarityCompute contains a linear method to compute
// a 2D similarity transformation. The H returned is such that
// \f\[ x_2 \sim H x_1 \f\]
//
// \author
//     David Capel, Oxford RRG, 13 May 98
// \verbatim
// Modifications:
//     FSM 23-08-98 made modification as described in HMatrix2DAffineCompute.h
// \endverbatim

#include <mvl/HMatrix2DCompute.h>

class HMatrix2DSimilarityCompute : public HMatrix2DCompute
{
 public:
  HMatrix2DSimilarityCompute(void);
  ~HMatrix2DSimilarityCompute() override;

  // left in for capes :
  static HMatrix2D compute(PairMatchSetCorner const& matches);
  static HMatrix2D compute(PointArray const& p1, PointArray const& p2);
  int minimum_number_of_correspondences() const override { return 2; }
 protected:
  bool compute_p(PointArray const&, PointArray const&, HMatrix2D *) override;
 private:
  static bool tmp_fun(PointArray const&, PointArray const&, HMatrix2D*);
};

#endif // HMatrix2DSimilarityCompute_h_
