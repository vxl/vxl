#ifndef mcal_trivial_ca_h_
#define mcal_trivial_ca_h_
//:
// \file
// \author Tim Cootes
// \brief Generates set of axis aligned modes for non-zero elements

#include <iostream>
#include <iosfwd>
#include <mcal/mcal_component_analyzer.h>
#include <mcal/mcal_single_basis_cost.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <mbl/mbl_data_wrapper.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Generates set of axis aligned modes for non-zero element
//  Given n-D input data, for each element which contains non-zero
//  variance, generate a unit mode vector along relevant axis.
//
//  This is primarily used for testing, and trivial initialisations.
class mcal_trivial_ca : public mcal_component_analyzer
{
 private:
 public:

    //: Dflt ctor
  mcal_trivial_ca();

    //: Destructor
  ~mcal_trivial_ca() override;

  //: Compute modes of the supplied data relative to the supplied mean
  //  Model is x = mean + modes*b,  where b is a vector of weights on each mode.
  //  mode_var[i] gives the variance of the data projected onto that mode.
  void build_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                const vnl_vector<double>& mean,
                                vnl_matrix<double>& modes,
                                vnl_vector<double>& mode_var) override;

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  std::string is_a() const override;

    //: Create a copy on the heap and return base class pointer
   mcal_component_analyzer*  clone()  const override;

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
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  void config_from_stream(std::istream & is) override;
};

#endif // mcal_trivial_ca_h_
