#ifndef mcal_sparse_basis_cost_h
#define mcal_sparse_basis_cost_h
//:
// \file
// \author Tim Cootes
// \brief Cost function to promote sparse basis vectors

#include <mcal/mcal_single_basis_cost.h>
#include <vnl/io/vnl_io_vector.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vcl_iosfwd.h>

//: Cost function to promote sparse basis vectors
//  Cost is log(variance) + alpha sum |e_i|, which encourages concentration
//  in non-zero elements of the basis vector
class mcal_sparse_basis_cost : public mcal_single_basis_cost
{
 private:
  //: Weighting for sparseness penalty
  double alpha_;

 public:

  //: Dflt ctor
  mcal_sparse_basis_cost();

  //: Destructor
  virtual ~mcal_sparse_basis_cost();

  //: Weighting for sparseness penalty
  double alpha() const { return alpha_; }

  //: Weighting for sparseness penalty
  void set_alpha(double a);

  //: Returns true since cost can be computed from the variance.
  virtual bool can_use_variance() const;

  //: Compute component of the cost function from given basis vector
  // \param: unit_basis - Unit vector defining basis direction
  // \param: projections - Projections of the dataset onto this basis vector
  virtual double cost(const vnl_vector<double>& unit_basis,
                      const vnl_vector<double>& projections);

  //: Compute component of the cost function from given basis vector
  // Cost is log(variance) + alpha sum |e_i|.
  // \param: unit_basis - Unit vector defining basis direction
  // \param: variance - Variance of projections of the dataset onto this basis vector
  virtual double cost_from_variance(const vnl_vector<double>& unit_basis,
                                    double variance);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual  mcal_single_basis_cost*  clone()  const;

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
  //   alpha: 1.0
  // }
  // \endverbatim
  // \throw mbl_exception_parse_error if the parse fails.
  virtual void config_from_stream(vcl_istream & is);
};

#endif // mcal_sparse_basis_cost_h
