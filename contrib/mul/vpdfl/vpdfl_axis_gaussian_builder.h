// This is mul/vpdfl/vpdfl_axis_gaussian_builder.h
#ifndef vpdfl_axis_gaussian_builder_h
#define vpdfl_axis_gaussian_builder_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes
// \date 12-Apr-2001
// \brief Class to build vpdfl_axis_gaussian objects

#include <vpdfl/vpdfl_builder_base.h>
#include <vcl_iosfwd.h>

//=======================================================================

class vpdfl_axis_gaussian;

//: Class to build vpdfl_axis_gaussian objects
class vpdfl_axis_gaussian_builder : public vpdfl_builder_base
{
  double min_var_;

  vpdfl_axis_gaussian& gaussian(vpdfl_pdf_base& model) const;
 public:

  //: Dflt ctor
  vpdfl_axis_gaussian_builder();

  //: Destructor
  virtual ~vpdfl_axis_gaussian_builder();

  //: Create empty model
  virtual vpdfl_pdf_base* new_model() const;

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var);

  //: Get lower threshold on variance for built models
  virtual double min_var() const;

  //: Build default model with given mean
  virtual void build(vpdfl_pdf_base& model,
                     const vnl_vector<double>& mean) const;

  //: Build model from data
  virtual void build(vpdfl_pdf_base& model,
                     mbl_data_wrapper<vnl_vector<double> >& data) const;

  //: Build model from weighted data
  virtual void weighted_build(vpdfl_pdf_base& model,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              const vcl_vector<double>& wts) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual vpdfl_builder_base* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);

  //: Read initialisation settings from a stream.
  // Parameters:
  // \verbatim
  // {
  //   min_var: 1.0e-6
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  virtual void config_from_stream(vcl_istream & is);

};

#endif // vpdfl_axis_gaussian_builder_h
