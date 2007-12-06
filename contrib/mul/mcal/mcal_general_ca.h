#ifndef mcal_general_ca_h_
#define mcal_general_ca_h_
//:
// \file
// \author Tim Cootes
// \brief Class to perform general Component Analysis

#include <mcal/mcal_component_analyzer.h>
#include <mcal/mcal_single_basis_cost.h>
#include <mbl/mbl_cloneable_ptr.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <mbl/mbl_data_wrapper.h>
#include <vcl_iosfwd.h>

//: Class to perform general Component Analysis
//  After an initial component analysis, this rotates pairs of 
//  basis vectors so as to optimise an objective function,
//  which includes terms to do with the basis vectors themselves.
//
//  It assumes that the initial components are orthogonal.
class mcal_general_ca : public mcal_component_analyzer
{
 private:
  //: Object to perform initial component analysis (usually PCA)
  mbl_cloneable_ptr<mcal_component_analyzer> initial_ca_;

  //: Object to evaluate component of cost associated with each basis vector
  mbl_cloneable_ptr<mcal_single_basis_cost> basis_cost_;

  //: Maximum number of passes during optimisation
  unsigned max_passes_;

  //: Threshold on angular movement, used to terminate optimisation
  double move_thresh_;

  //: Optimise the mode vectors so as to minimise the cost function
  double optimise_mode_pair(vnl_vector<double>& proj1,
                                vnl_vector<double>& proj2,
                                vnl_vector<double>& mode1,
                                vnl_vector<double>& mode2);

  //: Optimise the mode vectors so as to minimise the cost function
  double optimise_one_pass(vcl_vector<vnl_vector<double> >& proj,
                                vnl_matrix<double>& modes);

  //: Optimise the mode vectors so as to minimise the cost function
  void optimise_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                const vnl_vector<double>& mean,
                                vnl_matrix<double>& modes,
                                vnl_vector<double>& mode_var);

  //: Compute projections onto each mode
  //  proj[j][i] is the projection of the i-th data sample onto the j-th mode
  void compute_projections(mbl_data_wrapper<vnl_vector<double> >& data,
                           const vnl_vector<double>& mean,
                           vnl_matrix<double>& modes,
                           vcl_vector<vnl_vector<double> >& proj);

  //: Set parameters to default values
  void set_defaults();
 public:

    //: Dflt ctor
  mcal_general_ca();

    //: Destructor
  virtual ~mcal_general_ca();

   //: Initialise, taking clones of supplied objects
  void set(const mcal_component_analyzer& initial_ca,
           const mcal_single_basis_cost& basis_cost);

  //: Object to perform initial component analysis (usually PCA)
  mcal_component_analyzer& initial_ca() { return initial_ca_; }

  //: Object to evaluate component of cost associated with each basis vector
  mcal_single_basis_cost& basis_cost() { return basis_cost_; }

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
  //   initial_ca: mcal_pca { ... }
  //   basis_cost: mcal_sparse_basis_cost { alpha: 0.1 }
  //   max_passes: 50
  //   move_thresh: 0.0001
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  virtual void config_from_stream(vcl_istream & is);
};

#endif // mcal_general_ca_h_
