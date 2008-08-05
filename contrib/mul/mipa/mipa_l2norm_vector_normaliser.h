#ifndef mipa_l2norm_vector_normaliser_h_
#define mipa_l2norm_vector_normaliser_h_
//:
// \file
// \author Martin Roberts
// \brief L2norm v->v/(||v||_2 + epsilon)

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <mipa/mipa_vector_normaliser.h>
class mbl_read_props_type;

//: L2 norm

class mipa_l2norm_vector_normaliser : public mipa_vector_normaliser
{
 public:

  virtual ~mipa_l2norm_vector_normaliser() {}

  //: Normalise the sample.
  virtual void normalise(vnl_vector<double>& sample) const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mipa_vector_normaliser* clone() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

  //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

#endif // mipa_l2norm_vector_normaliser_h_
