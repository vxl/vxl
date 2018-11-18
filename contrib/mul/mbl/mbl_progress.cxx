#include "mbl_progress.h"
// This is mul/mbl/mbl_progress.cxx

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//=======================================================================
void mbl_progress::set_estimated_iterations(const std::string& identifier,
                                            const int iterations,
                                            const std::string& display_text)
{
  identifier2estimatediterations_[identifier] = iterations;
  identifier2progress_[identifier] = 0;
  identifier2cancel_[identifier] = false;
  identifier2displaytext_[identifier] = display_text;
  on_set_estimated_iterations(identifier, iterations);
}


//=======================================================================
void mbl_progress::set_progress(const std::string& identifier,
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

  auto it = identifier2progress_.find(identifier);
  if (it != identifier2progress_.end())
  {
    it->second = progress;
    on_set_progress(identifier, progress);
  }
}


//=======================================================================
void mbl_progress::increment_progress(const std::string& identifier,
                                      const int n)
{
  std::map<std::string, int>::const_iterator it =
    identifier2progress_.find(identifier);
  if (it != identifier2progress_.end())
  {
    int progress = it->second + n;
    set_progress(identifier, progress);
  }
}


//=======================================================================
void mbl_progress::end_progress(const std::string& identifier)
{
  on_end_progress(identifier);
}


//=======================================================================
int mbl_progress::progress(const std::string& identifier) const
{
  int p = -1;
  auto it =
    identifier2progress_.find(identifier);
  if (it != identifier2progress_.end())
  {
    p = it->second;
  }
  return p;
}


//=======================================================================
std::string mbl_progress::display_text(const std::string& identifier) const
{
  std::string dt;
  auto it =
    identifier2displaytext_.find(identifier);
  if (it != identifier2displaytext_.end())
  {
    dt = it->second;
  }
  return dt;
}


//=======================================================================
int mbl_progress::estimated_iterations(const std::string& identifier) const
{
  int n = -1;
  auto it =
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
void mbl_progress::set_cancelled(const std::string& identifier,
                                 const bool cancel)
{
  auto it = identifier2cancel_.find(identifier);
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
bool mbl_progress::is_cancelled(const std::string &identifier) const
{
  bool retval = false;
  auto it = identifier2cancel_.find(identifier);
  if (it != identifier2cancel_.end())
  {
    retval = it->second;
  }
  return retval;
}
