#include <iomanip>
#include <iostream>
#include <utility>
#include "HMatrix1DComputeOptimize1.h"
#include "HMatrix1DComputeDesign.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vgl/vgl_homg_point_1d.h>
#include <mvl/HMatrix1D.h>

//********************************************************************************
//
//
//
//********************************************************************************


class XXX : public vnl_least_squares_function
{
  unsigned N;
  const std::vector<double> &z1,z2;
 public:
  XXX(const std::vector<double> &z1_,std::vector<double> z2_)
    : vnl_least_squares_function(3, z1_.size(), no_gradient)
    , N(z1_.size())
    , z1(z1_) , z2(std::move(z2_))
    {
      assert(N == z1.size());
      assert(N == z2.size());
      //      std::cerr << "N=" << N << std::endl;
    }
  ~XXX() override { N=0; }

  void boo(const vnl_vector<double> &x) {
    assert(x.size()==3);
    std::cerr << std::showpos << std::fixed; // <iomanip>
    double z,y;
    for (unsigned i=0;i<N;i++) {
      z=z1[i];
      y=(z+x[0])/(x[1]*z+1+x[2]);
      std::cerr << z << ' ' << y << '[' << z2[i] << ']' << std::endl;
    }
  }

  //    the matrix is   [ 1.0    x[0] ]
  //                    [ x[1] 1+x[2] ]
  void f(const vnl_vector<double>& x, vnl_vector<double>& fx) override {
    assert(x.size()==3);
    assert(fx.size()==N);
    double z,y;
    for (unsigned k=0;k<N;k++) {
      z=z1[k];
      y=(z+x[0])/(x[1]*z+1+x[2]);
      fx[k]=z2[k]-y;
    }
  }
};

static
void do_compute(const std::vector<double> &z1,const std::vector<double> &z2,HMatrix1D &M)
{
  //
  // **** minimise over the set of 2x2 matrices of the form [  1     x[0] ] ****
  // ****                                                   [ x[1] 1+x[2] ] ****
  //

  // Set up a compute object :
  XXX f(z1,z2);

  // Set up the initial guess :
  vnl_vector<double> x(3);
  x.fill(0);

  // Make a Levenberg Marquardt minimizer and attach f to it :
  vnl_levenberg_marquardt LM(f);

  // Run minimiser :
  //    f.boo(x);
  LM.minimize(x);
  //    f.boo(x);

  // convert back to matrix format.
  vnl_double_2x2 T;
  T(0,0)=1; T(0,1)=x[0];
  T(1,0)=x[1]; T(1,1)=1+x[2];
  M.set(T);
}

HMatrix1DComputeOptimize1::HMatrix1DComputeOptimize1(void) : HMatrix1DCompute() { }
HMatrix1DComputeOptimize1::~HMatrix1DComputeOptimize1() = default;

bool
HMatrix1DComputeOptimize1::compute_cool_homg(const std::vector<vgl_homg_point_1d<double> >&p1,
                                             const std::vector<vgl_homg_point_1d<double> >&p2,
                                             HMatrix1D *M)
{
  unsigned N=p1.size();
  assert(N==p2.size());
  if (N<3) return false;

  std::vector<double> z1(N,0.0),z2(N,0.0);
  HMatrix1DComputeDesign C;
  C.compute(p1,p2,M);

  // map the points in p1 under M so that we are looking for a correction near the identity :
  for (unsigned i=0;i<N;i++) {
    vgl_homg_point_1d<double> v = (*M)(p1[i]);
    if (v.w() == 0.0) return false;
    z1[i] = v.x()/v.w(); // make nonhomogeneous
    if (p2[i].w()) return false;
    z2[i] = p2[i].x()/p2[i].w(); // make nonhomogeneous
  }

  HMatrix1D K;
  do_compute(z1,z2,K);
  *M=HMatrix1D(K,*M);      // refine M using the correction K.
  return true;
}
