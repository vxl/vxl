#ifndef pdf1d_mixture_sampler_h_
#define pdf1d_mixture_sampler_h_
//:
// \file
// \brief Implements sampling for a mixture model (a set of individual pdfs + weights)
// \author Tim Cootes and Ian Scott

#include <vcl_vector.h>
#include <pdf1d/pdf1d_sampler.h>
#include <vnl/vnl_random.h>
class pdf1d_mixture;

//: Instance class for pdf1d_mixture
//  Implements calculation of prob, gradient, sampling etc
class pdf1d_mixture_sampler : public pdf1d_sampler
{
  //: workspace
  vcl_vector<pdf1d_sampler*> inst_;

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
  virtual ~pdf1d_mixture_sampler();

  //: Set model for which this is an instance
  virtual void set_model(const pdf1d_pdf&);

  const pdf1d_mixture& mixture() const;

  //: Draw random sample from distribution
  virtual double sample();

  //: Reseeds the static random number generator (one per derived class)
  virtual void reseed(unsigned long);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_sampler* clone() const;
};

#endif // pdf1d_mixture_sampler_h_
