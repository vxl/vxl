#ifndef mcal_single_basis_cost_h
#define mcal_single_basis_cost_h


//:
// \file
// \author Tim Cootes
// \brief Base for objects which compute a cost function for one basis direction

#include <vsl/vsl_fwd.h>
#include <vcl_string.h>
#include <vcl_memory.h>
#include <vnl/vnl_vector.h>

//: Base for objects which compute a cost function for one basis direction.
// We assume that the component analysis aims to choose a set of orthogonal
// directions which minimise a cost function which can we expressed
// as a sum of terms, one per basis vector.
//
// Objects derived from this class are responsible for computing that
// component of the total cost function.
//
// Some cost functions only require the variance of the projection of the
// data onto the direction of interest (about the origin).  In that case
// the can_use_variance() should return true, and the cost_from_variance()
// function implemented.
class mcal_single_basis_cost {
private:
public:

  //: Dflt ctor
  mcal_single_basis_cost();

  //: Destructor
  virtual ~mcal_single_basis_cost();

  //: Returns true if cost can be computed from the variance, rather than requiring all the data
  virtual bool can_use_variance() const =0;

  //: Compute component of the cost function from given basis vector
  // \param: unit_basis - Unit vector defining basis direction
  // \param: projections - Projections of the dataset onto this basis vector
  virtual double cost(const vnl_vector<double>& unit_basis,
                      const vnl_vector<double>& projections) =0;

  //: Compute component of the cost function from given basis vector
  // Only relevant if can_use_variance() is true.  This allows more
  // efficient function evaluation in that case.
  // \param: unit_basis - Unit vector defining basis direction
  // \param: variance - Variance of projections of the dataset onto this basis vector
  virtual double cost_from_variance(const vnl_vector<double>& unit_basis,
                                    double variance) =0;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const = 0;

  //: Create a copy on the heap and return base class pointer
  virtual mcal_single_basis_cost* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const =0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const=0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs)=0;

  //: Read initialisation settings from a stream.
  // The default implementation merely checks that no properties have
  // been specified.
  virtual void config_from_stream(vcl_istream &);

  //: Create a concrete mcal_single_basis_cost object, from a text specification.
  static vcl_auto_ptr<mcal_single_basis_cost> new_single_basis_cost_from_stream(vcl_istream &is);
};

  //: Allows derived class to be loaded by base-class pointer
  //  A loader object exists which is invoked by calls
  //  of the form "vsl_b_read(bfs,base_ptr);".  This loads derived class
  //  objects from the disk, places them on the heap and
  //  returns a base class pointer.
  //  In order to work the loader object requires
  //  an instance of each derived class that might be
  //  found.  This function gives the model class to
  //  the appropriate loader.
void vsl_add_to_binary_loader(const mcal_single_basis_cost& b);

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mcal_single_basis_cost& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mcal_single_basis_cost& b);

  //: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mcal_single_basis_cost& b);

  //: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mcal_single_basis_cost* b);

#endif // mcal_single_basis_cost_h



