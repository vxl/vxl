#ifndef mcal_trivial_ca_h_
#define mcal_trivial_ca_h_
//:
// \file
// \author Tim Cootes
// \brief Generates set of axis aligned modes for non-zero elements

#include <mcal/mcal_component_analyzer.h>
#include <mcal/mcal_single_basis_cost.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <mbl/mbl_data_wrapper.h>
#include <vcl_iosfwd.h>

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
  virtual ~mcal_trivial_ca();

  //: Compute modes of the supplied data relative to the supplied mean
  //  Model is x = mean + modes*b,  where b is a vector of weights on each mode.
  //  mode_var[i] gives the variance of the data projected onto that mode.
  virtual void build_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                const vnl_vector<double>& mean,
                                vnl_matrix<double>& modes,
                                vnl_vector<double>& mode_var);

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Create a copy on the heap and return base class pointer
  virtual  mcal_component_analyzer*  clone()  const;

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
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  virtual void config_from_stream(vcl_istream & is);
};

#endif // mcal_trivial_ca_h_
