#ifndef vpdfl_sampler_base_h
#define vpdfl_sampler_base_h
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author Ian Scott
// \date 19-Apr-2001
// \brief Base class for Multi-Variate random sampler classes.

#include <vsl/vsl_binary_io.h>
#include <vnl/io/vnl_io_vector.h>
#include <vcl_string.h>

//=======================================================================

class vpdfl_pdf_base;

//: Base class for Multi-Variate Random Sampler classes.
// This is really a multivariate random number generator whose
// outputs have the PDF of the relevent vpdfl_pdf_base.
// Sampler objects should only exist as long as their pdf object,
// and are not meant to be persistent.

class vpdfl_sampler_base {
protected:
  const vpdfl_pdf_base *pdf_model_;
public:

    //: Dflt ctor
  vpdfl_sampler_base();

    //: Destructor
  virtual ~vpdfl_sampler_base();

    //: PDF of which this is an instance
  const vpdfl_pdf_base& model() const;

    //: Set model for which this is an instance
  virtual void set_model(const vpdfl_pdf_base&);

    //: Draw random sample from distribution
  virtual void sample(vnl_vector<double>& x)=0;

    //: Reseeds the internal random number generator
    // To achieve quasi-random initialisation use;
    // \verbatim
    // #include <vcl_ctime.h>
    // ..
    // sampler.reseed(vcl_time(0));
    // \endverbatim
  virtual void reseed(unsigned long)=0;

  //========= methods which do not change state (const) ==========//

    //: Name of the class
  virtual vcl_string is_a() const = 0;
    //: Name of the class
  virtual bool is_a(vcl_string const& s) const;

    //: Create a copy on the heap and return base class pointer
  virtual vpdfl_sampler_base* clone() const = 0;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;
};


  //: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const vpdfl_sampler_base& b);

  //: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const vpdfl_sampler_base* b);


#endif // vpdfl_sampler_base_h
