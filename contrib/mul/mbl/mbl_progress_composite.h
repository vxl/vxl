#ifndef mbl_progress_composite_h_
#define mbl_progress_composite_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif


//:
// \file
// \brief  A composite  of progress objects.
// \author Graham Vincent
// \date  25 Feb 2005


#include <vcl_string.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
#include <mbl/mbl_progress.h>


//=======================================================================
//:  A composite  of progress objects.
//
// Individual progress objects are attached to an instance of
// mbl_progress_composite which is passed to an algorithm.
// The mbl_progress_composite will inform all attached progress objects
// if and when it is informed of any progress.
class mbl_progress_composite : public mbl_progress
{
 public:

  //: Constructor
  mbl_progress_composite();

  //: Destructor
  virtual ~mbl_progress_composite();

  //: Adds a progress object to the composite.
  //  These are assumed to exist for the lifetime of this object.
  void add_progress(mbl_progress *progress);

  //: Version number for I/O
  short version_no() const;

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Print class to os
  virtual void print_summary(vcl_ostream& ) const {};

 protected:

  //: Called when set_estimate_iterations() is called for a given identifier.
  //  \param identifier The operation being monitored.
  virtual void on_set_estimated_iterations(const vcl_string &identifier,
                                           const int total_iterations);

  //: Called when set_progress() is called for a given identifier.
  //  \param identifier The operation being monitored.
  //  \param progress The new progress status.
  virtual void on_set_progress(const vcl_string &identifier,
                               const int progress);

  //: Called when end_progress() is called for a given identifier.
  //  \param identifier The operation being monitored.
  virtual void on_end_progress(const vcl_string &identifier);

 private:

  //: Stores estimated iterations for each identifier
  vcl_vector<mbl_progress*> progress_objects_;
};

//=======================================================================

#endif
