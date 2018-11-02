// This is brl/bpro/bprb/tests/bprb_test_process.h
#ifndef bprb_test_process_h_
#define bprb_test_process_h_
//:
// \file
// \brief A test process

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_database.h>
#include <bprb/bprb_process.h>

class bprb_test_process : public bprb_process
{
 public:
  bprb_test_process();

  ~bprb_test_process() override = default;

  //: Clone the process
  bprb_process* clone() const override { return new bprb_test_process(*this); }

  //: The name of the process
  std::string name() const override { return "Process"; }

  //: Perform any initialization required by the process
  bool init() override {return true;}

  //: Execute the process
  bool execute() override;

  //: Perform any clean up or final computation
  bool finish() override {return true;}

#if 0
 protected:
  // Copy Constructor
  bprb_test_process(bprb_test_process const& other);
#endif
 private:
  //: The parameters of this process
  bprb_parameters_sptr parameters_;
  //: The database of inputs and outputs for the process
  brdb_database_sptr db_;
};

#endif // bprb_test_process_h_
