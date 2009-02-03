#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Progress class that outputs elapsed time reporting on progress
// \author Kevin de Souza
// \date 26 June 2008

#include "mbl_progress_time.h"
#include <vcl_ios.h>


//========================================================================
// Constructor
//========================================================================
mbl_progress_time::mbl_progress_time(vcl_ostream& os/*=vcl_cout*/)
: os_(os), ios_state_(os_)
{
  os_.precision(3);
  os_.setf(vcl_ios_fixed, vcl_ios_floatfield);
}


//========================================================================
// Destructor
//========================================================================
mbl_progress_time::~mbl_progress_time()
{
}


//========================================================================
// Name of the class
//========================================================================
vcl_string mbl_progress_time::is_a() const
{
  return "mbl_progress_time";
}


//========================================================================
// Called when set_estimate_iterations() is called for a given identifier.
//========================================================================
void mbl_progress_time::on_set_estimated_iterations(const vcl_string& identifier,
                                                    const int /*total_iterations*/)
{
  os_ << "Starting mbl_progress_time \"" << identifier << "\"" << vcl_endl;
  timer_.mark();
}


//========================================================================
// Called when set_progress() is called for a given identifier.
//========================================================================
void mbl_progress_time::on_set_progress(const vcl_string& identifier,
                                        const int progress)
{
  double tsec = timer_.real()/1000.0;
  os_ << "Elapsed time for mbl_progress_time \"" << identifier << "\": " << tsec << " s" << vcl_endl;
}


//========================================================================
// Called when end_progress() is called for a given identifier.
//========================================================================
void mbl_progress_time::on_end_progress(const vcl_string &identifier)
{
  double tsec = timer_.real()/1000.0;
  os_ << "Finishing mbl_progress_time \"" << identifier << "\" (total elapsed time: " << tsec << " s)" << vcl_endl;
}
