// This is mul/pdf1d/pdf1d_exponential_builder.h
#ifndef pdf1d_exponential_builder_h
#define pdf1d_exponential_builder_h

//:
// \file
// \author Tim Cootes
// \brief Class to build pdf1d_exponential objects

#include <iostream>
#include <iosfwd>
#include <pdf1d/pdf1d_builder.h>
#include <vcl_compiler.h>

//=======================================================================

class pdf1d_exponential;

//: Class to build pdf1d_exponential objects
class pdf1d_exponential_builder : public pdf1d_builder
{
  double min_var_;

  pdf1d_exponential& exponential(pdf1d_pdf& model) const;
 public:

  //: Dflt ctor
  pdf1d_exponential_builder();

  //: Destructor
  virtual ~pdf1d_exponential_builder();

  //: Create empty model
  virtual pdf1d_pdf* new_model() const;

  //: Name of the model class returned by new_model()
  virtual std::string new_model_type() const;

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var);

  //: Get lower threshold on variance for built models
  virtual double min_var() const;

  //: Build exponential from n elements in data[i]
  virtual void build_from_array(pdf1d_pdf& model, const double* data, int n) const;

  //: Build default model with given mean
  virtual void build(pdf1d_pdf& model, double mean) const;

  //: Build model from data
  virtual void build(pdf1d_pdf& model,
                     mbl_data_wrapper<double>& data) const;

  //: Build model from weighted data
  virtual void weighted_build(pdf1d_pdf& model,
                              mbl_data_wrapper<double>& data,
                              const std::vector<double>& wts) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(std::string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_builder* clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // pdf1d_exponential_builder_h
