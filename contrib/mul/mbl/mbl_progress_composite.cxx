#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "mbl_progress_composite.h"

//=======================================================================
mbl_progress_composite::mbl_progress_composite() = default;


//=======================================================================
mbl_progress_composite::~mbl_progress_composite() = default;


//=======================================================================
void mbl_progress_composite::add_progress(mbl_progress *progress_object)
{
  progress_objects_.push_back(progress_object);
}


//=======================================================================
short mbl_progress_composite::version_no() const
{
  return 1;
}


//=======================================================================
std::string mbl_progress_composite::is_a() const
{
  return std::string("mbl_progress_composite");
}


// =================================================
void mbl_progress_composite::on_set_estimated_iterations(const std::string &identifier,int total_iterations)
{
  for (unsigned int i=0;i<progress_objects_.size();++i)
  {
    progress_objects_[i]->set_estimated_iterations(identifier,total_iterations,display_text(identifier));
  }
}


// =================================================
void mbl_progress_composite::on_set_progress(const std::string &identifier, int progress)
{
  for (unsigned int i=0;i<progress_objects_.size();++i)
  {
    progress_objects_[i]->set_progress(identifier,progress);
  }
}


// =================================================
void mbl_progress_composite::on_end_progress(const std::string &identifier)
{
  for (unsigned int i=0;i<progress_objects_.size();++i)
  {
    progress_objects_[i]->end_progress(identifier);
  }
}
