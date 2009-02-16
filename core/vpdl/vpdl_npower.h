// This is core/vpdl/vpdl_npower.h
#ifndef vpdl_npower_h_
#define vpdl_npower_h_
//:
// \file
// \author Matthew Leotta
// \date February 11, 2009
// \brief efficiently compute nth powers
//
// \verbatim
// Modifications
//   None
// \endverbatim

#include <vnl/vnl_math.h>


//: compute the nth power efficiently 
template<unsigned n>
struct vpdl_npower
{
  //: compute the nth power of v efficiently
  // v^n = (v^(n/2))^2 * (n%2 == 1)?v:1)
  static inline double fixed_power(const double& v)
  { 
    return vpdl_npower<n%2>::combine(v,vpdl_npower<n/2>::fixed_power(v)); 
  }
  
  //: run-time case, ignore the run-time parameter for n>0
  static inline double power(const double& v, unsigned d)
  { return fixed_power(v); }
};

//: compute the nth power efficiently - base case 1
VCL_DEFINE_SPECIALIZATION
struct vpdl_npower<1>
{
  //: compute the nth power of v efficiently - base case
  static inline double fixed_power(const double& v) { return v; }
  
  //: special base case function to deal with odd powers
  static inline double combine(const double& v, const double& v2) 
  { return v*v2*v2; }
  
  //: run-time case, ignore the run-time parameter for n>0
  static inline double power(const double& v, unsigned d)
  { return fixed_power(v); }
};

//: compute the nth power efficiently - base case 1
VCL_DEFINE_SPECIALIZATION
struct vpdl_npower<0>
{
  //: compute the nth power of v efficiently - base case
  static inline double fixed_power(const double& v) { return 1.0; }
  
  //: special base case function to deal with odd powers
  static inline double combine(const double& v, const double& v2) 
  { return v2*v2; }
  
  // run time recursive version
  static inline double power(const double& v, unsigned d)
  {
    unsigned d2 = d/2;
    double val = (d%2)?v:1.0;
    if(d2 > 0)
    {
      double v2 = power(v,d2);
      val *= v2*v2;
    }
    return val;
  }
};


//: precompute powers in a table
struct vpdl_npower_table
{
  //: initialize the first 8 powers of val
  vpdl_npower_table(double val);
  
  //: return the nth power of 2*pi
  // the first 8 are precomputed for common dimensions
  double operator()(unsigned int n) const;
 
private:
  //: the precomputed lookup table
  double values[8];
};

//: A static variable for precomputed powers of 2*pi
extern vpdl_npower_table vpdl_two_pi_power;

#endif // vpdl_npower_h_
