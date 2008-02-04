
//:
// \file
// \brief Assumes a hierarchy in the identifiers and reports a single float score.
// \author Ian Scott
// \date 10 Jan 2008

#include "mbl_progress_hierarchy.h"
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <mbl/mbl_log.h>


inline mbl_logger& logger()
{
  static mbl_logger my_logger("mul.mbl.progress.hierarchy");
  return my_logger;
}


//: Called when set_estimate_iterations() is called for a given identifier.
//  Derived classes may take some action here.
//  \param identifier The operation being monitored.
void mbl_progress_hierarchy::on_set_estimated_iterations(const vcl_string& identifier,
  const int total_iterations)
{
  if (vcl_find(identifier_stack_.begin(), identifier_stack_.end(), identifier) !=
    identifier_stack_.end())
  {
    MBL_LOG(WARN, logger(), "Trying to reset estimated_iterations "
      "for existing identifier: \"" << identifier << '"');
      return;
  }
  if (just_ended_)
    MBL_LOG(WARN, logger(), "No change in parent progress before starting new "
      "identifier: \"" << identifier << '"');

  just_ended_ = false;
  identifier_stack_.push_back(identifier);
}

//: Called when set_progress() is called for a given identifier.
//  Derived classes may take some action here.
//  \param identifier The operation being monitored.
//  \param progress The new progress status.
void mbl_progress_hierarchy::on_set_progress(const vcl_string& identifier,
                               const int progress)
{
  just_ended_=false;

  if (identifier != identifier_stack_.back())
    MBL_LOG(WARN, logger(), "set progress for identifier \"" << identifier << 
      "\" rather than most recently created identifier \"" << identifier_stack_.back() << '"');

  double a=0.0, b=1.0; // The lower and upper bound on the current value
  for (vcl_vector<vcl_string>::const_iterator it=identifier_stack_.begin(),
    end=identifier_stack_.end(); it!=end; ++it)
  {
    int n_its = this->estimated_iterations(*it)+1;
    int its = this->progress(*it);
    double width = (b-a) / n_its;
    if (its < n_its)
    {
      a=a+width*its;
      b=a+width;
    }
    else
      a = b - width*vcl_exp(1.0 - its/(n_its-0.5));
  }
  on_changed_progress(a);
}
  
//: Called when end_progress() is called for a given identifier.
//  Derived classes may take some action here.
//  \param identifier The operation being monitored.
void mbl_progress_hierarchy::on_end_progress(const vcl_string& identifier)
{
  just_ended_ = true;


  double a=0.0, b=1.0; // The lower and upper bound on the current value
  for (vcl_vector<vcl_string>::const_iterator it=identifier_stack_.begin(),
    end=identifier_stack_.end(); it!=end; ++it)
  {
    int n_its = estimated_iterations(*it)+1;
    int its = progress(*it);
    double width = (b-a) / n_its;
    if (its < n_its)
    {
      a=a+width*its;
      b=a+width;
    }
    else
      a = b - width*vcl_exp(1.0 - its/(n_its-0.5));

  }
  on_changed_progress(0.1*a+0.9*b);

  if (identifier_stack_.back() != identifier)
    MBL_LOG(WARN, logger(), "Trying to end_progress on identifier: \"" << identifier <<
    "\" when most recently created identifier is \"" << identifier_stack_.back() << '"');

  identifier_stack_.pop_back();

}
