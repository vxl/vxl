#ifndef vpdfl_mixture_sampler_h_
#define vpdfl_mixture_sampler_h_
//=======================================================================
//
//    Copyright: (C) 2000 Victoria University of Manchester
//
//=======================================================================


//:
//  \file

//=======================================================================

#include <vpdfl/vpdfl_sampler_base.h>
#include <vpdfl/vpdfl_mixture.h>
#include <vnl/vnl_random.h>

//=======================================================================

class vpdfl_mixture;

//: Instance class for vpdfl_mixture
//  Implements calculation of prob, gradient, sampling etc
class vpdfl_mixture_sampler : public vpdfl_sampler_base
{
 private:
  //: workspace
  vcl_vector<vpdfl_sampler_base*> inst_;

  void init();
  void delete_stuff();

 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  vpdfl_mixture_sampler();

  //: Copy ctor
  vpdfl_mixture_sampler(const vpdfl_mixture_sampler&);

  //: Copy operator
  vpdfl_mixture_sampler& operator=(const vpdfl_mixture_sampler&);

  //: Destructor
  virtual ~vpdfl_mixture_sampler();

  //: Set model for which this is an instance
  virtual void set_model(const vpdfl_pdf_base&);

  const vpdfl_mixture& mixture() const;

  //: Draw random sample from distribution
  virtual void sample(vnl_vector<double>& x);

  //: Reseeds the static random number generator (one per derived class)
  virtual void reseed(unsigned long);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_sampler_base* clone() const;
};

#endif // vpdfl_mixture_sampler_h_
