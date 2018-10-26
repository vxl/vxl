// This is brl/bpro/bprb/bprb_null_process.h
#ifndef bprb_null_process_h_
#define bprb_null_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A process that does nothing
//
// \author
//   J.L. Mundy
//--------------------------------------------------------------------------------

#include <bprb/bprb_process.h>

class bprb_null_process : public bprb_process
{
 public:
  //: Copy Constructor (no local data)
  bprb_null_process(const bprb_null_process& other)
  : bprb_process(*static_cast<const bprb_process*>(&other)) {}

  bprb_null_process();

  ~bprb_null_process() override = default;

  //: Clone the process
  bprb_null_process* clone() const override { return new bprb_null_process(*this); }

  //: The name of the process
  std::string name() const override { return "NullProcess"; }

  //: Perform any initialization required by the process
  bool init() override { return true; }

  //: Execute the process
  bool execute() override;

  //: Perform any clean up or final computation
  bool finish() override { return true; }
};

#endif // bprb_null_process_h_
