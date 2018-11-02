#ifndef ComputeGRIC_h
#define ComputeGRIC_h

#include <vector>
#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/PairMatchSetCorner.h>
#include <mvl/FMatrix.h>
#include <mvl/HMatrix2D.h>

class ComputeGRIC
{
 public:
  ComputeGRIC(double std);
  ~ComputeGRIC();

  // Computations
  // Compute the GRIC score and indicate which model
  // is the best match for the set of point matches
  // Returning a 0 for a HMatrix2D and a 1 for FMatrix
  bool compute(PairMatchSetCorner* matches);

  // Data Access
  FMatrix* get_F() const { return F_.get(); }
  HMatrix2D* get_H() const { return H_.get(); }

  // Is Degenerate ?
  bool degenerate() const { return degenerate_; }

  // Return inliers after the computation
  std::vector<bool>& get_inliers() { return degenerate_ ? inliersH_ : inliersF_; }

  // Get the 4 and 7 points bases for the resulting relations
  std::vector<int>& get_basisF() { return basisF_; }
  std::vector<int>& get_basisH() { return basisH_; }

 protected:
  double std_;
  std::unique_ptr<FMatrix> F_;
  std::unique_ptr<HMatrix2D> H_;
  std::vector<double> residualsF_;
  std::vector<double> residualsH_;
  std::vector<bool> inliersF_;
  std::vector<bool> inliersH_;
  std::vector<int> basisF_;
  std::vector<int> basisH_;
  bool degenerate_;
};

#endif // ComputeGRIC_h
