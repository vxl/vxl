// This is mul/vpdfl/vpdfl_gaussian_sampler.h
#ifndef vpdfl_gaussian_sampler_h
#define vpdfl_gaussian_sampler_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Sampler class for Multi-Variate Gaussian.

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_random.h>
#include <vpdfl/vpdfl_sampler_base.h>
class vpdfl_gaussian;

//=======================================================================

//: Samples from a Gaussian PDF
class vpdfl_gaussian_sampler :public vpdfl_sampler_base
{
 protected:
  //: The random number generator
  vnl_random rng_;
  //: workspace variable
  vnl_vector<double> b_;
 public:

  //: Dflt ctor
  vpdfl_gaussian_sampler();

  //: Destructor
  virtual ~vpdfl_gaussian_sampler();

  //: Draw random sample from Gaussian distribution
  virtual void sample(vnl_vector<double>& x);

  //: Reseeds the internal random number generator
  // To achieve quasi-random initialisation use;
  // \code
  // #include <vcl_ctime.h>
  // ..
  // sampler.reseed(vcl_time(0));
  // \endcode
  virtual void reseed(unsigned long);

  //: Return a reference to the pdf model
  // This is properly cast.
  const vpdfl_gaussian& gaussian() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_sampler_base* clone() const;
};

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_gaussian_sampler& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const vpdfl_gaussian_sampler* b);

#endif // vpdfl_gaussian_sampler_h
