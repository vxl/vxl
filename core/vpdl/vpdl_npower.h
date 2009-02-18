// This is core/vpdl/vpdl_npower.h
#ifndef vpdl_npower_h_
#define vpdl_npower_h_
//:
// \file
// \author Matthew Leotta
// \date February 11, 2009
// \brief lookup table of precomputed nth powers (e.g. for 2*pi)
//
// \verbatim
// Modifications
//   None
// \endverbatim



//: precompute powers in a table
struct vpdl_npower_table
{
  //: initialize the first \a num powers of \a val
  vpdl_npower_table(double val, unsigned int num = 8);
  
  //: destructor
  ~vpdl_npower_table();
  
  //: return the nth power of the value in the table
  // the first \c num are precomputed for common dimensions
  double operator()(unsigned int n) const;
  
private:
  //: the size of the lookup table
  unsigned int num_v_;
  //: the precomputed lookup table
  double* values_;
};


//: A static variable for precomputed powers of 2*pi
extern vpdl_npower_table vpdl_two_pi_power;

#endif // vpdl_npower_h_
