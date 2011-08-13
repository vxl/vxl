// This is brl/bseg/bvpl/bvpl_octree/bvpl_corner_functors.h
#ifndef bvpl_corner_functors_h
#define bvpl_corner_functors_h
//:
// \file
// \brief Set of functors based on Harris measures
// \author Isabel Restrepo mir@lems.brown.edu
// \date  20-Jul-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_determinant.h>

//  Functor to threshold on Harris corner measure proposed in:
//  I. Laptev. On space-time interest points. Int. J. Computer Vision, 64(2):107-123, 2005
class bvpl_harris_laptev_functor
{
 public:
  bvpl_harris_laptev_functor(double k)
  {
    H_ = vnl_double_3x3(0.0);
    count_ = 0;
    k_ = k;
  }

  //: Apply a given operation to value \p val, depending on the dispatch character
  void apply(vnl_vector_fixed<double,10> taylor_coeff)
  {
    vnl_double_3x3 temp_H;
    for (unsigned mi = 0; mi <3; mi++)
      for (unsigned mj = 0; mj <3; mj++)
        temp_H.put(mi,mj, taylor_coeff[mi+1] * taylor_coeff[mj+1]);

    H_ = H_ + temp_H;
    count_++;
  }

  //: Returns true if the harris threshold is passed, returns by reference the value of the harris ration
  bool result( double &C)
  {
    H_ /= (double)count_;

    double trace_H = H_[0][0] + H_[1][1] + H_[2][2];
    bool pass = false;

    C = vnl_determinant(H_) - k_*(trace_H * trace_H * trace_H);

    if (C >= 1e-7)
      pass = true;

    count_ = 0;
    H_.fill(0.0);

    return pass;
  }

 protected:

  vnl_double_3x3 H_;
  unsigned count_;
  double k_;
};

#endif // bvpl_corner_functors_h
