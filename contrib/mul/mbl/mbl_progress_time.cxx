//:
// \file
// \brief Progress class that outputs elapsed time reporting on progress
// \author Kevin de Souza
// \date 26 June 2008

#include <iostream>
#include <ios>
#include "mbl_progress_time.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//========================================================================
// Constructor
//========================================================================
mbl_progress_time::mbl_progress_time(std::ostream& os/*=std::cout*/)
: os_(os), ios_state_(os_)
{
  os_.precision(3);
  os_.setf(std::ios::fixed, std::ios::floatfield);
}


//========================================================================
// Destructor
//========================================================================
mbl_progress_time::~mbl_progress_time() = default;


//========================================================================
// Name of the class
//========================================================================
std::string mbl_progress_time::is_a() const
{
  return "mbl_progress_time";
}


//========================================================================
// Called when set_estimate_iterations() is called for a given identifier.
//========================================================================
void mbl_progress_time::on_set_estimated_iterations(const std::string& identifier,
                                                    const int /*total_iterations*/)
{
  os_ << "Starting mbl_progress_time \"" << identifier << "\"" << std::endl;
  timer_.mark();
}


//========================================================================
// Called when set_progress() is called for a given identifier.
//========================================================================
void mbl_progress_time::on_set_progress(const std::string& identifier,
                                        const int  /*progress*/)
{
  double tsec = timer_.real()/1000.0;
  os_ << "Elapsed time for mbl_progress_time \"" << identifier << "\": " << tsec << " s" << std::endl;
}


//========================================================================
// Called when end_progress() is called for a given identifier.
//========================================================================
void mbl_progress_time::on_end_progress(const std::string &identifier)
{
  double tsec = timer_.real()/1000.0;
  os_ << "Finishing mbl_progress_time \"" << identifier << "\" (total elapsed time: " << tsec << " s)" << std::endl;
}
