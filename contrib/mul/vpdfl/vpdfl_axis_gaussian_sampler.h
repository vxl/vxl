// This is mul/vpdfl/vpdfl_axis_gaussian_sampler.h
#ifndef vpdfl_axis_gaussian_sampler_h
#define vpdfl_axis_gaussian_sampler_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Sampler class for Multi-Variate axis aligned Gaussian.

#include <vcl_string.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_random.h>
#include <vpdfl/vpdfl_sampler_base.h>
class vpdfl_axis_gaussian;

//=======================================================================

//: Samples from an axis aligned Gaussian PDF
class vpdfl_axis_gaussian_sampler :public vpdfl_sampler_base
{
 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  vpdfl_axis_gaussian_sampler();

  //: Destructor
  virtual ~vpdfl_axis_gaussian_sampler();

  //: Set model for which this is an instance
  // Error check that it is an axis gaussian.
  virtual void set_model(const vpdfl_pdf_base&);

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
  const vpdfl_axis_gaussian& axis_gaussian() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_sampler_base* clone() const;
};

#endif // vpdfl_axis_gaussian_sampler_h
