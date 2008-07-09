#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Progress class that outputs simple text reporting on progress
// \author Graham Vincent and Kevin de Souza
// \date 25 Feb 2005

#include "mbl_progress_text.h"


//========================================================================
// Constructor
//========================================================================
mbl_progress_text::mbl_progress_text(vcl_ostream& os/*=vcl_cout*/)
: os_(os)
{
}


//========================================================================
// Destructor
//========================================================================
mbl_progress_text::~mbl_progress_text()
{
}


//========================================================================
// Name of the class
//========================================================================
vcl_string mbl_progress_text::is_a() const
{
  return "mbl_progress_text";
}


//========================================================================
// Called when set_estimate_iterations() is called for a given identifier.
//========================================================================
void mbl_progress_text::on_set_estimated_iterations(const vcl_string& identifier,
                                                    const int /*total_iterations*/)
{
  os_ << "Starting mbl_progress_text \"" << identifier << "\"" << vcl_endl;
}


//========================================================================
// Called when set_progress() is called for a given identifier.
//========================================================================
void mbl_progress_text::on_set_progress(const vcl_string& identifier,
                                        const int progress)
{
  os_ << "Progress for mbl_progress_text \"" << identifier << "\" is " << progress
      << " (out of " << estimated_iterations(identifier) << ')'
      << vcl_endl;
}


//========================================================================
// Called when end_progress() is called for a given identifier.
//========================================================================
void mbl_progress_text::on_end_progress(const vcl_string &identifier)
{
  os_ << "Finishing mbl_progress_text \"" << identifier << "\"" << vcl_endl;
}
