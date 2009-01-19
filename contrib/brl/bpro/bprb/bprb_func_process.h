// This is brl/bpro/bprb/bprb_func_process.h
#ifndef bprb_func_process_h_
#define bprb_func_process_h_
//------------------------------------------------------------------------------
//:
// \file
// \brief The bmdl process class
//
//  A specialized process class for bpro library. This method receives 
//  a seperate function pointer for init, execute and finish method to 
//  execute 
//  
// \author
//   Gamze D. Tunali
//
// \verbatim
//  Modifications:
//   Gamze D. Tunali    November 20, 2008  Initial version.
// \endverbatim
//------------------------------------------------------------------------------
#include <bprb/bprb_process_ext.h>

class bprb_func_process: public bprb_process_ext
{
public:
  bprb_func_process(){};

  //bprb_func_process(bool(*fpt)(vcl_vector<brdb_value_sptr>, vcl_vector<brdb_value_sptr>&), const char* name) 
  bprb_func_process(bool(*fpt)(bprb_func_process&), const char* name) 
    :fpt_(fpt), fpt_init_(0), fpt_finish_(0), name_(name)
  {}

  ~bprb_func_process(){}

  bprb_func_process* clone() const { return new bprb_func_process(fpt_, name_.c_str()); }

  void set_init_func(bool(*fpt)(bprb_func_process&)) { fpt_init_ = fpt; }

  void set_finish_func(bool(*fpt)(bprb_func_process&)) { fpt_finish_ = fpt; }

  vcl_string name() { return name_; }

  template <class T>
  T get_input(unsigned i);

  //: Execute the process
  bool execute();

  //: Perform any initialization required by the process
  bool init(){ if (fpt_init_) return fpt_init_(*this); else return false;}

  //: Perform any clean up or final computation
  bool finish(){ if (fpt_finish_) return fpt_finish_(*this); else return false;}


private:
  bool (*fpt_)(bprb_func_process&);        // pointer to execute method
  bool (*fpt_init_)(bprb_func_process&);   // pointer to init method
  bool (*fpt_finish_)(bprb_func_process&); // pointer to finish method
  vcl_string name_;
};

#endif