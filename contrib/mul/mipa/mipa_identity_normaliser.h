#ifndef mipa_identity_normaliser_h_
#define mipa_identity_normaliser_h_
//:
// \file
// \author Martin Roberts
// \brief Do nowt

#include <string>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <mipa/mipa_vector_normaliser.h>

class mbl_read_props_type;

class mipa_identity_normaliser : public mipa_vector_normaliser
{
 public:

  ~mipa_identity_normaliser() override = default;

  //: Normalise the sample.
  void normalise(vnl_vector<double>& sample) const override;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mipa_vector_normaliser* clone() const override;

  //: Print class to os
  void print_summary(std::ostream& os) const override;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const override;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs) override;
};

#endif // mipa_identity_normaliser_h_
