#ifdef __GNUC__
#pragma implementation
#endif

#include <mbl/mbl_progress.h>
#include <vcl_iostream.h>
#include <vsl/vsl_indent.h>


//=======================================================================
mbl_progress::mbl_progress()
  : throw_exception_on_cancel_(false)
{
}


//=======================================================================
mbl_progress::~mbl_progress()
{
}


//=======================================================================
void mbl_progress::set_estimated_iterations(const vcl_string& identifier,
                                            const int iterations, 
                                            const vcl_string& display_text)
{
  identifier2estimatediterations_[identifier] = iterations;
  identifier2progress_[identifier] = 0;
  identifier2cancel_[identifier] = false;
  identifier2displaytext_[identifier] = display_text;
  on_set_estimated_iterations(identifier, iterations);
}


//=======================================================================
void mbl_progress::set_progress(const vcl_string& identifier,
                                const int progress)
{
  if (is_cancelled(identifier))
  {
    end_progress(identifier);
    if (throw_exception_on_cancel_)
    {	
      throw mbl_progress_cancel_exception();
    }
  }  
  
  vcl_map<vcl_string, int>::iterator it = identifier2progress_.find(identifier);
  if (it != identifier2progress_.end())
  {
    it->second = progress;
    on_set_progress(identifier, progress);  
  }
}


//=======================================================================
void mbl_progress::increment_progress(const vcl_string& identifier,
                                      const int n)
{
  vcl_map<vcl_string, int>::const_iterator it = 
    identifier2progress_.find(identifier);
  if (it != identifier2progress_.end())
  {
    int progress = it->second + n;
    set_progress(identifier, progress);
  }
}


//=======================================================================
void mbl_progress::end_progress(const vcl_string& identifier)
{
  on_end_progress(identifier);
}


//=======================================================================
int mbl_progress::progress(const vcl_string& identifier) const
{
  int p = -1;
  vcl_map<vcl_string, int>::const_iterator it = 
    identifier2progress_.find(identifier);
  if (it != identifier2progress_.end())
  {
    p = it->second;
  }
  return p;
}


//=======================================================================
vcl_string mbl_progress::display_text(const vcl_string& identifier) const
{
  vcl_string dt;  
  vcl_map<vcl_string, vcl_string>::const_iterator it = 
    identifier2displaytext_.find(identifier);
  if (it != identifier2displaytext_.end())
  {
    dt = it->second;
  }  
  return dt;
}


//=======================================================================
int mbl_progress::estimated_iterations(const vcl_string& identifier) const
{
  int n = -1;
  vcl_map<vcl_string, int>::const_iterator it = 
    identifier2estimatediterations_.find(identifier);
  if (it != identifier2estimatediterations_.end())
  {
    n = it->second;
  }
  return n;
}


//=======================================================================
// Modify the flag to cancel the current process.
// identifier: Progress object to cancel.
//=======================================================================
void mbl_progress::set_cancelled(const vcl_string& identifier, 
                                 const bool cancel)
{
  vcl_map<vcl_string, bool>::iterator it = identifier2cancel_.find(identifier);
  if (it != identifier2cancel_.end())
  {
    it->second = cancel;
  }
}


//=======================================================================
// Check whether a cancel request has been registered.
// identifier:  Progress object to check for a cancel request.
// Returns True if a cancel request has been received for this progress object.
//=======================================================================
bool mbl_progress::is_cancelled(const vcl_string &identifier) const
{
  bool retval = false;
  vcl_map<vcl_string, bool>::const_iterator it = identifier2cancel_.find(identifier);
  if (it != identifier2cancel_.end())
  {
    retval = it->second;
  }
  return retval;
}


//=======================================================================
short mbl_progress::version_no() const
{
  return 1;
}


//=======================================================================
vcl_string mbl_progress::is_a() const
{
  return vcl_string("mbl_progress");
}


//=======================================================================
