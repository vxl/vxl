// This is brl/bpro/bprb/bprb_func_process.h
#ifndef bprb_func_process_h_
#define bprb_func_process_h_
//------------------------------------------------------------------------------
//:
// \file
// \brief The bmdl process class
//
//  A specialized process class for bpro library. This method receives
//  a separate function pointer for init, execute and finish method to
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
#include <iostream>
#include <bprb/bprb_process_ext.h>
#include <vcl_compiler.h>

class bprb_func_process: public bprb_process_ext
{
 public:
  bprb_func_process() {}

  bprb_func_process(bool(*fpt)(bprb_func_process&), const char* name)
  : fpt_(fpt), fpt_cons_(VXL_NULLPTR), fpt_init_(VXL_NULLPTR), fpt_finish_(VXL_NULLPTR), name_(name)
  {}

  bprb_func_process(bool(*fpt)(bprb_func_process&), const char* name,
                    bool(*cons)(bprb_func_process&),
                    bool(*init)(bprb_func_process&),
                    bool(*finish)(bprb_func_process&))
  : fpt_(fpt), fpt_cons_(cons), fpt_init_(init),
    fpt_finish_(finish), name_(name)
  { if (fpt_cons_) fpt_cons_(*this); }

  ~bprb_func_process() {}

  bprb_func_process* clone() const { return new bprb_func_process(fpt_, name_.c_str(),fpt_cons_, fpt_init_, fpt_finish_); }

  void set_init_func(bool(*fpt)(bprb_func_process&)) { fpt_init_ = fpt; }

  void set_finish_func(bool(*fpt)(bprb_func_process&)) { fpt_finish_ = fpt; }

  virtual std::string name() const { return name_; }

  template <class T>
  T get_input(unsigned i)
  {
    if (input_types_.size()>i) {
      if (!input_data_[i]) {
        std::cerr << "ERROR: input_data_[" << i << "] == NULL" << std::endl;
        return 0;
      }
      if (!(input_data_[i]->is_a()==input_types_[i])) {
        std::cerr << "Input: [" << i << "] has wrong INPUT TYPE! \n" << "Should be: " << input_types_[i] << " is: " <<input_data_[i]->is_a() << "\n";
        return 0;
      }
    }
    brdb_value_t<T>* input = static_cast<brdb_value_t<T>* >(input_data_[i].ptr());
    T val = input->value();
    return val;
  }

  template <class T>
  void set_output_val(unsigned int i, T data)
  {
    brdb_value_sptr output = new brdb_value_t<T>(data);
    set_output(i, output);
  }

  //: Execute the process
  virtual bool execute() { return fpt_(*this); }

  //: Perform any initialization required by the process
  virtual bool init() { if (fpt_init_) return fpt_init_(*this); else return false; }

  //: Perform any clean up or final computation
  virtual bool finish() { if (fpt_finish_) return fpt_finish_(*this); else return false; }

 private:
  bool (*fpt_)(bprb_func_process&);        // pointer to execute method
  bool (*fpt_cons_)(bprb_func_process&);   // pointer to cons method (like constructor)
  bool (*fpt_init_)(bprb_func_process&);   // pointer to init method
  bool (*fpt_finish_)(bprb_func_process&); // pointer to finish method
  std::string name_;
};

#endif // bprb_func_process_h_
