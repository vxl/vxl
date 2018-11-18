//:
// \file
// \brief Progress class that does nothing.
// \author Tim Cootes
// \date 25 Feb 2005

#include "mbl_progress_null.h"

//: Constructor
mbl_progress_null::mbl_progress_null() = default;

//: Destructor
mbl_progress_null::~mbl_progress_null() = default;

//: Name of the class
std::string mbl_progress_null::is_a() const
{ return "mbl_progress_null"; }


void mbl_progress_null::on_set_estimated_iterations(const std::string& ,const int )
{
}


void mbl_progress_null::on_set_progress(const std::string&,const int )
{
}


void mbl_progress_null::on_end_progress(const std::string &)
{
}
