// This is mul/vpdfl/vpdfl_gaussian_builder.h
#ifndef vpdfl_gaussian_builder_h
#define vpdfl_gaussian_builder_h
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

#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_builder_base.h>
#include <vnl/vnl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  ~vpdfl_gaussian_builder() override;

  //: Create empty model
  vpdfl_pdf_base* new_model() const override;

  //: Define lower threshold on variance for built models
  void set_min_var(double min_var) override;

  //: Get lower threshold on variance for built models
  double min_var() const override;

  //: Build default model with given mean
  void build(vpdfl_pdf_base& model,
                     const vnl_vector<double>& mean) const override;

  //: Build model from data
  void build(vpdfl_pdf_base& model,
                     mbl_data_wrapper<vnl_vector<double> >& data) const override;

  //: Build model from weighted data
  void weighted_build(vpdfl_pdf_base& model,
                              mbl_data_wrapper<vnl_vector<double> >& data,
                              const std::vector<double>& wts) const override;

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
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const&) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_builder_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;

  //: Read initialisation settings from a stream.
  // Parameters:
  // \verbatim
  // {
  //   min_var: 1.0e-6
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  void config_from_stream(std::istream & is) override;

};

#endif // vpdfl_gaussian_builder_h
