#ifndef HMatrix2DSimilarityCompute_h_
#define HMatrix2DSimilarityCompute_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME HMatrix2DSimilarityCompute
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix2DSimilarityCompute.h
// .FILE HMatrix2DSimilarityCompute.cxx
//
// .SECTION Description:
// HMatrix2DSimilarityCompute contains a linear method to compute
// a 2D similarity transformation. The H returned is such that
// @{ \[ x_2 \sim H x_1 \] @}
//
// .SECTION Author
//     David Capel, Oxford RRG, 13 May 98
// .SECTION Modifications:
//     FSM 23-08-98 made modification as described in HMatrix2DAffineCompute.h

#include <mvl/HMatrix2DCompute.h>

class HomgPoint2D;

class HMatrix2DSimilarityCompute : public HMatrix2DCompute {
protected:
  bool compute_p(const vcl_vector<HomgPoint2D>&,
                 const vcl_vector<HomgPoint2D>&,
                 HMatrix2D *);
public:
  HMatrix2DSimilarityCompute(void);
  ~HMatrix2DSimilarityCompute();

  // left in for capes :
  static HMatrix2D compute (const PairMatchSetCorner &matches);
  static HMatrix2D compute (const vcl_vector<HomgPoint2D>&p1, const vcl_vector<HomgPoint2D>&p2);
  int minimum_number_of_correspondences() const { return 2; }
private:
  static bool tmp_fun(const PointArray&,
                       const PointArray&,
                       HMatrix2D*);
};

#endif // HMatrix2DSimilarityCompute_h_
