#ifndef mipa_identity_normaliser_h_
#define mipa_identity_normaliser_h_
//:
// \file
// \author Martin Roberts
// \brief Do nowt

#include <string>
#include <vcl_compiler.h>
#include <iostream>
#include <iosfwd>
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <mipa/mipa_vector_normaliser.h>

class mbl_read_props_type;

class mipa_identity_normaliser : public mipa_vector_normaliser
{
 public:

  virtual ~mipa_identity_normaliser() {}

  //: Normalise the sample.
  virtual void normalise(vnl_vector<double>& sample) const;

  //: Name of the class
  virtual std::string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mipa_vector_normaliser* clone() const;

  //: Print class to os
  virtual void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // mipa_identity_normaliser_h_
