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
  bprb_null_process(const bprb_null_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

  bprb_null_process();

  virtual ~bprb_null_process(){}

  //: Clone the process
  virtual bprb_null_process* clone() const {return new bprb_null_process(*this);}
  
    //: The name of the process
  virtual vcl_string name() {return "NullProcess";}

  //: Perform any initialization required by the process
  virtual bool init() {return true;}

    //: Execute the process 
  virtual bool execute();

  //: Perform any clean up or final computation 
  virtual bool finish() {return true;}
  
 private:
};
#endif // bprb_null_process_h_
