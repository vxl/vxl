#ifndef HMatrix2DCompute_h_
#define HMatrix2DCompute_h_
//:
// \file
//
// Abstract interface for classes that compute plane-to-plane
// projectivities from point and line correspondences.
//
// \verbatim
// Modifications:
//   08-02-98 FSM
//      1. Added virtual compute methods that actually take arguments :
//         generic estimator using points, lines or both.
//      2. Obsoleted bool compute(HMatrix2D *). So don't use it!
//      3. made arguments to compute method 'const ... &',
//         thereby potentially breaking the code of certain other people.
// \endverbatim

class HMatrix2D;
class PairMatchSetCorner;
#include <iostream>
#include <vector>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine2D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HMatrix2DCompute {
public:
  HMatrix2DCompute() : verbose_(false) { }
  virtual ~HMatrix2DCompute() = default;

  // set this to true for verbose run-time information
  void verbose(bool v) { verbose_ = v; }

  // fsm
  virtual int minimum_number_of_correspondences() const = 0;

  // these reduce the size of the method signatures somewhat.
  typedef std::vector<HomgPoint2D> PointArray;
  typedef std::vector<HomgLine2D>  LineArray;

  // Compute methods :
  //
  // Some use point correspondences, some use line
  // correspondences, some use both. They are implemented
  // in terms of the compute_(p|l|pl) methods.

  bool compute(PointArray const&, PointArray const&, HMatrix2D *);
  bool compute(LineArray const&, LineArray const&, HMatrix2D *);
  bool compute(PointArray const&, PointArray const&, LineArray const&, LineArray const&, HMatrix2D *);
  bool compute(PairMatchSetCorner const &, HMatrix2D *);

  HMatrix2D compute(PointArray const&, PointArray const&);
  HMatrix2D compute(LineArray const&, LineArray const&);
  HMatrix2D compute(PointArray const&, PointArray const&, LineArray const&, LineArray const&);
  HMatrix2D compute(PairMatchSetCorner const &);

protected:
  bool verbose_;

  virtual bool compute_p (PointArray const&, PointArray const&, HMatrix2D *);
  virtual bool compute_l (LineArray const&, LineArray const&, HMatrix2D *);
  virtual bool compute_pl(PointArray const&, PointArray const&, LineArray const&, LineArray const&, HMatrix2D *);
};

#endif // HMatrix2DCompute_h_
