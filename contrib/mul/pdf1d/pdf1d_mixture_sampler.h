#ifndef pdf1d_mixture_sampler_h_
#define pdf1d_mixture_sampler_h_
//:
// \file
// \brief Implements sampling for a mixture model (a set of individual pdfs + weights)
// \author Tim Cootes and Ian Scott

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <pdf1d/pdf1d_sampler.h>
#include <vnl/vnl_random.h>
class pdf1d_mixture;

//: Instance class for pdf1d_mixture
//  Implements calculation of prob, gradient, sampling etc
class pdf1d_mixture_sampler : public pdf1d_sampler
{
  //: workspace
  std::vector<pdf1d_sampler*> inst_;

  void init();
  void delete_stuff();

 protected:
  //: The random number generator
  vnl_random rng_;
 public:

  //: Dflt ctor
  pdf1d_mixture_sampler();

  //: Copy ctor
  pdf1d_mixture_sampler(const pdf1d_mixture_sampler&);

  //: Copy operator
  pdf1d_mixture_sampler& operator=(const pdf1d_mixture_sampler&);

  //: Destructor
  ~pdf1d_mixture_sampler() override;

  //: Set model for which this is an instance
  void set_model(const pdf1d_pdf&) override;

  const pdf1d_mixture& mixture() const;

  //: Draw random sample from distribution
  double sample() override;

  //: Reseeds the static random number generator (one per derived class)
  void reseed(unsigned long) override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_sampler* clone() const override;
};

#endif // pdf1d_mixture_sampler_h_
