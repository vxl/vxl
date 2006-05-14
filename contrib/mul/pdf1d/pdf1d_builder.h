// This is mul/pdf1d/pdf1d_builder.h
#ifndef pdf1d_builder_h
#define pdf1d_builder_h

//:
// \file
// \author Tim Cootes
// \brief Base for classes to build pdf1d_pdf objects.


#include <vcl_vector.h>
#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_data_wrapper.h>

//=======================================================================

class pdf1d_pdf;


//: Base for classes to build pdf1d_pdf objects
//
// Design Notes:
//
// Models are passed to builder, rather than being created
// by builder, for efficiency when used in mixture models.
// (During building they'd have to be recreated at every
// iteration, which is expensive).
class pdf1d_builder
{
 public:
  //: Dflt ctor
  pdf1d_builder();

  //: Destructor
  virtual ~pdf1d_builder();

  //: Create empty model
  virtual pdf1d_pdf* new_model() const = 0;

  //: Name of the model class returned by new_model()
  virtual vcl_string new_model_type() const =0;

  //: Define lower threshold on variance for built models
  virtual void set_min_var(double min_var) =0;

  //: Get lower threshold on variance for built models
  virtual double min_var() const =0;

  //: Build default model with given mean
  virtual void build(pdf1d_pdf& model,
                     double mean) const = 0;

  //: Build model from data
  virtual void build(pdf1d_pdf& model,
                     mbl_data_wrapper<double>& data) const = 0;

  //: Build model from data
  //  Default wraps up data and calls build(model,wrapper);
  virtual void build_from_array(pdf1d_pdf& model,
                                const double* data, int n) const;

  //: Build model from weighted data
  virtual void weighted_build(pdf1d_pdf& model,
                              mbl_data_wrapper<double>& data,
                              const vcl_vector<double>& wts) const = 0;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Does the name of the class match the argument?
  virtual bool is_class(vcl_string const& s) const;

  //: Create a copy on the heap and return base class pointer
  virtual pdf1d_builder* clone() const = 0;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs) = 0;
};


//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "bfs>>base_ptr;".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const pdf1d_builder& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const pdf1d_builder& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, pdf1d_builder& b);

//: Stream output operator for class reference
void vsl_print_summary(vcl_ostream& os,const pdf1d_builder& b);

//: Stream output operator for class pointer
void vsl_print_summary(vcl_ostream& os,const pdf1d_builder* b);

//: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_builder& b);

//: Stream output operator for class pointer
vcl_ostream& operator<<(vcl_ostream& os,const pdf1d_builder* b);

#endif // pdf1d_builder_h
