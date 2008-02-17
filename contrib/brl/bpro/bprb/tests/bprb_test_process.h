// This is brl/bpro/bprb/tests/bprb_test_process.h
#ifndef bprb_test_process_h_
#define bprb_test_process_h_
//:
// \file
// \brief A test process

#include <vcl_string.h>
#include <brdb/brdb_database.h>
#include <bprb/bprb_process.h>

class bprb_test_process : public bprb_process
{
 public:
  bprb_test_process();

  virtual ~bprb_test_process() {}

  //: Clone the process
  virtual bprb_process* clone() const { return new bprb_test_process(*this); }

  //: The name of the process
  virtual vcl_string name() { return "Process"; }

  //: Perform any initialization required by the process
  virtual bool init() {return true;}

  //: Execute the process
  virtual bool execute();

  //: Perform any clean up or final computation
  virtual bool finish() {return true;}

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
