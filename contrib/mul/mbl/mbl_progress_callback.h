#ifndef mbl_progress_callback_h_
#define mbl_progress_callback_h_


//:
// \file
// \brief Assumes a hierarchy in the identifiers and reports a single float score to callback function.
// \author Ian Scott
// \date 04 Feb 2008


#include <mbl/mbl_progress_hierarchy.h>


//========================================================================
//: Reports a single progress fraction to a C callback function.
//
// This class assumes that each progress identifier is arranged in a hierarchy,
// and that calls to set_estimated_iterations and end_progress are correctly nested.
class mbl_progress_callback: public mbl_progress_hierarchy
{
 public:
  typedef void (*callback_ptr_t)(double progress);

  //: Pass in callback function pointer on construction.
  mbl_progress_callback (callback_ptr_t func): func_(func) {}

 protected:

  callback_ptr_t func_;

  //: Called when the progress has started, changed, or finished.
  // \param fraction Progress of all identifiers towards the end.
  // value is out of 1.0.
  void on_changed_progress(double fraction) override
  {
    (*func_)(fraction);
  }

};


#endif // mbl_progress_callback_h_
