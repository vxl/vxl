//:
// \file
// \brief Progress class that outputs simple text reporting on progress
// \author Graham Vincent and Kevin de Souza
// \date 25 Feb 2005

#include "mbl_progress_text.h"


//========================================================================
// Constructor
//========================================================================
mbl_progress_text::mbl_progress_text(std::ostream& os/*=std::cout*/)
: os_(os)
{
}


//========================================================================
// Destructor
//========================================================================
mbl_progress_text::~mbl_progress_text() = default;


//========================================================================
// Name of the class
//========================================================================
std::string mbl_progress_text::is_a() const
{
  return "mbl_progress_text";
}


//========================================================================
// Called when set_estimate_iterations() is called for a given identifier.
//========================================================================
void mbl_progress_text::on_set_estimated_iterations(const std::string& identifier,
                                                    const int /*total_iterations*/)
{
  os_ << "Starting mbl_progress_text \"" << identifier << "\"" << std::endl;
}


//========================================================================
// Called when set_progress() is called for a given identifier.
//========================================================================
void mbl_progress_text::on_set_progress(const std::string& identifier,
                                        const int progress)
{
  os_ << "Progress for mbl_progress_text \"" << identifier << "\" is " << progress
      << " (out of " << estimated_iterations(identifier) << ')'
      << std::endl;
}


//========================================================================
// Called when end_progress() is called for a given identifier.
//========================================================================
void mbl_progress_text::on_end_progress(const std::string &identifier)
{
  os_ << "Finishing mbl_progress_text \"" << identifier << "\"" << std::endl;
}
