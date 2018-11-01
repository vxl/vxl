// This is mul/vpdfl/vpdfl_pc_gaussian_builder.h
#ifndef vpdfl_pc_gaussian_builder_h
#define vpdfl_pc_gaussian_builder_h
//:
// \file
// \brief Interface for Multi-variate Principle Component Gaussian PDF Builder.
// \author Ian Scott
// \date 21-Jul-2000
//
// \verbatim
//  Modifications
//   23 April 2001 IMS - Ported to VXL
// \endverbatim
//=======================================================================

#include <iostream>
#include <iosfwd>
#include <vpdfl/vpdfl_gaussian_builder.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================

class vpdfl_gaussian;
class vpdfl_pdf_base;
class vpdfl_pc_gaussian;

//: Class to build vpdfl_pc_gaussian objects with a fixed number of principle components
class vpdfl_pc_gaussian_builder : public vpdfl_gaussian_builder
{
 public:
  enum partitionMethods { fixed, proportionate };
 private:

  vpdfl_pc_gaussian& gaussian(vpdfl_pdf_base& model) const;

  //: The method used to decide how to calculate the number of principle components.
  // defaults to fixed.
  partitionMethods partitionMethod_;

  //: The proportion of variance that should be encoded with the principle components..
  // Isn't used by default..
  double proportionOfVariance_;

  //: The number of components to represent in the principle space.
  unsigned fixed_partition_;

 public:
  //: Dflt ctor
  vpdfl_pc_gaussian_builder();

  //: Destructor
  ~vpdfl_pc_gaussian_builder() override;

  //: Create empty model
  vpdfl_pdf_base* new_model() const override;

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
  void mean_covar(vnl_vector<double>& mean, vnl_matrix<double>& covar,
                  mbl_data_wrapper<vnl_vector<double> >& data) const;

  //: Decide where to partition an Eigenvector space
  // Returns the number of principle components to be used.
  // Pass in the eigenvalues (eVals), the number of samples
  // that went to make up this Gaussian (nSamples), and the noise floor
  // for the dataset. The method may use simplified algorithms if
  // you indicate that the number of samples or noise floor is unknown
  // (by setting the latter parameters to 0.)
  virtual unsigned decide_partition(const vnl_vector<double>& eVals,
                                    unsigned nSamples=0, double noise=0.0) const;

  //: Return the number of principle components when using fixed partition.
  int fixed_partition() const
  {
    if (partitionMethod_ == vpdfl_pc_gaussian_builder::fixed) return fixed_partition_;
    else return -1;
  }

  //: Set the number of principle components when using fixed partition.
  void set_fixed_partition(int n_principle_components);

  //: Use proportion of variance to decide on the number of principle components.
  // Specify the proportion (between 0 and 1).
  // The default setting uses a fixed number of principle components.
  void set_proportion_partition( double proportion);

  //: Find the proportion of variance to decide on the number of principle components.
  // returns a negative value if not using proportion of variance method.
  double proportion_partition() const
  {
    if (partitionMethod_ == proportionate) return proportionOfVariance_;
    else return -1.0;
  }

  //: How is the partition between principle and complementary spaces
  partitionMethods partition_method() const
  {return partitionMethod_;}

  //: Read initialisation settings from a stream.
  // Parameters:
  // \verbatim
  // {
  //   mode_choice: fixed  // Alternative: proportionate
  //   var_prop: 0.95
  //   n_modes: 3
  //   min_var: 1.0e-6
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  void config_from_stream(std::istream & is) override;


  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  std::string is_a() const override;

  //: Does the name of the class match the argument?
  bool is_class(std::string const& s) const override;

  //: Create a copy on the heap and return base class pointer
  vpdfl_builder_base* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // vpdfl_pc_gaussian_builder_h
