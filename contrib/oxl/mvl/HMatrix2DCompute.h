#ifndef _HMatrix2DCompute_h
#define _HMatrix2DCompute_h

//--------------------------------------------------------------
//
// .NAME HMatrix2DCompute
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix2DCompute.h
// .FILE HMatrix2DCompute.cxx
//
// .SECTION Description:
// Base class of classes to generate a plane-to-plane projectivity matrix from
// a set of matched features.
//
// .SECTION Modifications:
//   08-02-98 FSM
//      1. Added virtual compute methods that actually take arguments :
//      generic estimator using points, lines or both.
//      2. Obsoleted bool compute(HMatrix2D *). So don't use it!
//      3. made arguments to compute method 'const ... &',
//      thereby potentially breaking the code of certain other people.
//

class HMatrix2D;
class PairMatchSetCorner;
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine2D.h>
#include <vcl_vector.h>

// these make the headers easier to read (in my opinion) - fsm
typedef vcl_vector<HomgPoint2D> PointArray;
typedef vcl_vector<HomgLine2D>  LineArray;

//
// definition of class :
//
class HMatrix2DCompute {
private:

protected:
  bool verbose_;

  // Derived classes should implement as many of
  // the following three methods as they can :
  virtual bool compute_p (const PointArray&,// points only.
                          const PointArray&,
                          HMatrix2D *);

  virtual bool compute_l (const LineArray&, // lines only.
                          const LineArray&,
                          HMatrix2D *);

  virtual bool compute_pl(const PointArray&,// points and lines.
                          const PointArray&,
                          const LineArray&,
                          const LineArray&,
                          HMatrix2D *);
public:
  // Constructors/Initializers/Destructors----------------------------------
  HMatrix2DCompute() : verbose_(false) { }
  virtual ~HMatrix2DCompute() { }

  //
  void verbose(bool);   // set this to true for verbose run-time information


  //
  // Compute methods :
  //
  // Some use point correspondences, some use line
  // correspondences, some use both. They are implemented
  // in terms of the compute_(p|l|pl) methods.
  //

  bool compute(const PointArray&,
               const PointArray&,
               HMatrix2D *);

  bool compute(const LineArray&,
               const LineArray&,
               HMatrix2D *);

  bool compute(const PointArray&,
               const PointArray&,
               const LineArray&,
               const LineArray&,
               HMatrix2D *);

  HMatrix2D compute(const PointArray&,
                    const PointArray&);

  HMatrix2D compute(const LineArray&,
                    const LineArray&);

  HMatrix2D compute(const PointArray&,
                    const PointArray&,
                    const LineArray&,
                    const LineArray&);

  bool compute(const PairMatchSetCorner &,
               HMatrix2D *);
  HMatrix2D compute(const PairMatchSetCorner &);
};

#endif // _HMatrix2DCompute_h
