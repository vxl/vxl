#ifndef vpdfl_mixture_sampler_h_
#define vpdfl_mixture_sampler_h_
//=======================================================================
//:
//  \file
//
//    Copyright: (C) 2000 Victoria University of Manchester
//
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
  std::vector<vpdfl_sampler_base*> inst_;

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
  ~vpdfl_mixture_sampler() override;

  //: Set model for which this is an instance
  void set_model(const vpdfl_pdf_base&) override;

  const vpdfl_mixture& mixture() const;

  //: Draw random sample from distribution
  void sample(vnl_vector<double>& x) override;

  //: Reseeds the static random number generator (one per derived class)
  void reseed(unsigned long) override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_sampler_base* clone() const override;
};

#endif // vpdfl_mixture_sampler_h_
