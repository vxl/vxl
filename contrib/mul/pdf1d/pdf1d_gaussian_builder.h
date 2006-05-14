// This is mul/pdf1d/pdf1d_gaussian_builder.h
#ifndef pdf1d_gaussian_builder_h
#define pdf1d_gaussian_builder_h

//:
// \file
// \author Tim Cootes
// \brief Class to build pdf1d_gaussian objects

#include <pdf1d/pdf1d_builder.h>
#include <vcl_iosfwd.h>

//=======================================================================

class pdf1d_gaussian;

//: Class to build pdf1d_gaussian objects
class pdf1d_gaussian_builder : public pdf1d_builder
{
  double min_var_;

  pdf1d_gaussian& gaussian(pdf1d_pdf& model) const;
 public:

  //: Dflt ctor
  pdf1d_gaussian_builder();

  //: Destructor
  virtual ~pdf1d_gaussian_builder();

  //: Create empty model
  virtual pdf1d_pdf* new_model() const;

  //: Name of the model class returned by new_model()
  virtual vcl_string new_model_type() const;

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var);

  //: Get lower threshold on variance for built models
  virtual double min_var() const;

  //: Build gaussian from n elements in data[i]
  virtual void build_from_array(pdf1d_pdf& model, const double* data, int n) const;

  //: Build default model with given mean
  virtual void build(pdf1d_pdf& model, double mean) const;

  //: Build model from data
  virtual void build(pdf1d_pdf& model,
                     mbl_data_wrapper<double>& data) const;

  //: Build model from weighted data
  virtual void weighted_build(pdf1d_pdf& model,
                              mbl_data_wrapper<double>& data,
                              const vcl_vector<double>& wts) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_builder* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // pdf1d_gaussian_builder_h
