#ifndef ComputeGRIC_h
#define ComputeGRIC_h

#include <mvl/PairMatchSetCorner.h>
#include <mvl/FMatrix.h>
#include <mvl/HMatrix2D.h>
#include <vcl_vector.h>
#include <vcl_memory.h>

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
  bool degenerate() { return degenerate_; }

  // Return inliers after the computation
  vcl_vector<bool>& get_inliers() { return degenerate_ ? inliersH_ : inliersF_; }

  // Get the 4 and 7 points bases for the resulting relations
  vcl_vector<int>& get_basisF() { return basisF_; }
  vcl_vector<int>& get_basisH() { return basisH_; }

 protected:
  double std_;
  vcl_auto_ptr<FMatrix> F_;
  vcl_auto_ptr<HMatrix2D> H_;
  vcl_vector<double> residualsF_;
  vcl_vector<double> residualsH_;
  vcl_vector<bool> inliersF_;
  vcl_vector<bool> inliersH_;
  vcl_vector<int> basisF_;
  vcl_vector<int> basisH_;
  bool degenerate_;
};

#endif // ComputeGRIC_h
