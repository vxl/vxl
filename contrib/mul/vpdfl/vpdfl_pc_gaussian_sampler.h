#ifndef vpdfl_pc_gaussian_sampler_h
#define vpdfl_pc_gaussian_sampler_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Ian Scott
// \date 21-Jul-2000
// \brief Interface for Multi-variate principal Component gaussian PDF sampler.
// Modifications
// 23 April 2001 IMS - Ported to VXL


//=======================================================================
// inclusions
#include "vpdfl_gaussian_sampler.h"


//=======================================================================

class vpdfl_gaussian;

//: Multi-Variate Axis-Aligned principal component Gaussian PDF
class vpdfl_pc_gaussian_sampler : public vpdfl_gaussian_sampler {
private:
  //: workspace;
  vnl_vector<double> dx_;
  //: workspace;
  vnl_vector<double> b_;
public:


    //: Calculate the log probability density at position x.
  double log_p(const vnl_vector<double>& x);


    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

    //: Create a copy on the heap and return base class pointer
  virtual vpdfl_sampler_base* clone() const;
};
//=======================================================================

#endif
