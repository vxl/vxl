#ifndef mcal_component_analyzer_h
#define mcal_component_analyzer_h


//:
// \file
// \author Tim Cootes
// \brief Base for objects which perform some form of linear component analysis.

#include <vsl/vsl_fwd.h>
#include <vcl_string.h>
#include <vcl_memory.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <mbl/mbl_data_wrapper.h>

//: Base for objects which perform some form of linear component analysis.
// Derived classes (such as mcal_pca) generate linear modes to span
// the data space so as to optimise certain criteria.
class mcal_component_analyzer {
private:
public:

  //: Dflt ctor
  mcal_component_analyzer();

  //: Destructor
  virtual ~mcal_component_analyzer();

  //: Compute the mean of the supplied data
  void compute_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                     vnl_vector<double>& mean);

  //: Compute mean and modes from the supplied data
  //  Model is x = mean + modes*b,  where b is a vector of weights on each mode.
  //  mode_var[i] gives the variance of the data projected onto that mode.
  //  Default computes the mean, then calls build_about_mean
  virtual void build(mbl_data_wrapper<vnl_vector<double> >& data,
                     vnl_vector<double>& mean,
                     vnl_matrix<double>& modes,
                     vnl_vector<double>& mode_var);

  //: Compute modes of the supplied data relative to the supplied mean
  //  Model is x = mean + modes*b,  where b is a vector of weights on each mode.
  //  mode_var[i] gives the variance of the data projected onto that mode.
  virtual void build_about_mean(mbl_data_wrapper<vnl_vector<double> >& data,
                                const vnl_vector<double>& mean,
                                vnl_matrix<double>& modes,
                     vnl_vector<double>& mode_var) =0;

  //: Build mean and modes from data in array data[0..n-1]
  //  Utility function - wraps data and uses build(data,mean,modes)
  virtual void build_from_array(const vnl_vector<double>* data, int n,
                                vnl_vector<double>& mean,
                                vnl_matrix<double>& modes,
                                vnl_vector<double>& mode_var);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const = 0;

  //: Create a copy on the heap and return base class pointer
  virtual mcal_component_analyzer* clone() const = 0;

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

  //: Create a concrete mcal_component_analyzer object, from a text specification.
  static vcl_auto_ptr<mcal_component_analyzer> new_component_analyzer_from_stream(vcl_istream &is);
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
void vsl_add_to_binary_loader(const mcal_component_analyzer& b);

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mcal_component_analyzer& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mcal_component_analyzer& b);

  //: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mcal_component_analyzer& b);

  //: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const mcal_component_analyzer* b);

#endif // mcal_component_analyzer_h



