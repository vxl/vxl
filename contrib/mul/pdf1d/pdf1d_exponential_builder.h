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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  ~pdf1d_exponential_builder() override;

  //: Create empty model
  pdf1d_pdf* new_model() const override;

  //: Name of the model class returned by new_model()
  std::string new_model_type() const override;

  //: Define lower threshold on variance for built models
  void set_min_var(double min_var) override;

  //: Get lower threshold on variance for built models
  double min_var() const override;

  //: Build exponential from n elements in data[i]
  void build_from_array(pdf1d_pdf& model, const double* data, int n) const override;

  //: Build default model with given mean
  void build(pdf1d_pdf& model, double mean) const override;

  //: Build model from data
  void build(pdf1d_pdf& model,
                     mbl_data_wrapper<double>& data) const override;

  //: Build model from weighted data
  void weighted_build(pdf1d_pdf& model,
                              mbl_data_wrapper<double>& data,
                              const std::vector<double>& wts) const override;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  pdf1d_builder* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // pdf1d_exponential_builder_h
