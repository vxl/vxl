#ifndef mbl_progress_text_h_
#define mbl_progress_text_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Progress class that outputs simple text reporting on progress
// \author Graham Vincent and Kevin de Souza
// \date 25 Feb 2005

#include <mbl/mbl_progress.h>

//========================================================================
//: Progress object that outputs simple text reporting on progress
class mbl_progress_text : public mbl_progress
{
 public:

  //: Constructor
  mbl_progress_text();

  //: Destructor
  ~mbl_progress_text();

  //: Name of the class
  virtual vcl_string is_a() const;

 protected:
  virtual void on_set_estimated_iterations(const vcl_string& identifier,
                                           const int total_iterations);

  virtual void on_set_progress(const vcl_string& identifier,
                               const int progress);

  virtual void on_end_progress(const vcl_string &identifier);
};

//========================================================================

#endif // mbl_progress_text_h_
