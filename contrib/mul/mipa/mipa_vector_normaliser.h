#ifndef mipa_vector_normaliser_h_
#define mipa_vector_normaliser_h_

//:
// \file
// \author Martin Roberts
// \brief Base class for normalisation algorithms for image feature vectors

#include <string>
#include <iostream>
#include <memory>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>
class mbl_read_props_type;

//: Base class for normalisation algorithms for image texture vectors
//  Initial vector is processed to normalise it in some way.

class mipa_vector_normaliser
{
 public:

  virtual ~mipa_vector_normaliser() = default;

  //: Normalise the sample.
  virtual void normalise(vnl_vector<double>& sample) const=0;

  //: Normalise the sample, preserving the direction of each nfeatures-sized sub-vector.
  // The default implementation assumes that this direction would be preserved anyway.
  virtual void normalise(vnl_vector<double>& sample,
                         unsigned /*nfeatures*/) const { normalise(sample); }

  //: Name of the class
  virtual std::string is_a() const = 0;

  //: Create a copy on the heap and return base class pointer
  virtual mipa_vector_normaliser* clone() const = 0;

  //: Print class to os
  virtual void print_summary(std::ostream& /*os*/) const = 0;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& /*bfs*/) const = 0;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& /*bfs*/) = 0;

  //: Create a concrete mipa_vector_normaliser-derived object, from a text specification.
  static std::unique_ptr<mipa_vector_normaliser> new_normaliser_from_stream(std::istream &is,
                                                                         const mbl_read_props_type &extra_props);

  //: Initialise from a text stream.
  // The default implementation is for attribute-less normalisers,
  // and throws if it finds any data in the stream.
  virtual void config_from_stream(
    std::istream &is, const mbl_read_props_type &extra_props);
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
void vsl_add_to_binary_loader(const mipa_vector_normaliser& b);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mipa_vector_normaliser& b);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mipa_vector_normaliser& b);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mipa_vector_normaliser& b);

//: Stream output operator for class pointer
std::ostream& operator<<(std::ostream& os,const mipa_vector_normaliser* b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mipa_vector_normaliser& b);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const mipa_vector_normaliser* b);

#endif // mipa_vector_normaliser_h_
