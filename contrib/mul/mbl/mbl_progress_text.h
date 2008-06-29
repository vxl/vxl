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

#include <vcl_iostream.h>
#include <mbl/mbl_progress.h>

//========================================================================
//: Progress object that outputs simple text reporting on progress
class mbl_progress_text : public mbl_progress
{
 public:

  //: Constructor
  // \param os Ostream for text output (defaults to cout)
  mbl_progress_text(vcl_ostream& os=vcl_cout);

  //: Destructor
  ~mbl_progress_text();

  //: Name of the class
  virtual vcl_string is_a() const;


 protected:

  //: Called when set_estimate_iterations() is called for a given identifier.
  //  \param identifier The operation being monitored.
  virtual void on_set_estimated_iterations(const vcl_string& identifier,
                                           const int total_iterations);

  //: Called when set_progress() is called for a given identifier.
  //  \param identifier The operation being monitored.
  //  \param progress The new progress status.
  virtual void on_set_progress(const vcl_string& identifier,
                               const int progress);

  //: Called when end_progress() is called for a given identifier.
  //  \param identifier The operation being monitored.
  virtual void on_end_progress(const vcl_string &identifier);

  vcl_ostream& os_;
};

//========================================================================

#endif // mbl_progress_text_h_
