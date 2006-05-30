// This is mul/vpdfl/vpdfl_gaussian_builder.h
#ifndef vpdfl_gaussian_builder_h
#define vpdfl_gaussian_builder_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Builders for Multi-variate gaussian PDF with arbitrary axes.
// \author Tim Cootes
// \date 16-Oct-98
// \verbatim
//  Modifications
//   IMS   Converted to VXL 18 April 2000
// \endverbatim
//=======================================================================

#include <vpdfl/vpdfl_builder_base.h>
#include <vnl/vnl_fwd.h>
#include <vcl_iosfwd.h>

//=======================================================================

class vpdfl_gaussian;

//: Class to build vpdfl_gaussian objects
class vpdfl_gaussian_builder : public vpdfl_builder_base
{
  double min_var_;

  vpdfl_gaussian& gaussian(vpdfl_pdf_base& model) const;

 protected:

  //: Add w*v*v.transpose() to S (resizing if necessary)
  // By repeatedly calling this function with a new vector v and weight w
  // You can calculate the Sum of Squares matrix.
  void updateCovar(vnl_matrix<double>& S, const vnl_vector<double>& v, double w) const;

 public:

  //: Dflt ctor
  vpdfl_gaussian_builder();

  //: Destructor
  virtual ~vpdfl_gaussian_builder();

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

  //: Computes mean and covariance of given data
  void meanCovar(vnl_vector<double>& mean, vnl_matrix<double>& covar,
                 mbl_data_wrapper<vnl_vector<double> >& data) const;

  //: Build model from mean and covariance
  void buildFromCovar(vpdfl_gaussian& model,
                      const vnl_vector<double>& mean,
                      const vnl_matrix<double>& covar) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const&) const;

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

#endif // vpdfl_gaussian_builder_h
