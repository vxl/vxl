// This is brl/bpro/bprb/bprb_process_ext.h
#ifndef bprb_process_ext_h_
#define bprb_process_ext_h_
//------------------------------------------------------------------------------
//:
// \file
// \brief The bprb process base class
//
//  An extension class to the bprb_process to modify some behaviour. bprb_process
//  assumes the input and output types are defined in the constructor and the
//  input and output array are allocated early on. This extension does not expect
//  to get the input-output types or arrays initialized.
//
// \author
//   G.D. Tunali
//
// \verbatim
//  Modifications:
// \endverbatim
//------------------------------------------------------------------------------
#include "bprb_process.h"

class bprb_process_ext : public bprb_process
{
 public:
  // Constructor
  bprb_process_ext();
  //: Clone the process
  bprb_process_ext* clone() const override = 0;

  bool set_input_types(std::vector<std::string> const& types);
  bool set_input(unsigned i, brdb_value_sptr const& value) override;
  bool set_input_data(std::vector<brdb_value_sptr> const& inputs) override;
  bool set_output_types(std::vector<std::string> const& types);
  bool set_output(unsigned i, const brdb_value_sptr& val);

 protected:
#if 0
  // Copy Constructor
  bprb_process_ext(const bprb_process_ext& other);
#endif // 0

  ~bprb_process_ext() override = default;
};

#endif // bprb_process_ext_h_
