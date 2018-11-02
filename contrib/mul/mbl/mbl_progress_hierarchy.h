#ifndef mbl_progress_hierarchy_h_
#define mbl_progress_hierarchy_h_
//:
// \file
// \brief Assumes a hierarchy in the identifiers and reports a single float score.
// \author Ian Scott
// \date 10 Jan 2008

#include <iostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_progress.h>

//========================================================================
//: API for reporting a single progress fraction from multiple hierarchical sources.
//
// This progress class assumes that each identifier is arranged in a hierarchy,
// and that calls to set_estimated_iterations and end_progress are correctly nested.
//
// In order to use this abstract you need to derive this class
// and provide a definition of on_changed_progress().
class mbl_progress_hierarchy: public mbl_progress
{
 protected:

  //: Called when set_estimate_iterations() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored.
  void on_set_estimated_iterations(const std::string& identifier,
                                           const int total_iterations) override;

  //: Called when set_progress() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored.
  //  \param progress The new progress status.
  void on_set_progress(const std::string& identifier,
                               const int progress) override;

  //: Called when end_progress() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored.
  void on_end_progress(const std::string& identifier) override;


  //: Called when the progress has started, changed, or finished.
  // \param fraction Progress of all identifiers towards the end.
  // value is out of 1.0.
  virtual void on_changed_progress(double fraction)=0;

  mbl_progress_hierarchy() : just_ended_(false) {}

 private:
  //: The ordered hierarchy of identifiers.
  std::vector<std::string> identifier_stack_;

  //: Has an existing identifier just been ended.
  bool just_ended_;
};

//========================================================================

#endif // mbl_progress_hierarchy_h_
