#ifndef HMatrix2DEuclideanCompute_h_
#define HMatrix2DEuclideanCompute_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME HMatrix2DEuclideanCompute
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix2DEuclideanCompute.h
// .FILE HMatrix2DEuclideanCompute.cxx
//
// .SECTION Description:
// HMatrix2DEuclideanCompute contains a linear method to compute
// a 2D Euclidean transformation. The H returned is such that
// @{ \[ x_2 \sim H x_1 \] @}
//
// .SECTION Author
//     David Capel, Oxford RRG, 13 May 98
// .SECTION Modifications:
//     FSM 23-08-98 made modification as described in HMatrix2DAffineCompute.h

#include <mvl/HMatrix2DCompute.h>

class HomgPoint2D;

class HMatrix2DEuclideanCompute : public HMatrix2DCompute {
protected:
  bool compute_p(PointArray const&, PointArray const&, HMatrix2D *);
public:
  HMatrix2DEuclideanCompute(void);
  ~HMatrix2DEuclideanCompute();

  // left in for capes :
  static HMatrix2D compute (PairMatchSetCorner const& matches);
  static HMatrix2D compute (PointArray const& p1, PointArray const& p2);
  int minimum_number_of_correspondences() const { return 2; }
private:
  static bool tmp_fun(PointArray const&, PointArray const&, HMatrix2D*);
};

#endif // HMatrix2DEuclideanCompute_h_
