#ifndef mbl_progress_hierarchy_h_
#define mbl_progress_hierarchy_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
// \brief Assumes a hierarchy in the identifiers and reports a single float score.
// \author Ian Scott
// \date 10 Jan 2008

#include <vcl_string.h>
#include <mbl/mbl_progress.h>


//========================================================================
//: API for reporting progress from multiple hierarchical sources.
//
// This progress class assumes that each identifier is arranged in a hierarchy,
// and that calls to set_estimated_iterations and end_progress are correctly nested.
//
// In order to use this abstract you need to derive this class
// and provide a defintion of on_changed_progress().
class mbl_progress_hierarchy
{
 public:

  //: Estimated number of iterations for the given identifier.
  void set_estimated_iterations(const vcl_string& identifier,
                                const int iterations,
                                const vcl_string& display_text);


  //: Sets progress for the given identifier.
  // Checks whether cancel has been set; if so calls end_progress(),
  // and throws an exception if "throw_exception_on_cancel" is true.
  void set_progress(const vcl_string& identifier,
                    const int progress);

  //: Increments progress for the given identifier by n.
  void increment_progress(const vcl_string& identifier,
                          const int n=1);

  //: Explicitly marks the end of loop for the given identifier.
  void end_progress(const vcl_string& identifier);

  //: Gets progress for given identifier.
  //  \param identifier to query.
  //  \return progress (-i if identifier is not known by this object).
  int progress(const vcl_string& identifier) const;

  //: Gets display text for given identifier.
  //  \param identifier to query.
  vcl_string display_text(const vcl_string& identifier) const;

  //: Gets estimated total iterations for given identifier.
  //  \param identifier to query.
  //  \return progress (-1 if identifier is not known by this object).
  int estimated_iterations(const vcl_string& identifier) const;

  //: Modify the flag to cancel the current process.
  //  \param identifier Progress object to cancel.
  void set_cancelled(const vcl_string& identifier,
                     const bool cancel);

  //: Check whether progress object is marked as cancelled.
  //  \param identifier Progress object to check.
  //  \return True if a cancel flag is set for this progress object.
  bool is_cancelled(const vcl_string& identifier) const;

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& ) const {};

 protected:

  //: Called when set_estimate_iterations() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored.
  virtual void on_set_estimated_iterations(const vcl_string& identifier,
                                           const int total_iterations);

  //: Called when set_progress() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored.
  //  \param progress The new progress status.
  virtual void on_set_progress(const vcl_string& identifier,
                               const int progress);

  //: Called when end_progress() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored.
  virtual void on_end_progress(const vcl_string& identifier);

  //: Called when the progress has started, changed, or finished.
  // \param fraction Progress of all identifiers towards the end.
  // value is out of 1.0.
  virtual void on_changed_progress(double fraction)=0;
 private:

  //: Stores display text for each identifier
  vcl_map<vcl_string, vcl_string> identifier2displaytext_;

  //: Stores estimated iterations for each identifier
  vcl_map<vcl_string, int> identifier2estimatediterations_;

  //: Stores current progress for each identifier
  vcl_map<vcl_string, int> identifier2progress_;

  //: Flags to indicate whether a request to cancel has been registered for each identifier
  vcl_map<vcl_string, bool> identifier2cancel_;

  //: If true set_progress() will throw an exception if cancel has been set.
  bool throw_exception_on_cancel_;
};

//========================================================================

#endif // mbl_progress_h_
