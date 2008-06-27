#ifndef mbl_progress_time_h_
#define mbl_progress_time_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif


//:
// \file
// \brief Progress class that outputs elapsed time reporting on progress
// \author Kevin de Souza
// \date 26 June 2008


#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vul/vul_ios_state.h>
#include <mbl/mbl_progress.h>


//========================================================================
//: \brief Progress class that outputs elapsed time reporting on progress
class mbl_progress_time : public mbl_progress
{
public:

  //: Constructor
  // \param os Ostream for text output (defaults to cout)
  mbl_progress_time(vcl_ostream& os=vcl_cout);

  //: Destructor
  ~mbl_progress_time();

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


protected:

  vcl_ostream& os_;
  vul_timer timer_;
  vul_ios_state_saver ios_state_; // Restores stream format after this progress object is destroyed
};

//========================================================================

#endif // mbl_progress_time_h_
