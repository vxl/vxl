#ifdef __GNUC__
#pragma implementation
#endif

#include <mbl/mbl_progress_text.h>

  //: Constructor
mbl_progress_text::mbl_progress_text()
{}

//: Destructor
mbl_progress_text::~mbl_progress_text()
{}

  //: Name of the class
vcl_string mbl_progress_text::is_a() const
{ return "mbl_progress_text"; }


void mbl_progress_text::on_set_estimated_iterations(const vcl_string& identifier,
                                           const int total_iterations)
{ vcl_cout << "Starting " << identifier << vcl_endl; }


void mbl_progress_text::on_set_progress(const vcl_string& identifier,
                               const int progress)
{
  vcl_cout << "Progress for " << identifier << " is " << progress
           << " (out of " << estimated_iterations(identifier) << ")"
           << vcl_endl;
}


void mbl_progress_text::on_end_progress(const vcl_string &identifier)
{ vcl_cout << "Finishing " << identifier << vcl_endl; }

